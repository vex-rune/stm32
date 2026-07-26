#include "is62wv51216.h"

void SRAM_WriteBurst(uint32_t startWord, const uint16_t *buf, uint32_t len)
{
    uint32_t addr = SRAM_BASE_ADDR + (startWord << 1);
    for(uint32_t i = 0; i < len; i++)
    {
        *(volatile uint16_t *)(addr + (i << 1)) = buf[i];
    }
}

void SRAM_ReadBurst(uint32_t startWord, uint16_t *buf, uint32_t len)
{
    uint32_t addr = SRAM_BASE_ADDR + (startWord << 1);
    for(uint32_t i = 0; i < len; i++)
    {
        buf[i] = *(volatile uint16_t *)(addr + (i << 1));
    }
}