# Kubernetes Deployment Guide

## ☸️ **Complete Kubernetes Deployment Documentation**

This comprehensive guide covers the deployment of the Meta-repo-architecture project on Kubernetes, including configuration, scaling, monitoring, and best practices.

---

## 🎯 **Deployment Overview**

### **Kubernetes Architecture**

```
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                        Kubernetes Cluster Architecture                           │
├─────────────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐  │
│  │   Master    │  │   Worker    │  │   Worker    │  │   Worker    │  │   Worker    │  │
│  │   Node      │  │   Node 1    │  │   Node 2    │  │   Node 3    │  │   Node N    │  │
│  │             │  │             │  │             │  │             │  │             │  │
│  │ API Server  │  │ Vortex      │  │ Air Swarm   │  │ StarNav     │  │ Quantum     │  │
│  │ Scheduler   │  │ Shield      │  │ OS          │  │ Core        │  │ Radar       │  │
│  │ Controller  │  │ Neural      │  │ Fusion      │  │ Hypersonic  │  │ Plasma      │  │
│  │ Manager     │  │ Flight      │  │ Propulsion  │  │ Engine      │  │ Shield      │  │
│  │ etcd        │  │ Anti-       │  │ Warp        │  │ Monitoring  │  │ Storage     │  │
│  │             │  │ Gravity     │  │ Drive       │  │             │  │             │  │
│  └─────────────┘  └─────────────┘  └─────────────┘  └─────────────┘  └─────────────┘  │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────────────┘
```

### **Deployment Strategy**

| Strategy | Description | Use Case |
|----------|-------------|----------|
| **Development** | Single-node cluster for development | Local testing |
| **Staging** | Multi-node cluster for testing | Pre-production |
| **Production** | High-availability multi-zone cluster | Production deployment |
| **Edge** | Lightweight cluster for edge computing | Remote operations |

---

## 🛠️ **Prerequisites**

### **Cluster Requirements**

#### **Minimum Cluster Configuration**
```yaml
# Minimum cluster requirements
cluster_requirements:
  kubernetes_version: "1.25+"
  nodes:
    master: 1
    worker: 3
    
  master_node:
    cpu: "4 cores"
    memory: "8GB"
    storage: "100GB"
    
  worker_node:
    cpu: "8 cores"
    memory: "16GB"
    storage: "200GB"
    
  network:
    pod_cidr: "10.244.0.0/16"
    service_cidr: "10.96.0.0/12"
    
  storage:
    persistent_storage: "1TB"
    storage_class: "standard"
```

#### **Recommended Production Configuration**
```yaml
# Production cluster requirements
production_cluster:
  kubernetes_version: "1.27+"
  high_availability: true
  
  master_nodes: 3
  worker_nodes: 6+
  
  master_node:
    cpu: "8 cores"
    memory: "16GB"
    storage: "200GB SSD"
    
  worker_node:
    cpu: "16 cores"
    memory: "64GB"
    storage: "500GB SSD"
    
  networking:
    cni: "Calico"
    load_balancer: "MetalLB"
    ingress: "NGINX"
    
  storage:
    class: "fast-ssd"
    provisioner: "ceph"
    backup: "velero"
```

### **Software Requirements**

```bash
# Required tools
kubectl version --client
helm version
docker version
kubectx
kubens

# Optional tools
istioctl version
kustomize version
skaffold version
```

---

## 🚀 **Quick Start Deployment**

### **Step 1: Prepare Cluster**

```bash
# Create namespace
kubectl create namespace meta-repo-architecture

# Set current namespace
kubectl config set-context --current --namespace=meta-repo-architecture

# Verify cluster
kubectl cluster-info
kubectl get nodes
```

### **Step 2: Deploy Core Infrastructure**

