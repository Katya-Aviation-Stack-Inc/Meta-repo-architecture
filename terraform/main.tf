# Terraform configuration for Meta-repo-architecture cloud infrastructure
# This file defines the complete infrastructure for deploying all 10 aviation technologies

terraform {
  required_version = ">= 1.0"
  required_providers {
    aws = {
      source  = "hashicorp/aws"
      version = "~> 5.0"
    }
    kubernetes = {
      source  = "hashicorp/kubernetes"
      version = "~> 2.0"
    }
    helm = {
      source  = "hashicorp/helm"
      version = "~> 2.0"
    }
    random = {
      source  = "hashicorp/random"
      version = "~> 3.0"
    }
  }
}

# Configure AWS provider
provider "aws" {
  region = var.aws_region
  
  default_tags {
    tags = {
      Project     = "meta-repo-architecture"
      Environment = var.environment
      ManagedBy   = "terraform"
      Team        = "aviation-technologies"
    }
  }
}

# Configure Kubernetes provider
provider "kubernetes" {
  host                   = module.eks.cluster_endpoint
  cluster_ca_certificate = base64decode(module.eks.cluster_certificate_authority_data)
  
  exec {
    api_version = "client.authentication.k8s.io/v1beta1"
    command     = "aws"
    args        = ["eks", "get-token", "--cluster-name", module.eks.cluster_name]
  }
}

# Configure Helm provider
provider "helm" {
  kubernetes {
    host                   = module.eks.cluster_endpoint
    cluster_ca_certificate = base64decode(module.eks.cluster_certificate_authority_data)
    
    exec {
      api_version = "client.authentication.k8s.io/v1beta1"
      command     = "aws"
      args        = ["eks", "get-token", "--cluster-name", module.eks.cluster_name]
    }
  }
}

# Random string for unique resource naming
resource "random_string" "suffix" {
  length  = 8
  special = false
  upper   = false
}

# VPC for the infrastructure
resource "aws_vpc" "main" {
  cidr_block           = "10.0.0.0/16"
  enable_dns_hostnames = true
  enable_dns_support   = true
  
  tags = {
    Name = "meta-repo-vpc-${random_string.suffix.result}"
  }
}

# Internet gateway
resource "aws_internet_gateway" "main" {
  vpc_id = aws_vpc.main.id
  
  tags = {
    Name = "meta-repo-igw-${random_string.suffix.result}"
  }
}

# Public subnets
resource "aws_subnet" "public" {
  count = 3
  
  vpc_id                  = aws_vpc.main.id
  cidr_block              = "10.0.${count.index + 1}.0/24"
  availability_zone       = data.aws_availability_zones.available.names[count.index]
  map_public_ip_on_launch = true
  
  tags = {
    Name = "meta-repo-public-${count.index + 1}-${random_string.suffix.result}"
    Type = "Public"
  }
}

# Private subnets
resource "aws_subnet" "private" {
  count = 3
  
  vpc_id            = aws_vpc.main.id
  cidr_block        = "10.0.${count.index + 4}.0/24"
  availability_zone = data.aws_availability_zones.available.names[count.index]
  
  tags = {
    Name = "meta-repo-private-${count.index + 1}-${random_string.suffix.result}"
    Type = "Private"
  }
}

# Route tables
resource "aws_route_table" "public" {
  vpc_id = aws_vpc.main.id
  
  route {
    cidr_block = "0.0.0.0/0"
    gateway_id = aws_internet_gateway.main.id
  }
  
  tags = {
    Name = "meta-repo-public-rt-${random_string.suffix.result}"
  }
}

resource "aws_route_table" "private" {
  count = 3
  
  vpc_id = aws_vpc.main.id
  
  route {
    cidr_block     = "0.0.0.0/0"
    nat_gateway_id = aws_nat_gateway.main[count.index].id
  }
  
  tags = {
    Name = "meta-repo-private-rt-${count.index + 1}-${random_string.suffix.result}"
  }
}

# NAT gateways
resource "aws_eip" "nat" {
  count = 3
  vpc   = true
  
  tags = {
    Name = "meta-repo-nat-eip-${count.index + 1}-${random_string.suffix.result}"
  }
}

resource "aws_nat_gateway" "main" {
  count = 3
  
  allocation_id = aws_eip.nat[count.index].id
  subnet_id     = aws_subnet.public[count.index].id
  
  tags = {
    Name = "meta-repo-nat-${count.index + 1}-${random_string.suffix.result}"
  }
  
  depends_on = [aws_internet_gateway.main]
}

