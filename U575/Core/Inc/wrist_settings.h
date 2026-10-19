#ifndef __WRIST_SETTINGS_H__
#define __WRIST_SETTINGS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"
#include <stdint.h>

#define WRIST_SETTINGS_ROW_COUNT    (4U)

typedef struct {
  lv_obj_t *overlay;
  lv_obj_t *panel;
  lv_obj_t *title;
  lv_obj_t *rows[WRIST_SETTINGS_ROW_COUNT];
  lv_obj_t *labels[WRIST_SETTINGS_ROW_COUNT];
  lv_obj_t *values[WRIST_SETTINGS_ROW_COUNT];
  uint8_t edit_heart_key_enabled;
  uint8_t edit_vibration_enabled;
  uint8_t edit_night_mode;
  uint8_t selected;
} WristSettingsView_t;

uint8_t WristSettings_IsOpen(const WristSettingsView_t *view);
void WristSettings_Open(WristSettingsView_t *view);
void WristSettings_Close(WristSettingsView_t *view);
void WristSettings_SelectNext(WristSettingsView_t *view);
void WristSettings_ApplySelected(WristSettingsView_t *view);

uint8_t WristSettings_IsHeartKeyEnabled(void);
uint8_t WristSettings_IsVibrationEnabled(void);
uint8_t WristSettings_IsNightMode(void);

#ifdef __cplusplus
}
#endif

#endif /* __WRIST_SETTINGS_H__ */
