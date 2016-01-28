#include "m25p80.h"

#include <string.h> // memset

#define OPCODE_WREN      0x06u // Write enable
#define OPCODE_WRDI      0x04u // Write disable
#define OPCODE_RDID      0x9Fu // Read identification
#define OPCODE_RDSR      0x05u // Read status register
#define OPCODE_WRSR      0x01u // Write status register
#define OPCODE_READ      0x03u // Read data bytes
#define OPCODE_FAST_READ 0x0Bu // Read data bytes at higher speed
#define OPCODE_PP        0x02u // Page program
#define OPCODE_SE        0xD8u // Sector erase
#define OPCODE_BE        0xC7u // Bulk erase
#define OPCODE_DP        0xB9u // Deep power-down
#define OPCODE_RES       0xABu // Release from deep power-down

#define M25P80_SIGNATURE  0x13u
#define GET_BYTE_N(w, n) ((uint8_t)(w >> (8 * n)))

#define INPUT_BUFFER_SIZE   (256 + 4 + 4 + 1)
#define OUTPUT_BUFFER_SIZE  (256 + 4 + 4 + 1)
static uint8_t in  [INPUT_BUFFER_SIZE] = {0};
static uint8_t out[OUTPUT_BUFFER_SIZE] = {0};

static bool m25p80_wip(void);
static bool m25p80_write_enable(void);

static m25p80_sr_t byte_to_status(uint8_t byte);

static void delay_ms(uint32_t ms);
static void spi_transceive(uint8_t *out, uint8_t *in, size_t len);

bool m25p80_verify_signature(void)
{
    memset(out, 0, OUTPUT_BUFFER_SIZE * sizeof(out[0]));
    memset(in,  0, INPUT_BUFFER_SIZE  * sizeof(in[0]));
    out[0] = OPCODE_RES;
    out[1] = 0x00;
    out[2] = 0x00;
    out[3] = 0x00;
    size_t out_len = 4;

    spi_transceive(out, in, out_len);

    return (GET_BYTE_N(in[3], 1) == M25P80_SIGNATURE);
}

m25p80_sr_t m25p80_get_status(void)
{
    m25p80_sr_t status;

    memset(out, 0, OUTPUT_BUFFER_SIZE * sizeof(out[0]));
    memset(in,  0, INPUT_BUFFER_SIZE  * sizeof(in[0]));
    out[0] = OPCODE_RDSR;
    size_t out_len = 2;

    spi_transceive(out, in, out_len);

    status = byte_to_status(in[1]);
    return status;
}

void m25p80_wakeup(void)
{
    memset(out, 0, OUTPUT_BUFFER_SIZE * sizeof(out[0]));
    out[0] = OPCODE_RES;
    size_t out_len = 1;

    spi_transceive(out, in, out_len);
}

void m25p80_power_down(void)
{
    memset(out, 0, OUTPUT_BUFFER_SIZE * sizeof(out[0]));
    out[0] = OPCODE_DP;
    size_t out_len = 1;

    spi_transceive(out, in, out_len);
}

void m25p80_erase_sector(uint32_t pageAddress)
{
    while (!m25p80_wip());
    while (!m25p80_write_enable());

    memset(out, 0, OUTPUT_BUFFER_SIZE * sizeof(out[0]));
    out[0] = OPCODE_SE;
    out[1] = GET_BYTE_N(pageAddress, 2);
    out[2] = GET_BYTE_N(pageAddress, 1);
    out[3] = GET_BYTE_N(pageAddress, 0);
    size_t out_len = 4;

    spi_transceive(out, in, out_len);

    while (!m25p80_wip());
}

void m25p80_erase_bulk(void)
{
    while (!m25p80_wip());
    while (!m25p80_write_enable());

    memset(out, 0, OUTPUT_BUFFER_SIZE * sizeof(out[0]));
    out[0] = OPCODE_BE;
    size_t out_len = 1;

    spi_transceive(out, in, out_len);

    while (!m25p80_wip());
}

void m25p80_write(uint8_t *buffer, size_t buffer_len, uint32_t pageAddress)
{
    while (!m25p80_wip());
    while (!m25p80_write_enable());

    memset(out, 0, OUTPUT_BUFFER_SIZE * sizeof(out[0]));
    out[0] = OPCODE_PP;
    out[1] = GET_BYTE_N(pageAddress, 2);
    out[2] = GET_BYTE_N(pageAddress, 1);
    out[3] = GET_BYTE_N(pageAddress, 0);
    size_t out_len = 4 + (buffer_len / sizeof(out[0]));

    memcpy(out + 4, buffer, buffer_len);

    spi_transceive(out, in, out_len);
}

void m25p80_read(uint8_t *buffer, size_t buffer_len, uint32_t pageAddress)
{
    buffer_len /= sizeof(out[0]);

    while (!m25p80_wip());

    memset(out, 0, OUTPUT_BUFFER_SIZE * sizeof(out[0]));
    memset(in,  0, OUTPUT_BUFFER_SIZE * sizeof(in[0]));
    out[0] = OPCODE_READ;
    out[1] = GET_BYTE_N(pageAddress, 2);
    out[2] = GET_BYTE_N(pageAddress, 1);
    out[3] = GET_BYTE_N(pageAddress, 0);
    size_t out_len = 4 + buffer_len;

    spi_transceive(out, in, out_len);

    memcpy(buffer, in + 4, buffer_len * sizeof(in[0]));
}

static bool m25p80_write_enable(void)
{
    m25p80_sr_t status;
    bool res;

    delay_ms(10);

    memset(out, 0, OUTPUT_BUFFER_SIZE * sizeof(out[0]));
    out[0] = OPCODE_WREN;
    size_t out_len = 1;

    spi_transceive(out, in, out_len);

    delay_ms(10);

    status = m25p80_get_status();
    res = status.wel ? true : false;

    delay_ms(10);
    return res;
}

static bool m25p80_wip(void)
{
    m25p80_sr_t status;
    bool res;

    delay_ms(10);

    status = m25p80_get_status();
    res = status.wel ? false : true;

    delay_ms(10);
    return res;
}

static m25p80_sr_t byte_to_status(uint8_t byte)
{
    m25p80_sr_t status;

    status.srwd    = byte >> 7;
    status.unused1 = byte >> 6;
    status.unused2 = byte >> 5;
    status.bp2     = byte >> 4;
    status.bp1     = byte >> 3;
    status.bp0     = byte >> 2;
    status.wel     = byte >> 1;
    status.wip     = byte >> 0;

    return status;
}

/*
    Wrapper for System-specific implementation.
*/
static void delay_ms(uint32_t ms)
{
    //
}

/*
    Wrapper for System-specific implementation.

    Expecting to send `len` bytes out of the `out`
    buffer simultaneously receiving `len` bytes
    in the `in` buffer.
*/
static void spi_transceive(uint8_t *out, uint8_t *in, size_t len)
{
    //
}
