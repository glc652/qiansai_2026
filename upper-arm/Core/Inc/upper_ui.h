/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    upper_ui.h
  * @brief   Upper-arm LVGL UI runtime and shared widget helpers.
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __UPPER_UI_H__
#define __UPPER_UI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"
#include <stdint.h>

lv_obj_t *UpperArm_LvglShape(lv_obj_t *parent,
                             lv_coord_t w,
                             lv_coord_t h,
                             lv_coord_t x,
                             lv_coord_t y,
                             lv_color_t color,
                             lv_coord_t radius);

void UpperArm_LvglCreateHome(void);
void UpperArm_LvglUpdateHome(uint32_t now);
void UpperArm_HandleHardwareKeys(uint32_t now);
uint8_t UpperArm_ProcessPendingVoiceCommand(void);
void UpperArm_HistoryLoadFromFlash(void);
uint8_t UpperArm_LvglIsHomePage(void);
uint8_t UpperArm_IsScreenOff(void);

#ifdef __cplusplus
}
#endif

#endif /* __UPPER_UI_H__ */
