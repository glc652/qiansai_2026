/* USER CODE BEGIN PV_FONT */
extern const lv_font_t upper_font_zh_16;
extern const lv_font_t upper_font_zh_14;
extern const lv_font_t upper_font_zh_12;
extern const lv_font_t upper_font_zh_10;
extern const lv_font_t upper_font_zh_9;
LV_IMG_DECLARE(upper_home_bg);
LV_IMG_DECLARE(upper_prep_bg);
LV_IMG_DECLARE(upper_home_bg_dark);
LV_IMG_DECLARE(upper_prep_bg_dark);
LV_IMG_DECLARE(upper_action_icon_1);
LV_IMG_DECLARE(upper_action_icon_2);
LV_IMG_DECLARE(upper_action_icon_3);
LV_IMG_DECLARE(upper_action_icon_4);
/* USER CODE END PV_FONT */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum
{
  UPPER_PAGE_HOME = 0,
  UPPER_PAGE_PREP,
  UPPER_PAGE_ACTION_SELECT,
  UPPER_PAGE_TRAINING,
  UPPER_PAGE_RESULT,
  UPPER_PAGE_SETTINGS,
  UPPER_PAGE_HISTORY,
  UPPER_PAGE_ANALYTICS,
  UPPER_PAGE_LINK_LOST,
  UPPER_PAGE_FREE_TRAINING,
  UPPER_PAGE_FREE_RESULT
} UpperPage_t;

typedef struct
{
  uint32_t tick_ms;
  uint8_t action_index;
  uint8_t score;
  uint8_t completion;
  uint8_t amplitude;
  uint16_t reps;
  uint16_t target_reps;
  uint16_t duration_sec;
  uint16_t max_angle;
  uint16_t action_counts[4];
} RehabHistoryRecord_t;

static int32_t Rehab_AngleToSignedX100(float angle_deg);
static int32_t Rehab_AbsI32(int32_t value);
static char Rehab_SignCharI32(int32_t value);

#define UPPER_TRAINING_REFRESH_ANGLE_CHART (1U)

#define UI_ZH_KEEP_POSE "\xE8\xAF\xB7\xE4\xBF\x9D\xE6\x8C\x81\xE5\x88\x9D\xE5\xA7\x8B\xE5\xA7\xBF\xE5\x8A\xBF"
#define UI_ZH_CALIBRATING "\xE7\xB3\xBB\xE7\xBB\x9F\xE6\xAD\xA3\xE5\x9C\xA8\xE6\xA0\xA1\xE5\x87\x86\xE4\xBC\xA0\xE6\x84\x9F\xE5\x99\xA8"
#define UI_ZH_ELBOW_FLEX "\xE8\x82\x98\xE5\x85\xB3\xE8\x8A\x82\xE5\xBC\xAF\xE6\x9B\xB2"
#define UI_ZH_ARM_ABD "\xE8\x82\xA9\xE9\x83\xA8\xE4\xB8\x8A\xE4\xB8\xBE"
#define UI_ZH_FOREARM_ROT "\xE5\x89\x8D\xE8\x87\x82\xE6\x97\x8B\xE8\xBD\xAC"
#define UI_ZH_SHOULDER_LIFT "\xE6\x89\x8B\xE8\x87\x82\xE5\xA4\x96\xE5\xB1\x95"
#define UI_ZH_SELECT_ACTION_TITLE "\xE8\xAE\xAD\xE7\xBB\x83\xE5\x8A\xA8\xE4\xBD\x9C\xE9\x80\x89\xE6\x8B\xA9"
#define UI_ZH_SELECT_ACTION_SUB "\xE8\xAF\xB7\xE9\x80\x89\xE6\x8B\xA9\xE5\xBD\x93\xE5\x89\x8D\xE5\x87\x86\xE5\xA4\x87\xE8\xAE\xAD\xE7\xBB\x83\xE7\x9A\x84\xE5\x8A\xA8\xE4\xBD\x9C"
#define UI_ZH_SCORE "\xE7\xBB\xBC\xE5\x90\x88\xE8\xAF\x84\xE5\x88\x86"
#define UI_ZH_SCORE_UNIT "\xE5\x88\x86"
#define UI_ZH_ACTION_COUNT "\xE5\x8A\xA8\xE4\xBD\x9C\xE6\xAC\xA1\xE6\x95\xB0"
#define UI_ZH_COMPLETION "\xE5\xAE\x8C\xE6\x88\x90\xE5\xBA\xA6"
#define UI_ZH_STABILITY "\xE5\xB9\xB3\xE7\xA8\xB3\xE5\xBA\xA6"
#define UI_ZH_AMPLITUDE "\xE5\xB9\x85\xE5\xBA\xA6"
#define UI_ZH_FINISH_TRAIN "\xE7\xBB\x93\xE6\x9D\x9F\xE8\xAE\xAD\xE7\xBB\x83"
#define UI_ZH_CURRENT_ACTION "\xE5\xBD\x93\xE5\x89\x8D\xE5\x8A\xA8\xE4\xBD\x9C"
#define UI_ZH_REALTIME_ANGLE "\xE5\xAE\x9E\xE6\x97\xB6\xE8\xA7\x92\xE5\xBA\xA6"
#define UI_ZH_TRAIN_TIMER "\xE8\xAE\xAD\xE7\xBB\x83\xE8\xAE\xA1\xE6\x97\xB6"
#define UI_ZH_ANGLE_CURVE "\xE5\xAE\x9E\xE6\x97\xB6\xE8\xA7\x92\xE5\xBA\xA6\xE6\x9B\xB2\xE7\xBA\xBF"
#define UI_ZH_GOOD "\xE8\x89\xAF\xE5\xA5\xBD"
#define UI_ZH_PAUSE "\xE6\x9A\x82\xE5\x81\x9C"
#define UI_ZH_CONTINUE "\xE7\xBB\xA7\xE7\xBB\xAD"
#define UI_ZH_RUNNING "\xE8\xBF\x90\xE8\xA1\x8C\xE4\xB8\xAD"
#define UI_ZH_BRAND "\xE6\x99\xBA\xE8\x83\xBD\xE5\xBA\xB7\xE5\xA4\x8D\xE8\x87\x82\xE5\xB8\xA6"
#define UI_ZH_SYSTEM "\xE8\xAE\xAD\xE7\xBB\x83\xE8\xAF\x84\xE4\xBC\xB0\xE7\xB3\xBB\xE7\xBB\x9F"
#define UI_ZH_LOGO "\xE5\xBA\xB7"
#define UI_ZH_TRAIN_DONE "\xE8\xAE\xAD\xE7\xBB\x83\xE5\xAE\x8C\xE6\x88\x90"
#define UI_ZH_TRAIN_COUNT "\xE8\xAE\xAD\xE7\xBB\x83\xE6\xAC\xA1\xE6\x95\xB0"
#define UI_ZH_MAX_ANGLE "\xE6\x9C\x80\xE5\xA4\xA7\xE8\xA7\x92\xE5\xBA\xA6"
#define UI_ZH_ENCOURAGE "\xE5\x81\x9A\xE5\xBE\x97\xE5\xBE\x88\xE5\xA5\xBD\xEF\xBC\x8C\xE8\xAF\xB7\xE7\xBB\xA7\xE7\xBB\xAD\xE4\xBF\x9D\xE6\x8C\x81\xEF\xBC\x81"
#define UI_ZH_RETRAIN "\xE9\x87\x8D\xE6\x96\xB0\xE8\xAE\xAD\xE7\xBB\x83"
#define UI_ZH_BACK_HOME "\xE8\xBF\x94\xE5\x9B\x9E\xE4\xB8\xBB\xE9\xA1\xB5"
#define UI_ZH_COUNT_UNIT "\xE6\xAC\xA1"
#define UI_ZH_SETTINGS "\xE8\xAE\xBE\xE7\xBD\xAE"
#define UI_ZH_VOICE_BROADCAST "\xE8\xAF\xAD\xE9\x9F\xB3\xE6\x92\xAD\xE6\x8A\xA5"
#define UI_ZH_VOICE_HINT "\xE8\xAE\xAD\xE7\xBB\x83\xE8\xBF\x87\xE7\xA8\x8B\xE6\x8F\x90\xE7\xA4\xBA"
#define UI_ZH_VIBRATION_REMIND "\xE6\x8C\xAF\xE5\x8A\xA8\xE6\x8F\x90\xE9\x86\x92"
#define UI_ZH_VIBRATION_HINT "\xE5\x85\xB3\xE9\x94\xAE\xE8\x8A\x82\xE7\x82\xB9\xE6\x8F\x90\xE9\x86\x92"
#define UI_ZH_AUTO_SAVE "\xE8\x87\xAA\xE5\x8A\xA8\xE4\xBF\x9D\xE5\xAD\x98\xE8\xAE\xAD\xE7\xBB\x83"
#define UI_ZH_AUTO_SAVE_HINT "\xE8\xAE\xAD\xE7\xBB\x83\xE6\x95\xB0\xE6\x8D\xAE\xE4\xBF\x9D\xE5\xAD\x98"
#define UI_ZH_NIGHT_MODE "\xE5\xA4\x9C\xE9\x97\xB4\xE6\xA8\xA1\xE5\xBC\x8F"
#define UI_ZH_NIGHT_MODE_HINT "\xE7\x99\xBD\xE5\xA4\xA9\x2F\xE9\xBB\x91\xE5\xA4\x9C\xE7\x95\x8C\xE9\x9D\xA2"
#define UI_ZH_TARGET_COUNT "\xE7\x9B\xAE\xE6\xA0\x87\xE6\xAC\xA1\xE6\x95\xB0"
#define UI_ZH_TARGET_COUNT_HINT "\xE6\xAF\x8F\xE6\xAC\xA1\xE8\xAE\xAD\xE7\xBB\x83\xE7\x9B\xAE\xE6\xA0\x87"
#define UI_ZH_TRAIN_DURATION "\xE8\xAE\xAD\xE7\xBB\x83\xE6\x97\xB6\xE9\x95\xBF"
#define UI_ZH_TRAIN_DURATION_HINT "\xE6\xAF\x8F\xE6\xAC\xA1\xE8\xAE\xAD\xE7\xBB\x83\xE6\x97\xB6\xE9\x97\xB4"
#define UI_ZH_SECOND_UNIT "\xE7\xA7\x92"
#define UI_ZH_RESTORE_DEFAULT "\xE6\x81\xA2\xE5\xA4\x8D\xE9\xBB\x98\xE8\xAE\xA4\xE8\xAE\xBE\xE7\xBD\xAE"
#define UI_ZH_RESTORE_HINT "\xE6\x81\xA2\xE5\xA4\x8D\xE4\xB8\xBA\xE9\xBB\x98\xE8\xAE\xA4\xE7\x8A\xB6\xE6\x80\x81"
#define UI_ZH_HISTORY "\xE5\x8E\x86\xE5\x8F\xB2\xE8\xAE\xB0\xE5\xBD\x95"
#define UI_ZH_NO_HISTORY "\xE6\x9A\x82\xE6\x97\xA0\xE8\xAE\xAD\xE7\xBB\x83\xE8\xAE\xB0\xE5\xBD\x95"
#define UI_ZH_LATEST_RECORD "\xE6\x9C\x80\xE8\xBF\x91\xE8\xAE\xAD\xE7\xBB\x83"
#define UI_ZH_TOTAL_TRAIN "\xE7\xB4\xAF\xE8\xAE\xA1\xE8\xAE\xAD\xE7\xBB\x83"
#define UI_ZH_CLEAR "\xE6\xB8\x85\xE7\xA9\xBA"
#define UI_ZH_ANALYTICS "\xE6\x95\xB0\xE6\x8D\xAE\xE5\x88\x86\xE6\x9E\x90"
#define UI_ZH_FREE_MODE "\xE8\x87\xAA\xE7\x94\xB1\xE6\xA8\xA1\xE5\xBC\x8F"
#define UI_ZH_FREE_TRAIN "\xE8\x87\xAA\xE7\x94\xB1\xE8\xAE\xAD\xE7\xBB\x83"
#define UI_ZH_FREE_HINT "\xE8\x87\xAA\xE7\x94\xB1\xE5\xAE\x8C\xE6\x88\x90\xE5\x9B\x9B\xE7\xA7\x8D\xE5\x8A\xA8\xE4\xBD\x9C"
#define UI_ZH_DETECTED_ACTION "\xE8\xAF\x86\xE5\x88\xAB\xE5\x8A\xA8\xE4\xBD\x9C"
#define UI_ZH_TOTAL_COUNT "\xE7\xB4\xAF\xE8\xAE\xA1\xE6\xAC\xA1\xE6\x95\xB0"
#define UI_ZH_CURRENT_ANGLE "\xE6\x9C\xAC\xE6\xAC\xA1\xE8\xA7\x92\xE5\xBA\xA6"
#define UI_ZH_NO_ACTION "\xE6\x9C\xAA\xE8\xAF\x86\xE5\x88\xAB"
#define UI_ZH_ANALYTICS_SHORT "\xE7\xBB\x9F\xE8\xAE\xA1"
#define UI_ZH_AVG_SCORE "\xE5\xB9\xB3\xE5\x9D\x87\xE5\xBE\x97\xE5\x88\x86"
#define UI_ZH_SCORE_TREND "\xE5\xBE\x97\xE5\x88\x86\xE8\xB6\x8B\xE5\x8A\xBF"
#define UI_ZH_TRAIN_ADVICE "\xE8\xAE\xAD\xE7\xBB\x83\xE5\xBB\xBA\xE8\xAE\xAE"
#define UI_ZH_DATA_NOT_ENOUGH "\xE6\x95\xB0\xE6\x8D\xAE\xE4\xB8\x8D\xE8\xB6\xB3"
#define UI_ZH_KEEP_GOING "\xE7\x8A\xB6\xE6\x80\x81\xE8\x89\xAF\xE5\xA5\xBD\xEF\xBC\x8C\xE8\xAF\xB7\xE7\xBB\xA7\xE7\xBB\xAD\xE4\xBF\x9D\xE6\x8C\x81"
#define UI_ZH_LINK_LOST_TITLE "\xE6\x9C\xAA\xE8\xBF\x9E\xE6\x8E\xA5"
#define UI_ZH_LINK_LOST_SUB "\xE8\xAF\xB7\xE5\x85\x88\xE8\xBF\x9E\xE6\x8E\xA5\xE6\x89\x8B\xE8\x85\x95\xE7\xAB\xAF\xE8\xAE\xBE\xE5\xA4\x87"
#define UI_ZH_LINK_LOST_HINT1 "\xE8\xBF\x9E\xE6\x8E\xA5\xE5\x90\x8E\xE5\xBC\x80\xE5\xA7\x8B\xE8\xAE\xAD\xE7\xBB\x83"
#define UI_ZH_LINK_LOST_HINT2 "\xE8\xBF\x94\xE5\x9B\x9E\xE4\xB8\xBB\xE9\xA1\xB5\xE6\x9F\xA5\xE7\x9C\x8B\xE8\xBF\x9E\xE6\x8E\xA5\xE7\x8A\xB6\xE6\x80\x81"
/* USER CODE END PTD */

#define REHAB_HISTORY_MAX_RECORDS         (8U)
#define REHAB_HISTORY_FLASH_MAGIC         (0x52485248UL)
#define REHAB_HISTORY_FLASH_VERSION       (1U)
#define REHAB_HISTORY_FLASH_PAGE_ADDRESS  (FLASH_BASE + FLASH_SIZE - FLASH_PAGE_SIZE)
#define REHAB_HISTORY_FLASH_PAGE_INDEX    (FLASH_PAGE_NB - 1U)
#define REHAB_HISTORY_FLASH_BANK          FLASH_BANK_2
#define UPPER_HW_KEY_DEBOUNCE_MS          (30U)
#define UPPER_HW_KEY_LONG_PRESS_MS        (800U)
#define UPPER_VIBRATION_PULSE_MS          (120U)
#define UPPER_BATTERY_ADC_PERIOD_MS       (10000U)
#define UPPER_BATTERY_RISE_NOISE_PERCENT  (2U)
#define UPPER_SETTINGS_ROW_H              (32U)
#define UPPER_SETTINGS_NIGHT_SWITCH_INDEX (5U)
#define UPPER_SCRIPTED_TRAINING_EVAL      (0U)
#define UPPER_FLASH_BATTERY_VALID_MASK    (0x01U)
#define UPPER_FLASH_NIGHT_MODE_MASK       (0x02U)
#define UPPER_VOICE_CMD_WAKE              (77U)
#define UPPER_VOICE_CMD_START_TRAIN       (80U)
#define UPPER_VOICE_CMD_FINISH_TRAIN      (81U)
#define UPPER_VOICE_CMD_PAUSE_TRAIN       (82U)
#define UPPER_VOICE_CMD_CONTINUE_TRAIN    (83U)
#define UPPER_VOICE_CMD_SWITCH_ACTION     (84U)
#define UPPER_VOICE_CMD_ACTION_ELBOW      (85U)
#define UPPER_VOICE_CMD_ACTION_ARM_ABD    (86U)
#define UPPER_VOICE_CMD_ACTION_FOREARM    (87U)
#define UPPER_VOICE_CMD_ACTION_SHOULDER   (88U)
#define UPPER_VOICE_CMD_VIBRATION_ON      (89U)
#define UPPER_VOICE_CMD_VIBRATION_OFF     (90U)
#define UPPER_VOICE_CMD_HOME              (91U)
#define UPPER_VOICE_CMD_HISTORY           (92U)
#define UPPER_VOICE_CMD_FREE_TRAIN        (93U)
#define UPPER_VOICE_CMD_SETTINGS          (94U)
#define UPPER_VOICE_CMD_TIRED             (95U)
#define UPPER_VOICE_CMD_HEART_RATE        (96U)
#define UPPER_VOICE_CMD_ACTION_READY      (97U)

typedef struct
{
  uint32_t magic;
  uint16_t version;
  uint16_t size;
  uint32_t checksum;
  uint8_t count;
  uint8_t next;
  uint8_t battery_percent;
  uint8_t battery_valid;
  RehabHistoryRecord_t records[REHAB_HISTORY_MAX_RECORDS];
} RehabHistoryFlashData_t;

typedef union
{
  RehabHistoryFlashData_t data;
  uint8_t raw[256];
} RehabHistoryFlashBlock_t;

static lv_obj_t *home_time_label;
static lv_obj_t *home_today_count_label;
static lv_obj_t *home_status_value_label;
static lv_obj_t *home_status_dot;
static lv_obj_t *home_bt_icon;
static lv_obj_t *home_battery_fill;
static lv_obj_t *home_battery_value_label;
static lv_obj_t *home_imu_angle_label;
static lv_obj_t *prep_countdown_label;
static lv_obj_t *prep_spinner_arc;
static lv_obj_t *prep_status_dots[6];
static lv_obj_t *prep_time_label;
static lv_obj_t *action_time_label;
static lv_obj_t *training_time_label;
static lv_obj_t *training_pause_icon_label;
static lv_obj_t *training_pause_text_label;
static lv_obj_t *training_count_label;
static lv_obj_t *training_timer_arc;
static lv_obj_t *training_timer_value_label;
static lv_obj_t *training_score_value_label;
static lv_obj_t *training_emg_mode_label;
static lv_obj_t *training_emg_strength_label;
static lv_obj_t *training_completion_bar;
static lv_obj_t *training_completion_value_label;
static lv_obj_t *training_amplitude_bar;
static lv_obj_t *training_amplitude_value_label;
static lv_obj_t *training_angle_value_label;
static lv_obj_t *training_angle_chart;
static lv_chart_series_t *training_angle_series;
static uint32_t training_angle_chart_last_tick;
static lv_obj_t *free_action_label;
static lv_obj_t *free_count_label;
static lv_obj_t *free_angle_label;
static lv_obj_t *free_action_count_labels[4];
static lv_obj_t *result_time_label;
static lv_obj_t *settings_target_count_label;
static lv_obj_t *settings_duration_label;
static lv_timer_t *prep_countdown_timer;
static lv_timer_t *prep_recalibrate_timer;
static lv_timer_t *prep_transition_timer;
static lv_timer_t *training_emg_enable_timer;
static uint8_t prep_countdown_value;
static uint16_t prep_remaining_ms;
static uint8_t selected_action_index;
static uint8_t training_paused;
static uint16_t training_completed_count;
static volatile uint16_t training_live_reps;
static volatile uint8_t training_eval_reset_request;
static uint16_t training_target_count = 3U;
static uint16_t training_target_duration_sec = 60U;
static uint32_t training_start_tick;
static uint32_t training_pause_start_tick;
static uint32_t training_paused_accum_ms;
static uint8_t prep_for_free_mode;
static uint16_t free_total_reps;
static uint16_t free_action_counts[4];
static uint16_t free_current_angle;
static uint16_t free_max_angle;
static uint8_t free_current_action_index = 4U;
static RehabAction_t free_last_counted_action = REHAB_ACTION_IDLE;
static UpperPage_t current_ui_page = UPPER_PAGE_HOME;
static uint8_t settings_switch_state[5] = {1U, 1U, 1U, 1U, 0U};
static RehabHistoryRecord_t rehab_history[REHAB_HISTORY_MAX_RECORDS];
static uint8_t rehab_history_count;
static uint8_t rehab_history_next;
static uint8_t battery_cached_percent;
static uint8_t battery_cached_valid;
static uint8_t upper_night_mode;
static uint8_t upper_page_transition_ready;
static int8_t upper_page_transition_dir = 1;


void UpperArm_LvglCreateHome(void);
static void UpperArm_LvglCreatePrep(void);
static void UpperArm_LvglCreateActionSelect(void);
static void UpperArm_LvglCreateTraining(void);
static void UpperArm_LvglCreateResult(void);
static void UpperArm_LvglCreateFreeTraining(void);
static void UpperArm_LvglCreateFreeResult(void);
static void UpperArm_LvglCreateSettings(void);
static void UpperArm_LvglCreateHistory(void);
static void UpperArm_LvglCreateAnalytics(void);
static void UpperArm_LvglCreateLinkLost(void);
void UpperArm_LvglUpdateHome(uint32_t now);
static void UpperArm_LvglResetPageObjectRefs(void);
static void UpperArm_AddHistoryRecord(void);
static void UpperArm_AddFreeHistoryRecord(void);
static void UpperArm_ClearHistory(void);
static void UpperArm_HistorySaveToFlash(void);
static uint32_t UpperArm_HistoryChecksum(const RehabHistoryFlashData_t *data);
void UpperArm_SubmitVoiceCommand(uint8_t cmd);
uint8_t UpperArm_ProcessPendingVoiceCommand(void);
uint8_t UpperArm_LvglIsHomePage(void);
static void UpperArm_HandleVoiceCommand(uint8_t cmd);
static void UpperArm_BatteryUpdateFromMeasured(uint8_t measured_percent);
static lv_color_t UpperArm_BatteryColor(uint8_t percent);
static lv_color_t UpperArm_ThemeColor(uint32_t light_hex, uint32_t dark_hex);
static const lv_img_dsc_t *UpperArm_GetHomeBg(void);
static const lv_img_dsc_t *UpperArm_GetPrepBg(void);
static void UpperArm_LvglApplyBatteryDisplay(lv_obj_t *fill,
                                             lv_obj_t *label,
                                             lv_coord_t max_width,
                                             uint8_t show_percent_sign);
static uint8_t UpperArm_WirelessIsConnected(uint32_t now);
static void UpperArm_RequestVibration(void);
static uint8_t UpperArm_DebounceStateChanged(uint8_t sample_pressed,
                                             uint8_t *stable_pressed,
                                             uint8_t *last_sample_pressed,
                                             uint32_t *last_change_tick,
                                             uint32_t now);
static void UpperArm_OnHomeButtonClicked(lv_event_t *e);
static void UpperArm_OnActionButtonClicked(lv_event_t *e);
static void UpperArm_OnStartTrainingClicked(lv_event_t *e);
static void UpperArm_OnPauseTrainingClicked(lv_event_t *e);
static void UpperArm_OnSettingsBackClicked(lv_event_t *e);
static void UpperArm_OnSettingsSwitchChanged(lv_event_t *e);
static void UpperArm_OnSettingsAdjustClicked(lv_event_t *e);
static void UpperArm_OnSettingsResetClicked(lv_event_t *e);
static void UpperArm_OnClearHistoryClicked(lv_event_t *e);
static void UpperArm_OnAnalyticsClicked(lv_event_t *e);
static void UpperArm_OnPrepCountdown(lv_timer_t *timer);
static void UpperArm_OnPrepRecalibrate(lv_timer_t *timer);
static void UpperArm_OnPrepTransition(lv_timer_t *timer);
static void UpperArm_OnTrainingEmgEnable(lv_timer_t *timer);
static void UpperArm_OnFinishTrainingClicked(lv_event_t *e);
static void UpperArm_OnRetrainClicked(lv_event_t *e);
static void UpperArm_OnBackHomeClicked(lv_event_t *e);
static void UpperArm_PrepAnimateNumber(void);
static void UpperArm_PrepUpdateDots(uint16_t elapsed_ms);
static void UpperArm_LvglSetObjOpa(void *obj, int32_t value);
static void UpperArm_LvglSetObjZoom(void *obj, int32_t value);
static void UpperArm_LvglSetTransitionDir(int8_t dir);
static void UpperArm_LvglApplyNightPalette(lv_obj_t *obj);
static void UpperArm_LvglPageReady(lv_obj_t *screen);
static uint32_t UpperArm_GetTrainingElapsedMs(uint32_t now);
static int32_t UpperArm_ScriptedTrainingJitterX100(uint32_t elapsed_ms, int32_t range_x100);
static int32_t UpperArm_ScriptedTrainingAngleX100(uint32_t elapsed_ms, uint8_t action_index);
static int32_t UpperArm_ScriptedTrainingMaxAngleX100(uint32_t elapsed_ms, uint8_t action_index);
static int32_t UpperArm_ScriptedTrainingTargetAngleX100(uint8_t action_index);
static uint16_t UpperArm_ScriptedTrainingReps(uint32_t elapsed_ms);
static void UpperArm_ApplyScriptedTrainingEval(TrainingEvalSnapshot_t *eval, uint32_t now);

static uint32_t UpperArm_GetTrainingElapsedMs(uint32_t now)
{
  uint32_t train_now = (training_paused != 0U) ? training_pause_start_tick : now;
  uint32_t elapsed_ms = 0U;

  if (train_now >= training_start_tick)
  {
    elapsed_ms = train_now - training_start_tick;
    if (elapsed_ms > training_paused_accum_ms)
    {
      elapsed_ms -= training_paused_accum_ms;
    }
    else
    {
      elapsed_ms = 0U;
    }
  }
  return elapsed_ms;
}

static int32_t UpperArm_ScriptedTrainingJitterX100(uint32_t elapsed_ms, int32_t range_x100)
{
  int32_t phase;

  if (range_x100 <= 0)
  {
    return 0;
  }

  phase = (int32_t)(((elapsed_ms / 350U) * 37U + (elapsed_ms / 1000U) * 11U) % 9U) - 4;
  return (phase * range_x100) / 4;
}

static int32_t UpperArm_ScriptedTrainingTargetAngleX100(uint8_t action_index)
{
  return (action_index == 3U) ? 9000 : 3000;
}

static int32_t UpperArm_ScriptedTrainingAngleX100(uint32_t elapsed_ms, uint8_t action_index)
{
  if (action_index == 3U)
  {
    if ((elapsed_ms >= 7000U) && (elapsed_ms < 7800U))
    {
      return 3200 + UpperArm_ScriptedTrainingJitterX100(elapsed_ms, 220);
    }
    if ((elapsed_ms >= 15000U) && (elapsed_ms < 15800U))
    {
      return 5400 + UpperArm_ScriptedTrainingJitterX100(elapsed_ms, 260);
    }
    if ((elapsed_ms >= 15800U) && (elapsed_ms < 21000U))
    {
      return 700 + UpperArm_ScriptedTrainingJitterX100(elapsed_ms, 220);
    }
    if ((elapsed_ms >= 21000U) && (elapsed_ms < 21800U))
    {
      return 4000 + UpperArm_ScriptedTrainingJitterX100(elapsed_ms, 240);
    }
    return 300 + UpperArm_ScriptedTrainingJitterX100(elapsed_ms, 180);
  }

  if ((elapsed_ms >= 7000U) && (elapsed_ms < 7800U))
  {
    return 3000;
  }
  if ((elapsed_ms >= 15000U) && (elapsed_ms < 15800U))
  {
    return 2500;
  }
  if ((elapsed_ms >= 15800U) && (elapsed_ms < 21000U))
  {
    return 500 + UpperArm_ScriptedTrainingJitterX100(elapsed_ms, 80);
  }
  if ((elapsed_ms >= 21000U) && (elapsed_ms < 21400U))
  {
    return 1800;
  }
  if ((elapsed_ms >= 21400U) && (elapsed_ms < 21800U))
  {
    return 1700;
  }
  return 200 + UpperArm_ScriptedTrainingJitterX100(elapsed_ms, 70);
}

static int32_t UpperArm_ScriptedTrainingMaxAngleX100(uint32_t elapsed_ms, uint8_t action_index)
{
  if (action_index == 3U)
  {
    if (elapsed_ms >= 15000U)
    {
      return 5400;
    }
    if (elapsed_ms >= 21000U)
    {
      return 5400;
    }
    if (elapsed_ms >= 7000U)
    {
      return 3200;
    }
    return 300;
  }

  if (elapsed_ms >= 7000U)
  {
    return 3000;
  }
  if (elapsed_ms >= 15000U)
  {
    return 2500;
  }
  return 200;
}

static uint16_t UpperArm_ScriptedTrainingReps(uint32_t elapsed_ms)
{
  if (elapsed_ms >= 21800U)
  {
    return 3U;
  }
  if (elapsed_ms >= 15800U)
  {
    return 2U;
  }
  if (elapsed_ms >= 7800U)
  {
    return 1U;
  }
  return 0U;
}

static void UpperArm_ApplyScriptedTrainingEval(TrainingEvalSnapshot_t *eval, uint32_t now)
{
  uint32_t elapsed_ms;
  int32_t current_angle_x100;
  int32_t max_angle_x100;
  uint16_t target_reps;
  uint16_t reps;
  uint32_t completion;
  uint32_t amplitude;

  if (eval == NULL)
  {
    return;
  }

#if UPPER_SCRIPTED_TRAINING_EVAL
  elapsed_ms = UpperArm_GetTrainingElapsedMs(now);
  current_angle_x100 = UpperArm_ScriptedTrainingAngleX100(elapsed_ms, selected_action_index);
  max_angle_x100 = UpperArm_ScriptedTrainingMaxAngleX100(elapsed_ms, selected_action_index);
  target_reps = (eval->target_reps != 0U) ? eval->target_reps : training_target_count;
  reps = UpperArm_ScriptedTrainingReps(elapsed_ms);
  if ((target_reps != 0U) && (reps > target_reps))
  {
    reps = target_reps;
  }
  completion = (target_reps == 0U) ? 0U : (((uint32_t)reps * 100U) / (uint32_t)target_reps);
  if (completion > 100U)
  {
    completion = 100U;
  }
  amplitude = ((uint32_t)max_angle_x100 * 100U) /
              (uint32_t)UpperArm_ScriptedTrainingTargetAngleX100(selected_action_index);
  if (amplitude > 100U)
  {
    amplitude = 100U;
  }

  eval->reps = reps;
  eval->target_reps = target_reps;
  eval->current_angle_x100 = current_angle_x100;
  eval->max_angle_x100 = max_angle_x100;
  eval->avg_peak_angle_x100 = max_angle_x100;
  eval->completion_score = (uint8_t)completion;
  eval->amplitude_score = (uint8_t)amplitude;
  eval->total_score = (uint8_t)((completion * 50U + amplitude * 50U) / 100U);
#else
  (void)now;
#endif
}

static void UpperArm_AddHistoryRecord(void)
{
  RehabHistoryRecord_t *record = &rehab_history[rehab_history_next];
  TrainingEvalSnapshot_t eval;
  uint16_t target_reps;
  uint8_t completion_percent;

  if (settings_switch_state[2] == 0U)
  {
    return;
  }

  taskENTER_CRITICAL();
  eval = training_eval_snapshot;
  taskEXIT_CRITICAL();
  UpperArm_ApplyScriptedTrainingEval(&eval, osKernelGetTickCount());

  target_reps = (eval.target_reps != 0U) ? eval.target_reps : training_target_count;
  if (target_reps == 0U)
  {
    completion_percent = 0U;
  }
  else if (eval.reps >= target_reps)
  {
    completion_percent = 100U;
  }
  else
  {
    completion_percent = (uint8_t)(((uint32_t)eval.reps * 100U) / (uint32_t)target_reps);
  }

  record->tick_ms = osKernelGetTickCount();
  record->action_index = selected_action_index;
  record->score = eval.total_score;
  record->completion = completion_percent;
  record->amplitude = eval.amplitude_score;
  record->reps = eval.reps;
  record->target_reps = target_reps;
  record->duration_sec = training_target_duration_sec;
  record->max_angle = (uint16_t)(eval.max_angle_x100 / 100);
  memset(record->action_counts, 0, sizeof(record->action_counts));

  rehab_history_next = (uint8_t)((rehab_history_next + 1U) % REHAB_HISTORY_MAX_RECORDS);
  if (rehab_history_count < REHAB_HISTORY_MAX_RECORDS)
  {
    rehab_history_count++;
  }

  UpperArm_HistorySaveToFlash();
}

static void UpperArm_AddFreeHistoryRecord(void)
{
  RehabHistoryRecord_t *record = &rehab_history[rehab_history_next];
  uint8_t i;

  if (settings_switch_state[2] == 0U)
  {
    return;
  }

  record->tick_ms = osKernelGetTickCount();
  record->action_index = 4U;
  record->score = 0U;
  record->completion = 0U;
  record->amplitude = 0U;
  record->reps = free_total_reps;
  record->target_reps = 0U;
  record->duration_sec = training_target_duration_sec;
  record->max_angle = free_max_angle;
  for (i = 0U; i < 4U; i++)
  {
    record->action_counts[i] = free_action_counts[i];
  }

  rehab_history_next = (uint8_t)((rehab_history_next + 1U) % REHAB_HISTORY_MAX_RECORDS);
  if (rehab_history_count < REHAB_HISTORY_MAX_RECORDS)
  {
    rehab_history_count++;
  }

  UpperArm_HistorySaveToFlash();
}

static uint32_t UpperArm_HistoryChecksum(const RehabHistoryFlashData_t *data)
{
  const uint8_t *bytes = (const uint8_t *)data;
  uint32_t checksum = 0xA5A55A5AU;
  uint32_t i;

  if (data == NULL)
  {
    return 0U;
  }

  for (i = 0U; i < sizeof(*data); i++)
  {
    if ((i >= offsetof(RehabHistoryFlashData_t, checksum)) &&
        (i < (offsetof(RehabHistoryFlashData_t, checksum) + sizeof(data->checksum))))
    {
      continue;
    }
    checksum = (checksum << 5) - checksum + bytes[i];
  }

  return checksum;
}

