/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    upper_ui.c
  * @brief   Upper-arm LVGL UI runtime and shared widget helpers.
  ******************************************************************************
  */
/* USER CODE END Header */

#include "upper_ui.h"
#include "app_freertos.h"
#include "lcd_st7789.h"
#include "touch_cst816t.h"
#include "lv_port_stm32u575.h"
#include "vibration_motor.h"
#include "emg_sensor.h"
#include "cmsis_os2.h"

#define UPPER_SCREEN_IDLE_TIMEOUT_MS  (30000U)
#define UPPER_VOICE_WAKE_CMD          (77U)
/* Normal mode: allow home screen idle sleep. Set to 1 only for EMG collection sessions. */
#define UPPER_EMG_COLLECTION_MODE     (0U)
#define UPPER_BOOT_LOGO_DURATION_MS   (1000U)
#define UPPER_ATTITUDE_LCD_DEBUG_UI   (0U)
#define UPPER_BLACK_SCREEN_TEST_UI    (0U)
#define UPPER_LVGL_LOOP_DELAY_MS      (5U)
#define UPPER_LVGL_DATA_REFRESH_MS    (200U)

static volatile uint8_t upper_screen_off_active;
LV_IMG_DECLARE(upper_boot_logo);

uint8_t UpperArm_IsScreenOff(void)
{
  return upper_screen_off_active;
}

static void UpperArm_EnterScreenOff(void)
{
  lv_obj_t *screen = lv_scr_act();

  upper_screen_off_active = 1U;
  EmgSensor_Stop();
  lv_obj_clean(screen);
  lv_obj_remove_style_all(screen);
  lv_obj_set_style_bg_color(screen, lv_color_black(), 0);
  lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
  lv_obj_set_style_bg_grad_dir(screen, LV_GRAD_DIR_NONE, 0);
  lv_obj_set_style_bg_img_src(screen, NULL, 0);
  lv_obj_invalidate(screen);
  LCD_FillScreen(LCD_COLOR_BLACK);
}

static void UpperArm_ExitScreenOff(void)
{
  upper_screen_off_active = 0U;
  (void)EmgSensor_Start();
  UpperArm_HistoryLoadFromFlash();
  UpperArm_LvglCreateHome();
  lv_indev_reset(NULL, NULL);
}

static void UpperArm_LvglCreateBootLogo(void)
{
  lv_obj_t *screen = lv_scr_act();
  lv_obj_t *img;

  lv_obj_clean(screen);
  lv_obj_remove_style_all(screen);
  lv_obj_set_style_bg_color(screen, lv_color_white(), 0);
  lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
  lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

  img = lv_img_create(screen);
  lv_img_set_src(img, &upper_boot_logo);
  lv_obj_center(img);
}

static void UpperArm_LvglHoldBootLogo(uint32_t duration_ms)
{
  uint32_t start = osKernelGetTickCount();
  uint32_t last = start;

  while ((osKernelGetTickCount() - start) < duration_ms)
  {
    uint32_t now = osKernelGetTickCount();
    uint32_t elapsed = now - last;

    if (elapsed != 0U)
    {
      lv_tick_inc(elapsed);
      last = now;
    }

    (void)lv_timer_handler();
    osDelay(5U);
  }
}

lv_obj_t *UpperArm_LvglShape(lv_obj_t *parent,
                             lv_coord_t w,
                             lv_coord_t h,
                             lv_coord_t x,
                             lv_coord_t y,
                             lv_color_t color,
                             lv_coord_t radius)
{
  lv_obj_t *obj = lv_obj_create(parent);

  lv_obj_remove_style_all(obj);
  lv_obj_set_size(obj, w, h);
  lv_obj_set_pos(obj, x, y);
  lv_obj_set_style_radius(obj, radius, 0);
  lv_obj_set_style_bg_color(obj, color, 0);
  lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
  lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
  return obj;
}

