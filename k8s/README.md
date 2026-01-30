# Kubernetes Deployment Guide

This directory contains Kubernetes manifests for deploying the Meta-repo-architecture project with all 10 revolutionary aviation technologies.

## 🚀 **Overview**

The Kubernetes deployment provides:
- **Containerized deployment** of all 10 aviation technologies
- **High availability** with multiple replicas and auto-scaling
- **Monitoring and observability** with Prometheus and Grafana
- **Security** with network policies and RBAC
- **Ingress** for external access
- **Persistent storage** for data persistence

## 📁 **File Structure**

```
k8s/
├── namespace.yaml           # Kubernetes namespace
├── configmap.yaml           # Application configuration
├── secrets.yaml            # Encrypted secrets and credentials
├── services.yaml           # Service definitions
├── deployments.yaml         # Pod deployments
├── ingress.yaml            # Ingress configuration
├── hpa.yaml                # Horizontal Pod Autoscalers
├── monitoring.yaml          # Prometheus and Grafana
├── network-policy.yaml      # Network security policies
├── rbac.yaml               # Role-based access control
└── README.md               # This file
```

## 🎯 **Technology Deployments**

### 🌐 **Air-to-Air Mesh Network**
- **Service**: `air-mesh-service` (Port: 8081)
- **Replicas**: 2-10 (auto-scaled)
- **Resources**: 64-128Mi memory, 250-500m CPU
- **Criticality**: Standard

### 🧠 **Neuro-FCC**
- **Service**: `neuro-fcc-service` (Port: 8082)
- **Replicas**: 2-6 (auto-scaled)
- **Resources**: 128-256Mi memory, 500-1000m CPU
- **Criticality**: **Critical** (flight control)

### 💠 **Self-Adaptive Rotor Blades**
- **Service**: `adaptive-blades-service` (Port: 8083)
- **Replicas**: 2-8 (auto-scaled)
- **Resources**: 96-192Mi memory, 400-800m CPU
- **Criticality**: Standard

### ⚛️ **Cold-Jet**
- **Service**: `cold-jet-service` (Port: 8084)
- **Replicas**: 2-6 (auto-scaled)
- **Resources**: 128-256Mi memory, 600-1200m CPU
- **Criticality**: Standard

### 🛰️ **Local Gravity Field Navigation**
- **Service**: `lgfn-service` (Port: 8085)
- **Replicas**: 2-6 (auto-scaled)
- **Resources**: 96-192Mi memory, 300-600m CPU
- **Criticality**: Standard

### 🎯 **Predictive Airflow Engine**
- **Service**: `predictive-cfd-service` (Port: 8086)
- **Replicas**: 2-4 (auto-scaled)
- **Resources**: 256-512Mi memory, 1000-2000m CPU + GPU
- **Criticality**: Standard

### 🧬 **Self-Healing Avionics BIOS**
- **Service**: `bios-service` (Port: 8087)
- **Replicas**: 2-4 (auto-scaled)
- **Resources**: 64-128Mi memory, 200-400m CPU
- **Criticality**: **Critical** (system health)

### 🌀 **Vortex Shield**
- **Service**: `vortex-shield-service` (Port: 8088)
- **Replicas**: 2-6 (auto-scaled)
- **Resources**: 96-192Mi memory, 400-800m CPU
- **Criticality**: **Critical** (safety system)

### ⚡ **Air Swarm OS**
- **Service**: `air-swarm-service` (Port: 8089)
- **Replicas**: 3-10 (auto-scaled)
- **Resources**: 192-384Mi memory, 600-1200m CPU
- **Criticality**: Standard

### 🌑 **StarNav Core**
- **Service**: `starnav-service` (Port: 8090)
- **Replicas**: 2-6 (auto-scaled)
- **Resources**: 128-256Mi memory, 500-1000m CPU
- **Criticality**: Standard

## 🔧 **Deployment Instructions**

### 📋 **Prerequisites**
- **Kubernetes cluster** v1.20+
- **kubectl** configured
- **Helm 3** (optional)
- **Ingress controller** (nginx recommended)
- **Storage class** for persistent volumes
- **GPU nodes** (for Predictive CFD)

