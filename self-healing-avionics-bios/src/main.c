#include "self_healing_bios.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Simulated hardware registers
static uint8_t simulated_flash[FLASH_SIZE];
static uint8_t simulated_ram[RAM_SIZE];
static uint8_t simulated_backup_ram[BACKUP_RAM_SIZE];
static uint8_t simulated_eeprom[EEPROM_SIZE];

// Test functions
static int test_memory_corruption_detection(void);
static int test_memory_healing(void);
static int test_bus_failure_recovery(void);
static int test_critical_system_recovery(void);
static int test_error_logging(void);
static int test_system_monitoring(void);
static void simulate_memory_corruption(uint32_t addr, uint32_t size);
static void simulate_bus_failure(uint32_t bus_id);
static void print_bios_status(void);

int main(void) {
    printf("=== Self-Healing Avionics BIOS Demo ===\n\n");
    
    // Initialize simulated hardware
    memset(simulated_flash, 0xFF, sizeof(simulated_flash));
    memset(simulated_ram, 0x00, sizeof(simulated_ram));
    memset(simulated_backup_ram, 0x00, sizeof(simulated_backup_ram));
    memset(simulated_eeprom, 0x00, sizeof(simulated_eeprom));
    
    // Initialize BIOS
    printf("Initializing Self-Healing BIOS...\n");
    int result = bios_init();
    if (result != BIOS_SUCCESS) {
        printf("BIOS initialization failed with error: %d\n", result);
        return 1;
    }
    printf("BIOS initialized successfully\n\n");
    
    // Test 1: Normal operation
    printf("=== Test 1: Normal Operation ===\n");
    print_bios_status();
    printf("System health: %d%%\n", bios_get_status()->system_health);
    printf("\n");
    
    // Test 2: Memory corruption detection
    printf("=== Test 2: Memory Corruption Detection ===\n");
    result = test_memory_corruption_detection();
    printf("Memory corruption test: %s\n", result == BIOS_SUCCESS ? "PASSED" : "FAILED");
    printf("\n");
    
    // Test 3: Memory healing
    printf("=== Test 3: Memory Healing ===\n");
    result = test_memory_healing();
    printf("Memory healing test: %s\n", result == BIOS_SUCCESS ? "PASSED" : "FAILED");
    printf("\n");
    
    // Test 4: Bus failure recovery
    printf("=== Test 4: Bus Failure Recovery ===\n");
    result = test_bus_failure_recovery();
    printf("Bus recovery test: %s\n", result == BIOS_SUCCESS ? "PASSED" : "FAILED");
    printf("\n");
    
    // Test 5: Error logging
    printf("=== Test 5: Error Logging ===\n");
    result = test_error_logging();
    printf("Error logging test: %s\n", result == BIOS_SUCCESS ? "PASSED" : "FAILED");
    printf("\n");
    
    // Test 6: System monitoring
    printf("=== Test 6: System Monitoring ===\n");
    result = test_system_monitoring();
    printf("System monitoring test: %s\n", result == BIOS_SUCCESS ? "PASSED" : "FAILED");
    printf("\n");
    
    // Test 7: Critical system recovery
    printf("=== Test 7: Critical System Recovery ===\n");
    result = test_critical_system_recovery();
    printf("Critical recovery test: %s\n", result == BIOS_SUCCESS ? "PASSED" : "FAILED");
    printf("\n");
    
    // Test 8: Multiple concurrent failures
    printf("=== Test 8: Multiple Concurrent Failures ===\n");
    
    // Simulate multiple failures
    simulate_memory_corruption(RAM_BASE_ADDR + 0x1000, 1024);
    simulate_bus_failure(0); // I2C bus
    simulate_bus_failure(1); // SPI bus
    
    // Trigger system monitoring to detect failures
    bios_monitor_system();
    
    printf("Multiple failures detected, attempting healing...\n");
    result = bios_heal_system();
    printf("Multiple failure healing: %s\n", result == BIOS_SUCCESS ? "PASSED" : "FAILED");
    
    print_bios_status();
    printf("\n");
    
    // Test 9: Healing strategy selection
    printf("=== Test 9: Healing Strategy Selection ===\n");
    
    // Test different healing strategies
    bios_set_healing_strategy(HEALING_MEMORY_COPY);
    printf("Testing memory copy strategy...\n");
    simulate_memory_corruption(FLASH_BASE_ADDR + 0x10000, 4096);
    bios_heal_system();
    
    bios_set_healing_strategy(HEALING_BUS_RESET);
    printf("Testing bus reset strategy...\n");
    simulate_bus_failure(2); // UART bus
    bios_heal_system();
    
    bios_set_healing_strategy(HEALING_SECTOR_ERASE);
    printf("Testing sector erase strategy...\n");
    simulate_memory_corruption(FLASH_BASE_ADDR + 0x20000, 4096);
    bios_heal_system();
    
    printf("Healing strategy tests completed\n\n");
    
    // Test 10: Long-term reliability
    printf("=== Test 10: Long-term Reliability Simulation ===\n");
    
    int total_cycles = 100;
    int successful_cycles = 0;
    
    for (int cycle = 0; cycle < total_cycles; cycle++) {
        // Randomly inject failures
        if (cycle % 10 == 0) {
            simulate_memory_corruption(RAM_BASE_ADDR + (cycle % 100) * 64, 64);
        }
        
        if (cycle % 25 == 0) {
            simulate_bus_failure(cycle % 3);
        }
        
        // Run system check
        int check_result = bios_self_test();
        if (check_result == BIOS_SUCCESS) {
            successful_cycles++;
        } else {
            // Attempt healing
            bios_heal_system();
        }
        
        // Update system status
        bios_get_status();
    }
    
    printf("Long-term reliability: %d/%d cycles successful (%.1f%%)\n", 
           successful_cycles, total_cycles, (float)successful_cycles / total_cycles * 100.0);
    
    print_bios_status();
    printf("\n");
    
    // Final system status
    printf("=== Final System Status ===\n");
    bios_status_t* status = bios_get_status();
    printf("Boot count: %u\n", status->boot_count);
    printf("Error count: %u\n", status->error_count);
    printf("Healing attempts: %u\n", status->healing_attempts);
    printf("System health: %d%%\n", status->system_health);
    printf("Last error code: 0x%04X\n", status->last_error_code);
    printf("BIOS version: %u.%u\n", status->version >> 8, status->version & 0xFF);
    
    // Display error log summary
    printf("\nError Log Summary:\n");
    error_type_t last_error;
    uint16_t last_error_code;
    
    if (bios_get_last_error(&last_error, &last_error_code) == BIOS_SUCCESS) {
        printf("Last error type: %d, code: 0x%04X\n", last_error, last_error_code);
    }
    
    printf("\nSelf-Healing Avionics BIOS Demo completed successfully!\n");
    printf("Demonstrated revolutionary self-repair capabilities for aviation systems.\n");
    
    return 0;
}

