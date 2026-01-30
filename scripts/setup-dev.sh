#!/bin/bash

# Meta-repo-architecture Development Environment Setup Script
# This script sets up the development environment for all 10 aviation technologies

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

# Detect operating system
detect_os() {
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        OS="linux"
        if command -v apt-get >/dev/null 2>&1; then
            DISTRO="debian"
        elif command -v yum >/dev/null 2>&1; then
            DISTRO="rhel"
        elif command -v pacman >/dev/null 2>&1; then
            DISTRO="arch"
        fi
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        OS="macos"
    elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]]; then
        OS="windows"
    else
        OS="unknown"
    fi
}

# Install dependencies for Linux
install_linux_deps() {
    print_status "Installing Linux dependencies..."
    
    case $DISTRO in
        debian)
            sudo apt-get update
            sudo apt-get install -y \
                cmake \
                build-essential \
                gcc \
                g++ \
                clang \
                clang-format \
                clang-tidy \
                valgrind \
                doxygen \
                graphviz \
                git \
                pkg-config \
                libpthread-stubs0-dev \
                libssl-dev \
                libboost-all-dev
            ;;
        rhel)
            sudo yum groupinstall -y "Development Tools"
            sudo yum install -y \
                cmake \
                gcc \
                gcc-c++ \
                clang \
                clang-tools-extra \
                valgrind \
                doxygen \
                graphviz \
                git \
                pkgconfig \
                openssl-devel \
                boost-devel
            ;;
        arch)
            sudo pacman -Syu --noconfirm
            sudo pacman -S --noconfirm \
                cmake \
                gcc \
                clang \
                clang-tools-extra \
                valgrind \
                doxygen \
                graphviz \
                git \
                pkgconf \
                openssl \
                boost
            ;;
    esac
}

# Install dependencies for macOS
install_macos_deps() {
    print_status "Installing macOS dependencies..."
    
    # Check if Homebrew is installed
    if ! command -v brew >/dev/null 2>&1; then
        print_status "Installing Homebrew..."
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    fi
    
    # Install dependencies
    brew update
    brew install \
        cmake \
        gcc \
        clang \
        clang-format \
        clang-tidy \
        valgrind \
        doxygen \
        graphviz \
        git \
        pkg-config \
        openssl \
        boost
}

# Install dependencies for Windows
install_windows_deps() {
    print_status "Installing Windows dependencies..."
    
    # Check if Chocolatey is installed
    if ! command -v choco >/dev/null 2>&1; then
        print_status "Installing Chocolatey..."
        powershell -Command "Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))"
    fi
    
    # Install dependencies
    choco install -y \
        cmake \
        visualstudio2019buildtools \
        git \
        llvm
}

# Setup Git configuration
setup_git() {
    print_status "Setting up Git configuration..."
    
    # Set default branch name
    git config --global init.defaultBranch main
    
    # Set reasonable defaults
    git config --global pull.rebase false
    git config --global core.autocrlf input
    
    # Setup Git hooks
    if [ ! -d ".git/hooks" ]; then
        mkdir -p .git/hooks
    fi
    
    # Install pre-commit hook
    cat > .git/hooks/pre-commit << 'EOF'
#!/bin/bash
# Pre-commit hook for Meta-repo-architecture

echo "Running pre-commit checks..."

# Format code
echo "Formatting code..."
find . -name "*.cpp" -o -name "*.c" -o -name "*.h" -o -name "*.hpp" | grep -v build | xargs clang-format -i || true

# Run basic linting
echo "Running basic linting..."
for tech in air-to-air-mesh neuro-fcc self-adaptive-rotor-blades cold-jet local-gravity-field-navigation predictive-airflow-engine self-healing-avionics-bios vortex-shield air-swarm-os star-nav-core; do
    if [ -d "$tech/src" ]; then
        find "$tech/src" -name "*.cpp" -o -name "*.c" | head -5 | xargs clang-tidy --warnings-as-errors='*' -checks='-*,modernize-*,performance-*,readability-*,bugprone-*' || true
    fi
done

echo "Pre-commit checks completed."
EOF
    
    chmod +x .git/hooks/pre-commit
    print_success "Git hooks installed"
}

# Setup build directories
setup_build_dirs() {
    print_status "Setting up build directories..."
    
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
    
    for tech in "${TECHNOLOGIES[@]}"; do
        if [ -d "$tech" ]; then
            mkdir -p "$tech/build"
            print_success "Created build directory for $tech"
        else
            print_warning "Technology directory $tech not found"
        fi
    done
}

