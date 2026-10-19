#ifndef __LV_PORT_STM32U575_H__
#define __LV_PORT_STM32U575_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void LV_Port_Init(void);
uint8_t LV_Port_TouchWasPressed(void);
uint32_t LV_Port_GetFlushFps(void);

#ifdef __cplusplus
}
#endif

#endif
