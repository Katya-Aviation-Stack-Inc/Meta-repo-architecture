output "resource_group_name" {
  description = "Name of the resource group"
  value       = azurerm_resource_group.main.name
}

output "location" {
  description = "Azure region where resources are deployed"
  value       = azurerm_resource_group.main.location
}

output "virtual_network_id" {
  description = "ID of the virtual network"
  value       = azurerm_virtual_network.main.id
}

output "virtual_network_name" {
  description = "Name of the virtual network"
  value       = azurerm_virtual_network.main.name
}

output "aks_cluster_name" {
  description = "Name of the AKS cluster"
  value       = azurerm_kubernetes_cluster.main.name
}

output "aks_cluster_id" {
  description = "ID of the AKS cluster"
  value       = azurerm_kubernetes_cluster.main.id
}

output "aks_kube_config" {
  description = "Kubeconfig command to connect to AKS"
  value       = "az aks get-credentials --resource-group ${azurerm_resource_group.main.name} --name ${azurerm_kubernetes_cluster.main.name}"
}

output "application_gateway_public_ip" {
  description = "Public IP address of the Application Gateway"
  value       = azurerm_public_ip.app_gateway.ip_address
}

output "application_gateway_dns_name" {
  description = "DNS name of the Application Gateway"
  value       = azurerm_application_gateway.main.name
}

output "container_registry_name" {
  description = "Name of the Azure Container Registry"
  value       = azurerm_container_registry.main.name
}

output "container_registry_login_server" {
  description = "Login server URL of the Azure Container Registry"
  value       = azurerm_container_registry.main.login_server
}

output "container_registry_username" {
  description = "Username for the Azure Container Registry"
  value       = azurerm_container_registry.main.admin_username
  sensitive   = true
}

output "container_registry_password" {
  description = "Password for the Azure Container Registry"
  value       = azurerm_container_registry.main.admin_password
  sensitive   = true
}

output "postgresql_server_name" {
  description = "Name of the PostgreSQL server"
  value       = azurerm_postgresql_flexible_server.main.name
}

output "postgresql_server_fqdn" {
  description = "Fully qualified domain name of the PostgreSQL server"
  value       = azurerm_postgresql_flexible_server.main.fqdn
}

output "postgresql_server_id" {
  description = "ID of the PostgreSQL server"
  value       = azurerm_postgresql_flexible_server.main.id
}

output "redis_cache_name" {
  description = "Name of the Redis cache"
  value       = azurerm_redis_cache.main.name
}

output "redis_cache_hostname" {
  description = "Hostname of the Redis cache"
  value       = azurerm_redis_cache.main.hostname
}

output "redis_cache_primary_key" {
  description = "Primary key of the Redis cache"
  value       = azurerm_redis_cache.main.primary_access_key
  sensitive   = true
}

output "storage_account_name" {
  description = "Name of the storage account"
  value       = azurerm_storage_account.main.name
}

output "storage_account_primary_connection_string" {
  description = "Primary connection string of the storage account"
  value       = azurerm_storage_account.main.primary_connection_string
  sensitive   = true
}

output "storage_container_name" {
  description = "Name of the storage container"
  value       = azurerm_storage_container.main.name
}

output "key_vault_name" {
  description = "Name of the Key Vault"
  value       = azurerm_key_vault.main.name
}

output "key_vault_id" {
  description = "ID of the Key Vault"
  value       = azurerm_key_vault.main.id
}

output "key_vault_uri" {
  description = "URI of the Key Vault"
  value       = azurerm_key_vault.main.vault_uri
}

output "servicebus_namespace_name" {
  description = "Name of the Service Bus namespace"
  value       = azurerm_servicebus_namespace.main.name
}

output "servicebus_namespace_id" {
  description = "ID of the Service Bus namespace"
  value       = azurerm_servicebus_namespace.main.id
}

output "servicebus_primary_connection_string" {
  description = "Primary connection string of the Service Bus namespace"
  value       = azurerm_servicebus_namespace.main.default_primary_connection_string
  sensitive   = true
}

output "log_analytics_workspace_id" {
  description = "ID of the Log Analytics workspace"
  value       = azurerm_log_analytics_workspace.main.workspace_id
}

output "log_analytics_workspace_name" {
  description = "Name of the Log Analytics workspace"
  value       = azurerm_log_analytics_workspace.main.name
}

output "application_insights_name" {
  description = "Name of the Application Insights"
  value       = azurerm_application_insights.main.name
}

output "application_insights_app_id" {
  description = "App ID of the Application Insights"
  value       = azurerm_application_insights.main.app_id
}

output "application_insights_instrumentation_key" {
  description = "Instrumentation key of the Application Insights"
  value       = azurerm_application_insights.main.instrumentation_key
  sensitive   = true
}

