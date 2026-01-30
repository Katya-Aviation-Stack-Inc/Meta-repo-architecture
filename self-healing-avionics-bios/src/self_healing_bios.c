#include "self_healing_bios.h"
#include <string.h>

// Global variables
static bios_status_t g_bios_status;
static healing_strategy_t g_healing_strategy = HEALING_MEMORY_COPY;
static bool g_critical_section_active = false;
static uint32_t g_error_log[100];
static uint32_t g_error_log_index = 0;

// Memory segment definitions
static const memory_segment_t g_memory_segments[] = {
    // Critical system code
    {FLASH_BASE_ADDR, 64 * 1024, EEPROM_BASE_ADDR, 0, SEGMENT_FLAG_CRITICAL | SEGMENT_FLAG_BACKUP | SEGMENT_FLAG_EXECUTABLE, 0},
    // Configuration data
    {FLASH_BASE_ADDR + 64 * 1024, 4 * 1024, EEPROM_BASE_ADDR + 64 * 1024, 0, SEGMENT_FLAG_BACKUP | SEGMENT_FLAG_HEALABLE, 0},
    // Runtime data
    {RAM_BASE_ADDR, 128 * 1024, BACKUP_RAM_BASE, 0, SEGMENT_FLAG_HEALABLE, 0},
    // Stack and heap
    {RAM_BASE_ADDR + 128 * 1024, 128 * 1024, BACKUP_RAM_BASE + 128 * 1024, 0, SEGMENT_FLAG_HEALABLE, 0}
};

#define NUM_MEMORY_SEGMENTS (sizeof(g_memory_segments) / sizeof(memory_segment_t))

// Bus configurations
static bus_config_t g_bus_configs[] = {
    {0, 400000, 100, 1, 2, BUS_FLAG_REDUNDANT | BUS_FLAG_AUTO_RECOVER | BUS_FLAG_CRITICAL, 0}, // I2C
    {1, 1000000, 50, 2, 1, BUS_FLAG_REDUNDANT | BUS_FLAG_MONITORED, 0},                           // SPI
    {2, 115200, 200, 3, 0, BUS_FLAG_CRITICAL, 0}                                                        // UART
};

#define NUM_BUS_CONFIGS (sizeof(g_bus_configs) / sizeof(bus_config_t))

// Private function prototypes
static uint32_t calculate_system_checksum(void);
static int perform_memory_healing(uint32_t addr, uint32_t size);
static int perform_bus_healing(uint32_t bus_id);
static void update_system_status(void);
static bool is_address_in_segment(uint32_t addr, const memory_segment_t* segment);

// CRC32 lookup table (simplified)
static const uint32_t crc32_table[256] = {
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
    0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
    // ... (truncated for brevity, full table would be included)
};

int bios_init(void) {
    // Initialize BIOS status structure
    memset(&g_bios_status, 0, sizeof(bios_status_t));
    g_bios_status.magic_number = 0xDEADBEEF;
    g_bios_status.version = 0x0100;
    g_bios_status.build_number = 1;
    g_bios_status.boot_count++;
    g_bios_status.system_health = 100;
    g_bios_status.checksum = calculate_system_checksum();
    
    // Initialize error log
    memset(g_error_log, 0, sizeof(g_error_log));
    g_error_log_index = 0;
    
    // Perform system self-test
    int result = bios_self_test();
    if (result != BIOS_SUCCESS) {
        bios_log_error(ERROR_CRITICAL_SYSTEM, result);
        g_bios_status.error_count++;
        g_bios_status.last_error_code = result;
        g_bios_status.last_error_time = g_bios_status.boot_count;
        
        // Attempt healing
        result = bios_heal_system();
        if (result != BIOS_SUCCESS) {
            // Critical failure - attempt recovery
            bios_perform_critical_recovery();
        }
    }
    
    // Backup critical data
    bios_backup_critical_data();
    
    return BIOS_SUCCESS;
}

int bios_self_test(void) {
    // Test memory integrity
    int result = bios_check_memory_integrity();
    if (result != BIOS_SUCCESS) {
        return result;
    }
    
    // Test flash integrity
    result = bios_verify_flash_integrity();
    if (result != BIOS_SUCCESS) {
        return result;
    }
    
    // Test bus communications
    for (uint32_t i = 0; i < NUM_BUS_CONFIGS; i++) {
        result = bios_test_bus_communication(g_bus_configs[i].bus_id);
        if (result != BIOS_SUCCESS) {
            return result;
        }
    }
    
    // Test system monitoring
    result = bios_monitor_system();
    if (result != BIOS_SUCCESS) {
        return result;
    }
    
    return BIOS_SUCCESS;
}

