#!/bin/bash

# Meta-repo-architecture Staging Deployment Script
# This script deploys the application to the staging environment

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
STAGING_DIR="/opt/meta-repo-architecture/staging"
BACKUP_DIR="/opt/meta-repo-architecture/backups"
SERVICE_USER="staging"
LOG_FILE="/var/log/meta-repo-architecture-staging-deploy.log"

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

# Check if running as root
check_permissions() {
    if [[ $EUID -ne 0 ]]; then
        print_error "This script must be run as root"
        exit 1
    fi
}

# Create backup
create_backup() {
    print_status "Creating backup of current deployment..."
    
    BACKUP_NAME="staging-backup-$(date +%Y%m%d-%H%M%S)"
    BACKUP_PATH="$BACKUP_DIR/$BACKUP_NAME"
    
    mkdir -p "$BACKUP_PATH"
    
    if [ -d "$STAGING_DIR" ]; then
        cp -r "$STAGING_DIR"/* "$BACKUP_PATH/" 2>/dev/null || true
        print_success "Backup created: $BACKUP_PATH"
    else
        print_warning "No existing deployment to backup"
    fi
    
    # Keep only last 10 backups
    find "$BACKUP_DIR" -type d -name "staging-backup-*" | sort -r | tail -n +11 | xargs rm -rf
}

# Stop services
stop_services() {
    print_status "Stopping existing services..."
    
    for tech in "${TECHNOLOGIES[@]}"; do
        service_name="meta-repo-$tech"
        if systemctl is-active --quiet "$service_name"; then
            systemctl stop "$service_name"
            print_success "Stopped $service_name"
        else
            print_warning "$service_name not running"
        fi
    done
    
    # Stop monitoring services
    if systemctl is-active --quiet "meta-repo-monitoring"; then
        systemctl stop "meta-repo-monitoring"
        print_success "Stopped monitoring service"
    fi
}

# Deploy application
deploy_application() {
    print_status "Deploying new application version..."
    
    # Ensure staging directory exists
    mkdir -p "$STAGING_DIR"
    
    # Copy new files
    cp -r ./* "$STAGING_DIR/"
    
    # Set permissions
    chown -R "$SERVICE_USER:$SERVICE_USER" "$STAGING_DIR"
    chmod -R 755 "$STAGING_DIR"
    
    # Make executables
    find "$STAGING_DIR" -name "*_demo" -type f -exec chmod +x {} \;
    
    print_success "Application deployed to $STAGING_DIR"
}

# Update configuration
update_configuration() {
    print_status "Updating configuration files..."
    
    # Update environment configuration
    cat > "$STAGING_DIR/.env" << 'EOF'
# Meta-repo-architecture Staging Environment
NODE_ENV=staging
LOG_LEVEL=debug
PORT=8080

# Database configuration
DATABASE_URL=postgresql://staging_user:staging_pass@localhost:5432/meta_repo_staging

# Redis configuration
REDIS_URL=redis://localhost:6379/1

# Monitoring configuration
PROMETHEUS_PORT=9090
GRAFANA_PORT=3000

# Security configuration
JWT_SECRET=staging-jwt-secret-key
ENCRYPTION_KEY=staging-encryption-key

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
EOF
    
    # Update service configurations
    for tech in "${TECHNOLOGIES[@]}"; do
        config_file="$STAGING_DIR/$tech/config/staging.json"
        if [ -f "$config_file" ]; then
            # Update staging-specific settings
            sed -i 's/"environment": "development"/"environment": "staging"/g' "$config_file"
            sed -i 's/"debug": true/"debug": true/g' "$config_file"
            sed -i 's/"log_level": "info"/"log_level": "debug"/g' "$config_file"
        fi
    done
    
    print_success "Configuration updated"
}

# Install dependencies
install_dependencies() {
    print_status "Installing dependencies..."
    
    cd "$STAGING_DIR"
    
    # Install system dependencies
    apt-get update
    apt-get install -y \
        postgresql-client \
        redis-tools \
        nginx \
        supervisor \
        python3 \
        python3-pip
    
    # Install Python dependencies
    if [ -f "requirements.txt" ]; then
        pip3 install -r requirements.txt
    fi
    
    # Install Node.js dependencies
    if [ -f "package.json" ]; then
        npm install
    fi
    
    print_success "Dependencies installed"
}

# Setup database
setup_database() {
    print_status "Setting up database..."
    
    # Create database if it doesn't exist
    sudo -u postgres psql -c "CREATE DATABASE meta_repo_staging;" 2>/dev/null || true
    sudo -u postgres psql -c "CREATE USER staging_user WITH PASSWORD 'staging_pass';" 2>/dev/null || true
    sudo -u postgres psql -c "GRANT ALL PRIVILEGES ON DATABASE meta_repo_staging TO staging_user;" 2>/dev/null || true
    
    # Run migrations
    if [ -f "migrate.sh" ]; then
        chmod +x migrate.sh
        ./migrate.sh staging
    fi
    
    print_success "Database setup completed"
}

# Setup monitoring
setup_monitoring() {
    print_status "Setting up monitoring..."
    
    # Create monitoring configuration
    mkdir -p "$STAGING_DIR/monitoring"
    
    cat > "$STAGING_DIR/monitoring/prometheus-staging.yml" << 'EOF'
global:
  scrape_interval: 15s

scrape_configs:
  - job_name: 'meta-repo-staging'
    static_configs:
      - targets: ['localhost:8081', 'localhost:8082', 'localhost:8083', 'localhost:8084', 'localhost:8085', 'localhost:8086', 'localhost:8087', 'localhost:8088', 'localhost:8089', 'localhost:8090']
    metrics_path: '/metrics'
    scrape_interval: 5s
EOF
    
    # Update Grafana configuration
    if [ -d "$STAGING_DIR/monitoring/grafana" ]; then
        cp "$STAGING_DIR/monitoring/grafana/datasources/prometheus.yml" "$STAGING_DIR/monitoring/grafana/datasources/prometheus-staging.yml"
        sed -i 's/prometheus:9090/prometheus-staging:9090/g' "$STAGING_DIR/monitoring/grafana/datasources/prometheus-staging.yml"
    fi
    
    print_success "Monitoring setup completed"
}

# Start services
start_services() {
    print_status "Starting services..."
    
    # Start technology services
    for tech in "${TECHNOLOGIES[@]}"; do
        service_name="meta-repo-$tech"
        service_file="/etc/systemd/system/$service_name.service"
        
        if [ -f "$service_file" ]; then
            systemctl daemon-reload
            systemctl enable "$service_name"
            systemctl start "$service_name"
            
            # Wait for service to start
            sleep 2
            
            if systemctl is-active --quiet "$service_name"; then
                print_success "Started $service_name"
            else
                print_error "Failed to start $service_name"
                systemctl status "$service_name"
            fi
        else
            print_warning "Service file not found: $service_file"
        fi
    done
    
    # Start monitoring services
    if [ -f "/etc/systemd/system/meta-repo-monitoring.service" ]; then
        systemctl daemon-reload
        systemctl enable "meta-repo-monitoring"
        systemctl start "meta-repo-monitoring"
        print_success "Started monitoring service"
    fi
}

# Health check
health_check() {
    print_status "Performing health check..."
    
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
        
        # Check if service is responding
        if curl -f -s "http://localhost:$port/health" >/dev/null 2>&1; then
            print_success "$tech is healthy (port $port)"
        else
            print_error "$tech is not responding (port $port)"
            FAILED_SERVICES+=("$tech")
        fi
    done
    
    if [ ${#FAILED_SERVICES[@]} -eq 0 ]; then
        print_success "All services are healthy"
        return 0
    else
        print_error "Failed services: ${FAILED_SERVICES[*]}"
        return 1
    fi
}

# Rollback function
rollback() {
    print_status "Rolling back to previous deployment..."
    
    LATEST_BACKUP=$(find "$BACKUP_DIR" -type d -name "staging-backup-*" | sort -r | head -n 1)
    
    if [ -z "$LATEST_BACKUP" ]; then
        print_error "No backup found for rollback"
        exit 1
    fi
    
    print_status "Rolling back to: $LATEST_BACKUP"
    
    # Stop services
    stop_services
    
    # Restore from backup
    rm -rf "$STAGING_DIR"/*
    cp -r "$LATEST_BACKUP"/* "$STAGING_DIR/"
    
    # Start services
    start_services
    
    # Health check
    if health_check; then
        print_success "Rollback completed successfully"
    else
        print_error "Rollback failed - services not healthy"
        exit 1
    fi
}

# Cleanup function
cleanup() {
    print_status "Cleaning up..."
    
    # Remove temporary files
    rm -rf /tmp/meta-repo-deploy-*
    
    # Clean old logs
    find /var/log -name "meta-repo-architecture-staging-deploy.log.*" -mtime +30 -delete
    
    print_success "Cleanup completed"
}

# Main deployment function
main() {
    log_message "Starting staging deployment"
    
    check_permissions
    create_backup
    stop_services
    deploy_application
    update_configuration
    install_dependencies
    setup_database
    setup_monitoring
    start_services
    
    # Health check with retry
    RETRY_COUNT=0
    MAX_RETRIES=5
    RETRY_DELAY=10
    
    while [ $RETRY_COUNT -lt $MAX_RETRIES ]; do
        if health_check; then
            print_success "Staging deployment completed successfully!"
            log_message "Staging deployment completed successfully"
            cleanup
            exit 0
        else
            RETRY_COUNT=$((RETRY_COUNT + 1))
            if [ $RETRY_COUNT -lt $MAX_RETRIES ]; then
                print_warning "Health check failed, retrying in $RETRY_DELAY seconds... (attempt $RETRY_COUNT/$MAX_RETRIES)"
                sleep $RETRY_DELAY
            fi
        fi
    done
    
    # If we get here, health check failed after all retries
    print_error "Health check failed after $MAX_RETRIES attempts"
    log_message "Health check failed after $MAX_RETRIES attempts"
    
    # Ask user if they want to rollback
    read -p "Do you want to rollback to the previous version? (y/n): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        rollback
    else
        print_error "Deployment failed and rollback was not performed"
        exit 1
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
