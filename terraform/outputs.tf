# Terraform outputs for Meta-repo-architecture infrastructure

output "cluster_name" {
  description = "EKS cluster name"
  value       = module.eks.cluster_name
}

output "cluster_endpoint" {
  description = "EKS cluster endpoint"
  value       = module.eks.cluster_endpoint
}

output "cluster_certificate_authority_data" {
  description = "EKS cluster certificate authority data"
  value       = module.eks.cluster_certificate_authority_data
  sensitive   = true
}

output "load_balancer_dns" {
  description = "Application load balancer DNS name"
  value       = aws_lb.main.dns_name
}

output "load_balancer_url" {
  description = "Application load balancer URL"
  value       = "https://${aws_lb.main.dns_name}"
}

output "database_endpoint" {
  description = "RDS PostgreSQL database endpoint"
  value       = aws_db_instance.postgres.endpoint
  sensitive   = true
}

output "database_port" {
  description = "RDS PostgreSQL database port"
  value       = aws_db_instance.postgres.port
}

output "redis_endpoint" {
  description = "ElastiCache Redis primary endpoint"
  value       = aws_elasticache_replication_group.redis.primary_endpoint_address
  sensitive   = true
}

output "redis_port" {
  description = "ElastiCache Redis port"
  value       = aws_elasticache_replication_group.redis.port
}

output "efs_file_system_id" {
  description = "EFS file system ID"
  value       = aws_efs_file_system.main.id
}

output "vpc_id" {
  description = "VPC ID"
  value       = aws_vpc.main.id
}

output "vpc_cidr_block" {
  description = "VPC CIDR block"
  value       = aws_vpc.main.cidr_block
}

output "public_subnet_ids" {
  description = "Public subnet IDs"
  value       = aws_subnet.public[*].id
}

output "private_subnet_ids" {
  description = "Private subnet IDs"
  value       = aws_subnet.private[*].id
}

output "security_group_cluster_id" {
  description = "EKS cluster security group ID"
  value       = aws_security_group.cluster.id
}

output "security_group_alb_id" {
  description = "Application load balancer security group ID"
  value       = aws_security_group.alb.id
}

output "target_group_arns" {
  description = "ALB target group ARNs for all technologies"
  value = {
    air_mesh      = aws_lb_target_group.air_mesh.arn
    neuro_fcc      = aws_lb_target_group.neuro_fcc.arn
    adaptive_blades = aws_lb_target_group.adaptive_blades.arn
    cold_jet       = aws_lb_target_group.cold_jet.arn
    lgfn           = aws_lb_target_group.lgfn.arn
    predictive_cfd = aws_lb_target_group.predictive_cfd.arn
    bios           = aws_lb_target_group.bios.arn
    vortex_shield  = aws_lb_target_group.vortex_shield.arn
    air_swarm      = aws_lb_target_group.air_swarm.arn
    starnav        = aws_lb_target_group.starnav.arn
  }
}

output "s3_bucket_app_data" {
  description = "S3 bucket for application data"
  value       = aws_s3_bucket.app_data.id
}

output "s3_bucket_logs" {
  description = "S3 bucket for logs"
  value       = aws_s3_bucket.logs.id
}

output "acm_certificate_arn" {
  description = "ACM certificate ARN"
  value       = aws_acm_certificate.main.arn
}

output "sns_topic_alerts_arn" {
  description = "SNS topic for alerts ARN"
  value       = aws_sns_topic.alerts.arn
}

output "cloudwatch_log_groups" {
  description = "CloudWatch log group names"
  value = {
    applications = aws_cloudwatch_log_group.applications.name
    audit        = aws_cloudwatch_log_group.audit.name
  }
}

output "cloudwatch_metric_alarms" {
  description = "CloudWatch metric alarm names"
  value = {
    neuro_fcc_high_latency        = aws_cloudwatch_metric_alarm.neuro_fcc_high_latency.alarm_name
    vortex_shield_detection_failure = aws_cloudwatch_metric_alarm.vortex_shield_detection_failure.alarm_name
  }
}

output "node_group_arns" {
  description = "EKS node group ARNs"
  value = {
    general  = module.eks.eks_managed_node_groups["general"].arn
    gpu      = module.eks.eks_managed_node_groups["gpu"].arn
    critical = module.eks.eks_managed_node_groups["critical"].arn
  }
}

output "oidc_provider_arn" {
  description = "OIDC provider ARN"
  value       = module.eks.oidc_provider_arn
}