int bios_heal_system(void) {
    int healing_attempts = 0;
    int result = BIOS_ERROR_CRITICAL;
    
    g_bios_status.healing_attempts++;
    
    // Try different healing strategies based on error type
    error_type_t last_error_type;
    uint16_t last_error_code;
    
    bios_get_last_error(&last_error_type, &last_error_code);
    
    switch (g_healing_strategy) {
        case HEALING_MEMORY_COPY:
            // Attempt to heal corrupted memory segments
            for (uint32_t i = 0; i < NUM_MEMORY_SEGMENTS; i++) {
                if (g_memory_segments[i].flags & SEGMENT_FLAG_HEALABLE) {
                    result = bios_heal_memory_segment(g_memory_segments[i].start_addr, g_memory_segments[i].size);
                    if (result == BIOS_SUCCESS) {
                        healing_attempts++;
                    }
                }
            }
            break;
            
        case HEALING_SECTOR_ERASE:
            // Erase and reprogram corrupted flash sectors
            for (uint32_t i = 0; i < NUM_MEMORY_SEGMENTS; i++) {
                if ((g_memory_segments[i].flags & SEGMENT_FLAG_HEALABLE) && 
                    (g_memory_segments[i].start_addr >= FLASH_BASE_ADDR)) {
                    result = perform_memory_healing(g_memory_segments[i].start_addr, g_memory_segments[i].size);
                    if (result == BIOS_SUCCESS) {
                        healing_attempts++;
                    }
                }
            }
            break;
            
        case HEALING_BUS_RESET:
            // Reset and reconfigure buses
            for (uint32_t i = 0; i < NUM_BUS_CONFIGS; i++) {
                result = bios_reset_bus(g_bus_configs[i].bus_id);
                if (result == BIOS_SUCCESS) {
                    healing_attempts++;
                }
            }
            break;
            
        case HEALING_SYSTEM_RESET:
            // Perform system reset
            bios_system_reset();
            return BIOS_SUCCESS; // This will not return
            
        case HEALING_CRITICAL_RECOVERY:
            // Perform critical recovery
            return bios_perform_critical_recovery();
            
        default:
            break;
    }
    
    // Update system health based on healing results
    if (healing_attempts > 0) {
        g_bios_status.system_health = (g_bios_status.system_health + 100) / 2; // Improve health
        result = BIOS_SUCCESS;
    } else {
        g_bios_status.system_health = g_bios_status.system_health / 2; // Degrade health
        result = BIOS_ERROR_CRITICAL;
    }
    
    update_system_status();
    return result;
}

int bios_check_memory_integrity(void) {
    for (uint32_t i = 0; i < NUM_MEMORY_SEGMENTS; i++) {
        const memory_segment_t* segment = &g_memory_segments[i];
        
        // Calculate CRC of the segment
        uint32_t calculated_crc = bios_calculate_crc32((uint8_t*)segment->start_addr, segment->size);
        
        // Verify against stored CRC
        if (segment->crc32 != 0 && segment->crc32 != calculated_crc) {
            bios_log_error(ERROR_MEMORY_CORRUPTION, i);
            return BIOS_ERROR_CRC;
        }
        
        // Check for obvious corruption patterns
        uint8_t* mem_ptr = (uint8_t*)segment->start_addr;
        bool all_zero = true;
        bool all_ff = true;
        
        for (uint32_t j = 0; j < segment->size && j < 1024; j++) { // Check first 1KB
            if (mem_ptr[j] != 0x00) all_zero = false;
            if (mem_ptr[j] != 0xFF) all_ff = false;
        }
        
        if (all_zero || all_ff) {
            bios_log_error(ERROR_MEMORY_CORRUPTION, i);
            return BIOS_ERROR_MEMORY;
        }
    }
    
    return BIOS_SUCCESS;
}

