#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {



#endif

#include <stdint.h>

bool BLE_Init();

void BLE_SendData(uint8_t  data[], uint16_t len);

void BLE_ReadData(uint8_t  data[],uint16_t *rxlen);


#ifdef __cplusplus
}
#endif

#endif /* __WIFI_H */