output "kubeconfig_command" {
  description = "Command to configure kubectl"
  value       = "aws eks update-kubeconfig --region ${var.aws_region} --name ${module.eks.cluster_name}"
}

output "helm_values" {
  description = "Helm values for all technologies"
  value = {
    air_mesh      = var.air_mesh_config
    neuro_fcc      = var.neuro_fcc_config
    adaptive_blades = var.adaptive_blades_config
    cold_jet       = var.cold_jet_config
    lgfn           = var.lgfn_config
    predictive_cfd = var.predictive_cfd_config
    bios           = var.bios_config
    vortex_shield  = var.vortex_shield_config
    air_swarm      = var.air_swarm_config
    starnav        = var.starnav_config
  }
}

output "technology_endpoints" {
  description = "Technology service endpoints"
  value = {
    air_mesh      = "https://api.${var.domain_name}/air-mesh"
    neuro_fcc      = "https://api.${var.domain_name}/neuro-fcc"
    adaptive_blades = "https://api.${var.domain_name}/adaptive-blades"
    cold_jet       = "https://api.${var.domain_name}/cold-jet"
    lgfn           = "https://api.${var.domain_name}/lgfn"
    predictive_cfd = "https://api.${var.domain_name}/predictive-cfd"
    bios           = "https://api.${var.domain_name}/bios"
    vortex_shield  = "https://api.${var.domain_name}/vortex-shield"
    air_swarm      = "https://api.${var.domain_name}/air-swarm"
    starnav        = "https://api.${var.domain_name}/starnav"
  }
}

output "monitoring_endpoints" {
  description = "Monitoring service endpoints"
  value = {
    grafana = "https://monitoring.${var.domain_name}"
    prometheus = "Internal access only"
  }
}

output "documentation_endpoints" {
  description = "Documentation service endpoints"
  value = {
    api_docs = "https://docs.${var.domain_name}"
    technical_docs = "https://docs.${var.domain_name}/technical"
    architecture = "https://docs.${var.domain_name}/architecture"
  }
}

output "cost_estimates" {
  description = "Estimated monthly costs"
  value = {
    compute = "$${length(module.eks.eks_managed_node_groups) * 200}"  # Rough estimate
    storage = "$${aws_db_instance.postgres.storage_allocated * 0.23}"  # $0.23/GB-month
    network = "$50"  # Rough estimate
    monitoring = "$30"  # Rough estimate
    total = "$${length(module.eks.eks_managed_node_groups) * 200 + aws_db_instance.postgres.storage_allocated * 0.23 + 50 + 30}"
  }
}

output "deployment_commands" {
  description = "Useful commands for deployment"
  value = {
    apply = "terraform apply"
    destroy = "terraform destroy"
    plan = "terraform plan"
    validate = "terraform validate"
    fmt = "terraform fmt"
    import = "terraform import"
    taint = "terraform taint"
    untaint = "terraform untaint"
    refresh = "terraform refresh"
  }
}

output "troubleshooting_commands" {
  description = "Useful commands for troubleshooting"
  value = {
    check_pods = "kubectl get pods -n meta-repo-architecture"
    check_services = "kubectl get services -n meta-repo-architecture"
    check_hpa = "kubectl get hpa -n meta-repo-architecture"
    check_logs = "kubectl logs -n meta-repo-architecture <pod-name>"
    describe_pod = "kubectl describe pod <pod-name> -n meta-repo-architecture"
    exec_pod = "kubectl exec -it <pod-name> -n meta-repo-architecture -- bash"
    port_forward = "kubectl port-forward service/<service-name> 8080 -n meta-repo-architecture"
  get_events = "kubectl get events -n meta-repo-architecture"
  top_pods = "kubectl top pods -n meta-repo-architecture"
    top_nodes = "kubectl top nodes"
  }
}

output "maintenance_commands" {
  description = "Useful commands for maintenance"
  value = {
    scale_up = "kubectl scale deployment <deployment-name> --replicas=4 -n meta-repo-architecture"
    scale_down = "kubectl scale deployment <deployment-name> --replicas=2 -n meta-repo-architecture"
    restart_deployment = "kubectl rollout restart deployment <deployment-name> -n meta-repo-architecture"
    check_rollout = "kubectl rollout status deployment <deployment-name> -n meta-repo-architecture"
    rollback = "kubectl rollout undo deployment <deployment-name> -n meta-repo-architecture"
    force_update = "kubectl patch deployment <deployment-name> -p '{\"spec\":{\"template\":{\"spec\":{\"containers\":[{\"name\":\"<container-name>\",\"image\":\"<new-image>\"}]}}}}' -n meta-repo-architecture"
  }
}

