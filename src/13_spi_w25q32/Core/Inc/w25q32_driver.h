#ifndef W25Q32_DRIVER_H
#define W25Q32_DRIVER_H
#include "spi.h"

void w25q32_start(void);

void w25q32_stop(void);
void w25q32_read_id(uint8_t* mid, uint16_t* did);
void w25q32_write_enable(void);
void w25q32_write_disable(void);

void w25q32_wait_not_busy(void);
void w25q32_erase_sector(uint8_t block, uint8_t sector);
void w25q32_write(
    uint8_t block,
    uint8_t sector,
    uint8_t page,
    const uint8_t* data,
    uint16_t len);

uint8_t w25q32_read(
    uint8_t block,
    uint8_t sector,
    uint8_t page,
    uint8_t innerAddr,
    uint8_t *data,
    int len
);


#endif