```bash
# Deploy all manifests
kubectl apply -f k8s/namespace.yaml
kubectl apply -f k8s/configmap.yaml
kubectl apply -f k8s/secrets.yaml
kubectl apply -f k8s/services.yaml
kubectl apply -f k8s/deployments.yaml
kubectl apply -f k8s/ingress.yaml
kubectl apply -f k8s/hpa.yaml
kubectl apply -f k8s/monitoring.yaml
kubectl apply -f k8s/network-policy.yaml
kubectl apply -f k8s/rbac.yaml
```

### **Step 3: Verify Deployment**

```bash
# Check all resources
kubectl get all
kubectl get pods
kubectl get services
kubectl get ingress

# Check pod status
kubectl get pods -o wide
kubectl describe pod <pod-name>

# Check logs
kubectl logs -f deployment/vortex-shield
kubectl logs -f deployment/air-swarm-os
```

---

## 📦 **Component Deployment Details**

### 🌀 **Vortex Shield Deployment**

#### **Deployment Manifest**
```yaml
# k8s/deployments.yaml (Vortex Shield portion)
apiVersion: apps/v1
kind: Deployment
metadata:
  name: vortex-shield
  namespace: meta-repo-architecture
  labels:
    app: vortex-shield
    component: aviation-technology
spec:
  replicas: 2
  selector:
    matchLabels:
      app: vortex-shield
  template:
    metadata:
      labels:
        app: vortex-shield
        component: aviation-technology
    spec:
      serviceAccountName: vortex-shield-sa
      containers:
      - name: vortex-shield
        image: meta-repo/vortex-shield:latest
        ports:
        - containerPort: 8080
          name: http
        - containerPort: 9090
          name: metrics
        env:
        - name: SENSOR_CONFIG_PATH
          value: "/config/sensors.yaml"
        - name: API_KEY
          valueFrom:
            secretKeyRef:
              name: meta-repo-secrets
              key: vortex-shield-api-key
        resources:
          requests:
            memory: "256Mi"
            cpu: "250m"
          limits:
            memory: "512Mi"
            cpu: "500m"
        livenessProbe:
          httpGet:
            path: /health
            port: 8080
          initialDelaySeconds: 30
          periodSeconds: 10
        readinessProbe:
          httpGet:
            path: /ready
            port: 8080
          initialDelaySeconds: 5
          periodSeconds: 5
        volumeMounts:
        - name: config-volume
          mountPath: /config
        - name: logs-volume
          mountPath: /logs
      volumes:
      - name: config-volume
        configMap:
          name: vortex-shield-config
      - name: logs-volume
        emptyDir: {}
      securityContext:
        runAsNonRoot: true
        runAsUser: 1000
        fsGroup: 1000
```

#### **Service Configuration**
```yaml
# k8s/services.yaml (Vortex Shield portion)
apiVersion: v1
kind: Service
metadata:
  name: vortex-shield-service
  namespace: meta-repo-architecture
  labels:
    app: vortex-shield
spec:
  type: ClusterIP
  ports:
  - port: 80
    targetPort: 8080
    protocol: TCP
    name: http
  - port: 9090
    targetPort: 9090
    protocol: TCP
    name: metrics
  selector:
    app: vortex-shield
```

#### **Horizontal Pod Autoscaler**
```yaml
# k8s/hpa.yaml (Vortex Shield portion)
apiVersion: autoscaling/v2
kind: HorizontalPodAutoscaler
metadata:
  name: vortex-shield-hpa
  namespace: meta-repo-architecture
spec:
  scaleTargetRef:
    apiVersion: apps/v1
    kind: Deployment
    name: vortex-shield
  minReplicas: 2
  maxReplicas: 10
  metrics:
  - type: Resource
    resource:
      name: cpu
      target:
        type: Utilization
        averageUtilization: 70
  - type: Resource
    resource:
      name: memory
      target:
        type: Utilization
        averageUtilization: 80
  behavior:
    scaleDown:
      stabilizationWindowSeconds: 300
      policies:
      - type: Percent
        value: 10
        periodSeconds: 60
    scaleUp:
      stabilizationWindowSeconds: 60
      policies:
      - type: Percent
        value: 50
        periodSeconds: 60
```