# Associate route tables with subnets
resource "aws_route_table_association" "public" {
  count = 3
  
  subnet_id      = aws_subnet.public[count.index].id
  route_table_id = aws_route_table.public.id
}

resource "aws_route_table_association" "private" {
  count = 3
  
  subnet_id      = aws_subnet.private[count.index].id
  route_table_id = aws_route_table.private[count.index].id
}

# EKS cluster
module "eks" {
  source          = "terraform-aws-modules/eks/aws"
  version         = "~> 19.0"
  cluster_name    = "meta-repo-cluster-${random_string.suffix.result}"
  cluster_version = var.kubernetes_version
  vpc_id          = aws_vpc.main.id
  subnet_ids      = aws_subnet.private[*].id
  
  cluster_endpoint_public_access = true
  
  cluster_addons = {
    coredns = {
      most_recent = true
    }
    kube-proxy = {
      most_recent = true
    }
    vpc-cni = {
      most_recent = true
    }
  }
  
  node_groups = {
    # General purpose nodes
    general = {
      desired_capacity = 3
      max_capacity     = 6
      min_capacity     = 2
      instance_types   = ["t3.large", "t3.xlarge"]
      
      k8s_labels = {
        Environment = var.environment
        NodePool     = "general"
      }
      
      additional_tags = {
        NodePool = "general"
      }
    }
    
    # GPU nodes for Predictive CFD
    gpu = {
      desired_capacity = 1
      max_capacity     = 2
      min_capacity     = 1
      instance_types   = ["g4dn.xlarge"]
      
      k8s_labels = {
        Environment = var.environment
        NodePool     = "gpu"
      }
      
      additional_tags = {
        NodePool = "gpu"
      }
    }
    
    # Critical systems nodes
    critical = {
      desired_capacity = 2
      max_capacity     = 4
      min_capacity     = 2
      instance_types   = ["m5.large", "m5.xlarge"]
      
      k8s_labels = {
        Environment = var.environment
        NodePool     = "critical"
      }
      
      additional_tags = {
        NodePool = "critical"
      }
    }
  }
  
  tags = {
    Project = "meta-repo-architecture"
  }
}

# EKS cluster security group
resource "aws_security_group" "cluster" {
  name        = "meta-repo-cluster-sg-${random_string.suffix.result}"
  description = "Security group for EKS cluster"
  vpc_id      = aws_vpc.main.id
  
  ingress {
    description = "Allow Kubernetes API server"
    from_port   = 443
    to_port     = 443
    protocol    = "tcp"
    cidr_blocks = ["0.0.0.0/0"]
  }
  
  ingress {
    description = "Allow SSH from bastion"
    from_port   = 22
    to_port     = 22
    protocol    = "tcp"
    security_groups = [aws_security_group.bastion.id]
  }
  
  egress {
    description = "Allow all outbound traffic"
    from_port   = 0
    to_port     = 0
    protocol    = "-1"
    cidr_blocks = ["0.0.0.0/0"]
  }
  
  tags = {
    Name = "meta-repo-cluster-sg-${random_string.suffix.result}"
  }
}

# Bastion host security group
resource "aws_security_group" "bastion" {
  name        = "meta-repo-bastion-sg-${random_string.suffix.result}"
  description = "Security group for bastion host"
  vpc_id      = aws_vpc.main.id
  
  ingress {
    description = "Allow SSH from specific IP"
    from_port   = 22
    to_port     = 22
    protocol    = "tcp"
    cidr_blocks = var.bastion_allowed_ips
  }
  
  egress {
    description = "Allow SSH to cluster"
    from_port   = 22
    to_port     = 22
    protocol    = "tcp"
    security_groups = [aws_security_group.cluster.id]
  }
  
  egress {
    description = "Allow HTTPS outbound"
    from_port   = 443
    to_port     = 443
    protocol    = "tcp"
    cidr_blocks = ["0.0.0.0/0"]
  }
  
  tags = {
    Name = "meta-repo-bastion-sg-${random_string.suffix.result}"
  }
}

# Application load balancer
resource "aws_lb" "main" {
  name               = "meta-repo-alb-${random_string.suffix.result}"
  internal           = false
  load_balancer_type = "application"
  security_groups    = [aws_security_group.alb.id]
  subnets           = aws_subnet.public[*].id
  
  enable_deletion_protection = false
  
  tags = {
    Name = "meta-repo-alb-${random_string.suffix.result}"
  }
}

