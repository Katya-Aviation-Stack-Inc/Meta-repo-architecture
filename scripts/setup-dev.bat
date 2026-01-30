@echo off
REM Meta-repo-architecture Development Environment Setup Script (Windows)
REM This script sets up the development environment for all 10 aviation technologies

setlocal enabledelayedexpansion

echo Meta-repo-architecture Development Environment Setup
echo ================================================

REM Check if Chocolatey is installed
where choco >nul 2>&1
if %errorlevel% neq 0 (
    echo Installing Chocolatey...
    powershell -Command "Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))"
    if %errorlevel% neq 0 (
        echo Failed to install Chocolatey
        exit /b 1
    )
)

REM Install dependencies
echo Installing Windows dependencies...
choco install -y cmake visualstudio2019buildtools git llvm

REM Check if Visual Studio Build Tools are installed
if not exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars64.bat" (
    echo Visual Studio Build Tools not found. Please install them manually.
    echo Download from: https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2019
    pause
    exit /b 1
)

REM Setup Git configuration
echo Setting up Git configuration...
git config --global init.defaultBranch main
git config --global pull.rebase false
git config --global core.autocrlf input

REM Create build directories
echo Setting up build directories...
for %%T in (
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
) do (
    if exist "%%T" (
        if not exist "%%T\build" mkdir "%%T\build"
        echo Created build directory for %%T
    ) else (
        echo Warning: Technology directory %%T not found
    )
)

REM Setup development tools
echo Setting up development tools...

REM Create .clang-format configuration
echo Creating .clang-format configuration...
(
echo BasedOnStyle: Google
echo IndentWidth: 4
echo TabWidth: 4
echo UseTab: Never
echo ColumnLimit: 100
echo BreakBeforeBraces: Attach
echo AllowShortFunctionsOnASingleLine: Empty
echo AllowShortIfStatementsOnASingleLine: false
echo AllowShortLoopsOnASingleLine: false
) > .clang-format

REM Create .clang-tidy configuration
echo Creating .clang-tidy configuration...
(
echo Checks: >
echo   modernize-*,
echo   performance-*,
echo   readability-*,
echo   bugprone-*,
echo   -modernize-use-trailing-return-type,
echo   -readability-magic-numbers,
echo   -bugprone-branch-clone
echo WarningsAsErrors: ''
echo HeaderFilterRegex: '.*'
echo FormatStyle: file
) > .clang-tidy

REM Create VS Code settings directory
if not exist ".vscode" mkdir .vscode

REM Create VS Code settings
echo Creating VS Code settings...
(
echo {
echo     "cmake.configureOnOpen": true,
echo     "cmake.buildDirectory": "${workspaceFolder}/${buildType}",
echo     "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools",
echo     "C_Cpp.default.cppStandard": "c++20",
echo     "C_Cpp.default.cStandard": "c99",
echo     "files.associations": {
echo         "*.h": "c",
echo         "*.hpp": "cpp",
echo         "*.cpp": "cpp",
echo         "*.c": "c"
echo     },
echo     "editor.formatOnSave": true,
echo     "editor.formatOnType": true,
echo     "editor.codeActionsOnSave": {
echo         "source.fixAll.clang-format": true
echo     }
echo }
) > .vscode\settings.json

REM Create VS Code tasks
echo Creating VS Code tasks...
(
echo {
echo     "version": "2.0.0",
echo     "tasks": [
echo         {
echo             "label": "Build All",
echo             "type": "shell",
echo             "command": "make -f build.mk all",
echo             "group": {
echo                 "kind": "build",
echo                 "isDefault": true
echo             },
echo             "problemMatcher": ["$msCompile"]
echo         },
echo         {
echo             "label": "Run Tests",
echo             "type": "shell",
echo             "command": "make -f build.mk test",
echo             "group": "test",
echo             "problemMatcher": ["$msCompile"]
echo         },
echo         {
echo             "label": "Run Demos",
echo             "type": "shell",
echo             "command": "make -f build.mk demos",
echo             "group": "test"
echo         },
echo         {
echo             "label": "Clean Build",
echo             "type": "shell",
echo             "command": "make -f build.mk clean",
echo             "group": "build"
echo         }
echo     ]
echo }
) > .vscode\tasks.json

REM Create VS Code launch configurations
echo Creating VS Code launch configurations...
(
echo {
echo     "version": "0.2.0",
echo     "configurations": [
echo         {
echo             "name": "Debug AirMesh Demo",
echo             "type": "cppvsdbg",
echo             "request": "launch",
echo             "program": "${workspaceFolder}/air-to-air-mesh/build/airmesh_demo.exe",
echo             "args": [],
echo             "stopAtEntry": false,
echo             "cwd": "${workspaceFolder}/air-to-air-mesh/build",
echo             "environment": [],
echo             "console": "externalTerminal"
echo         },
echo         {
echo             "name": "Debug Neuro-FCC Demo",
echo             "type": "cppvsdbg",
echo             "request": "launch",
echo             "program": "${workspaceFolder}/neuro-fcc/build/neuro_fcc_demo.exe",
echo             "args": [],
echo             "stopAtEntry": false,
echo             "cwd": "${workspaceFolder}/neuro-fcc/build",
echo             "environment": [],
echo             "console": "externalTerminal"
echo         }
echo     ]
echo }
) > .vscode\launch.json

REM Setup Docker environment
echo Setting up Docker environment...
where docker >nul 2>&1
if %errorlevel% equ 0 (
    docker network create aviation-network 2>nul
    echo Docker environment configured
) else (
    echo Docker not found. Please install Docker Desktop for Windows.
)

REM Setup monitoring
echo Setting up monitoring...
if not exist "monitoring" mkdir monitoring
if not exist "monitoring\grafana\dashboards" mkdir monitoring\grafana\dashboards
if not exist "monitoring\grafana\datasources" mkdir monitoring\grafana\datasources

REM Create Prometheus configuration
echo Creating Prometheus configuration...
(
echo global:
echo   scrape_interval: 15s
echo.
echo scrape_configs:
echo   - job_name: 'aviation-technologies'
echo     static_configs:
echo       - targets: ['localhost:8080', 'localhost:8081', 'localhost:8082']
) > monitoring\prometheus.yml

REM Create Grafana datasource configuration
echo Creating Grafana datasource configuration...
(
echo apiVersion: 1
echo.
echo datasources:
echo   - name: Prometheus
echo     type: prometheus
echo     access: proxy
echo     url: http://prometheus:9090
echo     isDefault: true
) > monitoring\grafana\datasources\prometheus.yml

REM Run initial build
echo Running initial build...
nmake -f build.mk all
if %errorlevel% equ 0 (
    echo Initial build completed successfully
) else (
    echo Initial build failed
    echo Please check the error messages above
)

echo.
echo Development environment setup completed!
echo.
echo Next steps:
echo 1. Review the documentation in docs/
echo 2. Run 'make -f build.mk help' for available commands
echo 3. Run 'make -f build.mk demos' to see all technologies in action
echo 4. Start developing!
echo.
echo Note: Some features may require additional manual configuration.
echo Please check the documentation for detailed setup instructions.

pause
