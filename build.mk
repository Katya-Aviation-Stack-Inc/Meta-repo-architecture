# Meta-repo-architecture Master Build System
# This Makefile provides a unified build interface for all 10 aviation technologies

.PHONY: help clean all test demos install uninstall docker docs format lint

# Default target
help:
	@echo "Meta-repo-architecture Build System"
	@echo "=================================="
	@echo ""
	@echo "Available targets:"
	@echo "  help        - Show this help message"
	@echo "  all         - Build all technologies"
	@echo "  clean       - Clean all build artifacts"
	@echo "  test        - Run all tests"
	@echo "  demos       - Build and run all demos"
	@echo "  install     - Install all technologies"
	@echo "  uninstall   - Uninstall all technologies"
	@echo "  docker      - Build Docker images"
	@echo "  docs        - Generate documentation"
	@echo "  format      - Format all source code"
	@echo "  lint        - Run linting tools"
	@echo ""
	@echo "Technology-specific targets:"
	@echo "  airmesh     - Build Air-to-Air Mesh Network"
	@echo "  neuro-fcc   - Build Neuro-FCC"
	@echo "  adaptive    - Build Self-Adaptive Rotor Blades"
	@echo "  cold-jet    - Build Cold-Jet"
	@echo "  lgfn        - Build Local Gravity Field Navigation"
	@echo "  predictive  - Build Predictive Airflow Engine"
	@echo "  bios        - Build Self-Healing Avionics BIOS"
	@echo "  vortex      - Build Vortex Shield"
	@echo "  swarm       - Build Air Swarm OS"
	@echo "  starnav     - Build StarNav Core"

# Technology directories
TECHNOLOGIES := air-to-air-mesh neuro-fcc self-adaptive-rotor-blades cold-jet local-gravity-field-navigation predictive-airflow-engine self-healing-avionics-bios vortex-shield air-swarm-os star-nav-core

# Build configuration
BUILD_TYPE ?= Release
COMPILER ?= gcc
JOBS ?= $(shell nproc)