# ALB security group
resource "aws_security_group" "alb" {
  name        = "meta-repo-alb-sg-${random_string.suffix.result}"
  description = "Security group for application load balancer"
  vpc_id      = aws_vpc.main.id
  
  ingress {
    description = "Allow HTTP"
    from_port   = 80
    to_port     = 80
    protocol    = "tcp"
    cidr_blocks = ["0.0.0.0/0"]
  }
  
  ingress {
    description = "Allow HTTPS"
    from_port   = 443
    to_port     = 443
    protocol    = "tcp"
    cidr_blocks = ["0.0.0.0/0"]
  }
  
  egress {
    description = "Allow all outbound"
    from_port   = 0
    to_port     = 0
    protocol    = "-1"
    cidr_blocks = ["0.0.0.0/0"]
  }
  
  tags = {
    Name = "meta-repo-alb-sg-${random_string.suffix.result}"
  }
}

# Target groups for each technology
resource "aws_lb_target_group" "air_mesh" {
  name     = "meta-repo-air-mesh-tg-${random_string.suffix.result}"
  port     = 8081
  protocol = "HTTP"
  vpc_id   = aws_vpc.main.id
  
  health_check {
    enabled             = true
    healthy_threshold   = 2
    interval            = 30
    matcher             = "200"
    path                = "/health"
    port                = "traffic-port"
    protocol            = "HTTP"
    timeout             = 5
    unhealthy_threshold = 2
  }
  
  tags = {
    Name = "meta-repo-air-mesh-tg-${random_string.suffix.result}"
  }
}

resource "aws_lb_target_group" "neuro_fcc" {
  name     = "meta-repo-neuro-fcc-tg-${random_string.suffix.result}"
  port     = 8082
  protocol = "HTTP"
  vpc_id   = aws_vpc.main.id
  
  health_check {
    enabled             = true
    healthy_threshold   = 2
    interval            = 15
    matcher             = "200"
    path                = "/health"
    port                = "traffic-port"
    protocol            = "HTTP"
    timeout             = 5
    unhealthy_threshold = 2
  }
  
  tags = {
    Name = "meta-repo-neuro-fcc-tg-${random_string.suffix.result}"
  }
}

resource "aws_lb_target_group" "adaptive_blades" {
  name     = "meta-repo-adaptive-blades-tg-${random_string.suffix.result}"
  port     = 8083
  protocol = "HTTP"
  vpc_id   = aws_vpc.main.id
  
  health_check {
    enabled             = true
    healthy_threshold   = 2
    interval            = 20
    matcher             = "200"
    path                = "/health"
    port                = "traffic-port"
    protocol            = "HTTP"
    timeout             = 5
    unhealthy_threshold = 2
  }
  
  tags = {
    Name = "meta-repo-adaptive-blades-tg-${random_string.suffix.result}"
  }
}

resource "aws_lb_target_group" "cold_jet" {
  name     = "meta-repo-cold-jet-tg-${random_string.suffix.result}"
  port     = 8084
  protocol = "HTTP"
  vpc_id   = aws_vpc.main.id
  
  health_check {
    enabled             = true
    healthy_threshold   = 2
    interval            = 30
    matcher             = "200"
    path                = "/health"
    port                = "traffic-port"
    protocol            = "HTTP"
    timeout             = 10
    unhealthy_threshold = 2
  }
  
  tags = {
    Name = "meta-repo-cold-jet-tg-${random_string.suffix.result}"
  }
}

resource "aws_lb_target_group" "lgfn" {
  name     = "meta-repo-lgfn-tg-${random_string.suffix.result}"
  port     = 8085
  protocol = "HTTP"
  vpc_id   = aws_vpc.main.id
  
  health_check {
    enabled             = true
    healthy_threshold   = 2
    interval            = 25
    matcher             = "200"
    path                = "/health"
    port                = "traffic-port"
    protocol            = "HTTP"
    timeout             = 5
    unhealthy_threshold = 2
  }
  
  tags = {
    Name = "meta-repo-lgfn-tg-${random_string.suffix.result}"
  }
}

resource "aws_lb_target_group" "predictive_cfd" {
  name     = "meta-repo-predictive-cfd-tg-${random_string.suffix.result}"
  port     = 8086
  protocol = "HTTP"
  vpc_id   = aws_vpc.main.id
  
  health_check {
    enabled             = true
    healthy_threshold   = 2
    interval            = 45
    matcher             = "200"
    path                = "/health"
    port                = "traffic-port"
    protocol            = "HTTP"
    timeout             = 15
    unhealthy_threshold = 2
  }
  
  tags = {
    Name = "meta-repo-predictive-cfd-tg-${random_string.suffix.result}"
  }
}