### ⚡ **Quick Deploy**
```bash
# Create namespace
kubectl apply -f namespace.yaml

# Deploy configuration and secrets
kubectl apply -f configmap.yaml
kubectl apply -f secrets.yaml

# Deploy services and deployments
kubectl apply -f services.yaml
kubectl apply -f deployments.yaml

# Deploy ingress and auto-scaling
kubectl apply -f ingress.yaml
kubectl apply -f hpa.yaml

# Deploy monitoring
kubectl apply -f monitoring.yaml

# Deploy security policies
kubectl apply -f network-policy.yaml
kubectl apply -f rbac.yaml
```

### 🔄 **Complete Deployment**
```bash
# Deploy everything in order
kubectl apply -f namespace.yaml
kubectl apply -f configmap.yaml
kubectl apply -f secrets.yaml
kubectl apply -f services.yaml
kubectl apply -f deployments.yaml
kubectl apply -f ingress.yaml
kubectl apply -f hpa.yaml
kubectl apply -f monitoring.yaml
kubectl apply -f network-policy.yaml
kubectl apply -f rbac.yaml

# Wait for all pods to be ready
kubectl wait --for=condition=ready pod --all --timeout=300s -n meta-repo-architecture
```

## 🌐 **Access URLs**

### 🏠 **Main Application**
- **Main Site**: https://meta-repo-architecture.org
- **API Gateway**: https://api.meta-repo-architecture.org

### 🔧 **Technology Endpoints**
- **Air-to-Air Mesh**: https://api.meta-repo-architecture.org/air-mesh
- **Neuro-FCC**: https://api.meta-repo-architecture.org/neuro-fcc
- **Adaptive Blades**: https://api.meta-repo-architecture.org/adaptive-blades
- **Cold-Jet**: https://api.meta-repo-architecture.org/cold-jet
- **LGFN**: https://api.meta-repo-architecture.org/lgfn
- **Predictive CFD**: https://api.meta-repo-architecture.org/predictive-cfd
- **BIOS**: https://api.meta-repo-architecture.org/bios
- **Vortex Shield**: https://api.meta-repo-architecture.org/vortex-shield
- **Air Swarm**: https://api.meta-repo-architecture.org/air-swarm
- **StarNav**: https://api.meta-repo-architecture.org/starnav

### 📊 **Monitoring**
- **Grafana Dashboard**: https://monitoring.meta-repo-architecture.org
- **Prometheus**: Internal access only
- **Alerts**: Configured for critical systems

### 📚 **Documentation**
- **API Docs**: https://docs.meta-repo-architecture.org
- **Technical Docs**: https://docs.meta-repo-architecture.org/technical

## 🔍 **Monitoring and Observability**

### 📈 **Prometheus Metrics**
All technologies expose metrics at `/metrics` endpoint:
- **Response time** histograms
- **Throughput** counters
- **Error rates** gauges
- **Resource usage** gauges
- **Technology-specific** metrics

### 📊 **Grafana Dashboards**
Pre-configured dashboards include:
- **System Overview**: All technologies status
- **Performance Metrics**: Latency and throughput
- **Resource Usage**: CPU, memory, storage
- **Safety Systems**: Critical component monitoring
- **Fleet Management**: Air Swarm OS metrics

### 🚨 **Alerting**
Critical alerts configured for:
- **Technology downtime** (all technologies)
- **High latency** (Neuro-FCC, Vortex Shield)
- **High error rate** (>5% for any technology)
- **Resource exhaustion** (>90% CPU/memory)
- **Critical system failures** (BIOS, Neuro-FCC, Vortex Shield)

## 🛡️ **Security Configuration**

### 🔒 **Network Policies**
- **Default deny** all ingress/egress
- **Technology-specific** communication rules
- **Monitoring access** allowed
- **External access** through ingress only
- **Critical systems** have restricted access

### 👥 **RBAC Configuration**
- **Service accounts** for each technology
- **Role-based access** control
- **Critical systems** have enhanced permissions
- **Admin account** for emergency operations
- **Monitoring account** for observability

### 🔐 **Secrets Management**
- **Encrypted secrets** stored in Kubernetes
- **Database credentials** isolated
- **API keys** and **tokens** secured
- **SSL certificates** managed by cert-manager
- **Rotation policies** for sensitive data

## 📈 **Auto-Scaling Configuration**

### ⚖️ **Horizontal Pod Autoscalers**
Each technology has HPA configured with:
- **CPU thresholds**: 60-80% utilization
- **Memory thresholds**: 70-85% utilization
- **Min/Max replicas**: Based on criticality
- **Scale-up/down policies**: Configured for stability