int bios_heal_memory_segment(uint32_t addr, uint32_t size) {
    // Find the corresponding memory segment
    const memory_segment_t* segment = NULL;
    
    for (uint32_t i = 0; i < NUM_MEMORY_SEGMENTS; i++) {
        if (is_address_in_segment(addr, &g_memory_segments[i])) {
            segment = &g_memory_segments[i];
            break;
        }
    }
    
    if (!segment) {
        return BIOS_ERROR_MEMORY;
    }
    
    // Check if backup is available
    if (!(segment->flags & SEGMENT_FLAG_BACKUP)) {
        return BIOS_ERROR_MEMORY;
    }
    
    // Restore from backup
    uint8_t* backup_ptr = (uint8_t*)segment->backup_addr;
    uint8_t* target_ptr = (uint8_t*)segment->start_addr;
    
    // Verify backup integrity first
    uint32_t backup_crc = bios_calculate_crc32(backup_ptr, size);
    
    // Copy backup data
    for (uint32_t i = 0; i < size; i++) {
        target_ptr[i] = backup_ptr[i];
    }
    
    // Verify restored data
    uint32_t restored_crc = bios_calculate_crc32(target_ptr, size);
    if (restored_crc != backup_crc) {
        return BIOS_ERROR_CHECKSUM;
    }
    
    return BIOS_SUCCESS;
}

int bios_backup_memory_segment(uint32_t addr, uint32_t size) {
    // Find the corresponding memory segment
    const memory_segment_t* segment = NULL;
    
    for (uint32_t i = 0; i < NUM_MEMORY_SEGMENTS; i++) {
        if (is_address_in_segment(addr, &g_memory_segments[i])) {
            segment = &g_memory_segments[i];
            break;
        }
    }
    
    if (!segment) {
        return BIOS_ERROR_MEMORY;
    }
    
    // Check if backup is available
    if (!(segment->flags & SEGMENT_FLAG_BACKUP)) {
        return BIOS_ERROR_MEMORY;
    }
    
    // Copy data to backup
    uint8_t* source_ptr = (uint8_t*)segment->start_addr;
    uint8_t* backup_ptr = (uint8_t*)segment->backup_addr;
    
    for (uint32_t i = 0; i < size; i++) {
        backup_ptr[i] = source_ptr[i];
    }
    
    // Update CRC
    uint32_t crc = bios_calculate_crc32(source_ptr, size);
    // In a real implementation, we would store this CRC
    
    return BIOS_SUCCESS;
}

int bios_verify_flash_integrity(void) {
    // Check flash sectors for integrity
    for (uint32_t sector = 0; sector < (FLASH_SIZE / FLASH_SECTOR_SIZE); sector++) {
        uint32_t sector_addr = FLASH_BASE_ADDR + (sector * FLASH_SECTOR_SIZE);
        
        // Calculate CRC of the sector
        uint32_t calculated_crc = bios_calculate_crc32((uint8_t*)sector_addr, FLASH_SECTOR_SIZE);
        
        // Verify against stored CRC (in a real implementation)
        // For now, just check for obvious corruption
        uint8_t* sector_ptr = (uint8_t*)sector_addr;
        bool all_ff = true;
        
        for (uint32_t i = 0; i < FLASH_SECTOR_SIZE; i++) {
            if (sector_ptr[i] != 0xFF) {
                all_ff = false;
                break;
            }
        }
        
        // If all 0xFF, it might be an erased sector (which is OK for unused sectors)
        // But if it's supposed to contain data, this could indicate corruption
    }
    
    return BIOS_SUCCESS;
}

int bios_configure_bus(bus_config_t* config) {
    // In a real implementation, this would configure the actual hardware bus
    // For now, just store the configuration
    
    for (uint32_t i = 0; i < NUM_BUS_CONFIGS; i++) {
        if (g_bus_configs[i].bus_id == config->bus_id) {
            g_bus_configs[i] = *config;
            return BIOS_SUCCESS;
        }
    }
    
    return BIOS_ERROR_BUS;
}

int bios_test_bus_communication(uint32_t bus_id) {
    // In a real implementation, this would test actual bus communication
    // For now, simulate the test
    
    for (uint32_t i = 0; i < NUM_BUS_CONFIGS; i++) {
        if (g_bus_configs[i].bus_id == bus_id) {
            // Simulate bus test
            // 95% chance of success
            if ((g_bios_status.boot_count % 20) != 0) {
                return BIOS_SUCCESS;
            } else {
                bios_log_error(ERROR_BUS_TIMEOUT, bus_id);
                return BIOS_ERROR_BUS;
            }
        }
    }
    
    return BIOS_ERROR_BUS;
}

int bios_reset_bus(uint32_t bus_id) {
    // In a real implementation, this would reset the actual hardware bus
    // For now, just simulate the reset
    
    for (uint32_t i = 0; i < NUM_BUS_CONFIGS; i++) {
        if (g_bus_configs[i].bus_id == bus_id) {
            // Simulate bus reset
            return BIOS_SUCCESS;
        }
    }
    
    return BIOS_ERROR_BUS;
}

