variable "prefix" {
  description = "Prefix for all Azure resources"
  type        = string
  default     = "mra"
}

variable "resource_group_name" {
  description = "Name of the Azure resource group"
  type        = string
  default     = "meta-repo-architecture-rg"
}

variable "location" {
  description = "Azure region for deployment"
  type        = string
  default     = "East US"
}

variable "environment" {
  description = "Environment name (dev, staging, prod)"
  type        = string
  default     = "dev"
}

variable "aks_node_count" {
  description = "Number of nodes in AKS default node pool"
  type        = number
  default     = 3
}

variable "aks_vm_size" {
  description = "VM size for AKS nodes"
  type        = string
  default     = "Standard_D4s_v3"
}

variable "db_username" {
  description = "PostgreSQL administrator username"
  type        = string
  default     = "postgresadmin"
}

variable "db_password" {
  description = "PostgreSQL administrator password"
  type        = string
  sensitive   = true
  default     = "ChangeMe123!"
}

variable "redis_password" {
  description = "Redis password"
  type        = string
  sensitive   = true
  default     = "ChangeMe456!"
}

variable "jwt_secret" {
  description = "JWT signing secret"
  type        = string
  sensitive   = true
  default     = "ChangeMe789!"
}

variable "domain_name" {
  description = "Custom domain name for the application"
  type        = string
  default     = ""
}

variable "enable_monitoring" {
  description = "Enable Azure Monitor and Application Insights"
  type        = bool
  default     = true
}

variable "enable_backup" {
  description = "Enable backup for databases and storage"
  type        = bool
  default     = true
}

variable "tags" {
  description = "Common tags for all resources"
  type        = map(string)
  default = {
    Project     = "meta-repo-architecture"
    ManagedBy   = "terraform"
    Owner       = "aviation-team"
    CostCenter  = "engineering"
  }
}

# Technology-specific variables
variable "vortex_shield_config" {
  description = "Configuration for Vortex Shield technology"
  type        = map(string)
  default = {
    cpu_limit    = "500m"
    memory_limit = "512Mi"
    replicas     = 2
  }
}

variable "air_swarm_config" {
  description = "Configuration for Air Swarm OS technology"
  type        = map(string)
  default = {
    cpu_limit    = "1000m"
    memory_limit = "1Gi"
    replicas     = 3
  }
}

variable "star_nav_config" {
  description = "Configuration for StarNav Core technology"
  type        = map(string)
  default = {
    cpu_limit    = "750m"
    memory_limit = "768Mi"
    replicas     = 2
  }
}

variable "quantum_radar_config" {
  description = "Configuration for Quantum Radar technology"
  type        = map(string)
  default = {
    cpu_limit    = "2000m"
    memory_limit = "2Gi"
    replicas     = 2
  }
}

variable "neural_flight_config" {
  description = "Configuration for Neural Flight Control technology"
  type        = map(string)
  default = {
    cpu_limit    = "1500m"
    memory_limit = "1.5Gi"
    replicas     = 3
  }
}

variable "fusion_prop_config" {
  description = "Configuration for Fusion Propulsion technology"
  type        = map(string)
  default = {
    cpu_limit    = "1000m"
    memory_limit = "1Gi"
    replicas     = 2
  }
}

variable "hypersonic_config" {
  description = "Configuration for Hypersonic Engine technology"
  type        = map(string)
  default = {
    cpu_limit    = "3000m"
    memory_limit = "3Gi"
    replicas     = 2
  }
}

variable "plasma_shield_config" {
  description = "Configuration for Plasma Shield technology"
  type        = map(string)
  default = {
    cpu_limit    = "2500m"
    memory_limit = "2.5Gi"
    replicas     = 2
  }
}

variable "anti_gravity_config" {
  description = "Configuration for Anti-Gravity Drive technology"
  type        = map(string)
  default = {
    cpu_limit    = "4000m"
    memory_limit = "4Gi"
    replicas     = 1
  }
}

variable "warp_drive_config" {
  description = "Configuration for Warp Drive technology"
  type        = map(string)
  default = {
    cpu_limit    = "5000m"
    memory_limit = "5Gi"
    replicas     = 1
  }
}

# Security variables
variable "enable_private_endpoints" {
  description = "Enable private endpoints for Azure services"
  type        = bool
  default     = true
}

variable "enable_network_security_groups" {
  description = "Enable network security groups"
  type        = bool
  default     = true
}

variable "enable_ddos_protection" {
  description = "Enable Azure DDoS protection"
  type        = bool
  default     = false
}

# Performance variables
variable "enable_autoscaling" {
  description = "Enable autoscaling for AKS"
  type        = bool
  default     = true
}

variable "enable_cluster_autoscaler" {
  description = "Enable cluster autoscaler"
  type        = bool
  default     = true
}

# Cost optimization variables
variable "enable_cost_management" {
  description = "Enable Azure Cost Management"
  type        = bool
  default     = true
}

variable "budget_alert_threshold" {
  description = "Budget alert threshold in USD"
  type        = number
  default     = 1000
}

# Compliance variables
variable "enable_policy_compliance" {
  description = "Enable Azure Policy compliance"
  type        = bool
  default     = true
}

variable "enable_security_center" {
  description = "Enable Azure Security Center"
  type        = bool
  default     = true
}