void StartDisplayTask(void *argument)
{
#if UPPER_ATTITUDE_LCD_DEBUG_UI
  int32_t last_roll = INT32_MIN;
  int32_t last_pitch = INT32_MIN;
  int32_t last_yaw = INT32_MIN;
#else
  uint32_t last_lv_tick;
  uint32_t last_home_update = 0U;
  uint32_t last_home_activity = 0U;
  uint8_t screen_off = 0U;
#endif

  (void)argument;

  LCD_Init();

#if UPPER_BLACK_SCREEN_TEST_UI
  {
    int32_t last_roll = INT32_MIN;
    int32_t last_pitch = INT32_MIN;
    int32_t last_yaw = INT32_MIN;

  upper_screen_off_active = 0U;
  LCD_SetBacklight(1U);
  LCD_FillScreen(LCD_COLOR_BLACK);
    LCD_Fill(6U, 20U, 24U, 66U, LCD_COLOR_RED);
    LCD_Fill(6U, 110U, 24U, 156U, LCD_COLOR_GREEN);
    LCD_Fill(6U, 200U, 24U, 246U, LCD_COLOR_BLUE);

  for (;;)
  {
      IMUProc_Euler_t euler;
      IMUProc_EulerX10_t euler_x10;
      uint8_t valid = UpperArm_GetLocalImuEuler(&euler);

      if (valid != 0U)
      {
        IMUProc_EulerToSignedX10(&euler, 5, &euler_x10);
        if (euler_x10.roll_x10 != last_roll)
        {
          (void)LCD_DrawNumberX10(34U, 20U, 30U, 46U, 6U, euler_x10.roll_x10, LCD_COLOR_RED, LCD_COLOR_BLACK);
          last_roll = euler_x10.roll_x10;
        }
        if (euler_x10.pitch_x10 != last_pitch)
        {
          (void)LCD_DrawNumberX10(34U, 110U, 30U, 46U, 6U, euler_x10.pitch_x10, LCD_COLOR_GREEN, LCD_COLOR_BLACK);
          last_pitch = euler_x10.pitch_x10;
        }
        if (euler_x10.yaw_x10 != last_yaw)
        {
          (void)LCD_DrawNumberX10(34U, 200U, 30U, 46U, 6U, euler_x10.yaw_x10, LCD_COLOR_BLUE, LCD_COLOR_BLACK);
          last_yaw = euler_x10.yaw_x10;
        }
      }

      osDelay(50U);
  }
  }
#else
#if UPPER_ATTITUDE_LCD_DEBUG_UI
  LCD_FillScreen(LCD_COLOR_BLACK);
  LCD_Fill(6U, 20U, 24U, 66U, LCD_COLOR_RED);
  LCD_Fill(6U, 110U, 24U, 156U, LCD_COLOR_GREEN);
  LCD_Fill(6U, 200U, 24U, 246U, LCD_COLOR_BLUE);
  upper_screen_off_active = 0U;

  for (;;)
  {
    IMUProc_Euler_t euler;
    IMUProc_EulerX10_t euler_x10;
    uint8_t valid = UpperArm_GetLocalImuEuler(&euler);

    if (valid != 0U)
    {
      IMUProc_EulerToSignedX10(&euler, 5, &euler_x10);
      if (euler_x10.roll_x10 != last_roll)
      {
        (void)LCD_DrawNumberX10(34U, 20U, 30U, 46U, 6U, euler_x10.roll_x10, LCD_COLOR_RED, LCD_COLOR_BLACK);
        last_roll = euler_x10.roll_x10;
      }
      if (euler_x10.pitch_x10 != last_pitch)
      {
        (void)LCD_DrawNumberX10(34U, 110U, 30U, 46U, 6U, euler_x10.pitch_x10, LCD_COLOR_GREEN, LCD_COLOR_BLACK);
        last_pitch = euler_x10.pitch_x10;
      }
      if (euler_x10.yaw_x10 != last_yaw)
      {
        (void)LCD_DrawNumberX10(34U, 200U, 30U, 46U, 6U, euler_x10.yaw_x10, LCD_COLOR_BLUE, LCD_COLOR_BLACK);
        last_yaw = euler_x10.yaw_x10;
      }
    }

    osDelay(50);
  }
#else
  (void)CST816T_Init();
  LV_Port_Init();
  UpperArm_LvglCreateBootLogo();
  UpperArm_LvglHoldBootLogo(UPPER_BOOT_LOGO_DURATION_MS);
  UpperArm_HistoryLoadFromFlash();
  UpperArm_LvglCreateHome();
  Vibration_Pulse(120U);
  upper_screen_off_active = 0U;

  last_lv_tick = osKernelGetTickCount();
  last_home_activity = last_lv_tick;

  for (;;)
  {
    uint32_t now = osKernelGetTickCount();
    uint32_t elapsed = now - last_lv_tick;
    uint8_t touch_pressed;
    uint8_t voice_cmd;

    if (elapsed != 0U)
    {
      lv_tick_inc(elapsed);
      last_lv_tick = now;
    }

    Vibration_Update(now);
    UpperArm_HandleHardwareKeys(now);
    voice_cmd = UpperArm_ProcessPendingVoiceCommand();
    touch_pressed = LV_Port_TouchWasPressed();

    if (screen_off != 0U)
    {
      if ((touch_pressed != 0U) ||
          (voice_cmd == UPPER_VOICE_WAKE_CMD))
      {
        screen_off = 0U;
        UpperArm_ExitScreenOff();
        last_home_activity = now;
      }

      (void)lv_timer_handler();
      osDelay(UPPER_LVGL_LOOP_DELAY_MS);
      continue;
    }

    if (UpperArm_LvglIsHomePage() == 0U)
    {
      last_home_activity = now;
    }
    else if (touch_pressed != 0U)
    {
      last_home_activity = now;
    }
    else if ((UPPER_EMG_COLLECTION_MODE == 0U) &&
             ((now - last_home_activity) >= UPPER_SCREEN_IDLE_TIMEOUT_MS))
    {
      screen_off = 1U;
      UpperArm_EnterScreenOff();
    }

    if ((screen_off == 0U) && ((now - last_home_update) >= UPPER_LVGL_DATA_REFRESH_MS))
    {
      last_home_update = now;
      UpperArm_LvglUpdateHome(now);
    }

    (void)lv_timer_handler();
    osDelay(UPPER_LVGL_LOOP_DELAY_MS);
  }
#endif
#endif
}
