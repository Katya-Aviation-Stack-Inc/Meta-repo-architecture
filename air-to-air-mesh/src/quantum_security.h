#ifndef QUANTUM_SECURITY_H
#define QUANTUM_SECURITY_H

#include <string>
#include <vector>
#include <memory>
#include <cstdint>

namespace revolutionary_aviation {

    // Quantum-resistant encryption algorithms
    enum class QuantumAlgorithm {
        CRYSTALS_KYBER,    // Lattice-based KEM
        CRYSTALS_DILITHIUM, // Lattice-based signature
        FALCON,            // Lattice-based signature
        SPHINCS_PLUS,      // Hash-based signature
        NTRU,              // NTRU encryption
        BLISS              // Lattice-based signature
    };

    // Quantum security configuration
    struct QuantumSecurityConfig {
        QuantumAlgorithm primary_algorithm = QuantumAlgorithm::CRYSTALS_KYBER;
        QuantumAlgorithm backup_algorithm = QuantumAlgorithm::NTRU;
        size_t key_size_bits = 256;
        size_t signature_size_bits = 256;
        bool enable_post_quantum_hybrid = true;
        bool enable_quantum_key_distribution = false;
        bool enable_quantum_random_number_generation = true;
    };

    // Quantum-resistant key pair
    struct QuantumKeyPair {
        std::vector<uint8_t> public_key;
        std::vector<uint8_t> private_key;
        QuantumAlgorithm algorithm;
        size_t key_size;
    };

    // Quantum security manager
    class QuantumSecurityManager {
    public:
        QuantumSecurityManager();
        ~QuantumSecurityManager();

        // Initialization and configuration
        bool initialize(const QuantumSecurityConfig& config = QuantumSecurityConfig{});
        bool shutdown();
        bool is_initialized() const;

        // Key management
        QuantumKeyPair generate_key_pair(QuantumAlgorithm algorithm = QuantumAlgorithm::CRYSTALS_KYBER);
        bool import_public_key(const std::vector<uint8_t>& key, QuantumAlgorithm algorithm);
        bool import_private_key(const std::vector<uint8_t>& key, QuantumAlgorithm algorithm);
        bool export_public_key(std::vector<uint8_t>& key) const;
        bool export_private_key(std::vector<uint8_t>& key) const;

        // Encryption and decryption
        std::vector<uint8_t> encrypt(const std::vector<uint8_t>& plaintext, 
                                   const std::vector<uint8_t>& public_key);
        std::vector<uint8_t> decrypt(const std::vector<uint8_t>& ciphertext, 
                                   const std::vector<uint8_t>& private_key);

        // Digital signatures
        std::vector<uint8_t> sign(const std::vector<uint8_t>& message, 
                                 const std::vector<uint8_t>& private_key);
        bool verify(const std::vector<uint8_t>& message, 
                   const std::vector<uint8_t>& signature, 
                   const std::vector<uint8_t>& public_key);

        // Key exchange
        std::vector<uint8_t> generate_key_exchange_request();
        std::vector<uint8_t> process_key_exchange_response(const std::vector<uint8_t>& response);
        std::vector<uint8_t> complete_key_exchange(const std::vector<uint8_t>& request);

        // Quantum random number generation
        std::vector<uint8_t> generate_quantum_random_bytes(size_t num_bytes);
        uint64_t generate_quantum_random_uint64();
        double generate_quantum_random_double();

        // Security analysis
        double calculate_security_strength() const;
        bool is_quantum_resistant() const;
        std::vector<std::string> get_security_recommendations() const;

        // Performance metrics
        double get_encryption_throughput() const;
        double get_decryption_throughput() const;
        double get_signature_generation_time() const;
        double get_signature_verification_time() const;

    private:
        struct QuantumSecurityImpl;
        std::unique_ptr<QuantumSecurityImpl> impl_;
        QuantumSecurityConfig config_;
        bool initialized_;

        // Internal methods
        bool validate_algorithm_support(QuantumAlgorithm algorithm) const;
        size_t get_key_size(QuantumAlgorithm algorithm) const;
        size_t get_signature_size(QuantumAlgorithm algorithm) const;
    };

    // Quantum-resistant communication channel
    class QuantumSecureChannel {
    public:
        QuantumSecureChannel(QuantumSecurityManager& security_manager);
        ~QuantumSecureChannel();

        // Channel establishment
        bool establish_outbound_channel(const std::vector<uint8_t>& peer_public_key);
        bool establish_inbound_channel(const std::vector<uint8_t>& local_private_key);
        bool complete_channel_establishment(const std::vector<uint8_t>& peer_response);

        // Secure communication
        bool send_message(const std::vector<uint8_t>& message);
        bool receive_message(std::vector<uint8_t>& message);
        
        // Channel status
        bool is_established() const;
        bool is_secure() const;
        double get_channel_security_level() const;

        // Channel management
        bool renegotiate_keys();
        bool close_channel();

    private:
        struct ChannelImpl;
        std::unique_ptr<ChannelImpl> channel_impl_;
        QuantumSecurityManager& security_manager_;
        bool established_;
    };

    // Quantum key distribution (QKD) simulation
    class QuantumKeyDistribution {
    public:
        QuantumKeyDistribution();
        ~QuantumKeyDistribution();

        // QKD protocol simulation
        bool initialize_bb84_protocol();
        bool initialize_e91_protocol();
        bool initialize_b92_protocol();

        // Key generation
        std::vector<uint8_t> generate_shared_key(size_t key_size_bits);
        bool detect_eavesdropping();
        double get_quantum_bit_error_rate() const;

        // QKD status
        bool is_key_available() const;
        size_t get_available_key_bits() const;
        bool refresh_key();

    private:
        struct QKDImpl;
        std::unique_ptr<QKDImpl> qkd_impl_;
        bool initialized_;
    };

    // Quantum security audit and monitoring
    class QuantumSecurityAuditor {
    public:
        QuantumSecurityAuditor(QuantumSecurityManager& security_manager);
        ~QuantumSecurityAuditor();

        // Security audit
        bool perform_full_security_audit();
        bool check_algorithm_strength();
        bool check_key_management_security();
        bool check_implementation_vulnerabilities();

        // Monitoring
        bool start_continuous_monitoring();
        bool stop_continuous_monitoring();
        std::vector<std::string> get_security_alerts() const;
        std::vector<std::string> get_security_recommendations() const;

        // Compliance
        bool check_nist_post_quantum_compliance();
        bool check_iso_27001_compliance();
        bool check_fips_140_2_compliance();

    private:
        struct AuditorImpl;
        std::unique_ptr<AuditorImpl> auditor_impl_;
        QuantumSecurityManager& security_manager_;
        bool monitoring_active_;
    };

} // namespace revolutionary_aviation

#endif // QUANTUM_SECURITY_H
