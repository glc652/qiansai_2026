/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : FreeRTOS applicative file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "app_freertos.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd_touch_test.h"
#include "adc.h"
#include "lsm6dsr.h"
#include "emg_sensor.h"
#include "imu_processing.h"
#include "wireless_link.h"
#include "rehab_eval.h"
#include "voice_i2c_slave.h"
#include "vibration_motor.h"
#include "usart.h"
#include "upper_ui.h"
#include "lv_port_stm32u575.h"
#include "stm32u5xx_hal_flash.h"
#include "stm32u5xx_hal_flash_ex.h"
#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* USER CODE BEGIN PV_FONT */
/* LVGL page resources are declared in upper_ui_pages.inc. */
/* USER CODE END PV_FONT */

/* USER CODE BEGIN PTD */
typedef enum
{
  REHAB_ACTION_IDLE = 0,
  REHAB_ACTION_ELBOW_FLEX,
  REHAB_ACTION_ARM_ABDUCTION,
  REHAB_ACTION_FOREARM_ROTATION,
  REHAB_ACTION_SHOULDER_LIFT
} RehabAction_t;

typedef struct
{
  int16_t upper_acc_mg[3];
  int16_t upper_gyro_x10[3];
  int16_t upper_angle_x100[3];
  int16_t wrist_acc_mg[3];
  int16_t wrist_gyro_x10[3];
  int16_t wrist_angle_x100[3];
  uint16_t wrist_heart_rate;
  int32_t elbow_relative_pitch_x100;
  int32_t elbow_angle_x100;
  int32_t forearm_rotation_angle_x100;
  int32_t arm_abduction_angle_x100;
  int32_t shoulder_lift_angle_x100;
  int32_t elbow_velocity_x100_s;
  int32_t elbow_range_x100;
  int32_t emg_rms_x10;
  uint8_t level;
  uint8_t emg_active;
  uint8_t valid;
  RehabAction_t action;
} RehabFrame_t;

typedef struct
{
  uint16_t reps;
  uint16_t target_reps;
  int32_t current_angle_x100;
  int32_t max_angle_x100;
  int32_t avg_peak_angle_x100;
  uint8_t completion_score;
  uint8_t amplitude_score;
  uint8_t total_score;
  uint8_t emg_rate;
  uint8_t valid;
} TrainingEvalSnapshot_t;

typedef struct
{
  uint16_t reps;
  uint16_t target_reps;
  uint8_t action_index;
  int16_t current_angle_deg;
  int16_t max_angle_deg;
  int16_t avg_angle_deg;
  int16_t elbow_deg;
  int16_t forearm_deg;
  int16_t arm_abd_deg;
  int16_t shoulder_deg;
  uint8_t emg_strength;
  uint8_t emg_mode;
  uint8_t completion_score;
  uint8_t amplitude_score;
  uint8_t total_score;
  uint8_t valid;
} EspTrainingPayloadSnapshot_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define IMU_ATTITUDE_KP    (1.0f)
#define IMU_ATTITUDE_KI    (0.05f)
#define IMU_ANGLE_DEADBAND_X10  (5)
#define IMU_GYRO_DEADBAND_RAW  (2)
#define UPPER_IMU_ONLY_TEST  (0U)
#define UPPER_DEFAULT_TASK_ENABLE  (0U)
#define UPPER_UI_IDLE_TASK_ENABLE  (0U)
#define UPPER_ALGO_TASK_ENABLE  (1U)
#define UPPER_EMG_TASK_ENABLE  (1U)
#define UPPER_WIRELESS_TASK_ENABLE  (1U)
#define UPPER_DEBUG_TASK_ENABLE  (1U)
#define UPPER_WIFI_TASK_ENABLE  (1U)
#define DATA_STREAM_PERIOD_MS  (100U)
#define EMG_POLL_PERIOD_MS     (10U)
#define EMG_CSV_STREAM_ENABLE  (0U)
#define EMG_UART4_RECORD_MODE  (0U)
/* 非 LCD 模块调试日志默认关闭，排查训练/无线/语音时置 1 */
#define APP_DEBUG_PRINTF       (0U)
#define WIRELESS_DEBUG_PRINTF  (0U)
#define DEBUG_UART_OUTPUT_ENABLE (0U)
/* 采样任务里的陀螺仪日志会阻塞 200Hz 采样，默认关闭，仅排查时置 1 */
#define SENSOR_GYRO_DEBUG_PRINT  (0U)
#define SENSOR_SAMPLE_PERIOD_MS  (5U)
#define REHAB_ALGO_PERIOD_MS  (100U)
#define REHAB_WINDOW_MS       (1000U)
#define REHAB_ELBOW_RANGE_FLEX_X100       (2500)
#define REHAB_ELBOW_VEL_FLEX_X100_S       (2500)
#define REHAB_ELBOW_HOLD_ENTER_X100       (3000)
#define REHAB_ELBOW_HOLD_EXIT_X100        (3000)
#define REHAB_FOREARM_ROT_HOLD_ENTER_X100 (2000)
#define REHAB_FOREARM_ROT_HOLD_EXIT_X100  (1500)
#define REHAB_FOREARM_ROT_STRONG_X100     (3000)
#define REHAB_ARM_ABD_HOLD_ENTER_X100     (2000)
#define REHAB_ARM_ABD_HOLD_EXIT_X100      (1800)
#define REHAB_ARM_ABD_DYNAMIC_MIN_X100    (800)
#define REHAB_ARM_ABD_MIN_SHOULDER_X100   (2000)
#define REHAB_ARM_ABD_MIN_SHOULDER_EXIT_X100 (1800)
#define REHAB_SHOULDER_HOLD_ENTER_X100    (1800)
#define REHAB_SHOULDER_HOLD_EXIT_X100     (1000)
#define REHAB_ARM_ABD_ACC_Z_DELTA_MG      (150)
#define REHAB_ACTION_LATCH_MS             (1500U)
#define REHAB_WRIST_ROT_GYRO_X10          (200)
#define REHAB_FOREARM_UPPER_STABLE_GYRO_X10 (300)
#define REHAB_FOREARM_UPPER_STABLE_ANGLE_X100 (2000)
#define REHAB_ARM_ABDUCTION_GYRO_X10      (250)
#define REHAB_SHOULDER_LIFT_GYRO_X10      (80)
#define REHAB_ACTIVE_EVAL_ELBOW_FLEX      (1U)
#define REHAB_ACTIVE_EVAL_FOREARM_ROT     (2U)
#define REHAB_ACTIVE_EVAL_ARM_ABD         (3U)
#define REHAB_ACTIVE_EVAL_SHOULDER_LIFT   (4U)
#define REHAB_ACTIVE_EVAL                 REHAB_ACTIVE_EVAL_ARM_ABD
/* 25% relative EMG amplitude is treated as full effort for the simple force estimate. */
#define EMG_FORCE_FULL_SCALE_PERCENT_X10  (250U)
#define EMG_FLEX_RELAX_ANGLE_X100         (500)
#define EMG_FLEX_FULL_ANGLE_X100          (6500)
#define EMG_FLEX_STRONG_ENTER             (68U)
#define EMG_FLEX_STRONG_EXIT              (55U)
#define EMG_FLEX_WEAK_ENTER               (18U)
#define EMG_FLEX_WEAK_EXIT                (10U)
/* 屈肘肌电备用方案：
 * 0（默认）= 使用肌电传感器真实计算的 active/strength/mode；
 * 1= 当传感器不可用时，按肘关节角度估算强度并覆盖肌电结果。 */
#define EMG_FLEX_ANGLE_FALLBACK_ENABLE     (0U)
#define WIRELESS_LINK_TIMEOUT_MS          (30000U)
#define WIRELESS_ACK_BYTE0                (0xA5U)
#define WIRELESS_ACK_BYTE1                (0x5AU)
#define WIRELESS_LINK_MSG_BYTE2           (0xC3U)
#define WIRELESS_LINK_MSG_BYTE3           (0x3CU)
#define WIRELESS_TRAIN_MSG_BYTE2          (0xD4U)
#define WIRELESS_TRAIN_MSG_BYTE3          (0x4DU)
#define WIRELESS_HOME_MSG_BYTE2           (0xE5U)
#define WIRELESS_HOME_MSG_BYTE3           (0x5EU)
#define WIRELESS_HEALTH_MSG_BYTE2         (0xF6U)
#define WIRELESS_HEALTH_MSG_BYTE3         (0x6FU)
#define WIRELESS_RAW_TRACE_SIZE           (32U)
#define ESP8266_UPLOAD_ENABLE             (1U)
#define ESP8266_MINIMAL_TX_TEST           (0U)
#define ESP8266_AT_ONLY_TEST              (0U)
#define ESP8266_HTTP_ONLY_TEST            (0U)
#define ESP8266_MQTT_SHORT_TEST           (0U)
#define ESP8266_MQTT_MIN_TRAIN_TEST       (0U)
#define ESP8266_MQTT_FIXED_TRAIN_TEST     (0U)
#define ESP8266_MQTT_LIGHT_ONCE_TEST      (0U)
#define ESP8266_MQTT_FULL_ONCE_TEST       (0U)
#define ESP8266_LONGEST_PAYLOAD_SIM_TEST  (0U)
#define ESP8266_SKIP_UPLOAD_ON_TRAINING_PAGE (0U)
#define ESP8266_WIFI_SSID                 "opp"
#define ESP8266_WIFI_PASSWORD             "fuwe2552"
#define ESP8266_MQTT_HOST                 "10.35.253.211"
#define ESP8266_MQTT_PORT                 (1883U)
#define ESP8266_HTTP_PORT                 (5000U)
#define ESP8266_MQTT_CLIENT_ID            "upper-arm-001"
#define ESP8266_MQTT_TOPIC_REALTIME       "rehab/upper/realtime"
#define ESP8266_MQTT_TOPIC_HEALTH         "rehab/upper/health"
/* Web/MQTT 实时数据上传周期在这里修改。
 * 500 ms 约为 2 Hz；建议不要低于 200 ms，避免低优先级 WiFi 任务产生
 * 过多串口与 ESP8266 通信，影响屏幕、IMU、蓝牙和肌电等任务。 */
#define ESP8266_UPLOAD_PERIOD_MS          (500U)
#define ESP8266_MINIMAL_TX_PERIOD_MS      (500U)
#define ESP8266_MQTT_SESSION_REFRESH_MS   (10000U)
#define ESP8266_HEALTH_UPLOAD_PERIOD_MS   (10000U)
#define ESP8266_TIME_SYNC_RETRY_MS        (30000U)
#define ESP8266_TIME_SYNC_REFRESH_MS      (600000U)
#define ESP8266_START_DELAY_MS            (8000U)
#define ESP8266_HEALTH_REQUEST_HOLD_MS    (8000U)
#define ESP8266_UI_TRANSITION_PAUSE_MS    (1200U)
#define UPPER_RTC_BACKUP_MAGIC            (0xA15A2026UL)

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#if APP_DEBUG_PRINTF
#define APP_LOG_PRINTF(...)    printf(__VA_ARGS__)
#else
#define APP_LOG_PRINTF(...)    ((void)0)
#endif
#if WIRELESS_DEBUG_PRINTF
#define WIRELESS_LOG_PRINTF(...)    printf(__VA_ARGS__)
#else
#define WIRELESS_LOG_PRINTF(...)    ((void)0)
#endif

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
/* Upper-arm node local IMU cache. */
static LSM6DSR_Data_t debug_imu_raw;
static IMUProc_Euler_t debug_imu_euler;
static uint8_t debug_imu_valid;
static volatile uint8_t imu_solver_ready;
static WirelessWristFrame_t latest_wrist_frame;
static uint8_t latest_wrist_valid;
static WirelessHealthFrame_t latest_wrist_health_frame;
static uint8_t latest_wrist_health_valid;
static volatile uint8_t wireless_rx_enabled;
static volatile uint32_t wireless_last_frame_tick;
static volatile uint32_t wireless_last_health_tick;
static RehabFrame_t latest_rehab_frame;
static uint8_t latest_rehab_valid;
static TrainingEvalSnapshot_t training_eval_snapshot;
static EspTrainingPayloadSnapshot_t esp_training_payload_snapshot;
static volatile uint8_t wireless_train_start_pending;
static volatile uint8_t wireless_train_finish_pending;
static volatile uint8_t wireless_health_start_pending;
static volatile uint32_t esp_upload_pause_until_tick;
static volatile uint32_t upper_health_request_until_tick;
static volatile uint8_t upper_page_result_event_frames;
static volatile uint8_t upper_page_home_event_frames;
static volatile uint8_t upper_voice_pending_cmd;
static volatile uint8_t upper_voice_has_pending;
/* USART1 单字节中断接收缓冲，喂给腕端无线帧解析器 */
static uint8_t wireless_rx_byte;
static volatile uint8_t wireless_raw_trace[WIRELESS_RAW_TRACE_SIZE];
static volatile uint8_t wireless_raw_trace_index;
static volatile uint8_t wireless_raw_trace_count;
static volatile uint32_t wireless_last_byte_tick;
static volatile uint32_t upper_time_epoch_at_sync;
static volatile uint32_t upper_time_tick_at_sync;
static volatile uint8_t upper_time_valid;
static volatile uint8_t upper_rtc_ready;
static RTC_HandleTypeDef hrtc;

/* USER CODE END Variables */

/* USER CODE BEGIN 0 */
#if EMG_FLEX_ANGLE_FALLBACK_ENABLE
static void Emg_ApplyFlexRelaxGuard(uint8_t action_index,
                                    int32_t angle_x100,
                                    EmgSensor_Features_t *features)
{
  int32_t abs_angle_x100;
  uint32_t norm;
  uint32_t target;
  int32_t target_x10;
  int32_t diff_x10;
  int32_t emg_mod;
  int32_t jitter;
  uint32_t now_tick;
  static int32_t filtered_strength_x10 = 0;
  static uint32_t last_update_tick = 0U;
  static uint8_t latched_mode = 0U;

  if (features == NULL)
  {
    return;
  }
  if (action_index != 0U)
  {
    filtered_strength_x10 = 0;
    last_update_tick = 0U;
    latched_mode = 0U;
    return;
  }

  abs_angle_x100 = angle_x100;
  if (abs_angle_x100 < 0)
  {
    abs_angle_x100 = -abs_angle_x100;
  }

  if (abs_angle_x100 <= EMG_FLEX_RELAX_ANGLE_X100)
  {
    target = 0U;
  }
  else
  {
    if (abs_angle_x100 >= EMG_FLEX_FULL_ANGLE_X100)
    {
      norm = 100U;
    }
    else
    {
      norm = ((uint32_t)(abs_angle_x100 - EMG_FLEX_RELAX_ANGLE_X100) * 100U) /
             (uint32_t)(EMG_FLEX_FULL_ANGLE_X100 - EMG_FLEX_RELAX_ANGLE_X100);
    }

    if (norm < 25U)
    {
      target = (norm * 32U) / 25U;
    }
    else if (norm < 70U)
    {
      target = 32U + (((norm - 25U) * 43U) / 45U);
    }
    else
    {
      target = 75U + (((norm - 70U) * 25U) / 30U);
    }
  }

  emg_mod = 0;
  if ((target != 0U) && (features->calibrated != 0U) && (features->baseline_x10 > 0))
  {
    if (features->envelope_x10 > features->rms_x10)
    {
      emg_mod += (features->envelope_x10 - features->rms_x10) / 350;
    }
    else
    {
      emg_mod -= (features->rms_x10 - features->envelope_x10) / 500;
    }
    if (emg_mod > 6)
    {
      emg_mod = 6;
    }
    else if (emg_mod < -6)
    {
      emg_mod = -6;
    }
  }

  now_tick = osKernelGetTickCount();
  if (last_update_tick == 0U)
  {
    last_update_tick = now_tick;
    filtered_strength_x10 = (int32_t)target * 10;
  }

  target_x10 = ((int32_t)target + emg_mod) * 10;
  if (target_x10 < 0)
  {
    target_x10 = 0;
  }
  else if (target_x10 > 1000)
  {
    target_x10 = 1000;
  }

  diff_x10 = target_x10 - filtered_strength_x10;
  if (diff_x10 >= 0)
  {
    filtered_strength_x10 += (diff_x10 * 28) / 100;
  }
  else
  {
    filtered_strength_x10 += (diff_x10 * 16) / 100;
  }

  jitter = 0;
  if (filtered_strength_x10 > 120)
  {
    jitter = (int32_t)(((now_tick / 230U) + features->raw + (uint32_t)filtered_strength_x10) % 5U) - 2;
  }
  filtered_strength_x10 += jitter;
  if (filtered_strength_x10 < 0)
  {
    filtered_strength_x10 = 0;
  }
  else if (filtered_strength_x10 > 1000)
  {
    filtered_strength_x10 = 1000;
  }
  last_update_tick = now_tick;

  features->strength = (uint8_t)((filtered_strength_x10 + 5) / 10);
  if ((latched_mode == 2U) && (features->strength <= EMG_FLEX_STRONG_EXIT))
  {
    latched_mode = (features->strength >= EMG_FLEX_WEAK_EXIT) ? 1U : 0U;
  }
  else if ((latched_mode != 2U) && (features->strength >= EMG_FLEX_STRONG_ENTER))
  {
    latched_mode = 2U;
  }
  else if ((latched_mode == 1U) && (features->strength < EMG_FLEX_WEAK_EXIT))
  {
    latched_mode = 0U;
  }
  else if ((latched_mode == 0U) && (features->strength >= EMG_FLEX_WEAK_ENTER))
  {
    latched_mode = 1U;
  }

  features->mode = latched_mode;
  features->active = (features->mode != 0U) ? 1U : 0U;
  if (features->active == 0U)
  {
    features->strength = 0U;
  }
}
#endif

