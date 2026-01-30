#ifndef SELF_HEALING_BIOS_H
#define SELF_HEALING_BIOS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Memory regions and protection
#define FLASH_BASE_ADDR      0x08000000UL
#define RAM_BASE_ADDR        0x20000000UL
#define BACKUP_RAM_BASE      0x40024000UL
#define EEPROM_BASE_ADDR     0x08080000UL

// Memory sizes
#define FLASH_SIZE           (1024 * 1024)     // 1MB
#define RAM_SIZE             (256 * 1024)      // 256KB
#define BACKUP_RAM_SIZE      (4 * 1024)       // 4KB
#define EEPROM_SIZE         (64 * 1024)       // 64KB

// Sector sizes for flash
#define FLASH_SECTOR_SIZE    (4 * 1024)       // 4KB sectors
#define BACKUP_SECTOR_SIZE   (1 * 1024)       // 1KB sectors

// Error codes
#define BIOS_SUCCESS         0
#define BIOS_ERROR_MEMORY    -1
#define BIOS_ERROR_FLASH     -2
#define BIOS_ERROR_CHECKSUM  -3
#define BIOS_ERROR_CRC       -4
#define BIOS_ERROR_BUS       -5
#define BIOS_ERROR_CRITICAL  -6

// System status flags
typedef struct {
    uint32_t magic_number;
    uint16_t version;
    uint16_t build_number;
    uint32_t boot_count;
    uint32_t error_count;
    uint32_t last_error_time;
    uint16_t last_error_code;
    uint8_t  healing_attempts;
    uint8_t  system_health;
    uint32_t checksum;
} bios_status_t;

// Memory segment descriptor
typedef struct {
    uint32_t start_addr;
    uint32_t size;
    uint32_t backup_addr;
    uint32_t crc32;
    uint16_t flags;
    uint16_t reserved;
} memory_segment_t;

// Memory segment flags
#define SEGMENT_FLAG_CRITICAL    0x0001
#define SEGMENT_FLAG_BACKUP      0x0002
#define SEGMENT_FLAG_ENCRYPTED   0x0004
#define SEGMENT_FLAG_READONLY    0x0008
#define SEGMENT_FLAG_EXECUTABLE  0x0010
#define SEGMENT_FLAG_HEALABLE    0x0020

// Bus configuration
typedef struct {
    uint32_t bus_id;
    uint32_t speed_hz;
    uint32_t timeout_ms;
    uint8_t  priority;
    uint8_t  redundancy_level;
    uint16_t flags;
} bus_config_t;

// Bus flags
#define BUS_FLAG_REDUNDANT      0x0001
#define BUS_FLAG_AUTO_RECOVER   0x0002
#define BUS_FLAG_CRITICAL       0x0004
#define BUS_FLAG_MONITORED      0x0008

// Error types
typedef enum {
    ERROR_NONE = 0,
    ERROR_MEMORY_CORRUPTION,
    ERROR_FLASH_FAILURE,
    ERROR_BUS_TIMEOUT,
    ERROR_CRC_MISMATCH,
    ERROR_WATCHDOG_TIMEOUT,
    ERROR_POWER_FLUCTUATION,
    ERROR_TEMPERATURE_HIGH,
    ERROR_RADIATION_DETECTED,
    ERROR_CRITICAL_SYSTEM
} error_type_t;

// Healing strategies
typedef enum {
    HEALING_NONE = 0,
    HEALING_MEMORY_COPY,
    HEALING_SECTOR_ERASE,
    HEALING_BUS_RESET,
    HEALING_SYSTEM_RESET,
    HEALING_CRITICAL_RECOVERY
} healing_strategy_t;

// Function prototypes
int bios_init(void);
int bios_self_test(void);
int bios_heal_system(void);
int bios_monitor_system(void);
int bios_backup_critical_data(void);
int bios_restore_from_backup(void);

// Memory management
int bios_check_memory_integrity(void);
int bios_heal_memory_segment(uint32_t addr, uint32_t size);
int bios_backup_memory_segment(uint32_t addr, uint32_t size);
int bios_verify_backup_integrity(void);

// Flash management
int bios_erase_flash_sector(uint32_t sector_addr);
int bios_program_flash(uint32_t addr, const uint8_t* data, uint32_t size);
int bios_read_flash(uint32_t addr, uint8_t* data, uint32_t size);
int bios_verify_flash_integrity(void);

// Bus management
int bios_configure_bus(bus_config_t* config);
int bios_reset_bus(uint32_t bus_id);
int bios_test_bus_communication(uint32_t bus_id);
int bios_switch_to_backup_bus(uint32_t bus_id);

// Error handling
int bios_detect_errors(void);
int bios_log_error(error_type_t error_type, uint16_t error_code);
int bios_get_last_error(error_type_t* type, uint16_t* code);
int bios_clear_error_log(void);

// System monitoring
int bios_monitor_temperature(void);
int bios_monitor_power_supply(void);
int bios_monitor_radiation(void);
int bios_check_watchdog(void);

// CRC and checksum functions
uint32_t bios_calculate_crc32(const uint8_t* data, uint32_t size);
uint16_t bios_calculate_checksum(const uint8_t* data, uint32_t size);
int bios_verify_data_integrity(const void* data, uint32_t size, uint32_t expected_crc);

// Low-level hardware access
uint8_t bios_read_byte(uint32_t addr);
void bios_write_byte(uint32_t addr, uint8_t value);
uint32_t bios_read_word(uint32_t addr);
void bios_write_word(uint32_t addr, uint32_t value);

// Critical section management
void bios_enter_critical_section(void);
void bios_exit_critical_section(void);
bool bios_in_critical_section(void);

// System reset and recovery
void bios_system_reset(void);
void bios_jump_to_backup_firmware(void);
int bios_perform_critical_recovery(void);

// Configuration and status
bios_status_t* bios_get_status(void);
int bios_set_healing_strategy(healing_strategy_t strategy);
healing_strategy_t bios_get_healing_strategy(void);

#ifdef __cplusplus
}
#endif

#endif // SELF_HEALING_BIOS_H
