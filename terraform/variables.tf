# Terraform variables for Meta-repo-architecture infrastructure

variable "aws_region" {
  description = "AWS region for infrastructure"
  type        = string
  default     = "us-east-1"
  
  validation {
    condition     = can(regex("^us-(east|west|central)-[1-3]$", var.aws_region))
    error_message = "AWS region must be a valid US region (us-east-1, us-west-1, etc.)."
  }
}

variable "environment" {
  description = "Environment name (dev, staging, production)"
  type        = string
  default     = "production"
  
  validation {
    condition     = contains(["dev", "staging", "production"], var.environment)
    error_message = "Environment must be one of: dev, staging, production"
  }
}

variable "kubernetes_version" {
  description = "Kubernetes version for EKS cluster"
  type        = string
  default     = "1.28"
  
  validation {
    condition     = can(regex("^1\\.[2-9][0-9]$", var.kubernetes_version))
    error_message = "Kubernetes version must be in format 1.XX (e.g., 1.28)"
  }
}

variable "domain_name" {
  description = "Domain name for the application"
  type        = string
  default     = "meta-repo-architecture.org"
  
  validation {
    condition     = can(regex("^[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$", var.domain_name))
    error_message = "Domain name must be a valid domain (e.g., example.com)"
  }
}

variable "create_dns_zone" {
  description = "Whether to create Route 53 hosted zone"
  type        = bool
  default     = true
}

variable "bastion_allowed_ips" {
  description = "List of CIDR blocks allowed to access bastion host"
  type        = list(string)
  default     = ["0.0.0.0/0"]
  
  validation {
    condition     = length(var.bastion_allowed_ips) > 0
    error_message = "At least one IP range must be specified for bastion access"
  }
}

variable "db_username" {
  description = "PostgreSQL database username"
  type        = string
  default     = "meta_repo_user"
  
  validation {
    condition     = can(regex("^[a-zA-Z][a-zA-Z0-9_]*$", var.db_username))
    error_message = "Database username must start with a letter and contain only letters, numbers, and underscores"
  }
}

variable "db_password" {
  description = "PostgreSQL database password"
  type        = string
  default     = ""
  sensitive   = true
  
  validation {
    condition     = length(var.db_password) >= 8
    error_message = "Database password must be at least 8 characters long"
  }
}

variable "db_instance_class" {
  description = "RDS instance class"
  type        = string
  default     = "db.t3.medium"
  
  validation {
    condition     = contains(["db.t3.micro", "db.t3.small", "db.t3.medium", "db.t3.large", "db.t3.xlarge", "db.t3.2xlarge"], var.db_instance_class)
    error_message = "DB instance class must be a valid t3 instance type"
  }
}

variable "db_allocated_storage" {
  description = "Allocated storage for RDS instance in GB"
  type        = number
  default     = 100
  
  validation {
    condition     = var.db_allocated_storage >= 20 && var.db_allocated_storage <= 65536
    error_message = "DB allocated storage must be between 20 and 65536 GB"
  }
}

variable "redis_node_type" {
  description = "ElastiCache Redis node type"
  type        = string
  default     = "cache.t3.micro"
  
  validation {
    condition     = contains(["cache.t3.micro", "cache.t3.small", "cache.t3.medium"], var.redis_node_type)
    error_message = "Redis node type must be a valid t3 cache instance"
  }
}

variable "enable_monitoring" {
  description = "Whether to enable CloudWatch monitoring and alarms"
  type        = bool
  default     = true
}

variable "enable_backup" {
  description = "Whether to enable automated backups"
  type        = bool
  default     = true
}

variable "enable_encryption" {
  description = "Whether to enable encryption for all resources"
  type        = bool
  default     = true
}

variable "tags" {
  description = "Additional tags for all resources"
  type        = map(string)
  default = {
    Project     = "meta-repo-architecture"
    ManagedBy   = "terraform"
    Environment = "production"
  }
}

# Technology-specific variables
variable "air_mesh_config" {
  description = "Configuration for Air-to-Air Mesh Network"
  type        = map(string)
  default = {
    network_bandwidth = "100"
    network_range     = "100"
    port              = "8081"
    replicas          = "3"
    cpu_limit         = "500"
    memory_limit      = "128"
  }
}

variable "neuro_fcc_config" {
  description = "Configuration for Neuro-FCC"
  type        = map(string)
  default = {
    latency_target    = "2"
    accuracy_target   = "99.9"
    port              = "8082"
    replicas          = "2"
    cpu_limit         = "1000"
    memory_limit      = "256"
    critical          = "true"
  }
}