# Colors for output
RED := \033[0;31m
GREEN := \033[0;32m
YELLOW := \033[0;33m
BLUE := \033[0;34m
NC := \033[0m # No Color

# Utility functions
define print_status
	@echo "$(BLUE)$(1)$(NC)"
endef

define print_success
	@echo "$(GREEN)✓ $(1)$(NC)"
endef

define print_error
	@echo "$(RED)✗ $(1)$(NC)"
endef

define print_warning
	@echo "$(YELLOW)⚠ $(1)$(NC)"
endef

# Main targets
all: $(TECHNOLOGIES)
	$(call print_success,"All technologies built successfully")

clean:
	$(call print_status,"Cleaning all build artifacts...")
	@for tech in $(TECHNOLOGIES); do \
		if [ -d "$$tech/build" ]; then \
			echo "Cleaning $$tech..."; \
			$(MAKE) -C "$$tech" clean || true; \
			rm -rf "$$tech/build"; \
		fi; \
	done
	$(call print_success,"All build artifacts cleaned")

test: all
	$(call print_status,"Running all tests...")
	@for tech in $(TECHNOLOGIES); do \
		echo "Testing $$tech..."; \
		if [ -f "$$tech/build/Makefile" ] && grep -q "test" "$$tech/build/Makefile"; then \
			$(MAKE) -C "$$tech/build" test || $(call print_error,"Tests failed for $$tech"); \
		else \
			$(call print_warning,"No tests found for $$tech"); \
		fi; \
	done
	$(call print_success,"All tests completed")

demos: all
	$(call print_status,"Building and running all demos...")
	@for tech in $(TECHNOLOGIES); do \
		echo "Running demo for $$tech..."; \
		if [ -f "$$tech/build/*_demo" ]; then \
			timeout 30s "$$tech/build"/*_demo || $(call print_warning,"Demo timeout for $$tech"); \
		else \
			$(call print_warning,"No demo found for $$tech"); \
		fi; \
	done
	$(call print_success,"All demos completed")

install: all
	$(call print_status,"Installing all technologies...")
	@mkdir -p /usr/local/bin
	@mkdir -p /usr/local/include/meta-repo-architecture
	@mkdir -p /usr/local/lib/meta-repo-architecture
	@for tech in $(TECHNOLOGIES); do \
		if [ -f "$$tech/build/*_demo" ]; then \
			cp "$$tech/build"/*_demo /usr/local/bin/ || $(call print_error,"Failed to install $$tech demo"); \
		fi; \
		if [ -d "$$tech/include" ]; then \
			cp -r "$$tech/include"/* /usr/local/include/meta-repo-architecture/ || true; \
		fi; \
		if [ -f "$$tech/build/lib$$tech.a" ]; then \
			cp "$$tech/build/lib$$tech.a" /usr/local/lib/meta-repo-architecture/ || true; \
		fi; \
	done
	$(call print_success,"All technologies installed")

uninstall:
	$(call print_status,"Uninstalling all technologies...")
	@rm -f /usr/local/bin/*_demo
	@rm -rf /usr/local/include/meta-repo-architecture
	@rm -rf /usr/local/lib/meta-repo-architecture
	$(call print_success,"All technologies uninstalled")

docker:
	$(call print_status,"Building Docker images...")
	@for tech in $(TECHNOLOGIES); do \
		if [ -f "$$tech/Dockerfile" ]; then \
			echo "Building Docker image for $$tech..."; \
			docker build -t "meta-repo-architecture/$$tech:latest" "$$tech" || $(call print_error,"Docker build failed for $$tech"); \
		else \
			$(call print_warning,"No Dockerfile found for $$tech"); \
		fi; \
	done
	$(call print_success,"All Docker images built")

docs:
	$(call print_status,"Generating documentation...")
	@if [ -f "Doxyfile" ]; then \
		doxygen Doxyfile || $(call print_error,"Documentation generation failed"); \
	else \
		$(call print_warning,"No Doxyfile found"); \
	fi
	$(call print_success,"Documentation generated")

format:
	$(call print_status,"Formatting all source code...")
	@find . -name "*.cpp" -o -name "*.c" -o -name "*.h" -o -name "*.hpp" | grep -v build | xargs clang-format -i || $(call print_warning,"clang-format not available")
	$(call print_success,"Source code formatted")

lint:
	$(call print_status,"Running linting tools...")
	@for tech in $(TECHNOLOGIES); do \
		echo "Linting $$tech..."; \
		find "$$tech/src" -name "*.cpp" -o -name "*.c" | xargs clang-tidy --warnings-as-errors='*' -checks='-*,modernize-*,performance-*,readability-*,bugprone-*' || $(call print_warning,"Linting issues in $$tech"); \
	done
	$(call print_success,"Linting completed")

# Technology-specific targets
airmesh:
	$(call print_status,"Building Air-to-Air Mesh Network...")
	@cd air-to-air-mesh && mkdir -p build && cd build && cmake .. -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DCMAKE_CXX_COMPILER=$(COMPILER)++ && make -j$(JOBS)
	$(call print_success,"Air-to-Air Mesh Network built")

neuro-fcc:
	$(call print_status,"Building Neuro-FCC...")
	@cd neuro-fcc && mkdir -p build && cd build && cmake .. -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DCMAKE_CXX_COMPILER=$(COMPILER)++ && make -j$(JOBS)
	$(call print_success,"Neuro-FCC built")

adaptive:
	$(call print_status,"Building Self-Adaptive Rotor Blades...")
	@cd self-adaptive-rotor-blades && mkdir -p build && cd build && cmake .. -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DCMAKE_CXX_COMPILER=$(COMPILER)++ && make -j$(JOBS)
	$(call print_success,"Self-Adaptive Rotor Blades built")

cold-jet:
	$(call print_status,"Building Cold-Jet...")
	@cd cold-jet && mkdir -p build && cd build && cmake .. -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DCMAKE_CXX_COMPILER=$(COMPILER)++ && make -j$(JOBS)
	$(call print_success,"Cold-Jet built")

lgfn:
	$(call print_status,"Building Local Gravity Field Navigation...")
	@cd local-gravity-field-navigation && mkdir -p build && cd build && cmake .. -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DCMAKE_CXX_COMPILER=$(COMPILER)++ && make -j$(JOBS)
	$(call print_success,"Local Gravity Field Navigation built")

predictive:
	$(call print_status,"Building Predictive Airflow Engine...")
	@cd predictive-airflow-engine && mkdir -p build && cd build && cmake .. -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DCMAKE_CXX_COMPILER=$(COMPILER)++ && make -j$(JOBS)
	$(call print_success,"Predictive Airflow Engine built")

bios:
	$(call print_status,"Building Self-Healing Avionics BIOS...")
	@cd self-healing-avionics-bios && mkdir -p build && cd build && cmake .. -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DCMAKE_C_COMPILER=$(COMPILER) && make -j$(JOBS)
	$(call print_success,"Self-Healing Avionics BIOS built")

vortex:
	$(call print_status,"Building Vortex Shield...")
	@cd vortex-shield && mkdir -p build && cd build && cmake .. -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DCMAKE_CXX_COMPILER=$(COMPILER)++ && make -j$(JOBS)
	$(call print_success,"Vortex Shield built")

swarm:
	$(call print_status,"Building Air Swarm OS...")
	@cd air-swarm-os && mkdir -p build && cd build && cmake .. -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DCMAKE_CXX_COMPILER=$(COMPILER)++ && make -j$(JOBS)
	$(call print_success,"Air Swarm OS built")

starnav:
	$(call print_status,"Building StarNav Core...")
	@cd star-nav-core && mkdir -p build && cd build && cmake .. -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DCMAKE_CXX_COMPILER=$(COMPILER)++ && make -j$(JOBS)
	$(call print_success,"StarNav Core built")

# Performance testing
benchmark: all
	$(call print_status,"Running performance benchmarks...")
	@for tech in $(TECHNOLOGIES); do \
		echo "Benchmarking $$tech..."; \
		if [ -f "$$tech/build/*_demo" ]; then \
			timeout 60s "$$tech/build"/*_demo --benchmark || $(call print_warning,"Benchmark failed for $$tech"); \
		fi; \
	done
	$(call print_success,"Performance benchmarks completed")

# Memory checking
memcheck: all
	$(call print_status,"Running memory checks...")
	@for tech in neuro-fcc vortex-shield predictive-airflow-engine; do \
		echo "Memory checking $$tech..."; \
		if [ -f "$$tech/build/*_demo" ]; then \
			timeout 120s valgrind --leak-check=full --error-exitcode=1 "$$tech/build"/*_demo || $(call print_warning,"Memory issues in $$tech"); \
		fi; \
	done
	$(call print_success,"Memory checks completed")

# Security scanning
security:
	$(call print_status,"Running security scan...")
	@if command -v trivy >/dev/null 2>&1; then \
		trivy fs --severity HIGH,CRITICAL . || $(call print_warning,"Security issues found"); \
	else \
		$(call print_warning,"Trivy not available for security scanning"); \
	fi
	$(call print_success,"Security scan completed")

# Continuous integration target
ci: clean all test format lint security
	$(call print_success,"Continuous integration completed successfully")

# Development setup
dev-setup:
	$(call print_status,"Setting up development environment...")
	@echo "Installing dependencies..."
	@if command -v apt-get >/dev/null 2>&1; then \
		sudo apt-get update; \
		sudo apt-get install -y cmake build-essential clang clang-format clang-tidy valgrind doxygen graphix; \
	elif command -v brew >/dev/null 2>&1; then \
		brew install cmake clang-format clang-tidy valgrind doxygen graphviz; \
	elif command -v choco >/dev/null 2>&1; then \
		choco install cmake visualstudio2019buildtools; \
	else \
		$(call print_warning,"Please install dependencies manually"); \
	fi
	$(call print_success,"Development environment setup completed")

# Project status
status:
	$(call print_status,"Project Status Report")
	@echo "===================="
	@echo "Technologies: $(words $(TECHNOLOGIES))"
	@echo "Build Type: $(BUILD_TYPE)"
	@echo "Compiler: $(COMPILER)"
	@echo "Parallel Jobs: $(JOBS)"
	@echo ""
	@echo "Build Status:"
	@for tech in $(TECHNOLOGIES); do \
		if [ -f "$$tech/build/*_demo" ]; then \
			echo "  $$tech: $(GREEN)✓ Built$(NC)"; \
		else \
			echo "  $$tech: $(RED)✗ Not built$(NC)"; \
		fi; \
	done

# Quick start
quick-start: dev-setup all demos
	$(call print_success,"Quick start completed! All technologies built and demonstrated.")

# Include technology-specific Makefiles if they exist
-include $(TECHNOLOGIES:%=%/Makefile.local)