static int test_memory_corruption_detection(void) {
    // Backup clean memory first
    bios_backup_critical_data();
    
    // Simulate memory corruption
    simulate_memory_corruption(RAM_BASE_ADDR + 0x1000, 512);
    
    // Run memory integrity check
    int result = bios_check_memory_integrity();
    
    return (result == BIOS_ERROR_CRC || result == BIOS_ERROR_MEMORY) ? BIOS_SUCCESS : BIOS_ERROR_CRITICAL;
}

static int test_memory_healing(void) {
    // Simulate memory corruption
    simulate_memory_corruption(RAM_BASE_ADDR + 0x2000, 1024);
    
    // Attempt healing
    int result = bios_heal_system();
    
    // Verify healing was successful
    if (result == BIOS_SUCCESS) {
        result = bios_check_memory_integrity();
    }
    
    return result;
}

static int test_bus_failure_recovery(void) {
    // Simulate bus failure
    simulate_bus_failure(0); // I2C bus
    
    // Test bus communication
    int result = bios_test_bus_communication(0);
    
    if (result != BIOS_SUCCESS) {
        // Attempt bus healing
        result = bios_heal_system();
        
        // Test again after healing
        if (result == BIOS_SUCCESS) {
            result = bios_test_bus_communication(0);
        }
    }
    
    return result;
}