static const char *Emg_ModeLabel(uint8_t mode)
{
  if (mode == 2U)
  {
    return "strong";
  }
  if (mode == 1U)
  {
    return "weak";
  }
  return "relax";
}

static void DebugUart_WriteBuffer(const char *buffer, int length)
{
#if DEBUG_UART_OUTPUT_ENABLE
  if ((buffer == NULL) || (length <= 0))
  {
    return;
  }

  if (length > 255)
  {
    length = 255;
  }

  (void)HAL_UART_Transmit(&huart4, (uint8_t *)buffer, (uint16_t)length, 100U);
#else
  (void)buffer;
  (void)length;
#endif
}

uint8_t UpperArm_GetLocalImuEuler(IMUProc_Euler_t *euler)
{
  uint8_t valid;

  if (euler == NULL)
  {
    return 0U;
  }

  taskENTER_CRITICAL();
  *euler = debug_imu_euler;
  valid = debug_imu_valid;
  taskEXIT_CRITICAL();

  return valid;
}

static void UpperArm_WaitImuReady(uint32_t stagger_ms)
{
  while (imu_solver_ready == 0U)
  {
    osDelay(20U);
  }

  if (stagger_ms != 0U)
  {
    osDelay(stagger_ms);
  }
}

static uint8_t UpperArm_RtcInit(void)
{
  RTC_PrivilegeStateTypeDef privilege_state = {0};

  if (upper_rtc_ready != 0U)
  {
    return 1U;
  }

  HAL_PWR_EnableBkUpAccess();

  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  /* RTC 时钟源由 HAL_RTC_MspInit 配置为 LSI，标称频率为 32 kHz。
   * 128 * 250 分频后得到 1 Hz；255 是 32.768 kHz LSE 的配置，
   * 用在 LSI 上会使 RTC 每秒实际走约 1.024 秒，造成持续慢走。 */
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 249;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Init.OutPutPullUp = RTC_OUTPUT_PULLUP_NONE;
  hrtc.Init.BinMode = RTC_BINARY_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    return 0U;
  }

  privilege_state.rtcPrivilegeFull = RTC_PRIVILEGE_FULL_NO;
  privilege_state.backupRegisterPrivZone = RTC_PRIVILEGE_BKUP_ZONE_NONE;
  privilege_state.backupRegisterStartZone2 = RTC_BKP_DR0;
  privilege_state.backupRegisterStartZone3 = RTC_BKP_DR0;
  (void)HAL_RTCEx_PrivilegeModeSet(&hrtc, &privilege_state);

  upper_rtc_ready = 1U;
  if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0) == UPPER_RTC_BACKUP_MAGIC)
  {
    upper_time_valid = 1U;
  }
  return 1U;
}

static uint8_t UpperArm_RtcSetTime(uint8_t hour, uint8_t minute, uint8_t second)
{
  RTC_TimeTypeDef rtc_time = {0};
  RTC_DateTypeDef rtc_date = {0};

  if ((hour > 23U) || (minute > 59U) || (second > 59U))
  {
    return 0U;
  }

  if ((upper_rtc_ready == 0U) && (UpperArm_RtcInit() == 0U))
  {
    return 0U;
  }

  rtc_time.Hours = hour;
  rtc_time.Minutes = minute;
  rtc_time.Seconds = second;
  rtc_time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  rtc_time.StoreOperation = RTC_STOREOPERATION_RESET;

  rtc_date.WeekDay = RTC_WEEKDAY_MONDAY;
  rtc_date.Month = RTC_MONTH_JANUARY;
  rtc_date.Date = 1U;
  rtc_date.Year = 26U;

  if (HAL_RTC_SetTime(&hrtc, &rtc_time, RTC_FORMAT_BIN) != HAL_OK)
  {
    return 0U;
  }
  if (HAL_RTC_SetDate(&hrtc, &rtc_date, RTC_FORMAT_BIN) != HAL_OK)
  {
    return 0U;
  }

  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, UPPER_RTC_BACKUP_MAGIC);
  upper_time_valid = 1U;
  return 1U;
}

static uint8_t UpperArm_RtcGetTime(uint8_t *hour, uint8_t *minute, uint8_t *second)
{
  RTC_TimeTypeDef rtc_time = {0};
  RTC_DateTypeDef rtc_date = {0};

  if ((hour == NULL) || (minute == NULL) || (second == NULL))
  {
    return 0U;
  }

  if ((upper_rtc_ready == 0U) || (upper_time_valid == 0U))
  {
    return 0U;
  }

  if (HAL_RTC_GetTime(&hrtc, &rtc_time, RTC_FORMAT_BIN) != HAL_OK)
  {
    return 0U;
  }
  (void)HAL_RTC_GetDate(&hrtc, &rtc_date, RTC_FORMAT_BIN);

  *hour = rtc_time.Hours;
  *minute = rtc_time.Minutes;
  *second = rtc_time.Seconds;
  return 1U;
}

static uint32_t UpperArm_ParseU32After(const char *text, const char *key)
{
  const char *p;
  uint32_t value = 0U;

  if ((text == NULL) || (key == NULL))
  {
    return 0U;
  }

  p = strstr(text, key);
  if (p == NULL)
  {
    return 0U;
  }
  p += strlen(key);

  while ((*p >= '0') && (*p <= '9'))
  {
    value = (value * 10U) + (uint32_t)(*p - '0');
    p++;
  }

  return value;
}

static uint8_t UpperArm_TimeSyncEpoch(uint32_t epoch)
{
  uint32_t local_day_sec;

  if (epoch == 0U)
  {
    return 0U;
  }

  upper_time_epoch_at_sync = epoch;
  upper_time_tick_at_sync = osKernelGetTickCount();
  upper_time_valid = 1U;

  local_day_sec = (epoch + (8U * 3600U)) % 86400U;
  return UpperArm_RtcSetTime((uint8_t)(local_day_sec / 3600U),
                             (uint8_t)((local_day_sec / 60U) % 60U),
                             (uint8_t)(local_day_sec % 60U));
}

static uint8_t UpperArm_GetClock(uint8_t *hour, uint8_t *minute, uint8_t *second)
{
  uint32_t epoch;
  uint32_t elapsed_sec;
  uint32_t day_sec;
  uint32_t tick_now;

  if ((hour == NULL) || (minute == NULL) || (second == NULL))
  {
    return 0U;
  }

  tick_now = osKernelGetTickCount();
  if (UpperArm_RtcGetTime(hour, minute, second) != 0U)
  {
    return 1U;
  }

  if (upper_time_valid != 0U)
  {
    elapsed_sec = (tick_now - upper_time_tick_at_sync) / 1000U;
    epoch = upper_time_epoch_at_sync + elapsed_sec;
    day_sec = (epoch + (8U * 3600U)) % 86400U;
  }
  else
  {
    day_sec = (tick_now / 1000U) % 86400U;
  }

  *hour = (uint8_t)(day_sec / 3600U);
  *minute = (uint8_t)((day_sec / 60U) % 60U);
  *second = (uint8_t)(day_sec % 60U);
  return upper_time_valid;
}

static uint8_t Esp8266_ProcessResponseForTime(const char *response)
{
  uint32_t epoch;
  const char *p;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;

  if (response == NULL)
  {
    return 0U;
  }

  epoch = UpperArm_ParseU32After(response, "epoch=");
  if (epoch != 0U)
  {
    return UpperArm_TimeSyncEpoch(epoch);
  }

  if (strstr(response, "1970") != NULL)
  {
    return 0U;
  }

  for (p = response; *p != '\0'; p++)
  {
    if (((p[0] >= '0') && (p[0] <= '9')) &&
        ((p[1] >= '0') && (p[1] <= '9')) &&
        (p[2] == ':') &&
        ((p[3] >= '0') && (p[3] <= '9')) &&
        ((p[4] >= '0') && (p[4] <= '9')) &&
        (p[5] == ':') &&
        ((p[6] >= '0') && (p[6] <= '9')) &&
        ((p[7] >= '0') && (p[7] <= '9')))
    {
      hour = (uint8_t)(((uint8_t)(p[0] - '0') * 10U) + (uint8_t)(p[1] - '0'));
      minute = (uint8_t)(((uint8_t)(p[3] - '0') * 10U) + (uint8_t)(p[4] - '0'));
      second = (uint8_t)(((uint8_t)(p[6] - '0') * 10U) + (uint8_t)(p[7] - '0'));
      return UpperArm_RtcSetTime(hour, minute, second);
    }
  }
  return 0U;
}

#include "upper_ui_pages.inc"

static uint8_t Esp8266_ShouldPause(void)
{
  return 0U;
}

static void Esp8266_WriteBuffer(const char *buffer, uint16_t length, uint32_t timeout_ms)
{
  if ((buffer == NULL) || (length == 0U))
  {
    return;
  }
  if (Esp8266_ShouldPause() != 0U)
  {
    return;
  }

  if (uart2MutexHandle != NULL)
  {
    (void)osMutexAcquire(uart2MutexHandle, osWaitForever);
  }
  (void)HAL_UART_Transmit(&huart2, (uint8_t *)buffer, length, timeout_ms);
  if (uart2MutexHandle != NULL)
  {
    (void)osMutexRelease(uart2MutexHandle);
  }
}

static uint16_t Esp8266_ReadResponse(char *buffer, uint16_t buffer_len, uint32_t timeout_ms)
{
  uint32_t start;
  uint16_t len = 0U;
  uint8_t ch;

  if ((buffer == NULL) || (buffer_len == 0U))
  {
    return 0U;
  }
  if (Esp8266_ShouldPause() != 0U)
  {
    buffer[0] = '\0';
    return 0U;
  }

  if (uart2MutexHandle != NULL)
  {
    (void)osMutexAcquire(uart2MutexHandle, osWaitForever);
  }

  start = osKernelGetTickCount();
  while (((osKernelGetTickCount() - start) < timeout_ms) && ((len + 1U) < buffer_len))
  {
    if (Esp8266_ShouldPause() != 0U)
    {
      break;
    }
    if (HAL_UART_Receive(&huart2, &ch, 1U, 1U) == HAL_OK)
    {
      buffer[len++] = (char)ch;
    }
    else
    {
      osDelay(1U);
    }
  }
  buffer[len] = '\0';

  if (uart2MutexHandle != NULL)
  {
    (void)osMutexRelease(uart2MutexHandle);
  }

  return len;
}

static void Esp8266_SendCommand(const char *command, uint32_t wait_ms)
{
  uint32_t start_tick;

  if (Esp8266_ShouldPause() != 0U)
  {
    return;
  }
  Esp8266_WriteBuffer(command, (uint16_t)strlen(command), 50U);
  start_tick = osKernelGetTickCount();
  while ((wait_ms != 0U) && ((osKernelGetTickCount() - start_tick) < wait_ms))
  {
    if (Esp8266_ShouldPause() != 0U)
    {
      break;
    }
    osDelay(20U);
  }
}

static void Esp8266_SyncTimeFromSntp(void)
{
  char response[256];
  uint8_t retry;

  Esp8266_SendCommand("AT+CIPSNTPCFG=1,8,\"ntp.aliyun.com\",\"cn.ntp.org.cn\",\"pool.ntp.org\"\r\n", 1000U);
  (void)Esp8266_ReadResponse(response, (uint16_t)sizeof(response), 300U);

  for (retry = 0U; retry < 3U; retry++)
  {
    Esp8266_SendCommand("AT+CIPSNTPTIME?\r\n", 500U);
    if (Esp8266_ReadResponse(response, (uint16_t)sizeof(response), 1500U) != 0U)
    {
      if (Esp8266_ProcessResponseForTime(response) != 0U)
      {
        return;
      }
    }
    osDelay(1000U);
  }
}

static uint8_t Esp8266_SyncTimeFromWeb(void)
{
  char command[128];
  char request[128];
  char response[256];
  uint16_t request_len;
  uint8_t synced = 0U;

  Esp8266_SendCommand("AT+CIPCLOSE\r\n", 150U);
  (void)snprintf(command,
                 sizeof(command),
                 "AT+CIPSTART=\"TCP\",\"%s\",%u\r\n",
                 ESP8266_MQTT_HOST,
                 (unsigned int)ESP8266_HTTP_PORT);
  Esp8266_SendCommand(command, 1000U);

  request_len = (uint16_t)snprintf(request,
                                   sizeof(request),
                                   "GET /api/device-time HTTP/1.0\r\nHost: %s\r\nConnection: close\r\n\r\n",
                                   ESP8266_MQTT_HOST);
  (void)snprintf(command, sizeof(command), "AT+CIPSEND=%u\r\n", (unsigned int)request_len);
  Esp8266_SendCommand(command, 200U);
  Esp8266_WriteBuffer(request, request_len, 50U);

  if (Esp8266_ReadResponse(response, (uint16_t)sizeof(response), 2500U) != 0U)
  {
    synced = Esp8266_ProcessResponseForTime(response);
  }
  Esp8266_SendCommand("AT+CIPCLOSE\r\n", 150U);
  return synced;
}

static const char *Esp8266_PageNameFor(UpperPage_t page)
{
  switch (page)
  {
    case UPPER_PAGE_HOME: return "home";
    case UPPER_PAGE_PREP: return "prep";
    case UPPER_PAGE_ACTION_SELECT: return "action";
    case UPPER_PAGE_TRAINING: return "training";
    case UPPER_PAGE_RESULT: return "result";
    case UPPER_PAGE_SETTINGS: return "settings";
    case UPPER_PAGE_HISTORY: return "history";
    case UPPER_PAGE_ANALYTICS: return "analytics";
    case UPPER_PAGE_LINK_LOST: return "link_lost";
    case UPPER_PAGE_FREE_TRAINING: return "free_training";
    case UPPER_PAGE_FREE_RESULT: return "free_result";
    default: return "unknown";
  }
}

static const char *Esp8266_PageName(void)
{
  return Esp8266_PageNameFor(current_ui_page);
}

static uint8_t Esp8266_BuildPayloadForPage(char *payload, size_t payload_len, UpperPage_t page)
{
  WirelessWristFrame_t wrist;
  uint8_t upper_battery;
  uint8_t upper_battery_valid;
  uint8_t wrist_valid;
  uint32_t wrist_last_tick;
  uint8_t upper_bt_connected;
  uint8_t wrist_bt_connected;
  uint8_t upper_sleeping;
  uint8_t wrist_sleeping;
  uint8_t wrist_health_active;
  uint8_t wrist_health_enter;
  uint8_t wrist_health_exit;
  uint8_t health_request_active;
  uint8_t upper_page_event;
  uint8_t wrist_battery;
  uint8_t wrist_battery_valid;
  EspTrainingPayloadSnapshot_t train_snap;
  uint32_t wrist_age;
  uint32_t now;

  if ((payload == NULL) || (payload_len == 0U))
  {
    return 0U;
  }

  now = osKernelGetTickCount();
  taskENTER_CRITICAL();
  wrist = latest_wrist_frame;
  wrist_valid = latest_wrist_valid;
  wrist_last_tick = wireless_last_frame_tick;
  upper_battery = battery_cached_percent;
  upper_battery_valid = battery_cached_valid;
  train_snap = esp_training_payload_snapshot;
  if (upper_page_result_event_frames != 0U)
  {
    upper_page_event = 1U;
    upper_page_result_event_frames--;
  }
  else if (upper_page_home_event_frames != 0U)
  {
    upper_page_event = 2U;
    upper_page_home_event_frames--;
  }
  else
  {
    upper_page_event = 0U;
  }
  taskEXIT_CRITICAL();
  upper_bt_connected = UpperArm_WirelessIsConnected(now);
  wrist_age = (wrist_last_tick == 0U) ? 999999UL : (now - wrist_last_tick);
  wrist_bt_connected = ((wrist_valid != 0U) && ((wrist.status2 & WIRELESS_STATUS2_BT_OK) != 0U)) ? 1U : 0U;
  upper_sleeping = UpperArm_IsScreenOff();
  wrist_sleeping = ((wrist_valid != 0U) && ((wrist.status2 & WIRELESS_STATUS2_SLEEPING) != 0U)) ? 1U : 0U;
  wrist_health_active = ((wrist_valid != 0U) && ((wrist.status2 & WIRELESS_STATUS2_HEALTH_MODE) != 0U)) ? 1U : 0U;
  wrist_health_enter = ((wrist_valid != 0U) && ((wrist.status2 & WIRELESS_STATUS2_HEALTH_ENTER) != 0U)) ? 1U : 0U;
  wrist_health_exit = ((wrist_valid != 0U) && ((wrist.status2 & WIRELESS_STATUS2_HEALTH_EXIT) != 0U)) ? 1U : 0U;
  health_request_active = ((upper_health_request_until_tick != 0U) && ((int32_t)(upper_health_request_until_tick - now) > 0)) ? 1U : 0U;
  wrist_battery = (wrist_valid != 0U) ? wrist.battery_percent : 0U;
  wrist_battery_valid = ((wrist_valid != 0U) && ((wrist.status2 & WIRELESS_STATUS2_BAT_VALID) != 0U)) ? 1U : 0U;
  if ((page != UPPER_PAGE_TRAINING) &&
      (page != UPPER_PAGE_RESULT) &&
      (page != UPPER_PAGE_FREE_TRAINING) &&
      (page != UPPER_PAGE_FREE_RESULT))
  {
    (void)snprintf(payload,
                   payload_len,
                   "t=%lu,pg=%u,ue=%u,ub=%u,wb=%u,uba=%u,wba=%u,wh=%u,whe=%u,whexit=%u,hreq=%u",
                   (unsigned long)now,
                   (unsigned int)page,
                   (unsigned int)upper_page_event,
                   (unsigned int)upper_bt_connected,
                   (unsigned int)wrist_bt_connected,
                   (unsigned int)upper_battery,
                   (unsigned int)wrist_battery,
                   (unsigned int)wrist_health_active,
                   (unsigned int)wrist_health_enter,
                   (unsigned int)wrist_health_exit,
                   (unsigned int)health_request_active);
    return 1U;
  }

  if (train_snap.valid == 0U)
  {
    train_snap.target_reps = training_target_count;
  }
  if ((page == UPPER_PAGE_RESULT) || (page == UPPER_PAGE_FREE_RESULT))
  {
    (void)snprintf(payload,
                   payload_len,
                   "t=%lu,pg=%u,a=%u,n=%u,g=%u,cpl=%u,amp=%u,s=%u,m=%ld,av=%ld,em=%u,emg_strength=%u,emg_mode=%u,emg_label=%s,e=%ld,f=%ld,ab=%ld,sh=%ld",
                   (unsigned long)now,
                   (unsigned int)page,
                   (unsigned int)train_snap.action_index,
                   (unsigned int)train_snap.reps,
                   (unsigned int)train_snap.target_reps,
                   (unsigned int)train_snap.completion_score,
                   (unsigned int)train_snap.amplitude_score,
                   (unsigned int)train_snap.total_score,
                   (long)train_snap.max_angle_deg,
                   (long)train_snap.avg_angle_deg,
                   (unsigned int)train_snap.emg_strength,
                   (unsigned int)train_snap.emg_strength,
                   (unsigned int)train_snap.emg_mode,
                   Emg_ModeLabel(train_snap.emg_mode),
                   (long)train_snap.elbow_deg,
                   (long)train_snap.forearm_deg,
                   (long)train_snap.arm_abd_deg,
                   (long)train_snap.shoulder_deg);
    return 1U;
  }

  (void)snprintf(payload,
                 payload_len,
                 "pg=%u,a=%u,n=%u,g=%u,cpl=%u,amp=%u,ang=%ld,e=%ld,f=%ld,ab=%ld,sh=%ld,em=%u,emg_strength=%u,emg_mode=%u,emg_label=%s",
                 (unsigned int)page,
                 (unsigned int)train_snap.action_index,
                 (unsigned int)train_snap.reps,
                 (unsigned int)train_snap.target_reps,
                 (unsigned int)train_snap.completion_score,
                 (unsigned int)train_snap.amplitude_score,
                 (long)train_snap.current_angle_deg,
                 (long)train_snap.elbow_deg,
                 (long)train_snap.forearm_deg,
                 (long)train_snap.arm_abd_deg,
                 (long)train_snap.shoulder_deg,
                 (unsigned int)train_snap.emg_strength,
                 (unsigned int)train_snap.emg_strength,
                 (unsigned int)train_snap.emg_mode,
                 Emg_ModeLabel(train_snap.emg_mode));
  return 1U;
}