### 🎯 **Critical Systems**
- **Neuro-FCC**: 2-6 replicas, fast scale-up
- **Vortex Shield**: 2-6 replicas, fast scale-up
- **BIOS**: 2-4 replicas, conservative scaling

### 🔄 **Standard Systems**
- **Air Mesh**: 2-10 replicas, moderate scaling
- **Air Swarm**: 3-10 replicas, moderate scaling
- **Others**: 2-8 replicas, moderate scaling

## 🗄️ **Storage Configuration**

### 💾 **Persistent Volumes**
- **Prometheus**: 50Gi for metrics storage
- **Grafana**: 10Gi for dashboards and configuration
- **Application data**: Configured per technology needs

### 🏷️ **Storage Classes**
- **fast-ssd**: For databases and metrics
- **standard**: For application data
- **backup**: For backup and archive

## 🚀 **Troubleshooting**

### 🔍 **Common Issues**

#### **Pods Not Starting**
```bash
# Check pod status
kubectl get pods -n meta-repo-architecture

# Check pod logs
kubectl logs <pod-name> -n meta-repo-architecture

# Describe pod for detailed information
kubectl describe pod <pod-name> -n meta-repo-architecture
```

#### **Service Not Accessible**
```bash
# Check service endpoints
kubectl get endpoints -n meta-repo-architecture

# Check service configuration
kubectl describe service <service-name> -n meta-repo-architecture

# Test connectivity
kubectl exec -it <pod-name> -n meta-repo-architecture -- curl http://localhost:8080/health
```

#### **Ingress Not Working**
```bash
# Check ingress controller
kubectl get ingress -n meta-repo-architecture

# Check ingress logs
kubectl logs -n ingress-nginx <ingress-pod>

# Test external access
curl -k https://meta-repo-architecture.org/health
```

#### **High Resource Usage**
```bash
# Check resource usage
kubectl top pods -n meta-repo-architecture

# Check HPA status
kubectl get hpa -n meta-repo-architecture

# Describe HPA for details
kubectl describe hpa <hpa-name> -n meta-repo-architecture
```

### 🚨 **Emergency Procedures**

#### **Critical System Failure**
```bash
# Scale up critical systems
kubectl scale deployment neuro-fcc-deployment --replicas=4 -n meta-repo-architecture
kubectl scale deployment vortex-shield-deployment --replicas=4 -n meta-repo-architecture
kubectl scale deployment bios-deployment --replicas=4 -n meta-repo-architecture

# Check system status
kubectl get pods -n meta-repo-architecture -l tier=critical
```

#### **Network Issues**
```bash
# Check network policies
kubectl get networkpolicy -n meta-repo-architecture

# Temporarily disable network policies
kubectl delete networkpolicy deny-all-egress -n meta-repo-architecture
kubectl delete networkpolicy deny-all-ingress -n meta-repo-architecture

# Re-enable after troubleshooting
kubectl apply -f network-policy.yaml
```

## 🔄 **Maintenance Operations**

### 📅 **Regular Maintenance**
```bash
# Update deployments
kubectl set image deployment/* meta-repo-architecture/*:new-version -n meta-repo-architecture

# Restart services
kubectl rollout restart deployment/* -n meta-repo-architecture

# Check rollout status
kubectl rollout status deployment/* -n meta-repo-architecture
```

### 🔄 **Rollback**
```bash
# Check rollout history
kubectl rollout history deployment <deployment-name> -n meta-repo-architecture

# Rollback to previous version
kubectl rollout undo deployment <deployment-name> -n meta-repo-architecture
```

### 📊 **Backup and Restore**
```bash
# Backup configurations
kubectl get all -n meta-repo-architecture -o yaml > backup.yaml

# Backup persistent data
kubectl exec -it prometheus-0 -n meta-repo-architecture -- tar czf /backup/prometheus-$(date +%Y%m%d).tar.gz /prometheus

# Restore from backup
kubectl apply -f backup.yaml
```

## 📞 **Support**

For Kubernetes deployment issues:
- **Documentation**: Check this README first
- **Logs**: Always check pod logs before reporting
- **Monitoring**: Use Grafana dashboards for system status
- **Emergency**: Use admin service account for critical issues

---

**🚀 Meta-repo-architecture - Kubernetes Deployment Guide**