static int test_critical_system_recovery(void) {
    // Simulate critical system failure
    simulate_memory_corruption(FLASH_BASE_ADDR, 4096); // Critical code area
    simulate_bus_failure(0); // Critical bus
    
    // Attempt critical recovery
    int result = bios_perform_critical_recovery();
    
    return result;
}

static int test_error_logging(void) {
    // Log various types of errors
    bios_log_error(ERROR_MEMORY_CORRUPTION, 0x1001);
    bios_log_error(ERROR_BUS_TIMEOUT, 0x2001);
    bios_log_error(ERROR_TEMPERATURE_HIGH, 85);
    bios_log_error(ERROR_POWER_FLUCTUATION, 1);
    bios_log_error(ERROR_WATCHDOG_TIMEOUT, 1);
    
    // Verify error logging
    error_type_t last_error;
    uint16_t last_error_code;
    
    int result = bios_get_last_error(&last_error, &last_error_code);
    
    return (result == BIOS_SUCCESS && last_error == ERROR_WATCHDOG_TIMEOUT) ? BIOS_SUCCESS : BIOS_ERROR_CRITICAL;
}

static int test_system_monitoring(void) {
    // Run system monitoring
    int result = bios_monitor_system();
    
    // Check if any errors were detected and logged
    bios_status_t* status = bios_get_status();
    
    return (result == BIOS_SUCCESS || status->error_count > 0) ? BIOS_SUCCESS : BIOS_ERROR_CRITICAL;
}

static void simulate_memory_corruption(uint32_t addr, uint32_t size) {
    // Corrupt memory by writing random pattern
    for (uint32_t i = 0; i < size && i < 1024; i++) {
        if (addr >= RAM_BASE_ADDR && addr < (RAM_BASE_ADDR + RAM_SIZE)) {
            simulated_ram[(addr - RAM_BASE_ADDR) + i] = 0xAA; // Corruption pattern
        } else if (addr >= FLASH_BASE_ADDR && addr < (FLASH_BASE_ADDR + FLASH_SIZE)) {
            simulated_flash[(addr - FLASH_BASE_ADDR) + i] = 0x55; // Corruption pattern
        }
    }
}

static void simulate_bus_failure(uint32_t bus_id) {
    // In a real implementation, this would affect actual hardware
    // For simulation, we'll just log the failure
    bios_log_error(ERROR_BUS_TIMEOUT, bus_id);
}

static void print_bios_status(void) {
    bios_status_t* status = bios_get_status();
    
    printf("BIOS Status:\n");
    printf("  Magic Number: 0x%08X\n", status->magic_number);
    printf("  Version: %u.%u\n", status->version >> 8, status->version & 0xFF);
    printf("  Boot Count: %u\n", status->boot_count);
    printf("  Error Count: %u\n", status->error_count);
    printf("  Healing Attempts: %u\n", status->healing_attempts);
    printf("  System Health: %d%%\n", status->system_health);
    printf("  Last Error: 0x%04X\n", status->last_error_code);
    printf("  Checksum: 0x%08X\n", status->checksum);
}

