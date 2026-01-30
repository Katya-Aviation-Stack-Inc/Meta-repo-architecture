#!/usr/bin/env python3
"""
Meta-repo-architecture Benchmark Runner
This script runs comprehensive benchmarks for all 10 aviation technologies
"""

import os
import sys
import time
import json
import logging
import subprocess
import threading
import statistics
import psutil
from pathlib import Path
from typing import Dict, List, Tuple, Optional
from dataclasses import dataclass, asdict
import concurrent.futures
import requests

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)

@dataclass
class BenchmarkResult:
    """Benchmark result for a technology"""
    name: str
    test_name: str
    iterations: int
    total_time: float
    avg_time: float
    min_time: float
    max_time: float
    p95_time: float
    p99_time: float
    std_dev: float
    throughput: float
    success_rate: float
    memory_usage_mb: float
    cpu_usage_percent: float

class BenchmarkRunner:
    """Benchmark runner for aviation technologies"""
    
    def __init__(self):
        self.project_root = Path(__file__).parent.parent
        self.build_dir = self.project_root / "build"
        self.results_dir = self.project_root / "benchmark-results"
        self.results_dir.mkdir(exist_ok=True)
        
        # Benchmark configurations
        self.benchmarks = {
            "air-to-air-mesh": {
                "executable": "airmesh_demo",
                "port": 8081,
                "tests": [
                    {"name": "network_throughput", "iterations": 1000, "duration": 60},
                    {"name": "message_latency", "iterations": 10000, "duration": 30},
                    {"name": "concurrent_connections", "iterations": 100, "duration": 45}
                ]
            },
            "neuro-fcc": {
                "executable": "neuro_fcc_demo",
                "port": 8082,
                "tests": [
                    {"name": "control_loop", "iterations": 10000, "duration": 30},
                    {"name": "neural_inference", "iterations": 5000, "duration": 20},
                    {"name": "sensor_fusion", "iterations": 2000, "duration": 25}
                ]
            },
            "self-adaptive-rotor-blades": {
                "executable": "adaptive_rotor_demo",
                "port": 8083,
                "tests": [
                    {"name": "blade_adjustment", "iterations": 10000, "duration": 30},
                    {"name": "vibration_analysis", "iterations": 5000, "duration": 20},
                    {"name": "performance_optimization", "iterations": 1000, "duration": 15}
                ]
            },
            "cold-jet": {
                "executable": "cold_jet_demo",
                "port": 8084,
                "tests": [
                    {"name": "thrust_control", "iterations": 1000, "duration": 60},
                    {"name": "temperature_regulation", "iterations": 2000, "duration": 45},
                    {"name": "cryogenic_system", "iterations": 500, "duration": 30}
                ]
            },
            "local-gravity-field-navigation": {
                "executable": "lgfn_demo",
                "port": 8085,
                "tests": [
                    {"name": "gravity_measurement", "iterations": 1000, "duration": 45},
                    {"name": "position_calculation", "iterations": 2000, "duration": 30},
                    {"name": "sensor_processing", "iterations": 5000, "duration": 25}
                ]
            },
            "predictive-airflow-engine": {
                "executable": "predictive_airflow_demo",
                "port": 8086,
                "tests": [
                    {"name": "cfd_prediction", "iterations": 500, "duration": 120},
                    {"name": "airflow_simulation", "iterations": 200, "duration": 90},
                    {"name": "neural_processing", "iterations": 1000, "duration": 60}
                ]
            },
            "self-healing-avionics-bios": {
                "executable": "self_healing_bios_demo",
                "port": 8087,
                "tests": [
                    {"name": "memory_check", "iterations": 10000, "duration": 30},
                    {"name": "system_recovery", "iterations": 100, "duration": 60},
                    {"name": "health_monitoring", "iterations": 5000, "duration": 20}
                ]
            },
            "vortex-shield": {
                "executable": "vortex_shield_demo",
                "port": 8088,
                "tests": [
                    {"name": "vortex_detection", "iterations": 5000, "duration": 30},
                    {"name": "flow_analysis", "iterations": 2000, "duration": 25},
                    {"name": "recovery_procedures", "iterations": 1000, "duration": 20}
                ]
            },
            "air-swarm-os": {
                "executable": "air_swarm_os_demo",
                "port": 8089,
                "tests": [
                    {"name": "consensus_algorithm", "iterations": 1000, "duration": 60},
                    {"name": "blockchain_processing", "iterations": 500, "duration": 45},
                    {"name": "fleet_communication", "iterations": 2000, "duration": 30}
                ]
            },
            "star-nav-core": {
                "executable": "starnav_demo",
                "port": 8090,
                "tests": [
                    {"name": "star_detection", "iterations": 1000, "duration": 45},
                    {"name": "position_calculation", "iterations": 2000, "duration": 30},
                    {"name": "attitude_estimation", "iterations": 1500, "duration": 35}
                ]
            }
        }
    
    def check_executable_exists(self, tech_key: str) -> bool:
        """Check if the executable exists"""
        config = self.benchmarks[tech_key]
        executable_path = self.build_dir / config["executable"]
        if sys.platform == "win32":
            executable_path = executable_path.with_suffix(".exe")
        
        return executable_path.exists()
    
    def start_service(self, tech_key: str) -> Optional[subprocess.Popen]:
        """Start a technology service for benchmarking"""
        config = self.benchmarks[tech_key]
        
        if not self.check_executable_exists(tech_key):
            logger.error(f"Executable not found for {tech_key}")
            return None
        
        try:
            executable_path = self.build_dir / config["executable"]
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
            
            # Wait for service to start
            time.sleep(5)
            
            # Check if process is still running
            if process.poll() is None:
                logger.info(f"Started {tech_key} service (PID: {process.pid})")
                return process
            else:
                stdout, stderr = process.communicate()
                logger.error(f"Failed to start {tech_key} service: {stderr}")
                return None
                
        except Exception as e:
            logger.error(f"Failed to start {tech_key} service: {e}")
            return None
    
    def stop_service(self, process: subprocess.Popen):
        """Stop a service"""
        try:
            process.terminate()
            try:
                process.wait(timeout=10)
            except subprocess.TimeoutExpired:
                process.kill()
                process.wait()
        except Exception as e:
            logger.error(f"Failed to stop service: {e}")
    
    def run_single_benchmark(self, tech_key: str, test_config: Dict, process: subprocess.Popen) -> BenchmarkResult:
        """Run a single benchmark test"""
        test_name = test_config["name"]
        iterations = test_config["iterations"]
        duration = test_config["duration"]
        
        logger.info(f"Running {test_name} benchmark for {tech_key}...")
        
        # Monitor system resources
        process_monitor = psutil.Process(process.pid)
        
        # Collect timing data
        times = []
        start_time = time.time()
        success_count = 0
        
        # Run benchmark
        for i in range(iterations):
            if time.time() - start_time > duration:
                break
            
            iteration_start = time.time()
            
            try:
                # Send benchmark request
                response = requests.post(
                    f"http://localhost:{self.benchmarks[tech_key]['port']}/benchmark",
                    json={"test": test_name, "iteration": i},
                    timeout=10
                )
                
                if response.status_code == 200:
                    success_count += 1
                
            except Exception as e:
                logger.warning(f"Benchmark request failed: {e}")
            
            iteration_end = time.time()
            iteration_time = iteration_end - iteration_start
            times.append(iteration_time)
        
        # Calculate statistics
        if times:
            avg_time = statistics.mean(times)
            min_time = min(times)
            max_time = max(times)
            std_dev = statistics.stdev(times) if len(times) > 1 else 0
            
            # Calculate percentiles
            sorted_times = sorted(times)
            p95_index = int(len(sorted_times) * 0.95)
            p99_index = int(len(sorted_times) * 0.99)
            p95_time = sorted_times[min(p95_index, len(sorted_times) - 1)]
            p99_time = sorted_times[min(p99_index, len(sorted_times) - 1)]
        else:
            avg_time = min_time = max_time = p95_time = p99_time = std_dev = 0
        
        # Calculate throughput
        total_time = time.time() - start_time
        throughput = success_count / total_time if total_time > 0 else 0
        success_rate = (success_count / iterations) * 100 if iterations > 0 else 0
        
        # Get resource usage
        try:
            memory_info = process_monitor.memory_info()
            memory_usage_mb = memory_info.rss / 1024 / 1024
            cpu_usage = process_monitor.cpu_percent()
        except:
            memory_usage_mb = 0
            cpu_usage = 0
        
        return BenchmarkResult(
            name=tech_key,
            test_name=test_name,
            iterations=len(times),
            total_time=total_time,
            avg_time=avg_time,
            min_time=min_time,
            max_time=max_time,
            p95_time=p95_time,
            p99_time=p99_time,
            std_dev=std_dev,
            throughput=throughput,
            success_rate=success_rate,
            memory_usage_mb=memory_usage_mb,
            cpu_usage_percent=cpu_usage
        )
    
    def run_technology_benchmarks(self, tech_key: str) -> List[BenchmarkResult]:
        """Run all benchmarks for a technology"""
        results = []
        
        # Start service
        process = self.start_service(tech_key)
        if not process:
            return results
        
        try:
            # Wait for service to be fully ready
            time.sleep(10)
            
            # Run each benchmark test
            for test_config in self.benchmarks[tech_key]["tests"]:
                result = self.run_single_benchmark(tech_key, test_config, process)
                results.append(result)
                
                # Small delay between tests
                time.sleep(2)
        
        finally:
            # Stop service
            self.stop_service(process)
        
        return results
    
    def run_all_benchmarks(self) -> Dict[str, List[BenchmarkResult]]:
        """Run benchmarks for all technologies"""
        logger.info("Starting comprehensive benchmark suite...")
        
        all_results = {}
        
        for tech_key in self.benchmarks.keys():
            logger.info(f"Running benchmarks for {tech_key}...")
            
            try:
                results = self.run_technology_benchmarks(tech_key)
                all_results[tech_key] = results
                
                # Log summary
                for result in results:
                    logger.info(f"  {result.test_name}: {result.avg_time*1000:.2f}ms avg, {result.throughput:.2f} ops/s")
                
            except Exception as e:
                logger.error(f"Failed to run benchmarks for {tech_key}: {e}")
                all_results[tech_key] = []
        
        return all_results
    
    def generate_benchmark_report(self, results: Dict[str, List[BenchmarkResult]]) -> str:
        """Generate comprehensive benchmark report"""
        logger.info("Generating benchmark report...")
        
        report = []
        report.append("# Benchmark Report")
        report.append(f"Generated: {time.strftime('%Y-%m-%d %H:%M:%S')}")
        report.append("")
        
        # Executive summary
        report.append("## Executive Summary")
        total_tests = sum(len(tests) for tests in results.values())
        total_iterations = sum(sum(r.iterations for r in tests) for tests in results.values())
        
        report.append(f"- **Total Technologies**: {len(results)}")
        report.append(f"- **Total Benchmark Tests**: {total_tests}")
        report.append(f"- **Total Iterations**: {total_iterations:,}")
        report.append("")
        
        # Performance summary
        all_results_flat = []
        for tech_results in results.values():
            all_results_flat.extend(tech_results)
        
        if all_results_flat:
            avg_response_time = statistics.mean([r.avg_time for r in all_results_flat])
            avg_throughput = statistics.mean([r.throughput for r in all_results_flat])
            avg_success_rate = statistics.mean([r.success_rate for r in all_results_flat])
            
            report.append(f"- **Average Response Time**: {avg_response_time*1000:.2f}ms")
            report.append(f"- **Average Throughput**: {avg_throughput:.2f} ops/s")
            report.append(f"- **Average Success Rate**: {avg_success_rate:.2f}%")
            report.append("")
        
        # Technology results
        report.append("## Technology Benchmark Results")
        report.append("")
        
        for tech_key, tech_results in results.items():
            if not tech_results:
                continue
            
            report.append(f"### {tech_key.replace('-', ' ').title()}")
            report.append("")
            
            # Summary table
            report.append("| Test | Iterations | Avg Time (ms) | Min Time (ms) | Max Time (ms) | P95 (ms) | P99 (ms) | Throughput (ops/s) | Success Rate |")
            report.append("|------|------------|---------------|---------------|---------------|---------|---------|-------------------|--------------|")
            
            for result in tech_results:
                report.append(f"| {result.test_name} | {result.iterations:,} | "
                            f"{result.avg_time*1000:.2f} | {result.min_time*1000:.2f} | "
                            f"{result.max_time*1000:.2f} | {result.p95_time*1000:.2f} | "
                            f"{result.p99_time*1000:.2f} | {result.throughput:.2f} | "
                            f"{result.success_rate:.2f}% |")
            
            report.append("")
            
            # Performance analysis
            avg_time = statistics.mean([r.avg_time for r in tech_results])
            avg_throughput = statistics.mean([r.throughput for r in tech_results])
            avg_memory = statistics.mean([r.memory_usage_mb for r in tech_results])
            avg_cpu = statistics.mean([r.cpu_usage_percent for r in tech_results])
            
            report.append("**Performance Summary:**")
            report.append(f"- Average Response Time: {avg_time*1000:.2f}ms")
            report.append(f"- Average Throughput: {avg_throughput:.2f} ops/s")
            report.append(f"- Average Memory Usage: {avg_memory:.1f}MB")
            report.append(f"- Average CPU Usage: {avg_cpu:.1f}%")
            report.append("")
        
        # Performance comparison
        report.append("## Performance Comparison")
        report.append("")
        
        # Create performance ranking
        tech_performance = {}
        for tech_key, tech_results in results.items():
            if tech_results:
                # Calculate overall performance score
                avg_time = statistics.mean([r.avg_time for r in tech_results])
                avg_throughput = statistics.mean([r.throughput for r in tech_results])
                success_rate = statistics.mean([r.success_rate for r in tech_results])
                
                # Simple scoring: lower time + higher throughput + higher success rate
                score = (100 / (avg_time * 1000 + 1)) + avg_throughput + success_rate
                tech_performance[tech_key] = score
        
        # Sort by performance
        sorted_techs = sorted(tech_performance.items(), key=lambda x: x[1], reverse=True)
        
        report.append("### Performance Ranking")
        for i, (tech_key, score) in enumerate(sorted_techs, 1):
            tech_name = tech_key.replace('-', ' ').title()
            report.append(f"{i}. **{tech_name}**: {score:.1f}")
        report.append("")
        
        # Recommendations
        report.append("## Performance Recommendations")
        report.append("")
        
        # Identify best performers
        if sorted_techs:
            best_performer = sorted_techs[0][0]
            worst_performer = sorted_techs[-1][0]
            
            report.append(f"**Best Performer**: {best_performer.replace('-', ' ').title()}")
            report.append(f"**Needs Improvement**: {worst_performer.replace('-', ' ').title()}")
            report.append("")
        
        # General recommendations
        report.append("### Optimization Recommendations")
        report.append("- **Response Time**: Focus on reducing P99 response times for better user experience")
        report.append("- **Throughput**: Implement connection pooling and request batching")
        report.append("- **Memory Usage**: Consider memory pooling and reduce allocations")
        report.append("- **CPU Usage**: Optimize algorithms and implement better caching")
        report.append("- **Success Rate**: Improve error handling and retry mechanisms")
        report.append("")
        
        return "\n".join(report)
    
    def save_results(self, results: Dict[str, List[BenchmarkResult]]):
        """Save benchmark results to JSON file"""
        json_results = {
            "timestamp": time.time(),
            "technologies": {}
        }
        
        for tech_key, tech_results in results.items():
            json_results["technologies"][tech_key] = [asdict(result) for result in tech_results]
        
        results_file = self.results_dir / "benchmark-results.json"
        with open(results_file, 'w') as f:
            json.dump(json_results, f, indent=2)
        
        logger.info(f"Benchmark results saved to {results_file}")
    
    def run_benchmarks(self):
        """Run complete benchmark suite"""
        logger.info("Starting benchmark suite...")
        
        # Run all benchmarks
        results = self.run_all_benchmarks()
        
        # Generate report
        report = self.generate_benchmark_report(results)
        
        # Save report
        report_file = self.results_dir / "benchmark-report.md"
        with open(report_file, 'w') as f:
            f.write(report)
        
        # Save results
        self.save_results(results)
        
        logger.info(f"Benchmark suite completed. Results saved to {self.results_dir}")
        
        return report, results

def main():
    """Main function"""
    runner = BenchmarkRunner()
    
    try:
        report, results = runner.run_benchmarks()
        
        # Print summary
        print("\n" + "="*60)
        print("BENCHMARK SUITE SUMMARY")
        print("="*60)
        print(report.split("## Technology Benchmark Results")[0])
        print("="*60)
        print(f"Full report saved to: {runner.results_dir / 'benchmark-report.md'}")
        print(f"Results saved to: {runner.results_dir / 'benchmark-results.json'}")
        print("="*60)
        
    except Exception as e:
        logger.error(f"Benchmark suite failed: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()
