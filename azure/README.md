# Azure Infrastructure for Meta-repo-architecture

This directory contains Terraform configurations for deploying the Meta-repo-architecture project to Microsoft Azure.

## Overview

The Azure infrastructure provides a complete cloud-native deployment solution for the Meta-repo-architecture project, including:

- **Azure Kubernetes Service (AKS)** for container orchestration
- **Azure Database for PostgreSQL** for persistent data storage
- **Azure Cache for Redis** for caching and session management
- **Application Gateway** with WAF for load balancing and security
- **Azure Container Registry** for container image management
- **Azure Key Vault** for secure secrets management
- **Azure Service Bus** for messaging and event processing
- **Azure Storage** for file storage and backups
- **Azure Monitor** and **Application Insights** for observability
- **Log Analytics Workspace** for centralized logging

## Architecture

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Internet      │────│ Application     │────│  AKS Cluster    │
│                 │    │ Gateway (WAF)   │    │                 │
└─────────────────┘    └─────────────────┘    └─────────────────┘
                                │                       │
                                │                       │
                       ┌─────────────────┐    ┌─────────────────┐
                       │ Service Bus     │    │ Azure Container  │
                       │ (Messaging)     │    │ Registry         │
                       └─────────────────┘    └─────────────────┘
                                │                       │
                                │                       │
                       ┌─────────────────┐    ┌─────────────────┐
                       │ Azure Storage   │    │ Key Vault       │
                       │ (Files/Backups) │    │ (Secrets)       │
                       └─────────────────┘    └─────────────────┘
                                │                       │
                                │                       │
                       ┌─────────────────┐    ┌─────────────────┐
                       │ PostgreSQL      │    │ Redis Cache     │
                       │ (Database)      │    │ (Cache)         │
                       └─────────────────┘    └─────────────────┘
```

## Prerequisites

- Azure CLI installed and configured
- Terraform >= 1.0 installed
- Appropriate Azure permissions (Owner or Contributor)
- SSH key pair for AKS access

## Quick Start

1. **Clone and navigate to the Azure directory:**
   ```bash
   cd azure
   ```

2. **Initialize Terraform:**
   ```bash
   terraform init
   ```

3. **Review and customize variables:**
   ```bash
   cp terraform.tfvars.example terraform.tfvars
   # Edit terraform.tfvars with your specific values
   ```

4. **Plan the deployment:**
   ```bash
   terraform plan -var-file=terraform.tfvars
   ```

5. **Apply the configuration:**
   ```bash
   terraform apply -var-file=terraform.tfvars
   ```

6. **Configure kubectl:**
   ```bash
   az aks get-credentials --resource-group $(terraform output -raw resource_group_name) --name $(terraform output -raw aks_cluster_name)
   ```

## Configuration

### Required Variables

| Variable | Description | Default |
|----------|-------------|---------|
| `prefix` | Prefix for all Azure resources | `mra` |
| `resource_group_name` | Name of the resource group | `meta-repo-architecture-rg` |
| `location` | Azure region | `East US` |
| `environment` | Environment name | `dev` |

### Optional Variables

| Variable | Description | Default |
|----------|-------------|---------|
| `aks_node_count` | Number of AKS nodes | `3` |
| `aks_vm_size` | VM size for AKS nodes | `Standard_D4s_v3` |
| `enable_monitoring` | Enable Azure Monitor | `true` |
| `enable_backup` | Enable backup services | `true` |
| `domain_name` | Custom domain name | `""` |

### Technology-Specific Configuration

Each aviation technology can be configured with custom resource limits and replica counts:

```hcl
vortex_shield_config = {
  cpu_limit    = "500m"
  memory_limit = "512Mi"
  replicas     = 2
}
```

## File Structure

```
azure/
├── main.tf              # Main Terraform configuration
├── variables.tf          # Input variables
├── outputs.tf           # Output values
├── README.md            # This documentation
├── terraform.tfvars.example  # Example variables file
└── modules/             # Optional modules (if needed)
```

## Deployment Steps

### 1. Environment Setup

```bash
# Login to Azure
az login

# Set subscription (if multiple)
az account set --subscription "your-subscription-id"

# Register required providers
az provider register --namespace Microsoft.ContainerService
az provider register --namespace Microsoft.Compute
az provider register --namespace Microsoft.Network
az provider register --namespace Microsoft.Storage
az provider register --namespace Microsoft.DBforPostgreSQL
```

### 2. Infrastructure Deployment

```bash
# Initialize Terraform
terraform init

# Review the plan
terraform plan

# Deploy infrastructure
terraform apply
```

### 3. Application Deployment

```bash
# Get AKS credentials
az aks get-credentials --resource-group meta-repo-architecture-rg --name mra-aks

# Deploy Kubernetes manifests
kubectl apply -f ../k8s/

