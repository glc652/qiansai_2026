/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    wireless_link.h
  * @brief   Wireless UART packet parser for wrist node data.
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __WIRELESS_LINK_H__
#define __WIRELESS_LINK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>

#define WIRELESS_FRAME_HEADER        (0xAAU)
#define WIRELESS_HEALTH_FRAME_HEADER (0xABU)
#define WIRELESS_WRIST_FRAME_SIZE    (31U)
#define WIRELESS_HEALTH_FRAME_SIZE   (17U)
#define WIRELESS_STATUS_IMU_VALID    (0x01U)
#define WIRELESS_STATUS_HEART_VALID  (0x02U)
#define WIRELESS_STATUS2_BT_OK       (0x01U)
#define WIRELESS_STATUS2_SLEEPING    (0x02U)
#define WIRELESS_STATUS2_BAT_VALID   (0x04U)
#define WIRELESS_STATUS2_HEALTH_MODE (0x08U)
#define WIRELESS_STATUS2_HEALTH_EXIT (0x10U)
#define WIRELESS_STATUS2_HEALTH_ENTER (0x20U)

typedef struct
{
  uint16_t seq;
  uint32_t tick;
  int16_t acc_mg[3];        /* Wrist acceleration, g * 1000. */
  int16_t gyro_dps_x10[3];  /* Wrist angular velocity, deg/s * 10. */
  int16_t angle_x100[3];    /* Wrist roll/pitch/yaw, deg * 100. */
  uint16_t heart_rate;      /* bpm, 0 = invalid. */
  uint8_t status;           /* bit0: wrist IMU valid; bit1: heart_rate valid. */
  uint8_t battery_percent;  /* 0-100. */
  uint8_t status2;          /* bit0: ACK; bit1: sleeping; bit2: battery valid; bit3: health page; bit4/5: health exit/enter event. */
} WirelessWristFrame_t;

typedef struct
{
  uint16_t seq;
  uint32_t tick;
  uint8_t heart_rate;
  uint8_t spo2;
  uint8_t micro_circ;
  uint8_t fatigue;
  uint8_t sbp;
  uint8_t dbp;
  uint8_t sdnn;
  uint8_t rmssd;
  uint8_t status;           /* bit0: health data valid. */
} WirelessHealthFrame_t;

typedef struct
{
  uint32_t rx_bytes;
  uint32_t frames_ok;
  uint32_t health_frames_ok;
  uint32_t checksum_errors;
  uint32_t resync_count;
} WirelessLinkStats_t;

void WirelessLink_Init(void);
uint8_t WirelessLink_PushByte(uint8_t byte, WirelessWristFrame_t *frame);
uint8_t WirelessLink_GetLatest(WirelessWristFrame_t *frame);
uint8_t WirelessLink_GetLatestHealth(WirelessHealthFrame_t *frame);
void WirelessLink_GetStats(WirelessLinkStats_t *stats);

#ifdef __cplusplus
}
#endif

#endif /* __WIRELESS_LINK_H__ */