### ⚡ **Air Swarm OS Deployment**

#### **StatefulSet Configuration**
```yaml
# k8s/air-swarm-os-statefulset.yaml
apiVersion: apps/v1
kind: StatefulSet
metadata:
  name: air-swarm-os
  namespace: meta-repo-architecture
spec:
  serviceName: air-swarm-os-headless
  replicas: 3
  selector:
    matchLabels:
      app: air-swarm-os
  template:
    metadata:
      labels:
        app: air-swarm-os
    spec:
      serviceAccountName: air-swarm-os-sa
      containers:
      - name: air-swarm-os
        image: meta-repo/air-swarm-os:latest
        ports:
        - containerPort: 8080
          name: http
        - containerPort: 8081
          name: swarm
        env:
        - name: NODE_ID
          valueFrom:
            fieldRef:
              fieldPath: metadata.name
        - name: SWARM_SIZE
          value: "3"
        - name: NETWORK_KEY
          valueFrom:
            secretKeyRef:
              name: meta-repo-secrets
              key: swarm-network-key
        resources:
          requests:
            memory: "512Mi"
            cpu: "500m"
          limits:
            memory: "1Gi"
            cpu: "1000m"
        volumeMounts:
        - name: data-volume
          mountPath: /data
        - name: config-volume
          mountPath: /config
  volumeClaimTemplates:
  - metadata:
      name: data-volume
    spec:
      accessModes: ["ReadWriteOnce"]
      storageClassName: "fast-ssd"
      resources:
        requests:
          storage: 10Gi
  volumes:
  - name: config-volume
    configMap:
      name: air-swarm-os-config
```

---

## 🔧 **Configuration Management**

### **ConfigMaps**

#### **Application Configuration**
```yaml
# k8s/configmap.yaml
apiVersion: v1
kind: ConfigMap
metadata:
  name: meta-repo-config
  namespace: meta-repo-architecture
data:
  # Vortex Shield Configuration
  vortex_shield_config.yaml: |
    sensors:
      pressure:
        sampling_rate: 1000
        resolution: 16
      flow:
        sampling_rate: 500
        resolution: 12
      acoustic:
        sampling_rate: 2000
        resolution: 24
    
    detection:
      sensitivity: 0.8
      response_time: 10
    
    recovery:
      max_pitch_change: 15
      safety_margin: 2.0

  # Air Swarm OS Configuration
  air_swarm_config.yaml: |
    swarm:
      max_nodes: 1000
      heartbeat_interval: 100
      consensus_timeout: 10000
    
    network:
      protocol: "udp-secure"
      port: 8081
      tls_version: "1.3"
    
    security:
      encryption: "aes-256-gcm"
      signature: "ecdsa-p384"

  # StarNav Core Configuration
  starnav_config.yaml: |
    camera:
      resolution: "4096x4096"
      bit_depth: 16
      exposure_range: [0.1, 10.0]
    
    processing:
      star_detection_algorithm: "deep_learning"
      position_calculation: "least_squares"
      attitude_estimation: "extended_kalman_filter"
    
    accuracy:
      position_accuracy: 1.0
      attitude_accuracy: 0.01
      update_rate: 100
```

### **Secrets Management**

