#ifndef M25P80_H
#define M25P80_H

#include <stdbool.h>  // bool
#include <stddef.h>   // size_t
#include <stdint.h>   // uint_t

#define M25P80_PAGE_SIZE      256  // bytes
#define M25P80_SECTOR_SIZE    256  // pages
#define M25P80_SECTOR_NUMBER  16
#define M25P80_PAGE_NUMBER    (M25P80_SECTOR_SIZE * M25P80_SECTOR_NUMBER)

typedef struct {
  uint8_t
    srwd:1,    // b7
    unused1:1,
    unused2:1,
    bp2:1,
    bp1:1,
    bp0:1,
    wel:1,
    wip:1;     // b0
} m25p80_sr_t;

bool m25p80_verify_signature(void);
m25p80_sr_t m25p80_get_status(void);

void m25p80_wakeup(void);
void m25p80_power_down(void);

void m25p80_erase_sector(uint32_t pageAddress);
void m25p80_erase_bulk(void);

void m25p80_write(uint8_t *buffer, size_t buffer_len, uint32_t pageAddress);
void m25p80_read (uint8_t *buffer, size_t buffer_len, uint32_t pageAddress);

#define m25p80_save_page(page, buffer) m25p80_write( ((uint32_t)(page)) << 8, (buffer), \
                                                     M25P80_PAGE_SIZE)
#define m25p80_load_page(page, buffer) m25p80_read ( ((uint32_t)(page)) << 8, (buffer), \
                                                     M25P80_PAGE_SIZE)

#endif // M25P80_H