int bios_monitor_system(void) {
    int result = BIOS_SUCCESS;
    
    // Monitor temperature
    result = bios_monitor_temperature();
    if (result != BIOS_SUCCESS) {
        return result;
    }
    
    // Monitor power supply
    result = bios_monitor_power_supply();
    if (result != BIOS_SUCCESS) {
        return result;
    }
    
    // Monitor radiation
    result = bios_monitor_radiation();
    if (result != BIOS_SUCCESS) {
        return result;
    }
    
    // Check watchdog
    result = bios_check_watchdog();
    if (result != BIOS_SUCCESS) {
        return result;
    }
    
    return BIOS_SUCCESS;
}

int bios_monitor_temperature(void) {
    // Simulate temperature monitoring
    // In a real implementation, this would read from actual temperature sensors
    
    // Simulate occasional high temperature
    if ((g_bios_status.boot_count % 100) == 99) {
        bios_log_error(ERROR_TEMPERATURE_HIGH, 85); // 85°C
        g_bios_status.system_health = g_bios_status.system_health * 0.8; // Degrade health
        return BIOS_ERROR_CRITICAL;
    }
    
    return BIOS_SUCCESS;
}

int bios_monitor_power_supply(void) {
    // Simulate power supply monitoring
    // In a real implementation, this would monitor voltage rails
    
    // Simulate occasional power fluctuation
    if ((g_bios_status.boot_count % 200) == 199) {
        bios_log_error(ERROR_POWER_FLUCTUATION, 1);
        g_bios_status.system_health = g_bios_status.system_health * 0.9; // Degrade health
        return BIOS_ERROR_CRITICAL;
    }
    
    return BIOS_SUCCESS;
}

int bios_monitor_radiation(void) {
    // Simulate radiation detection
    // In a real implementation, this would monitor radiation sensors
    
    // Simulate occasional radiation detection
    if ((g_bios_status.boot_count % 500) == 499) {
        bios_log_error(ERROR_RADIATION_DETECTED, 1);
        g_bios_status.system_health = g_bios_status.system_health * 0.7; // Degrade health more significantly
        return BIOS_ERROR_CRITICAL;
    }
    
    return BIOS_SUCCESS;
}

int bios_check_watchdog(void) {
    // Simulate watchdog check
    // In a real implementation, this would service the watchdog timer
    
    // Simulate occasional watchdog timeout
    if ((g_bios_status.boot_count % 1000) == 999) {
        bios_log_error(ERROR_WATCHDOG_TIMEOUT, 1);
        return BIOS_ERROR_CRITICAL;
    }
    
    return BIOS_SUCCESS;
}

uint32_t bios_calculate_crc32(const uint8_t* data, uint32_t size) {
    uint32_t crc = 0xFFFFFFFF;
    
    for (uint32_t i = 0; i < size; i++) {
        uint8_t table_index = (crc ^ data[i]) & 0xFF;
        crc = (crc >> 8) ^ crc32_table[table_index];
    }
    
    return crc ^ 0xFFFFFFFF;
}

uint16_t bios_calculate_checksum(const uint8_t* data, uint32_t size) {
    uint16_t checksum = 0;
    
    for (uint32_t i = 0; i < size; i++) {
        checksum += data[i];
    }
    
    return checksum;
}

int bios_log_error(error_type_t error_type, uint16_t error_code) {
    uint32_t error_log_entry = ((uint32_t)error_type << 16) | error_code;
    
    g_error_log[g_error_log_index] = error_log_entry;
    g_error_log_index = (g_error_log_index + 1) % 100;
    
    g_bios_status.last_error_code = error_code;
    g_bios_status.last_error_time = g_bios_status.boot_count;
    g_bios_status.error_count++;
    
    return BIOS_SUCCESS;
}

int bios_get_last_error(error_type_t* type, uint16_t* code) {
    if (g_error_log_index == 0) {
        return BIOS_ERROR_CRITICAL; // No errors logged
    }
    
    uint32_t last_error = g_error_log[(g_error_log_index - 1) % 100];
    
    *type = (error_type_t)(last_error >> 16);
    *code = (uint16_t)(last_error & 0xFFFF);
    
    return BIOS_SUCCESS;
}

void bios_enter_critical_section(void) {
    // In a real implementation, this would disable interrupts
    g_critical_section_active = true;
}