#### **Sensitive Configuration**
```yaml
# k8s/secrets.yaml
apiVersion: v1
kind: Secret
metadata:
  name: meta-repo-secrets
  namespace: meta-repo-architecture
type: Opaque
data:
  # Base64 encoded values
  vortex-shield-api-key: <base64-encoded-api-key>
  swarm-network-key: <base64-encoded-network-key>
  starnav-database-password: <base64-encoded-password>
  quantum-radar-encryption-key: <base64-encoded-key>
  neural-flight-model-key: <base64-encoded-key>
  fusion-reactor-control-key: <base64-encoded-key>
  hypersonic-engine-auth-token: <base64-encoded-token>
  plasma-shield-field-key: <base64-encoded-key>
  anti-gravity-quantum-key: <base64-encoded-key>
  warp-drive-exotic-matter-key: <base64-encoded-key>
  
  # Database credentials
  database-username: <base64-encoded-username>
  database-password: <base64-encoded-password>
  database-host: <base64-encoded-host>
  
  # Redis credentials
  redis-password: <base64-encoded-password>
  redis-host: <base64-encoded-host>
  
  # JWT secrets
  jwt-secret: <base64-encoded-jwt-secret>
  jwt-issuer: <base64-encoded-issuer>
  
  # SSL certificates
  ssl-cert: <base64-encoded-ssl-cert>
  ssl-key: <base64-encoded-ssl-key>
  
  # Monitoring credentials
  prometheus-password: <base64-encoded-password>
  grafana-password: <base64-encoded-password>
  
  # Backup credentials
  backup-access-key: <base64-encoded-access-key>
  backup-secret-key: <base64-encoded-secret-key>
```

---

## 🌐 **Networking Configuration**

### **Ingress Configuration**

#### **Main Application Ingress**
```yaml
# k8s/ingress.yaml
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: meta-repo-ingress
  namespace: meta-repo-architecture
  annotations:
    kubernetes.io/ingress.class: "nginx"
    nginx.ingress.kubernetes.io/ssl-redirect: "true"
    nginx.ingress.kubernetes.io/use-regex: "true"
    nginx.ingress.kubernetes.io/rewrite-target: /$2
    nginx.ingress.kubernetes.io/rate-limit: "100"
    nginx.ingress.kubernetes.io/rate-limit-window: "1m"
    cert-manager.io/cluster-issuer: "letsencrypt-prod"
spec:
  tls:
  - hosts:
    - api.meta-repo-architecture.com
    secretName: meta-repo-tls
  rules:
  - host: api.meta-repo-architecture.com
    http:
      paths:
      - path: /vortex-shield(/|$)(.*)
        pathType: Prefix
        backend:
          service:
            name: vortex-shield-service
            port:
              number: 80
      - path: /air-swarm-os(/|$)(.*)
        pathType: Prefix
        backend:
          service:
            name: air-swarm-os-service
            port:
              number: 80
      - path: /starnav-core(/|$)(.*)
        pathType: Prefix
        backend:
          service:
            name: starnav-core-service
            port:
              number: 80
      - path: /quantum-radar(/|$)(.*)
        pathType: Prefix
        backend:
          service:
            name: quantum-radar-service
            port:
              number: 80
      - path: /neural-flight(/|$)(.*)
        pathType: Prefix
        backend:
          service:
            name: neural-flight-service
            port:
              number: 80
```

#### **Monitoring Ingress**
```yaml
# k8s/monitoring-ingress.yaml
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: monitoring-ingress
  namespace: meta-repo-architecture
  annotations:
    kubernetes.io/ingress.class: "nginx"
    nginx.ingress.kubernetes.io/auth-type: "basic"
    nginx.ingress.kubernetes.io/auth-secret: "monitoring-auth"
    nginx.ingress.kubernetes.io/auth-realm: "Monitoring Authentication"
    cert-manager.io/cluster-issuer: "letsencrypt-prod"
spec:
  tls:
  - hosts:
    - monitoring.meta-repo-architecture.com
    secretName: monitoring-tls
  rules:
  - host: monitoring.meta-repo-architecture.com
    http:
      paths:
      - path: /prometheus(/|$)(.*)
        pathType: Prefix
        backend:
          service:
            name: prometheus-service
            port:
              number: 9090
      - path: /grafana(/|$)(.*)
        pathType: Prefix
        backend:
          service:
            name: grafana-service
            port:
              number: 3000
```

### **Network Policies**