output "security_commands" {
  description = "Useful commands for security"
  value = {
    check_network_policies = "kubectl get networkpolicy -n meta-repo-architecture"
    check_rbac = "kubectl get role,rolebinding -n meta-repo-architecture"
    check_secrets = "kubectl get secrets -n meta-repo-architecture"
    check_service_accounts = "kubectl get serviceaccounts -n meta-repo-architecture"
    check_pod_security = "kubectl auth can-i create pod --as=system:serviceaccount:<service-account> -n meta-repo-architecture"
  }
}

output "monitoring_commands" {
  description = "Useful commands for monitoring"
  value = {
    check_metrics = "kubectl top pods --containers -n meta-repo-architecture"
    check_resource_usage = "kubectl describe node <node-name>"
    check_cluster_health = "kubectl get componentstatuses"
    check_api_resources = "kubectl api-resources"
    check_cluster_info = "kubectl cluster-info"
    get_all_resources = "kubectl get all -n meta-repo-architecture"
  }
}

output "backup_commands" {
  description = "Useful commands for backup and restore"
  value = {
    backup_configmaps = "kubectl get configmaps -n meta-repo-architecture -o yaml > backup-configmaps.yaml"
    backup_secrets = "kubectl get secrets -n meta-repo-architecture -o yaml > backup-secrets.yaml"
    backup_deployments = "kubectl get deployments -n meta-repo-architecture -o yaml > backup-deployments.yaml"
    backup_services = "kubectl get services -n meta-repo-architecture -o yaml > backup-services.yaml"
    restore_configmaps = "kubectl apply -f backup-configmaps.yaml"
    restore_secrets = "kubectl apply -f backup-secrets.yaml"
    restore_deployments = "kubectl apply -f backup-deployments.yaml"
    restore_services = "kubectl apply -f backup-services.yaml"
  }
}

output "aws_cli_commands" {
  description = "Useful AWS CLI commands"
  value = {
    check_eks = "aws eks describe-cluster --name ${module.eks.cluster_name}"
    check_rds = "aws rds describe-db-instances --db-instance-identifier ${aws_db_instance.postgres.identifier}"
    check_elasticache = "aws elasticache describe-replication-groups --replication-group-id ${aws_elasticache_replication_group.redis.replication_group_id}"
    check_s3 = "aws s3 ls s3://meta-repo-app-data-${random_string.suffix.result}"
    check_cloudwatch = "aws cloudwatch describe-alarms --alarm-names meta-repo-neuro-fcc-high-latency-${random_string.suffix.result}"
    check_sns = "aws sns list-subscriptions --topic-arn ${aws_sns_topic.alerts.arn}"
  }
}

output "helm_commands" {
  description = "Useful Helm commands"
  value = {
    list_releases = "helm list -n meta-repo-architecture"
    install_chart = "helm install <release-name> <chart-path> -n meta-repo-architecture"
    upgrade_chart = "helm upgrade <release-name> <chart-path> -n meta-repo-architecture"
    uninstall_chart = "helm uninstall <release-name> -n meta-repo-architecture"
    rollback_chart = "helm rollback <release-name> <revision> -n meta-repo-architecture"
    chart_history = "helm history <release-name> -n meta-repo-architecture"
    get_values = "helm get values <release-name> -n meta-repo-architecture"
  test_chart = "helm test <release-name> -n meta-repo-architecture"
  lint_chart = "helm lint <chart-path>"
  package_chart = "helm package <chart-path>"
  dependency_update = "helm dependency update <chart-path>"
  template_chart = "helm template <release-name> <chart-path>"
  }
}

output "terraform_state" {
  description = "Terraform state file location"
  value       = "terraform.tfstate"
}

output "configuration_summary" {
  description = "Configuration summary"
  value = {
    region                = var.aws_region
    environment           = var.environment
    kubernetes_version    = var.kubernetes_version
    domain_name           = var.domain_name
    create_dns_zone      = var.create_dns_zone
    enable_monitoring     = var.enable_monitoring
    enable_backup         = var.enable_backup
    enable_encryption     = var.enable_encryption
    total_technologies   = 10
    critical_technologies = 3
    standard_technologies = 7
  }
}