# Useful commands
output "docker_login_command" {
  description = "Docker login command for Azure Container Registry"
  value       = "docker login ${azurerm_container_registry.main.login_server} -u ${azurerm_container_registry.main.admin_username} -p <password>"
}

output "kubectl_get_nodes" {
  description = "kubectl command to get AKS nodes"
  value       = "kubectl get nodes"
}

output "kubectl_get_services" {
  description = "kubectl command to get services"
  value       = "kubectl get services --all-namespaces"
}

output "kubectl_get_pods" {
  description = "kubectl command to get pods"
  value       = "kubectl get pods --all-namespaces"
}

output "az_monitor_logs_command" {
  description = "Azure CLI command to query monitor logs"
  value       = "az monitor log-analytics query --workspace ${azurerm_log_analytics_workspace.main.workspace_id} --analytics-query 'AzureActivity | limit 10'"
}

output "az_acr_build_command" {
  description = "Azure CLI command to build container image"
  value       = "az acr build --registry ${azurerm_container_registry.main.name} --image meta-repo-architecture:latest ."
}

# Service endpoints
output "main_application_url" {
  description = "URL for the main application"
  value       = "http://${azurerm_public_ip.app_gateway.ip_address}"
}

output "monitoring_dashboard_url" {
  description = "URL for the monitoring dashboard"
  value       = "https://portal.azure.com/#blade/HubsExtension/BrowseResource/resourceType/Microsoft.Insights%2Fcomponents"
}

output "kubernetes_dashboard_url" {
  description = "URL for the Kubernetes dashboard"
  value       = "https://portal.azure.com/#blade/HubsExtension/BrowseResource/resourceType/Microsoft.ContainerService%2FmanagedClusters"
}

# Cost and billing information
output "estimated_monthly_cost" {
  description = "Estimated monthly cost in USD"
  value       = "Please check Azure Cost Management for actual costs"
}

output "cost_management_url" {
  description = "URL for Azure Cost Management"
  value       = "https://portal.azure.com/#blade/HubsExtension/BrowseResource/resourceType/Microsoft.Consumption%2FsubscriptionResourceGroups"
}

# Security and compliance
output "security_center_url" {
  description = "URL for Azure Security Center"
  value       = "https://portal.azure.com/#blade/HubsExtension/BrowseResource/resourceType/Microsoft.Security%2FsecuritySolutions"
}

output "policy_compliance_url" {
  description = "URL for Azure Policy compliance"
  value       = "https://portal.azure.com/#blade/HubsExtension/BrowseResource/resourceType/Microsoft.Authorization%2FpolicySetDefinitions"
}

# Backup and disaster recovery
output "backup_center_url" {
  description = "URL for Azure Backup Center"
  value       = "https://portal.azure.com/#blade/HubsExtension/BrowseResource/resourceType/Microsoft.RecoveryServices%2Fvaults"
}

# Network information
output "network_watcher_url" {
  description = "URL for Azure Network Watcher"
  value       = "https://portal.azure.com/#blade/HubsExtension/BrowseResource/resourceType/Microsoft.Network%2FnetworkWatchers"
}

output "load_balancer_url" {
  description = "URL for Application Gateway management"
  value       = "https://portal.azure.com/#blade/HubsExtension/BrowseResource/resourceType/Microsoft.Network%2FapplicationGateways"
}

# Database management
output "postgresql_management_url" {
  description = "URL for PostgreSQL management"
  value       = "https://portal.azure.com/#blade/HubsExtension/BrowseResource/resourceType/Microsoft.DBforPostgreSQL%2FflexibleServers"
}

output "redis_management_url" {
  description = "URL for Redis management"
  value       = "https://portal.azure.com/#blade/HubsExtension/BrowseResource/resourceType/Microsoft.Cache%2Fredis"
}

# Storage management
output "storage_management_url" {
  description = "URL for Storage Account management"
  value       = "https://portal.azure.com/#blade/HubsExtension/BrowseResource/resourceType/Microsoft.Storage%2FstorageAccounts"
}

# Messaging and communication
output "servicebus_management_url" {
  description = "URL for Service Bus management"
  value       = "https://portal.azure.com/#blade/HubsExtension/BrowseResource/resourceType/Microsoft.ServiceBus%2Fnamespaces"
}

# Development and deployment
output "dev_portal_url" {
  description = "URL for Azure Dev Portal"
  value       = "https://portal.azure.com/#blade/HubsExtension/BrowseResource/resourceType/Microsoft.DevPortal%2Fportals"
}

output "resource_graph_url" {
  description = "URL for Azure Resource Graph"
  value       = "https://portal.azure.com/#blade/HubsExtension/BrowseResource/resourceType/Microsoft.ResourceGraph%2FresourceGraphs"
}