#### **Default Deny Policy**
```yaml
# k8s/network-policy.yaml
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: default-deny-all
  namespace: meta-repo-architecture
spec:
  podSelector: {}
  policyTypes:
  - Ingress
  - Egress
```

#### **Technology-Specific Policies**
```yaml
# Vortex Shield Network Policy
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: vortex-shield-policy
  namespace: meta-repo-architecture
spec:
  podSelector:
    matchLabels:
      app: vortex-shield
  policyTypes:
  - Ingress
  - Egress
  ingress:
  - from:
    - podSelector:
        matchLabels:
          app: neural-flight-control
    - podSelector:
        matchLabels:
          app: gateway
    ports:
    - protocol: TCP
      port: 8080
    - protocol: TCP
      port: 9090
  egress:
  - to:
    - podSelector:
        matchLabels:
          app: database
    ports:
    - protocol: TCP
      port: 5432
  - to: []
    ports:
    - protocol: TCP
      port: 53
    - protocol: UDP
      port: 53
    - protocol: TCP
      port: 443
```

---

## 🔒 **Security Configuration**

### **RBAC Configuration**

#### **Service Accounts**
```yaml
# k8s/rbac.yaml
apiVersion: v1
kind: ServiceAccount
metadata:
  name: vortex-shield-sa
  namespace: meta-repo-architecture
---
apiVersion: v1
kind: ServiceAccount
metadata:
  name: air-swarm-os-sa
  namespace: meta-repo-architecture
---
apiVersion: v1
kind: ServiceAccount
metadata:
  name: starnav-core-sa
  namespace: meta-repo-architecture
```

#### **Roles and Role Bindings**
```yaml
# Vortex Shield Role
apiVersion: rbac.authorization.k8s.io/v1
kind: Role
metadata:
  namespace: meta-repo-architecture
  name: vortex-shield-role
rules:
- apiGroups: [""]
  resources: ["configmaps", "secrets"]
  verbs: ["get", "list", "watch"]
- apiGroups: [""]
  resources: ["pods"]
  verbs: ["get", "list"]
- apiGroups: ["apps"]
  resources: ["deployments"]
  verbs: ["get", "list"]
---
apiVersion: rbac.authorization.k8s.io/v1
kind: RoleBinding
metadata:
  name: vortex-shield-binding
  namespace: meta-repo-architecture
subjects:
- kind: ServiceAccount
  name: vortex-shield-sa
  namespace: meta-repo-architecture
roleRef:
  kind: Role
  name: vortex-shield-role
  apiGroup: rbac.authorization.k8s.io
```

### **Pod Security Policies**

#### **Security Context**
```yaml
# Pod Security Policy
apiVersion: policy/v1beta1
kind: PodSecurityPolicy
metadata:
  name: meta-repo-psp
spec:
  privileged: false
  allowPrivilegeEscalation: false
  requiredDropCapabilities:
    - ALL
  volumes:
    - 'configMap'
    - 'emptyDir'
    - 'projected'
    - 'secret'
    - 'downwardAPI'
    - 'persistentVolumeClaim'
  runAsUser:
    rule: 'MustRunAsNonRoot'
  seLinux:
    rule: 'RunAsAny'
  fsGroup:
    rule: 'RunAsAny'
```

---

## 📊 **Monitoring and Observability**

### **Prometheus Configuration**

