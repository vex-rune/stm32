#ifndef  __LORA_H
#define  __LORA_H

#include  "driver_llcc68_interface.h"

// 初始化
void lora_init(void);

// 发送数据
void lora_send(uint8_t* buf, uint16_t len);

// 接受数据
uint8_t lora_receive(uint8_t* buf, uint16_t* len);

#endif
