#include "m25p80.h"
#include "dbg_print.h"

#include <stdint.h> // uint_t

#define DATA_LEN 255
static uint8_t data_out[DATA_LEN] = {0};
static uint8_t data_in [DATA_LEN] = {0};

static bool test_page_write_read_confirm(void);
static bool test_page_write_read_confirm_all_sectors(void);
static void purge_state(void);

int main(void)
{
    if (!m25p80_verify_signature()) {
        dbg_printf("Error: Couldn't find M25P80.\n");
        return -1;
    }

    if (!test_page_write_read_confirm()) {
        dbg_printf("Failed: test_page_write_read_confirm.\n");
        return -1;
    }

    if (!test_page_write_read_confirm_all_sectors()) {
        dbg_printf("Failed: test_page_write_read_confirm_all_sectors.\n");
        return -1;
    }

    dbg_printf("All tests successful.\n");

    return 0;
}

static bool test_page_write_read_confirm()
{
    purge_state();

    for (uint8_t i = 0; i < DATA_LEN; i++) data_out[i] = i;

    uint32_t page_address = 0;

    m25p80_write(data_out, DATA_LEN, page_address);
    m25p80_read (data_in,  DATA_LEN, page_address);

    if (memcmp(data_out, data_in, DATA_LEN) == 0) {
        return true;
    } else {
        return false;
    }
}

static bool test_page_write_read_confirm_all_sectors()
{
    purge_state();

    for (uint8_t i = 0; i < DATA_LEN; i++) data_out[i] = i;

    // Create page addresses
    uint32_t page_addresses[M25P80_SECTOR_NUMBER] = {0};
    for (uint8_t sector_i = 0; sector_i < M25P80_SECTOR_NUMBER; sector_i++) {
        uint32_t page_address = sector_i * M25P80_SECTOR_SIZE;
        page_addresses[sector_i] = page_address;
    }

    // Write data to addresses
    for (uint32_t page_i = 0; page_i < sizeof(page_addresses); page_i++) {
        uint32_t page_address = page_addresses[page_i];
        m25p80_write(data_out, DATA_LEN, page_address);
    }

    // Read data from addresses and compare
    for (uint32_t page_i = 0; page_i < sizeof(page_addresses); page_i++) {
        uint32_t page_address = page_addresses[page_i];
        m25p80_read(data_in, DATA_LEN, page_address);

        if (memcmp(data_out, data_in, DATA_LEN) != 0) {
            return false;
        }
    }

    return true;
}

static void purge_state()
{
    memset(data_out, 0, DATA_LEN);
    memset(data_in,  0, DATA_LEN);

    m25p80_erase_bulk();
}