resource "aws_lb_target_group" "bios" {
  name     = "meta-repo-bios-tg-${random_string.suffix.result}"
  port     = 8087
  protocol = "HTTP"
  vpc_id   = aws_vpc.main.id
  
  health_check {
    enabled             = true
    healthy_threshold   = 2
    interval            = 10
    matcher             = "200"
    path                = "/health"
    port                = "traffic-port"
    protocol            = "HTTP"
    timeout             = 5
    unhealthy_threshold = 2
  }
  
  tags = {
    Name = "meta-repo-bios-tg-${random_string.suffix.result}"
  }
}

resource "aws_lb_target_group" "vortex_shield" {
  name     = "meta-repo-vortex-shield-tg-${random_string.suffix.result}"
  port     = 8088
  protocol = "HTTP"
  vpc_id   = aws_vpc.main.id
  
  health_check {
    enabled             = true
    healthy_threshold   = 2
    interval            = 15
    matcher             = "200"
    path                = "/health"
    port                = "traffic-port"
    protocol            = "HTTP"
    timeout             = 5
    unhealthy_threshold = 2
  }
  
  tags = {
    Name = "meta-repo-vortex-shield-tg-${random_string.suffix.result}"
  }
}

resource "aws_lb_target_group" "air_swarm" {
  name     = "meta-repo-air-swarm-tg-${random_string.suffix.result}"
  port     = 8089
  protocol = "HTTP"
  vpc_id   = aws_vpc.main.id
  
  health_check {
    enabled             = true
    healthy_threshold   = 2
    interval            = 30
    matcher             = "200"
    path                = "/health"
    port                = "traffic-port"
    protocol            = "HTTP"
    timeout             = 10
    unhealthy_threshold = 2
  }
  
  tags = {
    Name = "meta-repo-air-swarm-tg-${random_string.suffix.result}"
  }
}

resource "aws_lb_target_group" "starnav" {
  name     = "meta-repo-starnav-tg-${random_string.suffix.result}"
  port     = 8090
  protocol = "HTTP"
  vpc_id   = aws_vpc.main.id
  
  health_check {
    enabled             = true
    healthy_threshold   = 2
    interval            = 40
    matcher             = "200"
    path                = "/health"
    port                = "traffic-port"
    protocol            = "HTTP"
    timeout             = 15
    unhealthy_threshold = 2
  }
  
  tags = {
    Name = "meta-repo-starnav-tg-${random_string.suffix.result}"
  }
}

# ALB listener
resource "aws_lb_listener" "http" {
  load_balancer_arn = aws_lb.main.arn
  port              = 80
  protocol          = "HTTP"
  
  default_action {
    type = "redirect"
    
    redirect {
      port        = "443"
      protocol    = "HTTPS"
      status_code = "HTTP_301"
    }
  }
}

resource "aws_lb_listener" "https" {
  load_balancer_arn = aws_lb.main.arn
  port              = 443
  protocol          = "HTTPS"
  ssl_policy        = "ELBSecurityPolicy-TLS-1-2-2017-06"
  certificate_arn   = aws_acm_certificate.main.arn
  
  default_action {
    type             = "forward"
    target_group_arn = aws_lb_target_group.neuro_fcc.arn
  }
}

# SSL certificate
resource "aws_acm_certificate" "main" {
  domain_name       = var.domain_name
  validation_method = "DNS"
  
  subject_alternative_names = [
    "www.${var.domain_name}",
    "api.${var.domain_name}",
    "monitoring.${var.domain_name}",
    "docs.${var.domain_name}"
  ]
  
  tags = {
    Name = "meta-repo-cert-${random_string.suffix.result}"
  }
}

# Route 53 hosted zone (if using Route 53)
resource "aws_route53_zone" "main" {
  count = var.create_dns_zone ? 1 : 0
  
  name = var.domain_name
  
  tags = {
    Name = "meta-repo-zone-${random_string.suffix.result}"
  }
}

# DNS records
resource "aws_route53_record" "www" {
  count = var.create_dns_zone ? 1 : 0
  
  zone_id = aws_route53_zone.main[0].zone_id
  name    = "www"
  type    = "A"
  ttl     = 300
  records = [aws_lb.main.dns_name]
}

