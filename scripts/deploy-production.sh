#!/bin/bash

# Meta-repo-architecture Production Deployment Script
# This script deploys the application to the production environment

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Utility functions
print_status() {
    echo -e "${BLUE}$1${NC}"
}

print_success() {
    echo -e "${GREEN}✓ $1${NC}"
}

print_error() {
    echo -e "${RED}✗ $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}⚠ $1${NC}"
}

# Configuration
PRODUCTION_DIR="/opt/meta-repo-architecture/production"
BACKUP_DIR="/opt/meta-repo-architecture/backups"
SERVICE_USER="production"
LOG_FILE="/var/log/meta-repo-architecture-production-deploy.log"
HEALTH_CHECK_TIMEOUT=300
ROLLBACK_ENABLED=true

# Technology services
TECHNOLOGIES=(
    "air-to-air-mesh"
    "neuro-fcc"
    "self-adaptive-rotor-blades"
    "cold-jet"
    "local-gravity-field-navigation"
    "predictive-airflow-engine"
    "self-healing-avionics-bios"
    "vortex-shield"
    "air-swarm-os"
    "star-nav-core"
)

# Logging function
log_message() {
    echo "$(date '+%Y-%m-%d %H:%M:%S') - $1" | tee -a "$LOG_FILE"
}

# Send notification
send_notification() {
    local message="$1"
    local severity="$2"
    
    # Send to Slack if webhook is configured
    if [ -n "$SLACK_WEBHOOK_URL" ]; then
        local color="good"
        case $severity in
            "error") color="danger" ;;
            "warning") color="warning" ;;
            "info") color="good" ;;
        esac
        
        curl -X POST -H 'Content-type: application/json' \
            --data "{\"text\":\"$message\", \"color\":\"$color\"}" \
            "$SLACK_WEBHOOK_URL" 2>/dev/null || true
    fi
    
    # Send email if configured
    if [ -n "$EMAIL_RECIPIENTS" ] && command -v mail >/dev/null 2>&1; then
        echo "$message" | mail -s "Meta-repo-architecture Production Deployment: $severity" "$EMAIL_RECIPIENTS" 2>/dev/null || true
    fi
}