# Verify deployment
kubectl get pods --all-namespaces
kubectl get services --all-namespaces
```

## Access URLs

After deployment, access the services at:

- **Main Application**: `http://<application-gateway-public-ip>`
- **Kubernetes Dashboard**: Azure Portal → Kubernetes Service
- **Monitoring Dashboard**: Azure Portal → Application Insights
- **Container Registry**: Azure Portal → Container Registry

## Monitoring and Logging

### Azure Monitor

- **Metrics**: CPU, memory, network, disk usage
- **Logs**: Container logs, system logs, application logs
- **Alerts**: Configured for critical thresholds

### Application Insights

- **Application Performance Monitoring**
- **Dependency tracking**
- **Exception monitoring**
- **User telemetry**

### Log Analytics

```bash
# Query logs
az monitor log-analytics query \
  --workspace $(terraform output -raw log_analytics_workspace_id) \
  --analytics-query "ContainerLog | limit 100"
```

## Security

### Network Security

- **Application Gateway WAF**: OWASP 3.2 rules enabled
- **Network Security Groups**: Restrict traffic as needed
- **Private Endpoints**: For database and cache (optional)
- **VNet Integration**: All resources in virtual network

### Identity and Access

- **Managed Identities**: For Azure resource access
- **RBAC**: Role-based access control
- **Key Vault**: Secure secrets management
- **AAD Integration**: Azure Active Directory authentication

### Compliance

- **Azure Policy**: Policy as code compliance
- **Security Center**: Security posture management
- **Audit Logs**: Complete audit trail

## Backup and Disaster Recovery

### Database Backup

- **Point-in-time restore**: 7-day retention
- **Geo-redundant backup**: Optional for production
- **Automated backups**: Daily with configurable retention

### Storage Backup

- **Azure Backup**: Integrated backup solution
- **Snapshot backup**: Point-in-time snapshots
- **Cross-region replication**: Optional for disaster recovery

## Cost Management

### Cost Optimization

- **Right-sizing**: Appropriate VM sizes
- **Reserved Instances**: For predictable workloads
- **Auto-scaling**: Scale based on demand
- **Cost alerts**: Budget notifications

### Monitoring Costs

```bash
# View cost analysis
az consumption usage list --resource-group $(terraform output -raw resource_group_name)
```

## Troubleshooting

### Common Issues

1. **AKS Cluster Creation Failure**
   - Check quota limits
   - Verify VM availability in region
   - Review network configuration

2. **Database Connection Issues**
   - Check VNet peering
   - Verify firewall rules
   - Validate credentials

3. **Application Gateway Issues**
   - Check backend health probes
   - Verify WAF rules
   - Review listener configuration

### Debug Commands

```bash
# Check AKS cluster status
az aks show --resource-group $(terraform output -raw resource_group_name) --name $(terraform output -raw aks_cluster_name)

# Check pod logs
kubectl logs -n meta-repo-architecture <pod-name>

# Check service status
kubectl get svc --all-namespaces

# Check resource utilization
kubectl top nodes
kubectl top pods --all-namespaces
```

## Maintenance

### Regular Tasks

- **Monitor resource utilization**
- **Review security alerts**
- **Update container images**
- **Backup verification**
- **Cost review**

### Scaling Operations

```bash
# Scale AKS nodes
az aks scale --resource-group $(terraform output -raw resource_group_name) --name $(terraform output -raw aks_cluster_name) --node-count 5

# Enable cluster autoscaler
az aks update --resource-group $(terraform output -raw resource_group_name) --name $(terraform output -raw aks_cluster_name) --enable-cluster-autoscaler --min-count 1 --max-count 10
```

## Support

### Azure Support

- **Documentation**: [Azure Documentation](https://docs.microsoft.com/en-us/azure/)
- **Support Tickets**: Create through Azure Portal
- **Community**: [Microsoft Q&A](https://docs.microsoft.com/en-us/answers/)

### Project Support

- **Issues**: Create GitHub issues
- **Documentation**: Project README and docs
- **Contact**: aviation-team@example.com

## Cleanup

To remove all resources:

```bash
terraform destroy
```

**Warning**: This will delete all resources including data, databases, and storage.

## Best Practices

1. **Use separate environments** (dev, staging, prod)
2. **Implement proper tagging** for cost management
3. **Enable monitoring and alerts** for all services
4. **Regular backup verification**
5. **Security review and compliance checks**
6. **Performance monitoring and optimization**
7. **Documentation and knowledge sharing**

## Next Steps

1. Configure CI/CD pipelines for automated deployments
2. Set up monitoring dashboards and alerts
3. Implement disaster recovery procedures
4. Configure custom domains and SSL certificates
5. Set up automated testing and validation
6. Implement governance and compliance policies