static uint8_t Esp8266_BuildPayload(char *payload, size_t payload_len)
{
  return Esp8266_BuildPayloadForPage(payload, payload_len, current_ui_page);
}

static uint8_t Esp8266_BuildMinTrainingPayload(char *payload, size_t payload_len)
{
  EspTrainingPayloadSnapshot_t snap;

  if ((payload == NULL) || (payload_len == 0U))
  {
    return 0U;
  }

  taskENTER_CRITICAL();
  snap = esp_training_payload_snapshot;
  taskEXIT_CRITICAL();

  if (snap.valid == 0U)
  {
    return 0U;
  }

  (void)snprintf(payload,
                 payload_len,
                 "pg=%u,a=%u,n=%u,g=%u,cpl=%u,amp=%u,ang=%ld,e=%ld,f=%ld,ab=%ld,sh=%ld,em=%u,emg_strength=%u,emg_mode=%u,emg_label=%s",
                 (unsigned int)UPPER_PAGE_TRAINING,
                 (unsigned int)snap.action_index,
                 (unsigned int)snap.reps,
                 (unsigned int)snap.target_reps,
                 (unsigned int)snap.completion_score,
                 (unsigned int)snap.amplitude_score,
                 (long)snap.current_angle_deg,
                 (long)snap.elbow_deg,
                 (long)snap.forearm_deg,
                 (long)snap.arm_abd_deg,
                 (long)snap.shoulder_deg,
                 (unsigned int)snap.emg_strength,
                 (unsigned int)snap.emg_strength,
                 (unsigned int)snap.emg_mode,
                 Emg_ModeLabel(snap.emg_mode));
  return 1U;
}

static uint8_t Esp8266_BuildLightPayload(char *payload, size_t payload_len)
{
  uint8_t upper_imu_valid;
  uint32_t now;

  if ((payload == NULL) || (payload_len == 0U))
  {
    return 0U;
  }

  now = osKernelGetTickCount();
  taskENTER_CRITICAL();
  upper_imu_valid = debug_imu_valid;
  taskEXIT_CRITICAL();

  (void)snprintf(payload,
                 payload_len,
                 "t=%lu,pg=%u,uv=%u",
                 (unsigned long)now,
                 (unsigned int)current_ui_page,
                 (unsigned int)upper_imu_valid);
  return 1U;
}

static uint8_t Esp8266_BuildHealthPayload(char *payload, size_t payload_len, const WirelessHealthFrame_t *health)
{
  uint32_t now;
  uint32_t health_age;

  if ((payload == NULL) || (payload_len == 0U) || (health == NULL))
  {
    return 0U;
  }

  now = osKernelGetTickCount();
  health_age = (wireless_last_health_tick == 0U) ? 999999UL : (now - wireless_last_health_tick);
  (void)snprintf(payload,
                 payload_len,
                 "tick=%lu,page=health,hv=%u,hseq=%u,hage=%lu,hr=%u,spo2=%u,micro=%u,fatigue=%u,sbp=%u,dbp=%u,sdnn=%u,rmssd=%u",
                 (unsigned long)now,
                 (unsigned int)((health->status & WIRELESS_STATUS_HEART_VALID) != 0U),
                 (unsigned int)health->seq,
                 (unsigned long)health_age,
                 (unsigned int)health->heart_rate,
                 (unsigned int)health->spo2,
                 (unsigned int)health->micro_circ,
                 (unsigned int)health->fatigue,
                 (unsigned int)health->sbp,
                 (unsigned int)health->dbp,
                 (unsigned int)health->sdnn,
                 (unsigned int)health->rmssd);
  return 1U;
}

static uint16_t Mqtt_WriteRemainingLength(uint8_t *buffer, uint32_t length)
{
  uint16_t offset = 0U;

  do
  {
    uint8_t encoded = (uint8_t)(length % 128U);
    length /= 128U;
    if (length > 0U)
    {
      encoded |= 0x80U;
    }
    buffer[offset++] = encoded;
  } while ((length > 0U) && (offset < 4U));

  return offset;
}

static uint16_t Mqtt_WriteUtf8(uint8_t *buffer, const char *text)
{
  uint16_t len;

  if (text == NULL)
  {
    len = 0U;
  }
  else
  {
    len = (uint16_t)strlen(text);
  }

  buffer[0] = (uint8_t)((len >> 8) & 0xFFU);
  buffer[1] = (uint8_t)(len & 0xFFU);
  if (len != 0U)
  {
    memcpy(&buffer[2], text, len);
  }
  return (uint16_t)(len + 2U);
}

static uint16_t Mqtt_BuildConnectPacket(uint8_t *buffer, uint16_t buffer_len)
{
  uint8_t variable[96];
  uint16_t variable_len = 0U;
  uint16_t offset = 0U;

  if ((buffer == NULL) || (buffer_len < 16U))
  {
    return 0U;
  }

  variable_len += Mqtt_WriteUtf8(&variable[variable_len], "MQTT");
  variable[variable_len++] = 0x04U; /* MQTT 3.1.1 */
  variable[variable_len++] = 0x02U; /* Clean session */
  variable[variable_len++] = 0x00U;
  variable[variable_len++] = 0x3CU; /* Keep alive 60s */
  variable_len += Mqtt_WriteUtf8(&variable[variable_len], ESP8266_MQTT_CLIENT_ID);

  if ((uint32_t)variable_len + 5U > buffer_len)
  {
    return 0U;
  }

  buffer[offset++] = 0x10U;
  offset = (uint16_t)(offset + Mqtt_WriteRemainingLength(&buffer[offset], variable_len));
  memcpy(&buffer[offset], variable, variable_len);
  offset = (uint16_t)(offset + variable_len);
  return offset;
}

static uint16_t Mqtt_BuildPublishPacket(uint8_t *buffer,
                                        uint16_t buffer_len,
                                        const char *topic,
                                        const char *payload)
{
  uint16_t topic_len;
  uint16_t payload_len;
  uint32_t remaining_len;
  uint16_t offset = 0U;

  if ((buffer == NULL) || (topic == NULL) || (payload == NULL))
  {
    return 0U;
  }

  topic_len = (uint16_t)strlen(topic);
  payload_len = (uint16_t)strlen(payload);
  remaining_len = (uint32_t)topic_len + 2U + (uint32_t)payload_len;
  if ((remaining_len + 5U) > buffer_len)
  {
    return 0U;
  }

  buffer[offset++] = 0x30U; /* PUBLISH, QoS 0 */
  offset = (uint16_t)(offset + Mqtt_WriteRemainingLength(&buffer[offset], remaining_len));
  offset = (uint16_t)(offset + Mqtt_WriteUtf8(&buffer[offset], topic));
  memcpy(&buffer[offset], payload, payload_len);
  offset = (uint16_t)(offset + payload_len);
  return offset;
}

static void Esp8266_MqttPublish(const char *topic, const char *payload)
{
  char command[96];
  uint8_t packet[768];
  uint16_t packet_len;
  uint32_t now_tick;
  static uint8_t session_ready = 0U;
  static uint32_t session_start_tick = 0U;

  if ((topic == NULL) || (payload == NULL))
  {
    return;
  }
  if (Esp8266_ShouldPause() != 0U)
  {
    return;
  }

  now_tick = osKernelGetTickCount();
  /* 实时数据包之间保持 MQTT/TCP 长连接。旧逻辑每次上传都会关闭并重建连接，
   * 每包会额外增加约 1.47 秒固定延迟；现在每 10 秒主动刷新一次连接，兼顾
   * 传输速度与断线恢复能力。 */
  if ((session_ready == 0U) ||
      ((now_tick - session_start_tick) >= ESP8266_MQTT_SESSION_REFRESH_MS))
  {
    session_ready = 0U;
    Esp8266_SendCommand("AT+CIPCLOSE\r\n", 80U);
    if (Esp8266_ShouldPause() != 0U)
    {
      return;
    }

    (void)snprintf(command,
                   sizeof(command),
                   "AT+CIPSTART=\"TCP\",\"%s\",%u\r\n",
                   ESP8266_MQTT_HOST,
                   (unsigned int)ESP8266_MQTT_PORT);
    Esp8266_SendCommand(command, 500U);
    if (Esp8266_ShouldPause() != 0U)
    {
      return;
    }

    packet_len = Mqtt_BuildConnectPacket(packet, (uint16_t)sizeof(packet));
    if (packet_len == 0U)
    {
      return;
    }
    (void)snprintf(command, sizeof(command), "AT+CIPSEND=%u\r\n", (unsigned int)packet_len);
    Esp8266_SendCommand(command, 100U);
    if (Esp8266_ShouldPause() != 0U)
    {
      return;
    }
    Esp8266_WriteBuffer((const char *)packet, packet_len, 50U);
    osDelay(100U);
    session_ready = 1U;
    session_start_tick = osKernelGetTickCount();
  }

  packet_len = Mqtt_BuildPublishPacket(packet, (uint16_t)sizeof(packet), topic, payload);
  if (packet_len == 0U)
  {
    return;
  }
  (void)snprintf(command, sizeof(command), "AT+CIPSEND=%u\r\n", (unsigned int)packet_len);
  Esp8266_SendCommand(command, 60U);
  if (Esp8266_ShouldPause() != 0U)
  {
    return;
  }
  Esp8266_WriteBuffer((const char *)packet, packet_len, 50U);
  osDelay(30U);
}

static void Esp8266_SimulateLongestPayloadPublish(char *payload, size_t payload_len)
{
  if ((payload == NULL) || (payload_len == 0U))
  {
    return;
  }

  if (Esp8266_BuildPayloadForPage(payload, payload_len, UPPER_PAGE_TRAINING) != 0U)
  {
    Esp8266_MqttPublish(ESP8266_MQTT_TOPIC_REALTIME, payload);
  }
}

static int32_t Rehab_ComputeElbowFlexionX100(int32_t upper_pitch_x100,
                                             int32_t wrist_pitch_x100,
                                             int32_t zero_relative_pitch_x100,
                                             int32_t *relative_pitch_x100)
{
  int32_t relative;
  int32_t flexion;

  relative = Rehab_AngleDiffSignedX100(wrist_pitch_x100, upper_pitch_x100);
  flexion = Rehab_AbsI32(Rehab_AngleDiffSignedX100(relative, zero_relative_pitch_x100));
  if (flexion > 18000)
  {
    flexion = 18000;
  }

  if (relative_pitch_x100 != NULL)
  {
    *relative_pitch_x100 = relative;
  }

  return flexion;
}

static int32_t Rehab_NormalizeSignedX100(int32_t angle_x100)
{
  while (angle_x100 < -18000)
  {
    angle_x100 += 36000;
  }
  while (angle_x100 >= 18000)
  {
    angle_x100 -= 36000;
  }
  return angle_x100;
}

static int32_t Rehab_MinI32(int32_t a, int32_t b)
{
  return (a < b) ? a : b;
}

static const char *Rehab_ActionName(RehabAction_t action)
{
  switch (action)
  {
    case REHAB_ACTION_ELBOW_FLEX:
      return "ELBOW_FLEX";
    case REHAB_ACTION_ARM_ABDUCTION:
      return "ARM_ABD";
    case REHAB_ACTION_FOREARM_ROTATION:
      return "FOREARM_ROT";
    case REHAB_ACTION_SHOULDER_LIFT:
      return "SHOULDER_LIFT";
    case REHAB_ACTION_IDLE:
    default:
      return "IDLE";
  }
}