#### **Prometheus Deployment**
```yaml
# k8s/monitoring.yaml (Prometheus portion)
apiVersion: apps/v1
kind: Deployment
metadata:
  name: prometheus
  namespace: meta-repo-architecture
spec:
  replicas: 1
  selector:
    matchLabels:
      app: prometheus
  template:
    metadata:
      labels:
        app: prometheus
    spec:
      serviceAccountName: prometheus-sa
      containers:
      - name: prometheus
        image: prom/prometheus:latest
        ports:
        - containerPort: 9090
        volumeMounts:
        - name: config-volume
          mountPath: /etc/prometheus
        - name: data-volume
          mountPath: /prometheus
        args:
        - '--config.file=/etc/prometheus/prometheus.yml'
        - '--storage.tsdb.path=/prometheus'
        - '--web.console.libraries=/etc/prometheus/console_libraries'
        - '--web.console.templates=/etc/prometheus/consoles'
        - '--storage.tsdb.retention.time=200h'
        - '--web.enable-lifecycle'
      volumes:
      - name: config-volume
        configMap:
          name: prometheus-config
      - name: data-volume
        persistentVolumeClaim:
          claimName: prometheus-pvc
```

#### **Prometheus Configuration**
```yaml
# Prometheus ConfigMap
apiVersion: v1
kind: ConfigMap
metadata:
  name: prometheus-config
  namespace: meta-repo-architecture
data:
  prometheus.yml: |
    global:
      scrape_interval: 15s
      evaluation_interval: 15s
    
    rule_files:
      - "/etc/prometheus/rules/*.yml"
    
    scrape_configs:
      - job_name: 'kubernetes-apiservers'
        kubernetes_sd_configs:
        - role: endpoints
        scheme: https
        tls_config:
          ca_file: /var/run/secrets/kubernetes.io/serviceaccount/ca.crt
        bearer_token_file: /var/run/secrets/kubernetes.io/serviceaccount/token
        relabel_configs:
        - source_labels: [__meta_kubernetes_namespace, __meta_kubernetes_service_name, __meta_kubernetes_endpoint_port_name]
          action: keep
          regex: default;kubernetes;https
      
      - job_name: 'vortex-shield'
        kubernetes_sd_configs:
        - role: endpoints
          namespaces:
            names:
            - meta-repo-architecture
        relabel_configs:
        - source_labels: [__meta_kubernetes_service_name]
          action: keep
          regex: vortex-shield-service
        - source_labels: [__meta_kubernetes_endpoint_port_name]
          action: keep
          regex: metrics
      
      - job_name: 'air-swarm-os'
        kubernetes_sd_configs:
        - role: endpoints
          namespaces:
            names:
            - meta-repo-architecture
        relabel_configs:
        - source_labels: [__meta_kubernetes_service_name]
          action: keep
          regex: air-swarm-os-service
        - source_labels: [__meta_kubernetes_endpoint_port_name]
          action: keep
          regex: metrics
```

### **Grafana Configuration**

#### **Grafana Deployment**
```yaml
# k8s/monitoring.yaml (Grafana portion)
apiVersion: apps/v1
kind: Deployment
metadata:
  name: grafana
  namespace: meta-repo-architecture
spec:
  replicas: 1
  selector:
    matchLabels:
      app: grafana
  template:
    metadata:
      labels:
        app: grafana
    spec:
      containers:
      - name: grafana
        image: grafana/grafana:latest
        ports:
        - containerPort: 3000
        env:
        - name: GF_SECURITY_ADMIN_PASSWORD
          valueFrom:
            secretKeyRef:
              name: meta-repo-secrets
              key: grafana-password
        - name: GF_INSTALL_PLUGINS
          value: "grafana-piechart-panel"
        volumeMounts:
        - name: data-volume
          mountPath: /var/lib/grafana
        - name: config-volume
          mountPath: /etc/grafana/provisioning
      volumes:
      - name: data-volume
        persistentVolumeClaim:
          claimName: grafana-pvc
      - name: config-volume
        configMap:
          name: grafana-config
```

---

## 🔄 **CI/CD Integration**

### **GitOps with ArgoCD**

#### **ArgoCD Application**
```yaml
# argocd/application.yaml
apiVersion: argoproj.io/v1alpha1
kind: Application
metadata:
  name: meta-repo-architecture
  namespace: argocd
spec:
  project: default
  source:
    repoURL: https://github.com/Katya-Aviation-Stack-Inc/Meta-repo-architecture.git
    targetRevision: HEAD
    path: k8s
  destination:
    server: https://kubernetes.default.svc
    namespace: meta-repo-architecture
  syncPolicy:
    automated:
      prune: true
      selfHeal: true
    syncOptions:
    - CreateNamespace=true
```

