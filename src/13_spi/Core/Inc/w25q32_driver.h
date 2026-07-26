#ifndef W25Q32_DRIVER_H
#define W25Q32_DRIVER_H
#include "spi.h"

void w25q32_init(void);
uint8_t w25q32_swap_read(uint8_t byte);
void w25q32_read_id(uint8_t* mid, uint16_t* did);

#endif