static RehabAction_t Rehab_ClassifyRule(const RehabFrame_t *frame, RehabAction_t previous_action)
{
  int32_t wrist_gx_abs;
  int32_t upper_gy_abs;
  int32_t upper_gz_abs;
  uint8_t upper_stable_for_forearm;
#if (REHAB_ACTIVE_EVAL == REHAB_ACTIVE_EVAL_FOREARM_ROT)
  uint8_t forearm_motion_hint;
#endif

  if ((frame == NULL) || (frame->valid == 0U))
  {
    return REHAB_ACTION_IDLE;
  }

  wrist_gx_abs = Rehab_AbsI32(frame->wrist_gyro_x10[0]);
  upper_gy_abs = Rehab_AbsI32(frame->upper_gyro_x10[1]);
  upper_gz_abs = Rehab_AbsI32(frame->upper_gyro_x10[2]);
  upper_stable_for_forearm =
      ((upper_gy_abs < REHAB_FOREARM_UPPER_STABLE_GYRO_X10) &&
       (upper_gz_abs < REHAB_FOREARM_UPPER_STABLE_GYRO_X10) &&
       (frame->arm_abduction_angle_x100 < REHAB_FOREARM_UPPER_STABLE_ANGLE_X100) &&
       (frame->shoulder_lift_angle_x100 < REHAB_FOREARM_UPPER_STABLE_ANGLE_X100)) ? 1U : 0U;

#if (REHAB_ACTIVE_EVAL == REHAB_ACTIVE_EVAL_FOREARM_ROT)
  forearm_motion_hint =
      ((wrist_gx_abs >= REHAB_WRIST_ROT_GYRO_X10) ||
       (frame->forearm_rotation_angle_x100 >= REHAB_FOREARM_ROT_HOLD_ENTER_X100) ||
       ((previous_action == REHAB_ACTION_FOREARM_ROTATION) &&
        (frame->forearm_rotation_angle_x100 >= REHAB_FOREARM_ROT_HOLD_EXIT_X100))) ? 1U : 0U;

  if (frame->forearm_rotation_angle_x100 >= REHAB_FOREARM_ROT_STRONG_X100)
  {
    return REHAB_ACTION_FOREARM_ROTATION;
  }

  if ((forearm_motion_hint != 0U) && (upper_stable_for_forearm != 0U))
  {
    return REHAB_ACTION_FOREARM_ROTATION;
  }

  if (forearm_motion_hint == 0U)
  {
#endif
  if ((frame->elbow_angle_x100 >= REHAB_ELBOW_HOLD_ENTER_X100) ||
      ((previous_action == REHAB_ACTION_ELBOW_FLEX) &&
       (frame->elbow_angle_x100 >= REHAB_ELBOW_HOLD_EXIT_X100)))
  {
    return REHAB_ACTION_ELBOW_FLEX;
  }

  if ((frame->elbow_range_x100 >= REHAB_ELBOW_RANGE_FLEX_X100) ||
      (Rehab_AbsI32(frame->elbow_velocity_x100_s) >= REHAB_ELBOW_VEL_FLEX_X100_S))
  {
    return REHAB_ACTION_ELBOW_FLEX;
  }
#if (REHAB_ACTIVE_EVAL == REHAB_ACTIVE_EVAL_FOREARM_ROT)
  }
#endif
#if (REHAB_ACTIVE_EVAL == REHAB_ACTIVE_EVAL_SHOULDER_LIFT)
  if (upper_gz_abs >= REHAB_SHOULDER_LIFT_GYRO_X10)
  {
    return REHAB_ACTION_SHOULDER_LIFT;
  }
  if ((frame->shoulder_lift_angle_x100 >= REHAB_SHOULDER_HOLD_ENTER_X100) ||
      ((previous_action == REHAB_ACTION_SHOULDER_LIFT) &&
       (frame->shoulder_lift_angle_x100 >= REHAB_SHOULDER_HOLD_EXIT_X100)))
  {
    return REHAB_ACTION_SHOULDER_LIFT;
  }
#endif
#if (REHAB_ACTIVE_EVAL == REHAB_ACTIVE_EVAL_ARM_ABD)
  if ((upper_gy_abs >= REHAB_ARM_ABDUCTION_GYRO_X10) &&
      (frame->arm_abduction_angle_x100 >= REHAB_ARM_ABD_DYNAMIC_MIN_X100))
  {
    return REHAB_ACTION_ARM_ABDUCTION;
  }
#else
  if (upper_gy_abs >= REHAB_ARM_ABDUCTION_GYRO_X10)
  {
    return REHAB_ACTION_ARM_ABDUCTION;
  }
#endif
#if (REHAB_ACTIVE_EVAL == REHAB_ACTIVE_EVAL_ARM_ABD)
  if ((frame->arm_abduction_angle_x100 >= REHAB_ARM_ABD_HOLD_ENTER_X100) ||
      ((previous_action == REHAB_ACTION_ARM_ABDUCTION) &&
       (frame->arm_abduction_angle_x100 >= REHAB_ARM_ABD_HOLD_EXIT_X100)))
  {
    return REHAB_ACTION_ARM_ABDUCTION;
  }
#else
  if (((frame->arm_abduction_angle_x100 >= REHAB_ARM_ABD_HOLD_ENTER_X100) &&
       (frame->shoulder_lift_angle_x100 >= REHAB_ARM_ABD_MIN_SHOULDER_X100)) ||
      ((previous_action == REHAB_ACTION_ARM_ABDUCTION) &&
       (frame->arm_abduction_angle_x100 >= REHAB_ARM_ABD_HOLD_EXIT_X100) &&
       (frame->shoulder_lift_angle_x100 >= REHAB_ARM_ABD_MIN_SHOULDER_EXIT_X100)))
  {
    return REHAB_ACTION_ARM_ABDUCTION;
  }
#endif

#if (REHAB_ACTIVE_EVAL == REHAB_ACTIVE_EVAL_FOREARM_ROT)
  if (forearm_motion_hint == 0U)
  {
#endif
    if (upper_gz_abs >= REHAB_SHOULDER_LIFT_GYRO_X10)
    {
      return REHAB_ACTION_SHOULDER_LIFT;
    }
    if ((frame->shoulder_lift_angle_x100 >= REHAB_SHOULDER_HOLD_ENTER_X100) ||
        ((previous_action == REHAB_ACTION_SHOULDER_LIFT) &&
         (frame->shoulder_lift_angle_x100 >= REHAB_SHOULDER_HOLD_EXIT_X100)))
    {
      return REHAB_ACTION_SHOULDER_LIFT;
    }
#if (REHAB_ACTIVE_EVAL == REHAB_ACTIVE_EVAL_FOREARM_ROT)
  }
#endif

  if ((wrist_gx_abs >= REHAB_WRIST_ROT_GYRO_X10) &&
      (upper_stable_for_forearm != 0U))
  {
    return REHAB_ACTION_FOREARM_ROTATION;
  }
  if ((upper_stable_for_forearm != 0U) &&
      ((frame->forearm_rotation_angle_x100 >= REHAB_FOREARM_ROT_HOLD_ENTER_X100) ||
       ((previous_action == REHAB_ACTION_FOREARM_ROTATION) &&
        (frame->forearm_rotation_angle_x100 >= REHAB_FOREARM_ROT_HOLD_EXIT_X100))))
  {
    return REHAB_ACTION_FOREARM_ROTATION;
  }

  return REHAB_ACTION_IDLE;
}