### **GitHub Actions Workflow**

#### **Deployment Pipeline**
```yaml
# .github/workflows/k8s-deploy.yml
name: Kubernetes Deployment

on:
  push:
    branches: [main]
  pull_request:
    branches: [main]

jobs:
  test:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v3
    
    - name: Set up Docker Buildx
      uses: docker/setup-buildx-action@v2
    
    - name: Build and test
      run: |
        docker build -t meta-repo/test .
        docker run meta-repo/test ./run_tests.sh
    
    - name: Run integration tests
      run: |
        kubectl apply -f k8s/
        kubectl wait --for=condition=ready pod -l app=vortex-shield --timeout=300s
        ./scripts/run_integration_tests.sh

  deploy:
    needs: test
    runs-on: ubuntu-latest
    if: github.ref == 'refs/heads/main'
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Configure kubectl
      uses: azure/k8s-set-context@v1
      with:
        method: kubeconfig
        kubeconfig: ${{ secrets.KUBE_CONFIG }}
    
    - name: Deploy to Kubernetes
      run: |
        kubectl apply -f k8s/
        kubectl rollout status deployment/vortex-shield
        kubectl rollout status deployment/air-swarm-os
        kubectl rollout status deployment/starnav-core
    
    - name: Verify deployment
      run: |
        kubectl get pods
        kubectl get services
        ./scripts/verify_deployment.sh
```

---

## 🚨 **Troubleshooting**

### **Common Issues**

#### **Pod Not Starting**
```bash
# Check pod status
kubectl get pods -o wide
kubectl describe pod <pod-name>

# Check events
kubectl get events --sort-by=.metadata.creationTimestamp

# Check logs
kubectl logs <pod-name>
kubectl logs <pod-name> --previous
```

#### **Service Not Accessible**
```bash
# Check service
kubectl get svc
kubectl describe svc <service-name>

# Check endpoints
kubectl get endpoints

# Test connectivity
kubectl exec -it <pod-name> -- curl http://<service-name>
```

#### **Ingress Issues**
```bash
# Check ingress
kubectl get ingress
kubectl describe ingress <ingress-name>

# Check nginx controller logs
kubectl logs -n ingress-nginx deployment/ingress-nginx-controller

# Test ingress
curl -H "Host: api.meta-repo-architecture.com" http://<ingress-ip>/vortex-shield/health
```

### **Performance Issues**

#### **High Resource Usage**
```bash
# Check resource usage
kubectl top nodes
kubectl top pods

# Check resource limits
kubectl describe pod <pod-name> | grep -A 10 Limits

# Adjust resources
kubectl patch deployment <deployment-name> -p '{"spec":{"template":{"spec":{"containers":[{"name":"<container-name>","resources":{"limits":{"cpu":"1000m","memory":"2Gi"}}}]}}}}'
```

#### **Network Issues**
```bash
# Check network policies
kubectl get networkpolicy
kubectl describe networkpolicy <policy-name>

# Test connectivity
kubectl exec -it <pod-name> -- ping <target-pod>
kubectl exec -it <pod-name> -- nslookup <service-name>

# Check DNS
kubectl exec -it <pod-name> -- nslookup kubernetes.default
```

---

## 📈 **Scaling and Performance**

### **Horizontal Scaling**

#### **Manual Scaling**
```bash
# Scale deployment
kubectl scale deployment vortex-shield --replicas=5
kubectl scale deployment air-swarm-os --replicas=4

# Scale statefulset
kubectl scale statefulset air-swarm-os --replicas=5
```