# Check prerequisites
check_prerequisites() {
    print_status "Checking prerequisites..."
    
    # Check if running as root
    if [[ $EUID -ne 0 ]]; then
        print_error "This script must be run as root"
        exit 1
    fi
    
    # Check if staging deployment exists and is healthy
    if [ ! -d "/opt/meta-repo-architecture/staging" ]; then
        print_error "Staging deployment not found"
        exit 1
    fi
    
    # Check disk space
    local available_space=$(df "$PRODUCTION_DIR" | awk 'NR==2 {print $4}')
    local required_space=1048576  # 1GB in KB
    
    if [ "$available_space" -lt "$required_space" ]; then
        print_error "Insufficient disk space. Required: ${required_space}KB, Available: ${available_space}KB"
        exit 1
    fi
    
    # Check if all required services are running in staging
    local failed_services=()
    for tech in "${TECHNOLOGIES[@]}"; do
        if ! systemctl is-active --quiet "meta-repo-$tech-staging"; then
            failed_services+=("$tech")
        fi
    done
    
    if [ ${#failed_services[@]} -ne 0 ]; then
        print_error "Some staging services are not running: ${failed_services[*]}"
        exit 1
    fi
    
    print_success "Prerequisites check passed"
}

# Create backup
create_backup() {
    print_status "Creating backup of current production deployment..."
    
    BACKUP_NAME="production-backup-$(date +%Y%m%d-%H%M%S)"
    BACKUP_PATH="$BACKUP_DIR/$BACKUP_NAME"
    
    mkdir -p "$BACKUP_PATH"
    
    if [ -d "$PRODUCTION_DIR" ]; then
        # Backup application files
        cp -r "$PRODUCTION_DIR"/* "$BACKUP_PATH/" 2>/dev/null || true
        
        # Backup database
        if command -v pg_dump >/dev/null 2>&1; then
            sudo -u postgres pg_dump meta_repo_production > "$BACKUP_PATH/database.sql" 2>/dev/null || true
        fi
        
        # Backup configuration
        cp -r /etc/systemd/system/meta-repo-*.service "$BACKUP_PATH/" 2>/dev/null || true
        
        print_success "Backup created: $BACKUP_PATH"
        log_message "Backup created: $BACKUP_PATH"
    else
        print_warning "No existing production deployment to backup"
    fi
    
    # Keep only last 10 production backups
    find "$BACKUP_DIR" -type d -name "production-backup-*" | sort -r | tail -n +11 | xargs rm -rf
    
    # Create backup metadata
    cat > "$BACKUP_PATH/backup-info.json" << EOF
{
    "timestamp": "$(date -Iseconds)",
    "version": "$(git describe --tags --always 2>/dev/null || echo 'unknown')",
    "commit": "$(git rev-parse HEAD 2>/dev/null || echo 'unknown')",
    "branch": "$(git rev-parse --abbrev-ref HEAD 2>/dev/null || echo 'unknown')",
    "deployed_by": "$(whoami)",
    "environment": "production"
}
EOF
}

# Stop production services
stop_production_services() {
    print_status "Stopping production services..."
    
    # Stop in reverse dependency order
    local stop_order=(
        "air-swarm-os"
        "star-nav-core"
        "local-gravity-field-navigation"
        "predictive-airflow-engine"
        "cold-jet"
        "self-healing-avionics-bios"
        "vortex-shield"
        "self-adaptive-rotor-blades"
        "neuro-fcc"
        "air-to-air-mesh"
    )
    
    for tech in "${stop_order[@]}"; do
        service_name="meta-repo-$tech"
        if systemctl is-active --quiet "$service_name"; then
            print_status "Stopping $service_name..."
            systemctl stop "$service_name"
            
            # Wait for service to stop
            local count=0
            while systemctl is-active --quiet "$service_name" && [ $count -lt 30 ]; do
                sleep 1
                count=$((count + 1))
            done
            
            if ! systemctl is-active --quiet "$service_name"; then
                print_success "Stopped $service_name"
            else
                print_error "Failed to stop $service_name"
                return 1
            fi
        else
            print_warning "$service_name not running"
        fi
    done
    
    # Stop monitoring services
    if systemctl is-active --quiet "meta-repo-monitoring"; then
        systemctl stop "meta-repo-monitoring"
        print_success "Stopped monitoring service"
    fi
    
    # Stop load balancer
    if systemctl is-active --quiet "nginx"; then
        systemctl stop nginx
        print_success "Stopped nginx"
    fi
}

# Deploy application from staging
deploy_application() {
    print_status "Deploying application from staging to production..."
    
    # Ensure production directory exists
    mkdir -p "$PRODUCTION_DIR"
    
    # Copy application files from staging
    rsync -av --exclude='*.log' --exclude='*.pid' --exclude='tmp/' \
        /opt/meta-repo-architecture/staging/ "$PRODUCTION_DIR/"
    
    # Set permissions
    chown -R "$SERVICE_USER:$SERVICE_USER" "$PRODUCTION_DIR"
    chmod -R 755 "$PRODUCTION_DIR"
    
    # Make executables
    find "$PRODUCTION_DIR" -name "*_demo" -type f -exec chmod +x {} \;
    
    # Update production configuration
    update_production_configuration
    
    print_success "Application deployed to production"
    log_message "Application deployed to production"
}

# Update production configuration
update_production_configuration() {
    print_status "Updating production configuration..."
    
    # Update environment configuration
    cat > "$PRODUCTION_DIR/.env" << 'EOF'
# Meta-repo-architecture Production Environment
NODE_ENV=production
LOG_LEVEL=info
PORT=8080

# Database configuration
DATABASE_URL=postgresql://production_user:secure_prod_pass@localhost:5432/meta_repo_production

# Redis configuration
REDIS_URL=redis://localhost:6379/0

# Monitoring configuration
PROMETHEUS_PORT=9090
GRAFANA_PORT=3000

# Security configuration
JWT_SECRET=$(openssl rand -base64 32)
ENCRYPTION_KEY=$(openssl rand -base64 32)

# Technology-specific configuration
AIR_MESH_NETWORK_PORT=8081
NEURO_FCC_PORT=8082
ADAPTIVE_BLADES_PORT=8083
COLD_JET_PORT=8084
LGFN_PORT=8085
PREDICTIVE_CFD_PORT=8086
BIOS_PORT=8087
VORTEX_SHIELD_PORT=8088
AIR_SWARM_OS_PORT=8089
STARNAV_CORE_PORT=8090

# Production-specific settings
ENABLE_SSL=true
SSL_CERT_PATH=/etc/ssl/certs/meta-repo-architecture.crt
SSL_KEY_PATH=/etc/ssl/private/meta-repo-architecture.key
MAX_CONNECTIONS=1000
TIMEOUT=30
EOF
    
    # Update service configurations
    for tech in "${TECHNOLOGIES[@]}"; do
        config_file="$PRODUCTION_DIR/$tech/config/production.json"
        staging_config="$PRODUCTION_DIR/$tech/config/staging.json"
        
        if [ -f "$staging_config" ]; then
            cp "$staging_config" "$config_file"
            # Update production-specific settings
            sed -i 's/"environment": "staging"/"environment": "production"/g' "$config_file"
            sed -i 's/"debug": true/"debug": false/g' "$config_file"
            sed -i 's/"log_level": "debug"/"log_level": "info"/g' "$config_file"
            sed -i 's/"max_connections": 100/"max_connections": 1000/g' "$config_file"
        fi
    done
    
    print_success "Production configuration updated"
}

# Setup production database
setup_production_database() {
    print_status "Setting up production database..."
    
    # Create database if it doesn't exist
    sudo -u postgres psql -c "CREATE DATABASE meta_repo_production;" 2>/dev/null || true
    sudo -u postgres psql -c "CREATE USER production_user WITH PASSWORD 'secure_prod_pass';" 2>/dev/null || true
    sudo -u postgres psql -c "GRANT ALL PRIVILEGES ON DATABASE meta_repo_production TO production_user;" 2>/dev/null || true
    
    # Run database migrations
    if [ -f "$PRODUCTION_DIR/migrate.sh" ]; then
        chmod +x "$PRODUCTION_DIR/migrate.sh"
        cd "$PRODUCTION_DIR"
        ./migrate.sh production
        cd -
    fi
    
    # Optimize database
    sudo -u postgres psql -d meta_repo_production -c "VACUUM ANALYZE;" 2>/dev/null || true
    
    print_success "Production database setup completed"
}

# Setup SSL certificates
setup_ssl() {
    print_status "Setting up SSL certificates..."
    
    SSL_DIR="/etc/ssl/meta-repo-architecture"
    mkdir -p "$SSL_DIR"
    
    # Generate self-signed certificate if not exists
    if [ ! -f "$SSL_DIR/meta-repo-architecture.crt" ]; then
        openssl req -x509 -nodes -days 365 -newkey rsa:2048 \
            -keyout "$SSL_DIR/meta-repo-architecture.key" \
            -out "$SSL_DIR/meta-repo-architecture.crt" \
            -subj "/C=US/ST=CA/L=San Francisco/O=Meta-repo-architecture/CN=meta-repo-architecture.com" \
            2>/dev/null || true
        
        chmod 600 "$SSL_DIR/meta-repo-architecture.key"
        chmod 644 "$SSL_DIR/meta-repo-architecture.crt"
        
        print_success "SSL certificates generated"
    else
        print_warning "SSL certificates already exist"
    fi
}

# Setup load balancer
setup_load_balancer() {
    print_status "Setting up load balancer..."
    
    cat > /etc/nginx/sites-available/meta-repo-architecture << 'EOF'
upstream meta_repo_backend {
    server 127.0.0.1:8081;  # Air-to-Air Mesh
    server 127.0.0.1:8082;  # Neuro-FCC
    server 127.0.0.1:8083;  # Adaptive Blades
    server 127.0.0.1:8084;  # Cold-Jet
    server 127.0.0.1:8085;  # LGFN
    server 127.0.0.1:8086;  # Predictive CFD
    server 127.0.0.1:8087;  # BIOS
    server 127.0.0.1:8088;  # Vortex Shield
    server 127.0.0.1:8089;  # Air Swarm OS
    server 127.0.0.1:8090;  # StarNav Core
}

server {
    listen 80;
    server_name meta-repo-architecture.com www.meta-repo-architecture.com;
    return 301 https://$server_name$request_uri;
}

server {
    listen 443 ssl http2;
    server_name meta-repo-architecture.com www.meta-repo-architecture.com;
    
    ssl_certificate /etc/ssl/meta-repo-architecture/meta-repo-architecture.crt;
    ssl_certificate_key /etc/ssl/meta-repo-architecture/meta-repo-architecture.key;
    
    ssl_protocols TLSv1.2 TLSv1.3;
    ssl_ciphers ECDHE-RSA-AES256-GCM-SHA512:DHE-RSA-AES256-GCM-SHA512:ECDHE-RSA-AES256-GCM-SHA384:DHE-RSA-AES256-GCM-SHA384;
    ssl_prefer_server_ciphers off;
    
    # Security headers
    add_header X-Frame-Options DENY;
    add_header X-Content-Type-Options nosniff;
    add_header X-XSS-Protection "1; mode=block";
    add_header Strict-Transport-Security "max-age=63072000; includeSubDomains; preload";
    
    # Rate limiting
    limit_req_zone $binary_remote_addr zone=api:10m rate=10r/s;
    limit_req zone=api burst=20 nodelay;
    
    location / {
        proxy_pass http://meta_repo_backend;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
        
        # Timeouts
        proxy_connect_timeout 30s;
        proxy_send_timeout 30s;
        proxy_read_timeout 30s;
    }
    
    location /health {
        access_log off;
        proxy_pass http://meta_repo_backend/health;
    }
    
    location /metrics {
        allow 127.0.0.1;
        allow 10.0.0.0/8;
        deny all;
        proxy_pass http://meta_repo_backend/metrics;
    }
}
EOF
    
    # Enable site
    ln -sf /etc/nginx/sites-available/meta-repo-architecture /etc/nginx/sites-enabled/
    rm -f /etc/nginx/sites-enabled/default
    
    # Test nginx configuration
    nginx -t
    
    print_success "Load balancer setup completed"
}

# Start production services
start_production_services() {
    print_status "Starting production services..."
    
    # Start in dependency order
    local start_order=(
        "air-to-air-mesh"
        "neuro-fcc"
        "self-adaptive-rotor-blades"
        "vortex-shield"
        "cold-jet"
        "self-healing-avionics-bios"
        "local-gravity-field-navigation"
        "predictive-airflow-engine"
        "star-nav-core"
        "air-swarm-os"
    )
    
    for tech in "${start_order[@]}"; do
        service_name="meta-repo-$tech"
        service_file="/etc/systemd/system/$service_name.service"
        
        if [ -f "$service_file" ]; then
            print_status "Starting $service_name..."
            systemctl daemon-reload
            systemctl enable "$service_name"
            systemctl start "$service_name"
            
            # Wait for service to start
            sleep 3
            
            if systemctl is-active --quiet "$service_name"; then
                print_success "Started $service_name"
            else
                print_error "Failed to start $service_name"
                systemctl status "$service_name"
                return 1
            fi
        else
            print_warning "Service file not found: $service_name"
        fi
    done
    
    # Start monitoring services
    if [ -f "/etc/systemd/system/meta-repo-monitoring.service" ]; then
        systemctl daemon-reload
        systemctl enable "meta-repo-monitoring"
        systemctl start "meta-repo-monitoring"
        print_success "Started monitoring service"
    fi
    
    # Start load balancer
    systemctl start nginx
    print_success "Started nginx"
}

# Comprehensive health check
health_check() {
    print_status "Performing comprehensive health check..."
    
    local start_time=$(date +%s)
    local end_time=$((start_time + HEALTH_CHECK_TIMEOUT))
    local healthy_services=0
    local total_services=${#TECHNOLOGIES[@]}
    
    while [ $(date +%s) -lt $end_time ]; do
        healthy_services=0
        FAILED_SERVICES=()
        
        for tech in "${TECHNOLOGIES[@]}"; do
            port_var="${tech^^}_PORT"
            port=${!port_var}
            
            if [ -z "$port" ]; then
                # Default port mapping
                case $tech in
                    "air-to-air-mesh") port=8081 ;;
                    "neuro-fcc") port=8082 ;;
                    "self-adaptive-rotor-blades") port=8083 ;;
                    "cold-jet") port=8084 ;;
                    "local-gravity-field-navigation") port=8085 ;;
                    "predictive-airflow-engine") port=8086 ;;
                    "self-healing-avionics-bios") port=8087 ;;
                    "vortex-shield") port=8088 ;;
                    "air-swarm-os") port=8089 ;;
                    "star-nav-core") port=8090 ;;
                    *) port=8080 ;;
                esac
            fi
            
            # Check service health
            if curl -f -s --max-time 5 "http://localhost:$port/health" >/dev/null 2>&1; then
                healthy_services=$((healthy_services + 1))
            else
                FAILED_SERVICES+=("$tech")
            fi
        done
        
        # Check load balancer
        if curl -f -s --max-time 5 "https://localhost/health" >/dev/null 2>&1; then
            healthy_services=$((healthy_services + 1))
        else
            FAILED_SERVICES+=("load-balancer")
        fi
        
        if [ $healthy_services -eq $((total_services + 1)) ]; then
            print_success "All services are healthy"
            return 0
        fi
        
        sleep 10
    done
    
    print_error "Health check failed after $HEALTH_CHECK_TIMEOUT seconds"
    print_error "Failed services: ${FAILED_SERVICES[*]}"
    return 1
}

# Rollback function
rollback() {
    if [ "$ROLLBACK_ENABLED" != "true" ]; then
        print_error "Rollback is disabled"
        return 1
    fi
    
    print_status "Rolling back to previous production deployment..."
    
    LATEST_BACKUP=$(find "$BACKUP_DIR" -type d -name "production-backup-*" | sort -r | head -n 1)
    
    if [ -z "$LATEST_BACKUP" ]; then
        print_error "No backup found for rollback"
        return 1
    fi
    
    print_status "Rolling back to: $LATEST_BACKUP"
    
    # Stop services
    stop_production_services
    
    # Restore from backup
    rm -rf "$PRODUCTION_DIR"/*
    cp -r "$LATEST_BACKUP"/* "$PRODUCTION_DIR/"
    
    # Restore database if backup exists
    if [ -f "$LATEST_BACKUP/database.sql" ]; then
        sudo -u postgres psql -d meta_repo_production < "$LATEST_BACKUP/database.sql"
        print_success "Database restored"
    fi
    
    # Restore service files
    if [ -d "$LATEST_BACKUP" ] && [ "$(ls -A "$LATEST_BACKUP"/*.service 2>/dev/null)" ]; then
        cp "$LATEST_BACKUP"/*.service /etc/systemd/system/
        systemctl daemon-reload
        print_success "Service files restored"
    fi
    
    # Start services
    start_production_services
    
    # Health check
    if health_check; then
        print_success "Rollback completed successfully"
        log_message "Rollback completed successfully"
        send_notification "Production rollback completed successfully" "warning"
    else
        print_error "Rollback failed - services not healthy"
        log_message "Rollback failed"
        send_notification "Production rollback failed" "error"
        return 1
    fi
}

# Cleanup function
cleanup() {
    print_status "Cleaning up..."
    
    # Remove temporary files
    rm -rf /tmp/meta-repo-deploy-*
    
    # Clean old logs
    find /var/log -name "meta-repo-architecture-production-deploy.log.*" -mtime +30 -delete
    
    # Clean old backups (keep last 10)
    find "$BACKUP_DIR" -type d -name "production-backup-*" | sort -r | tail -n +11 | xargs rm -rf
    
    print_success "Cleanup completed"
}

# Main deployment function
main() {
    log_message "Starting production deployment"
    send_notification "Starting production deployment" "info"
    
    check_prerequisites
    create_backup
    stop_production_services
    deploy_application
    setup_production_database
    setup_ssl
    setup_load_balancer
    start_production_services
    
    # Health check
    if health_check; then
        print_success "Production deployment completed successfully!"
        log_message "Production deployment completed successfully"
        send_notification "Production deployment completed successfully" "info"
        cleanup
        exit 0
    else
        print_error "Production deployment failed - health check failed"
        log_message "Production deployment failed - health check failed"
        send_notification "Production deployment failed - health check failed" "error"
        
        if [ "$ROLLBACK_ENABLED" = "true" ]; then
            print_status "Attempting automatic rollback..."
            rollback
        else
            exit 1
        fi
    fi
}

# Handle script arguments
case "${1:-deploy}" in
    deploy)
        main
        ;;
    rollback)
        rollback
        ;;
    health)
        health_check
        ;;
    cleanup)
        cleanup
        ;;
    *)
        echo "Usage: $0 {deploy|rollback|health|cleanup}"
        exit 1
        ;;
esac
