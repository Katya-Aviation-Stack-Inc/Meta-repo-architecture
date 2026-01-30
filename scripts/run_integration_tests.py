#!/usr/bin/env python3
"""
Meta-repo-architecture Integration Test Runner
This script runs comprehensive integration tests for all 10 aviation technologies
"""

import os
import sys
import time
import json
import logging
import subprocess
import threading
import signal
from typing import Dict, List, Tuple, Optional
from dataclasses import dataclass
from pathlib import Path
import requests
import psutil

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('integration-test-results.log'),
        logging.StreamHandler()
    ]
)
logger = logging.getLogger(__name__)

@dataclass
class TechnologyConfig:
    """Configuration for a technology"""
    name: str
    executable: str
    port: int
    health_endpoint: str
    startup_time: int
    dependencies: List[str]
    critical: bool = False

class IntegrationTestRunner:
    """Main integration test runner"""
    
    def __init__(self):
        self.project_root = Path(__file__).parent.parent
        self.build_dir = self.project_root / "build"
        self.processes: Dict[str, subprocess.Popen] = {}
        self.test_results: Dict[str, Dict] = {}
        self.running = True
        
        # Technology configurations
        self.technologies = {
            "air-to-air-mesh": TechnologyConfig(
                name="Air-to-Air Mesh Network",
                executable="airmesh_demo",
                port=8081,
                health_endpoint="/health",
                startup_time=10,
                dependencies=[],
                critical=False
            ),
            "neuro-fcc": TechnologyConfig(
                name="Neuro-FCC",
                executable="neuro_fcc_demo",
                port=8082,
                health_endpoint="/health",
                startup_time=5,
                dependencies=["air-to-air-mesh"],
                critical=True
            ),
            "self-adaptive-rotor-blades": TechnologyConfig(
                name="Self-Adaptive Rotor Blades",
                executable="adaptive_rotor_demo",
                port=8083,
                health_endpoint="/health",
                startup_time=8,
                dependencies=["neuro-fcc"],
                critical=False
            ),
            "cold-jet": TechnologyConfig(
                name="Cold-Jet",
                executable="cold_jet_demo",
                port=8084,
                health_endpoint="/health",
                startup_time=15,
                dependencies=["self-adaptive-rotor-blades"],
                critical=False
            ),
            "local-gravity-field-navigation": TechnologyConfig(
                name="Local Gravity Field Navigation",
                executable="lgfn_demo",
                port=8085,
                health_endpoint="/health",
                startup_time=12,
                dependencies=["cold-jet"],
                critical=False
            ),
            "predictive-airflow-engine": TechnologyConfig(
                name="Predictive Airflow Engine",
                executable="predictive_airflow_demo",
                port=8086,
                health_endpoint="/health",
                startup_time=20,
                dependencies=["local-gravity-field-navigation"],
                critical=False
            ),
            "self-healing-avionics-bios": TechnologyConfig(
                name="Self-Healing Avionics BIOS",
                executable="self_healing_bios_demo",
                port=8087,
                health_endpoint="/health",
                startup_time=3,
                dependencies=[],
                critical=True
            ),
            "vortex-shield": TechnologyConfig(
                name="Vortex Shield",
                executable="vortex_shield_demo",
                port=8088,
                health_endpoint="/health",
                startup_time=7,
                dependencies=["neuro-fcc"],
                critical=True
            ),
            "air-swarm-os": TechnologyConfig(
                name="Air Swarm OS",
                executable="air_swarm_os_demo",
                port=8089,
                health_endpoint="/health",
                startup_time=25,
                dependencies=["predictive-airflow-engine", "self-healing-avionics-bios"],
                critical=False
            ),
            "star-nav-core": TechnologyConfig(
                name="StarNav Core",
                executable="starnav_demo",
                port=8090,
                health_endpoint="/health",
                startup_time=30,
                dependencies=["air-swarm-os"],
                critical=False
            )
        }
        
        # Setup signal handlers
        signal.signal(signal.SIGINT, self._signal_handler)
        signal.signal(signal.SIGTERM, self._signal_handler)
    
    def _signal_handler(self, signum, frame):
        """Handle shutdown signals"""
        logger.info(f"Received signal {signum}, shutting down...")
        self.running = False
        self.stop_all_services()
    
    def check_executable_exists(self, tech_config: TechnologyConfig) -> bool:
        """Check if the executable exists"""
        executable_path = self.build_dir / tech_config.executable
        if sys.platform == "win32":
            executable_path = executable_path.with_suffix(".exe")
        
        return executable_path.exists()
    
    def start_service(self, tech_key: str) -> bool:
        """Start a technology service"""
        tech_config = self.technologies[tech_key]
        
        if not self.check_executable_exists(tech_config):
            logger.error(f"Executable not found for {tech_config.name}")
            return False
        
        try:
            executable_path = self.build_dir / tech_config.executable
            if sys.platform == "win32":
                executable_path = executable_path.with_suffix(".exe")
            
            # Start the process
            process = subprocess.Popen(
                [str(executable_path)],
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True,
                cwd=str(self.build_dir)
            )
            
            self.processes[tech_key] = process
            
            logger.info(f"Started {tech_config.name} (PID: {process.pid})")
            
            # Wait for startup
            time.sleep(tech_config.startup_time)
            
            # Check if process is still running
            if process.poll() is None:
                return True
            else:
                stdout, stderr = process.communicate()
                logger.error(f"{tech_config.name} failed to start: {stderr}")
                return False
                
        except Exception as e:
            logger.error(f"Failed to start {tech_config.name}: {e}")
            return False
    
    def stop_service(self, tech_key: str) -> bool:
        """Stop a technology service"""
        if tech_key not in self.processes:
            return True
        
        process = self.processes[tech_key]
        tech_config = self.technologies[tech_key]
        
        try:
            # Try graceful shutdown first
            process.terminate()
            
            # Wait for graceful shutdown
            try:
                process.wait(timeout=10)
            except subprocess.TimeoutExpired:
                # Force kill if graceful shutdown fails
                process.kill()
                process.wait()
            
            logger.info(f"Stopped {tech_config.name}")
            del self.processes[tech_key]
            return True
            
        except Exception as e:
            logger.error(f"Failed to stop {tech_config.name}: {e}")
            return False
    
    def stop_all_services(self):
        """Stop all running services"""
        logger.info("Stopping all services...")
        
        # Stop in reverse dependency order
        stop_order = [
            "star-nav-core",
            "air-swarm-os",
            "predictive-airflow-engine",
            "local-gravity-field-navigation",
            "cold-jet",
            "self-adaptive-rotor-blades",
            "vortex-shield",
            "neuro-fcc",
            "air-to-air-mesh",
            "self-healing-avionics-bios"
        ]
        
        for tech_key in stop_order:
            if tech_key in self.processes:
                self.stop_service(tech_key)
    
    def check_service_health(self, tech_key: str) -> Tuple[bool, str]:
        """Check if a service is healthy"""
        tech_config = self.technologies[tech_key]
        
        try:
            response = requests.get(
                f"http://localhost:{tech_config.port}{tech_config.health_endpoint}",
                timeout=5
            )
            
            if response.status_code == 200:
                return True, response.text
            else:
                return False, f"HTTP {response.status_code}"
                
        except requests.exceptions.RequestException as e:
            return False, str(e)
    
    def test_service_communication(self, tech1: str, tech2: str) -> bool:
        """Test communication between two services"""
        try:
            # Test basic connectivity
            tech1_config = self.technologies[tech1]
            tech2_config = self.technologies[tech2]
            
            # Send test request from tech1 to tech2
            response = requests.post(
                f"http://localhost:{tech1_config.port}/test_communication",
                json={"target": f"http://localhost:{tech2_config.port}"},
                timeout=10
            )
            
            return response.status_code == 200
            
        except Exception as e:
            logger.error(f"Communication test failed between {tech1} and {tech2}: {e}")
            return False
    
    def run_performance_test(self, tech_key: str) -> Dict:
        """Run performance test for a technology"""
        tech_config = self.technologies[tech_key]
        
        try:
            # Measure response time
            start_time = time.time()
            response = requests.get(
                f"http://localhost:{tech_config.port}/performance_test",
                timeout=30
            )
            end_time = time.time()
            
            if response.status_code == 200:
                performance_data = response.json()
                performance_data["measured_response_time"] = end_time - start_time
                return performance_data
            else:
                return {"error": f"HTTP {response.status_code}"}
                
        except Exception as e:
            return {"error": str(e)}
    
    def run_stress_test(self, tech_key: str) -> Dict:
        """Run stress test for a technology"""
        tech_config = self.technologies[tech_key]
        
        try:
            # Send multiple concurrent requests
            import concurrent.futures
            
            def send_request():
                try:
                    response = requests.get(
                        f"http://localhost:{tech_config.port}/stress_test",
                        timeout=5
                    )
                    return response.status_code == 200
                except:
                    return False
            
            # Run 50 concurrent requests
            with concurrent.futures.ThreadPoolExecutor(max_workers=10) as executor:
                futures = [executor.submit(send_request) for _ in range(50)]
                results = [future.result() for future in concurrent.futures.as_completed(futures)]
            
            success_rate = sum(results) / len(results) * 100
            
            return {
                "total_requests": len(results),
                "successful_requests": sum(results),
                "success_rate": success_rate,
                "passed": success_rate >= 95  # 95% success rate threshold
            }
            
        except Exception as e:
            return {"error": str(e)}
    
    def run_integration_tests(self):
        """Run all integration tests"""
        logger.info("Starting integration tests...")
        
        # Test results structure
        results = {
            "timestamp": time.time(),
            "total_tests": 0,
            "passed_tests": 0,
            "failed_tests": 0,
            "technologies": {}
        }
        
        # Start services in dependency order
        start_order = [
            "air-to-air-mesh",
            "self-healing-avionics-bios",
            "neuro-fcc",
            "self-adaptive-rotor-blades",
            "vortex-shield",
            "cold-jet",
            "local-gravity-field-navigation",
            "predictive-airflow-engine",
            "air-swarm-os",
            "star-nav-core"
        ]
        
        # Start all services
        started_services = []
        for tech_key in start_order:
            if self.start_service(tech_key):
                started_services.append(tech_key)
                logger.info(f"Successfully started {self.technologies[tech_key].name}")
            else:
                logger.error(f"Failed to start {self.technologies[tech_key].name}")
                if self.technologies[tech_key].critical:
                    logger.error("Critical service failed, stopping tests")
                    self.stop_all_services()
                    return results
        
        # Wait for all services to be ready
        logger.info("Waiting for services to be ready...")
        time.sleep(30)
        
        # Run tests for each technology
        for tech_key in started_services:
            if not self.running:
                break
                
            tech_config = self.technologies[tech_key]
            tech_results = {
                "name": tech_config.name,
                "tests": {}
            }
            
            logger.info(f"Testing {tech_config.name}...")
            
            # Health check test
            results["total_tests"] += 1
            healthy, health_message = self.check_service_health(tech_key)
            tech_results["tests"]["health_check"] = {
                "passed": healthy,
                "message": health_message
            }
            if healthy:
                results["passed_tests"] += 1
            else:
                results["failed_tests"] += 1
            
            # Performance test
            if healthy:
                results["total_tests"] += 1
                perf_result = self.run_performance_test(tech_key)
                tech_results["tests"]["performance"] = perf_result
                if "error" not in perf_result:
                    results["passed_tests"] += 1
                else:
                    results["failed_tests"] += 1
            
            # Stress test
            if healthy:
                results["total_tests"] += 1
                stress_result = self.run_stress_test(tech_key)
                tech_results["tests"]["stress"] = stress_result
                if stress_result.get("passed", False):
                    results["passed_tests"] += 1
                else:
                    results["failed_tests"] += 1
            
            results["technologies"][tech_key] = tech_results
            
            # Small delay between tests
            time.sleep(2)
        
        # Test inter-service communication
        logger.info("Testing inter-service communication...")
        communication_pairs = [
            ("neuro-fcc", "self-adaptive-rotor-blades"),
            ("neuro-fcc", "vortex-shield"),
            ("air-swarm-os", "star-nav-core"),
            ("predictive-airflow-engine", "neuro-fcc")
        ]
        
        for tech1, tech2 in communication_pairs:
            if not self.running:
                break
                
            if tech1 in started_services and tech2 in started_services:
                results["total_tests"] += 1
                comm_result = self.test_service_communication(tech1, tech2)
                
                if "communication_tests" not in results:
                    results["communication_tests"] = {}
                
                results["communication_tests"][f"{tech1}_to_{tech2}"] = {
                    "passed": comm_result
                }
                
                if comm_result:
                    results["passed_tests"] += 1
                else:
                    results["failed_tests"] += 1
        
        # Stop all services
        self.stop_all_services()
        
        # Calculate success rate
        if results["total_tests"] > 0:
            results["success_rate"] = (results["passed_tests"] / results["total_tests"]) * 100
        else:
            results["success_rate"] = 0
        
        # Overall result
        results["overall_passed"] = results["success_rate"] >= 90
        
        logger.info(f"Integration tests completed: {results['passed_tests']}/{results['total_tests']} passed ({results['success_rate']:.1f}%)")
        
        return results
    
    def save_results(self, results: Dict):
        """Save test results to file"""
        results_file = self.project_root / "integration-test-results.json"
        
        with open(results_file, 'w') as f:
            json.dump(results, f, indent=2)
        
        logger.info(f"Results saved to {results_file}")
    
    def generate_report(self, results: Dict) -> str:
        """Generate a human-readable test report"""
        report = []
        report.append("# Integration Test Report")
        report.append(f"Generated: {time.strftime('%Y-%m-%d %H:%M:%S')}")
        report.append("")
        
        # Summary
        report.append("## Summary")
        report.append(f"- Total Tests: {results['total_tests']}")
        report.append(f"- Passed: {results['passed_tests']}")
        report.append(f"- Failed: {results['failed_tests']}")
        report.append(f"- Success Rate: {results['success_rate']:.1f}%")
        report.append(f"- Overall Result: {'PASSED' if results['overall_passed'] else 'FAILED'}")
        report.append("")
        
        # Technology results
        report.append("## Technology Results")
        for tech_key, tech_results in results['technologies'].items():
            report.append(f"### {tech_results['name']}")
            
            for test_name, test_result in tech_results['tests'].items():
                status = "✅ PASSED" if test_result.get('passed', False) else "❌ FAILED"
                report.append(f"- {test_name.replace('_', ' ').title()}: {status}")
                
                if not test_result.get('passed', False) and 'message' in test_result:
                    report.append(f"  - Message: {test_result['message']}")
            
            report.append("")
        
        # Communication tests
        if 'communication_tests' in results:
            report.append("## Communication Tests")
            for comm_test, comm_result in results['communication_tests'].items():
                status = "✅ PASSED" if comm_result.get('passed', False) else "❌ FAILED"
                report.append(f"- {comm_test.replace('_', ' -> ').title()}: {status}")
            report.append("")
        
        return "\n".join(report)
    
    def save_report(self, report: str):
        """Save test report to file"""
        report_file = self.project_root / "integration-test-report.md"
        
        with open(report_file, 'w') as f:
            f.write(report)
        
        logger.info(f"Report saved to {report_file}")

def main():
    """Main function"""
    runner = IntegrationTestRunner()
    
    try:
        # Run integration tests
        results = runner.run_integration_tests()
        
        # Save results
        runner.save_results(results)
        
        # Generate and save report
        report = runner.generate_report(results)
        runner.save_report(report)
        
        # Print summary
        print("\n" + "="*50)
        print("INTEGRATION TEST SUMMARY")
        print("="*50)
        print(f"Total Tests: {results['total_tests']}")
        print(f"Passed: {results['passed_tests']}")
        print(f"Failed: {results['failed_tests']}")
        print(f"Success Rate: {results['success_rate']:.1f}%")
        print(f"Overall Result: {'PASSED' if results['overall_passed'] else 'FAILED'}")
        print("="*50)
        
        # Exit with appropriate code
        sys.exit(0 if results['overall_passed'] else 1)
        
    except KeyboardInterrupt:
        logger.info("Tests interrupted by user")
        runner.stop_all_services()
        sys.exit(1)
    except Exception as e:
        logger.error(f"Unexpected error: {e}")
        runner.stop_all_services()
        sys.exit(1)

if __name__ == "__main__":
    main()