#### **Auto Scaling**
```bash
# Check HPA status
kubectl get hpa
kubectl describe hpa <hpa-name>

# Adjust HPA
kubectl patch hpa vortex-shield-hpa --patch '{"spec":{"minReplicas":3,"maxReplicas":20}}'
```

### **Vertical Scaling**

#### **Resource Requests and Limits**
```yaml
# Example vertical scaling
resources:
  requests:
    memory: "1Gi"
    cpu: "500m"
  limits:
    memory: "2Gi"
    cpu: "1000m"
```

#### **Vertical Pod Autoscaler**
```yaml
# VPA Configuration
apiVersion: autoscaling.k8s.io/v1
kind: VerticalPodAutoscaler
metadata:
  name: vortex-shield-vpa
  namespace: meta-repo-architecture
spec:
  targetRef:
    apiVersion: apps/v1
    kind: Deployment
    name: vortex-shield
  updatePolicy:
    updateMode: "Auto"
  resourcePolicy:
    containerPolicies:
    - containerName: vortex-shield
      maxAllowed:
        cpu: 2
        memory: 4Gi
      minAllowed:
        cpu: 100m
        memory: 128Mi
```

---

## 🔄 **Maintenance and Updates**

### **Rolling Updates**

#### **Update Strategy**
```yaml
# Deployment update strategy
spec:
  strategy:
    type: RollingUpdate
    rollingUpdate:
      maxUnavailable: 25%
      maxSurge: 25%
```

#### **Update Commands**
```bash
# Update image
kubectl set image deployment/vortex-shield vortex-shield=meta-repo/vortex-shield:v2.0.0

# Rollout status
kubectl rollout status deployment/vortex-shield

# Rollback if needed
kubectl rollout undo deployment/vortex-shield
kubectl rollout undo deployment/vortex-shield --to-revision=2
```

### **Backup and Recovery**

#### **Velero Backup**
```bash
# Install Velero
velero install --provider aws --bucket velero-backups --secret-file ./credentials-velero

# Create backup
velero backup create meta-repo-backup --include-namespaces meta-repo-architecture

# Restore from backup
velero restore create --from-backup meta-repo-backup
```

---

## 📚 **Best Practices**

### **Security Best Practices**

1. **Use Network Policies**: Implement least privilege network access
2. **RBAC**: Implement proper role-based access control
3. **Secrets Management**: Use Kubernetes secrets or external secret management
4. **Image Security**: Use signed images and vulnerability scanning
5. **Pod Security**: Implement security contexts and policies

### **Performance Best Practices**

1. **Resource Limits**: Set appropriate resource requests and limits
2. **Horizontal Scaling**: Use HPA for automatic scaling
3. **Vertical Scaling**: Use VPA for resource optimization
4. **Node Affinity**: Use node affinity for performance-critical workloads
5. **Local Storage**: Use local SSD for high-performance storage

### **Reliability Best Practices**

1. **High Availability**: Deploy multiple replicas
2. **Health Checks**: Implement proper liveness and readiness probes
3. **Graceful Shutdown**: Handle SIGTERM properly
4. **Backup Strategy**: Regular backups and disaster recovery
5. **Monitoring**: Comprehensive monitoring and alerting

---

## 🚀 **Next Steps**

### **Advanced Features**

1. **Service Mesh**: Implement Istio or Linkerd for advanced networking
2. **GitOps**: Full GitOps workflow with ArgoCD or Flux
3. **Multi-Cluster**: Multi-cluster deployment with cluster federation
4. **Edge Computing**: Lightweight Kubernetes for edge deployment
5. **Serverless**: Knative or OpenFaaS for serverless functions

### **Production Readiness**

- [ ] Security audit completed
- [ ] Performance testing completed
- [ ] Disaster recovery tested
- [ ] Monitoring and alerting configured
- [ ] Backup and restore verified
- [ ] Team training completed

---

*Last updated: January 2026*  
*Kubernetes Team*  
*Version: 1.0*