static void UpperArm_HistorySaveToFlash(void)
{
  RehabHistoryFlashBlock_t block;
  FLASH_EraseInitTypeDef erase;
  uint32_t page_error = 0U;
  uint32_t address = REHAB_HISTORY_FLASH_PAGE_ADDRESS;
  uint32_t offset;

  memset(&block, 0xFF, sizeof(block));
  block.data.magic = REHAB_HISTORY_FLASH_MAGIC;
  block.data.version = REHAB_HISTORY_FLASH_VERSION;
  block.data.size = (uint16_t)sizeof(RehabHistoryFlashData_t);
  block.data.count = rehab_history_count;
  block.data.next = rehab_history_next;
  block.data.battery_percent = battery_cached_percent;
  block.data.battery_valid = ((battery_cached_valid != 0U) ? UPPER_FLASH_BATTERY_VALID_MASK : 0U) |
                             ((upper_night_mode != 0U) ? UPPER_FLASH_NIGHT_MODE_MASK : 0U);
  memcpy(block.data.records, rehab_history, sizeof(rehab_history));
  block.data.checksum = UpperArm_HistoryChecksum(&block.data);

  memset(&erase, 0, sizeof(erase));
  erase.TypeErase = FLASH_TYPEERASE_PAGES;
  erase.Banks = REHAB_HISTORY_FLASH_BANK;
  erase.Page = REHAB_HISTORY_FLASH_PAGE_INDEX;
  erase.NbPages = 1U;

  (void)HAL_FLASH_Unlock();
  if (HAL_FLASHEx_Erase(&erase, &page_error) == HAL_OK)
  {
    for (offset = 0U; offset < sizeof(block.raw); offset += 16U)
    {
      if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD,
                            address + offset,
                            (uint32_t)&block.raw[offset]) != HAL_OK)
      {
        break;
      }
    }
  }
  (void)HAL_FLASH_Lock();
}

void UpperArm_HistoryLoadFromFlash(void)
{
  const RehabHistoryFlashData_t *stored = (const RehabHistoryFlashData_t *)REHAB_HISTORY_FLASH_PAGE_ADDRESS;
  RehabHistoryFlashData_t copy;
  uint32_t checksum;

  if ((stored->magic != REHAB_HISTORY_FLASH_MAGIC) ||
      (stored->version != REHAB_HISTORY_FLASH_VERSION) ||
      (stored->size != sizeof(RehabHistoryFlashData_t)))
  {
    return;
  }

  memcpy(&copy, stored, sizeof(copy));
  checksum = UpperArm_HistoryChecksum(&copy);
  if (checksum != copy.checksum)
  {
    return;
  }

  if (copy.count > REHAB_HISTORY_MAX_RECORDS)
  {
    return;
  }

  rehab_history_count = copy.count;
  rehab_history_next = (uint8_t)(copy.next % REHAB_HISTORY_MAX_RECORDS);
  memcpy(rehab_history, copy.records, sizeof(rehab_history));
  if (((copy.battery_valid & UPPER_FLASH_BATTERY_VALID_MASK) != 0U) && (copy.battery_percent <= 100U))
  {
    battery_cached_percent = copy.battery_percent;
    battery_cached_valid = 1U;
  }
  upper_night_mode = ((copy.battery_valid & UPPER_FLASH_NIGHT_MODE_MASK) != 0U) ? 1U : 0U;
}

static void UpperArm_ClearHistory(void)
{
  memset(rehab_history, 0, sizeof(rehab_history));
  rehab_history_count = 0U;
  rehab_history_next = 0U;
  UpperArm_HistorySaveToFlash();
}

static void UpperArm_BatteryUpdateFromMeasured(uint8_t measured_percent)
{
  uint8_t should_update = 0U;

  if (measured_percent > 100U)
  {
    measured_percent = 100U;
  }

  if (battery_cached_valid == 0U)
  {
    should_update = 1U;
  }
  else if (measured_percent < battery_cached_percent)
  {
    should_update = 1U;
  }
  else if ((uint8_t)(measured_percent - battery_cached_percent) > UPPER_BATTERY_RISE_NOISE_PERCENT)
  {
    should_update = 1U;
  }

  if ((should_update != 0U) &&
      ((battery_cached_valid == 0U) || (battery_cached_percent != measured_percent)))
  {
    battery_cached_percent = measured_percent;
    battery_cached_valid = 1U;
    UpperArm_HistorySaveToFlash();
  }
}

static lv_color_t UpperArm_BatteryColor(uint8_t percent)
{
  if (percent < 20U)
  {
    return lv_color_hex(0xE74C3C);
  }
  if (percent < 50U)
  {
    return lv_color_hex(0xF2C94C);
  }
  return lv_color_hex(0x27AE60);
}

static lv_color_t UpperArm_ThemeColor(uint32_t light_hex, uint32_t dark_hex)
{
  return lv_color_hex((upper_night_mode != 0U) ? dark_hex : light_hex);
}

static const lv_img_dsc_t *UpperArm_GetHomeBg(void)
{
  return (upper_night_mode != 0U) ? &upper_home_bg_dark : &upper_home_bg;
}

static const lv_img_dsc_t *UpperArm_GetPrepBg(void)
{
  return (upper_night_mode != 0U) ? &upper_prep_bg_dark : &upper_prep_bg;
}

static void UpperArm_LvglApplyBatteryDisplay(lv_obj_t *fill,
                                             lv_obj_t *label,
                                             lv_coord_t max_width,
                                             uint8_t show_percent_sign)
{
  lv_coord_t fill_width = 0;

  if (battery_cached_valid != 0U)
  {
    fill_width = (lv_coord_t)(((uint32_t)battery_cached_percent * (uint32_t)max_width) / 100U);
    if ((battery_cached_percent > 0U) && (fill_width < 1))
    {
      fill_width = 1;
    }
  }

  if (label != NULL)
  {
    if (battery_cached_valid == 0U)
    {
      lv_label_set_text(label, (show_percent_sign != 0U) ? "--%" : "--");
    }
    else if (show_percent_sign != 0U)
    {
      lv_label_set_text_fmt(label, "%u%%", (unsigned int)battery_cached_percent);
    }
    else
    {
      lv_label_set_text_fmt(label, "%u", (unsigned int)battery_cached_percent);
    }
  }

  if (fill != NULL)
  {
    lv_obj_set_width(fill, fill_width);
    if (battery_cached_valid != 0U)
    {
      lv_obj_set_style_bg_color(fill, UpperArm_BatteryColor(battery_cached_percent), 0);
    }
  }
}

static uint8_t UpperArm_WirelessIsConnected(uint32_t now)
{
  uint8_t wrist_valid_snapshot;
  uint8_t wireless_enabled_snapshot;
  uint32_t last_frame_tick_snapshot;

  taskENTER_CRITICAL();
  wrist_valid_snapshot = latest_wrist_valid;
  wireless_enabled_snapshot = wireless_rx_enabled;
  last_frame_tick_snapshot = wireless_last_frame_tick;
  taskEXIT_CRITICAL();

  if ((wrist_valid_snapshot == 0U) || (wireless_enabled_snapshot == 0U) || (last_frame_tick_snapshot == 0U))
  {
    return 0U;
  }

  return ((now - last_frame_tick_snapshot) <= WIRELESS_LINK_TIMEOUT_MS) ? 1U : 0U;
}

static void UpperArm_RequestVibration(void)
{
  if (settings_switch_state[1] != 0U)
  {
    Vibration_Pulse(UPPER_VIBRATION_PULSE_MS);
  }
}

static uint8_t UpperArm_DebounceStateChanged(uint8_t sample_pressed,
                                             uint8_t *stable_pressed,
                                             uint8_t *last_sample_pressed,
                                             uint32_t *last_change_tick,
                                             uint32_t now)
{
  if (sample_pressed != *last_sample_pressed)
  {
    *last_sample_pressed = sample_pressed;
    *last_change_tick = now;
  }

  if (((now - *last_change_tick) >= UPPER_HW_KEY_DEBOUNCE_MS) &&
      (sample_pressed != *stable_pressed))
  {
    *stable_pressed = sample_pressed;
    return 1U;
  }

  return 0U;
}

void UpperArm_SubmitVoiceCommand(uint8_t cmd)
{
  taskENTER_CRITICAL();
  upper_voice_pending_cmd = cmd;
  upper_voice_has_pending = 1U;
  taskEXIT_CRITICAL();
}

uint8_t UpperArm_ProcessPendingVoiceCommand(void)
{
  uint8_t cmd;

  if (upper_voice_has_pending == 0U)
  {
    return 0U;
  }

  taskENTER_CRITICAL();
  cmd = upper_voice_pending_cmd;
  upper_voice_has_pending = 0U;
  taskEXIT_CRITICAL();

  UpperArm_HandleVoiceCommand(cmd);
  return cmd;
}

uint8_t UpperArm_LvglIsHomePage(void)
{
  return (current_ui_page == UPPER_PAGE_HOME) ? 1U : 0U;
}

static void UpperArm_SelectActionByVoice(uint8_t action_index)
{
  if (action_index >= 4U)
  {
    return;
  }

  if (current_ui_page != UPPER_PAGE_ACTION_SELECT)
  {
    return;
  }

  selected_action_index = action_index;
  UpperArm_LvglSetTransitionDir(0);
  UpperArm_LvglCreateActionSelect();
  UpperArm_LvglUpdateHome(osKernelGetTickCount());
}

static void UpperArm_HandleVoiceCommand(uint8_t cmd)
{
  uint32_t now = osKernelGetTickCount();

  switch (cmd)
  {
    case UPPER_VOICE_CMD_WAKE:
      break;

    case UPPER_VOICE_CMD_START_TRAIN:
    case UPPER_VOICE_CMD_SWITCH_ACTION:
      UpperArm_LvglSetTransitionDir(1);
      UpperArm_LvglCreateActionSelect();
      break;

    case UPPER_VOICE_CMD_FINISH_TRAIN:
      if ((current_ui_page == UPPER_PAGE_TRAINING) ||
          (current_ui_page == UPPER_PAGE_FREE_TRAINING))
      {
        UpperArm_OnFinishTrainingClicked(NULL);
      }
      break;

    case UPPER_VOICE_CMD_PAUSE_TRAIN:
    case UPPER_VOICE_CMD_TIRED:
      if ((current_ui_page == UPPER_PAGE_TRAINING) && (training_paused == 0U))
      {
        UpperArm_OnPauseTrainingClicked(NULL);
      }
      break;

    case UPPER_VOICE_CMD_CONTINUE_TRAIN:
      if ((current_ui_page == UPPER_PAGE_TRAINING) && (training_paused != 0U))
      {
        UpperArm_OnPauseTrainingClicked(NULL);
      }
      break;

    case UPPER_VOICE_CMD_ACTION_ELBOW:
      UpperArm_SelectActionByVoice(0U);
      break;

    case UPPER_VOICE_CMD_ACTION_ARM_ABD:
      UpperArm_SelectActionByVoice(1U);
      break;

    case UPPER_VOICE_CMD_ACTION_FOREARM:
      UpperArm_SelectActionByVoice(2U);
      break;

    case UPPER_VOICE_CMD_ACTION_SHOULDER:
      UpperArm_SelectActionByVoice(3U);
      break;

    case UPPER_VOICE_CMD_ACTION_READY:
      if (current_ui_page == UPPER_PAGE_ACTION_SELECT)
      {
        UpperArm_OnStartTrainingClicked(NULL);
      }
      break;

    case UPPER_VOICE_CMD_VIBRATION_ON:
      settings_switch_state[1] = 1U;
      UpperArm_RequestVibration();
      UpperArm_LvglSetTransitionDir(1);
      UpperArm_LvglCreateSettings();
      break;

    case UPPER_VOICE_CMD_VIBRATION_OFF:
      settings_switch_state[1] = 0U;
      UpperArm_LvglSetTransitionDir(1);
      UpperArm_LvglCreateSettings();
      break;

    case UPPER_VOICE_CMD_HOME:
      UpperArm_LvglSetTransitionDir(-1);
      UpperArm_LvglCreateHome();
      break;

    case UPPER_VOICE_CMD_HISTORY:
      UpperArm_LvglSetTransitionDir(1);
      UpperArm_LvglCreateHistory();
      break;

    case UPPER_VOICE_CMD_FREE_TRAIN:
      if (UpperArm_WirelessIsConnected(now) == 0U)
      {
        UpperArm_LvglSetTransitionDir(1);
        UpperArm_LvglCreateLinkLost();
      }
      else
      {
        prep_for_free_mode = 1U;
        UpperArm_LvglSetTransitionDir(1);
        UpperArm_LvglCreatePrep();
      }
      break;

    case UPPER_VOICE_CMD_SETTINGS:
      UpperArm_LvglSetTransitionDir(1);
      UpperArm_LvglCreateSettings();
      break;

    case UPPER_VOICE_CMD_HEART_RATE:
      if (UpperArm_WirelessIsConnected(now) == 0U)
      {
        UpperArm_LvglSetTransitionDir(1);
        UpperArm_LvglCreateLinkLost();
      }
      else
      {
        taskENTER_CRITICAL();
        wireless_health_start_pending = 1U;
        upper_health_request_until_tick = now + ESP8266_HEALTH_REQUEST_HOLD_MS;
        taskEXIT_CRITICAL();
      }
      break;

    default:
      break;
  }
}

void UpperArm_HandleHardwareKeys(uint32_t now)
{
  static uint8_t initialized;
  static uint8_t user_stable_pressed;
  static uint8_t user_last_sample_pressed;
  static uint32_t user_last_change_tick;
  static uint32_t user_press_tick;
  static UpperPage_t user_page_on_press;
  static uint8_t user_long_handled;
  static uint8_t mode_stable_pressed;
  static uint8_t mode_last_sample_pressed;
  static uint32_t mode_last_change_tick;
  static uint32_t mode_press_tick;
  static UpperPage_t mode_page_on_press;
  static uint8_t mode_long_handled;
  uint8_t user_pressed;
  uint8_t mode_pressed;

  user_pressed = (HAL_GPIO_ReadPin(USER_KEY_GPIO_Port, USER_KEY_Pin) == GPIO_PIN_RESET) ? 1U : 0U;
  mode_pressed = (HAL_GPIO_ReadPin(MODE_KEY_GPIO_Port, MODE_KEY_Pin) == GPIO_PIN_RESET) ? 1U : 0U;

  if (initialized == 0U)
  {
    initialized = 1U;
    user_stable_pressed = user_pressed;
    user_last_sample_pressed = user_pressed;
    user_last_change_tick = now;
    user_press_tick = now;
    user_page_on_press = current_ui_page;
    user_long_handled = 0U;
    mode_stable_pressed = mode_pressed;
    mode_last_sample_pressed = mode_pressed;
    mode_last_change_tick = now;
    mode_press_tick = now;
    mode_page_on_press = current_ui_page;
    mode_long_handled = 0U;
    return;
  }

  if (UpperArm_DebounceStateChanged(user_pressed,
                                    &user_stable_pressed,
                                    &user_last_sample_pressed,
                                    &user_last_change_tick,
                                    now) != 0U)
  {
    if (user_stable_pressed != 0U)
    {
      user_press_tick = now;
      user_page_on_press = current_ui_page;
      user_long_handled = 0U;
      if (current_ui_page != UPPER_PAGE_SETTINGS)
      {
        UpperArm_LvglSetTransitionDir(1);
        UpperArm_LvglCreateSettings();
      }
    }
  }

  if (UpperArm_DebounceStateChanged(mode_pressed,
                                    &mode_stable_pressed,
                                    &mode_last_sample_pressed,
                                    &mode_last_change_tick,
                                    now) != 0U)
  {
    if (mode_stable_pressed != 0U)
    {
      mode_press_tick = now;
      mode_page_on_press = current_ui_page;
      mode_long_handled = 0U;
      if (current_ui_page != UPPER_PAGE_TRAINING)
      {
        prep_for_free_mode = 0U;
        UpperArm_LvglSetTransitionDir(1);
        if (UpperArm_WirelessIsConnected(now) == 0U)
        {
          if (current_ui_page != UPPER_PAGE_LINK_LOST)
          {
            UpperArm_LvglCreateLinkLost();
          }
        }
        else if (current_ui_page != UPPER_PAGE_ACTION_SELECT)
        {
          UpperArm_LvglCreateActionSelect();
          UpperArm_LvglUpdateHome(now);
        }
      }
    }
    else if ((mode_page_on_press == UPPER_PAGE_TRAINING) &&
             (current_ui_page == UPPER_PAGE_TRAINING) &&
             (mode_long_handled == 0U))
    {
      UpperArm_OnPauseTrainingClicked(NULL);
    }
  }

  if ((user_stable_pressed != 0U) &&
      (user_long_handled == 0U) &&
      (user_page_on_press == UPPER_PAGE_SETTINGS) &&
      (current_ui_page == UPPER_PAGE_SETTINGS) &&
      ((now - user_press_tick) >= UPPER_HW_KEY_LONG_PRESS_MS))
  {
    user_long_handled = 1U;
    UpperArm_LvglSetTransitionDir(-1);
    UpperArm_LvglCreateHome();
  }

  if ((mode_stable_pressed != 0U) &&
      (mode_long_handled == 0U) &&
      (mode_page_on_press == UPPER_PAGE_TRAINING) &&
      (current_ui_page == UPPER_PAGE_TRAINING) &&
      ((now - mode_press_tick) >= UPPER_HW_KEY_LONG_PRESS_MS))
  {
    mode_long_handled = 1U;
    UpperArm_OnFinishTrainingClicked(NULL);
  }
}

