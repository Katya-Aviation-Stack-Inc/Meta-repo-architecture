#ifndef AI_OPTIMIZATION_ENGINE_H
#define AI_OPTIMIZATION_ENGINE_H

#include <vector>
#include <memory>
#include <functional>
#include <chrono>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace revolutionary_aviation {

    // AI optimization algorithms
    enum class OptimizationAlgorithm {
        GENETIC_ALGORITHM,        // Evolutionary optimization
        PARTICLE_SWARM,          // Swarm intelligence
        NEURAL_EVOLUTION,        // Neuroevolution
        REINFORCEMENT_LEARNING,  // RL-based optimization
        BAYESIAN_OPTIMIZATION,   // Bayesian optimization
        SIMULATED_ANNEALING,     // Simulated annealing
        GRADIENT_DESCENT,        // Gradient-based optimization
        ENSEMBLE_METHOD          // Ensemble of multiple algorithms
    };

    // Optimization objective types
    enum class OptimizationObjective {
        MINIMIZE_LATENCY,         // Minimize system latency
        MAXIMIZE_THROUGHPUT,      // Maximize data throughput
        MAXIMIZE_RELIABILITY,     // Maximize system reliability
        MINIMIZE_POWER_CONSUMPTION, // Minimize power usage
        MAXIMIZE_SECURITY,        // Maximize security level
        MINIMIZE_ERROR_RATE,      // Minimize error rates
        BALANCE_PERFORMANCE,      // Balance multiple objectives
        CUSTOM_OBJECTIVE          // User-defined objective
    };

    // Optimization parameters
    struct OptimizationParameters {
        OptimizationAlgorithm algorithm = OptimizationAlgorithm::ENSEMBLE_METHOD;
        OptimizationObjective primary_objective = OptimizationObjective::BALANCE_PERFORMANCE;
        
        // Algorithm-specific parameters
        size_t population_size = 100;
        size_t max_generations = 1000;
        double mutation_rate = 0.1;
        double crossover_rate = 0.8;
        double learning_rate = 0.01;
        double exploration_rate = 0.3;
        
        // Convergence criteria
        double convergence_threshold = 1e-6;
        size_t max_iterations_without_improvement = 100;
        std::chrono::seconds max_optimization_time{300}; // 5 minutes
        
        // Multi-objective weights
        double latency_weight = 0.25;
        double throughput_weight = 0.25;
        double reliability_weight = 0.25;
        double security_weight = 0.25;
        
        // Constraints
        double max_latency_ms = 10.0;
        double min_throughput_mbps = 100.0;
        double min_reliability = 0.99;
        double max_power_watts = 1000.0;
    };

    // Optimization solution
    struct OptimizationSolution {
        std::vector<double> parameters;
        double fitness_score;
        std::vector<double> objective_values;
        std::chrono::milliseconds computation_time;
        size_t generation_found;
        bool is_feasible;
        std::string algorithm_used;
    };

    // Real-time optimization context
    struct OptimizationContext {
        std::vector<double> current_system_state;
        std::vector<double> sensor_readings;
        std::vector<std::string> active_constraints;
        std::chrono::steady_clock::time_point timestamp;
        double system_load;
        std::vector<std::string> optimization_goals;
    };

    // AI Optimization Engine
    class AIOptimizationEngine {
    public:
        AIOptimizationEngine();
        ~AIOptimizationEngine();

        // Initialization and configuration
        bool initialize(const OptimizationParameters& params = OptimizationParameters{});
        bool shutdown();
        bool is_initialized() const;

        // Optimization control
        bool start_real_time_optimization();
        bool stop_real_time_optimization();
        bool is_optimization_active() const;

        // Single optimization run
        OptimizationSolution optimize_system(const OptimizationContext& context);
        OptimizationSolution optimize_with_constraints(
            const std::vector<double>& initial_guess,
            const std::vector<std::function<bool(const std::vector<double>&)>>& constraints);

        // Multi-objective optimization
        std::vector<OptimizationSolution> pareto_optimize(const OptimizationContext& context);
        std::vector<OptimizationSolution> get_pareto_frontier() const;

        // Continuous learning
        bool enable_online_learning();
        bool disable_online_learning();
        bool update_model_with_feedback(const OptimizationSolution& solution, double actual_performance);
        bool save_optimization_history(const std::string& filename) const;
        bool load_optimization_history(const std::string& filename);

        // Performance monitoring
        double get_optimization_performance() const;
        std::vector<double> get_convergence_history() const;
        size_t get_total_optimizations_performed() const;
        double get_average_optimization_time() const;

        // Algorithm management
        bool set_primary_algorithm(OptimizationAlgorithm algorithm);
        bool enable_ensemble_optimization();
        bool add_custom_algorithm(const std::string& name, 
                                 std::function<OptimizationSolution(const OptimizationContext&)> algorithm);

        // Advanced features
        bool enable_predictive_optimization();
        bool enable_adaptive_parameters();
        bool enable_distributed_optimization();
        bool enable_quantum_inspired_optimization();

    private:
        struct OptimizationImpl;
        std::unique_ptr<OptimizationImpl> impl_;
        OptimizationParameters params_;
        std::atomic<bool> initialized_;
        std::atomic<bool> optimization_active_;

        // Threading
        std::thread optimization_thread_;
        mutable std::mutex optimization_mutex_;
        std::condition_variable optimization_cv_;

        // Internal methods
        void optimization_loop();
        OptimizationSolution run_genetic_algorithm(const OptimizationContext& context);
        OptimizationSolution run_particle_swarm(const OptimizationContext& context);
        OptimizationSolution run_neural_evolution(const OptimizationContext& context);
        OptimizationSolution run_reinforcement_learning(const OptimizationContext& context);
        OptimizationSolution run_ensemble_method(const OptimizationContext& context);
    };

    // Neural Network Optimizer
    class NeuralNetworkOptimizer {
    public:
        NeuralNetworkOptimizer();
        ~NeuralNetworkOptimizer();

        // Network architecture optimization
        bool optimize_network_architecture(size_t max_layers, size_t max_neurons_per_layer);
        std::vector<size_t> get_optimal_architecture() const;

        // Hyperparameter optimization
        bool optimize_hyperparameters(const std::vector<std::string>& hyperparameter_names);
        std::vector<double> get_optimal_hyperparameters() const;

        // Training optimization
        bool optimize_training_strategy();
        double get_optimal_learning_rate() const;
        size_t get_optimal_batch_size() const;

        // Real-time adaptation
        bool enable_real_time_adaptation();
        bool adapt_to_new_data(const std::vector<double>& new_data);

    private:
        struct NeuralOptimizerImpl;
        std::unique_ptr<NeuralOptimizerImpl> neural_impl_;
    };

    // Swarm Intelligence Optimizer
    class SwarmIntelligenceOptimizer {
    public:
        SwarmIntelligenceOptimizer();
        ~SwarmIntelligenceOptimizer();

        // Particle swarm optimization
        bool initialize_particle_swarm(size_t swarm_size, size_t dimension);
        std::vector<double> optimize_with_pso(const std::function<double(const std::vector<double>&)>& objective);

        // Ant colony optimization
        bool initialize_ant_colony(size_t colony_size);
        std::vector<int> optimize_with_aco(const std::function<double(const std::vector<int>&)>& objective);

        // Bee colony optimization
        bool initialize_bee_colony(size_t colony_size);
        std::vector<double> optimize_with_bco(const std::function<double(const std::vector<double>&)>& objective);

        // Multi-swarm coordination
        bool enable_multi_swarm_coordination();
        bool coordinate_multiple_swarms(const std::vector<std::string>& swarm_objectives);

    private:
        struct SwarmOptimizerImpl;
        std::unique_ptr<SwarmOptimizerImpl> swarm_impl_;
    };

    // Predictive Optimization Engine
    class PredictiveOptimizationEngine {
    public:
        PredictiveOptimizationEngine();
        ~PredictiveOptimizationEngine();

        // Future state prediction
        bool train_prediction_models(const std::vector<OptimizationContext>& historical_data);
        std::vector<OptimizationContext> predict_future_states(double time_horizon_seconds);

        // Proactive optimization
        OptimizationSolution optimize_for_predicted_state(const OptimizationContext& predicted_context);
        bool enable_proactive_optimization();

        // Anomaly detection
        bool detect_optimization_anomalies(const OptimizationContext& current_context);
        std::vector<std::string> get_anomaly_descriptions() const;

        // Performance forecasting
        std::vector<double> forecast_system_performance(size_t forecast_steps);
        double get_confidence_level() const;

    private:
        struct PredictiveOptimizerImpl;
        std::unique_ptr<PredictiveOptimizerImpl> predictive_impl_;
    };

    // Distributed Optimization Coordinator
    class DistributedOptimizationCoordinator {
    public:
        DistributedOptimizationCoordinator();
        ~DistributedOptimizationCoordinator();

        // Node management
        bool add_optimization_node(const std::string& node_id, const std::string& address);
        bool remove_optimization_node(const std::string& node_id);
        std::vector<std::string> get_active_nodes() const;

        // Distributed optimization
        bool start_distributed_optimization(const OptimizationContext& context);
        OptimizationSolution get_distributed_solution();
        bool is_distributed_optimization_complete() const;

        // Load balancing
        bool enable_load_balancing();
        bool redistribute_optimization_load();

        // Result aggregation
        bool aggregate_partial_solutions(const std::vector<OptimizationSolution>& partial_solutions);
        OptimizationSolution get_consensus_solution() const;

    private:
        struct DistributedCoordinatorImpl;
        std::unique_ptr<DistributedCoordinatorImpl> distributed_impl_;
    };

} // namespace revolutionary_aviation

#endif // AI_OPTIMIZATION_ENGINE_H