resource "aws_route53_record" "api" {
  count = var.create_dns_zone ? 1 : 0
  
  zone_id = aws_route53_zone.main[0].zone_id
  name    = "api"
  type    = "A"
  ttl     = 300
  records = [aws_lb.main.dns_name]
}

resource "aws_route53_record" "monitoring" {
  count = var.create_dns_zone ? 1 : 0
  
  zone_id = aws_route53_zone.main[0].zone_id
  name    = "monitoring"
  type    = "A"
  ttl     = 300
  records = [aws_lb.main.dns_name]
}

resource "aws_route53_record" "docs" {
  count = var.create_dns_zone ? 1 : 0
  
  zone_id = aws_route53_zone.main[0].zone_id
  name    = "docs"
  type    = "A"
  ttl     = 300
  records = [aws_lb.main.dns_name]
}

# S3 bucket for application data
resource "aws_s3_bucket" "app_data" {
  bucket = "meta-repo-app-data-${random_string.suffix.result}"
  
  tags = {
    Name = "meta-repo-app-data-${random_string.suffix.result}"
  }
}

# S3 bucket for logs
resource "aws_s3_bucket" "logs" {
  bucket = "meta-repo-logs-${random_string.suffix.result}"
  
  tags = {
    Name = "meta-repo-logs-${random_string.suffix.result}"
  }
}

# RDS instance for PostgreSQL
resource "aws_db_instance" "postgres" {
  identifier = "meta-repo-postgres-${random_string.suffix.result}"
  
  engine         = "postgres"
  engine_version = "14.9"
  instance_class = "db.t3.medium"
  
  allocated_storage     = 100
  max_allocated_storage = 1000
  storage_type          = "gp2"
  storage_encrypted     = true
  
  db_name  = "meta_repo"
  username = var.db_username
  password = var.db_password
  
  vpc_security_group_ids = [aws_security_group.rds.id]
  db_subnet_group_name   = aws_db_subnet_group.main.name
  
  backup_retention_period = 7
  backup_window          = "03:00-04:00"
  maintenance_window     = "sun:04:00-sun:05:00"
  
  skip_final_snapshot       = false
  final_snapshot_identifier = "meta-repo-postgres-final-${random_string.suffix.result}"
  
  tags = {
    Name = "meta-repo-postgres-${random_string.suffix.result}"
  }
}

# RDS subnet group
resource "aws_db_subnet_group" "main" {
  name       = "meta-repo-db-subnet-group-${random_string.suffix.result}"
  subnet_ids = aws_subnet.private[*].id
  
  tags = {
    Name = "meta-repo-db-subnet-group-${random_string.suffix.result}"
  }
}

# RDS security group
resource "aws_security_group" "rds" {
  name        = "meta-repo-rds-sg-${random_string.suffix.result}"
  description = "Security group for RDS instance"
  vpc_id      = aws_vpc.main.id
  
  ingress {
    description = "Allow PostgreSQL from private subnets"
    from_port   = 5432
    to_port     = 5432
    protocol    = "tcp"
    cidr_blocks = [aws_vpc.main.cidr_block]
  }
  
  tags = {
    Name = "meta-repo-rds-sg-${random_string.suffix.result}"
  }
}

# ElastiCache Redis
resource "aws_elasticache_subnet_group" "redis" {
  name       = "meta-repo-redis-subnet-group-${random_string.suffix.result}"
  subnet_ids = aws_subnet.private[*].id
  
  tags = {
    Name = "meta-repo-redis-subnet-group-${random_string.suffix.result}"
  }
}

resource "aws_elasticache_replication_group" "redis" {
  replication_group_id       = "meta-repo-redis-${random_string.suffix.result}"
  description               = "Redis cluster for Meta-repo-architecture"
  
  node_type                  = "cache.t3.micro"
  port                       = 6379
  parameter_group_name       = "default.redis6.x"
  num_cache_clusters         = 1
  automatic_failover_enabled = true
  multi_az_enabled           = true
  
  subnet_group_name = aws_elasticcache_subnet_group.redis.name
  security_group_ids = [aws_security_group.redis.id]
  
  tags = {
    Name = "meta-repo-redis-${random_string.suffix.result}"
  }
}