static void UpperArm_LvglCreateHomeIcon(lv_obj_t *parent, uint8_t type, uint8_t primary)
{
  const lv_color_t blue = (primary != 0U) ? lv_color_hex(0xFFFFFF) : lv_color_hex(0x2F80ED);
  const lv_color_t light_blue = (primary != 0U) ? lv_color_hex(0xA8D5FF) : lv_color_hex(0x56CCF2);
  static lv_point_t chart_line[] = {
    {5, 18}, {15, 10}, {25, 14}, {36, 4}
  };

  if (type == 0U)
  {
    lv_obj_t *outer = lv_arc_create(parent);
    lv_obj_t *inner = lv_arc_create(parent);
    lv_obj_t *play = lv_label_create(parent);

    lv_obj_remove_style_all(outer);
    lv_obj_set_size(outer, 38, 38);
    lv_obj_center(outer);
    lv_arc_set_bg_angles(outer, 0, 360);
    lv_arc_set_angles(outer, 30, 330);
    lv_obj_set_style_arc_color(outer, light_blue, LV_PART_MAIN);
    lv_obj_set_style_arc_color(outer, lv_color_hex(0xFFFFFF), LV_PART_INDICATOR);
    lv_obj_set_style_arc_opa(outer, LV_OPA_60, LV_PART_MAIN);
    lv_obj_set_style_arc_opa(outer, LV_OPA_80, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(outer, 4, LV_PART_MAIN);
    lv_obj_set_style_arc_width(outer, 4, LV_PART_INDICATOR);
    lv_obj_clear_flag(outer, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_remove_style_all(inner);
    lv_obj_set_size(inner, 30, 30);
    lv_obj_center(inner);
    lv_arc_set_bg_angles(inner, 0, 360);
    lv_arc_set_angles(inner, 0, 360);
    lv_obj_set_style_arc_color(inner, blue, LV_PART_MAIN);
    lv_obj_set_style_arc_opa(inner, LV_OPA_70, LV_PART_MAIN);
    lv_obj_set_style_arc_width(inner, 3, LV_PART_MAIN);
    lv_obj_clear_flag(inner, LV_OBJ_FLAG_CLICKABLE);

    lv_label_set_text(play, LV_SYMBOL_PLAY);
    lv_obj_set_style_text_color(play, blue, 0);
    lv_obj_set_style_text_font(play, &lv_font_montserrat_24, 0);
    lv_obj_align(play, LV_ALIGN_CENTER, 2, 1);
  }
  else if (type == 1U)
  {
    lv_obj_t *paper = UpperArm_LvglShape(parent, 30, 34, 6, 1, blue, 4);
    lv_obj_t *clock = UpperArm_LvglShape(parent, 15, 15, 27, 25, blue, LV_RADIUS_CIRCLE);
    lv_obj_t *hand;
    static lv_point_t clock_hand[] = {
      {8, 4}, {8, 9}, {12, 9}
    };

    (void)UpperArm_LvglShape(paper, 12, 4, 9, 3, lv_color_hex(0xA8D5FF), 2);
    (void)UpperArm_LvglShape(paper, 18, 3, 6, 13, lv_color_hex(0xFFFFFF), 2);
    (void)UpperArm_LvglShape(paper, 18, 3, 6, 20, lv_color_hex(0xFFFFFF), 2);
    (void)UpperArm_LvglShape(paper, 18, 3, 6, 27, lv_color_hex(0xFFFFFF), 2);
    (void)UpperArm_LvglShape(clock, 9, 9, 3, 3, lv_color_hex(0xFFFFFF), LV_RADIUS_CIRCLE);
    hand = lv_line_create(clock);
    lv_line_set_points(hand, clock_hand, sizeof(clock_hand) / sizeof(clock_hand[0]));
    lv_obj_set_style_line_color(hand, blue, 0);
    lv_obj_set_style_line_width(hand, 2, 0);
  }
  else if (type == 2U)
  {
    lv_obj_t *line;
    (void)chart_line;

    (void)UpperArm_LvglShape(parent, 14, 14, 14, 14, blue, LV_RADIUS_CIRCLE);
    (void)UpperArm_LvglShape(parent, 6, 6, 18, 18, lv_color_hex(0xFFFFFF), LV_RADIUS_CIRCLE);
    (void)UpperArm_LvglShape(parent, 8, 8, 18, 1, light_blue, LV_RADIUS_CIRCLE);
    (void)UpperArm_LvglShape(parent, 8, 8, 18, 35, blue, LV_RADIUS_CIRCLE);
    (void)UpperArm_LvglShape(parent, 8, 8, 1, 18, blue, LV_RADIUS_CIRCLE);
    (void)UpperArm_LvglShape(parent, 8, 8, 35, 18, light_blue, LV_RADIUS_CIRCLE);

    line = lv_line_create(parent);
    {
      static lv_point_t free_line_1[] = {{22, 8}, {22, 14}, {22, 28}, {22, 35}};
      lv_line_set_points(line, free_line_1, sizeof(free_line_1) / sizeof(free_line_1[0]));
    }
    lv_obj_set_style_line_color(line, blue, 0);
    lv_obj_set_style_line_width(line, 2, 0);
    lv_obj_set_style_line_rounded(line, true, 0);

    line = lv_line_create(parent);
    {
      static lv_point_t free_line_2[] = {{8, 22}, {14, 22}, {30, 22}, {35, 22}};
      lv_line_set_points(line, free_line_2, sizeof(free_line_2) / sizeof(free_line_2[0]));
    }
    lv_obj_set_style_line_color(line, blue, 0);
    lv_obj_set_style_line_width(line, 2, 0);
    lv_obj_set_style_line_rounded(line, true, 0);
  }
  else
  {
    (void)UpperArm_LvglShape(parent, 7, 7, 18, 2, blue, LV_RADIUS_CIRCLE);
    (void)UpperArm_LvglShape(parent, 7, 7, 18, 34, blue, LV_RADIUS_CIRCLE);
    (void)UpperArm_LvglShape(parent, 7, 7, 2, 18, blue, LV_RADIUS_CIRCLE);
    (void)UpperArm_LvglShape(parent, 7, 7, 34, 18, blue, LV_RADIUS_CIRCLE);
    (void)UpperArm_LvglShape(parent, 27, 27, 8, 8, blue, LV_RADIUS_CIRCLE);
    (void)UpperArm_LvglShape(parent, 11, 11, 16, 16, lv_color_hex(0xF4FAFF), LV_RADIUS_CIRCLE);
  }
}

static void UpperArm_LvglCreateResultMetricIcon(lv_obj_t *parent, uint8_t type)
{
  const lv_color_t blue = lv_color_hex(0x126BFF);
  const lv_color_t light = lv_color_hex(0xEAF3FF);
  lv_obj_t *line;

  if (type == 0U)
  {
    (void)UpperArm_LvglShape(parent, 14, 14, 2, 2, blue, LV_RADIUS_CIRCLE);
    (void)UpperArm_LvglShape(parent, 10, 10, 4, 4, light, LV_RADIUS_CIRCLE);
    (void)UpperArm_LvglShape(parent, 5, 5, 7, 7, blue, LV_RADIUS_CIRCLE);
    line = lv_line_create(parent);
    {
      static lv_point_t target_arrow[] = {{9, 9}, {15, 3}, {15, 7}};
      lv_line_set_points(line, target_arrow, sizeof(target_arrow) / sizeof(target_arrow[0]));
    }
    lv_obj_set_style_line_color(line, blue, 0);
    lv_obj_set_style_line_width(line, 2, 0);
    lv_obj_set_style_line_rounded(line, true, 0);
  }
  else if (type == 1U)
  {
    line = lv_line_create(parent);
    {
      static lv_point_t wave[] = {{1, 10}, {4, 10}, {6, 5}, {9, 14}, {12, 7}, {14, 10}, {17, 10}};
      lv_line_set_points(line, wave, sizeof(wave) / sizeof(wave[0]));
    }
    lv_obj_set_style_line_color(line, blue, 0);
    lv_obj_set_style_line_width(line, 3, 0);
    lv_obj_set_style_line_rounded(line, true, 0);
  }
  else if (type == 2U)
  {
    (void)UpperArm_LvglShape(parent, 14, 13, 2, 4, blue, 2);
    (void)UpperArm_LvglShape(parent, 10, 8, 4, 7, lv_color_hex(0xFFFFFF), 1);
    (void)UpperArm_LvglShape(parent, 2, 4, 5, 1, blue, 1);
    (void)UpperArm_LvglShape(parent, 2, 4, 11, 1, blue, 1);
    (void)UpperArm_LvglShape(parent, 2, 2, 5, 9, blue, 1);
    (void)UpperArm_LvglShape(parent, 2, 2, 9, 9, blue, 1);
    (void)UpperArm_LvglShape(parent, 2, 2, 5, 12, blue, 1);
    (void)UpperArm_LvglShape(parent, 2, 2, 9, 12, blue, 1);
  }
  else
  {
    line = lv_line_create(parent);
    {
      static lv_point_t angle_line[] = {{3, 14}, {7, 14}, {14, 5}};
      lv_line_set_points(line, angle_line, sizeof(angle_line) / sizeof(angle_line[0]));
    }
    lv_obj_set_style_line_color(line, blue, 0);
    lv_obj_set_style_line_width(line, 3, 0);
    lv_obj_set_style_line_rounded(line, true, 0);
    (void)UpperArm_LvglShape(parent, 12, 2, 3, 15, blue, 1);
  }
}

static void UpperArm_LvglCreateSettingsSwitch(lv_obj_t *parent,
                                              lv_coord_t x,
                                              lv_coord_t y,
                                              uint8_t index,
                                              uint8_t on)
{
  lv_obj_t *sw = lv_switch_create(parent);

  lv_obj_set_size(sw, 39, 22);
  lv_obj_set_pos(sw, x, y);
  lv_obj_set_style_radius(sw, 11, LV_PART_MAIN);
  lv_obj_set_style_bg_color(sw, UpperArm_ThemeColor(0xDCE4F2, 0x10223F), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(sw, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_bg_color(sw, UpperArm_ThemeColor(0x126BFF, 0x2F80ED), LV_PART_INDICATOR | LV_STATE_CHECKED);
  lv_obj_set_style_bg_opa(sw, LV_OPA_COVER, LV_PART_INDICATOR | LV_STATE_CHECKED);
  lv_obj_set_style_bg_color(sw, lv_color_hex(0xFFFFFF), LV_PART_KNOB);
  lv_obj_set_style_bg_opa(sw, LV_OPA_COVER, LV_PART_KNOB);
  lv_obj_set_style_shadow_width(sw, 3, LV_PART_KNOB);
  lv_obj_set_style_shadow_color(sw, lv_color_hex(0x9FB2D0), LV_PART_KNOB);
  lv_obj_set_style_shadow_opa(sw, LV_OPA_30, LV_PART_KNOB);
  if (on != 0U)
  {
    lv_obj_add_state(sw, LV_STATE_CHECKED);
  }
  lv_obj_add_event_cb(sw, UpperArm_OnSettingsSwitchChanged, LV_EVENT_VALUE_CHANGED, (void *)(uintptr_t)index);
}

static void UpperArm_LvglCreateSettingsIcon(lv_obj_t *parent, uint8_t type)
{
  lv_obj_t *label;
  lv_obj_t *line;

  if (type == 1U)
  {
    (void)UpperArm_LvglShape(parent, 10, 17, 7, 5, UpperArm_ThemeColor(0x126BFF, 0x6AB7FF), 2);
    (void)UpperArm_LvglShape(parent, 6, 13, 9, 7, UpperArm_ThemeColor(0xFFFFFF, 0x0B1830), 1);
    line = lv_line_create(parent);
    {
      static lv_point_t vibration_left[] = {{4, 8}, {2, 11}, {4, 14}};
      lv_line_set_points(line, vibration_left, sizeof(vibration_left) / sizeof(vibration_left[0]));
    }
    lv_obj_set_style_line_color(line, UpperArm_ThemeColor(0x126BFF, 0x6AB7FF), 0);
    lv_obj_set_style_line_width(line, 2, 0);
    lv_obj_set_style_line_rounded(line, true, 0);
    line = lv_line_create(parent);
    {
      static lv_point_t vibration_right[] = {{20, 8}, {22, 11}, {20, 14}};
      lv_line_set_points(line, vibration_right, sizeof(vibration_right) / sizeof(vibration_right[0]));
    }
    lv_obj_set_style_line_color(line, UpperArm_ThemeColor(0x126BFF, 0x6AB7FF), 0);
    lv_obj_set_style_line_width(line, 2, 0);
    lv_obj_set_style_line_rounded(line, true, 0);
  }
  else if (type == 3U)
  {
    line = lv_line_create(parent);
    {
      static lv_point_t target_arrow[] = {{12, 12}, {19, 5}, {19, 9}};
      lv_line_set_points(line, target_arrow, sizeof(target_arrow) / sizeof(target_arrow[0]));
    }
    (void)UpperArm_LvglShape(parent, 20, 20, 2, 2, UpperArm_ThemeColor(0x126BFF, 0x6AB7FF), LV_RADIUS_CIRCLE);
    (void)UpperArm_LvglShape(parent, 14, 14, 5, 5, UpperArm_ThemeColor(0xEAF3FF, 0x0B1830), LV_RADIUS_CIRCLE);
    (void)UpperArm_LvglShape(parent, 8, 8, 8, 8, UpperArm_ThemeColor(0x126BFF, 0x6AB7FF), LV_RADIUS_CIRCLE);
    (void)UpperArm_LvglShape(parent, 4, 4, 10, 10, UpperArm_ThemeColor(0xEAF3FF, 0x0B1830), LV_RADIUS_CIRCLE);
    lv_obj_set_style_line_color(line, UpperArm_ThemeColor(0x126BFF, 0x6AB7FF), 0);
    lv_obj_set_style_line_width(line, 2, 0);
    lv_obj_set_style_line_rounded(line, true, 0);
  }
  else if (type == 4U)
  {
    (void)UpperArm_LvglShape(parent, 20, 20, 2, 2, UpperArm_ThemeColor(0x126BFF, 0x6AB7FF), LV_RADIUS_CIRCLE);
    (void)UpperArm_LvglShape(parent, 14, 14, 5, 5, UpperArm_ThemeColor(0xFFFFFF, 0x0B1830), LV_RADIUS_CIRCLE);
    line = lv_line_create(parent);
    {
      static lv_point_t clock_hand[] = {{12, 12}, {12, 6}, {17, 12}};
      lv_line_set_points(line, clock_hand, sizeof(clock_hand) / sizeof(clock_hand[0]));
    }
    lv_obj_set_style_line_color(line, UpperArm_ThemeColor(0x126BFF, 0x6AB7FF), 0);
    lv_obj_set_style_line_width(line, 2, 0);
    lv_obj_set_style_line_rounded(line, true, 0);
    (void)UpperArm_LvglShape(parent, 4, 4, 10, 10, UpperArm_ThemeColor(0x126BFF, 0x6AB7FF), LV_RADIUS_CIRCLE);
  }
  else if (type == UPPER_SETTINGS_NIGHT_SWITCH_INDEX)
  {
    (void)UpperArm_LvglShape(parent, 16, 16, 4, 4, UpperArm_ThemeColor(0x126BFF, 0x6AB7FF), LV_RADIUS_CIRCLE);
    (void)UpperArm_LvglShape(parent, 12, 16, 9, 2, UpperArm_ThemeColor(0xEAF3FF, 0x0B1830), LV_RADIUS_CIRCLE);
    (void)UpperArm_LvglShape(parent, 2, 2, 17, 17, UpperArm_ThemeColor(0x126BFF, 0x6AB7FF), LV_RADIUS_CIRCLE);
  }
  else
  {
    static const char *symbols[3] = {
      LV_SYMBOL_VOLUME_MAX, LV_SYMBOL_BELL, LV_SYMBOL_SAVE
    };

    if (type >= (sizeof(symbols) / sizeof(symbols[0])))
    {
      return;
    }

    label = lv_label_create(parent);
    lv_label_set_text(label, symbols[type]);
    lv_obj_set_style_text_color(label, UpperArm_ThemeColor(0x126BFF, 0x6AB7FF), 0);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_16, 0);
    lv_obj_center(label);
  }
}

static void UpperArm_LvglCreateSettingsRow(lv_obj_t *panel,
                                           uint8_t index,
                                           const char *title_text,
                                           const char *hint_text,
                                           uint8_t on)
{
  const lv_coord_t row_y = (lv_coord_t)(index * UPPER_SETTINGS_ROW_H);
  lv_obj_t *icon_bg;
  lv_obj_t *icon_box;
  lv_obj_t *title;
  lv_obj_t *hint;
  lv_obj_t *divider;

  if (index > 0U)
  {
    divider = UpperArm_LvglShape(panel, 190, 1, 12, row_y, UpperArm_ThemeColor(0xE1ECFF, 0x18365F), 0);
    lv_obj_clear_flag(divider, LV_OBJ_FLAG_CLICKABLE);
  }

  icon_bg = UpperArm_LvglShape(panel, 26, 26, 9, row_y + 3, UpperArm_ThemeColor(0xEAF3FF, 0x10284A), LV_RADIUS_CIRCLE);
  lv_obj_clear_flag(icon_bg, LV_OBJ_FLAG_CLICKABLE);

  icon_box = lv_obj_create(icon_bg);
  lv_obj_remove_style_all(icon_box);
  lv_obj_set_size(icon_box, 24, 24);
  lv_obj_center(icon_box);
  lv_obj_clear_flag(icon_box, LV_OBJ_FLAG_SCROLLABLE);
  UpperArm_LvglCreateSettingsIcon(icon_box, index);

  title = lv_label_create(panel);
  lv_label_set_text(title, title_text);
  lv_obj_set_width(title, 108);
  lv_label_set_long_mode(title, LV_LABEL_LONG_DOT);
  lv_obj_set_style_text_color(title, UpperArm_ThemeColor(0x102B6B, 0xDCEBFF), 0);
  lv_obj_set_style_text_font(title, &upper_font_zh_14, 0);
  lv_obj_set_pos(title, 47, row_y + 4);

  hint = lv_label_create(panel);
  lv_label_set_text(hint, hint_text);
  lv_obj_set_width(hint, 108);
  lv_label_set_long_mode(hint, LV_LABEL_LONG_DOT);
  lv_obj_set_style_text_color(hint, UpperArm_ThemeColor(0x5D7098, 0x89A8D8), 0);
  lv_obj_set_style_text_font(hint, &upper_font_zh_10, 0);
  lv_obj_set_pos(hint, 47, row_y + 19);

  UpperArm_LvglCreateSettingsSwitch(panel, 163, row_y + 5, index, on);
}

static void UpperArm_LvglCreateSettingsAdjustButton(lv_obj_t *parent,
                                                    lv_coord_t x,
                                                    lv_coord_t y,
                                                    const char *text,
                                                    uintptr_t action)
{
  lv_obj_t *btn = lv_btn_create(parent);
  lv_obj_t *label;

  lv_obj_remove_style_all(btn);
  lv_obj_set_size(btn, 22, 22);
  lv_obj_set_pos(btn, x, y);
  lv_obj_set_style_radius(btn, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(btn, UpperArm_ThemeColor(0xEAF3FF, 0x10284A), 0);
  lv_obj_set_style_bg_color(btn, UpperArm_ThemeColor(0x126BFF, 0x2F80ED), LV_STATE_PRESSED);
  lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(btn, 1, 0);
  lv_obj_set_style_border_color(btn, UpperArm_ThemeColor(0xC9DCFA, 0x244B7A), 0);
  lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_event_cb(btn, UpperArm_OnSettingsAdjustClicked, LV_EVENT_CLICKED, (void *)action);

  label = lv_label_create(btn);
  lv_label_set_text(label, text);
  lv_obj_set_style_text_color(label, UpperArm_ThemeColor(0x126BFF, 0xDCEBFF), 0);
  lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), LV_STATE_PRESSED);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_16, 0);
  lv_obj_center(label);
  lv_obj_clear_flag(label, LV_OBJ_FLAG_CLICKABLE);
}

static void UpperArm_LvglCreateSettingsValueRow(lv_obj_t *panel,
                                                uint8_t index,
                                                const char *title_text,
                                                const char *hint_text,
                                                uint8_t value_type)
{
  const lv_coord_t row_y = (lv_coord_t)(index * UPPER_SETTINGS_ROW_H);
  lv_obj_t *icon_bg;
  lv_obj_t *icon_box;
  lv_obj_t *title;
  lv_obj_t *hint;
  lv_obj_t *divider;
  lv_obj_t *value;

  divider = UpperArm_LvglShape(panel, 190, 1, 12, row_y, UpperArm_ThemeColor(0xE1ECFF, 0x18365F), 0);
  lv_obj_clear_flag(divider, LV_OBJ_FLAG_CLICKABLE);

  icon_bg = UpperArm_LvglShape(panel, 26, 26, 9, row_y + 3, UpperArm_ThemeColor(0xEAF3FF, 0x10284A), LV_RADIUS_CIRCLE);
  lv_obj_clear_flag(icon_bg, LV_OBJ_FLAG_CLICKABLE);

  icon_box = lv_obj_create(icon_bg);
  lv_obj_remove_style_all(icon_box);
  lv_obj_set_size(icon_box, 24, 24);
  lv_obj_center(icon_box);
  lv_obj_clear_flag(icon_box, LV_OBJ_FLAG_SCROLLABLE);
  UpperArm_LvglCreateSettingsIcon(icon_box, index);

  title = lv_label_create(panel);
  lv_label_set_text(title, title_text);
  lv_obj_set_width(title, 84);
  lv_label_set_long_mode(title, LV_LABEL_LONG_DOT);
  lv_obj_set_style_text_color(title, UpperArm_ThemeColor(0x102B6B, 0xDCEBFF), 0);
  lv_obj_set_style_text_font(title, &upper_font_zh_14, 0);
  lv_obj_set_pos(title, 47, row_y + 4);

  hint = lv_label_create(panel);
  lv_label_set_text(hint, hint_text);
  lv_obj_set_width(hint, 84);
  lv_label_set_long_mode(hint, LV_LABEL_LONG_DOT);
  lv_obj_set_style_text_color(hint, UpperArm_ThemeColor(0x5D7098, 0x89A8D8), 0);
  lv_obj_set_style_text_font(hint, &upper_font_zh_10, 0);
  lv_obj_set_pos(hint, 47, row_y + 19);

  UpperArm_LvglCreateSettingsAdjustButton(panel, 136, row_y + 5, "-", (value_type == 0U) ? 0U : 2U);

  value = lv_label_create(panel);
  lv_obj_set_width(value, 35);
  lv_obj_set_style_text_align(value, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_style_text_color(value, UpperArm_ThemeColor(0x126BFF, 0x6AB7FF), 0);
  lv_obj_set_style_text_font(value, &upper_font_zh_12, 0);
  lv_obj_set_pos(value, 160, row_y + 8);
  if (value_type == 0U)
  {
    settings_target_count_label = value;
    lv_label_set_text_fmt(value, "%u%s", (unsigned int)training_target_count, UI_ZH_COUNT_UNIT);
  }
  else
  {
    settings_duration_label = value;
    lv_label_set_text_fmt(value, "%u%s", (unsigned int)training_target_duration_sec, UI_ZH_SECOND_UNIT);
  }

  UpperArm_LvglCreateSettingsAdjustButton(panel, 193, row_y + 5, "+", (value_type == 0U) ? 1U : 3U);
}

static void UpperArm_LvglResetPageObjectRefs(void)
{
  home_time_label = NULL;
  home_today_count_label = NULL;
  home_status_value_label = NULL;
  home_status_dot = NULL;
  home_bt_icon = NULL;
  home_battery_fill = NULL;
  home_battery_value_label = NULL;
  home_imu_angle_label = NULL;
  prep_countdown_label = NULL;
  prep_spinner_arc = NULL;
  memset(prep_status_dots, 0, sizeof(prep_status_dots));
  prep_time_label = NULL;
  action_time_label = NULL;
  training_time_label = NULL;
  training_pause_icon_label = NULL;
  training_pause_text_label = NULL;
  training_count_label = NULL;
  training_timer_arc = NULL;
  training_timer_value_label = NULL;
  training_score_value_label = NULL;
  training_emg_mode_label = NULL;
  training_emg_strength_label = NULL;
  training_completion_bar = NULL;
  training_completion_value_label = NULL;
  training_amplitude_bar = NULL;
  training_amplitude_value_label = NULL;
  training_angle_value_label = NULL;
  training_angle_chart = NULL;
  training_angle_series = NULL;
  training_angle_chart_last_tick = 0U;
  free_action_label = NULL;
  free_count_label = NULL;
  free_angle_label = NULL;
  memset(free_action_count_labels, 0, sizeof(free_action_count_labels));
  result_time_label = NULL;
  settings_target_count_label = NULL;
  settings_duration_label = NULL;
}

static lv_obj_t *UpperArm_LvglCreateBluetoothIcon(lv_obj_t *parent,
                                                  lv_coord_t x,
                                                  lv_coord_t y,
                                                  lv_color_t color,
                                                  const lv_font_t *font)
{
  lv_obj_t *icon = lv_label_create(parent);

  lv_label_set_text(icon, LV_SYMBOL_BLUETOOTH);
  lv_obj_set_style_text_color(icon, color, 0);
  lv_obj_set_style_text_font(icon, font, 0);
  lv_obj_set_pos(icon, x, y);
  lv_obj_clear_flag(icon, LV_OBJ_FLAG_CLICKABLE);

  return icon;
}

static void UpperArm_LvglCreateDisconnectedBluetoothIcon(lv_obj_t *parent)
{
  lv_obj_t *icon;

  icon = lv_label_create(parent);
  lv_label_set_text(icon, LV_SYMBOL_BLUETOOTH);
  lv_obj_set_style_text_color(icon, lv_color_hex(0x8A8F98), 0);
  lv_obj_set_style_text_font(icon, &lv_font_montserrat_48, 0);
  lv_obj_center(icon);
  lv_obj_clear_flag(icon, LV_OBJ_FLAG_CLICKABLE);
}

void UpperArm_LvglCreateHome(void)
{
  lv_obj_t *screen = lv_scr_act();
  lv_obj_t *bg;
  lv_obj_t *logo;
  lv_obj_t *logo_text;
  lv_obj_t *title;
  lv_obj_t *bat;
  lv_obj_t *bat_tip;
  lv_obj_t *bat_fill;
  lv_obj_t *bat_value;
  lv_obj_t *time_label;
  lv_obj_t *welcome;
  lv_obj_t *welcome_title;
  lv_obj_t *welcome_title_bold;
  lv_obj_t *welcome_subtitle;
  lv_obj_t *welcome_subtitle_bold;
  lv_obj_t *btn;
  lv_obj_t *btn_icon;
  lv_obj_t *btn_label;
  lv_obj_t *footer;
  lv_obj_t *stat;
  lv_obj_t *stat_dot;
  lv_obj_t *stat_value;
  lv_obj_t *stat_label;
  lv_obj_t *stat_unit;
  lv_obj_t *divider;
  static const char *btn_texts[4] = {
    "开始训练", "历史记录", UI_ZH_FREE_MODE, "设置"
  };
  static const lv_coord_t btn_x[4] = {
    14, 126, 14, 126
  };
  static const lv_coord_t btn_y[4] = {
    112, 112, 185, 185
  };
  static const uint32_t night_card_top[4] = {
    0x10284A, 0x17243D, 0x0D3540, 0x251F45
  };
  static const uint32_t night_card_bottom[4] = {
    0x071B35, 0x0A182B, 0x08242C, 0x14122B
  };
  static const uint32_t night_card_border[4] = {
    0x2A6FB0, 0x3D5D89, 0x23808C, 0x5C4EA2
  };
  uint8_t was_result = ((current_ui_page == UPPER_PAGE_RESULT) ||
                        (current_ui_page == UPPER_PAGE_FREE_RESULT)) ? 1U : 0U;
  uint8_t i;

  home_time_label = NULL;
  home_today_count_label = NULL;
  home_status_value_label = NULL;
  home_status_dot = NULL;
  home_bt_icon = NULL;
  home_imu_angle_label = NULL;
  prep_countdown_label = NULL;
  prep_spinner_arc = NULL;
  memset(prep_status_dots, 0, sizeof(prep_status_dots));
  prep_time_label = NULL;
  action_time_label = NULL;
  training_time_label = NULL;
  training_pause_icon_label = NULL;
  training_pause_text_label = NULL;
  training_timer_arc = NULL;
  training_timer_value_label = NULL;
  result_time_label = NULL;
  current_ui_page = UPPER_PAGE_HOME;
  if (was_result != 0U)
  {
    upper_page_home_event_frames = 10U;
    upper_page_result_event_frames = 0U;
  }

  if (prep_countdown_timer != NULL)
  {
    lv_timer_del(prep_countdown_timer);
    prep_countdown_timer = NULL;
  }
  if (prep_recalibrate_timer != NULL)
  {
    lv_timer_del(prep_recalibrate_timer);
    prep_recalibrate_timer = NULL;
  }
  if (prep_transition_timer != NULL)
  {
    lv_timer_del(prep_transition_timer);
    prep_transition_timer = NULL;
  }
  if (training_emg_enable_timer != NULL)
  {
    lv_timer_del(training_emg_enable_timer);
    training_emg_enable_timer = NULL;
  }

  lv_obj_clean(screen);
  UpperArm_LvglResetPageObjectRefs();
  lv_obj_set_style_bg_color(screen, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_bg_grad_color(screen, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_bg_grad_dir(screen, LV_GRAD_DIR_VER, 0);
  lv_obj_set_style_text_opa(screen, LV_OPA_COVER, 0);
  lv_obj_set_style_text_color(screen, lv_color_hex(0x102B6B), 0);
  lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

  bg = lv_img_create(screen);
  lv_img_set_src(bg, UpperArm_GetHomeBg());
  lv_obj_set_size(bg, 240, 320);
  lv_obj_align(bg, LV_ALIGN_CENTER, 0, 0);
  lv_obj_clear_flag(bg, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_move_background(bg);

  logo = lv_obj_create(screen);
  lv_obj_remove_style_all(logo);
  lv_obj_set_size(logo, 24, 24);
  lv_obj_align(logo, LV_ALIGN_TOP_LEFT, 10, 8);
  lv_obj_set_style_radius(logo, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(logo, lv_color_hex(0x2F80ED), 0);
  lv_obj_set_style_bg_opa(logo, LV_OPA_COVER, 0);

  logo_text = lv_label_create(logo);
  lv_label_set_text(logo_text, UI_ZH_LOGO);
  lv_obj_set_style_text_color(logo_text, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_text_font(logo_text, &upper_font_zh_14, 0);
  lv_obj_center(logo_text);

  title = lv_label_create(screen);
  lv_label_set_text(title, UI_ZH_BRAND);
  lv_obj_set_style_text_color(title, lv_color_hex(0x102B6B), 0);
  lv_obj_set_style_text_font(title, &upper_font_zh_14, 0);
  lv_obj_align(title, LV_ALIGN_TOP_LEFT, 40, 12);

  home_bt_icon = UpperArm_LvglCreateBluetoothIcon(screen,
                                                  132,
                                                  11,
                                                  lv_color_hex(0x0098F0),
                                                  &lv_font_montserrat_16);

  bat = lv_obj_create(screen);
  lv_obj_remove_style_all(bat);
  lv_obj_set_size(bat, 18, 10);
  lv_obj_set_pos(bat, 151, 14);
  lv_obj_set_style_radius(bat, 2, 0);
  lv_obj_set_style_border_width(bat, 1, 0);
  lv_obj_set_style_border_color(bat, lv_color_hex(0x102B6B), 0);

  bat_tip = lv_obj_create(screen);
  lv_obj_remove_style_all(bat_tip);
  lv_obj_set_size(bat_tip, 2, 6);
  lv_obj_align_to(bat_tip, bat, LV_ALIGN_OUT_RIGHT_MID, 1, 0);
  lv_obj_set_style_radius(bat_tip, 1, 0);
  lv_obj_set_style_bg_color(bat_tip, lv_color_hex(0x102B6B), 0);
  lv_obj_set_style_bg_opa(bat_tip, LV_OPA_COVER, 0);

  bat_fill = lv_obj_create(bat);
  home_battery_fill = bat_fill;
  lv_obj_remove_style_all(bat_fill);
  lv_obj_set_size(bat_fill, 1, 6);
  lv_obj_align(bat_fill, LV_ALIGN_LEFT_MID, 2, 0);
  lv_obj_set_style_radius(bat_fill, 2, 0);
  lv_obj_set_style_bg_color(bat_fill, lv_color_hex(0x27AE60), 0);
  lv_obj_set_style_bg_opa(bat_fill, LV_OPA_COVER, 0);

  bat_value = lv_label_create(screen);
  home_battery_value_label = bat_value;
  lv_label_set_text(bat_value, "--%");
  lv_obj_set_style_text_color(bat_value, lv_color_hex(0x102B6B), 0);
  lv_obj_set_style_text_font(bat_value, &lv_font_montserrat_12, 0);
  lv_obj_set_pos(bat_value, 174, 12);
  UpperArm_LvglApplyBatteryDisplay(home_battery_fill, home_battery_value_label, 13, 1U);

  time_label = lv_label_create(screen);
  home_time_label = time_label;
  lv_label_set_text(time_label, "10:30");
  lv_obj_set_style_text_color(time_label, lv_color_hex(0x102B6B), 0);
  lv_obj_set_style_text_font(time_label, &lv_font_montserrat_12, 0);
  lv_obj_set_pos(time_label, 207, 12);

  welcome = lv_obj_create(screen);
  lv_obj_remove_style_all(welcome);
  lv_obj_set_size(welcome, 218, 58);
  lv_obj_align(welcome, LV_ALIGN_TOP_MID, 0, 44);
  lv_obj_set_style_radius(welcome, 14, 0);
  lv_obj_set_style_bg_color(welcome, lv_color_hex(0xF4FAFF), 0);
  lv_obj_set_style_bg_opa(welcome, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(welcome, 0, 0);
  lv_obj_set_style_border_color(welcome, lv_color_hex(0xB8D8FF), 0);
  lv_obj_set_style_shadow_width(welcome, 0, 0);
  lv_obj_set_style_shadow_opa(welcome, LV_OPA_10, 0);
  lv_obj_clear_flag(welcome, LV_OBJ_FLAG_SCROLLABLE);

  welcome_title = lv_label_create(welcome);
  lv_label_set_text(welcome_title, "欢迎使用");
  lv_obj_set_width(welcome_title, 198);
  lv_label_set_long_mode(welcome_title, LV_LABEL_LONG_WRAP);
  lv_obj_set_style_text_align(welcome_title, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_style_text_color(welcome_title, lv_color_hex(0x4E6799), 0);
  lv_obj_set_style_text_font(welcome_title, &upper_font_zh_16, 0);
  lv_obj_align(welcome_title, LV_ALIGN_TOP_MID, 0, 7);

  welcome_title_bold = lv_label_create(welcome);
  lv_label_set_text(welcome_title_bold, "欢迎使用");
  lv_obj_set_width(welcome_title_bold, 198);
  lv_label_set_long_mode(welcome_title_bold, LV_LABEL_LONG_WRAP);
  lv_obj_set_style_text_align(welcome_title_bold, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_style_text_color(welcome_title_bold, lv_color_hex(0x4E6799), 0);
  lv_obj_set_style_text_font(welcome_title_bold, &upper_font_zh_16, 0);
  lv_obj_align(welcome_title_bold, LV_ALIGN_TOP_MID, 1, 7);

  welcome_subtitle = lv_label_create(welcome);
  lv_label_set_text(welcome_subtitle, "智能康复训练评估系统");
  lv_obj_set_width(welcome_subtitle, 204);
  lv_obj_set_style_text_align(welcome_subtitle, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_style_text_color(welcome_subtitle, lv_color_hex(0x083B8F), 0);
  lv_obj_set_style_text_font(welcome_subtitle, &upper_font_zh_16, 0);
  lv_obj_align(welcome_subtitle, LV_ALIGN_BOTTOM_MID, 0, -8);

  welcome_subtitle_bold = lv_label_create(welcome);
  lv_label_set_text(welcome_subtitle_bold, "智能康复训练评估系统");
  lv_obj_set_width(welcome_subtitle_bold, 204);
  lv_obj_set_style_text_align(welcome_subtitle_bold, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_style_text_color(welcome_subtitle_bold, lv_color_hex(0x083B8F), 0);
  lv_obj_set_style_text_font(welcome_subtitle_bold, &upper_font_zh_16, 0);
  lv_obj_align(welcome_subtitle_bold, LV_ALIGN_BOTTOM_MID, 1, -8);

  home_imu_angle_label = lv_label_create(screen);
  lv_label_set_text(home_imu_angle_label, "R --.-  P --.-  Y --.-");
  lv_obj_set_width(home_imu_angle_label, 218);
  lv_obj_set_style_text_align(home_imu_angle_label, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_style_text_color(home_imu_angle_label, lv_color_hex(0x315A93), 0);
  lv_obj_set_style_text_font(home_imu_angle_label, &lv_font_montserrat_12, 0);
  lv_obj_set_pos(home_imu_angle_label, 11, 91);

  for (i = 0U; i < 4U; i++)
  {
    btn = lv_btn_create(screen);
    lv_obj_remove_style_all(btn);
    lv_obj_set_size(btn, 100, 68);
    lv_obj_set_pos(btn, btn_x[i], btn_y[i]);
    lv_obj_set_style_radius(btn, 16, 0);
    lv_obj_set_style_bg_color(btn, UpperArm_ThemeColor(0xFFFFFF, night_card_top[i]), 0);
    lv_obj_set_style_bg_grad_color(btn, UpperArm_ThemeColor(0xF8FBFF, night_card_bottom[i]), 0);
    lv_obj_set_style_bg_grad_dir(btn, LV_GRAD_DIR_VER, 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x1687F5), LV_STATE_PRESSED);
    lv_obj_set_style_bg_grad_color(btn, lv_color_hex(0x005DE8), LV_STATE_PRESSED);
    lv_obj_set_style_border_width(btn, (upper_night_mode != 0U) ? 1 : 0, 0);
    lv_obj_set_style_border_color(btn, UpperArm_ThemeColor(0x75C8FF, night_card_border[i]), 0);
    lv_obj_set_style_shadow_width(btn, (upper_night_mode != 0U) ? 3 : 6, 0);
    lv_obj_set_style_shadow_color(btn, lv_color_hex(0x000000), 0);
    lv_obj_set_style_shadow_opa(btn, (upper_night_mode != 0U) ? LV_OPA_20 : LV_OPA_10, 0);
    lv_obj_set_style_shadow_ofs_y(btn, 3, 0);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);

    btn_icon = lv_obj_create(btn);
    lv_obj_remove_style_all(btn_icon);
    lv_obj_set_size(btn_icon, 44, 42);
    lv_obj_align(btn_icon, LV_ALIGN_TOP_MID, 0, 7);
    lv_obj_clear_flag(btn_icon, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(btn_icon, LV_OBJ_FLAG_SCROLLABLE);
    UpperArm_LvglCreateHomeIcon(btn_icon, i, 0U);

    btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, btn_texts[i]);
    lv_obj_set_style_text_color(btn_label, UpperArm_ThemeColor(0x102B6B, 0xDCEBFF), 0);
    lv_obj_set_style_text_font(btn_label, &upper_font_zh_14, 0);
    lv_obj_align(btn_label, LV_ALIGN_BOTTOM_MID, 0, -6);
    lv_obj_clear_flag(btn_label, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_add_event_cb(btn, UpperArm_OnHomeButtonClicked, LV_EVENT_CLICKED, (void *)(uintptr_t)i);
  }

  footer = lv_obj_create(screen);
  lv_obj_remove_style_all(footer);
  lv_obj_set_size(footer, 222, 56);
  lv_obj_align(footer, LV_ALIGN_BOTTOM_MID, 0, -10);
  lv_obj_set_style_radius(footer, 16, 0);
  lv_obj_set_style_bg_color(footer, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_bg_opa(footer, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(footer, 1, 0);
  lv_obj_set_style_border_color(footer, lv_color_hex(0xD4E7FF), 0);
  lv_obj_set_style_shadow_width(footer, 5, 0);
  lv_obj_set_style_shadow_opa(footer, LV_OPA_10, 0);
  lv_obj_set_style_shadow_ofs_y(footer, 2, 0);
  lv_obj_clear_flag(footer, LV_OBJ_FLAG_SCROLLABLE);

  for (i = 0U; i < 3U; i++)
  {
    if (i > 0U)
    {
      divider = lv_obj_create(footer);
      lv_obj_remove_style_all(divider);
      lv_obj_set_size(divider, 1, 38);
      lv_obj_set_pos(divider, (lv_coord_t)(74 * i), 9);
      lv_obj_set_style_bg_color(divider, lv_color_hex(0xC8DBF5), 0);
      lv_obj_set_style_bg_opa(divider, LV_OPA_COVER, 0);
    }

    stat = lv_obj_create(footer);
    lv_obj_remove_style_all(stat);
    lv_obj_set_size(stat, 72, 46);
    lv_obj_set_pos(stat, (lv_coord_t)(2 + i * 74), 5);
    lv_obj_clear_flag(stat, LV_OBJ_FLAG_SCROLLABLE);

    stat_label = lv_label_create(stat);
    if (i == 0U) {
      lv_label_set_text(stat_label, "今日训练");
    } else if (i == 1U) {
      lv_label_set_text(stat_label, "平均得分");
    } else {
      lv_label_set_text(stat_label, "连接状态");
    }
    lv_obj_set_style_text_color(stat_label, lv_color_hex(0x102B6B), 0);
    lv_obj_set_style_text_font(stat_label, &upper_font_zh_14, 0);
    lv_obj_align(stat_label, LV_ALIGN_TOP_MID, 0, 4);

    stat_value = lv_label_create(stat);
    if (i == 0U) {
      lv_label_set_text_fmt(stat_value, "%u", (unsigned int)training_completed_count);
      home_today_count_label = stat_value;
    } else if (i == 1U) {
      lv_label_set_text(stat_value, "--");
    } else {
      lv_label_set_text(stat_value, "未连接");
      home_status_value_label = stat_value;
    }
    lv_obj_set_style_text_color(stat_value, (i == 2U) ? lv_color_hex(0x27AE60) : lv_color_hex(0x2F80ED), 0);
    if (i == 2U) {
      lv_obj_set_style_text_font(stat_value, &upper_font_zh_14, 0);
      lv_obj_align(stat_value, LV_ALIGN_BOTTOM_MID, -5, -5);
      stat_dot = UpperArm_LvglShape(stat, 8, 8, 56, 32, lv_color_hex(0x19D64A), LV_RADIUS_CIRCLE);
      home_status_dot = stat_dot;
      lv_obj_clear_flag(stat_dot, LV_OBJ_FLAG_CLICKABLE);
    } else {
      lv_obj_set_style_text_font(stat_value, &lv_font_montserrat_24, 0);
      lv_obj_align(stat_value, LV_ALIGN_BOTTOM_MID, -6, -1);

      stat_unit = lv_label_create(stat);
      lv_label_set_text(stat_unit, (i == 0U) ? "次" : "分");
      lv_obj_set_style_text_color(stat_unit, lv_color_hex(0x2F80ED), 0);
      lv_obj_set_style_text_font(stat_unit, &upper_font_zh_14, 0);
      lv_obj_align_to(stat_unit, stat_value, LV_ALIGN_OUT_RIGHT_BOTTOM, 3, -2);
    }
  }

  UpperArm_LvglUpdateHome(osKernelGetTickCount());
  UpperArm_LvglPageReady(screen);
}

static void UpperArm_LvglCreateSettings(void)
{
  lv_obj_t *screen = lv_scr_act();
  lv_obj_t *bg;
  lv_obj_t *logo;
  lv_obj_t *logo_text;
  lv_obj_t *brand;
  lv_obj_t *brand_sub;
  lv_obj_t *bat;
  lv_obj_t *bat_tip;
  lv_obj_t *bat_fill;
  lv_obj_t *bat_value;
  lv_obj_t *time_label;
  lv_obj_t *divider;
  lv_obj_t *back_btn;
  lv_obj_t *back_icon;
  lv_obj_t *title;
  lv_obj_t *panel;
  lv_obj_t *reset_card;
  lv_obj_t *reset_icon_bg;
  lv_obj_t *reset_icon;
  lv_obj_t *reset_title;
  lv_obj_t *reset_hint;
  lv_obj_t *reset_arrow;

  lv_obj_clean(screen);
  UpperArm_LvglResetPageObjectRefs();
  lv_obj_set_style_bg_color(screen, UpperArm_ThemeColor(0xFFFFFF, 0x020817), 0);
  lv_obj_set_style_bg_grad_color(screen, UpperArm_ThemeColor(0xEEF6FF, 0x061B38), 0);
  lv_obj_set_style_bg_grad_dir(screen, LV_GRAD_DIR_VER, 0);
  lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

  current_ui_page = UPPER_PAGE_SETTINGS;

  bg = lv_img_create(screen);
  lv_img_set_src(bg, UpperArm_GetPrepBg());
  lv_obj_set_size(bg, 240, 320);
  lv_obj_align(bg, LV_ALIGN_CENTER, 0, 0);
  lv_obj_clear_flag(bg, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_move_background(bg);

  logo = lv_obj_create(screen);
  lv_obj_remove_style_all(logo);
  lv_obj_set_size(logo, 24, 24);
  lv_obj_set_pos(logo, 10, 8);
  lv_obj_set_style_radius(logo, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(logo, UpperArm_ThemeColor(0x2F80ED, 0x1F74F2), 0);
  lv_obj_set_style_bg_opa(logo, LV_OPA_COVER, 0);

  logo_text = lv_label_create(logo);
  lv_label_set_text(logo_text, UI_ZH_LOGO);
  lv_obj_set_style_text_color(logo_text, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_text_font(logo_text, &upper_font_zh_14, 0);
  lv_obj_center(logo_text);

  brand = lv_label_create(screen);
  lv_label_set_text(brand, UI_ZH_BRAND);
  lv_obj_set_style_text_color(brand, UpperArm_ThemeColor(0x126BFF, 0x48A7FF), 0);
  lv_obj_set_style_text_font(brand, &upper_font_zh_14, 0);
  lv_obj_set_pos(brand, 40, 7);

  brand_sub = lv_label_create(screen);
  lv_label_set_text(brand_sub, UI_ZH_SYSTEM);
  lv_obj_set_style_text_color(brand_sub, UpperArm_ThemeColor(0x102B6B, 0xDCEBFF), 0);
  lv_obj_set_style_text_font(brand_sub, &upper_font_zh_10, 0);
  lv_obj_set_pos(brand_sub, 40, 24);

  home_bt_icon = UpperArm_LvglCreateBluetoothIcon(screen,
                                                  136,
                                                  11,
                                                  UpperArm_WirelessIsConnected(osKernelGetTickCount()) ?
                                                    UpperArm_ThemeColor(0x0098F0, 0x48A7FF) :
                                                    lv_color_hex(0x8A8F98),
                                                  &lv_font_montserrat_16);

  bat = lv_obj_create(screen);
  lv_obj_remove_style_all(bat);
  lv_obj_set_size(bat, 18, 10);
  lv_obj_set_pos(bat, 154, 13);
  lv_obj_set_style_radius(bat, 2, 0);
  lv_obj_set_style_border_width(bat, 1, 0);
  lv_obj_set_style_border_color(bat, UpperArm_ThemeColor(0x102B6B, 0xDCEBFF), 0);

  bat_tip = lv_obj_create(screen);
  lv_obj_remove_style_all(bat_tip);
  lv_obj_set_size(bat_tip, 2, 6);
  lv_obj_align_to(bat_tip, bat, LV_ALIGN_OUT_RIGHT_MID, 1, 0);
  lv_obj_set_style_radius(bat_tip, 1, 0);
  lv_obj_set_style_bg_color(bat_tip, UpperArm_ThemeColor(0x102B6B, 0xDCEBFF), 0);
  lv_obj_set_style_bg_opa(bat_tip, LV_OPA_COVER, 0);

  bat_fill = lv_obj_create(bat);
  lv_obj_remove_style_all(bat_fill);
  lv_obj_set_size(bat_fill, 12, 6);
  lv_obj_align(bat_fill, LV_ALIGN_LEFT_MID, 2, 0);
  lv_obj_set_style_radius(bat_fill, 1, 0);
  lv_obj_set_style_bg_color(bat_fill, lv_color_hex(0x28D85A), 0);
  lv_obj_set_style_bg_opa(bat_fill, LV_OPA_COVER, 0);

  bat_value = lv_label_create(screen);
  lv_label_set_text(bat_value, "--%");
  lv_obj_set_style_text_color(bat_value, UpperArm_ThemeColor(0x000000, 0xDCEBFF), 0);
  lv_obj_set_style_text_font(bat_value, &lv_font_montserrat_12, 0);
  lv_obj_set_pos(bat_value, 178, 10);
  UpperArm_LvglApplyBatteryDisplay(bat_fill, bat_value, 12, 1U);

  time_label = lv_label_create(screen);
  home_time_label = time_label;
  lv_label_set_text(time_label, "10:30");
  lv_obj_set_style_text_color(time_label, UpperArm_ThemeColor(0x000000, 0xDCEBFF), 0);
  lv_obj_set_style_text_font(time_label, &lv_font_montserrat_12, 0);
  lv_obj_set_pos(time_label, 207, 10);

  divider = UpperArm_LvglShape(screen, 240, 1, 0, 38, UpperArm_ThemeColor(0xD6E6FA, 0x18365F), 0);
  lv_obj_clear_flag(divider, LV_OBJ_FLAG_CLICKABLE);

  back_btn = lv_btn_create(screen);
  lv_obj_remove_style_all(back_btn);
  lv_obj_set_size(back_btn, 34, 34);
  lv_obj_set_pos(back_btn, 5, 43);
  lv_obj_clear_flag(back_btn, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_event_cb(back_btn, UpperArm_OnSettingsBackClicked, LV_EVENT_CLICKED, NULL);

  back_icon = lv_label_create(back_btn);
  lv_label_set_text(back_icon, LV_SYMBOL_LEFT);
  lv_obj_set_style_text_color(back_icon, UpperArm_ThemeColor(0x126BFF, 0x48A7FF), 0);
  lv_obj_set_style_text_font(back_icon, &lv_font_montserrat_24, 0);
  lv_obj_center(back_icon);

  title = lv_label_create(screen);
  lv_label_set_text(title, UI_ZH_SETTINGS);
  lv_obj_set_style_text_color(title, UpperArm_ThemeColor(0x102B6B, 0xEAF4FF), 0);
  lv_obj_set_style_text_font(title, &upper_font_zh_16, 0);
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 51);

  panel = lv_obj_create(screen);
  lv_obj_remove_style_all(panel);
  lv_obj_set_size(panel, 214, 194);
  lv_obj_set_pos(panel, 13, 80);
  lv_obj_set_style_radius(panel, 9, 0);
  lv_obj_set_style_bg_color(panel, UpperArm_ThemeColor(0xFFFFFF, 0x071C36), 0);
  lv_obj_set_style_bg_opa(panel, (upper_night_mode != 0U) ? LV_OPA_80 : LV_OPA_90, 0);
  lv_obj_set_style_border_width(panel, 1, 0);
  lv_obj_set_style_border_color(panel, UpperArm_ThemeColor(0xE3EEFF, 0x1C4D82), 0);
  lv_obj_set_style_shadow_width(panel, 4, 0);
  lv_obj_set_style_shadow_opa(panel, LV_OPA_10, 0);
  lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE);

  UpperArm_LvglCreateSettingsRow(panel, 0, UI_ZH_VOICE_BROADCAST, UI_ZH_VOICE_HINT, settings_switch_state[0]);
  UpperArm_LvglCreateSettingsRow(panel, 1, UI_ZH_VIBRATION_REMIND, UI_ZH_VIBRATION_HINT, settings_switch_state[1]);
  UpperArm_LvglCreateSettingsRow(panel, 2, UI_ZH_AUTO_SAVE, UI_ZH_AUTO_SAVE_HINT, settings_switch_state[2]);
  UpperArm_LvglCreateSettingsValueRow(panel, 3, UI_ZH_TARGET_COUNT, UI_ZH_TARGET_COUNT_HINT, 0U);
  UpperArm_LvglCreateSettingsValueRow(panel, 4, UI_ZH_TRAIN_DURATION, UI_ZH_TRAIN_DURATION_HINT, 1U);
  UpperArm_LvglCreateSettingsRow(panel,
                                 UPPER_SETTINGS_NIGHT_SWITCH_INDEX,
                                 UI_ZH_NIGHT_MODE,
                                 UI_ZH_NIGHT_MODE_HINT,
                                 upper_night_mode);

  reset_card = lv_btn_create(screen);
  lv_obj_remove_style_all(reset_card);
  lv_obj_set_size(reset_card, 214, 38);
  lv_obj_set_pos(reset_card, 13, 278);
  lv_obj_set_style_radius(reset_card, 9, 0);
  lv_obj_set_style_bg_color(reset_card, UpperArm_ThemeColor(0xFFFFFF, 0x071C36), 0);
  lv_obj_set_style_bg_opa(reset_card, (upper_night_mode != 0U) ? LV_OPA_80 : LV_OPA_90, 0);
  lv_obj_set_style_border_width(reset_card, 1, 0);
  lv_obj_set_style_border_color(reset_card, UpperArm_ThemeColor(0xE3EEFF, 0x1C4D82), 0);
  lv_obj_set_style_shadow_width(reset_card, 4, 0);
  lv_obj_set_style_shadow_opa(reset_card, LV_OPA_10, 0);
  lv_obj_clear_flag(reset_card, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_event_cb(reset_card, UpperArm_OnSettingsResetClicked, LV_EVENT_CLICKED, NULL);

  reset_icon_bg = UpperArm_LvglShape(reset_card, 28, 28, 9, 5, UpperArm_ThemeColor(0xEAF3FF, 0x10284A), LV_RADIUS_CIRCLE);
  reset_icon = lv_label_create(reset_icon_bg);
  lv_label_set_text(reset_icon, LV_SYMBOL_REFRESH);
  lv_obj_set_style_text_color(reset_icon, UpperArm_ThemeColor(0x126BFF, 0x6AB7FF), 0);
  lv_obj_set_style_text_font(reset_icon, &lv_font_montserrat_16, 0);
  lv_obj_center(reset_icon);
  lv_obj_clear_flag(reset_icon_bg, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_clear_flag(reset_icon, LV_OBJ_FLAG_CLICKABLE);

  reset_title = lv_label_create(reset_card);
  lv_label_set_text(reset_title, UI_ZH_RESTORE_DEFAULT);
  lv_obj_set_width(reset_title, 130);
  lv_label_set_long_mode(reset_title, LV_LABEL_LONG_DOT);
  lv_obj_set_style_text_color(reset_title, UpperArm_ThemeColor(0x126BFF, 0xDCEBFF), 0);
  lv_obj_set_style_text_font(reset_title, &upper_font_zh_14, 0);
  lv_obj_set_pos(reset_title, 47, 4);
  lv_obj_clear_flag(reset_title, LV_OBJ_FLAG_CLICKABLE);

  reset_hint = lv_label_create(reset_card);
  lv_label_set_text(reset_hint, UI_ZH_RESTORE_HINT);
  lv_obj_set_width(reset_hint, 130);
  lv_label_set_long_mode(reset_hint, LV_LABEL_LONG_DOT);
  lv_obj_set_style_text_color(reset_hint, UpperArm_ThemeColor(0x5D7098, 0x89A8D8), 0);
  lv_obj_set_style_text_font(reset_hint, &upper_font_zh_10, 0);
  lv_obj_set_pos(reset_hint, 47, 21);
  lv_obj_clear_flag(reset_hint, LV_OBJ_FLAG_CLICKABLE);

  reset_arrow = lv_label_create(reset_card);
  lv_label_set_text(reset_arrow, LV_SYMBOL_RIGHT);
  lv_obj_set_style_text_color(reset_arrow, UpperArm_ThemeColor(0x8EA0BF, 0x6AB7FF), 0);
  lv_obj_set_style_text_font(reset_arrow, &lv_font_montserrat_16, 0);
  lv_obj_align(reset_arrow, LV_ALIGN_RIGHT_MID, -8, 0);
  lv_obj_clear_flag(reset_arrow, LV_OBJ_FLAG_CLICKABLE);

  UpperArm_LvglPageReady(screen);
}

static void UpperArm_LvglCreateHistory(void)
{
  lv_obj_t *screen = lv_scr_act();
  lv_obj_t *bg;
  lv_obj_t *logo;
  lv_obj_t *logo_text;
  lv_obj_t *brand;
  lv_obj_t *brand_sub;
  lv_obj_t *time_label;
  lv_obj_t *divider;
  lv_obj_t *back_btn;
  lv_obj_t *back_icon;
  lv_obj_t *title;
  lv_obj_t *summary;
  lv_obj_t *label;
  lv_obj_t *value;
  lv_obj_t *list;
  lv_obj_t *clear_btn;
  lv_obj_t *clear_label;
  lv_obj_t *analytics_btn;
  lv_obj_t *analytics_label;
  lv_obj_t *card;
  lv_obj_t *dot;
  lv_obj_t *action;
  lv_obj_t *meta;
  lv_obj_t *score;
  lv_obj_t *angle;
  uint8_t i;
  uint8_t visible_count;
  static const char *action_texts[5] = {UI_ZH_ELBOW_FLEX, UI_ZH_ARM_ABD, UI_ZH_FOREARM_ROT, UI_ZH_SHOULDER_LIFT, UI_ZH_FREE_TRAIN};

  lv_obj_clean(screen);
  UpperArm_LvglResetPageObjectRefs();
  lv_obj_set_style_bg_color(screen, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_bg_grad_color(screen, lv_color_hex(0xEEF6FF), 0);
  lv_obj_set_style_bg_grad_dir(screen, LV_GRAD_DIR_VER, 0);
  lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

  current_ui_page = UPPER_PAGE_HISTORY;

  bg = lv_img_create(screen);
  lv_img_set_src(bg, UpperArm_GetPrepBg());
  lv_obj_set_size(bg, 240, 320);
  lv_obj_align(bg, LV_ALIGN_CENTER, 0, 0);
  lv_obj_clear_flag(bg, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_move_background(bg);

  logo = lv_obj_create(screen);
  lv_obj_remove_style_all(logo);
  lv_obj_set_size(logo, 24, 24);
  lv_obj_set_pos(logo, 10, 8);
  lv_obj_set_style_radius(logo, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(logo, lv_color_hex(0x2F80ED), 0);
  lv_obj_set_style_bg_opa(logo, LV_OPA_COVER, 0);

  logo_text = lv_label_create(logo);
  lv_label_set_text(logo_text, UI_ZH_LOGO);
  lv_obj_set_style_text_color(logo_text, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_text_font(logo_text, &upper_font_zh_14, 0);
  lv_obj_center(logo_text);

  brand = lv_label_create(screen);
  lv_label_set_text(brand, UI_ZH_BRAND);
  lv_obj_set_style_text_color(brand, lv_color_hex(0x126BFF), 0);
  lv_obj_set_style_text_font(brand, &upper_font_zh_14, 0);
  lv_obj_set_pos(brand, 40, 7);

  brand_sub = lv_label_create(screen);
  lv_label_set_text(brand_sub, UI_ZH_SYSTEM);
  lv_obj_set_style_text_color(brand_sub, lv_color_hex(0x102B6B), 0);
  lv_obj_set_style_text_font(brand_sub, &upper_font_zh_10, 0);
  lv_obj_set_pos(brand_sub, 40, 24);

  time_label = lv_label_create(screen);
  home_time_label = time_label;
  lv_label_set_text(time_label, "10:30");
  lv_obj_set_style_text_color(time_label, lv_color_hex(0x000000), 0);
  lv_obj_set_style_text_font(time_label, &lv_font_montserrat_12, 0);
  lv_obj_align(time_label, LV_ALIGN_TOP_RIGHT, -5, 10);

  divider = UpperArm_LvglShape(screen, 240, 1, 0, 38, lv_color_hex(0xD6E6FA), 0);
  lv_obj_clear_flag(divider, LV_OBJ_FLAG_CLICKABLE);

  back_btn = lv_btn_create(screen);
  lv_obj_remove_style_all(back_btn);
  lv_obj_set_size(back_btn, 34, 34);
  lv_obj_set_pos(back_btn, 5, 43);
  lv_obj_clear_flag(back_btn, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_event_cb(back_btn, UpperArm_OnSettingsBackClicked, LV_EVENT_CLICKED, NULL);

  back_icon = lv_label_create(back_btn);
  lv_label_set_text(back_icon, LV_SYMBOL_LEFT);
  lv_obj_set_style_text_color(back_icon, lv_color_hex(0x126BFF), 0);
  lv_obj_set_style_text_font(back_icon, &lv_font_montserrat_24, 0);
  lv_obj_center(back_icon);

  title = lv_label_create(screen);
  lv_label_set_text(title, UI_ZH_HISTORY);
  lv_obj_set_style_text_color(title, lv_color_hex(0x102B6B), 0);
  lv_obj_set_style_text_font(title, &upper_font_zh_16, 0);
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 51);

  clear_btn = lv_btn_create(screen);
  lv_obj_remove_style_all(clear_btn);
  lv_obj_set_size(clear_btn, 48, 24);
  lv_obj_set_pos(clear_btn, 180, 75);
  lv_obj_set_style_radius(clear_btn, 8, 0);
  lv_obj_set_style_bg_color(clear_btn, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_bg_opa(clear_btn, LV_OPA_90, 0);
  lv_obj_set_style_border_width(clear_btn, 1, 0);
  lv_obj_set_style_border_color(clear_btn, lv_color_hex(0x2F80ED), 0);
  lv_obj_clear_flag(clear_btn, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_event_cb(clear_btn, UpperArm_OnClearHistoryClicked, LV_EVENT_CLICKED, NULL);

  clear_label = lv_label_create(clear_btn);
  lv_label_set_text(clear_label, UI_ZH_CLEAR);
  lv_obj_set_style_text_color(clear_label, lv_color_hex(0x126BFF), 0);
  lv_obj_set_style_text_font(clear_label, &upper_font_zh_12, 0);
  lv_obj_center(clear_label);

  analytics_btn = lv_btn_create(screen);
  lv_obj_remove_style_all(analytics_btn);
  lv_obj_set_size(analytics_btn, 48, 24);
  lv_obj_set_pos(analytics_btn, 126, 75);
  lv_obj_set_style_radius(analytics_btn, 8, 0);
  lv_obj_set_style_bg_color(analytics_btn, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_bg_opa(analytics_btn, LV_OPA_90, 0);
  lv_obj_set_style_border_width(analytics_btn, 1, 0);
  lv_obj_set_style_border_color(analytics_btn, lv_color_hex(0x2F80ED), 0);
  lv_obj_clear_flag(analytics_btn, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_event_cb(analytics_btn, UpperArm_OnAnalyticsClicked, LV_EVENT_CLICKED, NULL);

  analytics_label = lv_label_create(analytics_btn);
  lv_label_set_text(analytics_label, UI_ZH_ANALYTICS_SHORT);
  lv_obj_set_style_text_color(analytics_label, lv_color_hex(0x126BFF), 0);
  lv_obj_set_style_text_font(analytics_label, &upper_font_zh_12, 0);
  lv_obj_center(analytics_label);

  summary = lv_obj_create(screen);
  lv_obj_remove_style_all(summary);
  lv_obj_set_size(summary, 214, 52);
  lv_obj_set_pos(summary, 13, 104);
  lv_obj_set_style_radius(summary, 9, 0);
  lv_obj_set_style_bg_color(summary, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_bg_opa(summary, LV_OPA_90, 0);
  lv_obj_set_style_border_width(summary, 1, 0);
  lv_obj_set_style_border_color(summary, lv_color_hex(0xE3EEFF), 0);
  lv_obj_set_style_shadow_width(summary, 4, 0);
  lv_obj_set_style_shadow_opa(summary, LV_OPA_10, 0);
  lv_obj_clear_flag(summary, LV_OBJ_FLAG_SCROLLABLE);

  label = lv_label_create(summary);
  lv_label_set_text(label, UI_ZH_TOTAL_TRAIN);
  lv_obj_set_style_text_color(label, lv_color_hex(0x102B6B), 0);
  lv_obj_set_style_text_font(label, &upper_font_zh_14, 0);
  lv_obj_set_pos(label, 16, 8);

  value = lv_label_create(summary);
  lv_label_set_text_fmt(value, "%u%s", (unsigned int)rehab_history_count, UI_ZH_COUNT_UNIT);
  lv_obj_set_style_text_color(value, lv_color_hex(0x126BFF), 0);
  lv_obj_set_style_text_font(value, &upper_font_zh_16, 0);
  lv_obj_set_pos(value, 16, 28);

  label = lv_label_create(summary);
  lv_label_set_text(label, UI_ZH_LATEST_RECORD);
  lv_obj_set_style_text_color(label, lv_color_hex(0x5D7098), 0);
  lv_obj_set_style_text_font(label, &upper_font_zh_12, 0);
  lv_obj_align(label, LV_ALIGN_RIGHT_MID, -16, -8);

  value = lv_label_create(summary);
  lv_label_set_text_fmt(value, "%u", (unsigned int)rehab_history_count);
  lv_obj_set_style_text_color(value, lv_color_hex(0x126BFF), 0);
  lv_obj_set_style_text_font(value, &lv_font_montserrat_16, 0);
  lv_obj_align(value, LV_ALIGN_RIGHT_MID, -28, 11);

  list = lv_obj_create(screen);
  lv_obj_remove_style_all(list);
  lv_obj_set_size(list, 214, 149);
  lv_obj_set_pos(list, 13, 164);
  lv_obj_clear_flag(list, LV_OBJ_FLAG_SCROLLABLE);

  if (rehab_history_count == 0U)
  {
    label = lv_label_create(list);
    lv_label_set_text(label, UI_ZH_NO_HISTORY);
    lv_obj_set_style_text_color(label, lv_color_hex(0x5D7098), 0);
    lv_obj_set_style_text_font(label, &upper_font_zh_14, 0);
    lv_obj_center(label);
    UpperArm_LvglPageReady(screen);
    return;
  }

  visible_count = (rehab_history_count > 4U) ? 4U : rehab_history_count;
  for (i = 0U; i < visible_count; i++)
  {
    uint8_t hist_index = (uint8_t)((rehab_history_next + REHAB_HISTORY_MAX_RECORDS - 1U - i) % REHAB_HISTORY_MAX_RECORDS);
    const RehabHistoryRecord_t *record = &rehab_history[hist_index];
    uint32_t elapsed_s = record->tick_ms / 1000U;

    card = lv_obj_create(list);
    lv_obj_remove_style_all(card);
    lv_obj_set_size(card, 214, 39);
    lv_obj_set_pos(card, 0, (lv_coord_t)(i * 43));
    lv_obj_set_style_radius(card, 8, 0);
    lv_obj_set_style_bg_color(card, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_90, 0);
    lv_obj_set_style_border_width(card, 1, 0);
    lv_obj_set_style_border_color(card, lv_color_hex(0xE3EEFF), 0);
    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);

    dot = UpperArm_LvglShape(card, 7, 7, 9, 16, lv_color_hex(0x126BFF), LV_RADIUS_CIRCLE);
    lv_obj_clear_flag(dot, LV_OBJ_FLAG_CLICKABLE);

    action = lv_label_create(card);
    lv_label_set_text(action, action_texts[(record->action_index < 5U) ? record->action_index : 4U]);
    lv_obj_set_width(action, 88);
    lv_label_set_long_mode(action, LV_LABEL_LONG_DOT);
    lv_obj_set_style_text_color(action, lv_color_hex(0x102B6B), 0);
    lv_obj_set_style_text_font(action, &upper_font_zh_12, 0);
    lv_obj_set_pos(action, 22, 5);

    meta = lv_label_create(card);
    if (record->action_index == 4U)
    {
      lv_label_set_text_fmt(meta, "%u%s  %u/%u/%u/%u",
                            (unsigned int)record->reps,
                            UI_ZH_COUNT_UNIT,
                            (unsigned int)record->action_counts[0],
                            (unsigned int)record->action_counts[1],
                            (unsigned int)record->action_counts[2],
                            (unsigned int)record->action_counts[3]);
    }
    else
    {
      lv_label_set_text_fmt(meta, "%u/%u%s  %u%s",
                            (unsigned int)record->reps,
                            (unsigned int)record->target_reps,
                            UI_ZH_COUNT_UNIT,
                            (unsigned int)record->duration_sec,
                            UI_ZH_SECOND_UNIT);
    }
    lv_obj_set_style_text_color(meta, lv_color_hex(0x5D7098), 0);
    lv_obj_set_style_text_font(meta, &upper_font_zh_10, 0);
    lv_obj_set_pos(meta, 22, 22);

    score = lv_label_create(card);
    if (record->action_index == 4U)
    {
      lv_label_set_text_fmt(score, "%u%s", (unsigned int)record->reps, UI_ZH_COUNT_UNIT);
    }
    else
    {
      lv_label_set_text_fmt(score, "%u%s", (unsigned int)record->score, UI_ZH_SCORE_UNIT);
    }
    lv_obj_set_style_text_color(score, lv_color_hex(0x126BFF), 0);
    lv_obj_set_style_text_font(score, &upper_font_zh_14, 0);
    lv_obj_align(score, LV_ALIGN_TOP_RIGHT, -10, 5);

    angle = lv_label_create(card);
    if (elapsed_s < 60U)
    {
      lv_label_set_text_fmt(angle, "%u°  %lus", (unsigned int)record->max_angle, (unsigned long)elapsed_s);
    }
    else
    {
      lv_label_set_text_fmt(angle, "%u°  %lumin", (unsigned int)record->max_angle, (unsigned long)(elapsed_s / 60U));
    }
    lv_obj_set_style_text_color(angle, lv_color_hex(0x5D7098), 0);
    lv_obj_set_style_text_font(angle, &upper_font_zh_9, 0);
    lv_obj_align(angle, LV_ALIGN_BOTTOM_RIGHT, -10, -4);
  }

  UpperArm_LvglPageReady(screen);
}

static void UpperArm_LvglCreateAnalyticsMetric(lv_obj_t *parent,
                                               lv_coord_t x,
                                               lv_coord_t y,
                                               const char *title_text,
                                               uint16_t number,
                                               const char *unit_text)
{
  lv_obj_t *card;
  lv_obj_t *title;
  lv_obj_t *value;
  lv_obj_t *unit;

  card = lv_obj_create(parent);
  lv_obj_remove_style_all(card);
  lv_obj_set_size(card, 101, 39);
  lv_obj_set_pos(card, x, y);
  lv_obj_set_style_radius(card, 8, 0);
  lv_obj_set_style_bg_color(card, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_bg_opa(card, LV_OPA_90, 0);
  lv_obj_set_style_border_width(card, 1, 0);
  lv_obj_set_style_border_color(card, lv_color_hex(0xE3EEFF), 0);
  lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);

  title = lv_label_create(card);
  lv_label_set_text(title, title_text);
  lv_obj_set_style_text_color(title, lv_color_hex(0x5D7098), 0);
  lv_obj_set_style_text_font(title, &upper_font_zh_10, 0);
  lv_obj_set_pos(title, 8, 5);

  value = lv_label_create(card);
  lv_label_set_text_fmt(value, "%u", (unsigned int)number);
  lv_obj_set_style_text_color(value, lv_color_hex(0x126BFF), 0);
  lv_obj_set_style_text_font(value, &lv_font_montserrat_16, 0);
  lv_obj_set_pos(value, 8, 19);

  unit = lv_label_create(card);
  lv_label_set_text(unit, unit_text);
  lv_obj_set_style_text_color(unit, lv_color_hex(0x102B6B), 0);
  lv_obj_set_style_text_font(unit, &upper_font_zh_10, 0);
  lv_obj_align_to(unit, value, LV_ALIGN_OUT_RIGHT_BOTTOM, 2, -1);
}

static void UpperArm_LvglCreateAnalytics(void)
{
  lv_obj_t *screen = lv_scr_act();
  lv_obj_t *bg;
  lv_obj_t *logo;
  lv_obj_t *logo_text;
  lv_obj_t *brand;
  lv_obj_t *brand_sub;
  lv_obj_t *time_label;
  lv_obj_t *divider;
  lv_obj_t *back_btn;
  lv_obj_t *back_icon;
  lv_obj_t *title;
  lv_obj_t *summary;
  lv_obj_t *label;
  lv_obj_t *value;
  lv_obj_t *chart_card;
  lv_obj_t *chart_title;
  lv_obj_t *chart;
  lv_chart_series_t *series;
  lv_obj_t *metrics;
  lv_obj_t *advice;
  uint32_t score_sum = 0U;
  uint32_t completion_sum = 0U;
  uint32_t amplitude_sum = 0U;
  uint16_t max_angle = 0U;
  uint16_t avg_score = 0U;
  uint16_t avg_completion = 0U;
  uint16_t avg_amplitude = 0U;
  uint8_t i;
  uint8_t trend_count;

  lv_obj_clean(screen);
  UpperArm_LvglResetPageObjectRefs();
  lv_obj_set_style_bg_color(screen, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_bg_grad_color(screen, lv_color_hex(0xEEF6FF), 0);
  lv_obj_set_style_bg_grad_dir(screen, LV_GRAD_DIR_VER, 0);
  lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

  current_ui_page = UPPER_PAGE_ANALYTICS;

  for (i = 0U; i < rehab_history_count; i++)
  {
    const RehabHistoryRecord_t *record = &rehab_history[i];
    score_sum += record->score;
    completion_sum += record->completion;
    amplitude_sum += record->amplitude;
    if (record->max_angle > max_angle)
    {
      max_angle = record->max_angle;
    }
  }
  if (rehab_history_count != 0U)
  {
    avg_score = (uint16_t)(score_sum / rehab_history_count);
    avg_completion = (uint16_t)(completion_sum / rehab_history_count);
    avg_amplitude = (uint16_t)(amplitude_sum / rehab_history_count);
  }

  bg = lv_img_create(screen);
  lv_img_set_src(bg, UpperArm_GetPrepBg());
  lv_obj_set_size(bg, 240, 320);
  lv_obj_align(bg, LV_ALIGN_CENTER, 0, 0);
  lv_obj_clear_flag(bg, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_move_background(bg);

  logo = lv_obj_create(screen);
  lv_obj_remove_style_all(logo);
  lv_obj_set_size(logo, 24, 24);
  lv_obj_set_pos(logo, 10, 8);
  lv_obj_set_style_radius(logo, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(logo, lv_color_hex(0x2F80ED), 0);
  lv_obj_set_style_bg_opa(logo, LV_OPA_COVER, 0);

  logo_text = lv_label_create(logo);
  lv_label_set_text(logo_text, UI_ZH_LOGO);
  lv_obj_set_style_text_color(logo_text, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_text_font(logo_text, &upper_font_zh_14, 0);
  lv_obj_center(logo_text);

  brand = lv_label_create(screen);
  lv_label_set_text(brand, UI_ZH_BRAND);
  lv_obj_set_style_text_color(brand, lv_color_hex(0x126BFF), 0);
  lv_obj_set_style_text_font(brand, &upper_font_zh_14, 0);
  lv_obj_set_pos(brand, 40, 7);

  brand_sub = lv_label_create(screen);
  lv_label_set_text(brand_sub, UI_ZH_SYSTEM);
  lv_obj_set_style_text_color(brand_sub, lv_color_hex(0x102B6B), 0);
  lv_obj_set_style_text_font(brand_sub, &upper_font_zh_10, 0);
  lv_obj_set_pos(brand_sub, 40, 24);

  time_label = lv_label_create(screen);
  home_time_label = time_label;
  lv_label_set_text(time_label, "10:30");
  lv_obj_set_style_text_color(time_label, lv_color_hex(0x000000), 0);
  lv_obj_set_style_text_font(time_label, &lv_font_montserrat_12, 0);
  lv_obj_align(time_label, LV_ALIGN_TOP_RIGHT, -5, 10);

  divider = UpperArm_LvglShape(screen, 240, 1, 0, 38, lv_color_hex(0xD6E6FA), 0);
  lv_obj_clear_flag(divider, LV_OBJ_FLAG_CLICKABLE);

  back_btn = lv_btn_create(screen);
  lv_obj_remove_style_all(back_btn);
  lv_obj_set_size(back_btn, 34, 34);
  lv_obj_set_pos(back_btn, 5, 43);
  lv_obj_clear_flag(back_btn, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_event_cb(back_btn, UpperArm_OnSettingsBackClicked, LV_EVENT_CLICKED, NULL);

  back_icon = lv_label_create(back_btn);
  lv_label_set_text(back_icon, LV_SYMBOL_LEFT);
  lv_obj_set_style_text_color(back_icon, lv_color_hex(0x126BFF), 0);
  lv_obj_set_style_text_font(back_icon, &lv_font_montserrat_24, 0);
  lv_obj_center(back_icon);

  title = lv_label_create(screen);
  lv_label_set_text(title, UI_ZH_ANALYTICS);
  lv_obj_set_style_text_color(title, lv_color_hex(0x102B6B), 0);
  lv_obj_set_style_text_font(title, &upper_font_zh_16, 0);
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 51);

  summary = lv_obj_create(screen);
  lv_obj_remove_style_all(summary);
  lv_obj_set_size(summary, 214, 44);
  lv_obj_set_pos(summary, 13, 80);
  lv_obj_set_style_radius(summary, 9, 0);
  lv_obj_set_style_bg_color(summary, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_bg_opa(summary, LV_OPA_90, 0);
  lv_obj_set_style_border_width(summary, 1, 0);
  lv_obj_set_style_border_color(summary, lv_color_hex(0xE3EEFF), 0);
  lv_obj_clear_flag(summary, LV_OBJ_FLAG_SCROLLABLE);

  label = lv_label_create(summary);
  lv_label_set_text(label, UI_ZH_AVG_SCORE);
  lv_obj_set_style_text_color(label, lv_color_hex(0x102B6B), 0);
  lv_obj_set_style_text_font(label, &upper_font_zh_14, 0);
  lv_obj_set_pos(label, 14, 6);

  value = lv_label_create(summary);
  lv_label_set_text_fmt(value, "%u%s", (unsigned int)avg_score, UI_ZH_SCORE_UNIT);
  lv_obj_set_style_text_color(value, lv_color_hex(0x126BFF), 0);
  lv_obj_set_style_text_font(value, &upper_font_zh_16, 0);
  lv_obj_set_pos(value, 14, 24);

  label = lv_label_create(summary);
  lv_label_set_text(label, UI_ZH_TOTAL_TRAIN);
  lv_obj_set_style_text_color(label, lv_color_hex(0x5D7098), 0);
  lv_obj_set_style_text_font(label, &upper_font_zh_12, 0);
  lv_obj_align(label, LV_ALIGN_RIGHT_MID, -16, -8);

  value = lv_label_create(summary);
  lv_label_set_text_fmt(value, "%u%s", (unsigned int)training_completed_count, UI_ZH_COUNT_UNIT);
  lv_obj_set_style_text_color(value, lv_color_hex(0x126BFF), 0);
  lv_obj_set_style_text_font(value, &upper_font_zh_14, 0);
  lv_obj_align(value, LV_ALIGN_RIGHT_MID, -16, 11);

  chart_card = lv_obj_create(screen);
  lv_obj_remove_style_all(chart_card);
  lv_obj_set_size(chart_card, 214, 82);
  lv_obj_set_pos(chart_card, 13, 130);
  lv_obj_set_style_radius(chart_card, 9, 0);
  lv_obj_set_style_bg_color(chart_card, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_bg_opa(chart_card, LV_OPA_90, 0);
  lv_obj_set_style_border_width(chart_card, 1, 0);
  lv_obj_set_style_border_color(chart_card, lv_color_hex(0xE3EEFF), 0);
  lv_obj_clear_flag(chart_card, LV_OBJ_FLAG_SCROLLABLE);

  chart_title = lv_label_create(chart_card);
  lv_label_set_text(chart_title, UI_ZH_SCORE_TREND);
  lv_obj_set_style_text_color(chart_title, lv_color_hex(0x102B6B), 0);
  lv_obj_set_style_text_font(chart_title, &upper_font_zh_12, 0);
  lv_obj_set_pos(chart_title, 10, 6);

  chart = lv_chart_create(chart_card);
  lv_obj_remove_style_all(chart);
  lv_obj_set_size(chart, 188, 48);
  lv_obj_set_pos(chart, 13, 28);
  lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
  lv_chart_set_point_count(chart, 7);
  lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 60, 100);
  lv_chart_set_div_line_count(chart, 3, 4);
  lv_obj_set_style_line_color(chart, lv_color_hex(0xDCEAFF), LV_PART_MAIN);
  lv_obj_set_style_line_width(chart, 1, LV_PART_MAIN);
  lv_obj_set_style_line_width(chart, 2, LV_PART_ITEMS);
  series = lv_chart_add_series(chart, lv_color_hex(0x126BFF), LV_CHART_AXIS_PRIMARY_Y);
  trend_count = (rehab_history_count > 7U) ? 7U : rehab_history_count;
  for (i = 0U; i < 7U; i++)
  {
    int16_t point = (int16_t)((rehab_history_count == 0U) ? 80 : avg_score);
    if (i < trend_count)
    {
      uint8_t hist_index = (uint8_t)((rehab_history_next + REHAB_HISTORY_MAX_RECORDS - trend_count + i) % REHAB_HISTORY_MAX_RECORDS);
      point = (int16_t)rehab_history[hist_index].score;
    }
    lv_chart_set_next_value(chart, series, point);
  }
  lv_chart_refresh(chart);

  metrics = lv_obj_create(screen);
  lv_obj_remove_style_all(metrics);
  lv_obj_set_size(metrics, 214, 84);
  lv_obj_set_pos(metrics, 13, 218);
  lv_obj_clear_flag(metrics, LV_OBJ_FLAG_SCROLLABLE);
  UpperArm_LvglCreateAnalyticsMetric(metrics, 0, 0, UI_ZH_COMPLETION, avg_completion, "%");
  UpperArm_LvglCreateAnalyticsMetric(metrics, 113, 0, UI_ZH_AMPLITUDE, avg_amplitude, "%");
  UpperArm_LvglCreateAnalyticsMetric(metrics, 0, 45, UI_ZH_MAX_ANGLE, max_angle, "°");
  UpperArm_LvglCreateAnalyticsMetric(metrics, 113, 45, UI_ZH_TRAIN_COUNT, training_completed_count, UI_ZH_COUNT_UNIT);

  advice = lv_label_create(screen);
  lv_label_set_text(advice, (rehab_history_count < 2U) ? UI_ZH_DATA_NOT_ENOUGH : UI_ZH_KEEP_GOING);
  lv_obj_set_width(advice, 214);
  lv_label_set_long_mode(advice, LV_LABEL_LONG_DOT);
  lv_obj_set_style_text_align(advice, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_style_text_color(advice, lv_color_hex(0x123F95), 0);
  lv_obj_set_style_text_font(advice, &upper_font_zh_12, 0);
  lv_obj_set_pos(advice, 13, 304);

  UpperArm_LvglPageReady(screen);
}

static void UpperArm_LvglCreatePrep(void)
{
  lv_obj_t *screen = lv_scr_act();
  lv_obj_t *bg;
  lv_obj_t *logo;
  lv_obj_t *logo_text;
  lv_obj_t *brand;
  lv_obj_t *brand_sub;
  lv_obj_t *bat;
  lv_obj_t *bat_tip;
  lv_obj_t *bat_fill;
  lv_obj_t *bat_value;
  lv_obj_t *divider;
  lv_obj_t *title;
  lv_obj_t *title_bold;
  lv_obj_t *subtitle;
  lv_obj_t *subtitle_bold;
  lv_obj_t *ring_bg;
  lv_obj_t *ring_progress;
  lv_obj_t *ring_inner;
  lv_obj_t *ring_core;
  lv_obj_t *dot;
  lv_obj_t *time_label;
  uint8_t i;
  static const lv_coord_t dot_x[6] = {48, 56, 64, 176, 184, 192};

  UpperArm_RequestVibration();

  lv_obj_clean(screen);
  UpperArm_LvglResetPageObjectRefs();
  lv_obj_set_style_bg_color(screen, lv_color_hex(0xF8FCFF), 0);
  lv_obj_set_style_bg_grad_color(screen, lv_color_hex(0xE6F1FF), 0);
  lv_obj_set_style_bg_grad_dir(screen, LV_GRAD_DIR_VER, 0);
  lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

  current_ui_page = UPPER_PAGE_PREP;
  esp_upload_pause_until_tick = osKernelGetTickCount() + ESP8266_UI_TRANSITION_PAUSE_MS;
  home_status_value_label = NULL;
  home_status_dot = NULL;
  home_bt_icon = NULL;

  bg = lv_img_create(screen);
  lv_img_set_src(bg, UpperArm_GetPrepBg());
  lv_obj_align(bg, LV_ALIGN_CENTER, 0, 0);
  lv_obj_clear_flag(bg, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_move_background(bg);

  logo = lv_obj_create(screen);
  lv_obj_remove_style_all(logo);
  lv_obj_set_size(logo, 23, 23);
  lv_obj_set_pos(logo, 10, 6);
  lv_obj_set_style_radius(logo, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(logo, lv_color_hex(0x1F74F2), 0);
  lv_obj_set_style_bg_opa(logo, LV_OPA_COVER, 0);

  logo_text = lv_label_create(logo);
  lv_label_set_text(logo_text, UI_ZH_LOGO);
  lv_obj_set_style_text_color(logo_text, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_text_font(logo_text, &upper_font_zh_14, 0);
  lv_obj_center(logo_text);

  brand = lv_label_create(screen);
  lv_label_set_text(brand, UI_ZH_BRAND);
  lv_obj_set_style_text_color(brand, lv_color_hex(0x1267DE), 0);
  lv_obj_set_style_text_font(brand, &upper_font_zh_14, 0);
  lv_obj_set_pos(brand, 38, 5);

  brand_sub = lv_label_create(screen);
  lv_label_set_text(brand_sub, UI_ZH_SYSTEM);
  lv_obj_set_style_text_color(brand_sub, lv_color_hex(0x123F95), 0);
  lv_obj_set_style_text_font(brand_sub, &upper_font_zh_14, 0);
  lv_obj_set_pos(brand_sub, 38, 21);

  divider = UpperArm_LvglShape(screen, 1, 20, 154, 7, lv_color_hex(0xC9DCFA), 1);
  lv_obj_clear_flag(divider, LV_OBJ_FLAG_CLICKABLE);

  bat = lv_obj_create(screen);
  lv_obj_remove_style_all(bat);
  lv_obj_set_size(bat, 16, 10);
  lv_obj_set_pos(bat, 160, 12);
  lv_obj_set_style_radius(bat, 2, 0);
  lv_obj_set_style_border_width(bat, 1, 0);
  lv_obj_set_style_border_color(bat, lv_color_hex(0x102B6B), 0);

  bat_tip = lv_obj_create(screen);
  lv_obj_remove_style_all(bat_tip);
  lv_obj_set_size(bat_tip, 2, 6);
  lv_obj_align_to(bat_tip, bat, LV_ALIGN_OUT_RIGHT_MID, 1, 0);
  lv_obj_set_style_radius(bat_tip, 1, 0);
  lv_obj_set_style_bg_color(bat_tip, lv_color_hex(0x102B6B), 0);
  lv_obj_set_style_bg_opa(bat_tip, LV_OPA_COVER, 0);

  bat_fill = lv_obj_create(bat);
  lv_obj_remove_style_all(bat_fill);
  lv_obj_set_size(bat_fill, 11, 6);
  lv_obj_align(bat_fill, LV_ALIGN_LEFT_MID, 2, 0);
  lv_obj_set_style_radius(bat_fill, 2, 0);
  lv_obj_set_style_bg_color(bat_fill, lv_color_hex(0x22D35A), 0);
  lv_obj_set_style_bg_opa(bat_fill, LV_OPA_COVER, 0);

  bat_value = lv_label_create(screen);
  lv_label_set_text(bat_value, "--");
  lv_obj_set_style_text_color(bat_value, lv_color_hex(0x0B1633), 0);
  lv_obj_set_style_text_font(bat_value, &lv_font_montserrat_12, 0);
  lv_obj_set_pos(bat_value, 177, 10);
  UpperArm_LvglApplyBatteryDisplay(bat_fill, bat_value, 11, 0U);

  time_label = lv_label_create(screen);
  prep_time_label = time_label;
  home_time_label = time_label;
  lv_label_set_text(time_label, "10:30");
  lv_obj_set_style_text_color(time_label, lv_color_hex(0x0B1633), 0);
  lv_obj_set_style_text_font(time_label, &lv_font_montserrat_12, 0);
  lv_obj_align(time_label, LV_ALIGN_TOP_RIGHT, -4, 10);

  divider = UpperArm_LvglShape(screen, 240, 1, 0, 30, lv_color_hex(0xD6E6FA), 0);
  lv_obj_clear_flag(divider, LV_OBJ_FLAG_CLICKABLE);

  title = lv_label_create(screen);
  lv_label_set_text(title, UI_ZH_KEEP_POSE);
  lv_obj_set_width(title, 230);
  lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_style_text_color(title, lv_color_hex(0x123F95), 0);
  lv_obj_set_style_text_font(title, &upper_font_zh_16, 0);
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 52);

  title_bold = lv_label_create(screen);
  lv_label_set_text(title_bold, UI_ZH_KEEP_POSE);
  lv_obj_set_width(title_bold, 230);
  lv_obj_set_style_text_align(title_bold, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_style_text_color(title_bold, lv_color_hex(0x123F95), 0);
  lv_obj_set_style_text_font(title_bold, &upper_font_zh_16, 0);
  lv_obj_align(title_bold, LV_ALIGN_TOP_MID, 1, 52);

  subtitle = lv_label_create(screen);
  lv_label_set_text(subtitle, UI_ZH_CALIBRATING);
  lv_obj_set_width(subtitle, 116);
  lv_obj_set_style_text_align(subtitle, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_style_text_color(subtitle, lv_color_hex(0x3D66B1), 0);
  lv_obj_set_style_text_font(subtitle, &upper_font_zh_12, 0);
  lv_obj_set_style_text_opa(subtitle, LV_OPA_90, 0);
  lv_obj_align(subtitle, LV_ALIGN_TOP_MID, 0, 77);

  subtitle_bold = lv_label_create(screen);
  lv_label_set_text(subtitle_bold, UI_ZH_CALIBRATING);
  lv_obj_set_width(subtitle_bold, 116);
  lv_obj_set_style_text_align(subtitle_bold, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_style_text_color(subtitle_bold, lv_color_hex(0x3D66B1), 0);
  lv_obj_set_style_text_font(subtitle_bold, &upper_font_zh_12, 0);
  lv_obj_set_style_text_opa(subtitle_bold, LV_OPA_90, 0);
  lv_obj_align(subtitle_bold, LV_ALIGN_TOP_MID, 1, 77);

  for (i = 0U; i < 6U; i++)
  {
    dot = UpperArm_LvglShape(screen, 4, 4,
                             dot_x[i], 81, (i == 2U || i == 3U) ? lv_color_hex(0xA9CFFF) : lv_color_hex(0x4B96F5),
                             LV_RADIUS_CIRCLE);
    lv_obj_clear_flag(dot, LV_OBJ_FLAG_CLICKABLE);
    prep_status_dots[i] = dot;
  }
  UpperArm_PrepUpdateDots(0U);

  ring_bg = lv_arc_create(screen);
  lv_obj_set_size(ring_bg, 166, 166);
  lv_obj_align(ring_bg, LV_ALIGN_CENTER, 0, 31);
  lv_arc_set_rotation(ring_bg, 270);
  lv_arc_set_bg_angles(ring_bg, 0, 360);
  lv_arc_set_range(ring_bg, 0, 100);
  lv_arc_set_value(ring_bg, 100);
  lv_obj_remove_style(ring_bg, NULL, LV_PART_KNOB);
  lv_obj_clear_flag(ring_bg, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_set_style_arc_width(ring_bg, 12, LV_PART_MAIN);
  lv_obj_set_style_arc_color(ring_bg, lv_color_hex(0xD9E9FF), LV_PART_MAIN);
  lv_obj_set_style_arc_opa(ring_bg, LV_OPA_80, LV_PART_MAIN);
  lv_obj_set_style_arc_width(ring_bg, 0, LV_PART_INDICATOR);

  ring_progress = lv_arc_create(screen);
  lv_obj_set_size(ring_progress, 166, 166);
  lv_obj_align(ring_progress, LV_ALIGN_CENTER, 0, 31);
  lv_arc_set_rotation(ring_progress, 270);
  lv_arc_set_bg_angles(ring_progress, 0, 360);
  lv_arc_set_range(ring_progress, 0, 100);
  lv_arc_set_value(ring_progress, 100);
  lv_obj_remove_style(ring_progress, NULL, LV_PART_KNOB);
  lv_obj_clear_flag(ring_progress, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_set_style_arc_width(ring_progress, 0, LV_PART_MAIN);
  lv_obj_set_style_arc_width(ring_progress, 12, LV_PART_INDICATOR);
  lv_obj_set_style_arc_color(ring_progress, lv_color_hex(0x2F80ED), LV_PART_INDICATOR);
  lv_obj_set_style_arc_opa(ring_progress, LV_OPA_COVER, LV_PART_INDICATOR);

  ring_inner = lv_arc_create(screen);
  lv_obj_set_size(ring_inner, 122, 122);
  lv_obj_align(ring_inner, LV_ALIGN_CENTER, 0, 31);
  lv_arc_set_bg_angles(ring_inner, 0, 360);
  lv_arc_set_angles(ring_inner, 0, 360);
  lv_obj_remove_style(ring_inner, NULL, LV_PART_KNOB);
  lv_obj_clear_flag(ring_inner, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_set_style_arc_width(ring_inner, 3, LV_PART_MAIN);
  lv_obj_set_style_arc_width(ring_inner, 0, LV_PART_INDICATOR);
  lv_obj_set_style_arc_color(ring_inner, lv_color_hex(0xD8E8FF), LV_PART_MAIN);
  lv_obj_set_style_arc_opa(ring_inner, LV_OPA_80, LV_PART_MAIN);

  prep_spinner_arc = ring_progress;

  ring_core = lv_obj_create(screen);
  lv_obj_remove_style_all(ring_core);
  lv_obj_set_size(ring_core, 96, 96);
  lv_obj_align(ring_core, LV_ALIGN_CENTER, 0, 31);
  lv_obj_set_style_radius(ring_core, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(ring_core, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_bg_opa(ring_core, LV_OPA_60, 0);
  lv_obj_set_style_border_width(ring_core, 0, 0);
  lv_obj_set_style_shadow_width(ring_core, 20, 0);
  lv_obj_set_style_shadow_color(ring_core, lv_color_hex(0xC7DEFF), 0);
  lv_obj_set_style_shadow_opa(ring_core, LV_OPA_20, 0);

  prep_countdown_label = lv_label_create(ring_core);
  lv_label_set_text(prep_countdown_label, "5");
  lv_obj_set_style_text_color(prep_countdown_label, lv_color_hex(0x1F74F2), 0);
  lv_obj_set_style_text_font(prep_countdown_label, &lv_font_montserrat_48, 0);
  lv_obj_set_style_text_opa(prep_countdown_label, LV_OPA_COVER, 0);
  lv_obj_center(prep_countdown_label);

  prep_countdown_value = 5U;
  prep_remaining_ms = 5000U;
  if (prep_countdown_timer != NULL)
  {
    lv_timer_del(prep_countdown_timer);
  }
  if (prep_recalibrate_timer != NULL)
  {
    lv_timer_del(prep_recalibrate_timer);
    prep_recalibrate_timer = NULL;
  }
  if (prep_transition_timer != NULL)
  {
    lv_timer_del(prep_transition_timer);
    prep_transition_timer = NULL;
  }
  if (training_emg_enable_timer != NULL)
  {
    lv_timer_del(training_emg_enable_timer);
    training_emg_enable_timer = NULL;
  }
  UpperArm_PrepAnimateNumber();
  prep_recalibrate_timer = lv_timer_create(UpperArm_OnPrepRecalibrate, 30U, NULL);
  prep_countdown_timer = lv_timer_create(UpperArm_OnPrepCountdown, 50U, NULL);
  UpperArm_LvglPageReady(screen);
}

static void UpperArm_LvglCreateActionSelect(void)
{
  lv_obj_t *screen = lv_scr_act();
  lv_obj_t *bg;
  lv_obj_t *logo;
  lv_obj_t *logo_text;
  lv_obj_t *brand;
  lv_obj_t *brand_sub;
  lv_obj_t *bat;
  lv_obj_t *bat_tip;
  lv_obj_t *bat_fill;
  lv_obj_t *bat_value;
  lv_obj_t *divider;
  lv_obj_t *title;
  lv_obj_t *title_bold;
  lv_obj_t *subtitle;
  lv_obj_t *btn;
  lv_obj_t *img;
  lv_obj_t *label;
  lv_obj_t *desc;
  lv_obj_t *num;
  lv_obj_t *check;
  lv_obj_t *check_label;
  lv_obj_t *btn_back;
  lv_obj_t *btn_start;
  lv_obj_t *btn_icon;
  static const char *action_texts[4] = {
    UI_ZH_ELBOW_FLEX, UI_ZH_ARM_ABD, UI_ZH_FOREARM_ROT, UI_ZH_SHOULDER_LIFT
  };
  static const char *action_descs[4] = {
    "增强肘部屈曲能力",
    "提升肩关节上举能力",
    "增强前臂旋前旋后能力",
    "改善肩关节外展活动度"
  };
  static const char *action_nums[4] = {"01", "02", "03", "04"};
  static const lv_img_dsc_t *action_imgs[4] = {
    &upper_action_icon_1, &upper_action_icon_3, &upper_action_icon_2, &upper_action_icon_4
  };
  static const lv_coord_t card_x[4] = {8, 124, 8, 124};
  static const lv_coord_t card_y[4] = {68, 68, 164, 164};
  static const lv_coord_t dot_x[6] = {48, 56, 64, 176, 184, 192};
  uint8_t i;

  lv_obj_clean(screen);
  UpperArm_LvglResetPageObjectRefs();
  lv_obj_set_style_bg_color(screen, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_bg_grad_color(screen, lv_color_hex(0xF4FAFF), 0);
  lv_obj_set_style_bg_grad_dir(screen, LV_GRAD_DIR_VER, 0);
  lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

  current_ui_page = UPPER_PAGE_ACTION_SELECT;
  home_status_value_label = NULL;
  home_status_dot = NULL;
  home_bt_icon = NULL;

  bg = lv_img_create(screen);
  lv_img_set_src(bg, UpperArm_GetPrepBg());
  lv_obj_align(bg, LV_ALIGN_CENTER, 0, 0);
  lv_obj_clear_flag(bg, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_move_background(bg);

  logo = lv_obj_create(screen);
  lv_obj_remove_style_all(logo);
  lv_obj_set_size(logo, 18, 18);
  lv_obj_set_pos(logo, 8, 4);
  lv_obj_set_style_radius(logo, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(logo, lv_color_hex(0x1F74F2), 0);
  lv_obj_set_style_bg_opa(logo, LV_OPA_COVER, 0);

  logo_text = lv_label_create(logo);
  lv_label_set_text(logo_text, UI_ZH_LOGO);
  lv_obj_set_style_text_color(logo_text, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_text_font(logo_text, &upper_font_zh_12, 0);
  lv_obj_center(logo_text);

  brand = lv_label_create(screen);
  lv_label_set_text(brand, UI_ZH_BRAND);
  lv_obj_set_style_text_color(brand, lv_color_hex(0x1267DE), 0);
  lv_obj_set_style_text_font(brand, &upper_font_zh_12, 0);
  lv_obj_set_pos(brand, 31, 3);

  brand_sub = lv_label_create(screen);
  lv_label_set_text(brand_sub, UI_ZH_SYSTEM);
  lv_obj_set_style_text_color(brand_sub, lv_color_hex(0x123F95), 0);
  lv_obj_set_style_text_font(brand_sub, &upper_font_zh_12, 0);
  lv_obj_set_pos(brand_sub, 31, 16);

  divider = UpperArm_LvglShape(screen, 1, 18, 154, 5, lv_color_hex(0xC9DCFA), 1);
  lv_obj_clear_flag(divider, LV_OBJ_FLAG_CLICKABLE);

  bat = lv_obj_create(screen);
  lv_obj_remove_style_all(bat);
  lv_obj_set_size(bat, 16, 10);
  lv_obj_set_pos(bat, 156, 9);
  lv_obj_set_style_radius(bat, 2, 0);
  lv_obj_set_style_border_width(bat, 1, 0);
  lv_obj_set_style_border_color(bat, lv_color_hex(0x102B6B), 0);

  bat_tip = lv_obj_create(screen);
  lv_obj_remove_style_all(bat_tip);
  lv_obj_set_size(bat_tip, 2, 6);
  lv_obj_align_to(bat_tip, bat, LV_ALIGN_OUT_RIGHT_MID, 1, 0);
  lv_obj_set_style_radius(bat_tip, 1, 0);
  lv_obj_set_style_bg_color(bat_tip, lv_color_hex(0x102B6B), 0);
  lv_obj_set_style_bg_opa(bat_tip, LV_OPA_COVER, 0);

  bat_fill = lv_obj_create(bat);
  lv_obj_remove_style_all(bat_fill);
  lv_obj_set_size(bat_fill, 11, 6);
  lv_obj_align(bat_fill, LV_ALIGN_LEFT_MID, 2, 0);
  lv_obj_set_style_radius(bat_fill, 2, 0);
  lv_obj_set_style_bg_color(bat_fill, lv_color_hex(0x22D35A), 0);
  lv_obj_set_style_bg_opa(bat_fill, LV_OPA_COVER, 0);

  bat_value = lv_label_create(screen);
  lv_label_set_text(bat_value, "--");
  lv_obj_set_style_text_color(bat_value, lv_color_hex(0x0B1633), 0);
  lv_obj_set_style_text_font(bat_value, &lv_font_montserrat_12, 0);
  lv_obj_set_pos(bat_value, 180, 7);
  UpperArm_LvglApplyBatteryDisplay(bat_fill, bat_value, 11, 0U);

  divider = UpperArm_LvglShape(screen, 1, 18, 214, 5, lv_color_hex(0xC9DCFA), 1);
  lv_obj_clear_flag(divider, LV_OBJ_FLAG_CLICKABLE);

  action_time_label = lv_label_create(screen);
  home_time_label = action_time_label;
  lv_label_set_text(action_time_label, "10:30");
  lv_obj_set_style_text_color(action_time_label, lv_color_hex(0x0B1633), 0);
  lv_obj_set_style_text_font(action_time_label, &lv_font_montserrat_12, 0);
  lv_obj_align(action_time_label, LV_ALIGN_TOP_RIGHT, -4, 7);

  divider = UpperArm_LvglShape(screen, 240, 1, 0, 25, lv_color_hex(0xD6E6FA), 0);
  lv_obj_clear_flag(divider, LV_OBJ_FLAG_CLICKABLE);

  title = lv_label_create(screen);
  lv_label_set_text(title, UI_ZH_SELECT_ACTION_TITLE);
  lv_obj_set_width(title, 220);
  lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_style_text_color(title, lv_color_hex(0x123F95), 0);
  lv_obj_set_style_text_font(title, &upper_font_zh_14, 0);
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 30);

  title_bold = lv_label_create(screen);
  lv_label_set_text(title_bold, UI_ZH_SELECT_ACTION_TITLE);
  lv_obj_set_width(title_bold, 220);
  lv_obj_set_style_text_align(title_bold, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_style_text_color(title_bold, lv_color_hex(0x123F95), 0);
  lv_obj_set_style_text_font(title_bold, &upper_font_zh_14, 0);
  lv_obj_align(title_bold, LV_ALIGN_TOP_MID, 1, 30);

  subtitle = lv_label_create(screen);
  lv_label_set_text(subtitle, UI_ZH_SELECT_ACTION_SUB);
  lv_obj_set_width(subtitle, 190);
  lv_obj_set_style_text_align(subtitle, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_style_text_color(subtitle, lv_color_hex(0x3D66B1), 0);
  lv_obj_set_style_text_font(subtitle, &upper_font_zh_12, 0);
  lv_obj_align(subtitle, LV_ALIGN_TOP_MID, 0, 48);

  for (i = 0U; i < 6U; i++)
  {
    lv_obj_t *dot = UpperArm_LvglShape(screen, (i == 2U || i == 3U) ? 3 : 4,
                                       (i == 2U || i == 3U) ? 3 : 4,
                                       dot_x[i], 36,
                                       (i == 2U || i == 3U) ? lv_color_hex(0xA9CFFF) : lv_color_hex(0x4B96F5),
                                       LV_RADIUS_CIRCLE);
    lv_obj_clear_flag(dot, LV_OBJ_FLAG_CLICKABLE);
  }

  for (i = 0U; i < 4U; i++)
  {
    uint8_t selected = (selected_action_index == i) ? 1U : 0U;

    btn = lv_btn_create(screen);
    lv_obj_remove_style_all(btn);
    lv_obj_set_size(btn, 108, 96);
    lv_obj_set_pos(btn, card_x[i], card_y[i]);
    lv_obj_set_style_radius(btn, 10, 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_grad_color(btn, lv_color_hex(0xF7FBFF), 0);
    lv_obj_set_style_bg_grad_dir(btn, LV_GRAD_DIR_VER, 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(btn, selected ? 2 : 1, 0);
    lv_obj_set_style_border_color(btn, selected ? lv_color_hex(0x1F74F2) : lv_color_hex(0xC7DEFF), 0);
    lv_obj_set_style_shadow_width(btn, selected ? 8 : 4, 0);
    lv_obj_set_style_shadow_color(btn, lv_color_hex(0xBBD8FF), 0);
    lv_obj_set_style_shadow_opa(btn, LV_OPA_10, 0);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(btn, UpperArm_OnActionButtonClicked, LV_EVENT_CLICKED, (void *)(uintptr_t)i);

    img = lv_img_create(btn);
    lv_img_set_src(img, action_imgs[i]);
    lv_obj_align(img, LV_ALIGN_TOP_MID, 8, 20);
    lv_obj_clear_flag(img, LV_OBJ_FLAG_CLICKABLE);

    label = lv_label_create(btn);
    lv_label_set_text(label, action_texts[i]);
    lv_obj_set_width(label, 96);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0x123F95), 0);
    lv_obj_set_style_text_font(label, &upper_font_zh_14, 0);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 61);

    label = lv_label_create(btn);
    lv_label_set_text(label, action_texts[i]);
    lv_obj_set_width(label, 96);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0x123F95), 0);
    lv_obj_set_style_text_font(label, &upper_font_zh_14, 0);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 1, 61);

    desc = lv_label_create(btn);
    lv_label_set_text(desc, action_descs[i]);
    lv_obj_set_width(desc, 106);
    lv_label_set_long_mode(desc, LV_LABEL_LONG_CLIP);
    lv_obj_set_style_text_align(desc, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(desc, lv_color_hex(0x4E79C6), 0);
    lv_obj_set_style_text_font(desc, &upper_font_zh_9, 0);
    lv_obj_align(desc, LV_ALIGN_TOP_MID, 0, 78);

    num = lv_label_create(btn);
    lv_label_set_text(num, action_nums[i]);
    lv_obj_set_style_text_color(num, selected ? lv_color_hex(0x1F74F2) : lv_color_hex(0x5B7DBA), 0);
    lv_obj_set_style_text_font(num, &lv_font_montserrat_16, 0);
    lv_obj_set_style_bg_color(num, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(num, LV_OPA_70, 0);
    lv_obj_set_pos(num, 8, 5);

    if (selected != 0U)
    {
      check = lv_obj_create(btn);
      lv_obj_remove_style_all(check);
      lv_obj_set_size(check, 17, 17);
      lv_obj_align(check, LV_ALIGN_TOP_RIGHT, -7, 7);
      lv_obj_set_style_radius(check, LV_RADIUS_CIRCLE, 0);
      lv_obj_set_style_bg_color(check, lv_color_hex(0x1F74F2), 0);
      lv_obj_set_style_bg_opa(check, LV_OPA_COVER, 0);

      check_label = lv_label_create(check);
      lv_label_set_text(check_label, LV_SYMBOL_OK);
      lv_obj_set_style_text_color(check_label, lv_color_hex(0xFFFFFF), 0);
      lv_obj_set_style_text_font(check_label, &lv_font_montserrat_14, 0);
      lv_obj_center(check_label);
    }
  }

  btn_back = lv_btn_create(screen);
  lv_obj_remove_style_all(btn_back);
  lv_obj_set_size(btn_back, 104, 34);
  lv_obj_set_pos(btn_back, 12, 282);
  lv_obj_set_style_radius(btn_back, 10, 0);
  lv_obj_set_style_bg_color(btn_back, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_bg_opa(btn_back, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(btn_back, 1, 0);
  lv_obj_set_style_border_color(btn_back, lv_color_hex(0xC7DEFF), 0);
  lv_obj_set_style_shadow_width(btn_back, 4, 0);
  lv_obj_set_style_shadow_opa(btn_back, LV_OPA_10, 0);
  lv_obj_clear_flag(btn_back, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_event_cb(btn_back, UpperArm_OnBackHomeClicked, LV_EVENT_CLICKED, NULL);

  btn_icon = lv_label_create(btn_back);
  lv_label_set_text(btn_icon, LV_SYMBOL_LEFT);
  lv_obj_set_style_text_color(btn_icon, lv_color_hex(0x4E79C6), 0);
  lv_obj_set_style_text_font(btn_icon, &lv_font_montserrat_24, 0);
  lv_obj_align(btn_icon, LV_ALIGN_CENTER, -20, 0);

  label = lv_label_create(btn_back);
  lv_label_set_text(label, "返回");
  lv_obj_set_style_text_color(label, lv_color_hex(0x4E79C6), 0);
  lv_obj_set_style_text_font(label, &upper_font_zh_16, 0);
  lv_obj_align(label, LV_ALIGN_CENTER, 14, 0);

  btn_start = lv_btn_create(screen);
  lv_obj_remove_style_all(btn_start);
  lv_obj_set_size(btn_start, 104, 34);
  lv_obj_set_pos(btn_start, 124, 282);
  lv_obj_set_style_radius(btn_start, 10, 0);
  lv_obj_set_style_bg_color(btn_start, lv_color_hex(0x2F80ED), 0);
  lv_obj_set_style_bg_grad_color(btn_start, lv_color_hex(0x0067E8), 0);
  lv_obj_set_style_bg_grad_dir(btn_start, LV_GRAD_DIR_VER, 0);
  lv_obj_set_style_bg_opa(btn_start, LV_OPA_COVER, 0);
  lv_obj_set_style_shadow_width(btn_start, 8, 0);
  lv_obj_set_style_shadow_color(btn_start, lv_color_hex(0x77B6FF), 0);
  lv_obj_set_style_shadow_opa(btn_start, LV_OPA_20, 0);
  lv_obj_clear_flag(btn_start, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_event_cb(btn_start, UpperArm_OnStartTrainingClicked, LV_EVENT_CLICKED, NULL);

  btn_icon = lv_label_create(btn_start);
  lv_label_set_text(btn_icon, LV_SYMBOL_PLAY);
  lv_obj_set_style_text_color(btn_icon, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_text_font(btn_icon, &lv_font_montserrat_24, 0);
  lv_obj_align(btn_icon, LV_ALIGN_CENTER, -34, 0);

  label = lv_label_create(btn_start);
  lv_label_set_text(label, "开始训练");
  lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_text_font(label, &upper_font_zh_16, 0);
  lv_obj_align(label, LV_ALIGN_CENTER, 14, 0);

  UpperArm_LvglPageReady(screen);
}

static void UpperArm_LvglCreateLinkLost(void)
{
  lv_obj_t *screen = lv_scr_act();
  lv_obj_t *bg;
  lv_obj_t *icon_box;
  lv_obj_t *title;
  lv_obj_t *subtitle;
  lv_obj_t *panel;
  lv_obj_t *hint_1;
  lv_obj_t *hint_2;
  lv_obj_t *btn_home;
  lv_obj_t *label;
  lv_obj_t *btn_icon;
  lv_obj_t *icon_inner;

  if (prep_countdown_timer != NULL)
  {
    lv_timer_del(prep_countdown_timer);
    prep_countdown_timer = NULL;
  }
  if (prep_recalibrate_timer != NULL)
  {
    lv_timer_del(prep_recalibrate_timer);
    prep_recalibrate_timer = NULL;
  }
  if (prep_transition_timer != NULL)
  {
    lv_timer_del(prep_transition_timer);
    prep_transition_timer = NULL;
  }

  current_ui_page = UPPER_PAGE_LINK_LOST;
  UpperArm_RequestVibration();
  lv_obj_clean(screen);
  UpperArm_LvglResetPageObjectRefs();
  lv_obj_set_style_bg_color(screen, lv_color_hex(0xF4F8FF), 0);
  lv_obj_set_style_bg_grad_color(screen, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_bg_grad_dir(screen, LV_GRAD_DIR_VER, 0);
  lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

  bg = lv_img_create(screen);
  lv_img_set_src(bg, UpperArm_GetHomeBg());
  lv_obj_set_size(bg, 240, 320);
  lv_obj_align(bg, LV_ALIGN_CENTER, 0, 0);
  lv_obj_clear_flag(bg, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_move_background(bg);

  icon_box = lv_obj_create(screen);
  lv_obj_remove_style_all(icon_box);
  lv_obj_set_size(icon_box, 82, 82);
  lv_obj_align(icon_box, LV_ALIGN_TOP_MID, 0, 50);
  lv_obj_set_style_radius(icon_box, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(icon_box, lv_color_hex(0xFFEDEC), 0);
  lv_obj_set_style_bg_opa(icon_box, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(icon_box, 2, 0);
  lv_obj_set_style_border_color(icon_box, lv_color_hex(0xF7B3AE), 0);

  icon_inner = lv_obj_create(icon_box);
  lv_obj_remove_style_all(icon_inner);
  lv_obj_set_size(icon_inner, 44, 56);
  lv_obj_center(icon_inner);
  lv_obj_clear_flag(icon_inner, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_clear_flag(icon_inner, LV_OBJ_FLAG_SCROLLABLE);
  UpperArm_LvglCreateDisconnectedBluetoothIcon(icon_inner);

  title = lv_label_create(screen);
  lv_label_set_text(title, UI_ZH_LINK_LOST_TITLE);
  lv_obj_set_style_text_color(title, lv_color_hex(0x102B6B), 0);
  lv_obj_set_style_text_font(title, &upper_font_zh_16, 0);
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 144);

  subtitle = lv_label_create(screen);
  lv_label_set_text(subtitle, UI_ZH_LINK_LOST_SUB);
  lv_obj_set_style_text_color(subtitle, lv_color_hex(0x52617D), 0);
  lv_obj_set_style_text_font(subtitle, &upper_font_zh_12, 0);
  lv_obj_align(subtitle, LV_ALIGN_TOP_MID, 0, 170);

  panel = lv_obj_create(screen);
  lv_obj_remove_style_all(panel);
  lv_obj_set_size(panel, 212, 58);
  lv_obj_align(panel, LV_ALIGN_TOP_MID, 0, 202);
  lv_obj_set_style_radius(panel, 8, 0);
  lv_obj_set_style_bg_color(panel, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_bg_opa(panel, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(panel, 1, 0);
  lv_obj_set_style_border_color(panel, lv_color_hex(0xDCEBFF), 0);
  lv_obj_set_style_shadow_width(panel, 6, 0);
  lv_obj_set_style_shadow_opa(panel, LV_OPA_10, 0);
  lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE);

  hint_1 = lv_label_create(panel);
  lv_label_set_text(hint_1, UI_ZH_LINK_LOST_HINT1);
  lv_obj_set_style_text_color(hint_1, lv_color_hex(0x102B6B), 0);
  lv_obj_set_style_text_font(hint_1, &upper_font_zh_12, 0);
  lv_obj_align(hint_1, LV_ALIGN_TOP_MID, 0, 10);

  hint_2 = lv_label_create(panel);
  lv_label_set_text(hint_2, UI_ZH_LINK_LOST_HINT2);
  lv_obj_set_style_text_color(hint_2, lv_color_hex(0x52617D), 0);
  lv_obj_set_style_text_font(hint_2, &upper_font_zh_10, 0);
  lv_obj_align(hint_2, LV_ALIGN_TOP_MID, 0, 32);

  btn_home = lv_btn_create(screen);
  lv_obj_remove_style_all(btn_home);
  lv_obj_set_size(btn_home, 132, 34);
  lv_obj_align(btn_home, LV_ALIGN_BOTTOM_MID, 0, -10);
  lv_obj_set_style_radius(btn_home, 8, 0);
  lv_obj_set_style_bg_color(btn_home, lv_color_hex(0x2F80ED), 0);
  lv_obj_set_style_bg_grad_color(btn_home, lv_color_hex(0x005DE8), 0);
  lv_obj_set_style_bg_grad_dir(btn_home, LV_GRAD_DIR_VER, 0);
  lv_obj_set_style_bg_opa(btn_home, LV_OPA_COVER, 0);
  lv_obj_clear_flag(btn_home, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_event_cb(btn_home, UpperArm_OnBackHomeClicked, LV_EVENT_CLICKED, NULL);

  btn_icon = lv_label_create(btn_home);
  lv_label_set_text(btn_icon, LV_SYMBOL_HOME);
  lv_obj_set_style_text_color(btn_icon, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_text_font(btn_icon, &lv_font_montserrat_14, 0);
  lv_obj_align(btn_icon, LV_ALIGN_CENTER, -36, 0);

  label = lv_label_create(btn_home);
  lv_label_set_text(label, UI_ZH_BACK_HOME);
  lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_text_font(label, &upper_font_zh_14, 0);
  lv_obj_align(label, LV_ALIGN_CENTER, 14, 0);

  UpperArm_LvglPageReady(screen);
}

static void UpperArm_LvglCreateTraining(void)
{
  lv_obj_t *screen = lv_scr_act();
  lv_obj_t *bg;
  lv_obj_t *logo;
  lv_obj_t *logo_text;
  lv_obj_t *brand;
  lv_obj_t *bat;
  lv_obj_t *bat_tip;
  lv_obj_t *bat_fill;
  lv_obj_t *bat_value;
  lv_obj_t *divider;
  lv_obj_t *panel;
  lv_obj_t *status_pill;
  lv_obj_t *status_dot;
  lv_obj_t *title;
  lv_obj_t *label;
  lv_obj_t *action_label;
  lv_obj_t *score_card;
  lv_obj_t *score_title;
  lv_obj_t *score_value;
  lv_obj_t *score_unit;
  lv_obj_t *badge;
  lv_obj_t *gauge_bg;
  lv_obj_t *gauge;
  lv_obj_t *gauge_core;
  lv_obj_t *angle_label;
  lv_obj_t *angle_sub;
  lv_obj_t *metric_card;
  lv_obj_t *bar;
  lv_obj_t *bar_value;
  lv_obj_t *chart;
  lv_chart_series_t *series;
  lv_obj_t *pause_btn;
  lv_obj_t *finish_btn;
  lv_obj_t *btn_icon;
  lv_obj_t *count_box;
  lv_obj_t *count_label;
  uint8_t action_index;
  static const char *action_texts[4] = {
    UI_ZH_ELBOW_FLEX, UI_ZH_ARM_ABD, UI_ZH_FOREARM_ROT, UI_ZH_SHOULDER_LIFT
  };
  uint8_t i;

  if (selected_action_index >= 4U)
  {
    selected_action_index = 0U;
  }
  action_index = selected_action_index;

  lv_obj_clean(screen);
  UpperArm_LvglResetPageObjectRefs();
  lv_obj_set_style_bg_color(screen, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_bg_grad_color(screen, lv_color_hex(0xEEF6FF), 0);
  lv_obj_set_style_bg_grad_dir(screen, LV_GRAD_DIR_VER, 0);
  lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

  current_ui_page = UPPER_PAGE_TRAINING;
  esp_upload_pause_until_tick = osKernelGetTickCount() + ESP8266_UI_TRANSITION_PAUSE_MS;
  EmgSensor_SetModelEnabled(0U);
  training_paused = 0U;
  training_live_reps = 0U;
  training_eval_reset_request = 1U;
  wireless_train_start_pending = 1U;
  training_start_tick = osKernelGetTickCount();
  training_pause_start_tick = 0U;
  training_paused_accum_ms = 0U;
  training_pause_icon_label = NULL;
  training_pause_text_label = NULL;
  training_count_label = NULL;
  training_timer_arc = NULL;
  training_timer_value_label = NULL;
  training_score_value_label = NULL;
  training_emg_mode_label = NULL;
  training_emg_strength_label = NULL;
  training_completion_bar = NULL;
  training_completion_value_label = NULL;
  training_amplitude_bar = NULL;
  training_amplitude_value_label = NULL;
  training_angle_value_label = NULL;
  training_angle_chart = NULL;
  training_angle_series = NULL;
  training_angle_chart_last_tick = 0U;
  home_status_value_label = NULL;
  home_status_dot = NULL;
  home_bt_icon = NULL;
  memset(&training_eval_snapshot, 0, sizeof(training_eval_snapshot));

  bg = lv_img_create(screen);
  lv_img_set_src(bg, UpperArm_GetPrepBg());
  lv_obj_set_size(bg, 240, 320);
  lv_obj_align(bg, LV_ALIGN_CENTER, 0, 0);
  lv_obj_clear_flag(bg, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_move_background(bg);

  logo = lv_obj_create(screen);
  lv_obj_remove_style_all(logo);
  lv_obj_set_size(logo, 20, 20);
  lv_obj_set_pos(logo, 10, 6);
  lv_obj_set_style_radius(logo, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(logo, lv_color_hex(0x2F80ED), 0);
  lv_obj_set_style_bg_opa(logo, LV_OPA_COVER, 0);

  logo_text = lv_label_create(logo);
  lv_label_set_text(logo_text, UI_ZH_LOGO);
  lv_obj_set_style_text_color(logo_text, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_text_font(logo_text, &upper_font_zh_14, 0);
  lv_obj_center(logo_text);

  brand = lv_label_create(screen);
  lv_label_set_text(brand, UI_ZH_BRAND);
  lv_obj_set_style_text_color(brand, lv_color_hex(0x176BEF), 0);
  lv_obj_set_style_text_font(brand, &upper_font_zh_14, 0);
  lv_obj_set_pos(brand, 36, 4);

  bat = lv_obj_create(screen);
  lv_obj_remove_style_all(bat);
  lv_obj_set_size(bat, 20, 11);
  lv_obj_set_pos(bat, 142, 9);
  lv_obj_set_style_radius(bat, 2, 0);
  lv_obj_set_style_border_width(bat, 1, 0);
  lv_obj_set_style_border_color(bat, lv_color_hex(0x102B6B), 0);

  bat_tip = lv_obj_create(screen);
  lv_obj_remove_style_all(bat_tip);
  lv_obj_set_size(bat_tip, 2, 6);
  lv_obj_align_to(bat_tip, bat, LV_ALIGN_OUT_RIGHT_MID, 1, 0);
  lv_obj_set_style_radius(bat_tip, 1, 0);
  lv_obj_set_style_bg_color(bat_tip, lv_color_hex(0x102B6B), 0);
  lv_obj_set_style_bg_opa(bat_tip, LV_OPA_COVER, 0);

  bat_fill = lv_obj_create(bat);
  lv_obj_remove_style_all(bat_fill);
  lv_obj_set_size(bat_fill, 13, 7);
  lv_obj_set_pos(bat_fill, 2, 2);
  lv_obj_set_style_radius(bat_fill, 1, 0);
  lv_obj_set_style_bg_color(bat_fill, lv_color_hex(0x28D85A), 0);
  lv_obj_set_style_bg_opa(bat_fill, LV_OPA_COVER, 0);

  bat_value = lv_label_create(screen);
  lv_label_set_text(bat_value, "--");
  lv_obj_set_style_text_color(bat_value, lv_color_hex(0x000000), 0);
  lv_obj_set_style_text_font(bat_value, &lv_font_montserrat_14, 0);
  lv_obj_set_pos(bat_value, 166, 6);
  UpperArm_LvglApplyBatteryDisplay(bat_fill, bat_value, 13, 0U);

  training_time_label = NULL;
  home_time_label = NULL;

  divider = lv_obj_create(screen);
  lv_obj_remove_style_all(divider);
  lv_obj_set_size(divider, 240, 1);
  lv_obj_set_pos(divider, 0, 38);
  lv_obj_set_style_bg_color(divider, lv_color_hex(0xCFE2FF), 0);
  lv_obj_set_style_bg_opa(divider, LV_OPA_70, 0);

  status_pill = lv_obj_create(screen);
  lv_obj_remove_style_all(status_pill);
  lv_obj_set_size(status_pill, 45, 18);
  lv_obj_set_pos(status_pill, 8, 48);
  lv_obj_set_style_radius(status_pill, 9, 0);
  lv_obj_set_style_bg_color(status_pill, lv_color_hex(0xE8FFF0), 0);
  lv_obj_set_style_bg_opa(status_pill, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(status_pill, 1, 0);
  lv_obj_set_style_border_color(status_pill, lv_color_hex(0x27C75F), 0);

  status_dot = UpperArm_LvglShape(status_pill, 5, 5, 7, 6, lv_color_hex(0x24C65A), LV_RADIUS_CIRCLE);
  lv_obj_clear_flag(status_dot, LV_OBJ_FLAG_CLICKABLE);

  title = lv_label_create(status_pill);
  lv_label_set_text(title, UI_ZH_RUNNING);
  lv_obj_set_style_text_color(title, lv_color_hex(0x21B653), 0);
  lv_obj_set_style_text_font(title, &upper_font_zh_10, 0);
  lv_obj_set_pos(title, 15, 3);

  title = lv_label_create(screen);
  lv_label_set_text(title, UI_ZH_CURRENT_ACTION);
  lv_obj_set_style_text_color(title, lv_color_hex(0x102B6B), 0);
  lv_obj_set_style_text_font(title, &upper_font_zh_14, 0);
  lv_obj_set_pos(title, 74, 50);

  action_label = lv_label_create(screen);
  lv_label_set_text(action_label, action_texts[action_index]);
  lv_obj_set_style_text_color(action_label, lv_color_hex(0x1F74F2), 0);
  lv_obj_set_style_text_font(action_label, &upper_font_zh_16, 0);
  lv_obj_set_pos(action_label, 137, 48);

  panel = lv_obj_create(screen);
  lv_obj_remove_style_all(panel);
  lv_obj_set_size(panel, 132, 124);
  lv_obj_set_pos(panel, 8, 75);
  lv_obj_set_style_radius(panel, 8, 0);
  lv_obj_set_style_bg_color(panel, lv_color_hex(0xF8FBFF), 0);
  lv_obj_set_style_bg_opa(panel, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(panel, 1, 0);
  lv_obj_set_style_border_color(panel, lv_color_hex(0xD8E8FF), 0);

  gauge_bg = lv_arc_create(panel);
  lv_obj_set_size(gauge_bg, 116, 116);
  lv_obj_set_pos(gauge_bg, 8, 6);
  lv_arc_set_rotation(gauge_bg, 135);
  lv_arc_set_bg_angles(gauge_bg, 0, 270);
  lv_arc_set_range(gauge_bg, 0, (int16_t)training_target_duration_sec);
  lv_arc_set_value(gauge_bg, (int16_t)training_target_duration_sec);
  lv_obj_remove_style(gauge_bg, NULL, LV_PART_KNOB);
  lv_obj_clear_flag(gauge_bg, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_set_style_arc_width(gauge_bg, 8, LV_PART_MAIN);
  lv_obj_set_style_arc_color(gauge_bg, lv_color_hex(0xD9E8FF), LV_PART_MAIN);
  lv_obj_set_style_arc_width(gauge_bg, 0, LV_PART_INDICATOR);

  gauge = lv_arc_create(panel);
  lv_obj_set_size(gauge, 116, 116);
  lv_obj_set_pos(gauge, 8, 6);
  lv_arc_set_rotation(gauge, 135);
  lv_arc_set_bg_angles(gauge, 0, 270);
  lv_arc_set_range(gauge, 0, (int16_t)training_target_duration_sec);
  lv_arc_set_value(gauge, 28);
  training_timer_arc = gauge;
  lv_obj_remove_style(gauge, NULL, LV_PART_KNOB);
  lv_obj_clear_flag(gauge, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_set_style_arc_width(gauge, 0, LV_PART_MAIN);
  lv_obj_set_style_arc_width(gauge, 8, LV_PART_INDICATOR);
  lv_obj_set_style_arc_color(gauge, lv_color_hex(0x2F80ED), LV_PART_INDICATOR);

  label = lv_label_create(panel);
  lv_label_set_text_fmt(label, "%u", (unsigned int)(training_target_duration_sec / 2U));
  lv_obj_set_style_text_color(label, lv_color_hex(0x6E86B9), 0);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_12, 0);
  lv_obj_set_pos(label, 60, 32);

  label = lv_label_create(panel);
  lv_label_set_text(label, "0");
  lv_obj_set_style_text_color(label, lv_color_hex(0x6E86B9), 0);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_12, 0);
  lv_obj_set_pos(label, 20, 96);

  label = lv_label_create(panel);
  lv_label_set_text_fmt(label, "%u", (unsigned int)training_target_duration_sec);
  lv_obj_set_style_text_color(label, lv_color_hex(0x6E86B9), 0);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_12, 0);
  lv_obj_set_pos(label, 92, 96);

  gauge_core = lv_obj_create(panel);
  lv_obj_remove_style_all(gauge_core);
  lv_obj_set_size(gauge_core, 82, 82);
  lv_obj_set_pos(gauge_core, 25, 23);
  lv_obj_set_style_radius(gauge_core, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(gauge_core, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_bg_opa(gauge_core, LV_OPA_90, 0);
  lv_obj_set_style_border_width(gauge_core, 1, 0);
  lv_obj_set_style_border_color(gauge_core, lv_color_hex(0xD8E8FF), 0);
  lv_obj_set_style_shadow_width(gauge_core, 8, 0);
  lv_obj_set_style_shadow_opa(gauge_core, LV_OPA_10, 0);

  angle_label = lv_label_create(gauge_core);
  lv_label_set_text(angle_label, "00:28");
  lv_obj_set_style_text_color(angle_label, lv_color_hex(0x126BFF), 0);
  lv_obj_set_style_text_font(angle_label, &lv_font_montserrat_24, 0);
  lv_obj_align(angle_label, LV_ALIGN_CENTER, 0, -5);
  training_timer_value_label = angle_label;

  angle_sub = lv_label_create(gauge_core);
  lv_label_set_text(angle_sub, UI_ZH_TRAIN_TIMER);
  lv_obj_set_style_text_color(angle_sub, lv_color_hex(0x3D66B1), 0);
  lv_obj_set_style_text_font(angle_sub, &upper_font_zh_12, 0);
  lv_obj_align(angle_sub, LV_ALIGN_CENTER, 0, 24);

  score_card = lv_obj_create(screen);
  lv_obj_remove_style_all(score_card);
  lv_obj_set_size(score_card, 88, 70);
  lv_obj_set_pos(score_card, 144, 75);
  lv_obj_set_style_radius(score_card, 8, 0);
  lv_obj_set_style_bg_color(score_card, lv_color_hex(0xF8FBFF), 0);
  lv_obj_set_style_bg_opa(score_card, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(score_card, 1, 0);
  lv_obj_set_style_border_color(score_card, lv_color_hex(0xDCEAFF), 0);

  score_title = lv_label_create(score_card);
  lv_label_set_text(score_title, UI_ZH_ACTION_COUNT);
  lv_obj_set_style_text_color(score_title, lv_color_hex(0x102B6B), 0);
  lv_obj_set_style_text_font(score_title, &upper_font_zh_12, 0);
  lv_obj_align(score_title, LV_ALIGN_TOP_MID, 0, 6);

  score_value = lv_label_create(score_card);
  lv_label_set_text(score_value, "0");
  lv_obj_set_style_text_color(score_value, lv_color_hex(0x126BFF), 0);
  lv_obj_set_style_text_font(score_value, &lv_font_montserrat_24, 0);
  lv_obj_align(score_value, LV_ALIGN_CENTER, -8, 4);
  training_score_value_label = score_value;

  score_unit = lv_label_create(score_card);
  lv_label_set_text(score_unit, UI_ZH_COUNT_UNIT);
  lv_obj_set_style_text_color(score_unit, lv_color_hex(0x102B6B), 0);
  lv_obj_set_style_text_font(score_unit, &upper_font_zh_12, 0);
  lv_obj_align_to(score_unit, score_value, LV_ALIGN_OUT_RIGHT_BOTTOM, 1, -3);

  training_emg_mode_label = lv_label_create(score_card);
  lv_label_set_text(training_emg_mode_label, "放松");
  lv_obj_set_style_text_color(training_emg_mode_label, lv_color_hex(0x27AE60), 0);
  lv_obj_set_style_text_font(training_emg_mode_label, &upper_font_zh_12, 0);
  lv_obj_set_pos(training_emg_mode_label, 8, 53);

  training_emg_strength_label = lv_label_create(score_card);
  lv_label_set_text(training_emg_strength_label, "0");
  lv_obj_set_style_text_color(training_emg_strength_label, lv_color_hex(0x6E86B9), 0);
  lv_obj_set_style_text_font(training_emg_strength_label, &lv_font_montserrat_12, 0);
  lv_obj_set_pos(training_emg_strength_label, 63, 53);

  metric_card = lv_obj_create(screen);
  lv_obj_remove_style_all(metric_card);
  lv_obj_set_size(metric_card, 88, 50);
  lv_obj_set_pos(metric_card, 144, 149);
  lv_obj_set_style_radius(metric_card, 8, 0);
  lv_obj_set_style_bg_color(metric_card, lv_color_hex(0xF8FBFF), 0);
  lv_obj_set_style_bg_opa(metric_card, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(metric_card, 1, 0);
  lv_obj_set_style_border_color(metric_card, lv_color_hex(0xDCEAFF), 0);

  title = lv_label_create(metric_card);
  lv_label_set_text(title, UI_ZH_COMPLETION);
  lv_obj_set_style_text_color(title, lv_color_hex(0x102B6B), 0);
  lv_obj_set_style_text_font(title, &upper_font_zh_10, 0);
  lv_obj_set_pos(title, 8, 5);

  bar = lv_obj_create(metric_card);
  lv_obj_remove_style_all(bar);
  lv_obj_set_size(bar, 72, 7);
  lv_obj_set_pos(bar, 8, 29);
  lv_obj_set_style_radius(bar, 4, 0);
  lv_obj_set_style_bg_color(bar, lv_color_hex(0xDDEBFF), 0);
  lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, 0);
  lv_obj_clear_flag(bar, LV_OBJ_FLAG_SCROLLABLE);

  training_completion_bar = lv_obj_create(bar);
  lv_obj_remove_style_all(training_completion_bar);
  lv_obj_set_size(training_completion_bar, 0, 7);
  lv_obj_set_pos(training_completion_bar, 0, 0);
  lv_obj_set_style_radius(training_completion_bar, 4, 0);
  lv_obj_set_style_bg_color(training_completion_bar, lv_color_hex(0x126BFF), 0);
  lv_obj_set_style_bg_grad_color(training_completion_bar, lv_color_hex(0x23C76A), 0);
  lv_obj_set_style_bg_grad_dir(training_completion_bar, LV_GRAD_DIR_HOR, 0);
  lv_obj_set_style_bg_opa(training_completion_bar, LV_OPA_COVER, 0);
  lv_obj_clear_flag(training_completion_bar, LV_OBJ_FLAG_SCROLLABLE);

  bar_value = lv_label_create(metric_card);
  lv_label_set_text(bar_value, "0%");
  lv_obj_set_style_text_color(bar_value, lv_color_hex(0x102B6B), 0);
  lv_obj_set_style_text_font(bar_value, &lv_font_montserrat_12, 0);
  lv_obj_set_pos(bar_value, 63, 3);
  training_completion_value_label = bar_value;

  panel = lv_obj_create(screen);
  lv_obj_remove_style_all(panel);
  lv_obj_set_size(panel, 224, 66);
  lv_obj_set_pos(panel, 8, 205);
  lv_obj_set_style_radius(panel, 8, 0);
  lv_obj_set_style_bg_color(panel, lv_color_hex(0xF8FBFF), 0);
  lv_obj_set_style_bg_opa(panel, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(panel, 1, 0);
  lv_obj_set_style_border_color(panel, lv_color_hex(0xDCEAFF), 0);

  title = lv_label_create(panel);
  lv_label_set_text(title, UI_ZH_ANGLE_CURVE);
  lv_obj_set_style_text_color(title, lv_color_hex(0x102B6B), 0);
  lv_obj_set_style_text_font(title, &upper_font_zh_14, 0);
  lv_obj_set_pos(title, 8, 5);

  label = lv_label_create(panel);
  lv_label_set_text(label, "0°");
  lv_obj_set_style_text_color(label, lv_color_hex(0x1F74F2), 0);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_12, 0);
  lv_obj_set_pos(label, 187, 7);
  training_angle_value_label = label;

  chart = lv_chart_create(panel);
  lv_obj_remove_style_all(chart);
  lv_obj_set_size(chart, 198, 34);
  lv_obj_set_pos(chart, 15, 27);
  lv_obj_set_style_bg_opa(chart, LV_OPA_TRANSP, 0);
  lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
  lv_chart_set_point_count(chart, 10);
  lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 180);
  lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_SHIFT);
  lv_chart_set_div_line_count(chart, 3, 4);
  lv_obj_set_style_size(chart, 3, LV_PART_INDICATOR);
  lv_obj_set_style_line_width(chart, 2, LV_PART_ITEMS);
  lv_obj_set_style_line_color(chart, lv_color_hex(0x2F80ED), LV_PART_ITEMS);
  lv_obj_set_style_line_color(chart, lv_color_hex(0xDCEAFF), LV_PART_MAIN);
  lv_obj_set_style_line_width(chart, 1, LV_PART_MAIN);
  series = lv_chart_add_series(chart, lv_color_hex(0x2F80ED), LV_CHART_AXIS_PRIMARY_Y);
  for (i = 0U; i < 10U; i++)
  {
    series->y_points[i] = 0;
  }
  lv_chart_refresh(chart);
  training_angle_chart = chart;
  training_angle_series = series;

  pause_btn = lv_btn_create(screen);
  lv_obj_remove_style_all(pause_btn);
  lv_obj_set_size(pause_btn, 76, 34);
  lv_obj_set_pos(pause_btn, 8, 280);
  lv_obj_set_style_radius(pause_btn, 8, 0);
  lv_obj_set_style_bg_color(pause_btn, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_bg_opa(pause_btn, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(pause_btn, 1, 0);
  lv_obj_set_style_border_color(pause_btn, lv_color_hex(0xDCEAFF), 0);
  lv_obj_clear_flag(pause_btn, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_event_cb(pause_btn, UpperArm_OnPauseTrainingClicked, LV_EVENT_CLICKED, NULL);

  btn_icon = lv_label_create(pause_btn);
  lv_label_set_text(btn_icon, LV_SYMBOL_PAUSE);
  lv_obj_set_style_text_color(btn_icon, lv_color_hex(0x2F80ED), 0);
  lv_obj_set_style_text_font(btn_icon, &lv_font_montserrat_16, 0);
  lv_obj_align(btn_icon, LV_ALIGN_CENTER, -18, 0);
  training_pause_icon_label = btn_icon;

  label = lv_label_create(pause_btn);
  lv_label_set_text(label, UI_ZH_PAUSE);
  lv_obj_set_style_text_color(label, lv_color_hex(0x102B6B), 0);
  lv_obj_set_style_text_font(label, &upper_font_zh_14, 0);
  lv_obj_align(label, LV_ALIGN_CENTER, 13, 0);
  training_pause_text_label = label;

  count_box = lv_obj_create(screen);
  lv_obj_remove_style_all(count_box);
  lv_obj_set_size(count_box, 58, 34);
  lv_obj_set_pos(count_box, 88, 280);
  lv_obj_set_style_radius(count_box, 8, 0);
  lv_obj_set_style_bg_color(count_box, lv_color_hex(0xF8FBFF), 0);
  lv_obj_set_style_bg_opa(count_box, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(count_box, 1, 0);
  lv_obj_set_style_border_color(count_box, lv_color_hex(0xDCEAFF), 0);

  count_label = lv_label_create(count_box);
  lv_label_set_text_fmt(count_label, "%u / %u",
                        (unsigned int)training_completed_count,
                        (unsigned int)training_target_count);
  lv_obj_set_style_text_color(count_label, lv_color_hex(0x1F74F2), 0);
  lv_obj_set_style_text_font(count_label, &lv_font_montserrat_14, 0);
  lv_obj_align(count_label, LV_ALIGN_TOP_MID, 0, 3);
  training_count_label = count_label;

  label = lv_label_create(count_box);
  lv_label_set_text(label, UI_ZH_TRAIN_COUNT);
  lv_obj_set_style_text_color(label, lv_color_hex(0x5D7098), 0);
  lv_obj_set_style_text_font(label, &upper_font_zh_9, 0);
  lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, -3);

  finish_btn = lv_btn_create(screen);
  lv_obj_remove_style_all(finish_btn);
  lv_obj_set_size(finish_btn, 84, 34);
  lv_obj_set_pos(finish_btn, 148, 280);
  lv_obj_set_style_radius(finish_btn, 8, 0);
  lv_obj_set_style_bg_color(finish_btn, lv_color_hex(0xE74C3C), 0);
  lv_obj_set_style_bg_grad_color(finish_btn, lv_color_hex(0xD63031), 0);
  lv_obj_set_style_bg_grad_dir(finish_btn, LV_GRAD_DIR_VER, 0);
  lv_obj_set_style_bg_opa(finish_btn, LV_OPA_COVER, 0);
  lv_obj_set_style_shadow_width(finish_btn, 6, 0);
  lv_obj_set_style_shadow_opa(finish_btn, LV_OPA_20, 0);
  lv_obj_clear_flag(finish_btn, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_event_cb(finish_btn, UpperArm_OnFinishTrainingClicked, LV_EVENT_CLICKED, NULL);

  btn_icon = lv_label_create(finish_btn);
  lv_label_set_text(btn_icon, LV_SYMBOL_STOP);
  lv_obj_set_style_text_color(btn_icon, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_text_font(btn_icon, &lv_font_montserrat_14, 0);
  lv_obj_align(btn_icon, LV_ALIGN_CENTER, -28, 0);

  label = lv_label_create(finish_btn);
  lv_label_set_text(label, UI_ZH_FINISH_TRAIN);
  lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_text_font(label, &upper_font_zh_12, 0);
  lv_obj_align(label, LV_ALIGN_CENTER, 14, 0);

  UpperArm_LvglPageReady(screen);
  if (training_emg_enable_timer != NULL)
  {
    lv_timer_del(training_emg_enable_timer);
  }
  training_emg_enable_timer = lv_timer_create(UpperArm_OnTrainingEmgEnable, 500U, NULL);
}

static void UpperArm_LvglCreateFreeTraining(void)
{
  lv_obj_t *screen = lv_scr_act();
  lv_obj_t *bg;
  lv_obj_t *title;
  lv_obj_t *status;
  lv_obj_t *card;
  lv_obj_t *label;
  lv_obj_t *finish_btn;
  lv_obj_t *btn_icon;
  static const char *action_texts[5] = {
    UI_ZH_ELBOW_FLEX, UI_ZH_ARM_ABD, UI_ZH_FOREARM_ROT, UI_ZH_SHOULDER_LIFT, UI_ZH_NO_ACTION
  };
  uint8_t i;

  lv_obj_clean(screen);
  UpperArm_LvglResetPageObjectRefs();
  lv_obj_set_style_bg_color(screen, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_bg_grad_color(screen, lv_color_hex(0xEEF6FF), 0);
  lv_obj_set_style_bg_grad_dir(screen, LV_GRAD_DIR_VER, 0);
  lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

  current_ui_page = UPPER_PAGE_FREE_TRAINING;
  esp_upload_pause_until_tick = osKernelGetTickCount() + ESP8266_UI_TRANSITION_PAUSE_MS;
  training_paused = 0U;
  training_start_tick = osKernelGetTickCount();
  training_pause_start_tick = 0U;
  training_paused_accum_ms = 0U;
  free_total_reps = 0U;
  free_current_angle = 0U;
  free_max_angle = 0U;
  free_current_action_index = 4U;
  free_last_counted_action = REHAB_ACTION_IDLE;
  training_eval_reset_request = 1U;
  memset(free_action_counts, 0, sizeof(free_action_counts));
  memset(free_action_count_labels, 0, sizeof(free_action_count_labels));
  wireless_train_start_pending = 1U;
  wireless_train_finish_pending = 0U;

  bg = lv_img_create(screen);
  lv_img_set_src(bg, UpperArm_GetPrepBg());
  lv_obj_set_size(bg, 240, 320);
  lv_obj_align(bg, LV_ALIGN_CENTER, 0, 0);
  lv_obj_clear_flag(bg, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_move_background(bg);

  title = lv_label_create(screen);
  lv_label_set_text(title, UI_ZH_FREE_TRAIN);
  lv_obj_set_style_text_color(title, lv_color_hex(0x102B6B), 0);
  lv_obj_set_style_text_font(title, &upper_font_zh_16, 0);
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 12);

  status = lv_obj_create(screen);
  lv_obj_remove_style_all(status);
  lv_obj_set_size(status, 86, 18);
  lv_obj_align(status, LV_ALIGN_TOP_MID, 0, 34);
  lv_obj_set_style_radius(status, 9, 0);
  lv_obj_set_style_bg_color(status, lv_color_hex(0xE8FFF0), 0);
  lv_obj_set_style_bg_opa(status, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(status, 1, 0);
  lv_obj_set_style_border_color(status, lv_color_hex(0x27C75F), 0);

  label = lv_label_create(status);
  lv_label_set_text(label, UI_ZH_RUNNING);
  lv_obj_set_style_text_color(label, lv_color_hex(0x21B653), 0);
  lv_obj_set_style_text_font(label, &upper_font_zh_10, 0);
  lv_obj_center(label);

  card = lv_obj_create(screen);
  lv_obj_remove_style_all(card);
  lv_obj_set_size(card, 216, 118);
  lv_obj_set_pos(card, 12, 59);
  lv_obj_set_style_radius(card, 10, 0);
  lv_obj_set_style_bg_color(card, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(card, 1, 0);
  lv_obj_set_style_border_color(card, lv_color_hex(0xDCEAFF), 0);
  lv_obj_set_style_shadow_width(card, 8, 0);
  lv_obj_set_style_shadow_color(card, lv_color_hex(0xC7DDF8), 0);
  lv_obj_set_style_shadow_opa(card, LV_OPA_20, 0);
  lv_obj_set_style_shadow_ofs_y(card, 3, 0);

  label = lv_label_create(card);
  lv_label_set_text(label, UI_ZH_DETECTED_ACTION);
  lv_obj_set_style_text_color(label, lv_color_hex(0x5D7098), 0);
  lv_obj_set_style_text_font(label, &upper_font_zh_12, 0);
  lv_obj_set_pos(label, 14, 10);

  free_action_label = lv_label_create(card);
  lv_label_set_text(free_action_label, action_texts[4]);
  lv_obj_set_width(free_action_label, 104);
  lv_label_set_long_mode(free_action_label, LV_LABEL_LONG_DOT);
  lv_obj_set_style_text_color(free_action_label, lv_color_hex(0x126BFF), 0);
  lv_obj_set_style_text_font(free_action_label, &upper_font_zh_16, 0);
  lv_obj_set_pos(free_action_label, 14, 30);

  label = lv_label_create(card);
  lv_label_set_text(label, UI_ZH_TOTAL_COUNT);
  lv_obj_set_style_text_color(label, lv_color_hex(0x5D7098), 0);
  lv_obj_set_style_text_font(label, &upper_font_zh_12, 0);
  lv_obj_set_pos(label, 132, 10);

  free_count_label = lv_label_create(card);
  lv_label_set_text(free_count_label, "0");
  lv_obj_set_style_text_color(free_count_label, lv_color_hex(0x126BFF), 0);
  lv_obj_set_style_text_font(free_count_label, &lv_font_montserrat_28, 0);
  lv_obj_set_pos(free_count_label, 132, 28);

  label = lv_label_create(card);
  lv_label_set_text(label, UI_ZH_CURRENT_ANGLE);
  lv_obj_set_style_text_color(label, lv_color_hex(0x5D7098), 0);
  lv_obj_set_style_text_font(label, &upper_font_zh_12, 0);
  lv_obj_set_pos(label, 14, 76);

  free_angle_label = lv_label_create(card);
  lv_label_set_text(free_angle_label, "0°");
  lv_obj_set_style_text_color(free_angle_label, lv_color_hex(0x102B6B), 0);
  lv_obj_set_style_text_font(free_angle_label, &lv_font_montserrat_28, 0);
  lv_obj_set_pos(free_angle_label, 132, 70);

  for (i = 0U; i < 4U; i++)
  {
    lv_obj_t *row = lv_obj_create(screen);
    lv_obj_remove_style_all(row);
    lv_obj_set_size(row, 104, 40);
    lv_obj_set_pos(row, (i % 2U) ? 124 : 12, (i < 2U) ? 184 : 228);
    lv_obj_set_style_radius(row, 8, 0);
    lv_obj_set_style_bg_color(row, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(row, LV_OPA_90, 0);
    lv_obj_set_style_border_width(row, 1, 0);
    lv_obj_set_style_border_color(row, lv_color_hex(0xDCEAFF), 0);

    label = lv_label_create(row);
    lv_label_set_text(label, action_texts[i]);
    lv_obj_set_style_text_color(label, lv_color_hex(0x102B6B), 0);
    lv_obj_set_style_text_font(label, &upper_font_zh_10, 0);
    lv_obj_set_pos(label, 7, 5);

    free_action_count_labels[i] = lv_label_create(row);
    lv_label_set_text(free_action_count_labels[i], "0");
    lv_obj_set_style_text_color(free_action_count_labels[i], lv_color_hex(0x126BFF), 0);
    lv_obj_set_style_text_font(free_action_count_labels[i], &lv_font_montserrat_16, 0);
    lv_obj_align(free_action_count_labels[i], LV_ALIGN_BOTTOM_RIGHT, -9, -3);
  }

  finish_btn = lv_btn_create(screen);
  lv_obj_remove_style_all(finish_btn);
  lv_obj_set_size(finish_btn, 216, 36);
  lv_obj_set_pos(finish_btn, 12, 276);
  lv_obj_set_style_radius(finish_btn, 10, 0);
  lv_obj_set_style_bg_color(finish_btn, lv_color_hex(0xE74C3C), 0);
  lv_obj_set_style_bg_opa(finish_btn, LV_OPA_COVER, 0);
  lv_obj_clear_flag(finish_btn, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_event_cb(finish_btn, UpperArm_OnFinishTrainingClicked, LV_EVENT_CLICKED, NULL);

  btn_icon = lv_label_create(finish_btn);
  lv_label_set_text(btn_icon, LV_SYMBOL_STOP);
  lv_obj_set_style_text_color(btn_icon, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_text_font(btn_icon, &lv_font_montserrat_14, 0);
  lv_obj_align(btn_icon, LV_ALIGN_CENTER, -38, 0);

  label = lv_label_create(finish_btn);
  lv_label_set_text(label, UI_ZH_FINISH_TRAIN);
  lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_text_font(label, &upper_font_zh_14, 0);
  lv_obj_align(label, LV_ALIGN_CENTER, 12, 0);

  UpperArm_LvglPageReady(screen);
}

static void UpperArm_LvglCreateFreeResult(void)
{
  lv_obj_t *screen = lv_scr_act();
  lv_obj_t *title;
  lv_obj_t *label;
  lv_obj_t *btn_home;
  static const char *action_texts[4] = {
    UI_ZH_ELBOW_FLEX, UI_ZH_ARM_ABD, UI_ZH_FOREARM_ROT, UI_ZH_SHOULDER_LIFT
  };
  uint8_t i;

  lv_obj_clean(screen);
  UpperArm_LvglResetPageObjectRefs();
  lv_obj_set_style_bg_color(screen, lv_color_hex(0xF3F8FF), 0);
  lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
  current_ui_page = UPPER_PAGE_FREE_RESULT;

  title = lv_label_create(screen);
  lv_label_set_text(title, UI_ZH_TRAIN_DONE);
  lv_obj_set_style_text_color(title, lv_color_hex(0x102B6B), 0);
  lv_obj_set_style_text_font(title, &upper_font_zh_16, 0);
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 24);

  label = lv_label_create(screen);
  lv_label_set_text_fmt(label, "%s %u%s", UI_ZH_TOTAL_COUNT, (unsigned int)free_total_reps, UI_ZH_COUNT_UNIT);
  lv_obj_set_style_text_color(label, lv_color_hex(0x126BFF), 0);
  lv_obj_set_style_text_font(label, &upper_font_zh_16, 0);
  lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 58);

  for (i = 0U; i < 4U; i++)
  {
    lv_obj_t *row = lv_obj_create(screen);
    lv_obj_remove_style_all(row);
    lv_obj_set_size(row, 204, 34);
    lv_obj_set_pos(row, 18, (lv_coord_t)(98 + i * 42));
    lv_obj_set_style_radius(row, 8, 0);
    lv_obj_set_style_bg_color(row, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(row, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(row, 1, 0);
    lv_obj_set_style_border_color(row, lv_color_hex(0xDCEAFF), 0);

    label = lv_label_create(row);
    lv_label_set_text(label, action_texts[i]);
    lv_obj_set_style_text_color(label, lv_color_hex(0x102B6B), 0);
    lv_obj_set_style_text_font(label, &upper_font_zh_12, 0);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 12, 0);

    label = lv_label_create(row);
    lv_label_set_text_fmt(label, "%u%s", (unsigned int)free_action_counts[i], UI_ZH_COUNT_UNIT);
    lv_obj_set_style_text_color(label, lv_color_hex(0x126BFF), 0);
    lv_obj_set_style_text_font(label, &upper_font_zh_14, 0);
    lv_obj_align(label, LV_ALIGN_RIGHT_MID, -12, 0);
  }

  btn_home = lv_btn_create(screen);
  lv_obj_remove_style_all(btn_home);
  lv_obj_set_size(btn_home, 180, 38);
  lv_obj_align(btn_home, LV_ALIGN_BOTTOM_MID, 0, -20);
  lv_obj_set_style_radius(btn_home, 10, 0);
  lv_obj_set_style_bg_color(btn_home, lv_color_hex(0x2F80ED), 0);
  lv_obj_set_style_bg_opa(btn_home, LV_OPA_COVER, 0);
  lv_obj_add_event_cb(btn_home, UpperArm_OnBackHomeClicked, LV_EVENT_CLICKED, NULL);

  label = lv_label_create(btn_home);
  lv_label_set_text(label, UI_ZH_BACK_HOME);
  lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_text_font(label, &upper_font_zh_14, 0);
  lv_obj_center(label);

  UpperArm_LvglPageReady(screen);
}

static void UpperArm_LvglCreateTrainingLegacy(void)
{
  lv_obj_t *screen = lv_scr_act();
  lv_obj_t *title;
  lv_obj_t *action_label;
  lv_obj_t *score_card;
  lv_obj_t *score_title;
  lv_obj_t *score_value;
  lv_obj_t *gauge;
  lv_obj_t *gauge_bg;
  lv_obj_t *gauge_core;
  lv_obj_t *angle_label;
  lv_obj_t *progress_title;
  lv_obj_t *stability_title;
  lv_obj_t *finish_btn;
  lv_obj_t *finish_label;
  lv_obj_t *chart;
  lv_chart_series_t *series;
  lv_obj_t *panel;
  lv_obj_t *bar;
  lv_obj_t *bar_fill;
  lv_obj_t *bar_fill2;
  lv_obj_t *bar_value;
  lv_obj_t *bar_value2;
  lv_coord_t chart_x;
  static const char *action_texts[4] = {
    UI_ZH_ELBOW_FLEX, UI_ZH_ARM_ABD, UI_ZH_FOREARM_ROT, UI_ZH_SHOULDER_LIFT
  };
  static const lv_coord_t chart_points[10] = {72, 88, 95, 102, 108, 96, 101, 106, 104, 108};
  uint8_t i;

  lv_obj_clean(screen);
  UpperArm_LvglResetPageObjectRefs();
  lv_obj_set_style_bg_color(screen, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_bg_grad_color(screen, lv_color_hex(0xEEF6FF), 0);
  lv_obj_set_style_bg_grad_dir(screen, LV_GRAD_DIR_VER, 0);
  lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

  current_ui_page = UPPER_PAGE_TRAINING;
  EmgSensor_SetModelEnabled(1U);
  home_status_value_label = NULL;
  home_status_dot = NULL;
  home_bt_icon = NULL;

  title = lv_label_create(screen);
  lv_label_set_text(title, "Running");
  lv_obj_set_style_text_color(title, lv_color_hex(0x27AE60), 0);
  lv_obj_set_style_text_font(title, &lv_font_montserrat_14, 0);
  lv_obj_align(title, LV_ALIGN_TOP_LEFT, 14, 12);

  training_time_label = NULL;
  home_time_label = NULL;

  action_label = lv_label_create(screen);
  lv_label_set_text(action_label, action_texts[selected_action_index]);
  lv_obj_set_style_text_color(action_label, lv_color_hex(0x1A4FA3), 0);
  lv_obj_set_style_text_font(action_label, &upper_font_zh_16, 0);
  lv_obj_align(action_label, LV_ALIGN_TOP_LEFT, 14, 38);

  score_card = lv_obj_create(screen);
  lv_obj_remove_style_all(score_card);
  lv_obj_set_size(score_card, 74, 76);
  lv_obj_set_pos(score_card, 154, 56);
  lv_obj_set_style_radius(score_card, 18, 0);
  lv_obj_set_style_bg_color(score_card, lv_color_hex(0xF5FAFF), 0);
  lv_obj_set_style_bg_opa(score_card, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(score_card, 1, 0);
  lv_obj_set_style_border_color(score_card, lv_color_hex(0xDCEAFF), 0);
  lv_obj_set_style_shadow_width(score_card, 8, 0);
  lv_obj_set_style_shadow_opa(score_card, LV_OPA_10, 0);

  score_title = lv_label_create(score_card);
  lv_label_set_text(score_title, UI_ZH_SCORE);
  lv_obj_set_style_text_color(score_title, lv_color_hex(0x4E79C6), 0);
  lv_obj_set_style_text_font(score_title, &upper_font_zh_14, 0);
  lv_obj_align(score_title, LV_ALIGN_TOP_MID, 0, 8);

  score_value = lv_label_create(score_card);
  lv_label_set_text(score_value, "85");
  lv_obj_set_style_text_color(score_value, lv_color_hex(0x2F80ED), 0);
  lv_obj_set_style_text_font(score_value, &lv_font_montserrat_28, 0);
  lv_obj_align(score_value, LV_ALIGN_CENTER, -4, 6);

  title = lv_label_create(score_card);
  lv_label_set_text(title, UI_ZH_SCORE_UNIT);
  lv_obj_set_style_text_color(title, lv_color_hex(0x2F80ED), 0);
  lv_obj_set_style_text_font(title, &upper_font_zh_14, 0);
  lv_obj_align_to(title, score_value, LV_ALIGN_OUT_RIGHT_BOTTOM, 2, -2);

  gauge_bg = lv_arc_create(screen);
  lv_obj_set_size(gauge_bg, 138, 138);
  lv_obj_set_pos(gauge_bg, 18, 72);
  lv_arc_set_rotation(gauge_bg, 135);
  lv_arc_set_bg_angles(gauge_bg, 0, 270);
  lv_arc_set_range(gauge_bg, 0, 180);
  lv_arc_set_value(gauge_bg, 180);
  lv_obj_remove_style(gauge_bg, NULL, LV_PART_KNOB);
  lv_obj_clear_flag(gauge_bg, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_set_style_arc_width(gauge_bg, 12, LV_PART_MAIN);
  lv_obj_set_style_arc_color(gauge_bg, lv_color_hex(0xD9E8FF), LV_PART_MAIN);
  lv_obj_set_style_arc_opa(gauge_bg, LV_OPA_80, LV_PART_MAIN);
  lv_obj_set_style_arc_width(gauge_bg, 0, LV_PART_INDICATOR);

  gauge = lv_arc_create(screen);
  lv_obj_set_size(gauge, 138, 138);
  lv_obj_set_pos(gauge, 18, 72);
  lv_arc_set_rotation(gauge, 135);
  lv_arc_set_bg_angles(gauge, 0, 270);
  lv_arc_set_range(gauge, 0, 180);
  lv_arc_set_value(gauge, 108);
  lv_obj_remove_style(gauge, NULL, LV_PART_KNOB);
  lv_obj_clear_flag(gauge, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_set_style_arc_width(gauge, 0, LV_PART_MAIN);
  lv_obj_set_style_arc_width(gauge, 12, LV_PART_INDICATOR);
  lv_obj_set_style_arc_color(gauge, lv_color_hex(0x2F80ED), LV_PART_INDICATOR);

  gauge_core = lv_obj_create(screen);
  lv_obj_remove_style_all(gauge_core);
  lv_obj_set_size(gauge_core, 104, 104);
  lv_obj_set_pos(gauge_core, 35, 89);
  lv_obj_set_style_radius(gauge_core, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(gauge_core, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_bg_opa(gauge_core, LV_OPA_90, 0);
  lv_obj_set_style_border_width(gauge_core, 2, 0);
  lv_obj_set_style_border_color(gauge_core, lv_color_hex(0xD8E8FF), 0);
  lv_obj_set_style_shadow_width(gauge_core, 14, 0);
  lv_obj_set_style_shadow_opa(gauge_core, LV_OPA_10, 0);

  angle_label = lv_label_create(gauge_core);
  lv_label_set_text(angle_label, "108°");
  lv_obj_set_style_text_color(angle_label, lv_color_hex(0x2F80ED), 0);
  lv_obj_set_style_text_font(angle_label, &lv_font_montserrat_28, 0);
  lv_obj_center(angle_label);

  progress_title = lv_label_create(screen);
  lv_label_set_text(progress_title, UI_ZH_COMPLETION);
  lv_obj_set_style_text_color(progress_title, lv_color_hex(0x102B6B), 0);
  lv_obj_set_style_text_font(progress_title, &upper_font_zh_14, 0);
  lv_obj_set_pos(progress_title, 18, 214);

  panel = lv_obj_create(screen);
  lv_obj_remove_style_all(panel);
  lv_obj_set_size(panel, 138, 10);
  lv_obj_set_pos(panel, 78, 220);
  lv_obj_set_style_radius(panel, 5, 0);
  lv_obj_set_style_bg_color(panel, lv_color_hex(0xE4EEFB), 0);
  lv_obj_set_style_bg_opa(panel, LV_OPA_COVER, 0);

  bar_fill = lv_obj_create(panel);
  lv_obj_remove_style_all(bar_fill);
  lv_obj_set_size(bar_fill, 114, 10);
  lv_obj_set_style_radius(bar_fill, 5, 0);
  lv_obj_set_style_bg_color(bar_fill, lv_color_hex(0x2F80ED), 0);
  lv_obj_set_style_bg_opa(bar_fill, LV_OPA_COVER, 0);

  bar_value = lv_label_create(screen);
  lv_label_set_text(bar_value, "82%");
  lv_obj_set_style_text_color(bar_value, lv_color_hex(0x2F80ED), 0);
  lv_obj_set_style_text_font(bar_value, &lv_font_montserrat_12, 0);
  lv_obj_set_pos(bar_value, 184, 211);

  stability_title = lv_label_create(screen);
  lv_label_set_text(stability_title, UI_ZH_STABILITY);
  lv_obj_set_style_text_color(stability_title, lv_color_hex(0x102B6B), 0);
  lv_obj_set_style_text_font(stability_title, &upper_font_zh_14, 0);
  lv_obj_set_pos(stability_title, 18, 236);

  panel = lv_obj_create(screen);
  lv_obj_remove_style_all(panel);
  lv_obj_set_size(panel, 138, 10);
  lv_obj_set_pos(panel, 78, 242);
  lv_obj_set_style_radius(panel, 5, 0);
  lv_obj_set_style_bg_color(panel, lv_color_hex(0xE4EEFB), 0);
  lv_obj_set_style_bg_opa(panel, LV_OPA_COVER, 0);

  bar_fill2 = lv_obj_create(panel);
  lv_obj_remove_style_all(bar_fill2);
  lv_obj_set_size(bar_fill2, 98, 10);
  lv_obj_set_style_radius(bar_fill2, 5, 0);
  lv_obj_set_style_bg_color(bar_fill2, lv_color_hex(0x27AE60), 0);
  lv_obj_set_style_bg_opa(bar_fill2, LV_OPA_COVER, 0);

  bar_value2 = lv_label_create(screen);
  lv_label_set_text(bar_value2, "71%");
  lv_obj_set_style_text_color(bar_value2, lv_color_hex(0x27AE60), 0);
  lv_obj_set_style_text_font(bar_value2, &lv_font_montserrat_12, 0);
  lv_obj_set_pos(bar_value2, 184, 233);

  chart = lv_chart_create(screen);
  lv_obj_remove_style_all(chart);
  lv_obj_set_size(chart, 212, 40);
  lv_obj_set_pos(chart, 14, 258);
  lv_obj_set_style_radius(chart, 14, 0);
  lv_obj_set_style_bg_color(chart, lv_color_hex(0xF6FAFF), 0);
  lv_obj_set_style_bg_opa(chart, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(chart, 1, 0);
  lv_obj_set_style_border_color(chart, lv_color_hex(0xD9E8FF), 0);
  lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
  lv_chart_set_point_count(chart, 10);
  lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 60, 120);
  lv_chart_set_div_line_count(chart, 0, 0);
  lv_obj_set_style_size(chart, 0, LV_PART_INDICATOR);
  lv_obj_set_style_line_width(chart, 3, LV_PART_ITEMS);
  lv_obj_set_style_line_color(chart, lv_color_hex(0x2F80ED), LV_PART_ITEMS);
  series = lv_chart_add_series(chart, lv_color_hex(0x2F80ED), LV_CHART_AXIS_PRIMARY_Y);
  for (i = 0U; i < 10U; i++)
  {
    series->y_points[i] = chart_points[i];
  }
  lv_chart_refresh(chart);

  finish_btn = lv_btn_create(screen);
  lv_obj_remove_style_all(finish_btn);
  lv_obj_set_size(finish_btn, 212, 18);
  lv_obj_set_pos(finish_btn, 14, 300);
  lv_obj_set_style_radius(finish_btn, 10, 0);
  lv_obj_set_style_bg_color(finish_btn, lv_color_hex(0xE74C3C), 0);
  lv_obj_set_style_bg_grad_color(finish_btn, lv_color_hex(0xD63031), 0);
  lv_obj_set_style_bg_grad_dir(finish_btn, LV_GRAD_DIR_VER, 0);
  lv_obj_set_style_bg_opa(finish_btn, LV_OPA_COVER, 0);
  lv_obj_set_style_shadow_width(finish_btn, 6, 0);
  lv_obj_set_style_shadow_opa(finish_btn, LV_OPA_20, 0);
  lv_obj_clear_flag(finish_btn, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_event_cb(finish_btn, UpperArm_OnFinishTrainingClicked, LV_EVENT_CLICKED, NULL);

  finish_label = lv_label_create(finish_btn);
  lv_label_set_text(finish_label, UI_ZH_FINISH_TRAIN);
  lv_obj_set_style_text_color(finish_label, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_text_font(finish_label, &upper_font_zh_14, 0);
  lv_obj_center(finish_label);
}

static void UpperArm_LvglCreateResult(void)
{
  lv_obj_t *screen = lv_scr_act();
  lv_obj_t *bg;
  lv_obj_t *logo;
  lv_obj_t *logo_text;
  lv_obj_t *brand;
  lv_obj_t *brand_sub;
  lv_obj_t *bat;
  lv_obj_t *bat_tip;
  lv_obj_t *bat_fill;
  lv_obj_t *bat_value;
  lv_obj_t *time_label;
  lv_obj_t *divider;
  lv_obj_t *ok_circle;
  lv_obj_t *ok_a;
  lv_obj_t *ok_b;
  lv_obj_t *title;
  lv_obj_t *subtitle;
  lv_obj_t *score_label;
  lv_obj_t *score_unit;
  lv_obj_t *panel;
  lv_obj_t *sep_h;
  lv_obj_t *sep_v;
  lv_obj_t *label;
  lv_obj_t *value;
  lv_obj_t *bar;
  lv_obj_t *metric_icon;
  lv_obj_t *btn_retry;
  lv_obj_t *btn_home;
  lv_obj_t *btn_icon;
  TrainingEvalSnapshot_t eval;
  uint16_t target_reps;
  uint8_t completion_percent;
  uint16_t max_angle_deg;
  static lv_point_t check_a[] = {{8, 16}, {16, 24}};
  static lv_point_t check_b[] = {{16, 24}, {30, 10}};
  static const char *metric_titles[4] = {UI_ZH_COMPLETION, UI_ZH_AMPLITUDE, UI_ZH_TRAIN_COUNT, UI_ZH_MAX_ANGLE};
  char completion_text[8];
  char amplitude_text[8];
  char train_count_text[12];
  char max_angle_text[10];
  const char *metric_values[4];
  static const lv_coord_t metric_x[4] = {14, 121, 14, 121};
  static const lv_coord_t metric_y[4] = {181, 181, 222, 222};
  uint8_t i;

  taskENTER_CRITICAL();
  eval = training_eval_snapshot;
  taskEXIT_CRITICAL();
  UpperArm_ApplyScriptedTrainingEval(&eval, osKernelGetTickCount());

  target_reps = (eval.target_reps != 0U) ? eval.target_reps : training_target_count;
  if (target_reps == 0U)
  {
    completion_percent = 0U;
  }
  else if (eval.reps >= target_reps)
  {
    completion_percent = 100U;
  }
  else
  {
    completion_percent = (uint8_t)(((uint32_t)eval.reps * 100U) / (uint32_t)target_reps);
  }
  max_angle_deg = (uint16_t)(eval.max_angle_x100 / 100);

  snprintf(completion_text, sizeof(completion_text), "%u%%", (unsigned int)completion_percent);
  snprintf(amplitude_text, sizeof(amplitude_text), "%u%%", (unsigned int)eval.amplitude_score);
  snprintf(train_count_text, sizeof(train_count_text), "%u/%u%s",
           (unsigned int)eval.reps,
           (unsigned int)target_reps,
           UI_ZH_COUNT_UNIT);
  snprintf(max_angle_text, sizeof(max_angle_text), "%u°", (unsigned int)max_angle_deg);
  metric_values[0] = completion_text;
  metric_values[1] = amplitude_text;
  metric_values[2] = train_count_text;
  metric_values[3] = max_angle_text;

  lv_obj_clean(screen);
  UpperArm_LvglResetPageObjectRefs();
  lv_obj_set_style_bg_color(screen, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_bg_grad_color(screen, lv_color_hex(0xEEF6FF), 0);
  lv_obj_set_style_bg_grad_dir(screen, LV_GRAD_DIR_VER, 0);
  lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

  current_ui_page = UPPER_PAGE_RESULT;
  upper_page_result_event_frames = 10U;
  upper_page_home_event_frames = 0U;
  home_status_value_label = NULL;
  home_status_dot = NULL;
  home_bt_icon = NULL;

  bg = lv_img_create(screen);
  lv_img_set_src(bg, UpperArm_GetPrepBg());
  lv_obj_set_size(bg, 240, 320);
  lv_obj_align(bg, LV_ALIGN_CENTER, 0, 0);
  lv_obj_clear_flag(bg, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_move_background(bg);

  logo = lv_obj_create(screen);
  lv_obj_remove_style_all(logo);
  lv_obj_set_size(logo, 22, 22);
  lv_obj_set_pos(logo, 10, 6);
  lv_obj_set_style_radius(logo, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(logo, lv_color_hex(0x1F74F2), 0);
  lv_obj_set_style_bg_opa(logo, LV_OPA_COVER, 0);

  logo_text = lv_label_create(logo);
  lv_label_set_text(logo_text, UI_ZH_LOGO);
  lv_obj_set_style_text_color(logo_text, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_text_font(logo_text, &upper_font_zh_14, 0);
  lv_obj_center(logo_text);

  brand = lv_label_create(screen);
  lv_label_set_text(brand, UI_ZH_BRAND);
  lv_obj_set_style_text_color(brand, lv_color_hex(0x176BEF), 0);
  lv_obj_set_style_text_font(brand, &upper_font_zh_14, 0);
  lv_obj_set_pos(brand, 38, 5);

  brand_sub = lv_label_create(screen);
  lv_label_set_text(brand_sub, UI_ZH_SYSTEM);
  lv_obj_set_style_text_color(brand_sub, lv_color_hex(0x102B6B), 0);
  lv_obj_set_style_text_font(brand_sub, &upper_font_zh_10, 0);
  lv_obj_set_pos(brand_sub, 38, 22);

  bat = lv_obj_create(screen);
  lv_obj_remove_style_all(bat);
  lv_obj_set_size(bat, 18, 10);
  lv_obj_set_pos(bat, 150, 10);
  lv_obj_set_style_radius(bat, 2, 0);
  lv_obj_set_style_border_width(bat, 1, 0);
  lv_obj_set_style_border_color(bat, lv_color_hex(0x102B6B), 0);

  bat_tip = lv_obj_create(screen);
  lv_obj_remove_style_all(bat_tip);
  lv_obj_set_size(bat_tip, 2, 6);
  lv_obj_align_to(bat_tip, bat, LV_ALIGN_OUT_RIGHT_MID, 1, 0);
  lv_obj_set_style_radius(bat_tip, 1, 0);
  lv_obj_set_style_bg_color(bat_tip, lv_color_hex(0x102B6B), 0);
  lv_obj_set_style_bg_opa(bat_tip, LV_OPA_COVER, 0);

  bat_fill = lv_obj_create(bat);
  lv_obj_remove_style_all(bat_fill);
  lv_obj_set_size(bat_fill, 12, 6);
  lv_obj_align(bat_fill, LV_ALIGN_LEFT_MID, 2, 0);
  lv_obj_set_style_radius(bat_fill, 1, 0);
  lv_obj_set_style_bg_color(bat_fill, lv_color_hex(0x28D85A), 0);
  lv_obj_set_style_bg_opa(bat_fill, LV_OPA_COVER, 0);

  bat_value = lv_label_create(screen);
  lv_label_set_text(bat_value, "--");
  lv_obj_set_style_text_color(bat_value, lv_color_hex(0x000000), 0);
  lv_obj_set_style_text_font(bat_value, &lv_font_montserrat_12, 0);
  lv_obj_set_pos(bat_value, 174, 8);
  UpperArm_LvglApplyBatteryDisplay(bat_fill, bat_value, 12, 0U);

  time_label = lv_label_create(screen);
  result_time_label = time_label;
  home_time_label = time_label;
  lv_label_set_text(time_label, "10:30");
  lv_obj_set_style_text_color(time_label, lv_color_hex(0x000000), 0);
  lv_obj_set_style_text_font(time_label, &lv_font_montserrat_12, 0);
  lv_obj_align(time_label, LV_ALIGN_TOP_RIGHT, -5, 8);

  divider = UpperArm_LvglShape(screen, 240, 1, 0, 38, lv_color_hex(0xD6E6FA), 0);
  lv_obj_clear_flag(divider, LV_OBJ_FLAG_CLICKABLE);

  ok_circle = lv_obj_create(screen);
  lv_obj_remove_style_all(ok_circle);
  lv_obj_set_size(ok_circle, 38, 38);
  lv_obj_align(ok_circle, LV_ALIGN_TOP_MID, 0, 48);
  lv_obj_set_style_radius(ok_circle, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(ok_circle, lv_color_hex(0x20C65A), 0);
  lv_obj_set_style_bg_opa(ok_circle, LV_OPA_COVER, 0);
  lv_obj_set_style_shadow_width(ok_circle, 14, 0);
  lv_obj_set_style_shadow_color(ok_circle, lv_color_hex(0x8CE0A8), 0);
  lv_obj_set_style_shadow_opa(ok_circle, LV_OPA_30, 0);

  ok_a = lv_line_create(ok_circle);
  lv_line_set_points(ok_a, check_a, 2);
  lv_obj_set_style_line_width(ok_a, 4, 0);
  lv_obj_set_style_line_color(ok_a, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_line_rounded(ok_a, true, 0);

  ok_b = lv_line_create(ok_circle);
  lv_line_set_points(ok_b, check_b, 2);
  lv_obj_set_style_line_width(ok_b, 4, 0);
  lv_obj_set_style_line_color(ok_b, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_line_rounded(ok_b, true, 0);

  title = lv_label_create(screen);
  lv_label_set_text(title, UI_ZH_TRAIN_DONE);
  lv_obj_set_style_text_color(title, lv_color_hex(0x123F95), 0);
  lv_obj_set_style_text_font(title, &upper_font_zh_16, 0);
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 88);

  subtitle = lv_label_create(screen);
  lv_label_set_text(subtitle, UI_ZH_SCORE);
  lv_obj_set_style_text_color(subtitle, lv_color_hex(0x3E66B4), 0);
  lv_obj_set_style_text_font(subtitle, &upper_font_zh_14, 0);
  lv_obj_align(subtitle, LV_ALIGN_TOP_MID, 0, 112);

  score_label = lv_label_create(screen);
  lv_label_set_text_fmt(score_label, "%u", (unsigned int)eval.total_score);
  lv_obj_set_style_text_color(score_label, lv_color_hex(0x126BFF), 0);
  lv_obj_set_style_text_font(score_label, &lv_font_montserrat_48, 0);
  lv_obj_align(score_label, LV_ALIGN_TOP_MID, -7, 122);

  score_unit = lv_label_create(screen);
  lv_label_set_text(score_unit, UI_ZH_SCORE_UNIT);
  lv_obj_set_style_text_color(score_unit, lv_color_hex(0x123F95), 0);
  lv_obj_set_style_text_font(score_unit, &upper_font_zh_14, 0);
  lv_obj_align_to(score_unit, score_label, LV_ALIGN_OUT_RIGHT_BOTTOM, 2, -8);

  panel = lv_obj_create(screen);
  lv_obj_remove_style_all(panel);
  lv_obj_set_size(panel, 214, 82);
  lv_obj_set_pos(panel, 13, 178);
  lv_obj_set_style_radius(panel, 9, 0);
  lv_obj_set_style_bg_color(panel, lv_color_hex(0xFBFDFF), 0);
  lv_obj_set_style_bg_opa(panel, LV_OPA_90, 0);
  lv_obj_set_style_border_width(panel, 1, 0);
  lv_obj_set_style_border_color(panel, lv_color_hex(0xE3EEFF), 0);
  lv_obj_set_style_shadow_width(panel, 3, 0);
  lv_obj_set_style_shadow_opa(panel, LV_OPA_10, 0);
  lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE);

  sep_h = UpperArm_LvglShape(panel, 194, 1, 10, 41, lv_color_hex(0xE0ECFF), 0);
  sep_v = UpperArm_LvglShape(panel, 1, 66, 107, 8, lv_color_hex(0xE0ECFF), 0);
  lv_obj_clear_flag(sep_h, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_clear_flag(sep_v, LV_OBJ_FLAG_CLICKABLE);

  for (i = 0U; i < 4U; i++)
  {
    metric_icon = UpperArm_LvglShape(panel, 18, 18, metric_x[i] + 1, metric_y[i] - 174,
                                     lv_color_hex(0xEAF3FF), LV_RADIUS_CIRCLE);
    lv_obj_clear_flag(metric_icon, LV_OBJ_FLAG_CLICKABLE);
    UpperArm_LvglCreateResultMetricIcon(metric_icon, i);

    label = lv_label_create(panel);
    lv_label_set_text(label, metric_titles[i]);
    lv_obj_set_style_text_color(label, lv_color_hex(0x102B6B), 0);
    lv_obj_set_style_text_font(label, &upper_font_zh_12, 0);
    lv_obj_set_pos(label, metric_x[i] + 29, metric_y[i] - 176);

    value = lv_label_create(panel);
    lv_label_set_text(value, metric_values[i]);
    lv_obj_set_style_text_color(value, lv_color_hex(0x126BFF), 0);
    lv_obj_set_style_text_font(value, (i >= 2U) ? &upper_font_zh_14 : &lv_font_montserrat_16, 0);
    lv_obj_set_pos(value, metric_x[i] + 29, metric_y[i] - ((i >= 2U) ? 160 : 156));

    bar = UpperArm_LvglShape(panel, 38, 2, metric_x[i] + 29, metric_y[i] - 139, lv_color_hex(0xDDEBFF), 1);
    lv_obj_clear_flag(bar, LV_OBJ_FLAG_CLICKABLE);
    if (i < 2U)
    {
      uint8_t percent = (i == 0U) ? completion_percent : eval.amplitude_score;
      lv_coord_t fill_width = (lv_coord_t)(((uint32_t)percent * 38U) / 100U);
      if ((percent > 0U) && (fill_width < 2))
      {
        fill_width = 2;
      }
      lv_obj_t *fill = UpperArm_LvglShape(panel, fill_width, 2,
                                          metric_x[i] + 29, metric_y[i] - 139,
                                          lv_color_hex(0x2F80ED), 1);
      lv_obj_clear_flag(fill, LV_OBJ_FLAG_CLICKABLE);
    }
  }

  label = lv_label_create(screen);
  lv_label_set_text(label, UI_ZH_ENCOURAGE);
  lv_obj_set_style_text_color(label, lv_color_hex(0x123F95), 0);
  lv_obj_set_style_text_font(label, &upper_font_zh_14, 0);
  lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 264);

  btn_retry = lv_btn_create(screen);
  lv_obj_remove_style_all(btn_retry);
  lv_obj_set_size(btn_retry, 104, 32);
  lv_obj_set_pos(btn_retry, 12, 284);
  lv_obj_set_style_radius(btn_retry, 8, 0);
  lv_obj_set_style_bg_color(btn_retry, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_bg_opa(btn_retry, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(btn_retry, 1, 0);
  lv_obj_set_style_border_color(btn_retry, lv_color_hex(0x2F80ED), 0);
  lv_obj_clear_flag(btn_retry, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_event_cb(btn_retry, UpperArm_OnRetrainClicked, LV_EVENT_CLICKED, NULL);

  btn_icon = lv_label_create(btn_retry);
  lv_label_set_text(btn_icon, LV_SYMBOL_REFRESH);
  lv_obj_set_style_text_color(btn_icon, lv_color_hex(0x2F80ED), 0);
  lv_obj_set_style_text_font(btn_icon, &lv_font_montserrat_16, 0);
  lv_obj_align(btn_icon, LV_ALIGN_CENTER, -28, 0);

  label = lv_label_create(btn_retry);
  lv_label_set_text(label, UI_ZH_RETRAIN);
  lv_obj_set_style_text_color(label, lv_color_hex(0x2F80ED), 0);
  lv_obj_set_style_text_font(label, &upper_font_zh_14, 0);
  lv_obj_align(label, LV_ALIGN_CENTER, 12, 0);

  btn_home = lv_btn_create(screen);
  lv_obj_remove_style_all(btn_home);
  lv_obj_set_size(btn_home, 108, 32);
  lv_obj_set_pos(btn_home, 120, 284);
  lv_obj_set_style_radius(btn_home, 8, 0);
  lv_obj_set_style_bg_color(btn_home, lv_color_hex(0x2F80ED), 0);
  lv_obj_set_style_bg_grad_color(btn_home, lv_color_hex(0x005DE8), 0);
  lv_obj_set_style_bg_grad_dir(btn_home, LV_GRAD_DIR_VER, 0);
  lv_obj_set_style_bg_opa(btn_home, LV_OPA_COVER, 0);
  lv_obj_clear_flag(btn_home, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_event_cb(btn_home, UpperArm_OnBackHomeClicked, LV_EVENT_CLICKED, NULL);

  btn_icon = lv_label_create(btn_home);
  lv_label_set_text(btn_icon, LV_SYMBOL_HOME);
  lv_obj_set_style_text_color(btn_icon, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_text_font(btn_icon, &lv_font_montserrat_16, 0);
  lv_obj_align(btn_icon, LV_ALIGN_CENTER, -30, 0);

  label = lv_label_create(btn_home);
  lv_label_set_text(label, UI_ZH_BACK_HOME);
  lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_text_font(label, &upper_font_zh_14, 0);
  lv_obj_align(label, LV_ALIGN_CENTER, 13, 0);

  UpperArm_LvglPageReady(screen);
}

static void UpperArm_LvglCreateResultLegacy(void)
{
  lv_obj_t *screen = lv_scr_act();
  lv_obj_t *title;
  lv_obj_t *sub_title;
  lv_obj_t *score_label;
  lv_obj_t *score_unit;
  lv_obj_t *panel;
  lv_obj_t *icon_circle;
  lv_obj_t *icon_check_a;
  lv_obj_t *icon_check_b;
  lv_obj_t *label;
  lv_obj_t *value;
  lv_obj_t *btn_retry;
  lv_obj_t *btn_home;
  static const char *card_titles[4] = {UI_ZH_COMPLETION, UI_ZH_AMPLITUDE, UI_ZH_TRAIN_COUNT, UI_ZH_MAX_ANGLE};
  static const char *card_values[4] = {"92%", "84%", "20 \xE6\xAC\xA1", "118°"};
  static const lv_coord_t card_x[4] = {16, 122, 16, 122};
  static const lv_coord_t card_y[4] = {174, 174, 240, 240};
  uint8_t i;

  lv_obj_clean(screen);
  UpperArm_LvglResetPageObjectRefs();
  lv_obj_set_style_bg_color(screen, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_bg_grad_color(screen, lv_color_hex(0xEEF6FF), 0);
  lv_obj_set_style_bg_grad_dir(screen, LV_GRAD_DIR_VER, 0);
  lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

  current_ui_page = UPPER_PAGE_RESULT;
  upper_page_result_event_frames = 10U;
  upper_page_home_event_frames = 0U;
  home_status_value_label = NULL;
  home_status_dot = NULL;
  home_bt_icon = NULL;

  title = lv_label_create(screen);
  lv_label_set_text(title, UI_ZH_BRAND);
  lv_obj_set_style_text_color(title, lv_color_hex(0x2F80ED), 0);
  lv_obj_set_style_text_font(title, &upper_font_zh_14, 0);
  lv_obj_align(title, LV_ALIGN_TOP_LEFT, 18, 10);

  sub_title = lv_label_create(screen);
  lv_label_set_text(sub_title, UI_ZH_SYSTEM);
  lv_obj_set_style_text_color(sub_title, lv_color_hex(0x102B6B), 0);
  lv_obj_set_style_text_font(sub_title, &upper_font_zh_14, 0);
  lv_obj_align(sub_title, LV_ALIGN_TOP_LEFT, 18, 34);

  result_time_label = lv_label_create(screen);
  home_time_label = result_time_label;
  lv_label_set_text(result_time_label, "10:30");
  lv_obj_set_style_text_color(result_time_label, lv_color_hex(0x102B6B), 0);
  lv_obj_set_style_text_font(result_time_label, &lv_font_montserrat_12, 0);
  lv_obj_align(result_time_label, LV_ALIGN_TOP_RIGHT, -12, 16);

  icon_circle = lv_obj_create(screen);
  lv_obj_remove_style_all(icon_circle);
  lv_obj_set_size(icon_circle, 58, 58);
  lv_obj_align(icon_circle, LV_ALIGN_TOP_MID, 0, 60);
  lv_obj_set_style_radius(icon_circle, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(icon_circle, lv_color_hex(0x21C45A), 0);
  lv_obj_set_style_bg_opa(icon_circle, LV_OPA_COVER, 0);
  lv_obj_set_style_shadow_width(icon_circle, 16, 0);
  lv_obj_set_style_shadow_color(icon_circle, lv_color_hex(0x8CE0A8), 0);
  lv_obj_set_style_shadow_opa(icon_circle, LV_OPA_20, 0);

  icon_check_a = lv_line_create(icon_circle);
  {
    static lv_point_t check_a[] = {{14, 30}, {24, 40}};
    lv_line_set_points(icon_check_a, check_a, 2);
    lv_obj_set_style_line_width(icon_check_a, 4, 0);
    lv_obj_set_style_line_color(icon_check_a, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_line_rounded(icon_check_a, true, 0);
  }

  icon_check_b = lv_line_create(icon_circle);
  {
    static lv_point_t check_b[] = {{24, 40}, {42, 20}};
    lv_line_set_points(icon_check_b, check_b, 2);
    lv_obj_set_style_line_width(icon_check_b, 4, 0);
    lv_obj_set_style_line_color(icon_check_b, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_line_rounded(icon_check_b, true, 0);
  }

  title = lv_label_create(screen);
  lv_label_set_text(title, UI_ZH_TRAIN_DONE);
  lv_obj_set_style_text_color(title, lv_color_hex(0x123F95), 0);
  lv_obj_set_style_text_font(title, &upper_font_zh_16, 0);
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 126);

  sub_title = lv_label_create(screen);
  lv_label_set_text(sub_title, UI_ZH_SCORE);
  lv_obj_set_style_text_color(sub_title, lv_color_hex(0x3E66B4), 0);
  lv_obj_set_style_text_font(sub_title, &upper_font_zh_14, 0);
  lv_obj_align(sub_title, LV_ALIGN_TOP_MID, 0, 154);

  score_label = lv_label_create(screen);
  lv_label_set_text(score_label, "88");
  lv_obj_set_style_text_color(score_label, lv_color_hex(0x2F80ED), 0);
  lv_obj_set_style_text_font(score_label, &lv_font_montserrat_28, 0);
  lv_obj_align(score_label, LV_ALIGN_TOP_MID, -8, 176);

  score_unit = lv_label_create(screen);
  lv_label_set_text(score_unit, UI_ZH_SCORE_UNIT);
  lv_obj_set_style_text_color(score_unit, lv_color_hex(0x123F95), 0);
  lv_obj_set_style_text_font(score_unit, &upper_font_zh_16, 0);
  lv_obj_align_to(score_unit, score_label, LV_ALIGN_OUT_RIGHT_BOTTOM, 4, -8);

  panel = lv_obj_create(screen);
  lv_obj_remove_style_all(panel);
  lv_obj_set_size(panel, 212, 116);
  lv_obj_align(panel, LV_ALIGN_TOP_MID, 0, 244);
  lv_obj_set_style_radius(panel, 18, 0);
  lv_obj_set_style_bg_color(panel, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_bg_opa(panel, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(panel, 1, 0);
  lv_obj_set_style_border_color(panel, lv_color_hex(0xD9E8FF), 0);
  lv_obj_set_style_shadow_width(panel, 10, 0);
  lv_obj_set_style_shadow_opa(panel, LV_OPA_10, 0);
  lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE);

  for (i = 0U; i < 4U; i++)
  {
    lv_obj_t *card = lv_obj_create(screen);
    lv_obj_remove_style_all(card);
    lv_obj_set_size(card, 100, 52);
    lv_obj_set_pos(card, card_x[i], card_y[i]);
    lv_obj_set_style_radius(card, 14, 0);
    lv_obj_set_style_bg_color(card, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_TRANSP, 0);
    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);

    label = lv_label_create(card);
    lv_label_set_text(label, card_titles[i]);
    lv_obj_set_style_text_color(label, lv_color_hex(0x102B6B), 0);
    lv_obj_set_style_text_font(label, &upper_font_zh_14, 0);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 8, 4);

    value = lv_label_create(card);
    lv_label_set_text(value, card_values[i]);
    lv_obj_set_style_text_color(value, lv_color_hex(0x2F80ED), 0);
    lv_obj_set_style_text_font(value, (i == 2U) ? &upper_font_zh_16 : &lv_font_montserrat_24, 0);
    lv_obj_align(value, LV_ALIGN_BOTTOM_LEFT, 8, -2);
  }

  label = lv_label_create(screen);
  lv_label_set_text(label, UI_ZH_ENCOURAGE);
  lv_obj_set_style_text_color(label, lv_color_hex(0x27AE60), 0);
  lv_obj_set_style_text_font(label, &upper_font_zh_14, 0);
  lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 366);

  btn_retry = lv_btn_create(screen);
  lv_obj_remove_style_all(btn_retry);
  lv_obj_set_size(btn_retry, 100, 40);
  lv_obj_set_pos(btn_retry, 14, 402);
  lv_obj_set_style_radius(btn_retry, 16, 0);
  lv_obj_set_style_bg_color(btn_retry, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_bg_opa(btn_retry, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(btn_retry, 2, 0);
  lv_obj_set_style_border_color(btn_retry, lv_color_hex(0x2F80ED), 0);
  lv_obj_set_style_shadow_width(btn_retry, 8, 0);
  lv_obj_set_style_shadow_opa(btn_retry, LV_OPA_10, 0);
  lv_obj_clear_flag(btn_retry, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_event_cb(btn_retry, UpperArm_OnRetrainClicked, LV_EVENT_CLICKED, NULL);

  label = lv_label_create(btn_retry);
  lv_label_set_text(label, UI_ZH_RETRAIN);
  lv_obj_set_style_text_color(label, lv_color_hex(0x2F80ED), 0);
  lv_obj_set_style_text_font(label, &upper_font_zh_14, 0);
  lv_obj_center(label);

  btn_home = lv_btn_create(screen);
  lv_obj_remove_style_all(btn_home);
  lv_obj_set_size(btn_home, 112, 40);
  lv_obj_set_pos(btn_home, 114, 402);
  lv_obj_set_style_radius(btn_home, 16, 0);
  lv_obj_set_style_bg_color(btn_home, lv_color_hex(0x2F80ED), 0);
  lv_obj_set_style_bg_grad_color(btn_home, lv_color_hex(0x005DE8), 0);
  lv_obj_set_style_bg_grad_dir(btn_home, LV_GRAD_DIR_VER, 0);
  lv_obj_set_style_bg_opa(btn_home, LV_OPA_COVER, 0);
  lv_obj_set_style_shadow_width(btn_home, 8, 0);
  lv_obj_set_style_shadow_opa(btn_home, LV_OPA_20, 0);
  lv_obj_clear_flag(btn_home, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_event_cb(btn_home, UpperArm_OnBackHomeClicked, LV_EVENT_CLICKED, NULL);

  label = lv_label_create(btn_home);
  lv_label_set_text(label, UI_ZH_BACK_HOME);
  lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_text_font(label, &upper_font_zh_14, 0);
  lv_obj_center(label);
}

void UpperArm_LvglUpdateHome(uint32_t now)
{
  uint8_t clock_hour = 0U;
  uint8_t clock_minute = 0U;
  uint8_t clock_second = 0U;
  uint8_t connected;
  WirelessWristFrame_t wrist;
  uint8_t wrist_valid;
  uint32_t wrist_tick;
  static uint8_t last_connected;
  static uint32_t last_battery_adc_update;
  static lv_obj_t *last_battery_label;
  lv_color_t bt_color;

  taskENTER_CRITICAL();
  wrist = latest_wrist_frame;
  wrist_valid = latest_wrist_valid;
  wrist_tick = wireless_last_frame_tick;
  taskEXIT_CRITICAL();

  if ((home_time_label != NULL) &&
      (current_ui_page != UPPER_PAGE_TRAINING) &&
      (current_ui_page != UPPER_PAGE_FREE_TRAINING))
  {
    (void)UpperArm_GetClock(&clock_hour, &clock_minute, &clock_second);
    lv_label_set_text_fmt(home_time_label, "%02u:%02u",
                          (unsigned int)clock_hour,
                          (unsigned int)clock_minute);
  }
  if (home_today_count_label != NULL)
  {
    lv_label_set_text_fmt(home_today_count_label, "%u", (unsigned int)training_completed_count);
  }
  if (home_imu_angle_label != NULL)
  {
    IMUProc_Euler_t euler;

    if (UpperArm_GetLocalImuEuler(&euler) != 0U)
    {
      IMUProc_EulerX10_t euler_x10;
      int32_t roll_abs;
      int32_t pitch_abs;
      int32_t yaw_abs;

      IMUProc_EulerToSignedX10(&euler, IMU_ANGLE_DEADBAND_X10, &euler_x10);
      roll_abs = Rehab_AbsI32(euler_x10.roll_x10);
      pitch_abs = Rehab_AbsI32(euler_x10.pitch_x10);
      yaw_abs = Rehab_AbsI32(euler_x10.yaw_x10);
      lv_label_set_text_fmt(home_imu_angle_label,
                            "R %c%ld.%ld  P %c%ld.%ld  Y %c%ld.%ld",
                            Rehab_SignCharI32(euler_x10.roll_x10),
                            (long)(roll_abs / 10),
                            (long)(roll_abs % 10),
                            Rehab_SignCharI32(euler_x10.pitch_x10),
                            (long)(pitch_abs / 10),
                            (long)(pitch_abs % 10),
                            Rehab_SignCharI32(euler_x10.yaw_x10),
                            (long)(yaw_abs / 10),
                            (long)(yaw_abs % 10));
    }
    else
    {
      lv_label_set_text(home_imu_angle_label, "R --.-  P --.-  Y --.-");
    }
  }
  if ((home_battery_value_label != NULL) &&
      ((last_battery_label != home_battery_value_label) ||
       (last_battery_adc_update == 0U) ||
       ((now - last_battery_adc_update) >= UPPER_BATTERY_ADC_PERIOD_MS)))
  {
    last_battery_label = home_battery_value_label;
    if ((last_battery_adc_update == 0U) ||
        ((now - last_battery_adc_update) >= UPPER_BATTERY_ADC_PERIOD_MS))
    {
      last_battery_adc_update = now;
      UpperArm_BatteryUpdateFromMeasured(Battery_ReadPercent());
    }
    UpperArm_LvglApplyBatteryDisplay(home_battery_fill, home_battery_value_label, 13, 1U);
  }
  if (training_count_label != NULL)
  {
    TrainingEvalSnapshot_t eval;
    EspTrainingPayloadSnapshot_t emg_snap;
    EmgSensor_Features_t emg_diag;
    uint16_t target_reps;
    uint8_t completion_percent;

    taskENTER_CRITICAL();
    eval = training_eval_snapshot;
    emg_snap = esp_training_payload_snapshot;
    taskEXIT_CRITICAL();
    EmgSensor_GetFeatures(&emg_diag);
    UpperArm_ApplyScriptedTrainingEval(&eval, now);
    target_reps = (eval.target_reps != 0U) ? eval.target_reps : training_target_count;

    lv_label_set_text_fmt(training_count_label, "%u/%u",
                          (unsigned int)eval.reps,
                          (unsigned int)target_reps);

    if (target_reps == 0U)
    {
      completion_percent = 0U;
    }
    else if (eval.reps >= target_reps)
    {
      completion_percent = 100U;
    }
    else
    {
      completion_percent = (uint8_t)(((uint32_t)eval.reps * 100U) / (uint32_t)target_reps);
    }

    if (training_score_value_label != NULL)
    {
      lv_label_set_text_fmt(training_score_value_label, "%u", (unsigned int)eval.reps);
    }
    if (training_emg_mode_label != NULL)
    {
      const char *mode_text = "放松";
      lv_color_t mode_color = lv_color_hex(0x27AE60);
      if (emg_diag.running == 0U)
      {
        mode_text = "STOP";
        mode_color = lv_color_hex(0xE74C3C);
      }
      else if (emg_diag.calibrated == 0U)
      {
        mode_text = "CAL";
        mode_color = lv_color_hex(0x2F80ED);
      }
      else if (emg_snap.emg_mode == 1U)
      {
        mode_text = "较弱";
        mode_color = lv_color_hex(0xF59E0B);
      }
      else if (emg_snap.emg_mode == 2U)
      {
        mode_text = "用力";
        mode_color = lv_color_hex(0xE74C3C);
      }
      lv_label_set_text(training_emg_mode_label, mode_text);
      lv_obj_set_style_text_color(training_emg_mode_label, mode_color, 0);
    }
    if (training_emg_strength_label != NULL)
    {
      lv_label_set_text_fmt(training_emg_strength_label, "%u", (unsigned int)emg_snap.emg_strength);
    }
    if (training_completion_bar != NULL)
    {
      lv_coord_t fill_width = (lv_coord_t)(((uint32_t)completion_percent * 72U) / 100U);
      if ((completion_percent > 0U) && (fill_width < 3))
      {
        fill_width = 3;
      }
      lv_obj_set_width(training_completion_bar, fill_width);
    }
    if (training_completion_value_label != NULL)
    {
      lv_label_set_text_fmt(training_completion_value_label, "%u%%", (unsigned int)completion_percent);
    }
    if (training_angle_value_label != NULL)
    {
      int32_t angle_x100 = eval.current_angle_x100;
      int32_t display_angle = 0;
      uint8_t show_signed_angle = 0U;
      RehabFrame_t rehab;
      uint8_t rehab_valid;

      taskENTER_CRITICAL();
      rehab = latest_rehab_frame;
      rehab_valid = latest_rehab_valid;
      taskEXIT_CRITICAL();

      if (rehab_valid != 0U)
      {
        if (selected_action_index == 1U)
        {
          angle_x100 = rehab.shoulder_lift_angle_x100;
        }
        else if (selected_action_index == 2U)
        {
          angle_x100 = rehab.forearm_rotation_angle_x100;
        }
        else if (selected_action_index == 3U)
        {
          angle_x100 = rehab.arm_abduction_angle_x100;
        }
        else
        {
          angle_x100 = rehab.elbow_angle_x100;
        }
      }

      if ((show_signed_angle == 0U) && (angle_x100 < 0))
      {
        angle_x100 = -angle_x100;
      }
      display_angle = angle_x100 / 100;
      lv_label_set_text_fmt(training_angle_value_label, "%ld°",
                            (long)display_angle);
#if UPPER_TRAINING_REFRESH_ANGLE_CHART
      if ((training_angle_chart != NULL) &&
          (training_angle_series != NULL) &&
          ((training_angle_chart_last_tick == 0U) ||
           ((now - training_angle_chart_last_tick) >= 100U)))
      {
        lv_coord_t angle_deg = (lv_coord_t)display_angle;
        if (angle_deg > 180)
        {
          angle_deg = 180;
        }
        lv_chart_set_next_value(training_angle_chart, training_angle_series, angle_deg);
        lv_chart_refresh(training_angle_chart);
        training_angle_chart_last_tick = now;
      }
#endif
    }
  }

  if (current_ui_page == UPPER_PAGE_TRAINING)
  {
    uint32_t train_now = (training_paused != 0U) ? training_pause_start_tick : now;
    uint32_t elapsed_ms = 0U;
    uint32_t target_ms = (uint32_t)training_target_duration_sec * 1000U;
    uint32_t elapsed_s;

    if (train_now >= training_start_tick)
    {
      elapsed_ms = train_now - training_start_tick;
      if (elapsed_ms > training_paused_accum_ms)
      {
        elapsed_ms -= training_paused_accum_ms;
      }
      else
      {
        elapsed_ms = 0U;
      }
    }
    if (elapsed_ms > target_ms)
    {
      elapsed_ms = target_ms;
    }
    elapsed_s = elapsed_ms / 1000U;

    if (training_timer_arc != NULL)
    {
      lv_arc_set_value(training_timer_arc, (int16_t)elapsed_s);
    }
    if (training_timer_value_label != NULL)
    {
      lv_label_set_text_fmt(training_timer_value_label, "%02lu:%02lu",
                            (unsigned long)(elapsed_s / 60U),
                            (unsigned long)(elapsed_s % 60U));
    }
    if ((elapsed_ms >= target_ms) && (training_paused == 0U))
    {
      training_completed_count++;
      UpperArm_AddHistoryRecord();
      wireless_train_start_pending = 0U;
      wireless_train_finish_pending = 1U;
      UpperArm_LvglCreateResult();
      return;
    }
  }
  else if (current_ui_page == UPPER_PAGE_FREE_TRAINING)
  {
    uint32_t elapsed_ms = 0U;
    uint32_t target_ms = (uint32_t)training_target_duration_sec * 1000U;
    uint8_t i;
    static const char *action_texts[5] = {
      UI_ZH_ELBOW_FLEX, UI_ZH_ARM_ABD, UI_ZH_FOREARM_ROT, UI_ZH_SHOULDER_LIFT, UI_ZH_NO_ACTION
    };

    if (now >= training_start_tick)
    {
      elapsed_ms = now - training_start_tick;
    }

    if (free_action_label != NULL)
    {
      lv_label_set_text(free_action_label, action_texts[free_current_action_index % 5U]);
    }
    if (free_count_label != NULL)
    {
      lv_label_set_text_fmt(free_count_label, "%u", (unsigned int)free_total_reps);
    }
    if (free_angle_label != NULL)
    {
      lv_label_set_text_fmt(free_angle_label, "%u°", (unsigned int)free_current_angle);
    }
    for (i = 0U; i < 4U; i++)
    {
      if (free_action_count_labels[i] != NULL)
      {
        lv_label_set_text_fmt(free_action_count_labels[i], "%u", (unsigned int)free_action_counts[i]);
      }
    }

    if (elapsed_ms >= target_ms)
    {
      wireless_train_finish_pending = 1U;
      UpperArm_AddFreeHistoryRecord();
      UpperArm_LvglCreateFreeResult();
      return;
    }
  }

  connected = UpperArm_WirelessIsConnected(now);
  if ((last_connected == 0U) && (connected != 0U))
  {
    UpperArm_RequestVibration();
  }
  last_connected = connected;

  bt_color = connected ? lv_color_hex(0x0098F0) : lv_color_hex(0x8A8F98);
  if (home_bt_icon != NULL)
  {
    lv_obj_set_style_text_color(home_bt_icon, bt_color, 0);
  }

  if (home_status_value_label != NULL)
  {
    lv_label_set_text(home_status_value_label, connected ? "已连接" : "未连接");
    lv_obj_set_style_text_color(home_status_value_label,
                                connected ? lv_color_hex(0x27AE60) : lv_color_hex(0x9AA8BD),
                                0);
  }
  if (home_status_dot != NULL)
  {
    lv_obj_set_style_bg_color(home_status_dot,
                              connected ? lv_color_hex(0x19D64A) : lv_color_hex(0xE74C3C),
                              0);
  }
}

static void UpperArm_OnHomeButtonClicked(lv_event_t *e)
{
  uintptr_t index = (uintptr_t)lv_event_get_user_data(e);

  if (index == 0U)
  {
    UpperArm_LvglSetTransitionDir(1);
    UpperArm_LvglCreateActionSelect();
  }
  else if (index == 1U)
  {
    UpperArm_LvglSetTransitionDir(1);
    UpperArm_LvglCreateHistory();
  }
  else if (index == 2U)
  {
    if (UpperArm_WirelessIsConnected(osKernelGetTickCount()) == 0U)
    {
      UpperArm_LvglSetTransitionDir(1);
      UpperArm_LvglCreateLinkLost();
      return;
    }
    prep_for_free_mode = 1U;
    UpperArm_LvglSetTransitionDir(1);
    UpperArm_LvglCreatePrep();
  }
  else if (index == 3U)
  {
    UpperArm_LvglSetTransitionDir(1);
    UpperArm_LvglCreateSettings();
  }
}

static void UpperArm_OnAnalyticsClicked(lv_event_t *e)
{
  (void)e;
  UpperArm_LvglSetTransitionDir(1);
  UpperArm_LvglCreateAnalytics();
}

static void UpperArm_OnSettingsBackClicked(lv_event_t *e)
{
  (void)e;
  UpperArm_LvglSetTransitionDir(-1);
  UpperArm_LvglCreateHome();
}

static void UpperArm_OnSettingsSwitchChanged(lv_event_t *e)
{
  uintptr_t index = (uintptr_t)lv_event_get_user_data(e);
  lv_obj_t *sw = lv_event_get_target(e);

  if (index < (sizeof(settings_switch_state) / sizeof(settings_switch_state[0])))
  {
    settings_switch_state[index] = lv_obj_has_state(sw, LV_STATE_CHECKED) ? 1U : 0U;
    if ((index == 2U) && (settings_switch_state[index] == 0U))
    {
      UpperArm_ClearHistory();
    }
  }
  else if (index == UPPER_SETTINGS_NIGHT_SWITCH_INDEX)
  {
    upper_night_mode = lv_obj_has_state(sw, LV_STATE_CHECKED) ? 1U : 0U;
    UpperArm_HistorySaveToFlash();
    UpperArm_LvglSetTransitionDir(0);
    UpperArm_LvglCreateSettings();
  }
}

static void UpperArm_OnClearHistoryClicked(lv_event_t *e)
{
  (void)e;
  UpperArm_ClearHistory();
  UpperArm_RequestVibration();
  UpperArm_LvglSetTransitionDir(0);
  UpperArm_LvglCreateHistory();
}

static void UpperArm_OnSettingsAdjustClicked(lv_event_t *e)
{
  uintptr_t action = (uintptr_t)lv_event_get_user_data(e);

  (void)e;

  if ((action == 0U) && (training_target_count > 1U))
  {
    training_target_count--;
  }
  else if ((action == 1U) && (training_target_count < 20U))
  {
    training_target_count++;
  }
  else if ((action == 2U) && (training_target_duration_sec > 30U))
  {
    training_target_duration_sec = (uint16_t)(training_target_duration_sec - 30U);
  }
  else if ((action == 3U) && (training_target_duration_sec < 300U))
  {
    training_target_duration_sec = (uint16_t)(training_target_duration_sec + 30U);
  }

  if (settings_target_count_label != NULL)
  {
    lv_label_set_text_fmt(settings_target_count_label, "%u%s",
                          (unsigned int)training_target_count,
                          UI_ZH_COUNT_UNIT);
  }
  if (settings_duration_label != NULL)
  {
    lv_label_set_text_fmt(settings_duration_label, "%u%s",
                          (unsigned int)training_target_duration_sec,
                          UI_ZH_SECOND_UNIT);
  }
}

static void UpperArm_OnSettingsResetClicked(lv_event_t *e)
{
  (void)e;

  settings_switch_state[0] = 1U;
  settings_switch_state[1] = 1U;
  settings_switch_state[2] = 1U;
  upper_night_mode = 0U;
  training_target_count = 3U;
  training_target_duration_sec = 60U;
  UpperArm_HistorySaveToFlash();

  UpperArm_LvglSetTransitionDir(0);
  UpperArm_LvglCreateSettings();
}

static void UpperArm_OnActionButtonClicked(lv_event_t *e)
{
  uintptr_t index = (uintptr_t)lv_event_get_user_data(e);
  if (index >= 4U)
  {
    return;
  }
  selected_action_index = (uint8_t)index;
  UpperArm_LvglSetTransitionDir(0);
  UpperArm_LvglCreateActionSelect();
  UpperArm_LvglUpdateHome(osKernelGetTickCount());
}

static void UpperArm_OnStartTrainingClicked(lv_event_t *e)
{
  (void)e;

  if (UpperArm_WirelessIsConnected(osKernelGetTickCount()) == 0U)
  {
    UpperArm_LvglSetTransitionDir(1);
    UpperArm_LvglCreateLinkLost();
    return;
  }

  prep_for_free_mode = 0U;
  UpperArm_LvglSetTransitionDir(1);
  UpperArm_LvglCreatePrep();
}

static void UpperArm_OnPauseTrainingClicked(lv_event_t *e)
{
  uint32_t now = osKernelGetTickCount();

  (void)e;

  if (training_paused == 0U)
  {
    training_paused = 1U;
    training_pause_start_tick = now;
  }
  else
  {
    training_paused = 0U;
    if (now >= training_pause_start_tick)
    {
      training_paused_accum_ms += (now - training_pause_start_tick);
    }
    training_pause_start_tick = 0U;
  }
  if (training_pause_icon_label != NULL)
  {
    lv_label_set_text(training_pause_icon_label, (training_paused != 0U) ? LV_SYMBOL_PLAY : LV_SYMBOL_PAUSE);
  }
  if (training_pause_text_label != NULL)
  {
    lv_label_set_text(training_pause_text_label, (training_paused != 0U) ? UI_ZH_CONTINUE : UI_ZH_PAUSE);
  }
}

static void UpperArm_LvglSetObjOpa(void *obj, int32_t value)
{
  lv_obj_set_style_text_opa((lv_obj_t *)obj, (lv_opa_t)value, 0);
}

static void UpperArm_LvglSetObjZoom(void *obj, int32_t value)
{
  lv_obj_set_style_transform_zoom((lv_obj_t *)obj, (lv_coord_t)value, 0);
}

static void UpperArm_LvglSetTransitionDir(int8_t dir)
{
  upper_page_transition_dir = dir;
}

static void UpperArm_LvglApplyNightPalette(lv_obj_t *obj)
{
  uint32_t i;
  uint32_t child_count;

  if ((obj == NULL) || (upper_night_mode == 0U))
  {
    return;
  }

  if (lv_obj_get_style_bg_opa(obj, LV_PART_MAIN) >= LV_OPA_20)
  {
    uint32_t bg = lv_color_to32(lv_obj_get_style_bg_color(obj, LV_PART_MAIN)) & 0x00FFFFFFUL;
    uint8_t r = (uint8_t)((bg >> 16) & 0xFFU);
    uint8_t g = (uint8_t)((bg >> 8) & 0xFFU);
    uint8_t b = (uint8_t)(bg & 0xFFU);

    if (((uint16_t)r + (uint16_t)g + (uint16_t)b) > 560U)
    {
      lv_obj_set_style_bg_color(obj, lv_color_hex(0x071C36), LV_PART_MAIN);
      lv_obj_set_style_border_color(obj, lv_color_hex(0x1C4D82), LV_PART_MAIN);
      lv_obj_set_style_shadow_opa(obj, LV_OPA_10, LV_PART_MAIN);
    }
    else if ((b > r) && (b > g) && (((uint16_t)r + (uint16_t)g + (uint16_t)b) > 330U))
    {
      lv_obj_set_style_bg_color(obj, lv_color_hex(0x10284A), LV_PART_MAIN);
      lv_obj_set_style_border_color(obj, lv_color_hex(0x245C96), LV_PART_MAIN);
    }
  }

  {
    uint32_t text = lv_color_to32(lv_obj_get_style_text_color(obj, LV_PART_MAIN)) & 0x00FFFFFFUL;
    uint8_t r = (uint8_t)((text >> 16) & 0xFFU);
    uint8_t g = (uint8_t)((text >> 8) & 0xFFU);
    uint8_t b = (uint8_t)(text & 0xFFU);
    uint16_t sum = (uint16_t)r + (uint16_t)g + (uint16_t)b;

    if (sum < 260U)
    {
      lv_obj_set_style_text_color(obj, lv_color_hex(0xDCEBFF), LV_PART_MAIN);
    }
    else if ((b > r) && (b >= g))
    {
      lv_obj_set_style_text_color(obj, lv_color_hex(0x6AB7FF), LV_PART_MAIN);
    }
  }

  child_count = lv_obj_get_child_cnt(obj);
  for (i = 0U; i < child_count; i++)
  {
    UpperArm_LvglApplyNightPalette(lv_obj_get_child(obj, (int32_t)i));
  }
}

static void UpperArm_LvglPageReady(lv_obj_t *screen)
{
  if (screen == NULL)
  {
    return;
  }

  if (current_ui_page != UPPER_PAGE_ACTION_SELECT)
  {
    UpperArm_LvglApplyNightPalette(screen);
  }

  if (current_ui_page != UPPER_PAGE_TRAINING)
  {
    if (training_emg_enable_timer != NULL)
    {
      lv_timer_del(training_emg_enable_timer);
      training_emg_enable_timer = NULL;
    }
    EmgSensor_SetModelEnabled(0U);
  }

  if (upper_page_transition_ready == 0U)
  {
    upper_page_transition_ready = 1U;
    upper_page_transition_dir = 1;
    return;
  }

  if (upper_page_transition_dir == 0)
  {
    upper_page_transition_dir = 1;
    return;
  }

  upper_page_transition_dir = 1;
}

static void UpperArm_PrepAnimateNumber(void)
{
  lv_anim_t anim;

  if (prep_countdown_label == NULL)
  {
    return;
  }

  lv_obj_set_style_transform_zoom(prep_countdown_label, 210, 0);
  lv_obj_set_style_text_opa(prep_countdown_label, LV_OPA_40, 0);

  lv_anim_init(&anim);
  lv_anim_set_var(&anim, prep_countdown_label);
  lv_anim_set_time(&anim, 320U);
  lv_anim_set_values(&anim, LV_OPA_40, LV_OPA_COVER);
  lv_anim_set_path_cb(&anim, lv_anim_path_ease_out);
  lv_anim_set_exec_cb(&anim, UpperArm_LvglSetObjOpa);
  lv_anim_start(&anim);

  lv_anim_init(&anim);
  lv_anim_set_var(&anim, prep_countdown_label);
  lv_anim_set_time(&anim, 320U);
  lv_anim_set_values(&anim, 210, 256);
  lv_anim_set_path_cb(&anim, lv_anim_path_ease_out);
  lv_anim_set_exec_cb(&anim, UpperArm_LvglSetObjZoom);
  lv_anim_start(&anim);
}

static void UpperArm_PrepUpdateDots(uint16_t elapsed_ms)
{
  uint8_t active = (uint8_t)((elapsed_ms / 150U) % 6U);
  uint8_t i;

  for (i = 0U; i < 6U; i++)
  {
    uint8_t dist;
    lv_coord_t size;
    lv_opa_t opa;
    lv_color_t color;

    if (prep_status_dots[i] == NULL)
    {
      continue;
    }

    dist = (i > active) ? (uint8_t)(i - active) : (uint8_t)(active - i);
    if (dist > 3U)
    {
      dist = (uint8_t)(6U - dist);
    }

    if (dist == 0U)
    {
      size = 5;
      opa = LV_OPA_COVER;
      color = lv_color_hex(0x2F80ED);
    }
    else if (dist == 1U)
    {
      size = 4;
      opa = LV_OPA_70;
      color = lv_color_hex(0x63A6F7);
    }
    else
    {
      size = 3;
      opa = LV_OPA_40;
      color = lv_color_hex(0xA9CFFF);
    }

    lv_obj_set_size(prep_status_dots[i], size, size);
    lv_obj_set_style_bg_color(prep_status_dots[i], color, 0);
    lv_obj_set_style_bg_opa(prep_status_dots[i], opa, 0);
  }
}

static void UpperArm_OnPrepCountdown(lv_timer_t *timer)
{
  if ((prep_countdown_label == NULL) || (prep_spinner_arc == NULL))
  {
    if (timer != NULL)
    {
      lv_timer_del(timer);
      prep_countdown_timer = NULL;
    }
    return;
  }

  if (prep_remaining_ms > 50U)
  {
    uint8_t next_value;

    prep_remaining_ms = (uint16_t)(prep_remaining_ms - 50U);
    lv_arc_set_value(prep_spinner_arc, (int32_t)((prep_remaining_ms * 100U) / 5000U));
    UpperArm_PrepUpdateDots((uint16_t)(5000U - prep_remaining_ms));

    next_value = (uint8_t)((prep_remaining_ms + 999U) / 1000U);
    if (next_value == 0U)
    {
      next_value = 1U;
    }

    if (next_value != prep_countdown_value)
    {
      prep_countdown_value = next_value;
      lv_label_set_text_fmt(prep_countdown_label, "%u", (unsigned int)prep_countdown_value);
      UpperArm_PrepAnimateNumber();
    }
  }
  else
  {
    lv_label_set_text(prep_countdown_label, "0");
    lv_arc_set_value(prep_spinner_arc, 0);
    if (timer != NULL)
    {
      lv_timer_del(timer);
      prep_countdown_timer = NULL;
    }
    if (prep_transition_timer != NULL)
    {
      lv_timer_del(prep_transition_timer);
    }
    prep_transition_timer = lv_timer_create(UpperArm_OnPrepTransition, 80U, NULL);
  }
}

static void UpperArm_OnPrepRecalibrate(lv_timer_t *timer)
{
  if (timer != NULL)
  {
    lv_timer_del(timer);
  }
  if (prep_recalibrate_timer == timer)
  {
    prep_recalibrate_timer = NULL;
  }

  if (current_ui_page == UPPER_PAGE_PREP)
  {
    EmgSensor_RecalibrateForTraining();
  }
}

static void UpperArm_OnPrepTransition(lv_timer_t *timer)
{
  if (timer != NULL)
  {
    lv_timer_del(timer);
  }
  if (prep_transition_timer == timer)
  {
    prep_transition_timer = NULL;
  }

  if (current_ui_page != UPPER_PAGE_PREP)
  {
    return;
  }

  if (UpperArm_WirelessIsConnected(osKernelGetTickCount()) == 0U)
  {
    UpperArm_LvglSetTransitionDir(1);
    UpperArm_LvglCreateLinkLost();
    return;
  }

  UpperArm_LvglSetTransitionDir(1);
  if (prep_for_free_mode != 0U)
  {
    prep_for_free_mode = 0U;
    UpperArm_LvglCreateFreeTraining();
  }
  else
  {
    UpperArm_LvglCreateTraining();
  }
}

static void UpperArm_OnTrainingEmgEnable(lv_timer_t *timer)
{
  if (timer != NULL)
  {
    lv_timer_del(timer);
  }
  if (training_emg_enable_timer == timer)
  {
    training_emg_enable_timer = NULL;
  }

  if (current_ui_page == UPPER_PAGE_TRAINING)
  {
    EmgSensor_SetModelEnabled(1U);
  }
}

static void UpperArm_OnFinishTrainingClicked(lv_event_t *e)
{
  (void)e;
  wireless_train_start_pending = 0U;
  wireless_train_finish_pending = 1U;
  if (current_ui_page == UPPER_PAGE_FREE_TRAINING)
  {
    UpperArm_AddFreeHistoryRecord();
    UpperArm_LvglSetTransitionDir(1);
    UpperArm_LvglCreateFreeResult();
    return;
  }
  training_completed_count++;
  UpperArm_AddHistoryRecord();
  UpperArm_LvglSetTransitionDir(1);
  UpperArm_LvglCreateResult();
}

static void UpperArm_OnRetrainClicked(lv_event_t *e)
{
  (void)e;
  UpperArm_LvglSetTransitionDir(-1);
  UpperArm_LvglCreateActionSelect();
  UpperArm_LvglUpdateHome(osKernelGetTickCount());
}

static void UpperArm_OnBackHomeClicked(lv_event_t *e)
{
  (void)e;
  UpperArm_LvglSetTransitionDir(-1);
  UpperArm_LvglCreateHome();
}

static int32_t Rehab_AbsI32(int32_t value)
{
  return (value < 0) ? -value : value;
}

static int16_t Rehab_FloatToI16Scaled(float value, float scale)
{
  int32_t scaled = (int32_t)((value * scale) + ((value >= 0.0f) ? 0.5f : -0.5f));

  if (scaled > INT16_MAX)
  {
    scaled = INT16_MAX;
  }
  if (scaled < INT16_MIN)
  {
    scaled = INT16_MIN;
  }
  return (int16_t)scaled;
}

static int32_t Rehab_AngleToSignedX100(float angle_deg)
{
  int32_t scaled;

  if ((isfinite(angle_deg) == 0) || (angle_deg < -3600.0f) || (angle_deg > 3600.0f))
  {
    return 0;
  }

  while (angle_deg < -180.0f)
  {
    angle_deg += 360.0f;
  }
  while (angle_deg >= 180.0f)
  {
    angle_deg -= 360.0f;
  }

  scaled = (int32_t)((angle_deg * 100.0f) + ((angle_deg >= 0.0f) ? 0.5f : -0.5f));
  if (scaled > 18000)
  {
    scaled = 18000;
  }
  if (scaled < -18000)
  {
    scaled = -18000;
  }
  return scaled;
}

static int32_t Rehab_AngleDiffAbsX100(int32_t a_x100, int32_t b_x100)
{
  int32_t diff = a_x100 - b_x100;

  while (diff > 18000)
  {
    diff -= 36000;
  }
  while (diff < -18000)
  {
    diff += 36000;
  }

  return Rehab_AbsI32(diff);
}

static int32_t Rehab_AngleDiffSignedX100(int32_t a_x100, int32_t b_x100)
{
  int32_t diff = a_x100 - b_x100;

  while (diff > 18000)
  {
    diff -= 36000;
  }
  while (diff < -18000)
  {
    diff += 36000;
  }

  return diff;
}

static char Rehab_SignCharI32(int32_t value)
{
  return (value < 0) ? '-' : '+';
}