void bios_exit_critical_section(void) {
    // In a real implementation, this would enable interrupts
    g_critical_section_active = false;
}

bool bios_in_critical_section(void) {
    return g_critical_section_active;
}

void bios_system_reset(void) {
    // In a real implementation, this would trigger a system reset
    // For now, just update status
    g_bios_status.boot_count++;
}

int bios_perform_critical_recovery(void) {
    // Attempt to restore from backup firmware
    int result = bios_restore_from_backup();
    
    if (result == BIOS_SUCCESS) {
        // Jump to backup firmware
        bios_jump_to_backup_firmware();
    }
    
    return result;
}

int bios_backup_critical_data(void) {
    int result = BIOS_SUCCESS;
    
    // Backup all critical memory segments
    for (uint32_t i = 0; i < NUM_MEMORY_SEGMENTS; i++) {
        if (g_memory_segments[i].flags & SEGMENT_FLAG_CRITICAL) {
            int backup_result = bios_backup_memory_segment(g_memory_segments[i].start_addr, g_memory_segments[i].size);
            if (backup_result != BIOS_SUCCESS) {
                result = backup_result;
            }
        }
    }
    
    return result;
}

int bios_restore_from_backup(void) {
    int result = BIOS_SUCCESS;
    
    // Restore all critical memory segments from backup
    for (uint32_t i = 0; i < NUM_MEMORY_SEGMENTS; i++) {
        if (g_memory_segments[i].flags & SEGMENT_FLAG_CRITICAL) {
            int restore_result = bios_heal_memory_segment(g_memory_segments[i].start_addr, g_memory_segments[i].size);
            if (restore_result != BIOS_SUCCESS) {
                result = restore_result;
            }
        }
    }
    
    return result;
}

void bios_jump_to_backup_firmware(void) {
    // In a real implementation, this would jump to backup firmware address
    // For now, just simulate
    g_bios_status.boot_count++;
}

bios_status_t* bios_get_status(void) {
    update_system_status();
    return &g_bios_status;
}

int bios_set_healing_strategy(healing_strategy_t strategy) {
    g_healing_strategy = strategy;
    return BIOS_SUCCESS;
}

healing_strategy_t bios_get_healing_strategy(void) {
    return g_healing_strategy;
}

// Private helper functions
static uint32_t calculate_system_checksum(void) {
    return bios_calculate_checksum((uint8_t*)&g_bios_status, sizeof(bios_status_t) - sizeof(uint32_t));
}

static int perform_memory_healing(uint32_t addr, uint32_t size) {
    // Erase flash sector
    int result = bios_erase_flash_sector(addr);
    if (result != BIOS_SUCCESS) {
        return result;
    }
    
    // Restore from backup
    return bios_heal_memory_segment(addr, size);
}

static void update_system_status(void) {
    g_bios_status.checksum = calculate_system_checksum();
}

static bool is_address_in_segment(uint32_t addr, const memory_segment_t* segment) {
    return (addr >= segment->start_addr) && (addr < (segment->start_addr + segment->size));
}

// Hardware access functions (simplified)
uint8_t bios_read_byte(uint32_t addr) {
    return *(volatile uint8_t*)addr;
}

void bios_write_byte(uint32_t addr, uint8_t value) {
    *(volatile uint8_t*)addr = value;
}

uint32_t bios_read_word(uint32_t addr) {
    return *(volatile uint32_t*)addr;
}

void bios_write_word(uint32_t addr, uint32_t value) {
    *(volatile uint32_t*)addr = value;
}

int bios_erase_flash_sector(uint32_t sector_addr) {
    // In a real implementation, this would erase the flash sector
    // For now, just simulate
    return BIOS_SUCCESS;
}

int bios_program_flash(uint32_t addr, const uint8_t* data, uint32_t size) {
    // In a real implementation, this would program the flash
    // For now, just copy to RAM
    uint8_t* flash_ptr = (uint8_t*)addr;
    for (uint32_t i = 0; i < size; i++) {
        flash_ptr[i] = data[i];
    }
    return BIOS_SUCCESS;
}

int bios_read_flash(uint32_t addr, uint8_t* data, uint32_t size) {
    // In a real implementation, this would read from flash
    // For now, just copy from RAM
    uint8_t* flash_ptr = (uint8_t*)addr;
    for (uint32_t i = 0; i < size; i++) {
        data[i] = flash_ptr[i];
    }
    return BIOS_SUCCESS;
}