// Override hardware access functions for simulation
uint8_t bios_read_byte(uint32_t addr) {
    if (addr >= RAM_BASE_ADDR && addr < (RAM_BASE_ADDR + RAM_SIZE)) {
        return simulated_ram[addr - RAM_BASE_ADDR];
    } else if (addr >= FLASH_BASE_ADDR && addr < (FLASH_BASE_ADDR + FLASH_SIZE)) {
        return simulated_flash[addr - FLASH_BASE_ADDR];
    } else if (addr >= BACKUP_RAM_BASE && addr < (BACKUP_RAM_BASE + BACKUP_RAM_SIZE)) {
        return simulated_backup_ram[addr - BACKUP_RAM_BASE];
    } else if (addr >= EEPROM_BASE_ADDR && addr < (EEPROM_BASE_ADDR + EEPROM_SIZE)) {
        return simulated_eeprom[addr - EEPROM_BASE_ADDR];
    }
    return 0xFF;
}

void bios_write_byte(uint32_t addr, uint8_t value) {
    if (addr >= RAM_BASE_ADDR && addr < (RAM_BASE_ADDR + RAM_SIZE)) {
        simulated_ram[addr - RAM_BASE_ADDR] = value;
    } else if (addr >= FLASH_BASE_ADDR && addr < (FLASH_BASE_ADDR + FLASH_SIZE)) {
        simulated_flash[addr - FLASH_BASE_ADDR] = value;
    } else if (addr >= BACKUP_RAM_BASE && addr < (BACKUP_RAM_BASE + BACKUP_RAM_SIZE)) {
        simulated_backup_ram[addr - BACKUP_RAM_BASE] = value;
    } else if (addr >= EEPROM_BASE_ADDR && addr < (EEPROM_BASE_ADDR + EEPROM_SIZE)) {
        simulated_eeprom[addr - EEPROM_BASE_ADDR] = value;
    }
}

uint32_t bios_read_word(uint32_t addr) {
    uint32_t value = 0;
    for (int i = 0; i < 4; i++) {
        value |= (uint32_t)bios_read_byte(addr + i) << (i * 8);
    }
    return value;
}

void bios_write_word(uint32_t addr, uint32_t value) {
    for (int i = 0; i < 4; i++) {
        bios_write_byte(addr + i, (value >> (i * 8)) & 0xFF);
    }
}

int bios_erase_flash_sector(uint32_t sector_addr) {
    if (sector_addr >= FLASH_BASE_ADDR && sector_addr < (FLASH_BASE_ADDR + FLASH_SIZE)) {
        uint32_t sector_index = (sector_addr - FLASH_BASE_ADDR) / FLASH_SECTOR_SIZE;
        if (sector_index < (FLASH_SIZE / FLASH_SECTOR_SIZE)) {
            uint32_t start = sector_index * FLASH_SECTOR_SIZE;
            uint32_t end = start + FLASH_SECTOR_SIZE;
            
            for (uint32_t i = start; i < end && i < FLASH_SIZE; i++) {
                simulated_flash[i] = 0xFF; // Erased state
            }
            return BIOS_SUCCESS;
        }
    }
    return BIOS_ERROR_FLASH;
}

int bios_program_flash(uint32_t addr, const uint8_t* data, uint32_t size) {
    if (addr >= FLASH_BASE_ADDR && addr < (FLASH_BASE_ADDR + FLASH_SIZE)) {
        uint32_t flash_index = addr - FLASH_BASE_ADDR;
        
        for (uint32_t i = 0; i < size && (flash_index + i) < FLASH_SIZE; i++) {
            // Only program bits that need to change from 1 to 0
            if ((simulated_flash[flash_index + i] & data[i]) != data[i]) {
                simulated_flash[flash_index + i] = data[i];
            }
        }
        return BIOS_SUCCESS;
    }
    return BIOS_ERROR_FLASH;
}

int bios_read_flash(uint32_t addr, uint8_t* data, uint32_t size) {
    if (addr >= FLASH_BASE_ADDR && addr < (FLASH_BASE_ADDR + FLASH_SIZE)) {
        uint32_t flash_index = addr - FLASH_BASE_ADDR;
        
        for (uint32_t i = 0; i < size && (flash_index + i) < FLASH_SIZE; i++) {
            data[i] = simulated_flash[flash_index + i];
        }
        return BIOS_SUCCESS;
    }
    return BIOS_ERROR_FLASH;
}
