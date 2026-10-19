#ifndef __RTC_H__
#define __RTC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

void MX_RTC_Init(void);
uint8_t RTC_GetHourMinute(uint8_t *hour, uint8_t *minute);
uint8_t RTC_SetTime(uint8_t hour, uint8_t minute, uint8_t second);

#ifdef __cplusplus
}
#endif

#endif /* __RTC_H__ */