variable "adaptive_blades_config" {
  description = "Configuration for Self-Adaptive Rotor Blades"
  type        = map(string)
  default = {
    servo_count       = "24"
    servo_rate        = "1000"
    port              = "8083"
    replicas          = "2"
    cpu_limit         = "800"
    memory_limit      = "192"
  }
}

variable "cold_jet_config" {
  description = "Configuration for Cold-Jet"
  type        = map(string)
  default = {
    thrust_increase   = "25"
    efficiency_improvement = "30"
    port              = "8084"
    replicas          = "2"
    cpu_limit         = "1200"
    memory_limit      = "256"
  }
}

variable "lgfn_config" {
  description = "Configuration for Local Gravity Field Navigation"
  type        = map(string)
  default = {
    accuracy          = "92"
    sensitivity        = "0.1"
    port              = "8085"
    replicas          = "2"
    cpu_limit         = "600"
    memory_limit      = "192"
  }
}

variable "predictive_cfd_config" {
  description = "Configuration for Predictive Airflow Engine"
  type        = map(string)
  default = {
    prediction_time    = "50"
    accuracy          = "95"
    port              = "8086"
    replicas          = "2"
    cpu_limit         = "2000"
    memory_limit      = "512"
    gpu_required      = "true"
  }
}

variable "bios_config" {
  description = "Configuration for Self-Healing Avionics BIOS"
  type        = map(string)
  default = {
    detection_time     = "1"
    recovery_time     = "5"
    uptime            = "99.99"
    port              = "8087"
    replicas          = "2"
    cpu_limit         = "400"
    memory_limit      = "128"
    critical          = "true"
  }
}

variable "vortex_shield_config" {
  description = "Configuration for Vortex Shield"
  type        = map(string)
  default = {
    detection_time     = "10"
    accuracy          = "99.5"
    recovery_success   = "98.7"
    port              = "8088"
    replicas          = "2"
    cpu_limit         = "800"
    memory_limit      = "192"
    critical          = "true"
  }
}

variable "air_swarm_config" {
  description = "Configuration for Air Swarm OS"
  type        = map(string)
  default = {
    consensus_time    = "100"
    transaction_throughput = "100"
    port              = "8089"
    replicas          = "3"
    cpu_limit         = "1200"
    memory_limit      = "384"
  }
}

variable "starnav_config" {
  description = "Configuration for StarNav Core"
  type        = map(string)
  default = {
    position_accuracy = "90"
    update_time       = "500"
    star_catalog_size = "10000"
    port              = "8090"
    replicas          = "2"
    cpu_limit         = "1000"
    memory_limit      = "256"
  }
}

# Performance tuning variables
variable "performance_tuning" {
  description = "Performance tuning parameters"
  type        = map(string)
  default = {
    enable_autoscaling = "true"
    scale_up_threshold = "70"
    scale_down_threshold = "30"
    max_replicas = "10"
    min_replicas = "2"
  }
}

# Security variables
variable "security_config" {
  description = "Security configuration"
  type        = map(string)
  default = {
    enable_network_policies = "true"
    enable_rbac = "true"
    enable_ssl = "true"
    enable_encryption = "true"
    enable_audit_logging = "true"
  }
}

# Monitoring variables
variable "monitoring_config" {
  description = "Monitoring configuration"
  type        = map(string)
  default = {
    enable_prometheus = "true"
    enable_grafana = "true"
    enable_cloudwatch = "true"
    enable_alerting = "true"
    log_retention_days = "30"
  }
}

# Backup variables
variable "backup_config" {
  description = "Backup configuration"
  type        = map(string)
  default = {
    enable_automated_backups = "true"
    backup_retention_days = "30"
    backup_window = "03:00-04:00"
    enable_cross_region_backup = "false"
  }
}

# Cost optimization variables
variable "cost_optimization" {
  description = "Cost optimization settings"
  type        = map(string)
  default = {
    enable_spot_instances = "false"
    enable_reserved_instances = "true"
    enable_autoscaling = "true"
    enable_resource_cleanup = "true"
  }
}

# Compliance variables
variable "compliance_config" {
  description = "Compliance configuration"
  type        = map(string)
  default = {
    enable_gdpr_compliance = "true"
    enable_hipaa_compliance = "true"
    enable_sox_compliance = "false"
    enable_pci_compliance = "false"
  }
}
