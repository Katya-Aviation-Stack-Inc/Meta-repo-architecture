#!/usr/bin/env python3
"""
Meta-repo-architecture Performance Analysis Tool
This script analyzes performance data from all 10 aviation technologies
"""

import os
import sys
import json
import time
import logging
import statistics
import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path
from typing import Dict, List, Tuple, Optional
from dataclasses import dataclass, asdict
import pandas as pd
import seaborn as sns

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)

@dataclass
class PerformanceMetrics:
    """Performance metrics for a technology"""
    name: str
    latency_ms: float
    throughput_hz: float
    accuracy_percent: float
    memory_mb: float
    power_w: float
    cpu_usage_percent: float
    response_time_p95_ms: float
    response_time_p99_ms: float
    error_rate_percent: float

class PerformanceAnalyzer:
    """Performance analysis tool"""
    
    def __init__(self):
        self.project_root = Path(__file__).parent.parent
        self.results_dir = self.project_root / "performance-results"
        self.results_dir.mkdir(exist_ok=True)
        
        # Technology performance targets
        self.performance_targets = {
            "air-to-air-mesh": {
                "latency_ms": 50,
                "throughput_mbps": 100,
                "accuracy_percent": 95,
                "memory_mb": 20,
                "power_w": 15
            },
            "neuro-fcc": {
                "latency_ms": 2,
                "throughput_hz": 500,
                "accuracy_percent": 99.9,
                "memory_mb": 5,
                "power_w": 10
            },
            "self-adaptive-rotor-blades": {
                "latency_ms": 1,
                "throughput_hz": 1000,
                "accuracy_percent": 95,
                "memory_mb": 10,
                "power_w": 8
            },
            "cold-jet": {
                "latency_ms": 100,
                "throughput_hz": 10,
                "accuracy_percent": 90,
                "memory_mb": 15,
                "power_w": 25
            },
            "local-gravity-field-navigation": {
                "latency_ms": 200,
                "throughput_hz": 5,
                "accuracy_percent": 92,
                "memory_mb": 25,
                "power_w": 20
            },
            "predictive-airflow-engine": {
                "latency_ms": 50,
                "throughput_hz": 20,
                "accuracy_percent": 95,
                "memory_mb": 15,
                "power_w": 45
            },
            "self-healing-avionics-bios": {
                "latency_ms": 1000,
                "throughput_hz": 1,
                "accuracy_percent": 99.9,
                "memory_mb": 3,
                "power_w": 5
            },
            "vortex-shield": {
                "latency_ms": 10,
                "throughput_hz": 100,
                "accuracy_percent": 99.5,
                "memory_mb": 8,
                "power_w": 8
            },
            "air-swarm-os": {
                "latency_ms": 100,
                "throughput_tps": 100,
                "accuracy_percent": 95,
                "memory_mb": 35,
                "power_w": 35
            },
            "star-nav-core": {
                "latency_ms": 500,
                "throughput_hz": 2,
                "accuracy_percent": 90,
                "memory_mb": 30,
                "power_w": 30
            }
        }
        
        # Set up matplotlib for non-interactive use
        plt.switch_backend('Agg')
        
        # Set style
        sns.set_style("whitegrid")
        plt.style.use('seaborn-v0_8')
    
    def collect_performance_data(self) -> Dict[str, PerformanceMetrics]:
        """Collect performance data from all technologies"""
        logger.info("Collecting performance data...")
        
        performance_data = {}
        
        # Simulated performance data (in real implementation, this would come from actual tests)
        simulated_data = {
            "air-to-air-mesh": PerformanceMetrics(
                name="Air-to-Air Mesh Network",
                latency_ms=42.3,
                throughput_hz=105.2,
                accuracy_percent=96.1,
                memory_mb=18.7,
                power_w=14.2,
                cpu_usage_percent=45.3,
                response_time_p95_ms=58.7,
                response_time_p99_ms=72.1,
                error_rate_percent=0.2
            ),
            "neuro-fcc": PerformanceMetrics(
                name="Neuro-FCC",
                latency_ms=1.8,
                throughput_hz=512.4,
                accuracy_percent=99.92,
                memory_mb=4.8,
                power_w=9.7,
                cpu_usage_percent=67.2,
                response_time_p95_ms=2.3,
                response_time_p99_ms=2.8,
                error_rate_percent=0.05
            ),
            "self-adaptive-rotor-blades": PerformanceMetrics(
                name="Self-Adaptive Rotor Blades",
                latency_ms=0.9,
                throughput_hz=1047.8,
                accuracy_percent=96.3,
                memory_mb=9.2,
                power_w=7.8,
                cpu_usage_percent=52.1,
                response_time_p95_ms=1.2,
                response_time_p99_ms=1.5,
                error_rate_percent=0.1
            ),
            "cold-jet": PerformanceMetrics(
                name="Cold-Jet",
                latency_ms=87.4,
                throughput_hz=11.2,
                accuracy_percent=91.8,
                memory_mb=14.3,
                power_w=23.9,
                cpu_usage_percent=38.7,
                response_time_p95_ms=95.2,
                response_time_p99_ms=108.7,
                error_rate_percent=0.3
            ),
            "local-gravity-field-navigation": PerformanceMetrics(
                name="Local Gravity Field Navigation",
                latency_ms=178.9,
                throughput_hz=5.3,
                accuracy_percent=92.7,
                memory_mb=23.8,
                power_w=19.4,
                cpu_usage_percent=41.2,
                response_time_p95_ms=195.3,
                response_time_p99_ms=212.8,
                error_rate_percent=0.4
            ),
            "predictive-airflow-engine": PerformanceMetrics(
                name="Predictive Airflow Engine",
                latency_ms=43.7,
                throughput_hz=21.8,
                accuracy_percent=95.6,
                memory_mb=14.7,
                power_w=44.2,
                cpu_usage_percent=78.9,
                response_time_p95_ms=51.2,
                response_time_p99_ms=58.9,
                error_rate_percent=0.2
            ),
            "self-healing-avionics-bios": PerformanceMetrics(
                name="Self-Healing Avionics BIOS",
                latency_ms=892.3,
                throughput_hz=1.1,
                accuracy_percent=99.94,
                memory_mb=2.9,
                power_w=4.8,
                cpu_usage_percent=12.4,
                response_time_p95_ms=950.7,
                response_time_p99_ms=1023.4,
                error_rate_percent=0.02
            ),
            "vortex-shield": PerformanceMetrics(
                name="Vortex Shield",
                latency_ms=8.7,
                throughput_hz=104.3,
                accuracy_percent=99.6,
                memory_mb=7.9,
                power_w=7.9,
                cpu_usage_percent=55.8,
                response_time_p95_ms=11.2,
                response_time_p99_ms=13.8,
                error_rate_percent=0.08
            ),
            "air-swarm-os": PerformanceMetrics(
                name="Air Swarm OS",
                latency_ms=92.4,
                throughput_hz=108.7,
                accuracy_percent=95.8,
                memory_mb=34.2,
                power_w=34.7,
                cpu_usage_percent=62.3,
                response_time_p95_ms=105.8,
                response_time_p99_ms=118.9,
                error_rate_percent=0.3
            ),
            "star-nav-core": PerformanceMetrics(
                name="StarNav Core",
                latency_ms=467.8,
                throughput_hz=2.1,
                accuracy_percent=90.9,
                memory_mb=29.4,
                power_w=29.2,
                cpu_usage_percent=48.7,
                response_time_p95_ms=512.3,
                response_time_p99_ms=547.8,
                error_rate_percent=0.5
            )
        }
        
        return simulated_data
    
    def analyze_performance(self, metrics: PerformanceMetrics, targets: Dict) -> Dict:
        """Analyze performance against targets"""
        analysis = {
            "meets_targets": True,
            "performance_score": 0,
            "issues": [],
            "improvements": []
        }
        
        # Check each metric against targets
        score = 0
        total_checks = 0
        
        # Latency check
        if "latency_ms" in targets:
            total_checks += 1
            if metrics.latency_ms <= targets["latency_ms"]:
                score += 1
            else:
                analysis["issues"].append(f"Latency {metrics.latency_ms:.1f}ms exceeds target {targets['latency_ms']}ms")
                analysis["improvements"].append("Optimize algorithms and reduce processing overhead")
        
        # Throughput check
        throughput_key = "throughput_hz" if "throughput_hz" in targets else "throughput_tps"
        if throughput_key in targets:
            total_checks += 1
            actual_throughput = getattr(metrics, throughput_key.replace("_", "_"))
            if actual_throughput >= targets[throughput_key]:
                score += 1
            else:
                analysis["issues"].append(f"Throughput {actual_throughput:.1f} below target {targets[throughput_key]}")
                analysis["improvements"].append("Increase parallelization and optimize data structures")
        
        # Accuracy check
        if "accuracy_percent" in targets:
            total_checks += 1
            if metrics.accuracy_percent >= targets["accuracy_percent"]:
                score += 1
            else:
                analysis["issues"].append(f"Accuracy {metrics.accuracy_percent:.1f}% below target {targets['accuracy_percent']}%")
                analysis["improvements"].append("Improve algorithms and enhance sensor data quality")
        
        # Memory check
        if "memory_mb" in targets:
            total_checks += 1
            if metrics.memory_mb <= targets["memory_mb"]:
                score += 1
            else:
                analysis["issues"].append(f"Memory {metrics.memory_mb:.1f}MB exceeds target {targets['memory_mb']}MB")
                analysis["improvements"].append("Optimize memory usage and implement memory pooling")
        
        # Power check
        if "power_w" in targets:
            total_checks += 1
            if metrics.power_w <= targets["power_w"]:
                score += 1
            else:
                analysis["issues"].append(f"Power {metrics.power_w:.1f}W exceeds target {targets['power_w']}W")
                analysis["improvements"].append("Implement power management and optimize processing")
        
        # Calculate performance score
        if total_checks > 0:
            analysis["performance_score"] = (score / total_checks) * 100
            analysis["meets_targets"] = score == total_checks
        
        return analysis
    
    def generate_performance_charts(self, performance_data: Dict[str, PerformanceMetrics]):
        """Generate performance visualization charts"""
        logger.info("Generating performance charts...")
        
        # Prepare data for plotting
        tech_names = list(performance_data.keys())
        
        # Create figure with subplots
        fig, axes = plt.subplots(2, 3, figsize=(18, 12))
        fig.suptitle('Meta-repo-architecture Performance Analysis', fontsize=16, fontweight='bold')
        
        # Latency chart
        latencies = [performance_data[tech].latency_ms for tech in tech_names]
        axes[0, 0].bar(tech_names, latencies, color='skyblue')
        axes[0, 0].set_title('Latency (ms)')
        axes[0, 0].set_ylabel('Latency (ms)')
        axes[0, 0].tick_params(axis='x', rotation=45)
        
        # Throughput chart
        throughputs = [performance_data[tech].throughput_hz for tech in tech_names]
        axes[0, 1].bar(tech_names, throughputs, color='lightgreen')
        axes[0, 1].set_title('Throughput (Hz)')
        axes[0, 1].set_ylabel('Throughput (Hz)')
        axes[0, 1].tick_params(axis='x', rotation=45)
        
        # Accuracy chart
        accuracies = [performance_data[tech].accuracy_percent for tech in tech_names]
        axes[0, 2].bar(tech_names, accuracies, color='orange')
        axes[0, 2].set_title('Accuracy (%)')
        axes[0, 2].set_ylabel('Accuracy (%)')
        axes[0, 2].tick_params(axis='x', rotation=45)
        
        # Memory usage chart
        memories = [performance_data[tech].memory_mb for tech in tech_names]
        axes[1, 0].bar(tech_names, memories, color='red')
        axes[1, 0].set_title('Memory Usage (MB)')
        axes[1, 0].set_ylabel('Memory (MB)')
        axes[1, 0].tick_params(axis='x', rotation=45)
        
        # Power consumption chart
        powers = [performance_data[tech].power_w for tech in tech_names]
        axes[1, 1].bar(tech_names, powers, color='purple')
        axes[1, 1].set_title('Power Consumption (W)')
        axes[1, 1].set_ylabel('Power (W)')
        axes[1, 1].tick_params(axis='x', rotation=45)
        
        # CPU usage chart
        cpu_usages = [performance_data[tech].cpu_usage_percent for tech in tech_names]
        axes[1, 2].bar(tech_names, cpu_usages, color='brown')
        axes[1, 2].set_title('CPU Usage (%)')
        axes[1, 2].set_ylabel('CPU Usage (%)')
        axes[1, 2].tick_params(axis='x', rotation=45)
        
        plt.tight_layout()
        
        # Save chart
        chart_file = self.results_dir / "performance-charts.png"
        plt.savefig(chart_file, dpi=300, bbox_inches='tight')
        plt.close()
        
        logger.info(f"Performance charts saved to {chart_file}")
    
    def generate_comparison_chart(self, performance_data: Dict[str, PerformanceMetrics]):
        """Generate radar chart for technology comparison"""
        logger.info("Generating comparison chart...")
        
        # Prepare data for radar chart
        categories = ['Latency', 'Throughput', 'Accuracy', 'Memory', 'Power', 'CPU']
        
        # Normalize metrics to 0-100 scale
        def normalize_metric(value, metric_type):
            if metric_type == 'Latency':
                return max(0, 100 - (value / 1000) * 100)  # Lower is better
            elif metric_type == 'Throughput':
                return min(100, (value / 1000) * 100)  # Higher is better
            elif metric_type == 'Accuracy':
                return value  # Already 0-100
            elif metric_type == 'Memory':
                return max(0, 100 - (value / 100) * 100)  # Lower is better
            elif metric_type == 'Power':
                return max(0, 100 - (value / 100) * 100)  # Lower is better
            elif metric_type == 'CPU':
                return max(0, 100 - value)  # Lower is better
            return 0
        
        # Create radar chart
        fig = plt.figure(figsize=(12, 8))
        ax = fig.add_subplot(111, projection='polar')
        
        # Number of variables
        N = len(categories)
        
        # Angle for each axis
        angles = [n / float(N) * 2 * np.pi for n in range(N)]
        angles += angles[:1]  # Complete the loop
        
        # Plot each technology
        colors = plt.cm.Set3(np.linspace(0, 1, len(performance_data)))
        
        for i, (tech_key, metrics) in enumerate(performance_data.items()):
            values = [
                normalize_metric(metrics.latency_ms, 'Latency'),
                normalize_metric(metrics.throughput_hz, 'Throughput'),
                normalize_metric(metrics.accuracy_percent, 'Accuracy'),
                normalize_metric(metrics.memory_mb, 'Memory'),
                normalize_metric(metrics.power_w, 'Power'),
                normalize_metric(metrics.cpu_usage_percent, 'CPU')
            ]
            values += values[:1]  # Complete the loop
            
            ax.plot(angles, values, 'o-', linewidth=2, label=metrics.name, color=colors[i])
            ax.fill(angles, values, alpha=0.25, color=colors[i])
        
        # Add labels
        ax.set_xticks(angles[:-1])
        ax.set_xticklabels(categories)
        ax.set_ylim(0, 100)
        ax.set_title('Technology Performance Comparison', size=15, fontweight='bold', pad=20)
        ax.legend(loc='upper right', bbox_to_anchor=(1.3, 1.0))
        
        plt.tight_layout()
        
        # Save chart
        chart_file = self.results_dir / "performance-comparison.png"
        plt.savefig(chart_file, dpi=300, bbox_inches='tight')
        plt.close()
        
        logger.info(f"Comparison chart saved to {chart_file}")
    
    def generate_performance_report(self, performance_data: Dict[str, PerformanceMetrics]) -> str:
        """Generate comprehensive performance report"""
        logger.info("Generating performance report...")
        
        report = []
        report.append("# Performance Analysis Report")
        report.append(f"Generated: {time.strftime('%Y-%m-%d %H:%M:%S')}")
        report.append("")
        
        # Executive summary
        report.append("## Executive Summary")
        total_score = 0
        tech_count = len(performance_data)
        
        for tech_key, metrics in performance_data.items():
            targets = self.performance_targets.get(tech_key, {})
            analysis = self.analyze_performance(metrics, targets)
            total_score += analysis["performance_score"]
        
        avg_score = total_score / tech_count if tech_count > 0 else 0
        
        report.append(f"- **Overall Performance Score**: {avg_score:.1f}%")
        report.append(f"- **Technologies Analyzed**: {tech_count}")
        report.append(f"- **Average Latency**: {statistics.mean([m.latency_ms for m in performance_data.values()]):.1f}ms")
        report.append(f"- **Average Accuracy**: {statistics.mean([m.accuracy_percent for m in performance_data.values()]):.1f}%")
        report.append(f"- **Total Power Consumption**: {sum([m.power_w for m in performance_data.values()]):.1f}W")
        report.append(f"- **Total Memory Usage**: {sum([m.memory_mb for m in performance_data.values()]):.1f}MB")
        report.append("")
        
        # Technology details
        report.append("## Technology Performance Details")
        report.append("")
        
        for tech_key, metrics in performance_data.items():
            targets = self.performance_targets.get(tech_key, {})
            analysis = self.analyze_performance(metrics, targets)
            
            report.append(f"### {metrics.name}")
            report.append(f"**Performance Score**: {analysis['performance_score']:.1f}%")
            report.append(f"**Meets Targets**: {'✅ Yes' if analysis['meets_targets'] else '❌ No'}")
            report.append("")
            
            # Metrics table
            report.append("| Metric | Actual | Target | Status |")
            report.append("|--------|--------|--------|--------|")
            
            # Latency
            if "latency_ms" in targets:
                status = "✅" if metrics.latency_ms <= targets["latency_ms"] else "❌"
                report.append(f"| Latency | {metrics.latency_ms:.1f}ms | {targets['latency_ms']}ms | {status} |")
            
            # Throughput
            throughput_key = "throughput_hz" if "throughput_hz" in targets else "throughput_tps"
            if throughput_key in targets:
                actual_throughput = getattr(metrics, throughput_key.replace("_", "_"))
                status = "✅" if actual_throughput >= targets[throughput_key] else "❌"
                report.append(f"| Throughput | {actual_throughput:.1f} | {targets[throughput_key]} | {status} |")
            
            # Accuracy
            if "accuracy_percent" in targets:
                status = "✅" if metrics.accuracy_percent >= targets["accuracy_percent"] else "❌"
                report.append(f"| Accuracy | {metrics.accuracy_percent:.1f}% | {targets['accuracy_percent']}% | {status} |")
            
            # Memory
            if "memory_mb" in targets:
                status = "✅" if metrics.memory_mb <= targets["memory_mb"] else "❌"
                report.append(f"| Memory | {metrics.memory_mb:.1f}MB | {targets['memory_mb']}MB | {status} |")
            
            # Power
            if "power_w" in targets:
                status = "✅" if metrics.power_w <= targets["power_w"] else "❌"
                report.append(f"| Power | {metrics.power_w:.1f}W | {targets['power_w']}W | {status} |")
            
            report.append("")
            
            # Issues and improvements
            if analysis["issues"]:
                report.append("**Issues:**")
                for issue in analysis["issues"]:
                    report.append(f"- {issue}")
                report.append("")
            
            if analysis["improvements"]:
                report.append("**Recommended Improvements:**")
                for improvement in analysis["improvements"]:
                    report.append(f"- {improvement}")
                report.append("")
        
        # Performance recommendations
        report.append("## Performance Recommendations")
        report.append("")
        
        # Identify top performers
        top_performers = sorted(
            performance_data.items(),
            key=lambda x: self.analyze_performance(x[1], self.performance_targets.get(x[0], {}))["performance_score"],
            reverse=True
        )[:3]
        
        report.append("### Top Performing Technologies")
        for tech_key, metrics in top_performers:
            analysis = self.analyze_performance(metrics, self.performance_targets.get(tech_key, {}))
            report.append(f"- **{metrics.name}**: {analysis['performance_score']:.1f}%")
        report.append("")
        
        # Technologies needing improvement
        needs_improvement = [
            (tech_key, metrics) for tech_key, metrics in performance_data.items()
            if not self.analyze_performance(metrics, self.performance_targets.get(tech_key, {}))["meets_targets"]
        ]
        
        if needs_improvement:
            report.append("### Technologies Requiring Improvement")
            for tech_key, metrics in needs_improvement:
                analysis = self.analyze_performance(metrics, self.performance_targets.get(tech_key, {}))
                report.append(f"- **{metrics.name}**: {analysis['performance_score']:.1f}%")
                for issue in analysis["issues"]:
                    report.append(f"  - {issue}")
            report.append("")
        
        # System-wide recommendations
        report.append("### System-Wide Recommendations")
        report.append("- **Memory Optimization**: Consider implementing memory pooling and reducing memory allocations")
        report.append("- **Power Management**: Implement dynamic power scaling based on workload")
        report.append("- **Latency Reduction**: Optimize critical path algorithms and reduce I/O overhead")
        report.append("- **Throughput Improvement**: Increase parallelization and optimize data structures")
        report.append("- **Accuracy Enhancement**: Improve sensor fusion and algorithm precision")
        report.append("")
        
        return "\n".join(report)
    
    def save_results(self, performance_data: Dict[str, PerformanceMetrics]):
        """Save performance results to JSON file"""
        results = {
            "timestamp": time.time(),
            "technologies": {}
        }
        
        for tech_key, metrics in performance_data.items():
            results["technologies"][tech_key] = asdict(metrics)
            # Add analysis
            targets = self.performance_targets.get(tech_key, {})
            analysis = self.analyze_performance(metrics, targets)
            results["technologies"][tech_key]["analysis"] = analysis
        
        results_file = self.results_dir / "performance-results.json"
        with open(results_file, 'w') as f:
            json.dump(results, f, indent=2)
        
        logger.info(f"Performance results saved to {results_file}")
    
    def run_analysis(self):
        """Run complete performance analysis"""
        logger.info("Starting performance analysis...")
        
        # Collect performance data
        performance_data = self.collect_performance_data()
        
        # Generate charts
        self.generate_performance_charts(performance_data)
        self.generate_comparison_chart(performance_data)
        
        # Generate report
        report = self.generate_performance_report(performance_data)
        
        # Save report
        report_file = self.results_dir / "performance-report.md"
        with open(report_file, 'w') as f:
            f.write(report)
        
        # Save results
        self.save_results(performance_data)
        
        logger.info(f"Performance analysis completed. Results saved to {self.results_dir}")
        
        return report

def main():
    """Main function"""
    analyzer = PerformanceAnalyzer()
    
    try:
        report = analyzer.run_analysis()
        
        # Print summary
        print("\n" + "="*60)
        print("PERFORMANCE ANALYSIS SUMMARY")
        print("="*60)
        print(report.split("## Technology Performance Details")[0])
        print("="*60)
        print(f"Full report saved to: {analyzer.results_dir / 'performance-report.md'}")
        print(f"Charts saved to: {analyzer.results_dir}")
        print("="*60)
        
    except Exception as e:
        logger.error(f"Performance analysis failed: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()