# Setup development tools
setup_dev_tools() {
    print_status "Setting up development tools..."
    
    # Create .clang-format configuration
    cat > .clang-format << 'EOF'
BasedOnStyle: Google
IndentWidth: 4
TabWidth: 4
UseTab: Never
ColumnLimit: 100
BreakBeforeBraces: Attach
AllowShortFunctionsOnASingleLine: Empty
AllowShortIfStatementsOnASingleLine: false
AllowShortLoopsOnASingleLine: false
EOF
    
    # Create .clang-tidy configuration
    cat > .clang-tidy << 'EOF'
Checks: >
  modernize-*,
  performance-*,
  readability-*,
  bugprone-*,
  -modernize-use-trailing-return-type,
  -readability-magic-numbers,
  -bugprone-branch-clone
WarningsAsErrors: ''
HeaderFilterRegex: '.*'
FormatStyle: file
EOF
    
    # Create VS Code settings
    mkdir -p .vscode
    cat > .vscode/settings.json << 'EOF'
{
    "cmake.configureOnOpen": true,
    "cmake.buildDirectory": "${workspaceFolder}/${buildType}",
    "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools",
    "C_Cpp.default.cppStandard": "c++20",
    "C_Cpp.default.cStandard": "c99",
    "files.associations": {
        "*.h": "c",
        "*.hpp": "cpp",
        "*.cpp": "cpp",
        "*.c": "c"
    },
    "editor.formatOnSave": true,
    "editor.formatOnType": true,
    "editor.codeActionsOnSave": {
        "source.fixAll.clang-format": true
    }
}
EOF
    
    # Create VS Code tasks
    cat > .vscode/tasks.json << 'EOF'
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Build All",
            "type": "shell",
            "command": "make -f build.mk all",
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "problemMatcher": ["$gcc"]
        },
        {
            "label": "Run Tests",
            "type": "shell",
            "command": "make -f build.mk test",
            "group": "test",
            "problemMatcher": ["$gcc"]
        },
        {
            "label": "Run Demos",
            "type": "shell",
            "command": "make -f build.mk demos",
            "group": "test"
        },
        {
            "label": "Clean Build",
            "type": "shell",
            "command": "make -f build.mk clean",
            "group": "build"
        }
    ]
}
EOF
    
    # Create VS Code launch configurations
    cat > .vscode/launch.json << 'EOF'
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug AirMesh Demo",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/air-to-air-mesh/build/airmesh_demo",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}/air-to-air-mesh/build",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb"
        },
        {
            "name": "Debug Neuro-FCC Demo",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/neuro-fcc/build/neuro_fcc_demo",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}/neuro-fcc/build",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb"
        }
    ]
}
EOF
    
    print_success "Development tools configured"
}

# Setup Docker environment
setup_docker() {
    print_status "Setting up Docker environment..."
    
    # Check if Docker is installed
    if ! command -v docker >/dev/null 2>&1; then
        print_warning "Docker not found. Please install Docker manually."
        return
    fi
    
    # Check if Docker Compose is installed
    if ! command -v docker-compose >/dev/null 2>&1; then
        print_warning "Docker Compose not found. Please install Docker Compose manually."
        return
    fi
    
    # Create Docker network
    docker network create aviation-network 2>/dev/null || true
    
    print_success "Docker environment configured"
}

# Setup monitoring
setup_monitoring() {
    print_status "Setting up monitoring..."
    
    # Create monitoring directories
    mkdir -p monitoring/grafana/dashboards
    mkdir -p monitoring/grafana/datasources
    
    # Create Prometheus configuration
    cat > monitoring/prometheus.yml << 'EOF'
global:
  scrape_interval: 15s

scrape_configs:
  - job_name: 'aviation-technologies'
    static_configs:
      - targets: ['localhost:8080', 'localhost:8081', 'localhost:8082']
EOF
    
    # Create Grafana datasource configuration
    cat > monitoring/grafana/datasources/prometheus.yml << 'EOF'
apiVersion: 1

datasources:
  - name: Prometheus
    type: prometheus
    access: proxy
    url: http://prometheus:9090
    isDefault: true
EOF
    
    print_success "Monitoring configured"
}

# Run initial build
run_initial_build() {
    print_status "Running initial build..."
    
    # Build all technologies
    if make -f build.mk all; then
        print_success "Initial build completed successfully"
    else
        print_error "Initial build failed"
        return 1
    fi
}

# Verify installation
verify_installation() {
    print_status "Verifying installation..."
    
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
    
    all_good=true
    
    # Check if all technology directories exist
    for tech in "${TECHNOLOGIES[@]}"; do
        if [ -d "$tech" ]; then
            print_success "Technology directory found: $tech"
        else
            print_error "Technology directory missing: $tech"
            all_good=false
        fi
    done
    
    # Check if build directories exist
    for tech in "${TECHNOLOGIES[@]}"; do
        if [ -d "$tech/build" ]; then
            print_success "Build directory found: $tech/build"
        else
            print_error "Build directory missing: $tech/build"
            all_good=false
        fi
    done
    
    # Check if demos are built
    for tech in "${TECHNOLOGIES[@]}"; do
        if [ -f "$tech/build/*_demo" ]; then
            print_success "Demo built: $tech"
        else
            print_warning "Demo not built: $tech"
        fi
    done
    
    if [ "$all_good" = true ]; then
        print_success "Installation verification completed successfully"
    else
        print_error "Installation verification failed"
        return 1
    fi
}

# Main setup function
main() {
    print_status "Meta-repo-architecture Development Environment Setup"
    print_status "=================================================="
    
    # Detect operating system
    detect_os
    print_status "Detected OS: $OS"
    
    if [ "$OS" = "unknown" ]; then
        print_error "Unsupported operating system"
        exit 1
    fi
    
    # Install dependencies
    case $OS in
        linux)
            install_linux_deps
            ;;
        macos)
            install_macos_deps
            ;;
        windows)
            install_windows_deps
            ;;
    esac
    
    # Setup development environment
    setup_git
    setup_build_dirs
    setup_dev_tools
    setup_docker
    setup_monitoring
    
    # Run initial build
    if [ "$1" != "--no-build" ]; then
        run_initial_build
    fi
    
    # Verify installation
    verify_installation
    
    print_success "Development environment setup completed!"
    print_status ""
    print_status "Next steps:"
    print_status "1. Review the documentation in docs/"
    print_status "2. Run 'make -f build.mk help' for available commands"
    print_status "3. Run 'make -f build.mk demos' to see all technologies in action"
    print_status "4. Start developing!"
}

# Script entry point
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    main "$@"
fi