# Redis security group
resource "aws_security_group" "redis" {
  name        = "meta-repo-redis-sg-${random_string.suffix.result}"
  description = "Security group for Redis cluster"
  vpc_id      = aws_vpc.main.id
  
  ingress {
    description = "Allow Redis from private subnets"
    from_port   = 6379
    to_port     = 6379
    protocol    = "tcp"
    cidr_blocks = [aws_vpc.main.cidr_block]
  }
  
  tags = {
    Name = "meta-repo-redis-sg-${random_string.suffix.result}"
  }
}

# EFS file system
resource "aws_efs_file_system" "main" {
  creation_token = "meta-repo-efs-${random_string.suffix.result}"
  
  tags = {
    Name = "meta-repo-efs-${random_string.suffix.result}"
  }
}

# EFS mount targets
resource "aws_efs_mount_target" "main" {
  count = 3
  
  file_system_id = aws_efs_file_system.main.id
  subnet_id      = aws_subnet.private[count.index].id
  security_groups = [aws_security_group.efs.id]
}

# EFS security group
resource "aws_security_group" "efs" {
  name        = "meta-repo-efs-sg-${random_string.suffix.result}"
  description = "Security group for EFS"
  vpc_id      = aws_vpc.main.id
  
  ingress {
    description = "Allow NFS from private subnets"
    from_port   = 2049
    to_port     = 2049
    protocol    = "tcp"
    cidr_blocks = [aws_vpc.main.cidr_block]
  }
  
  tags = {
    Name = "meta-repo-efs-sg-${random_string.suffix.result}"
  }
}

# CloudWatch log groups
resource "aws_cloudwatch_log_group" "applications" {
  name = "/aws/eks/meta-repo-cluster-${random_string.suffix.result}/applications"
  
  tags = {
    Name = "meta-repo-applications-logs-${random_string.suffix.result}"
  }
}

resource "aws_cloudwatch_log_group" "audit" {
  name = "/aws/eks/meta-repo-cluster-${random_string.suffix.result}/audit"
  
  tags = {
    Name = "meta-repo-audit-logs-${random_string.suffix.result}"
  }
}

# CloudWatch alarms for critical systems
resource "aws_cloudwatch_metric_alarm" "neuro_fcc_high_latency" {
  alarm_name          = "meta-repo-neuro-fcc-high-latency-${random_string.suffix.result}"
  comparison_operator = "GreaterThanThreshold"
  evaluation_periods  = "2"
  metric_name         = "neuro_fcc_latency_p95"
  namespace           = "MetaRepoArchitecture"
  period              = "60"
  statistic           = "Average"
  threshold           = "5"
  alarm_description   = "This metric monitors neuro-fcc P95 latency"
  
  alarm_actions = [aws_sns_topic.alerts.arn]
  
  tags = {
    Name = "meta-repo-neuro-fcc-high-latency-${random_string.suffix.result}"
  }
}

resource "aws_cloudwatch_metric_alarm" "vortex_shield_detection_failure" {
  alarm_name          = "meta-repo-vortex-shield-detection-failure-${random_string.suffix.result}"
  comparison_operator = "LessThanThreshold"
  evaluation_periods  = "2"
  metric_name         = "vortex_shield_detection_rate"
  namespace           = "MetaRepoArchitecture"
  period              = "60"
  statistic           = "Average"
  threshold           = "95"
  alarm_description   = "This metric monitors vortex shield detection rate"
  
  alarm_actions = [aws_sns_topic.alerts.arn]
  
  tags = {
    Name = "meta-repo-vortex-shield-detection-failure-${random_string.suffix.result}"
  }
}

# SNS topic for alerts
resource "aws_sns_topic" "alerts" {
  name = "meta-repo-alerts-${random_string.suffix.result}"
  
  tags = {
    Name = "meta-repo-alerts-${random_string.suffix.result}"
  }
}

# Data sources
data "aws_availability_zones" "available" {
  state = "available"
}

# Outputs
output "cluster_name" {
  description = "EKS cluster name"
  value       = module.eks.cluster_name
}

output "cluster_endpoint" {
  description = "EKS cluster endpoint"
  value       = module.eks.cluster_endpoint
}

output "load_balancer_dns" {
  description = "Application load balancer DNS name"
  value       = aws_lb.main.dns_name
}

output "database_endpoint" {
  description = "RDS database endpoint"
  value       = aws_db_instance.postgres.endpoint
}

output "redis_endpoint" {
  description = "ElastiCache Redis endpoint"
  value       = aws_elasticache_replication_group.redis.primary_endpoint_address
}

output "efs_id" {
  description = "EFS file system ID"
  value       = aws_efs_file_system.main.id
}