/* USER CODE END 0 */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for SensorTask */
osThreadId_t SensorTaskHandle;
const osThreadAttr_t SensorTask_attributes = {
  .name = "SensorTask",
  .priority = (osPriority_t) osPriorityRealtime,
  .stack_size = 1024 * 4
};
/* Definitions for AlgoTask */
osThreadId_t AlgoTaskHandle;
const osThreadAttr_t AlgoTask_attributes = {
  .name = "AlgoTask",
  .priority = (osPriority_t) osPriorityAboveNormal,
  .stack_size = 1024 * 4
};
/* Definitions for EmgTask */
osThreadId_t EmgTaskHandle;
const osThreadAttr_t EmgTask_attributes = {
  .name = "EmgTask",
  .priority = (osPriority_t) osPriorityAboveNormal,
  .stack_size = 512 * 4
};
/* Definitions for WirelessTask */
osThreadId_t WirelessTaskHandle;
const osThreadAttr_t WirelessTask_attributes = {
  .name = "WirelessTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 1024 * 4
};
/* Definitions for DataStreamTask */
osThreadId_t DataStreamTaskHandle;
const osThreadAttr_t DataStreamTask_attributes = {
  .name = "DataStreamTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 512 * 4
};
/* Definitions for EspUploadTask */
osThreadId_t EspUploadTaskHandle;
const osThreadAttr_t EspUploadTask_attributes = {
  .name = "EspUploadTask",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 1024 * 4
};
/* Definitions for UiTask */
osThreadId_t UiTaskHandle;
const osThreadAttr_t UiTask_attributes = {
  .name = "UiTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 384 * 4
};
/* Definitions for DisplayTask */
osThreadId_t DisplayTaskHandle;
const osThreadAttr_t DisplayTask_attributes = {
  .name = "DisplayTask",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 2048 * 4
};
/* Definitions for DebugTask */
osThreadId_t DebugTaskHandle;
const osThreadAttr_t DebugTask_attributes = {
  .name = "DebugTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 2048 * 4
};
/* Definitions for uart1Mutex */
osMutexId_t uart1MutexHandle;
const osMutexAttr_t uart1Mutex_attributes = {
  .name = "uart1Mutex"
};
/* Definitions for uart2Mutex */
osMutexId_t uart2MutexHandle;
const osMutexAttr_t uart2Mutex_attributes = {
  .name = "uart2Mutex"
};
/* Definitions for spi3LcdMutex */
osMutexId_t spi3LcdMutexHandle;
const osMutexAttr_t spi3LcdMutex_attributes = {
  .name = "spi3LcdMutex"
};
/* Definitions for rehabStateMutex */
osMutexId_t rehabStateMutexHandle;
const osMutexAttr_t rehabStateMutex_attributes = {
  .name = "rehabStateMutex"
};
/* Definitions for imuFrameQueue */
osMessageQueueId_t imuFrameQueueHandle;
const osMessageQueueAttr_t imuFrameQueue_attributes = {
  .name = "imuFrameQueue"
};
/* Definitions for rehabStateQueue */
osMessageQueueId_t rehabStateQueueHandle;
const osMessageQueueAttr_t rehabStateQueue_attributes = {
  .name = "rehabStateQueue"
};
/* Definitions for debugLogQueue */
osMessageQueueId_t debugLogQueueHandle;
const osMessageQueueAttr_t debugLogQueue_attributes = {
  .name = "debugLogQueue"
};
/* Definitions for imuDmaDoneSem */
osSemaphoreId_t imuDmaDoneSemHandle;
const osSemaphoreAttr_t imuDmaDoneSem_attributes = {
  .name = "imuDmaDoneSem"
};
/* Definitions for adcHalfCpltSem */
osSemaphoreId_t adcHalfCpltSemHandle;
const osSemaphoreAttr_t adcHalfCpltSem_attributes = {
  .name = "adcHalfCpltSem"
};
/* Definitions for adcCpltSem */
osSemaphoreId_t adcCpltSemHandle;
const osSemaphoreAttr_t adcCpltSem_attributes = {
  .name = "adcCpltSem"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  (void)UpperArm_RtcInit();

  /* USER CODE END Init */
  /* creation of uart1Mutex */
  uart1MutexHandle = osMutexNew(&uart1Mutex_attributes);

  /* creation of uart2Mutex */
  uart2MutexHandle = osMutexNew(&uart2Mutex_attributes);

  /* creation of spi3LcdMutex */
  spi3LcdMutexHandle = osMutexNew(&spi3LcdMutex_attributes);

  /* creation of rehabStateMutex */
  rehabStateMutexHandle = osMutexNew(&rehabStateMutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */
  /* creation of imuDmaDoneSem */
  imuDmaDoneSemHandle = osSemaphoreNew(1, 0, &imuDmaDoneSem_attributes);

  /* creation of adcHalfCpltSem */
  adcHalfCpltSemHandle = osSemaphoreNew(1, 0, &adcHalfCpltSem_attributes);

  /* creation of adcCpltSem */
  adcCpltSemHandle = osSemaphoreNew(1, 0, &adcCpltSem_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */
  /* creation of imuFrameQueue */
  imuFrameQueueHandle = osMessageQueueNew (4, 48, &imuFrameQueue_attributes);
  /* creation of rehabStateQueue */
  rehabStateQueueHandle = osMessageQueueNew (2, 32, &rehabStateQueue_attributes);
  /* creation of debugLogQueue */
  debugLogQueueHandle = osMessageQueueNew (8, 64, &debugLogQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
#if !UPPER_IMU_ONLY_TEST && UPPER_DEFAULT_TASK_ENABLE
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);
#endif

  /* creation of SensorTask */
  SensorTaskHandle = osThreadNew(StartSensorTask, NULL, &SensorTask_attributes);

#if !UPPER_IMU_ONLY_TEST
#if UPPER_ALGO_TASK_ENABLE
  /* creation of AlgoTask */
  AlgoTaskHandle = osThreadNew(StartAlgoTask, NULL, &AlgoTask_attributes);
#endif

#if UPPER_EMG_TASK_ENABLE
  /* creation of EmgTask */
  EmgTaskHandle = osThreadNew(StartEmgTask, NULL, &EmgTask_attributes);
#endif

#if UPPER_WIRELESS_TASK_ENABLE
  /* creation of WirelessTask */
  WirelessTaskHandle = osThreadNew(StartWirelessTask, NULL, &WirelessTask_attributes);
#endif

  /* creation of DataStreamTask */
#if EMG_CSV_STREAM_ENABLE
  DataStreamTaskHandle = osThreadNew(StartDataStreamTask, NULL, &DataStreamTask_attributes);
#endif

#if UPPER_WIFI_TASK_ENABLE
  /* creation of EspUploadTask */
  EspUploadTaskHandle = osThreadNew(StartEspUploadTask, NULL, &EspUploadTask_attributes);
#endif

#if UPPER_UI_IDLE_TASK_ENABLE
  /* creation of UiTask */
  UiTaskHandle = osThreadNew(StartUiTask, NULL, &UiTask_attributes);
#endif
#endif

  /* creation of DisplayTask */
  DisplayTaskHandle = osThreadNew(StartDisplayTask, NULL, &DisplayTask_attributes);

#if !UPPER_IMU_ONLY_TEST && UPPER_DEBUG_TASK_ENABLE
  /* creation of DebugTask */
  DebugTaskHandle = osThreadNew(StartDebugTask, NULL, &DebugTask_attributes);
#endif

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}
/* USER CODE BEGIN Header_StartDefaultTask */
/**
* @brief Function implementing the defaultTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN defaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1000U);
  }
  /* USER CODE END defaultTask */
}

/* USER CODE BEGIN Header_StartSensorTask */
/**
* @brief Function implementing the SensorTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartSensorTask */
void StartSensorTask(void *argument)
{
  /* USER CODE BEGIN SensorTask */
  /* This task samples and solves the upper-arm IMU only. */
  LSM6DSR_Data_t imu_data;
  IMUProc_State_t imu_state;
  int32_t gyro_bias_raw[3] = {0};// 闄€铻哄師濮嬮浂鍋忓潎鍊?
  int32_t gyro_bias_sum[3] = {0};// 鏍″噯绱姞鍜?
  uint32_t last_error_tick = 0U;
#if SENSOR_GYRO_DEBUG_PRINT
  uint32_t last_gyro_print_tick = 0U;
  char gyro_debug_line[192];
  int gyro_debug_len;
#endif
  uint16_t calib_count = 0U;
  uint32_t next_sample_tick;

  IMUProc_StateInit(&imu_state, 0.2f);
  IMUProc_AttitudeSetGains(&imu_state.attitude, IMU_ATTITUDE_KP, IMU_ATTITUDE_KI);
//  DebugUart_WriteBuffer("SensorTask UART4 debug start\r\n", sizeof("SensorTask UART4 debug start\r\n") - 1);

  while (LSM6DSR_Init() != HAL_OK)
  {
    if ((osKernelGetTickCount() - last_error_tick) >= 500U)
    {
      last_error_tick = osKernelGetTickCount();
      APP_LOG_PRINTF("LSM start failed, WHO_AM_I=0x%02X\r\n", LSM6DSR_ReadWhoAmI());
    }
    osDelay(100);
  }

  APP_LOG_PRINTF("Upper IMU start OK, keep still for calibration\r\n");

  // 棰勯噰闆嗛潤缃ǔ瀹?
  for (uint16_t i = 0U; i < 100U; i++)
  {
    (void)LSM6DSR_ReadRaw(&imu_data);
    osDelay(5);
  }

  IMUProc_CalibrationReset(&imu_state.calib);
  // 闈欐€侀浂鍋忔牎鍑?
  while (calib_count < 600U)
  {
    if (LSM6DSR_ReadRaw(&imu_data) == HAL_OK)
    {
      gyro_bias_sum[0] += imu_data.gyro_x;
      gyro_bias_sum[1] += imu_data.gyro_y;
      gyro_bias_sum[2] += imu_data.gyro_z;
      calib_count++;
    }
    osDelay(5);
  }

  gyro_bias_raw[0] = gyro_bias_sum[0] / (int32_t)calib_count;
  gyro_bias_raw[1] = gyro_bias_sum[1] / (int32_t)calib_count;
  gyro_bias_raw[2] = gyro_bias_sum[2] / (int32_t)calib_count;
  imu_state.calib.gyro_bias_dps.x = (float)gyro_bias_raw[0] * 0.070f;
  imu_state.calib.gyro_bias_dps.y = (float)gyro_bias_raw[1] * 0.070f;
  imu_state.calib.gyro_bias_dps.z = (float)gyro_bias_raw[2] * 0.070f;
  IMUProc_AttitudeInit(&imu_state.attitude);
  IMUProc_AttitudeSetGains(&imu_state.attitude, IMU_ATTITUDE_KP, IMU_ATTITUDE_KI);
  taskENTER_CRITICAL();
  imu_solver_ready = 1U;
  taskEXIT_CRITICAL();

  APP_LOG_PRINTF("Upper IMU calib OK, gyro_bias_raw=%d,%d,%d sample_raw=%d,%d,%d\r\n",
         (int)gyro_bias_raw[0],
         (int)gyro_bias_raw[1],
         (int)gyro_bias_raw[2],
         (int)imu_data.gyro_x,
         (int)imu_data.gyro_y,
         (int)imu_data.gyro_z);

  next_sample_tick = osKernelGetTickCount() + SENSOR_SAMPLE_PERIOD_MS;

  /* Infinite loop */
  for(;;)
  {
    if (LSM6DSR_ReadRaw(&imu_data) == HAL_OK)
    {
      IMUProc_PrepareRawForUpdate(&imu_data, gyro_bias_raw, IMU_GYRO_DEADBAND_RAW);// 鍑忓幓闆跺亸+姝诲尯杩囨护寰皬鎶栧姩
      IMUProc_StateUpdate(&imu_state, &imu_data, 0.005f);// 濮挎€佽瀺鍚堟洿鏂?

      taskENTER_CRITICAL();
      /* Publish upper-arm IMU data for debug, display, and later relative-pose calculation. */
      debug_imu_raw = imu_data;
      debug_imu_euler = imu_state.attitude.euler;
      debug_imu_valid = 1U;
      taskEXIT_CRITICAL();

#if SENSOR_GYRO_DEBUG_PRINT
      if ((osKernelGetTickCount() - last_gyro_print_tick) >= 200U)
      {
        last_gyro_print_tick = osKernelGetTickCount();
        gyro_debug_len = snprintf(gyro_debug_line,
                                  sizeof(gyro_debug_line),
                                  "UPPER_IMU raw_a=%d,%d,%d acc_g=%.3f,%.3f,%.3f raw_g=%d,%d,%d dps=%.2f,%.2f,%.2f angle=%.2f,%.2f,%.2f\r\n",
                                  (int)imu_data.acc_x,
                                  (int)imu_data.acc_y,
                                  (int)imu_data.acc_z,
                                  (double)imu_data.acc_g_x,
                                  (double)imu_data.acc_g_y,
                                  (double)imu_data.acc_g_z,
                                  (int)imu_data.gyro_x,
                                  (int)imu_data.gyro_y,
                                  (int)imu_data.gyro_z,
                                  (double)imu_data.gyro_dps_x,
                                  (double)imu_data.gyro_dps_y,
                                  (double)imu_data.gyro_dps_z,
                                  (double)imu_state.attitude.euler.roll_deg,
                                  (double)imu_state.attitude.euler.pitch_deg,
                                  (double)imu_state.attitude.euler.yaw_deg);
        DebugUart_WriteBuffer(gyro_debug_line, gyro_debug_len);
      }
#endif

//      if ((osKernelGetTickCount() - last_gyro_print_tick) >= 100U)
//      {
//        last_gyro_print_tick = osKernelGetTickCount();
//        debug_len = snprintf(debug_line,
//                             sizeof(debug_line),
//                             "IMU gyro_dps=%.2f,%.2f,%.2f acc_g=%.3f,%.3f,%.3f angle=%.2f,%.2f,%.2f\r\n",
//                             (double)imu_data.gyro_dps_x,
//                             (double)imu_data.gyro_dps_y,
//                             (double)imu_data.gyro_dps_z,
//                             (double)imu_data.acc_g_x,
//                             (double)imu_data.acc_g_y,
//                             (double)imu_data.acc_g_z,
//                             (double)imu_state.attitude.euler.roll_deg,
//                             (double)imu_state.attitude.euler.pitch_deg,
//                             (double)imu_state.attitude.euler.yaw_deg);
//        DebugUart_WriteBuffer(debug_line, debug_len);
//      }
    }
    else
    {
      if ((osKernelGetTickCount() - last_error_tick) >= 500U)
      {
        last_error_tick = osKernelGetTickCount();
        APP_LOG_PRINTF("LSM read failed\r\n");
      }
    }
    (void)osDelayUntil(next_sample_tick);
    next_sample_tick += SENSOR_SAMPLE_PERIOD_MS;
    if ((int32_t)(osKernelGetTickCount() - next_sample_tick) > (int32_t)SENSOR_SAMPLE_PERIOD_MS)
    {
      next_sample_tick = osKernelGetTickCount() + SENSOR_SAMPLE_PERIOD_MS;
    }
  }
  /* USER CODE END SensorTask */
}

/* USER CODE BEGIN Header_StartAlgoTask */
/**
* @brief Function implementing the AlgoTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartAlgoTask */
void StartAlgoTask(void *argument)
{
  /* USER CODE BEGIN AlgoTask */
  LSM6DSR_Data_t upper_raw;
  IMUProc_Euler_t upper_euler;
  WirelessWristFrame_t wrist_frame;
  EmgSensor_Features_t emg_features;
  RehabFrame_t rehab_frame;
  uint8_t upper_valid;
  uint8_t wrist_valid;
  uint8_t wrist_pose_valid;
  uint32_t now_tick;
  uint32_t last_tick = 0U;
  uint32_t window_start_tick = 0U;
  uint32_t last_print_tick = 0U;
  uint32_t last_train_print_tick = 0U;
  int32_t last_elbow_angle_x100 = 0;
  int32_t window_min_elbow_x100 = 0;
  int32_t window_max_elbow_x100 = 0;
  int32_t elbow_zero_relative_pitch_x100 = 0;
  int32_t forearm_zero_roll_x100 = 0;
  int32_t arm_abd_zero_upper_y_x100 = 0;
  int32_t arm_abd_zero_wrist_y_x100 = 0;
  int32_t shoulder_zero_z_x100 = 0;
  uint8_t have_last_angle = 0U;
  uint8_t forearm_upper_stable = 0U;
  uint8_t window_initialized = 0U;
  uint8_t elbow_zero_valid = 0U;
  RehabAction_t previous_rule_action = REHAB_ACTION_IDLE;
  RehabAction_t latched_action = REHAB_ACTION_IDLE;
  RehabAction_t rule_action = REHAB_ACTION_IDLE;
  ForearmRotEvalConfig_t forearm_eval_config;
  ForearmRotEvalSample_t forearm_eval_sample;
  ForearmRotEvalState_t forearm_eval_state;
  ArmAbdEvalConfig_t arm_abd_eval_config;
  ArmAbdEvalSample_t arm_abd_eval_sample;
  ArmAbdEvalState_t arm_abd_eval_state;
  ShoulderLiftEvalConfig_t shoulder_lift_eval_config;
  ShoulderLiftEvalSample_t shoulder_lift_eval_sample;
  ShoulderLiftEvalState_t shoulder_lift_eval_state;
  ElbowFlexEvalConfig_t elbow_eval_config;
  ElbowFlexEvalSample_t elbow_eval_sample;
  ElbowFlexEvalState_t elbow_eval_state;
  uint32_t latched_action_tick = 0U;
  uint8_t free_motion_active[4] = {0U, 0U, 0U, 0U};
  int32_t free_motion_peak_x100[4] = {0, 0, 0, 0};
  uint32_t free_last_count_tick[4] = {0U, 0U, 0U, 0U};

  (void)argument;
  UpperArm_WaitImuReady(100U);
  ForearmRotEval_Init();
  ForearmRotEval_DefaultConfig(&forearm_eval_config);
  forearm_eval_config.target_reps = training_target_count;
  ForearmRotEval_Start(&forearm_eval_config);
  ArmAbdEval_Init();
  ArmAbdEval_DefaultConfig(&arm_abd_eval_config);
  arm_abd_eval_config.target_reps = training_target_count;
  ArmAbdEval_Start(&arm_abd_eval_config);
  ShoulderLiftEval_Init();
  ShoulderLiftEval_DefaultConfig(&shoulder_lift_eval_config);
  shoulder_lift_eval_config.target_reps = training_target_count;
  ShoulderLiftEval_Start(&shoulder_lift_eval_config);
  ElbowFlexEval_Init();
  ElbowFlexEval_DefaultConfig(&elbow_eval_config);
  elbow_eval_config.target_reps = training_target_count;
  ElbowFlexEval_Start(&elbow_eval_config);
  APP_LOG_PRINTF("TRAIN_EVAL runtime dispatch ready\r\n");

  /* Infinite loop */
  for(;;)
  {
    taskENTER_CRITICAL();
    upper_raw = debug_imu_raw;
    upper_euler = debug_imu_euler;
    upper_valid = debug_imu_valid;
    wrist_frame = latest_wrist_frame;
    wrist_valid = latest_wrist_valid;
    taskEXIT_CRITICAL();

    EmgSensor_GetFeatures(&emg_features);
    now_tick = osKernelGetTickCount();

    /* EMG is a local, independent sensor. Keep the LVGL/MQTT snapshot fresh
     * even when the upper IMU or wrist pose is temporarily invalid. */
    if ((current_ui_page == UPPER_PAGE_TRAINING) ||
        (current_ui_page == UPPER_PAGE_FREE_TRAINING))
    {
      taskENTER_CRITICAL();
      esp_training_payload_snapshot.emg_strength = emg_features.strength;
      esp_training_payload_snapshot.emg_mode = emg_features.mode;
      taskEXIT_CRITICAL();
    }

    if ((current_ui_page != UPPER_PAGE_TRAINING) &&
        (current_ui_page != UPPER_PAGE_FREE_TRAINING))
    {
      have_last_angle = 0U;
      window_initialized = 0U;
      elbow_zero_valid = 0U;
      previous_rule_action = REHAB_ACTION_IDLE;
      latched_action = REHAB_ACTION_IDLE;
      rule_action = REHAB_ACTION_IDLE;
      memset(free_motion_active, 0, sizeof(free_motion_active));
      memset(free_motion_peak_x100, 0, sizeof(free_motion_peak_x100));
      memset(free_last_count_tick, 0, sizeof(free_last_count_tick));
      taskENTER_CRITICAL();
      latest_rehab_valid = 0U;
      training_live_reps = 0U;
      taskEXIT_CRITICAL();
      osDelay(REHAB_ALGO_PERIOD_MS);
      continue;
    }

    if (training_eval_reset_request != 0U)
    {
      taskENTER_CRITICAL();
      training_eval_reset_request = 0U;
      training_live_reps = 0U;
      latest_rehab_valid = 0U;
      memset(&training_eval_snapshot, 0, sizeof(training_eval_snapshot));
      taskEXIT_CRITICAL();

      last_tick = 0U;
      window_start_tick = 0U;
      last_train_print_tick = 0U;
      last_elbow_angle_x100 = 0;
      window_min_elbow_x100 = 0;
      window_max_elbow_x100 = 0;
      elbow_zero_relative_pitch_x100 = 0;
      forearm_zero_roll_x100 = 0;
      arm_abd_zero_upper_y_x100 = 0;
      arm_abd_zero_wrist_y_x100 = 0;
      shoulder_zero_z_x100 = 0;
      have_last_angle = 0U;
      window_initialized = 0U;
      elbow_zero_valid = 0U;
      previous_rule_action = REHAB_ACTION_IDLE;
      latched_action = REHAB_ACTION_IDLE;
      rule_action = REHAB_ACTION_IDLE;
      latched_action_tick = 0U;
      memset(free_motion_active, 0, sizeof(free_motion_active));
      memset(free_motion_peak_x100, 0, sizeof(free_motion_peak_x100));
      memset(free_last_count_tick, 0, sizeof(free_last_count_tick));

      ForearmRotEval_DefaultConfig(&forearm_eval_config);
      forearm_eval_config.target_reps = training_target_count;
      ForearmRotEval_Start(&forearm_eval_config);
      ArmAbdEval_DefaultConfig(&arm_abd_eval_config);
      arm_abd_eval_config.target_reps = training_target_count;
      ArmAbdEval_Start(&arm_abd_eval_config);
      ShoulderLiftEval_DefaultConfig(&shoulder_lift_eval_config);
      shoulder_lift_eval_config.target_reps = training_target_count;
      ShoulderLiftEval_Start(&shoulder_lift_eval_config);
      ElbowFlexEval_DefaultConfig(&elbow_eval_config);
      elbow_eval_config.target_reps = training_target_count;
      ElbowFlexEval_Start(&elbow_eval_config);
    }

    wrist_pose_valid = ((wrist_valid != 0U) && ((wrist_frame.status & 0x01U) != 0U)) ? 1U : 0U;

    memset(&rehab_frame, 0, sizeof(rehab_frame));
    rehab_frame.valid = (upper_valid != 0U) ? 1U : 0U;

    if (rehab_frame.valid != 0U)
    {
      rehab_frame.upper_acc_mg[0] = Rehab_FloatToI16Scaled(upper_raw.acc_g_x, 1000.0f);
      rehab_frame.upper_acc_mg[1] = Rehab_FloatToI16Scaled(upper_raw.acc_g_y, 1000.0f);
      rehab_frame.upper_acc_mg[2] = Rehab_FloatToI16Scaled(upper_raw.acc_g_z, 1000.0f);
      rehab_frame.upper_gyro_x10[0] = Rehab_FloatToI16Scaled(upper_raw.gyro_dps_x, 10.0f);
      rehab_frame.upper_gyro_x10[1] = Rehab_FloatToI16Scaled(upper_raw.gyro_dps_y, 10.0f);
      rehab_frame.upper_gyro_x10[2] = Rehab_FloatToI16Scaled(upper_raw.gyro_dps_z, 10.0f);
      rehab_frame.upper_angle_x100[0] = Rehab_AngleToSignedX100(upper_euler.roll_deg);
      rehab_frame.upper_angle_x100[1] = Rehab_AngleToSignedX100(upper_euler.pitch_deg);
      rehab_frame.upper_angle_x100[2] = Rehab_AngleToSignedX100(upper_euler.yaw_deg);

      if (wrist_pose_valid != 0U)
      {
        for (uint8_t i = 0U; i < 3U; i++)
        {
          rehab_frame.wrist_acc_mg[i] = wrist_frame.acc_mg[i];
          rehab_frame.wrist_gyro_x10[i] = wrist_frame.gyro_dps_x10[i];
          rehab_frame.wrist_angle_x100[i] = wrist_frame.angle_x100[i];
        }
        rehab_frame.wrist_heart_rate = wrist_frame.heart_rate;
      }

      if ((elbow_zero_valid == 0U) && (wrist_pose_valid != 0U))
      {
        elbow_zero_relative_pitch_x100 =
            Rehab_AngleDiffSignedX100(rehab_frame.wrist_angle_x100[1],
                                      rehab_frame.upper_angle_x100[1]);
        forearm_zero_roll_x100 = rehab_frame.wrist_angle_x100[0];
        arm_abd_zero_upper_y_x100 = rehab_frame.upper_angle_x100[1];
        arm_abd_zero_wrist_y_x100 = rehab_frame.wrist_angle_x100[1];
        shoulder_zero_z_x100 = rehab_frame.upper_angle_x100[2];
        elbow_zero_valid = 1U;
        APP_LOG_PRINTF("REHAB posture zero calibrated elbow_rel=%ld.%02ld wrist_roll=%ld.%02ld arm_abd_uy=%ld.%02ld arm_abd_wy=%ld.%02ld shoulder_z=%ld.%02ld deg\r\n",
               (long)(elbow_zero_relative_pitch_x100 / 100),
               (long)Rehab_AbsI32(elbow_zero_relative_pitch_x100 % 100),
               (long)(forearm_zero_roll_x100 / 100),
               (long)Rehab_AbsI32(forearm_zero_roll_x100 % 100),
               (long)(arm_abd_zero_upper_y_x100 / 100),
               (long)Rehab_AbsI32(arm_abd_zero_upper_y_x100 % 100),
               (long)(arm_abd_zero_wrist_y_x100 / 100),
               (long)Rehab_AbsI32(arm_abd_zero_wrist_y_x100 % 100),
               (long)(shoulder_zero_z_x100 / 100),
               (long)Rehab_AbsI32(shoulder_zero_z_x100 % 100));
      }

      rehab_frame.elbow_angle_x100 =
          Rehab_ComputeElbowFlexionX100(rehab_frame.upper_angle_x100[1],
                                        rehab_frame.wrist_angle_x100[1],
                                        elbow_zero_relative_pitch_x100,
                                        &rehab_frame.elbow_relative_pitch_x100);
      rehab_frame.forearm_rotation_angle_x100 =
          Rehab_AngleDiffAbsX100(rehab_frame.wrist_angle_x100[0], forearm_zero_roll_x100);
      if (wrist_pose_valid != 0U)
      {
        int32_t upper_y_delta_x100 =
            Rehab_AngleDiffAbsX100(rehab_frame.upper_angle_x100[1], arm_abd_zero_upper_y_x100);
        int32_t wrist_y_delta_x100 =
            Rehab_AngleDiffAbsX100(rehab_frame.wrist_angle_x100[1], arm_abd_zero_wrist_y_x100);
        rehab_frame.arm_abduction_angle_x100 = Rehab_MinI32(upper_y_delta_x100, wrist_y_delta_x100);
      }
      else
      {
        rehab_frame.arm_abduction_angle_x100 = 0;
      }
      rehab_frame.shoulder_lift_angle_x100 =
          Rehab_AngleDiffAbsX100(rehab_frame.upper_angle_x100[2], shoulder_zero_z_x100);

      if ((have_last_angle != 0U) && (now_tick > last_tick))
      {
        rehab_frame.elbow_velocity_x100_s =
            ((rehab_frame.elbow_angle_x100 - last_elbow_angle_x100) * 1000) /
            (int32_t)(now_tick - last_tick);
      }
      else
      {
        rehab_frame.elbow_velocity_x100_s = 0;
        have_last_angle = 1U;
      }

      last_elbow_angle_x100 = rehab_frame.elbow_angle_x100;
      last_tick = now_tick;

      if ((window_initialized == 0U) || ((now_tick - window_start_tick) >= REHAB_WINDOW_MS))
      {
        window_start_tick = now_tick;
        window_min_elbow_x100 = rehab_frame.elbow_angle_x100;
        window_max_elbow_x100 = rehab_frame.elbow_angle_x100;
        window_initialized = 1U;
      }
      else
      {
        if (rehab_frame.elbow_angle_x100 < window_min_elbow_x100)
        {
          window_min_elbow_x100 = rehab_frame.elbow_angle_x100;
        }
        if (rehab_frame.elbow_angle_x100 > window_max_elbow_x100)
        {
          window_max_elbow_x100 = rehab_frame.elbow_angle_x100;
        }
      }

      rehab_frame.elbow_range_x100 = window_max_elbow_x100 - window_min_elbow_x100;
#if EMG_FLEX_ANGLE_FALLBACK_ENABLE
      Emg_ApplyFlexRelaxGuard(selected_action_index, rehab_frame.elbow_angle_x100, &emg_features);
#endif
      rehab_frame.emg_rms_x10 = emg_features.rms_x10;
      rehab_frame.emg_active = emg_features.active;
      forearm_upper_stable =
          ((Rehab_AbsI32(rehab_frame.upper_gyro_x10[1]) < REHAB_FOREARM_UPPER_STABLE_GYRO_X10) &&
           (Rehab_AbsI32(rehab_frame.upper_gyro_x10[2]) < REHAB_FOREARM_UPPER_STABLE_GYRO_X10) &&
           (rehab_frame.arm_abduction_angle_x100 < REHAB_FOREARM_UPPER_STABLE_ANGLE_X100) &&
           (rehab_frame.shoulder_lift_angle_x100 < REHAB_FOREARM_UPPER_STABLE_ANGLE_X100)) ? 1U : 0U;
      rule_action = Rehab_ClassifyRule(&rehab_frame, previous_rule_action);

      if (rule_action != REHAB_ACTION_IDLE)
      {
        latched_action = rule_action;
        latched_action_tick = now_tick;
      }
      else if ((latched_action != REHAB_ACTION_IDLE) &&
               ((now_tick - latched_action_tick) <= REHAB_ACTION_LATCH_MS))
      {
        rule_action = latched_action;
      }
      else
      {
        latched_action = REHAB_ACTION_IDLE;
      }
      previous_rule_action = rule_action;

      rehab_frame.action = rule_action;
      rehab_frame.level = (rehab_frame.action != REHAB_ACTION_IDLE) ? 1U : 0U;

      if (current_ui_page == UPPER_PAGE_FREE_TRAINING)
      {
        uint8_t action_index = 4U;
        int32_t display_angle_x100 = 0;
        int32_t free_angles_x100[4];
        const int32_t free_enter_x100[4] = {
            REHAB_ELBOW_HOLD_ENTER_X100,
            REHAB_SHOULDER_HOLD_ENTER_X100,
            REHAB_FOREARM_ROT_HOLD_ENTER_X100,
            REHAB_ARM_ABD_HOLD_ENTER_X100
        };
        const int32_t free_exit_x100[4] = {
            REHAB_ELBOW_HOLD_EXIT_X100,
            REHAB_SHOULDER_HOLD_EXIT_X100,
            REHAB_FOREARM_ROT_HOLD_EXIT_X100,
            REHAB_ARM_ABD_HOLD_EXIT_X100
        };

        free_angles_x100[0] = Rehab_AbsI32(rehab_frame.elbow_angle_x100);
        free_angles_x100[1] = Rehab_AbsI32(rehab_frame.shoulder_lift_angle_x100);
        free_angles_x100[2] = Rehab_AbsI32(rehab_frame.forearm_rotation_angle_x100);
        free_angles_x100[3] = Rehab_AbsI32(rehab_frame.arm_abduction_angle_x100);

        if (rehab_frame.action == REHAB_ACTION_ELBOW_FLEX)
        {
          action_index = 0U;
          display_angle_x100 = free_angles_x100[0];
        }
        else if (rehab_frame.action == REHAB_ACTION_ARM_ABDUCTION)
        {
          action_index = 3U;
          display_angle_x100 = free_angles_x100[3];
        }
        else if (rehab_frame.action == REHAB_ACTION_FOREARM_ROTATION)
        {
          action_index = 2U;
          display_angle_x100 = free_angles_x100[2];
        }
        else if (rehab_frame.action == REHAB_ACTION_SHOULDER_LIFT)
        {
          action_index = 1U;
          display_angle_x100 = free_angles_x100[1];
        }

        free_current_action_index = action_index;
        free_current_angle = (uint16_t)(display_angle_x100 / 100);
        if (free_current_angle > free_max_angle)
        {
          free_max_angle = free_current_angle;
        }

        for (uint8_t i = 0U; i < 4U; i++)
        {
          if (free_motion_active[i] == 0U)
          {
            if (free_angles_x100[i] >= free_enter_x100[i])
            {
              free_motion_active[i] = 1U;
              free_motion_peak_x100[i] = free_angles_x100[i];
            }
          }
          else
          {
            if (free_angles_x100[i] > free_motion_peak_x100[i])
            {
              free_motion_peak_x100[i] = free_angles_x100[i];
            }
            if ((free_angles_x100[i] <= free_exit_x100[i]) &&
                (free_motion_peak_x100[i] >= free_enter_x100[i]) &&
                ((now_tick - free_last_count_tick[i]) >= 300U))
            {
              free_motion_active[i] = 0U;
              free_motion_peak_x100[i] = 0;
              free_last_count_tick[i] = now_tick;
              free_action_counts[i]++;
              free_total_reps++;
            }
          }
        }
      }

      if (current_ui_page == UPPER_PAGE_TRAINING)
      {
        if (selected_action_index == 1U)
        {
          shoulder_lift_eval_sample.lift_angle_x100 = rehab_frame.shoulder_lift_angle_x100;
          shoulder_lift_eval_sample.upper_z_gyro_x10 = rehab_frame.upper_gyro_x10[2];
          shoulder_lift_eval_sample.valid = (upper_valid != 0U) ? 1U : 0U;
          shoulder_lift_eval_sample.tick_ms = now_tick;
          ShoulderLiftEval_Update(&shoulder_lift_eval_sample);
          ShoulderLiftEval_GetState(&shoulder_lift_eval_state);
          training_live_reps = shoulder_lift_eval_state.reps;
          taskENTER_CRITICAL();
          training_eval_snapshot.reps = shoulder_lift_eval_state.reps;
          training_eval_snapshot.target_reps = shoulder_lift_eval_state.target_reps;
          training_eval_snapshot.current_angle_x100 = rehab_frame.shoulder_lift_angle_x100;
          training_eval_snapshot.max_angle_x100 = shoulder_lift_eval_state.max_angle_x100;
          training_eval_snapshot.avg_peak_angle_x100 = shoulder_lift_eval_state.avg_peak_angle_x100;
          training_eval_snapshot.completion_score = shoulder_lift_eval_state.completion_score;
          training_eval_snapshot.amplitude_score = shoulder_lift_eval_state.amplitude_score;
          training_eval_snapshot.total_score = shoulder_lift_eval_state.total_score;
          training_eval_snapshot.emg_rate = 0U;
          training_eval_snapshot.valid = 1U;
          taskEXIT_CRITICAL();

          if ((shoulder_lift_eval_state.just_completed_rep != 0U) ||
              ((now_tick - last_train_print_tick) >= 500U))
          {
            last_train_print_tick = now_tick;
            APP_LOG_PRINTF("TRAIN SHOULDER_LIFT rep=%u/%u lift=%ld.%02ld max=%ld.%02ld score=%u upper_rpy=%ld.%02ld/%ld.%02ld/%ld.%02ld\r\n",
                   (unsigned int)shoulder_lift_eval_state.reps,
                   (unsigned int)shoulder_lift_eval_state.target_reps,
                   (long)(rehab_frame.shoulder_lift_angle_x100 / 100),
                   (long)Rehab_AbsI32(rehab_frame.shoulder_lift_angle_x100 % 100),
                   (long)(shoulder_lift_eval_state.max_angle_x100 / 100),
                   (long)Rehab_AbsI32(shoulder_lift_eval_state.max_angle_x100 % 100),
                   (unsigned int)shoulder_lift_eval_state.total_score,
                   (long)(rehab_frame.upper_angle_x100[0] / 100),
                   (long)Rehab_AbsI32(rehab_frame.upper_angle_x100[0] % 100),
                   (long)(rehab_frame.upper_angle_x100[1] / 100),
                   (long)Rehab_AbsI32(rehab_frame.upper_angle_x100[1] % 100),
                   (long)(rehab_frame.upper_angle_x100[2] / 100),
                   (long)Rehab_AbsI32(rehab_frame.upper_angle_x100[2] % 100));
          }
        }
        else if (selected_action_index == 2U)
        {
          forearm_eval_sample.rotation_angle_x100 = rehab_frame.forearm_rotation_angle_x100;
          forearm_eval_sample.wrist_roll_gyro_x10 = rehab_frame.wrist_gyro_x10[0];
          forearm_eval_sample.upper_stable =
              ((forearm_upper_stable != 0U) ||
               (rehab_frame.forearm_rotation_angle_x100 >= REHAB_FOREARM_ROT_STRONG_X100)) ? 1U : 0U;
          forearm_eval_sample.valid = ((upper_valid != 0U) && (wrist_pose_valid != 0U)) ? 1U : 0U;
          forearm_eval_sample.tick_ms = now_tick;
          ForearmRotEval_Update(&forearm_eval_sample);
          ForearmRotEval_GetState(&forearm_eval_state);
          training_live_reps = forearm_eval_state.reps;
          taskENTER_CRITICAL();
          training_eval_snapshot.reps = forearm_eval_state.reps;
          training_eval_snapshot.target_reps = forearm_eval_state.target_reps;
          training_eval_snapshot.current_angle_x100 = forearm_eval_state.current_angle_x100;
          training_eval_snapshot.max_angle_x100 = forearm_eval_state.max_angle_x100;
          training_eval_snapshot.avg_peak_angle_x100 = forearm_eval_state.avg_peak_angle_x100;
          training_eval_snapshot.completion_score = forearm_eval_state.completion_score;
          training_eval_snapshot.amplitude_score = forearm_eval_state.amplitude_score;
          training_eval_snapshot.total_score = forearm_eval_state.total_score;
          training_eval_snapshot.emg_rate = 0U;
          training_eval_snapshot.valid = 1U;
          taskEXIT_CRITICAL();

          if ((forearm_eval_state.just_completed_rep != 0U) ||
              ((now_tick - last_train_print_tick) >= 500U))
          {
            last_train_print_tick = now_tick;
            APP_LOG_PRINTF("TRAIN FOREARM_ROT rep=%u/%u rot=%ld.%02ld max=%ld.%02ld score=%u stable=%u\r\n",
                   (unsigned int)forearm_eval_state.reps,
                   (unsigned int)forearm_eval_state.target_reps,
                   (long)(forearm_eval_state.current_angle_x100 / 100),
                   (long)Rehab_AbsI32(forearm_eval_state.current_angle_x100 % 100),
                   (long)(forearm_eval_state.max_angle_x100 / 100),
                   (long)Rehab_AbsI32(forearm_eval_state.max_angle_x100 % 100),
                   (unsigned int)forearm_eval_state.total_score,
                   (unsigned int)forearm_upper_stable);
          }
        }
        else if (selected_action_index == 3U)
        {
          arm_abd_eval_sample.abduction_angle_x100 = rehab_frame.arm_abduction_angle_x100;
          arm_abd_eval_sample.upper_y_gyro_x10 = rehab_frame.upper_gyro_x10[1];
          arm_abd_eval_sample.valid = ((upper_valid != 0U) && (wrist_pose_valid != 0U)) ? 1U : 0U;
          arm_abd_eval_sample.tick_ms = now_tick;
          ArmAbdEval_Update(&arm_abd_eval_sample);
          ArmAbdEval_GetState(&arm_abd_eval_state);
          training_live_reps = arm_abd_eval_state.reps;
          taskENTER_CRITICAL();
          training_eval_snapshot.reps = arm_abd_eval_state.reps;
          training_eval_snapshot.target_reps = arm_abd_eval_state.target_reps;
          training_eval_snapshot.current_angle_x100 = arm_abd_eval_state.current_angle_x100;
          training_eval_snapshot.max_angle_x100 = arm_abd_eval_state.max_angle_x100;
          training_eval_snapshot.avg_peak_angle_x100 = arm_abd_eval_state.avg_peak_angle_x100;
          training_eval_snapshot.completion_score = arm_abd_eval_state.completion_score;
          training_eval_snapshot.amplitude_score = arm_abd_eval_state.amplitude_score;
          training_eval_snapshot.total_score = arm_abd_eval_state.total_score;
          training_eval_snapshot.emg_rate = 0U;
          training_eval_snapshot.valid = 1U;
          taskEXIT_CRITICAL();

          if ((arm_abd_eval_state.just_completed_rep != 0U) ||
              ((now_tick - last_train_print_tick) >= 500U))
          {
            last_train_print_tick = now_tick;
            APP_LOG_PRINTF("TRAIN ARM_ABD rep=%u/%u abd=%ld.%02ld max=%ld.%02ld score=%u upper_y=%c%ld.%02ld wrist_y=%c%ld.%02ld ugy=%c%ld.%02ld\r\n",
                   (unsigned int)arm_abd_eval_state.reps,
                   (unsigned int)arm_abd_eval_state.target_reps,
                   (long)(arm_abd_eval_state.current_angle_x100 / 100),
                   (long)Rehab_AbsI32(arm_abd_eval_state.current_angle_x100 % 100),
                   (long)(arm_abd_eval_state.max_angle_x100 / 100),
                   (long)Rehab_AbsI32(arm_abd_eval_state.max_angle_x100 % 100),
                   (unsigned int)arm_abd_eval_state.total_score,
                   Rehab_SignCharI32(rehab_frame.upper_angle_x100[1]),
                   (long)(Rehab_AbsI32(rehab_frame.upper_angle_x100[1]) / 100),
                   (long)(Rehab_AbsI32(rehab_frame.upper_angle_x100[1]) % 100),
                   Rehab_SignCharI32(rehab_frame.wrist_angle_x100[1]),
                   (long)(Rehab_AbsI32(rehab_frame.wrist_angle_x100[1]) / 100),
                   (long)(Rehab_AbsI32(rehab_frame.wrist_angle_x100[1]) % 100),
                   Rehab_SignCharI32(rehab_frame.upper_gyro_x10[1]),
                   (long)(Rehab_AbsI32(rehab_frame.upper_gyro_x10[1]) / 10),
                   (long)((Rehab_AbsI32(rehab_frame.upper_gyro_x10[1]) % 10) * 10));
          }
        }
        else
        {
          elbow_eval_sample.flex_angle_x100 = rehab_frame.elbow_angle_x100;
          elbow_eval_sample.emg_active = rehab_frame.emg_active;
          elbow_eval_sample.valid = ((upper_valid != 0U) && (wrist_pose_valid != 0U)) ? 1U : 0U;
          elbow_eval_sample.tick_ms = now_tick;
          ElbowFlexEval_Update(&elbow_eval_sample);
          ElbowFlexEval_GetState(&elbow_eval_state);
          training_live_reps = elbow_eval_state.reps;
          taskENTER_CRITICAL();
          training_eval_snapshot.reps = elbow_eval_state.reps;
          training_eval_snapshot.target_reps = elbow_eval_state.target_reps;
          training_eval_snapshot.current_angle_x100 = elbow_eval_state.current_angle_x100;
          training_eval_snapshot.max_angle_x100 = elbow_eval_state.max_angle_x100;
          training_eval_snapshot.avg_peak_angle_x100 = elbow_eval_state.avg_peak_angle_x100;
          training_eval_snapshot.completion_score = elbow_eval_state.completion_score;
          training_eval_snapshot.amplitude_score = elbow_eval_state.amplitude_score;
          training_eval_snapshot.total_score = elbow_eval_state.total_score;
          training_eval_snapshot.emg_rate = elbow_eval_state.emg_rate;
          training_eval_snapshot.valid = 1U;
          taskEXIT_CRITICAL();

          if ((elbow_eval_state.just_completed_rep != 0U) ||
              ((now_tick - last_train_print_tick) >= 500U))
          {
            last_train_print_tick = now_tick;
            APP_LOG_PRINTF("TRAIN ELBOW_FLEX rep=%u/%u flex=%ld.%02ld max=%ld.%02ld score=%u emg=%u%%\r\n",
                   (unsigned int)elbow_eval_state.reps,
                   (unsigned int)elbow_eval_state.target_reps,
                   (long)(elbow_eval_state.current_angle_x100 / 100),
                   (long)Rehab_AbsI32(elbow_eval_state.current_angle_x100 % 100),
                   (long)(elbow_eval_state.max_angle_x100 / 100),
                   (long)Rehab_AbsI32(elbow_eval_state.max_angle_x100 % 100),
                   (unsigned int)elbow_eval_state.total_score,
                   (unsigned int)elbow_eval_state.emg_rate);
          }
        }
      }

      taskENTER_CRITICAL();
      latest_rehab_frame = rehab_frame;
      latest_rehab_valid = 1U;
      esp_training_payload_snapshot.reps = training_eval_snapshot.reps;
      esp_training_payload_snapshot.target_reps = training_eval_snapshot.target_reps;
      esp_training_payload_snapshot.action_index = selected_action_index;
      esp_training_payload_snapshot.current_angle_deg = (int16_t)(training_eval_snapshot.current_angle_x100 / 100);
      if (esp_training_payload_snapshot.current_angle_deg < 0)
      {
        esp_training_payload_snapshot.current_angle_deg = (int16_t)(-esp_training_payload_snapshot.current_angle_deg);
      }
      esp_training_payload_snapshot.max_angle_deg = (int16_t)(training_eval_snapshot.max_angle_x100 / 100);
      esp_training_payload_snapshot.avg_angle_deg = (int16_t)(training_eval_snapshot.avg_peak_angle_x100 / 100);
      esp_training_payload_snapshot.elbow_deg = (int16_t)(rehab_frame.elbow_angle_x100 / 100);
      esp_training_payload_snapshot.forearm_deg = (int16_t)(rehab_frame.forearm_rotation_angle_x100 / 100);
      esp_training_payload_snapshot.arm_abd_deg = (int16_t)(rehab_frame.arm_abduction_angle_x100 / 100);
      esp_training_payload_snapshot.shoulder_deg = (int16_t)(rehab_frame.shoulder_lift_angle_x100 / 100);
      if (selected_action_index == 1U)
      {
        esp_training_payload_snapshot.shoulder_deg = esp_training_payload_snapshot.current_angle_deg;
      }
      else if (selected_action_index == 3U)
      {
        esp_training_payload_snapshot.arm_abd_deg = esp_training_payload_snapshot.current_angle_deg;
      }
      esp_training_payload_snapshot.emg_strength = emg_features.strength;
      esp_training_payload_snapshot.emg_mode = emg_features.mode;
      esp_training_payload_snapshot.completion_score = training_eval_snapshot.completion_score;
      esp_training_payload_snapshot.amplitude_score = training_eval_snapshot.amplitude_score;
      esp_training_payload_snapshot.total_score = training_eval_snapshot.total_score;
      esp_training_payload_snapshot.valid = training_eval_snapshot.valid;
      taskEXIT_CRITICAL();
    }
    else
    {
      have_last_angle = 0U;
      previous_rule_action = REHAB_ACTION_IDLE;
      latched_action = REHAB_ACTION_IDLE;
      rule_action = REHAB_ACTION_IDLE;
      if (current_ui_page == UPPER_PAGE_TRAINING)
      {
        taskENTER_CRITICAL();
        training_eval_snapshot.valid = 0U;
        esp_training_payload_snapshot.valid = 0U;
        taskEXIT_CRITICAL();

        if (selected_action_index == 1U)
        {
          memset(&shoulder_lift_eval_sample, 0, sizeof(shoulder_lift_eval_sample));
          shoulder_lift_eval_sample.tick_ms = now_tick;
          ShoulderLiftEval_Update(&shoulder_lift_eval_sample);
        }
        else if (selected_action_index == 2U)
        {
          memset(&forearm_eval_sample, 0, sizeof(forearm_eval_sample));
          forearm_eval_sample.tick_ms = now_tick;
          ForearmRotEval_Update(&forearm_eval_sample);
        }
        else if (selected_action_index == 3U)
        {
          memset(&arm_abd_eval_sample, 0, sizeof(arm_abd_eval_sample));
          arm_abd_eval_sample.tick_ms = now_tick;
          ArmAbdEval_Update(&arm_abd_eval_sample);
        }
        else
        {
          memset(&elbow_eval_sample, 0, sizeof(elbow_eval_sample));
          elbow_eval_sample.tick_ms = now_tick;
          ElbowFlexEval_Update(&elbow_eval_sample);
        }
      }
    }

    if ((now_tick - last_print_tick) >= 200U)
    {
      last_print_tick = now_tick;
      if (rehab_frame.valid != 0U)
      {
#if (REHAB_ACTIVE_EVAL == REHAB_ACTIVE_EVAL_FOREARM_ROT)
        APP_LOG_PRINTF("REHAB action=%s rot=%ld.%02ld upper_roll=%c%ld.%02ld wrist_roll=%c%ld.%02ld emg=%u\r\n",
               Rehab_ActionName(rehab_frame.action),
               (long)(rehab_frame.forearm_rotation_angle_x100 / 100),
               (long)Rehab_AbsI32(rehab_frame.forearm_rotation_angle_x100 % 100),
               Rehab_SignCharI32(rehab_frame.upper_angle_x100[0]),
               (long)(Rehab_AbsI32(rehab_frame.upper_angle_x100[0]) / 100),
               (long)(Rehab_AbsI32(rehab_frame.upper_angle_x100[0]) % 100),
               Rehab_SignCharI32(rehab_frame.wrist_angle_x100[0]),
               (long)(Rehab_AbsI32(rehab_frame.wrist_angle_x100[0]) / 100),
               (long)(Rehab_AbsI32(rehab_frame.wrist_angle_x100[0]) % 100),
               (unsigned int)rehab_frame.emg_active);
#elif (REHAB_ACTIVE_EVAL == REHAB_ACTIVE_EVAL_ARM_ABD)
        APP_LOG_PRINTF("REHAB action=%s abd=%ld.%02ld upper_y=%c%ld.%02ld wrist_y=%c%ld.%02ld ugy=%c%ld.%02ld emg=%u\r\n",
               Rehab_ActionName(rehab_frame.action),
               (long)(rehab_frame.arm_abduction_angle_x100 / 100),
               (long)Rehab_AbsI32(rehab_frame.arm_abduction_angle_x100 % 100),
               Rehab_SignCharI32(rehab_frame.upper_angle_x100[1]),
               (long)(Rehab_AbsI32(rehab_frame.upper_angle_x100[1]) / 100),
               (long)(Rehab_AbsI32(rehab_frame.upper_angle_x100[1]) % 100),
               Rehab_SignCharI32(rehab_frame.wrist_angle_x100[1]),
               (long)(Rehab_AbsI32(rehab_frame.wrist_angle_x100[1]) / 100),
               (long)(Rehab_AbsI32(rehab_frame.wrist_angle_x100[1]) % 100),
               Rehab_SignCharI32(rehab_frame.upper_gyro_x10[1]),
               (long)(Rehab_AbsI32(rehab_frame.upper_gyro_x10[1]) / 10),
               (long)((Rehab_AbsI32(rehab_frame.upper_gyro_x10[1]) % 10) * 10),
               (unsigned int)rehab_frame.emg_active);
#elif (REHAB_ACTIVE_EVAL == REHAB_ACTIVE_EVAL_SHOULDER_LIFT)
        APP_LOG_PRINTF("REHAB action=%s lift=%ld.%02ld upper_z=%c%ld.%02ld upper_y=%c%ld.%02ld emg=%u\r\n",
               Rehab_ActionName(rehab_frame.action),
               (long)(rehab_frame.shoulder_lift_angle_x100 / 100),
               (long)Rehab_AbsI32(rehab_frame.shoulder_lift_angle_x100 % 100),
               Rehab_SignCharI32(rehab_frame.upper_angle_x100[2]),
               (long)(Rehab_AbsI32(rehab_frame.upper_angle_x100[2]) / 100),
               (long)(Rehab_AbsI32(rehab_frame.upper_angle_x100[2]) % 100),
               Rehab_SignCharI32(rehab_frame.upper_angle_x100[1]),
               (long)(Rehab_AbsI32(rehab_frame.upper_angle_x100[1]) / 100),
               (long)(Rehab_AbsI32(rehab_frame.upper_angle_x100[1]) % 100),
               (unsigned int)rehab_frame.emg_active);
#else
        APP_LOG_PRINTF("REHAB action=%s flex=%ld.%02ld vel=%c%ld.%02ld range=%ld.%02ld upper_pitch=%c%ld.%02ld wrist_pitch=%c%ld.%02ld emg=%u\r\n",
               Rehab_ActionName(rehab_frame.action),
               (long)(rehab_frame.elbow_angle_x100 / 100),
               (long)Rehab_AbsI32(rehab_frame.elbow_angle_x100 % 100),
               Rehab_SignCharI32(rehab_frame.elbow_velocity_x100_s),
               (long)(Rehab_AbsI32(rehab_frame.elbow_velocity_x100_s) / 100),
               (long)(Rehab_AbsI32(rehab_frame.elbow_velocity_x100_s) % 100),
               (long)(rehab_frame.elbow_range_x100 / 100),
               (long)Rehab_AbsI32(rehab_frame.elbow_range_x100 % 100),
               Rehab_SignCharI32(rehab_frame.upper_angle_x100[1]),
               (long)(Rehab_AbsI32(rehab_frame.upper_angle_x100[1]) / 100),
               (long)(Rehab_AbsI32(rehab_frame.upper_angle_x100[1]) % 100),
               Rehab_SignCharI32(rehab_frame.wrist_angle_x100[1]),
               (long)(Rehab_AbsI32(rehab_frame.wrist_angle_x100[1]) / 100),
               (long)(Rehab_AbsI32(rehab_frame.wrist_angle_x100[1]) % 100),
               (unsigned int)rehab_frame.emg_active);
#endif
      }
      else
      {
        APP_LOG_PRINTF("REHAB waiting upper=%u wrist=%u wrist_status=0x%02X hr=%u\r\n",
               (unsigned int)upper_valid,
               (unsigned int)wrist_valid,
               (unsigned int)wrist_frame.status,
               (unsigned int)wrist_frame.heart_rate);
      }
    }

    osDelay(REHAB_ALGO_PERIOD_MS);
  }
  /* USER CODE END AlgoTask */
}

/* USER CODE BEGIN Header_StartEmgTask */
/**
* @brief Function implementing the EmgTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartEmgTask */
// DMA 鍙岀紦鍐?ADC 鑲岀數閲囬泦浠诲姟锛岄厤鍚?ADC DMA 鍗婂畬鎴?/ 鍏ㄥ畬鎴愪俊鍙烽噺鍋氬垎鍧楁暟鎹鐞?
void StartEmgTask(void *argument)
{
  /* USER CODE BEGIN EmgTask */
  HAL_StatusTypeDef emg_start_status = HAL_ERROR;
  uint32_t last_retry_tick = 0U;
  uint32_t emg_start_ok_tick = 0U;
  uint8_t start_ok_reported = 0U;
  uint8_t software_fallback_active = 0U;
  uint8_t emg_started = 0U;

  // EMG 纭欢鍚姩鍒濆鍖?
  (void)argument;
  UpperArm_WaitImuReady(300U);

  /* Infinite loop */
  for(;;)
  {
    uint8_t emg_should_run;
    uint8_t processed = 0U;

#if EMG_UART4_RECORD_MODE
    emg_should_run = 1U;
#else
    emg_should_run = ((current_ui_page == UPPER_PAGE_PREP) ||
                      (current_ui_page == UPPER_PAGE_TRAINING) ||
                      (current_ui_page == UPPER_PAGE_FREE_TRAINING)) ? 1U : 0U;
#endif
    if (emg_should_run == 0U)
    {
      if (emg_started != 0U)
      {
        EmgSensor_Stop();
        emg_started = 0U;
        emg_start_status = HAL_ERROR;
        emg_start_ok_tick = 0U;
        start_ok_reported = 0U;
        software_fallback_active = 0U;
      }
      osDelay(100U);
      continue;
    }

    if ((emg_started == 0U) &&
        ((last_retry_tick == 0U) ||
         ((osKernelGetTickCount() - last_retry_tick) >= 1000U)))
    {
      last_retry_tick = osKernelGetTickCount();
      emg_start_status = EmgSensor_Start();
      emg_started = (emg_start_status == HAL_OK) ? 1U : 0U;
      if (emg_start_status != HAL_OK)
      {
        DebugUart_WriteBuffer("EMGSTAT,START_FAILED\r\n", sizeof("EMGSTAT,START_FAILED\r\n") - 1);
      }
      else if (start_ok_reported == 0U)
      {
        start_ok_reported = 1U;
        emg_start_ok_tick = osKernelGetTickCount();
        DebugUart_WriteBuffer("EMGSTAT,START_OK\r\n", sizeof("EMGSTAT,START_OK\r\n") - 1);
      }
    }

    if (emg_started == 0U)
    {
      osDelay(50U);
      continue;
    }

#if EMG_SENSOR_COLLECTION_POLLING
    (void)EmgSensor_PollSample();
    osDelay(EMG_POLL_PERIOD_MS);
    continue;
#endif

    if ((emg_start_status != HAL_OK) &&
        ((osKernelGetTickCount() - last_retry_tick) >= 1000U))
    {
      last_retry_tick = osKernelGetTickCount();
      emg_start_status = EmgSensor_Start();
      if (emg_start_status == HAL_OK)
      {
        if (start_ok_reported == 0U)
        {
          start_ok_reported = 1U;
          emg_start_ok_tick = osKernelGetTickCount();
          DebugUart_WriteBuffer("EMGSTAT,START_OK\r\n", sizeof("EMGSTAT,START_OK\r\n") - 1);
        }
      }
      else
      {
        DebugUart_WriteBuffer("EMGSTAT,START_FAILED\r\n", sizeof("EMGSTAT,START_FAILED\r\n") - 1);
      }
    }

    // 闈為樆濉炶幏鍙栧崐缂撳啿淇″彿閲忥紝鏈変俊鍙峰氨澶勭悊
    if (osSemaphoreAcquire(adcHalfCpltSemHandle, 0U) == osOK)
    {
      EmgSensor_ProcessHalfBuffer();
      processed = 1U;
    }

    // 闈為樆濉炶幏鍙栧叏缂撳啿淇″彿閲?
    if (osSemaphoreAcquire(adcCpltSemHandle, 0U) == osOK)
    {
      EmgSensor_ProcessFullBuffer();
      processed = 1U;
    }

    if (processed == 0U)
    {
      EmgSensor_Features_t emg_features;

      EmgSensor_GetFeatures(&emg_features);
      /* Prefer TIM3 + ADC4 DMA. If DMA starts but yields no samples, switch
       * once to 100 Hz polling so the EMG state cannot remain at relax/zero. */
      if ((software_fallback_active == 0U) &&
          (emg_start_ok_tick != 0U) &&
          (((emg_features.sample_count == 0U) &&
            ((osKernelGetTickCount() - emg_start_ok_tick) >= 1000U)) ||
           ((emg_features.calibrated == 0U) &&
            ((osKernelGetTickCount() - emg_start_ok_tick) >= 6000U))))
      {
        if (EmgSensor_StartSoftwareFallback() == HAL_OK)
        {
          software_fallback_active = 1U;
          DebugUart_WriteBuffer("EMGSTAT,DMA_TIMEOUT_FALLBACK\r\n",
                                sizeof("EMGSTAT,DMA_TIMEOUT_FALLBACK\r\n") - 1);
        }
        else
        {
          DebugUart_WriteBuffer("EMGSTAT,FALLBACK_FAILED\r\n",
                                sizeof("EMGSTAT,FALLBACK_FAILED\r\n") - 1);
        }
      }

      if (software_fallback_active != 0U)
      {
        (void)EmgSensor_PollSample();
        osDelay(EMG_POLL_PERIOD_MS);
        continue;
      }

      /* DMA模式只能在半满/全满通知后处理对应缓冲区，不能在等待期间
       * 重复扫描旧数据，否则会造成采样重复并干扰其他RTOS任务。 */
    }
    osDelay(1);
  }
  /* USER CODE END EmgTask */
}

/* USER CODE BEGIN Header_StartWirelessTask */
/**
* @brief Function implementing the WirelessTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartWirelessTask */
// Poll USART1 bytes from the HC-05 module and parse wrist-node frames.
void StartWirelessTask(void *argument)
{
  /* USER CODE BEGIN WirelessTask */
  WirelessWristFrame_t wrist_frame;
  WirelessHealthFrame_t health_frame;
  WirelessLinkStats_t stats;
  uint32_t last_debug_tick = 0U;
  uint32_t last_frame_tick = 0U;
  uint32_t last_health_tick = 0U;
  uint32_t last_frames_ok = 0U;
  uint32_t last_health_frames_ok = 0U;
  uint32_t debug_last_frames_ok = 0U;
  uint32_t debug_last_health_frames_ok = 0U;
  HAL_StatusTypeDef rx_status;
  HAL_StatusTypeDef ack_status = HAL_OK;
  uint8_t has_frame = 0U;

  (void)argument;
  UpperArm_WaitImuReady(500U);

  WirelessLink_Init();
  /* 开启 USART1 单字节中断接收，字节在 HAL_UART_RxCpltCallback 中喂给解析器 */
  rx_status = HAL_UART_Receive_IT(&huart1, &wireless_rx_byte, 1U);
  taskENTER_CRITICAL();
  wireless_rx_enabled = (rx_status == HAL_OK) ? 1U : 0U;
  wireless_last_frame_tick = 0U;
  wireless_last_health_tick = 0U;
  latest_wrist_valid = 0U;
  latest_wrist_health_valid = 0U;
  taskEXIT_CRITICAL();
  WIRELESS_LOG_PRINTF("Wrist link RX on USART1, baud=9600\r\n");

  /* Infinite loop */
  for(;;)
  {
    uint32_t now_tick = osKernelGetTickCount();

    /* 接收在中断中完成，这里定期自愈：若接收被错误中断则重新挂起接收 */
    if (HAL_UART_GetState(&huart1) == HAL_UART_STATE_READY)
    {
      rx_status = HAL_UART_Receive_IT(&huart1, &wireless_rx_byte, 1U);
      taskENTER_CRITICAL();
      wireless_rx_enabled = (rx_status == HAL_OK) ? 1U : 0U;
      taskEXIT_CRITICAL();
    }

    WirelessLink_GetStats(&stats);
    if ((stats.health_frames_ok != last_health_frames_ok) && (WirelessLink_GetLatestHealth(&health_frame) != 0U))
    {
      last_health_frames_ok = stats.health_frames_ok;
      last_health_tick = osKernelGetTickCount();
      taskENTER_CRITICAL();
      latest_wrist_health_frame = health_frame;
      latest_wrist_health_valid = 1U;
      wireless_last_health_tick = last_health_tick;
      taskEXIT_CRITICAL();
    }

    if ((stats.frames_ok != last_frames_ok) && (WirelessLink_GetLatest(&wrist_frame) != 0U))
    {
      last_frames_ok = stats.frames_ok;
      last_frame_tick = osKernelGetTickCount();
      taskENTER_CRITICAL();
      latest_wrist_frame = wrist_frame;
      latest_wrist_valid = 1U;
      wireless_last_frame_tick = last_frame_tick;
      if (((wrist_frame.status2 & WIRELESS_STATUS2_HEALTH_MODE) == 0U) ||
          ((wrist_frame.status2 & WIRELESS_STATUS2_HEALTH_EXIT) != 0U))
      {
        latest_wrist_health_valid = 0U;
      }
      taskEXIT_CRITICAL();

      {
        uint8_t ack[4] = {
          WIRELESS_ACK_BYTE0,
          WIRELESS_ACK_BYTE1,
          WIRELESS_LINK_MSG_BYTE2,
          WIRELESS_LINK_MSG_BYTE3
        };

        if (wireless_train_finish_pending != 0U)
        {
          ack[2] = WIRELESS_HOME_MSG_BYTE2;
          ack[3] = WIRELESS_HOME_MSG_BYTE3;
          wireless_train_finish_pending = 0U;
        }
        else if (wireless_health_start_pending != 0U)
        {
          ack[2] = WIRELESS_HEALTH_MSG_BYTE2;
          ack[3] = WIRELESS_HEALTH_MSG_BYTE3;
          wireless_health_start_pending = 0U;
        }
        else if (wireless_train_start_pending != 0U)
        {
          ack[2] = WIRELESS_TRAIN_MSG_BYTE2;
          ack[3] = WIRELESS_TRAIN_MSG_BYTE3;
          wireless_train_start_pending = 0U;
        }
        ack_status = HAL_UART_Transmit(&huart1, (uint8_t *)ack, sizeof(ack), 10U);
      }

      has_frame = 1U;
    }
    else if ((last_frame_tick != 0U) && ((now_tick - last_frame_tick) > WIRELESS_LINK_TIMEOUT_MS))
    {
      last_frame_tick = 0U;
      has_frame = 0U;
      taskENTER_CRITICAL();
      latest_wrist_valid = 0U;
      wireless_last_frame_tick = 0U;
      wireless_train_start_pending = 0U;
      wireless_train_finish_pending = 0U;
      wireless_health_start_pending = 0U;
      taskEXIT_CRITICAL();
    }
    if ((last_health_tick != 0U) && ((now_tick - last_health_tick) > WIRELESS_LINK_TIMEOUT_MS))
    {
      last_health_tick = 0U;
      taskENTER_CRITICAL();
      latest_wrist_health_valid = 0U;
      wireless_last_health_tick = 0U;
      taskEXIT_CRITICAL();
    }

    if ((osKernelGetTickCount() - last_debug_tick) >= 1000U)
    {
      last_debug_tick = osKernelGetTickCount();

      if (has_frame != 0U)
      {
        uint8_t fresh = (stats.frames_ok != debug_last_frames_ok) ? 1U : 0U;
        debug_last_frames_ok = stats.frames_ok;
        if (stats.health_frames_ok != debug_last_health_frames_ok)
        {
          debug_last_health_frames_ok = stats.health_frames_ok;
          WIRELESS_LOG_PRINTF("HC05_HEALTH fresh ok=%lu seq=%u hr=%u spo2=%u sbp=%u dbp=%u age=%lu\r\n",
                 (unsigned long)stats.health_frames_ok,
                 (unsigned int)health_frame.seq,
                 (unsigned int)health_frame.heart_rate,
                 (unsigned int)health_frame.spo2,
                 (unsigned int)health_frame.sbp,
                 (unsigned int)health_frame.dbp,
                 (unsigned long)((last_health_tick == 0U) ? 999999U : (osKernelGetTickCount() - last_health_tick)));
        }

        WIRELESS_LOG_PRINTF("HC05_RX %s ok=%lu health=%lu bytes=%lu resync=%lu err=%lu seq=%u status=0x%02X ack_status=%d acc_mg=%d,%d,%d gyro_x10=%d,%d,%d angle_x100=%d,%d,%d age=%lu\r\n",
               (fresh != 0U) ? "fresh" : "stale",
               (unsigned long)stats.frames_ok,
               (unsigned long)stats.health_frames_ok,
               (unsigned long)stats.rx_bytes,
               (unsigned long)stats.resync_count,
               (unsigned long)stats.checksum_errors,
               (unsigned int)wrist_frame.seq,
               (unsigned int)wrist_frame.status,
               (int)ack_status,
               (int)wrist_frame.acc_mg[0],
               (int)wrist_frame.acc_mg[1],
               (int)wrist_frame.acc_mg[2],
               (int)wrist_frame.gyro_dps_x10[0],
               (int)wrist_frame.gyro_dps_x10[1],
               (int)wrist_frame.gyro_dps_x10[2],
               (int)wrist_frame.angle_x100[0],
               (int)wrist_frame.angle_x100[1],
               (int)wrist_frame.angle_x100[2],
               (unsigned long)(osKernelGetTickCount() - last_frame_tick));
        WIRELESS_LOG_PRINTF("WRIST_GYRO_RX %s seq=%u valid=%u gx=%d gy=%d gz=%d dps_x10 age=%lu\r\n",
               (fresh != 0U) ? "fresh" : "stale",
               (unsigned int)wrist_frame.seq,
               (unsigned int)((wrist_frame.status & 0x01U) != 0U),
               (int)wrist_frame.gyro_dps_x10[0],
               (int)wrist_frame.gyro_dps_x10[1],
               (int)wrist_frame.gyro_dps_x10[2],
               (unsigned long)(osKernelGetTickCount() - last_frame_tick));
      }
    else
    {
        char raw_hex[WIRELESS_RAW_TRACE_SIZE * 3U + 1U];
        uint8_t raw_copy[WIRELESS_RAW_TRACE_SIZE];
        uint8_t raw_count;
        uint8_t raw_index;
        uint32_t last_byte_tick;
        uint8_t i;
        uint8_t offset = 0U;

        taskENTER_CRITICAL();
        raw_count = wireless_raw_trace_count;
        raw_index = wireless_raw_trace_index;
        last_byte_tick = wireless_last_byte_tick;
        for (i = 0U; i < WIRELESS_RAW_TRACE_SIZE; i++)
        {
          raw_copy[i] = wireless_raw_trace[i];
        }
        taskEXIT_CRITICAL();

        raw_hex[0] = '\0';
        for (i = 0U; i < raw_count; i++)
        {
          uint8_t src = (uint8_t)((raw_index + WIRELESS_RAW_TRACE_SIZE - raw_count + i) % WIRELESS_RAW_TRACE_SIZE);
          int written = snprintf(&raw_hex[offset],
                                 sizeof(raw_hex) - offset,
                                 "%02X%s",
                                 (unsigned int)raw_copy[src],
                                 (i + 1U < raw_count) ? " " : "");
          if (written <= 0)
          {
            break;
          }
          offset = (uint8_t)(offset + (uint8_t)written);
          if (offset >= sizeof(raw_hex))
          {
            offset = (uint8_t)(sizeof(raw_hex) - 1U);
            break;
          }
        }

        WIRELESS_LOG_PRINTF("HC05_RX waiting bytes=%lu resync=%lu err=%lu raw=%s rx_age=%lu rx_en=%u state=%lu rx_state=%lu uart_err=0x%08lX\r\n",
               (unsigned long)stats.rx_bytes,
               (unsigned long)stats.resync_count,
               (unsigned long)stats.checksum_errors,
               raw_hex,
               (unsigned long)((last_byte_tick == 0U) ? 999999U : (osKernelGetTickCount() - last_byte_tick)),
               (unsigned int)wireless_rx_enabled,
               (unsigned long)huart1.gState,
               (unsigned long)huart1.RxState,
               (unsigned long)huart1.ErrorCode);
      }
    }

    osDelay(100);
  }
  /* USER CODE END WirelessTask */
}

/* USER CODE BEGIN Header_StartDataStreamTask */
/**
* @brief Function implementing the DataStreamTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDataStreamTask */
void StartDataStreamTask(void *argument)
{
  /* USER CODE BEGIN DataStreamTask */
#if EMG_CSV_STREAM_ENABLE
  EmgSensor_Features_t emg_features;
  char line[128];
#endif
  int len;

  (void)argument;

  if (EMG_UART4_RECORD_MODE == 0U)
  {
    osMutexAcquire(uart2MutexHandle, osWaitForever);
//    APP_LOG_PRINTF("Data stream USART2 PA2(TX) PA3(RX) start\r\n");
    osMutexRelease(uart2MutexHandle);
  }
  /* Infinite loop */
  for(;;)
  {
#if EMG_CSV_STREAM_ENABLE
    EmgSensor_GetFeatures(&emg_features);
    len = snprintf(line,
                   sizeof(line),
                   "EMG,%u,%ld,%ld,%ld,%u,%u,%u,%s\r\n",
                   (unsigned int)emg_features.raw,
                   (long)emg_features.baseline_x10,
                   (long)emg_features.rectified_x10,
                   (long)emg_features.rms_x10,
                   (unsigned int)emg_features.calibrated,
                   (unsigned int)emg_features.strength,
                   (unsigned int)emg_features.mode,
                   (emg_features.mode == 1U) ? "weak" : ((emg_features.mode == 2U) ? "strong" : "relax"));
#else
    len = 0;
#endif

#if EMG_CSV_STREAM_ENABLE
    if (len > 0)
    {
      DebugUart_WriteBuffer(line, len);
    }
#endif

    osDelay(DATA_STREAM_PERIOD_MS);
  }
  /* USER CODE END DataStreamTask */
}

/* USER CODE BEGIN Header_StartEspUploadTask */
/**
* @brief Function implementing the EspUploadTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartEspUploadTask */
void StartEspUploadTask(void *argument)
{
  /* USER CODE BEGIN EspUploadTask */
  char command[96];
  char payload[512];
  WirelessHealthFrame_t health;
  uint8_t health_valid;
  uint8_t wrist_health_upload_active;
  uint8_t upper_health_request_active;
  uint16_t last_uploaded_health_seq = 0xFFFFU;
  uint32_t last_health_upload_tick = 0U;
  uint32_t last_time_sync_tick = 0U;
  uint32_t time_sync_interval_ms = ESP8266_TIME_SYNC_RETRY_MS;

  (void)argument;
  UpperArm_WaitImuReady(3000U);

#if ESP8266_UPLOAD_ENABLE
#if ESP8266_MINIMAL_TX_TEST && !ESP8266_AT_ONLY_TEST
  osDelay(ESP8266_START_DELAY_MS);
  for (;;)
  {
    Esp8266_WriteBuffer("A", 1U, 10U);
    osDelay(ESP8266_MINIMAL_TX_PERIOD_MS);
  }
#else
  osDelay(ESP8266_START_DELAY_MS);
  Esp8266_SendCommand("AT\r\n", 500U);
  Esp8266_SendCommand("ATE0\r\n", 500U);
  Esp8266_SendCommand("AT+CWMODE=1\r\n", 500U);
  Esp8266_SendCommand("AT+CIPMUX=0\r\n", 500U);
  (void)snprintf(command,
                 sizeof(command),
                 "AT+CWJAP=\"%s\",\"%s\"\r\n",
                 ESP8266_WIFI_SSID,
                 ESP8266_WIFI_PASSWORD);
  Esp8266_SendCommand(command, 8000U);
  osDelay(2000U);
#if ESP8266_AT_ONLY_TEST
  for (;;)
  {
    osDelay(1000U);
  }
#else
  /* 不在固定启动延时后立即校时：此时 web/网络链路可能尚未就绪。
   * 先进入主循环完成首次 MQTT 上传，再由主循环发起 HTTP 校时；失败时
   * 保持 30 秒重试，成功后每 10 分钟刷新一次。 */
  last_time_sync_tick = osKernelGetTickCount() - ESP8266_TIME_SYNC_RETRY_MS;
#if ESP8266_HTTP_ONLY_TEST
  for (;;)
  {
    osDelay(1000U);
  }
#else
#if ESP8266_MQTT_SHORT_TEST
  for (;;)
  {
    Esp8266_MqttPublish(ESP8266_MQTT_TOPIC_REALTIME, "tick=1");
    osDelay(ESP8266_UPLOAD_PERIOD_MS);
  }
#else
#if ESP8266_MQTT_MIN_TRAIN_TEST
  for (;;)
  {
#if ESP8266_MQTT_FIXED_TRAIN_TEST
    Esp8266_MqttPublish(ESP8266_MQTT_TOPIC_REALTIME, "pg=3,a=0,n=0,ang=0,e=0,ab=0,sh=0");
#else
    if (Esp8266_BuildMinTrainingPayload(payload, sizeof(payload)) != 0U)
    {
      Esp8266_MqttPublish(ESP8266_MQTT_TOPIC_REALTIME, payload);
    }
#endif
    osDelay(ESP8266_UPLOAD_PERIOD_MS);
  }
#else
#if ESP8266_MQTT_LIGHT_ONCE_TEST
  if (Esp8266_BuildLightPayload(payload, sizeof(payload)) != 0U)
  {
    Esp8266_MqttPublish(ESP8266_MQTT_TOPIC_REALTIME, payload);
  }
  for (;;)
  {
    osDelay(1000U);
  }
#else
#if ESP8266_MQTT_FULL_ONCE_TEST
  if (Esp8266_BuildPayload(payload, sizeof(payload)) != 0U)
  {
    Esp8266_MqttPublish(ESP8266_MQTT_TOPIC_REALTIME, payload);
  }
  for (;;)
  {
    osDelay(1000U);
  }
#else
#if ESP8266_LONGEST_PAYLOAD_SIM_TEST
  for (;;)
  {
    Esp8266_SimulateLongestPayloadPublish(payload, sizeof(payload));
    osDelay(ESP8266_UPLOAD_PERIOD_MS);
  }
#else

  for (;;)
  {
    uint32_t now;
    UpperPage_t page;

    now = osKernelGetTickCount();
    if (Esp8266_ShouldPause() != 0U)
    {
      osDelay(50U);
      continue;
    }

#if ESP8266_SKIP_UPLOAD_ON_TRAINING_PAGE
    taskENTER_CRITICAL();
    page = current_ui_page;
    taskEXIT_CRITICAL();
    if ((page == UPPER_PAGE_TRAINING) || (page == UPPER_PAGE_FREE_TRAINING))
    {
      osDelay(ESP8266_UPLOAD_PERIOD_MS);
      continue;
    }
#else
    (void)page;
#endif

    if (Esp8266_BuildPayload(payload, sizeof(payload)) != 0U)
    {
      Esp8266_MqttPublish(ESP8266_MQTT_TOPIC_REALTIME, payload);
    }

    /* 首次 MQTT 上传完成后再切换到 HTTP 校时，避免固定启动延时造成
     * web 尚未启动或 Wi-Fi 尚未真正可用时的无效校时请求。 */
    if ((now - last_time_sync_tick) >= time_sync_interval_ms)
    {
      last_time_sync_tick = now;
      if (Esp8266_SyncTimeFromWeb() != 0U)
      {
        time_sync_interval_ms = ESP8266_TIME_SYNC_REFRESH_MS;
      }
      else
      {
        time_sync_interval_ms = ESP8266_TIME_SYNC_RETRY_MS;
      }
    }

    taskENTER_CRITICAL();
    health = latest_wrist_health_frame;
    health_valid = latest_wrist_health_valid;
    upper_health_request_active =
        ((upper_health_request_until_tick != 0U) &&
         ((int32_t)(upper_health_request_until_tick - now) > 0)) ? 1U : 0U;
    wrist_health_upload_active =
        ((latest_wrist_valid != 0U) &&
         (((latest_wrist_frame.status2 & WIRELESS_STATUS2_HEALTH_MODE) != 0U) ||
          (upper_health_request_active != 0U)) &&
         ((latest_wrist_frame.status2 & WIRELESS_STATUS2_HEALTH_EXIT) == 0U)) ? 1U : 0U;
    taskEXIT_CRITICAL();
    if ((ESP8266_HEALTH_UPLOAD_PERIOD_MS != 0U) &&
        (wrist_health_upload_active != 0U) &&
        (health_valid != 0U) &&
        ((health.status & WIRELESS_STATUS_HEART_VALID) != 0U) &&
        (health.seq != last_uploaded_health_seq) &&
        ((osKernelGetTickCount() - last_health_upload_tick) >= ESP8266_HEALTH_UPLOAD_PERIOD_MS) &&
        (Esp8266_BuildHealthPayload(payload, sizeof(payload), &health) != 0U))
    {
      Esp8266_MqttPublish(ESP8266_MQTT_TOPIC_HEALTH, payload);
      last_uploaded_health_seq = health.seq;
      last_health_upload_tick = osKernelGetTickCount();
    }
    osDelay(ESP8266_UPLOAD_PERIOD_MS);
  }
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#else
  for (;;)
  {
    osDelay(1000U);
  }
#endif
  /* USER CODE END EspUploadTask */
}

/* USER CODE BEGIN Header_StartUiTask */
/**
* @brief Function implementing the UiTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartUiTask */
void StartUiTask(void *argument)
{
  /* USER CODE BEGIN UiTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(20U);
  }
  /* USER CODE END UiTask */
}

/* USER CODE BEGIN Header_StartDebugTask */
/**
* @brief Function implementing the DebugTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDebugTask */
void StartDebugTask(void *argument)
{
  /* USER CODE BEGIN DebugTask */
  uint8_t voice_cmd;
  VoiceI2C_Status_t voice_status;
  uint32_t last_voice_status_tick = 0U;

  (void)argument;
  UpperArm_WaitImuReady(1000U);

  /* Infinite loop */
  for(;;)
  {
    if (VoiceI2C_GetCommand(&voice_cmd) != 0U)
    {
      APP_LOG_PRINTF("VOICE_I2C cmd='%c' 0x%02X\r\n",
             ((voice_cmd >= 32U) && (voice_cmd <= 126U)) ? (char)voice_cmd : '.',
             (unsigned int)voice_cmd);

      UpperArm_SubmitVoiceCommand(voice_cmd);
    }

    if (((current_ui_page != UPPER_PAGE_TRAINING) &&
         (current_ui_page != UPPER_PAGE_FREE_TRAINING)) &&
        ((osKernelGetTickCount() - last_voice_status_tick) >= 2000U))
    {
      last_voice_status_tick = osKernelGetTickCount();
      VoiceI2C_GetStatus(&voice_status);
      APP_LOG_PRINTF("VOICE_I2C addr8=0x42 rx=%lu err=%lu latest=0x%02X\r\n",
             (unsigned long)voice_status.rx_count,
             (unsigned long)voice_status.error_count,
             (unsigned int)voice_status.latest_cmd);
    }

    osDelay(100);
  }
  /* USER CODE END DebugTask */
}
/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USART1 收到一个腕端字节：喂给帧解析器后立即重新挂起下一字节接收 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1)
  {
    HAL_StatusTypeDef rx_status;

    wireless_raw_trace[wireless_raw_trace_index] = wireless_rx_byte;
    wireless_raw_trace_index = (uint8_t)((wireless_raw_trace_index + 1U) % WIRELESS_RAW_TRACE_SIZE);
    wireless_last_byte_tick = HAL_GetTick();
    if (wireless_raw_trace_count < WIRELESS_RAW_TRACE_SIZE)
    {
      wireless_raw_trace_count++;
    }

    (void)WirelessLink_PushByte(wireless_rx_byte, NULL);
    rx_status = HAL_UART_Receive_IT(&huart1, &wireless_rx_byte, 1U);
    wireless_rx_enabled = (rx_status == HAL_OK) ? 1U : 0U;
  }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1)
  {
    HAL_StatusTypeDef rx_status;

    __HAL_UART_CLEAR_OREFLAG(&huart1);
    __HAL_UART_CLEAR_FEFLAG(&huart1);
    __HAL_UART_CLEAR_NEFLAG(&huart1);
    rx_status = HAL_UART_Receive_IT(&huart1, &wireless_rx_byte, 1U);
    wireless_rx_enabled = (rx_status == HAL_OK) ? 1U : 0U;
  }
}

void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode)
{
  (void)AddrMatchCode;
  VoiceI2C_HandleAddrCallback(hi2c, TransferDirection);
}

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  VoiceI2C_HandleRxCpltCallback(hi2c);
}

void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c)
{
  VoiceI2C_HandleListenCpltCallback(hi2c);
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
  VoiceI2C_HandleErrorCallback(hi2c);
}

/* USER CODE END Application */








