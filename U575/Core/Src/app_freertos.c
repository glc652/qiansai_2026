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
#include "lcd_st7789.h"
#include "lsm6dsr.h"
#include "imu_processing.h"
#include "i2c.h"
#include "spi.h"
#include "adc.h"
#include "touch_cst816t.h"
#include "usart.h"
#include "wireless_link.h"
#include "mks142.h"
#include "lvgl.h"
#include "lv_port_stm32u575.h"
#include "vibration_motor.h"
#include "wrist_settings.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define IMU_ATTITUDE_KP    (1.0f)
#define IMU_ATTITUDE_KI    (0.05f)
#define IMU_ANGLE_DEADBAND_X10  (5)
#define IMU_GYRO_DEADBAND_RAW  (2)
#define WRIST_ATTITUDE_LCD_DEBUG_UI  (0U)
#define WRIST_WIRELESS_PERIOD_MS  (50U)
#define WRIST_HEALTH_WIRELESS_PERIOD_MS  (200U)
#define WRIST_WIRELESS_ASCII_PING_DEBUG  (0U)
#define WRIST_WIRELESS_STATUS_DEBUG  (0U)
#define WRIST_HEART_DATA_TIMEOUT_MS  (3000U)
#define WRIST_HEALTH_UPDATE_MS       (5U)
#define WRIST_HEALTH_PULSE_TREND_POINTS (32U)
#define WRIST_HEALTH_PULSE_STEP_MS   (33U)
#define WRIST_HEALTH_PULSE_BLEND_STEPS (6U)
#define WRIST_VIBRATION_PROMPT_MS  (250U)
#define WRIST_VIBRATION_BOOT_TEST  (0U)
#define WRIST_USER_KEY_DEBOUNCE_MS  (30U)
#define WRIST_USER_KEY_LONG_MS      (800U)
#define WRIST_HOME_SLEEP_TIMEOUT_MS (30000U)
#define WRIST_SLEEP_POLL_MS         (50U)
#define WRIST_BATTERY_UPDATE_MS     (10000U)
#define WRIST_BATTERY_ADC_MAX       (4095U)
#define WRIST_BATTERY_ADC_VREF_MV   (3300U)
#define WRIST_BATTERY_DIVIDER_NUM   (2U)
#define WRIST_BATTERY_DIVIDER_DEN   (1U)
#define WRIST_BATTERY_EMPTY_MV      (3400U)
#define WRIST_BATTERY_FULL_MV       (3900U)
#define WRIST_BATTERY_STORE_ADDR    (0x081FE000U)
#define WRIST_BATTERY_STORE_PAGE    (127U)
#define WRIST_BATTERY_STORE_BANK    FLASH_BANK_2
#define WRIST_BATTERY_STORE_MAGIC   (0x57524254U)
#define WRIST_BATTERY_STORE_CRC_XOR (0xA55A5AA5U)
#define WRIST_BATTERY_RISE_NOISE_MAX (2U)
/* 缂?1 闂?DebugTask 缂?UART4 闂傚倸鍊烽懗鍫曞箠閹剧粯鍊舵慨妯挎硾缁犳壆绱掔€ｎ厽纭堕柡鍡愬€濋弻娑㈠箛閸忓摜鎸夐梺绋款儐閹瑰洤鐣烽悜绛嬫晣闁绘劗澧楅崟鍐╃節绾板纾块柛瀣洴瀹曟劘銇愰幒鎴犵杽闂侀潧艌閺呮稓绮绘导鏉戠閻庢稒鈼ら鍛洸闁诡垼鐏愯ぐ鎺撴櫜闁搞儮鏅滈幉妯衡攽閻愮偣鈧鎹㈠┑瀣摕闁靛鍎Σ鍫熶繆椤栨粌甯舵い锝嗘そ濮婂搫效閸パ呭姶闂佺顑呭Λ娑㈠礆閹烘鏁囬柕蹇婂墲濞呭洭姊洪柅鐐茶嫰婢ф煡鏌曢崶銊ュ鐎规洜顭堣灃濞达綀顫夐悵顐︽⒑鐠囪尙绠抽柛瀣Т铻為柛鏇ㄥ幘閻棝鎮楅敐搴樺亾?0.1闂傚倷绀佸﹢閬嶅磿閵堝棛绠惧┑鐘叉搐閺嬩胶鈧箍鍎卞Λ妤冩閻愬绠鹃柟瀵稿仦閻ㄦ垶銇勯敂璇叉灍缂佺粯绋撻埀顒傛暩椤牆鐡梻浣侯攰濞呮洟骞戦崶顒€绠氶柛鎰靛枟閸嬪嫰鏌ｉ幋鐏活亪鎮垫导瀛樷拺闁告繂瀚婵嗏攽椤旂偓鏆€?*/
#define DEBUG_ANGLE_PRINT  (0)
/* 缂?1 闂?HeartRateTask 缂?UART4 闂傚倸鍊烽懗鍫曞箠閹剧粯鍊舵慨妯挎硾缁犳壆绱掔€ｎ厽纭堕柡鍡愬€濋弻娑㈠箛闂堟稑绠婚梺姹囧€愰崑鎾绘⒒娴ｅ憡鍟為柛鏃撻檮缁傚秴顭ㄩ崼鐔锋疅?闂傚倷娴囧畷鐢稿磻閻愮數鐭欓柛顐犲劚绾惧鏌熼幆褏鎽犻柛?闂傚倷娴囧畷鐢稿磻閻愮數鐭欓柛顐犲劚绾惧鏌熼崜褏甯涢柣鎾存礃娣囧﹪顢涘▎鎺濆妳闂佺瀛╃划搴ｆ閹烘挻缍囬柕濞у懐鏆紓鍌欑閸婂湱鏁垾宕囨殾闁靛ň鏅涚粻銉︺亜閺冨倻鎽傞柛鐔插亾闂傚倷娴囧畷鍨叏閺夋嚚娲晝閳ь剛鍙呭┑鐘诧工閻楀棝寮?*/
#define DEBUG_HR_PRINT  (0)
/* 缂?1闂傚倸鍊烽悞锔锯偓绗涘懐鐭欓柟鐑橆殕閸庢銇勯弮鍥棄缂佸墎鍋熼埀顒€绠嶉崕閬嵥囬婊呭暗鐎广儱顦伴悡鏇㈡煙閹规劕鐨洪柛鏂跨Ч閺?USART1(PA9/PA10) 闂傚倷娴囧畷鐢稿窗瀹ュ鍊块柨鏇炲€归崑銈夋煛閸ャ儱鐒洪柡浣割儔閺屸剝寰勭€ｅ墎绌块梺闈╁瘜閸樻悂宕戦幘缁樻櫜閹肩补鈧磭顔掓繝?MKS-142闂傚倸鍊烽悞锔锯偓绗涘懐鐭欓柟鐑樻⒒閻瑩鏌熼悜妯烩拻闁活厼鐗撻幃妤呮偨閻㈢偣鈧﹪鏌涚€ｎ偅宕岄柟顔惧厴楠炲秹顢欓崣銉х闂?0 闂傚倸鍊烽悞锕傚箖閸洖纾块柟鎯版绾惧鏌ｉ幇顏囶劅闁轰礁妫欑换婵囩節閸屾碍鐏撻梺?USART2 闂傚倸鍊风粈渚€骞栭锕€纾瑰ù鐘差儜缂嶆牕顭块懜闈涘闁?*/
#define MKS142_USE_WIRELESS_USART1   (0)

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
/* Wrist node local IMU cache. This node sends these values to the upper-arm node. */
static LSM6DSR_Data_t wrist_imu_raw;
static IMUProc_Euler_t wrist_imu_euler;
static uint8_t wrist_imu_valid;
static volatile uint32_t g_wireless_last_ok_tick;
static volatile uint32_t g_wireless_last_error_tick;
static volatile uint8_t g_wireless_uart_ok;
static volatile uint8_t g_wireless_rx_enabled;
static volatile uint8_t g_wrist_sleeping;
static volatile uint8_t g_wrist_health_mode_active;
static volatile uint8_t g_wrist_health_enter_frames;
static volatile uint8_t g_wrist_health_exit_frames;
static uint32_t s_battery_store_seq;
static uint8_t s_battery_percent;
static uint8_t s_battery_percent_valid;

/* MKS-142 闂傚倸鍊搁…顒勫磻閸曨個娲Χ婢跺﹦鏌堥梺鍝勵槹椤戞瑩宕崨瀛樼厽闁规儳鍟块鍌炴煟閺冨倸甯堕柣蹇斿▕閺屟嗙疀閿濆懍绨绘繛瀵稿У閹稿啿顫忓ú顏勭閹艰揪绲烘慨鍥⒑閻撳海绉虹紒鐘崇墵閻涱喗鎯旈埥鍡樺兊闁哄鐗勯崝宀勫几閸岀偞鈷戦柟绋挎捣缁犳挻淇婇锝囨创闁糕斁鍋撳銈嗗灱濞夋洟鎯屽▎鎴斿亾鐟欏嫭绀冨┑鐐诧攻娣囧﹪骞栨担鑲澭囨煕閵夈垺娅嗛柛鐘崇洴濮婅櫣鎷犻崣澶嬪闯闂佽桨鐒﹂幃鍌炵嵁婵犲洤绠婚柛鎴炴緲濞诧妇鎹㈠┑鍡╂僵妞ゆ帒鍊婚崢顒勬⒒娴ｇ儤鍤€闁宦板妿閹广垽宕熼锝嗘櫆闂佸湱铏庨崰妤呮偂閻斿吋鐓忓┑鐐茬仢閸旀碍銇勮箛濠冩珔闂囧鏌ｅΟ鍨毢闁圭晫濞€閺岀喖顢涘☉娆樻闂佸搫琚崝搴ㄥ焵椤掑﹦绉甸柛瀣嚇椤㈡瑥顓兼径瀣ф嫼缂佺虎鍘奸幏瀣吹閸愵喗鐓曢柨婵嗛閻忥絾淇婇崣澶婂闁宠閰ｉ獮妯尖偓鐢殿焾婵兘姊绘担鍝ョШ闁稿锕畷婊冣槈閵忕姷顔戦梺缁樻煥閸氬鎮￠悩缁樼厱婵犻潧妫楅鈺呮煃瑜滈崜娑㈠礂濮椻偓楠炲﹤鈻庨幘宕囶吅闂佹寧娲嶉崑鎾翠繆椤愶缉鎴﹀Φ閸曨垰绠婚悹铏瑰劋閻庣粯绻濋姀锝嗙【缂佸缍婂濠氭晸閻樿尙鍔﹀銈嗗笒閸婅鍒婃總鍛婄厱闁挎棁顕ч獮妯汇亜?*/
static MKS142_Parser_t s_mks142_parser;
static MKS142_Data_t g_heart_data;
static uint8_t g_heart_valid;
static volatile uint32_t g_heart_last_frame_tick;
static volatile uint8_t g_screen_heart_rate;
static volatile uint8_t g_screen_spo2;
static volatile uint8_t g_heart_start_request;
static volatile uint32_t s_mks142_rx_events;
static volatile uint32_t s_mks142_rx_bytes;
static uint8_t s_mks142_u2_byte;   /* USART2 婵犵數濮甸鏍窗濡ゅ啯宕查柟閭﹀枛缁躲倝鏌ｉ敐鍛拱妞ゎ偅娲熼弻銊╂偄閸濆嫅锝夋煛閸滀礁澧柟渚垮妼椤粓宕卞鎯у毐闂備礁鎲￠敃鈺呭礈閻旂厧钃熼柨鐔哄Т绾惧ジ鏌曢崼婵囧櫣缂侇偄绉瑰娲焻閻愯尪瀚板褜鍨堕弻娑㈠籍閳ь剙煤閻旂厧绠栧Δ锔筋儥濡嫰姊洪悷鎵紞闁稿鍊曢悾鐑藉箳閹存梹顫嶅┑鈽嗗灟鐠€锕€危婵犳碍鈷掑ù锝咁潟閳ь兘鍋撻梺绋款儐閹瑰洭寮诲☉銏犵闁艰壈娉涢崢鈩冪節閵忥絾纭鹃悗姘嵆瀵?*/
static uint8_t s_mks142_u1_byte;   /* USART1 婵犵數濮烽弫鎼佸磻閻愬樊鐒芥繛鍡樻惄閺佸嫰鏌涢鐘插姕闁稿骸锕ラ妵鍕冀椤愵澀娌梺鍝勬媼閸撴岸骞堥妸銉建闁糕剝銇為崙浠嬫⒑閸涘渚涢柛鎾跺枛瀵鏁撻悩鑼紲闂侀潧鐗嗛幊搴ｇ尵瀹ュ鈷戦梺顐ゅ仜閼活垱鏅堕鐐寸厱闁哄啠鍋撴繛鑼枛楠炲啯銈﹀▎鐐枌闂備胶鎳撶壕顓㈠磻閵堝懐鏆﹂柟鐑樺灍濡插牊淇婇娆掝劅婵℃彃顭峰缁樻媴婵劏鍋撻埀顒勬煕鐎ｎ偅宕岄柡灞剧洴楠炴﹢鎳犵捄鍝勫腐濠电姰鍨煎▔娑氣偓姘嵆瀵?*/

osThreadId_t HeartRateTaskHandle;
const osThreadAttr_t HeartRateTask_attributes = {
  .name = "HeartRateTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 512 * 4
};
osSemaphoreId_t mks142FrameSemHandle;
const osSemaphoreAttr_t mks142FrameSem_attributes = {
  .name = "mks142FrameSem"
};

/* USER CODE END Variables */

/* USER CODE BEGIN 0 */
static int16_t AngleToX100(float angle_deg)
{
  int32_t scaled;

  scaled = (int32_t)((angle_deg * 100.0f) + ((angle_deg >= 0.0f) ? 0.5f : -0.5f));
  if (scaled > INT16_MAX)
  {
    scaled = INT16_MAX;
  }
  else if (scaled < INT16_MIN)
  {
    scaled = INT16_MIN;
  }

  return (int16_t)scaled;
}

static int16_t FloatToI16Scaled(float value, float scale)
{
  int32_t scaled;

  scaled = (int32_t)((value * scale) + ((value >= 0.0f) ? 0.5f : -0.5f));
  if (scaled > INT16_MAX)
  {
    scaled = INT16_MAX;
  }
  else if (scaled < INT16_MIN)
  {
    scaled = INT16_MIN;
  }

  return (int16_t)scaled;
}

static void FillWristWirelessFrame(WirelessWristFrame_t *frame,
                                   uint16_t seq,
                                   uint32_t tick,
                                   const LSM6DSR_Data_t *imu_raw,
                                   const IMUProc_Euler_t *euler,
                                   uint8_t imu_valid)
{
  uint8_t battery_percent;
  uint8_t battery_valid;
  uint8_t uart_ok;
  uint8_t sleeping;
  uint8_t health_mode;
  uint8_t health_enter_event;
  uint8_t health_exit_event;

  /* 闂傚倸鍊搁…顒勫磻閸曨個娲Χ婢跺﹦鏌堥梺鍝勵槹椤戞瑩宕崨顓涙斀闁稿本纰嶉崯鐐烘煕?USART2 ISR 闂傚倸鍊风粈渚€骞夐敓鐘冲仭闁挎洖鍊搁崹鍌炴煟閵忋垺鏆╅柛妤佸哺閺岋綁骞嬮敐鍛呮捇鏌￠崪浣稿⒋闁诡喗锕㈤幃娆撴濞戞ü绮俊鐐€ч梽鍕偂閿熺姴钃熼柨婵嗩槹閺呮煡鏌涘☉娆愮凡妞ゅ浚鍙冮弻锝堢疀閹惧顩伴梺鎼炲妼閻栧ジ鎮伴璺ㄧ杸闁规崘鍩栧▓婵嬫⒑缂佹﹩娈旈柣妤€妫楅妴鎺楀箛閻楀牃鎷虹紓鍌欑劍钃遍悗鍨懇閺屾稓浠﹂崜褏鐓傚銈嗘⒐濡啴寮?valid 濠?bpm 濠电姷鏁搁崑鐐哄垂閸洖绠伴柟缁㈠枛绾惧鏌熼崜褏甯涢柣鎾跺█閺岋綁濡舵惔锛勪画閻熸粍濡搁崨顖滐紲闁哄鐗勯崝搴ㄥ箠閸モ斁鍋撶憴鍕┛缂傚秳绀侀悾鐑藉箳閹搭厾鍙嗛梺鍛婂姂閸斿矂顢?wrist_imu_* 缂傚倸鍊搁崐鐑芥倿閿曞倸绠繝闈涚墢娑撳秹鏌熸潏楣冩闁稿鏅涜灃闁挎繂鎳庨弳鐐烘煛閸滀礁澧い顓℃硶閹瑰嫰鎼归悷閭︽殼闂?*/
  taskENTER_CRITICAL();
  battery_percent = s_battery_percent;
  battery_valid = s_battery_percent_valid;
  uart_ok = g_wireless_uart_ok;
  sleeping = g_wrist_sleeping;
  health_mode = g_wrist_health_mode_active;
  health_enter_event = (g_wrist_health_enter_frames != 0U) ? 1U : 0U;
  health_exit_event = (g_wrist_health_exit_frames != 0U) ? 1U : 0U;
  if (g_wrist_health_enter_frames != 0U)
  {
    g_wrist_health_enter_frames--;
  }
  if (g_wrist_health_exit_frames != 0U)
  {
    g_wrist_health_exit_frames--;
  }
  taskEXIT_CRITICAL();

  frame->seq = seq;
  frame->tick = tick;
  frame->acc_mg[0] = FloatToI16Scaled(imu_raw->acc_g_x, 1000.0f);
  frame->acc_mg[1] = FloatToI16Scaled(imu_raw->acc_g_y, 1000.0f);
  frame->acc_mg[2] = FloatToI16Scaled(imu_raw->acc_g_z, 1000.0f);
  frame->gyro_dps_x10[0] = FloatToI16Scaled(imu_raw->gyro_dps_x, 10.0f);
  frame->gyro_dps_x10[1] = FloatToI16Scaled(imu_raw->gyro_dps_y, 10.0f);
  frame->gyro_dps_x10[2] = FloatToI16Scaled(imu_raw->gyro_dps_z, 10.0f);
  frame->angle_x100[0] = AngleToX100(euler->roll_deg);
  frame->angle_x100[1] = AngleToX100(euler->pitch_deg);
  frame->angle_x100[2] = AngleToX100(euler->yaw_deg);
  /* 闂傚倸鍊峰ù鍥綖婢跺顩插ù鐘差儏绾惧潡姊洪鈧粔鎾儗?MKS142 闂傚倸鍊搁…顒勫磻閸曨個娲Χ婢跺﹦鏌堥梺鍝勵槹椤戞瑩宕崨顓涙斀闁稿本纰嶉崯鐐烘煛閸滀礁澧柟渚垮妼椤粓宕卞Δ鈧粻铏圭磽娴ｅ搫校閻㈩垽绻濆濠氬焺閸愨晛顎撻梺鍛婄缚閸庨亶路閳ь剚绻濈喊澶岀？闁稿鐩畷鎰板捶椤撶喎搴婇梺鍛婄⊕閹矂寮崘顔界厪闊洦娲栧瓭濠?bpm 婵犲痉鏉库偓妤佹叏閻戣棄纾绘繛鎴炵懄濞呯娀骞栧ǎ顒€濡介柛?status bit1闂?*/
  frame->heart_rate = 0U;
  frame->status = (uint8_t)(imu_valid != 0U ? WIRELESS_STATUS_IMU_VALID : 0x00U);
  frame->battery_percent = battery_percent;
  frame->status2 = (uint8_t)((uart_ok != 0U ? WIRELESS_STATUS2_BT_OK : 0x00U)
                           | (sleeping != 0U ? WIRELESS_STATUS2_SLEEPING : 0x00U)
                           | (battery_valid != 0U ? WIRELESS_STATUS2_BAT_VALID : 0x00U)
                           | (health_mode != 0U ? WIRELESS_STATUS2_HEALTH_MODE : 0x00U)
                           | (health_exit_event != 0U ? WIRELESS_STATUS2_HEALTH_EXIT : 0x00U)
                           | (health_enter_event != 0U ? WIRELESS_STATUS2_HEALTH_ENTER : 0x00U));
}

#define WRIST_TXT_TITLE       "\xE6""\x99""\xBA""\xE8""\x83""\xBD""\xE5""\xBA""\xB7""\xE5""\xA4""\x8D""\xE8""\xAF""\x84""\xE4""\xBC""\xB0""\xE8""\x87""\x82""\xE5""\xB8""\xA6"
#define WRIST_TXT_BT          "\xE8""\x93""\x9D""\xE7""\x89""\x99"
#define WRIST_TXT_TRAINING    "\xE8""\xAE""\xAD""\xE7""\xBB""\x83""\xE6""\xA8""\xA1""\xE5""\xBC""\x8F"
#define WRIST_TXT_REHAB       "\xE5""\xBC""\x80""\xE5""\xA7""\x8B""\xE5""\xBA""\xB7""\xE5""\xA4""\x8D""\xE8""\xAE""\xAD""\xE7""\xBB""\x83"
#define WRIST_TXT_HEART       "\xE5""\xBF""\x83""\xE7""\x8E""\x87"
#define WRIST_TXT_REALTIME    "\xE5""\xBC""\x80""\xE5""\xA7""\x8B""\xE5""\xAE""\x9E""\xE6""\x97""\xB6""\xE7""\x9B""\x91""\xE6""\xB5""\x8B"
#define WRIST_TXT_CONNECTED   "\xE5""\xB7""\xB2""\xE8""\xBF""\x9E""\xE6""\x8E""\xA5"
#define WRIST_TXT_DISCONNECTED "\xE6""\x9C""\xAA""\xE8""\xBF""\x9E""\xE6""\x8E""\xA5"
#define WRIST_TXT_HEALTH_TITLE "\xE5""\x81""\xA5""\xE5""\xBA""\xB7""\xE7""\x9B""\x91""\xE6""\xB5""\x8B"
#define WRIST_TXT_SPO2        "\xE8""\xA1""\x80""\xE6""\xB0""\xA7"
#define WRIST_TXT_PULSE       "\xE8""\x84""\x89""\xE6""\x90""\x8F"
#define WRIST_TXT_MONITORING  "\xE5""\xAE""\x9E""\xE6""\x97""\xB6""\xE7""\x9B""\x91""\xE6""\xB5""\x8B""\xE4""\xB8""\xAD"
#define WRIST_TXT_WAIT_HEART  "\xE7""\xAD""\x89""\xE5""\xBE""\x85""\xE5""\xBF""\x83""\xE7""\x8E""\x87""\xE6""\x95""\xB0""\xE6""\x8D""\xAE"
#define WRIST_TXT_TRAINING_ACTIVE "\xE8""\xAE""\xAD""\xE7""\xBB""\x83""\xE4""\xB8""\xAD"
#define WRIST_TXT_TRAINING_NOW "\xE6""\xAD""\xA3""\xE5""\x9C""\xA8""\xE8""\xAE""\xAD""\xE7""\xBB""\x83"
#define WRIST_TXT_FOLLOW_ARM  "\xE8""\xAF""\xB7""\xE8""\xB7""\x9F""\xE9""\x9A""\x8F""\xE4""\xB8""\x8A""\xE8""\x87""\x82""\xE7""\xAB""\xAF"
#define WRIST_TXT_FINISH_ACTION "\xE5""\xAE""\x8C""\xE6""\x88""\x90""\xE8""\xAE""\xAD""\xE7""\xBB""\x83""\xE5""\x8A""\xA8""\xE4""\xBD""\x9C"
#define WRIST_TXT_KEEP_POSTURE "\xE4""\xBF""\x9D""\xE6""\x8C""\x81""\xE6""\xAD""\xA3""\xE7""\xA1""\xAE""\xE5""\xA7""\xBF""\xE5""\x8A""\xBF"
#define WRIST_TXT_BT_DISCONNECTED "\xE8""\x93""\x9D""\xE7""\x89""\x99""\xE6""\x9C""\xAA""\xE8""\xBF""\x9E""\xE6""\x8E""\xA5"
#define WRIST_TXT_CONNECT_ARM "\xE8""\xAF""\xB7""\xE5""\x85""\x88""\xE8""\xBF""\x9E""\xE6""\x8E""\xA5""\xE4""\xB8""\x8A""\xE8""\x87""\x82""\xE7""\xAB""\xAF""\xE8""\xAE""\xBE""\xE5""\xA4""\x87"
#define WRIST_TXT_BACK_HOME "\xE8""\xBF""\x94""\xE5""\x9B""\x9E""\xE4""\xB8""\xBB""\xE9""\xA1""\xB5"
#define WRIST_TXT_WAIT_ARM "\xE7""\xAD""\x89""\xE5""\xBE""\x85""\xE4""\xB8""\x8A""\xE8""\x87""\x82""\xE7""\xAB""\xAF"
#define WRIST_TXT_SELECT_ACTION "\xE8""\xAF""\xB7""\xE5""\x9C""\xA8""\xE4""\xB8""\x8A""\xE8""\x87""\x82""\xE7""\xAB""\xAF""\xE9""\x80""\x89""\xE6""\x8B""\xA9""\xE8""\xAE""\xAD""\xE7""\xBB""\x83""\xE5""\x8A""\xA8""\xE4""\xBD""\x9C"
#define WRIST_TXT_AUTO_START "\xE9""\x80""\x89""\xE6""\x8B""\xA9""\xE5""\xAE""\x8C""\xE6""\x88""\x90""\xE5""\x90""\x8E""\xE4""\xBC""\x9A""\xE8""\x87""\xAA""\xE5""\x8A""\xA8""\xE5""\xBC""\x80""\xE5""\xA7""\x8B"

static lv_color_t wrist_ui_bg_color(void)
{
  return (WristSettings_IsNightMode() != 0U) ? lv_color_hex(0x071426) : lv_color_hex(0xF4FAFF);
}

static lv_color_t wrist_ui_bg_grad_color(void)
{
  return (WristSettings_IsNightMode() != 0U) ? lv_color_hex(0x0B2038) : lv_color_hex(0xFFFFFF);
}

static lv_color_t wrist_ui_surface_color(void)
{
  return (WristSettings_IsNightMode() != 0U) ? lv_color_hex(0x10233A) : lv_color_hex(0xFFFFFF);
}

static lv_color_t wrist_ui_pressed_surface_color(void)
{
  return (WristSettings_IsNightMode() != 0U) ? lv_color_hex(0x17365A) : lv_color_hex(0xEAF4FF);
}

static lv_color_t wrist_ui_text_color(void)
{
  return (WristSettings_IsNightMode() != 0U) ? lv_color_hex(0xF7FBFF) : lv_color_hex(0x102B6B);
}

static lv_color_t wrist_ui_muted_text_color(void)
{
  return (WristSettings_IsNightMode() != 0U) ? lv_color_hex(0x9DB0C9) : lv_color_hex(0x687894);
}

static lv_color_t wrist_ui_border_color(void)
{
  return (WristSettings_IsNightMode() != 0U) ? lv_color_hex(0x245C96) : lv_color_hex(0xDCEBFF);
}

static lv_color_t wrist_ui_shadow_color(void)
{
  return (WristSettings_IsNightMode() != 0U) ? lv_color_hex(0x000000) : lv_color_hex(0xC7DDF8);
}

static lv_color_t wrist_ui_blue_soft_color(void)
{
  return (WristSettings_IsNightMode() != 0U) ? lv_color_hex(0x123A64) : lv_color_hex(0xEAF4FF);
}

static lv_color_t wrist_ui_red_soft_color(void)
{
  return (WristSettings_IsNightMode() != 0U) ? lv_color_hex(0x4A1B2A) : lv_color_hex(0xFFECEF);
}

static lv_color_t wrist_ui_bt_off_color(void)
{
  return (WristSettings_IsNightMode() != 0U) ? lv_color_hex(0x64748B) : lv_color_hex(0x9AA8BD);
}

typedef struct {
  lv_obj_t *footer_label;
  lv_obj_t *bt_icon;
  lv_obj_t *battery_label;
  lv_obj_t *battery_fill;
  WristSettingsView_t settings;
  uint8_t wireless_connected;
  uint8_t last_battery_percent;
} WristHomeView_t;

typedef struct {
  lv_obj_t *hr_value_label;
  lv_obj_t *spo2_value_label;
  lv_obj_t *status_label;
  lv_obj_t *status_dot;
  lv_obj_t *trend_chart;
  lv_chart_series_t *trend_series;
  int8_t trend_wave[MKS142_OFF_WAVE_LEN];
  uint8_t trend_has_sample;
  uint8_t trend_blend_steps;
  uint8_t pulse_phase;
  uint8_t trend_draw_index;
  int8_t trend_prev_sample;
  int8_t trend_last_sample;
  int8_t trend_blend_from;
  int16_t trend_low;
  int16_t trend_high;
  uint8_t trend_scale_ready;
  uint32_t pulse_noise_state;
  uint32_t last_trend_events;
  uint32_t last_trend_step_tick;
  uint32_t last_heart_events;
  uint8_t last_heart_valid;
  uint8_t last_heart_rate;
  uint8_t last_spo2;
  uint8_t last_blink_phase;
  uint8_t last_dot_valid;
} WristHealthView_t;

typedef struct {
  uint32_t magic;
  uint32_t seq;
  uint32_t percent;
  uint32_t crc;
} WristBatteryStoreRecord_t;

typedef struct {
  lv_obj_t *pulse_dot_1;
  lv_obj_t *pulse_dot_2;
  lv_obj_t *pulse_dot_3;
  lv_obj_t *arm_line;
  lv_point_t arm_points[4];
  uint8_t last_phase;
} WristTrainingView_t;

typedef enum {
  WRIST_HOME_ICON_TRAINING = 0,
  WRIST_HOME_ICON_HEART
} WristHomeIcon_t;

typedef enum {
  WRIST_UI_PAGE_HOME = 0,
  WRIST_UI_PAGE_HEALTH,
  WRIST_UI_PAGE_TRAINING,
  WRIST_UI_PAGE_LINK_LOST,
  WRIST_UI_PAGE_WAIT_ARM
} WristUiPage_t;

typedef struct {
  WristUiPage_t page;
  WristHomeView_t home;
  WristHealthView_t health;
  WristTrainingView_t training;
} WristUiContext_t;

#define WRIST_HOME_HEART_CANVAS_W  52
#define WRIST_HOME_HEART_CANVAS_H  52
#define WRIST_WIRELESS_CONNECTED_TIMEOUT_MS  (30000U)
#define WRIST_WIRELESS_CONNECT_LATCH_MS      (300U)
#define WRIST_WIRELESS_DISCONNECT_LATCH_MS   (3000U)
#define WIRELESS_ACK_BYTE0                   (0xA5U)
#define WIRELESS_ACK_BYTE1                   (0x5AU)
#define WIRELESS_LINK_MSG_BYTE2              (0xC3U)
#define WIRELESS_LINK_MSG_BYTE3              (0x3CU)
#define WIRELESS_TRAIN_MSG_BYTE2             (0xD4U)
#define WIRELESS_TRAIN_MSG_BYTE3             (0x4DU)
#define WIRELESS_HOME_MSG_BYTE2              (0xE5U)
#define WIRELESS_HOME_MSG_BYTE3              (0x5EU)
#define WIRELESS_HEALTH_MSG_BYTE2            (0xF6U)
#define WIRELESS_HEALTH_MSG_BYTE3            (0x6FU)
#define WRIST_KEY_EVENT_SHORT                (0x01U)
#define WRIST_KEY_EVENT_LONG                 (0x02U)

static volatile uint8_t g_wrist_train_start_request;
static volatile uint8_t g_wrist_home_request;
static volatile uint8_t g_wrist_health_start_request;
static volatile uint8_t g_wrist_force_refresh_request;
static volatile uint8_t g_wrist_training_mode_active;
static uint8_t s_wireless_ack_rx_byte;
static uint8_t s_wireless_ack_rx_state;
static GPIO_PinState s_user_key_stable_state = GPIO_PIN_SET;
static GPIO_PinState s_user_key_last_raw_state = GPIO_PIN_SET;
static uint32_t s_user_key_last_change_tick;
static uint32_t s_user_key_press_start_tick;
static uint8_t s_user_key_long_sent;
static GPIO_PinState s_setting_key_stable_state = GPIO_PIN_SET;
static GPIO_PinState s_setting_key_last_raw_state = GPIO_PIN_SET;
static uint32_t s_setting_key_last_change_tick;
static uint32_t s_setting_key_press_start_tick;
static uint8_t s_setting_key_long_sent;
static void wrist_home_create(WristUiContext_t *ui);
static void wrist_health_create(WristUiContext_t *ui);
static void wrist_training_create(WristUiContext_t *ui);
static void wrist_link_lost_create(WristUiContext_t *ui);
static void wrist_wait_arm_create(WristUiContext_t *ui);
static void wrist_ui_request_force_refresh(void);
static void wrist_ui_wait_lcd_idle(void);
static void wrist_ui_force_full_refresh(void);
static uint8_t wrist_home_update_wireless_status(WristHomeView_t *view, uint32_t now);
static uint8_t wrist_home_update_battery(WristHomeView_t *view);
static uint8_t wrist_battery_load_percent(uint8_t *percent);
static uint8_t wrist_setting_key_scan(uint32_t now);
static void wrist_setting_key_reset(uint32_t now);
static void wrist_heart_request_start(void);
static uint8_t wrist_user_key_scan(uint32_t now);
static void wrist_user_key_reset(uint32_t now);
static uint8_t wrist_sleep_is_active(void);
LV_IMG_DECLARE(wrist_training_icon);
LV_IMG_DECLARE(wrist_boot_logo);

#define WRIST_BOOT_LOGO_DURATION_MS   (1000U)

static uint8_t wrist_label_set_text_if_changed(lv_obj_t *label, const char *text)
{
  const char *current;

  if ((label == NULL) || (text == NULL))
  {
    return 0U;
  }

  current = lv_label_get_text(label);
  if ((current == NULL) || (strcmp(current, text) != 0))
  {
    lv_label_set_text(label, text);
    lv_obj_invalidate(label);
    if (lv_obj_get_parent(label) != NULL)
    {
      lv_obj_invalidate(lv_obj_get_parent(label));
    }
    return 1U;
  }

  return 0U;
}

static void wrist_boot_create(void)
{
  lv_obj_t *screen = lv_scr_act();
  lv_obj_t *img;

  lv_obj_clean(screen);
  lv_obj_remove_style_all(screen);
  lv_obj_set_style_bg_color(screen, lv_color_white(), 0);
  lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
  lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

  img = lv_img_create(screen);
  lv_img_set_src(img, &wrist_boot_logo);
  lv_obj_center(img);
}

static void wrist_boot_hold(uint32_t duration_ms)
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

static uint8_t wrist_label_set_u8_or_dash(lv_obj_t *label, uint8_t value)
{
  char text[4];

  if (value == 0U)
  {
    return wrist_label_set_text_if_changed(label, "--");
  }
  else
  {
    (void)snprintf(text, sizeof(text), "%u", (unsigned int)value);
    return wrist_label_set_text_if_changed(label, text);
  }
}

static uint32_t wrist_battery_store_crc(const WristBatteryStoreRecord_t *record)
{
  return record->magic ^ record->seq ^ record->percent ^ WRIST_BATTERY_STORE_CRC_XOR;
}

static uint8_t wrist_battery_store_is_valid(const WristBatteryStoreRecord_t *record)
{
  return ((record->magic == WRIST_BATTERY_STORE_MAGIC) &&
          (record->percent <= 100U) &&
          (record->crc == wrist_battery_store_crc(record))) ? 1U : 0U;
}

static uint8_t wrist_battery_load_percent(uint8_t *percent)
{
  const WristBatteryStoreRecord_t *records =
      (const WristBatteryStoreRecord_t *)WRIST_BATTERY_STORE_ADDR;
  uint32_t i;
  uint8_t found = 0U;

  if (percent == NULL)
  {
    return 0U;
  }

  for (i = 0U; i < (FLASH_PAGE_SIZE / sizeof(WristBatteryStoreRecord_t)); i++)
  {
    if (wrist_battery_store_is_valid(&records[i]) != 0U)
    {
      s_battery_store_seq = records[i].seq;
      s_battery_percent = (uint8_t)records[i].percent;
      found = 1U;
    }
    else if (records[i].magic == 0xFFFFFFFFU)
    {
      break;
    }
  }

  s_battery_percent_valid = found;
  if (found != 0U)
  {
    *percent = s_battery_percent;
  }

  return found;
}

static uint8_t wrist_battery_store_percent(uint8_t percent)
{
  const WristBatteryStoreRecord_t *records =
      (const WristBatteryStoreRecord_t *)WRIST_BATTERY_STORE_ADDR;
  WristBatteryStoreRecord_t record;
  FLASH_EraseInitTypeDef erase;
  uint32_t page_error = 0U;
  uint32_t slot;
  HAL_StatusTypeDef status;

  if (percent > 100U)
  {
    return 0U;
  }

  for (slot = 0U; slot < (FLASH_PAGE_SIZE / sizeof(WristBatteryStoreRecord_t)); slot++)
  {
    if (records[slot].magic == 0xFFFFFFFFU)
    {
      break;
    }
  }

  status = HAL_FLASH_Unlock();
  if (status != HAL_OK)
  {
    return 0U;
  }

  if (slot >= (FLASH_PAGE_SIZE / sizeof(WristBatteryStoreRecord_t)))
  {
    erase.TypeErase = FLASH_TYPEERASE_PAGES;
    erase.Banks = WRIST_BATTERY_STORE_BANK;
    erase.Page = WRIST_BATTERY_STORE_PAGE;
    erase.NbPages = 1U;
    if (HAL_FLASHEx_Erase(&erase, &page_error) != HAL_OK)
    {
      (void)HAL_FLASH_Lock();
      return 0U;
    }
    slot = 0U;
  }

  record.magic = WRIST_BATTERY_STORE_MAGIC;
  record.seq = s_battery_store_seq + 1U;
  record.percent = percent;
  record.crc = wrist_battery_store_crc(&record);

  status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD,
                             WRIST_BATTERY_STORE_ADDR + (slot * sizeof(record)),
                             (uint32_t)&record);
  (void)HAL_FLASH_Lock();

  if (status == HAL_OK)
  {
    s_battery_store_seq = record.seq;
    return 1U;
  }

  return 0U;
}

static uint8_t wrist_battery_filter_percent(uint8_t measured_percent, uint8_t *display_percent)
{
  uint8_t diff;

  if (display_percent == NULL)
  {
    return 0U;
  }

  if (s_battery_percent_valid == 0U)
  {
    s_battery_percent = measured_percent;
    s_battery_percent_valid = 1U;
    (void)wrist_battery_store_percent(measured_percent);
    *display_percent = measured_percent;
    return 1U;
  }

  if (measured_percent > s_battery_percent)
  {
    diff = (uint8_t)(measured_percent - s_battery_percent);
    if (diff <= WRIST_BATTERY_RISE_NOISE_MAX)
    {
      *display_percent = s_battery_percent;
      return 0U;
    }
  }

  if (measured_percent == s_battery_percent)
  {
    *display_percent = s_battery_percent;
    return 0U;
  }

  s_battery_percent = measured_percent;
  (void)wrist_battery_store_percent(measured_percent);
  *display_percent = measured_percent;
  return 1U;
}

static uint8_t wrist_home_set_battery(WristHomeView_t *view, uint8_t percent)
{
  uint8_t fill_w;
  char text[5];

  if ((view == NULL) ||
      (view->battery_label == NULL) ||
      (view->battery_fill == NULL))
  {
    return 0U;
  }

  if (percent == view->last_battery_percent)
  {
    return 0U;
  }

  view->last_battery_percent = percent;
  (void)snprintf(text, sizeof(text), "%u%%", (unsigned int)percent);
  lv_label_set_text(view->battery_label, text);

  fill_w = (uint8_t)((17U * percent) / 100U);
  if ((percent > 0U) && (fill_w == 0U))
  {
    fill_w = 1U;
  }
  lv_obj_set_width(view->battery_fill, fill_w);
  if (percent < 20U)
  {
    lv_obj_set_style_bg_color(view->battery_fill, lv_color_hex(0xF24E5E), 0);
  }
  else if (percent < 50U)
  {
    lv_obj_set_style_bg_color(view->battery_fill, lv_color_hex(0xF2C94C), 0);
  }
  else
  {
    lv_obj_set_style_bg_color(view->battery_fill, lv_color_hex(0x28D95B), 0);
  }

  lv_obj_invalidate(view->battery_label);
  lv_obj_invalidate(view->battery_fill);
  return 1U;
}

static uint8_t wrist_battery_read(uint16_t *battery_mv, uint8_t *percent)
{
  uint32_t raw_sum = 0U;
  uint32_t adc_mv;
  uint32_t mv;
  uint32_t pct;
  uint8_t samples = 0U;

  if ((battery_mv == NULL) || (percent == NULL))
  {
    return 0U;
  }

  if (hadc1.State != HAL_ADC_STATE_READY)
  {
    (void)HAL_ADC_Stop(&hadc1);
  }

  if (HAL_ADC_Start(&hadc1) == HAL_OK)
  {
    if (HAL_ADC_PollForConversion(&hadc1, 2U) == HAL_OK)
    {
      raw_sum = HAL_ADC_GetValue(&hadc1);
      samples = 1U;
    }
    (void)HAL_ADC_Stop(&hadc1);
  }

  if (samples == 0U)
  {
    return 0U;
  }

  adc_mv = ((raw_sum / samples) * WRIST_BATTERY_ADC_VREF_MV) / WRIST_BATTERY_ADC_MAX;
  mv = (adc_mv * WRIST_BATTERY_DIVIDER_NUM) / WRIST_BATTERY_DIVIDER_DEN;
  if (mv > 65535U)
  {
    mv = 65535U;
  }

  if (mv <= WRIST_BATTERY_EMPTY_MV)
  {
    pct = 0U;
  }
  else if (mv >= WRIST_BATTERY_FULL_MV)
  {
    pct = 100U;
  }
  else
  {
    pct = ((mv - WRIST_BATTERY_EMPTY_MV) * 100U) /
          (WRIST_BATTERY_FULL_MV - WRIST_BATTERY_EMPTY_MV);
  }

  *battery_mv = (uint16_t)mv;
  *percent = (uint8_t)pct;
  return 1U;
}

static uint8_t wrist_home_update_battery(WristHomeView_t *view)
{
  uint16_t battery_mv;
  uint8_t measured_percent;
  uint8_t display_percent;
  uint8_t changed;

  if ((view == NULL) ||
      (view->battery_label == NULL) ||
      (view->battery_fill == NULL) ||
      (wrist_battery_read(&battery_mv, &measured_percent) == 0U))
  {
    return 0U;
  }

  (void)battery_mv;
  changed = wrist_battery_filter_percent(measured_percent, &display_percent);
  if (wrist_home_set_battery(view, display_percent) != 0U)
  {
    changed = 1U;
  }

  return changed;
}

static void wrist_home_init_styles(lv_style_t *screen,
                                   lv_style_t *title,
                                   lv_style_t *status,
                                   lv_style_t *card,
                                   lv_style_t *card_blue_pressed,
                                   lv_style_t *card_red_pressed,
                                   lv_style_t *card_blue,
                                   lv_style_t *card_red,
                                   lv_style_t *icon_blue,
                                   lv_style_t *icon_red,
                                   lv_style_t *card_title,
                                   lv_style_t *card_subtitle,
                                   lv_style_t *blue_text,
                                   lv_style_t *red_text,
                                   lv_style_t *footer)
{
  lv_style_reset(screen);
  lv_style_set_bg_color(screen, wrist_ui_bg_color());
  lv_style_set_bg_opa(screen, LV_OPA_COVER);
  lv_style_set_bg_grad_color(screen, wrist_ui_bg_grad_color());
  lv_style_set_bg_grad_dir(screen, LV_GRAD_DIR_VER);

  lv_style_reset(title);
  lv_style_set_text_color(title, wrist_ui_text_color());
  lv_style_set_text_font(title, &wrist_font_zh_16);

  lv_style_reset(status);
  lv_style_set_text_color(status, wrist_ui_muted_text_color());
  lv_style_set_text_font(status, &wrist_font_zh_16);

  lv_style_reset(card);
  lv_style_set_radius(card, 18);
  lv_style_set_border_width(card, 1);
  lv_style_set_shadow_width(card, 8);
  lv_style_set_shadow_opa(card, LV_OPA_20);
  lv_style_set_shadow_ofs_y(card, 4);

  lv_style_reset(card_blue_pressed);
  lv_style_set_bg_color(card_blue_pressed, wrist_ui_pressed_surface_color());
  lv_style_set_bg_opa(card_blue_pressed, LV_OPA_COVER);
  lv_style_set_border_color(card_blue_pressed, lv_color_hex(0x005BEA));
  lv_style_set_shadow_width(card_blue_pressed, 3);
  lv_style_set_shadow_opa(card_blue_pressed, LV_OPA_10);
  lv_style_set_shadow_ofs_y(card_blue_pressed, 1);

  lv_style_reset(card_red_pressed);
  lv_style_set_bg_color(card_red_pressed, wrist_ui_pressed_surface_color());
  lv_style_set_bg_opa(card_red_pressed, LV_OPA_COVER);
  lv_style_set_border_color(card_red_pressed, lv_color_hex(0xD92D45));
  lv_style_set_shadow_width(card_red_pressed, 3);
  lv_style_set_shadow_opa(card_red_pressed, LV_OPA_10);
  lv_style_set_shadow_ofs_y(card_red_pressed, 1);

  lv_style_reset(card_blue);
  lv_style_set_bg_color(card_blue, wrist_ui_surface_color());
  lv_style_set_bg_opa(card_blue, LV_OPA_COVER);
  lv_style_set_border_color(card_blue, lv_color_hex(0x95C3FF));
  lv_style_set_shadow_color(card_blue, wrist_ui_shadow_color());

  lv_style_reset(card_red);
  lv_style_set_bg_color(card_red, wrist_ui_surface_color());
  lv_style_set_bg_opa(card_red, LV_OPA_COVER);
  lv_style_set_border_color(card_red, lv_color_hex(0xFFC3CC));
  lv_style_set_shadow_color(card_red, wrist_ui_shadow_color());

  lv_style_reset(icon_blue);
  lv_style_set_bg_color(icon_blue, wrist_ui_blue_soft_color());
  lv_style_set_bg_opa(icon_blue, LV_OPA_COVER);
  lv_style_set_radius(icon_blue, LV_RADIUS_CIRCLE);
  lv_style_set_border_width(icon_blue, 0);

  lv_style_reset(icon_red);
  lv_style_set_bg_color(icon_red, wrist_ui_red_soft_color());
  lv_style_set_bg_opa(icon_red, LV_OPA_COVER);
  lv_style_set_radius(icon_red, LV_RADIUS_CIRCLE);
  lv_style_set_border_width(icon_red, 0);

  lv_style_reset(card_title);
  lv_style_set_text_color(card_title, wrist_ui_text_color());
  lv_style_set_text_font(card_title, &wrist_font_zh_16);

  lv_style_reset(card_subtitle);
  lv_style_set_text_color(card_subtitle, wrist_ui_muted_text_color());
  lv_style_set_text_font(card_subtitle, &wrist_font_zh_14);

  lv_style_reset(blue_text);
  lv_style_set_text_color(blue_text, lv_color_hex(0x1877F2));
  lv_style_set_text_font(blue_text, &lv_font_montserrat_14);

  lv_style_reset(red_text);
  lv_style_set_text_color(red_text, lv_color_hex(0xF24E5E));
  lv_style_set_text_font(red_text, &lv_font_montserrat_14);

  lv_style_reset(footer);
  lv_style_set_text_color(footer, wrist_ui_text_color());
  lv_style_set_text_font(footer, &wrist_font_zh_16);
}

static void wrist_ui_request_force_refresh(void)
{
  g_wrist_force_refresh_request = 1U;
}

static void wrist_ui_page_fade_in(lv_obj_t *screen)
{
  if (screen == NULL)
  {
    return;
  }

  lv_anim_del(screen, NULL);
  lv_obj_set_style_opa(screen, LV_OPA_TRANSP, 0);
  lv_obj_fade_in(screen, 160U, 0U);
}

static void wrist_ui_force_full_refresh(void)
{
  lv_obj_invalidate(lv_scr_act());
  LCD_ProcessDmaTransfer();
  lv_refr_now(NULL);
  LCD_ProcessDmaTransfer();
  wrist_ui_wait_lcd_idle();
}

static uint8_t wrist_sleep_is_active(void)
{
  uint8_t sleeping;

  taskENTER_CRITICAL();
  sleeping = g_wrist_sleeping;
  taskEXIT_CRITICAL();

  return sleeping;
}

static void wrist_heart_request_start(void)
{
  taskENTER_CRITICAL();
  g_heart_valid = 0U;
  g_heart_data.heart_rate = 0U;
  g_heart_data.spo2 = 0U;
  g_screen_heart_rate = 0U;
  g_screen_spo2 = 0U;
  g_heart_start_request = 1U;
  taskEXIT_CRITICAL();
}

static uint8_t wrist_user_key_scan(uint32_t now)
{
  GPIO_PinState raw_state = HAL_GPIO_ReadPin(USER_KEY_GPIO_Port, USER_KEY_Pin);
  uint8_t event = 0U;

  if (raw_state != s_user_key_last_raw_state)
  {
    s_user_key_last_raw_state = raw_state;
    s_user_key_last_change_tick = now;
  }

  if ((now - s_user_key_last_change_tick) >= WRIST_USER_KEY_DEBOUNCE_MS)
  {
    if (s_user_key_stable_state != raw_state)
    {
      s_user_key_stable_state = raw_state;
      if (s_user_key_stable_state == GPIO_PIN_RESET)
      {
        s_user_key_press_start_tick = now;
        s_user_key_long_sent = 0U;
      }
      else
      {
        if (s_user_key_long_sent == 0U)
        {
          event |= WRIST_KEY_EVENT_SHORT;
        }
      }
    }
  }

  if ((s_user_key_stable_state == GPIO_PIN_RESET) &&
      (s_user_key_long_sent == 0U) &&
      ((now - s_user_key_press_start_tick) >= WRIST_USER_KEY_LONG_MS))
  {
    s_user_key_long_sent = 1U;
    event |= WRIST_KEY_EVENT_LONG;
  }

  return event;
}

static void wrist_user_key_reset(uint32_t now)
{
  GPIO_PinState raw_state = HAL_GPIO_ReadPin(USER_KEY_GPIO_Port, USER_KEY_Pin);

  s_user_key_stable_state = raw_state;
  s_user_key_last_raw_state = raw_state;
  s_user_key_last_change_tick = now;
  s_user_key_press_start_tick = now;
  s_user_key_long_sent = (raw_state == GPIO_PIN_RESET) ? 1U : 0U;
}

static uint8_t wrist_setting_key_scan(uint32_t now)
{
  GPIO_PinState raw_state = HAL_GPIO_ReadPin(SETTING_KEY_GPIO_Port, SETTING_KEY_Pin);
  uint8_t event = 0U;

  if (raw_state != s_setting_key_last_raw_state)
  {
    s_setting_key_last_raw_state = raw_state;
    s_setting_key_last_change_tick = now;
  }

  if ((now - s_setting_key_last_change_tick) >= WRIST_USER_KEY_DEBOUNCE_MS)
  {
    if (s_setting_key_stable_state != raw_state)
    {
      s_setting_key_stable_state = raw_state;
      if (s_setting_key_stable_state == GPIO_PIN_RESET)
      {
        s_setting_key_press_start_tick = now;
        s_setting_key_long_sent = 0U;
      }
      else
      {
        if (s_setting_key_long_sent == 0U)
        {
          event |= WRIST_KEY_EVENT_SHORT;
        }
      }
    }
  }

  if ((s_setting_key_stable_state == GPIO_PIN_RESET) &&
      (s_setting_key_long_sent == 0U) &&
      ((now - s_setting_key_press_start_tick) >= WRIST_USER_KEY_LONG_MS))
  {
    s_setting_key_long_sent = 1U;
    event |= WRIST_KEY_EVENT_LONG;
  }

  return event;
}

static void wrist_setting_key_reset(uint32_t now)
{
  GPIO_PinState raw_state = HAL_GPIO_ReadPin(SETTING_KEY_GPIO_Port, SETTING_KEY_Pin);

  s_setting_key_stable_state = raw_state;
  s_setting_key_last_raw_state = raw_state;
  s_setting_key_last_change_tick = now;
  s_setting_key_press_start_tick = now;
  s_setting_key_long_sent = (raw_state == GPIO_PIN_RESET) ? 1U : 0U;
}

static void wrist_ui_invalidate_obj_full(lv_obj_t *obj, lv_coord_t extra)
{
  lv_area_t area;

  if (obj == NULL)
  {
    return;
  }

  lv_obj_get_coords(obj, &area);
  area.x1 -= extra;
  area.y1 -= extra;
  area.x2 += extra;
  area.y2 += extra;
  lv_obj_invalidate_area(obj, &area);
}
static lv_obj_t *wrist_home_icon_part(lv_obj_t *parent,
                                      lv_coord_t w,
                                      lv_coord_t h,
                                      lv_color_t color,
                                      lv_coord_t radius)
{
  lv_obj_t *part = lv_obj_create(parent);
  lv_obj_remove_style_all(part);
  lv_obj_set_style_bg_color(part, color, 0);
  lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(part, 0, 0);
  lv_obj_set_style_radius(part, radius, 0);
  lv_obj_set_size(part, w, h);
  return part;
}

static void wrist_home_create_training_icon(lv_obj_t *parent)
{
  const lv_color_t blue = lv_color_hex(0x1877F2);
  lv_obj_t *bar = wrist_home_icon_part(parent, 30, 7, blue, 4);
  lv_obj_t *left = wrist_home_icon_part(parent, 10, 32, blue, 6);
  lv_obj_t *right = wrist_home_icon_part(parent, 10, 32, blue, 6);
  lv_obj_t *left_cap = wrist_home_icon_part(parent, 5, 20, blue, 3);
  lv_obj_t *right_cap = wrist_home_icon_part(parent, 5, 20, blue, 3);

  lv_obj_align(bar, LV_ALIGN_CENTER, 0, 0);
  lv_obj_align(left, LV_ALIGN_CENTER, -17, 0);
  lv_obj_align(right, LV_ALIGN_CENTER, 17, 0);
  lv_obj_align(left_cap, LV_ALIGN_CENTER, -27, 0);
  lv_obj_align(right_cap, LV_ALIGN_CENTER, 27, 0);
}

static uint8_t wrist_home_point_in_polygon(const lv_point_t *points,
                                           size_t point_count,
                                           lv_coord_t px,
                                           lv_coord_t py)
{
  uint8_t inside = 0U;
  size_t i;
  size_t j = point_count - 1U;

  for (i = 0U; i < point_count; i++)
  {
    const int32_t xi = points[i].x;
    const int32_t yi = points[i].y;
    const int32_t xj = points[j].x;
    const int32_t yj = points[j].y;

    if (((yi > py) != (yj > py)) &&
        (px < (lv_coord_t)(((xj - xi) * ((int32_t)py - yi) / (yj - yi)) + xi)))
    {
      inside = (inside == 0U) ? 1U : 0U;
    }
    j = i;
  }

  return inside;
}

static void wrist_home_create_heart_icon(lv_obj_t *parent)
{
  static uint8_t heart_buf[LV_CANVAS_BUF_SIZE_TRUE_COLOR_ALPHA(WRIST_HOME_HEART_CANVAS_W,
                                                               WRIST_HOME_HEART_CANVAS_H)];
  static const lv_point_t heart_points[] = {
    {26, 20}, {27, 17}, {28, 16}, {29, 15}, {31, 13}, {33, 12},
    {35, 12}, {37, 12}, {40, 13}, {42, 14}, {44, 16}, {45, 18},
    {46, 20}, {46, 22}, {46, 24}, {45, 27}, {44, 29}, {42, 31},
    {40, 33}, {37, 35}, {35, 37}, {33, 39}, {31, 41}, {29, 43},
    {28, 44}, {27, 45}, {26, 48}, {25, 45}, {24, 44}, {23, 43},
    {21, 41}, {19, 39}, {17, 37}, {15, 35}, {12, 33}, {10, 31},
    {8, 29}, {7, 27}, {6, 24}, {6, 22}, {6, 20}, {7, 18},
    {8, 16}, {10, 14}, {12, 13}, {15, 12}, {17, 12}, {19, 12},
    {21, 13}, {23, 15}, {24, 16}, {25, 17}
  };
  static const lv_point_t ecg_points[] = {
    {7, 28}, {16, 28}, {21, 21}, {27, 37}, {33, 28}, {45, 28}
  };
  const lv_color_t red = lv_color_hex(0xF24E5E);
  lv_obj_t *canvas = lv_canvas_create(parent);
  lv_coord_t x;
  lv_coord_t y;
  size_t i;

  lv_canvas_set_buffer(canvas, heart_buf,
                       WRIST_HOME_HEART_CANVAS_W,
                       WRIST_HOME_HEART_CANVAS_H,
                       LV_IMG_CF_TRUE_COLOR_ALPHA);
  lv_canvas_fill_bg(canvas, lv_color_hex(0x000000), LV_OPA_TRANSP);

  for (y = 0; y < WRIST_HOME_HEART_CANVAS_H; y++)
  {
    for (x = 0; x < WRIST_HOME_HEART_CANVAS_W; x++)
    {
      if (wrist_home_point_in_polygon(heart_points,
                                      sizeof(heart_points) / sizeof(heart_points[0]),
                                      x,
                                      y) != 0U)
      {
        lv_canvas_set_px_color(canvas, x, y, red);
        lv_canvas_set_px_opa(canvas, x, y, LV_OPA_COVER);
      }
    }
  }

  for (i = 0; i < (sizeof(ecg_points) / sizeof(ecg_points[0])) - 1U; i++)
  {
    lv_coord_t min_x = LV_MIN(ecg_points[i].x, ecg_points[i + 1U].x) - 2;
    lv_coord_t max_x = LV_MAX(ecg_points[i].x, ecg_points[i + 1U].x) + 2;
    lv_coord_t min_y = LV_MIN(ecg_points[i].y, ecg_points[i + 1U].y) - 2;
    lv_coord_t max_y = LV_MAX(ecg_points[i].y, ecg_points[i + 1U].y) + 2;
    const int32_t x1 = ecg_points[i].x;
    const int32_t y1 = ecg_points[i].y;
    const int32_t dx = ecg_points[i + 1U].x - x1;
    const int32_t dy = ecg_points[i + 1U].y - y1;
    const int32_t len2 = (dx * dx) + (dy * dy);

    if (min_x < 0) min_x = 0;
    if (min_y < 0) min_y = 0;
    if (max_x >= WRIST_HOME_HEART_CANVAS_W) max_x = WRIST_HOME_HEART_CANVAS_W - 1;
    if (max_y >= WRIST_HOME_HEART_CANVAS_H) max_y = WRIST_HOME_HEART_CANVAS_H - 1;

    for (y = min_y; y <= max_y; y++)
    {
      for (x = min_x; x <= max_x; x++)
      {
        const int32_t px = x - x1;
        const int32_t py = y - y1;
        int32_t t = (len2 != 0) ? (((px * dx) + (py * dy)) * 256 / len2) : 0;
        int32_t near_x;
        int32_t near_y;
        int32_t dist2;

        if (t < 0) t = 0;
        if (t > 256) t = 256;

        near_x = x1 + ((dx * t) / 256);
        near_y = y1 + ((dy * t) / 256);
        dist2 = ((x - near_x) * (x - near_x)) + ((y - near_y) * (y - near_y));
        if (dist2 <= 4)
        {
          lv_canvas_set_px_opa(canvas, x, y, LV_OPA_TRANSP);
        }
      }
    }
  }

  lv_obj_center(canvas);
}

static void wrist_home_create_icon(lv_obj_t *parent, WristHomeIcon_t icon)
{
  if (icon == WRIST_HOME_ICON_TRAINING)
  {
    wrist_home_create_training_icon(parent);
  }
  else
  {
    wrist_home_create_heart_icon(parent);
  }
}
typedef struct {
  lv_color_t normal_bg;
  lv_color_t normal_border;
  lv_color_t pressed_bg;
  lv_color_t pressed_border;
} WristHomeCardPressEffect_t;

typedef struct {
  lv_obj_t *card;
  const WristHomeCardPressEffect_t *effect;
} WristHomeCardPressState_t;

static void wrist_home_card_press_event_cb(lv_event_t *event)
{
  WristHomeCardPressState_t *state = (WristHomeCardPressState_t *)lv_event_get_user_data(event);
  lv_event_code_t code = lv_event_get_code(event);
  lv_obj_t *card;
  const WristHomeCardPressEffect_t *effect;

  if ((state == NULL) || (state->card == NULL) || (state->effect == NULL))
  {
    return;
  }

  card = state->card;
  effect = state->effect;

  if (code == LV_EVENT_PRESSED)
  {
    lv_obj_set_style_bg_color(card, effect->pressed_bg, 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(card, effect->pressed_border, 0);
    lv_obj_set_style_shadow_width(card, 3, 0);
    lv_obj_set_style_shadow_opa(card, LV_OPA_10, 0);
    lv_obj_set_style_shadow_ofs_y(card, 1, 0);
    wrist_ui_invalidate_obj_full(card, 10);
    lv_refr_now(NULL);
    wrist_ui_wait_lcd_idle();
  }
  else if (code == LV_EVENT_PRESS_LOST)
  {
    lv_obj_set_style_bg_color(card, effect->normal_bg, 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(card, effect->normal_border, 0);
    lv_obj_set_style_shadow_width(card, 8, 0);
    lv_obj_set_style_shadow_opa(card, LV_OPA_20, 0);
    lv_obj_set_style_shadow_ofs_y(card, 4, 0);
    wrist_ui_invalidate_obj_full(card, 10);
  }
}

static void WristVibration_PromptAsync(void);
static void wrist_home_create_card(lv_obj_t *parent,
                                   lv_style_t *card,
                                   lv_style_t *card_pressed,
                                   lv_style_t *card_color,
                                   lv_style_t *icon_style,
                                   lv_style_t *main_text,
                                   lv_style_t *sub_text,
                                   lv_style_t *accent_text,
                                   const char *title,
                                   const char *subtitle,
                                   WristHomeIcon_t icon,
                                   lv_coord_t y,
                                   lv_event_cb_t event_cb,
                                   void *event_user_data)
{
  static WristHomeCardPressEffect_t blue_effect;
  static WristHomeCardPressEffect_t red_effect;
  const WristHomeCardPressEffect_t *press_effect = (icon == WRIST_HOME_ICON_TRAINING) ? &blue_effect : &red_effect;
  static WristHomeCardPressState_t blue_press_state;
  static WristHomeCardPressState_t red_press_state;
  WristHomeCardPressState_t *press_state = (icon == WRIST_HOME_ICON_TRAINING) ? &blue_press_state : &red_press_state;
  lv_obj_t *btn = lv_obj_create(parent);

  blue_effect.normal_bg = wrist_ui_surface_color();
  blue_effect.normal_border = lv_color_hex(0x95C3FF);
  blue_effect.pressed_bg = wrist_ui_pressed_surface_color();
  blue_effect.pressed_border = lv_color_hex(0x005BEA);
  red_effect.normal_bg = wrist_ui_surface_color();
  red_effect.normal_border = lv_color_hex(0xFFC3CC);
  red_effect.pressed_bg = wrist_ui_pressed_surface_color();
  red_effect.pressed_border = lv_color_hex(0xD92D45);

  lv_obj_remove_style_all(btn);
  lv_obj_add_style(btn, card, 0);
  lv_obj_add_style(btn, card_color, 0);
  lv_obj_add_style(btn, card_pressed, LV_STATE_PRESSED);
  lv_obj_set_style_pad_all(btn, 0, 0);
  lv_obj_set_style_clip_corner(btn, true, 0);
  lv_obj_set_size(btn, 216, 86);
  lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, y);
  lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_flag(btn, LV_OBJ_FLAG_CLICKABLE);

  press_state->card = btn;
  press_state->effect = press_effect;

  lv_obj_t *icon_bg = lv_obj_create(btn);
  lv_obj_remove_style_all(icon_bg);
  lv_obj_add_style(icon_bg, icon_style, 0);
  lv_obj_clear_flag(icon_bg, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_set_size(icon_bg, 52, 52);
  lv_obj_align(icon_bg, LV_ALIGN_LEFT_MID, 13, 0);

  (void)accent_text;
  wrist_home_create_icon(icon_bg, icon);

  lv_obj_t *title_label = lv_label_create(btn);
  lv_obj_add_style(title_label, main_text, 0);
  lv_obj_clear_flag(title_label, LV_OBJ_FLAG_CLICKABLE);
  lv_label_set_text(title_label, title);
  lv_obj_align(title_label, LV_ALIGN_TOP_LEFT, 78, 25);

  lv_obj_t *sub_label = lv_label_create(btn);
  lv_obj_add_style(sub_label, sub_text, 0);
  lv_obj_clear_flag(sub_label, LV_OBJ_FLAG_CLICKABLE);
  lv_label_set_text(sub_label, subtitle);
  lv_obj_align(sub_label, LV_ALIGN_TOP_LEFT, 78, 52);

  lv_obj_t *arrow = lv_label_create(btn);
  lv_obj_add_style(arrow, accent_text, 0);
  lv_obj_clear_flag(arrow, LV_OBJ_FLAG_CLICKABLE);
  lv_label_set_text(arrow, ">");
  lv_obj_align(arrow, LV_ALIGN_RIGHT_MID, -17, 0);
  lv_obj_add_event_cb(btn, wrist_home_card_press_event_cb, LV_EVENT_PRESSED, press_state);
  lv_obj_add_event_cb(btn, wrist_home_card_press_event_cb, LV_EVENT_PRESS_LOST, press_state);
  if (event_cb != NULL)
  {
    lv_obj_add_event_cb(btn, event_cb, LV_EVENT_CLICKED, event_user_data);
  }
}


static uint8_t wrist_home_wireless_is_connected(uint32_t now)
{
  uint32_t last_ok;
  uint8_t uart_ok;

  taskENTER_CRITICAL();
  last_ok = g_wireless_last_ok_tick;
  uart_ok = g_wireless_uart_ok;
  taskEXIT_CRITICAL();

  if ((uart_ok == 0U) || (last_ok == 0U))
  {
    return 0U;
  }

  return ((now - last_ok) <= WRIST_WIRELESS_CONNECTED_TIMEOUT_MS) ? 1U : 0U;
}

static uint8_t wrist_home_wireless_display_is_connected(uint32_t now)
{
  static uint8_t latched_connected = 0xFFU;
  static uint8_t candidate_connected = 0xFFU;
  static uint32_t candidate_since = 0U;
  uint8_t raw_connected = wrist_home_wireless_is_connected(now);
  uint32_t latch_ms;

  if (latched_connected == 0xFFU)
  {
    latched_connected = raw_connected;
    candidate_connected = raw_connected;
    candidate_since = now;
    return latched_connected;
  }

  if (raw_connected == latched_connected)
  {
    candidate_connected = raw_connected;
    candidate_since = now;
    return latched_connected;
  }

  if (raw_connected != candidate_connected)
  {
    candidate_connected = raw_connected;
    candidate_since = now;
    return latched_connected;
  }

  latch_ms = (raw_connected != 0U) ? WRIST_WIRELESS_CONNECT_LATCH_MS : WRIST_WIRELESS_DISCONNECT_LATCH_MS;
  if ((now - candidate_since) >= latch_ms)
  {
    latched_connected = raw_connected;
  }

  return latched_connected;
}

static void wrist_home_set_bluetooth_color(WristHomeView_t *view, lv_color_t color)
{
  if (view->bt_icon != NULL)
  {
    lv_obj_set_style_text_color(view->bt_icon, color, 0);
    lv_obj_invalidate(view->bt_icon);
    if (lv_obj_get_parent(view->bt_icon) != NULL)
    {
      lv_obj_invalidate(lv_obj_get_parent(view->bt_icon));
    }
  }
}

static uint8_t wrist_home_update_wireless_status(WristHomeView_t *view, uint32_t now)
{
  uint8_t connected = wrist_home_wireless_display_is_connected(now);
  uint8_t previous = view->wireless_connected;

  if (connected == view->wireless_connected)
  {
    return 0U;
  }

  view->wireless_connected = connected;
  if (connected != 0U)
  {
    wrist_home_set_bluetooth_color(view, lv_color_hex(0x1877F2));
    lv_label_set_text(view->footer_label, WRIST_TXT_CONNECTED);
    lv_obj_set_style_text_color(view->footer_label, wrist_ui_text_color(), 0);
    lv_obj_align(view->footer_label, LV_ALIGN_BOTTOM_RIGHT, -14, -13);
    lv_obj_invalidate(view->footer_label);
    if (previous == 0U)
    {
      WristVibration_PromptAsync();
    }
  }
  else
  {
    wrist_home_set_bluetooth_color(view, wrist_ui_bt_off_color());
    lv_label_set_text(view->footer_label, WRIST_TXT_DISCONNECTED);
    lv_obj_set_style_text_color(view->footer_label, wrist_ui_muted_text_color(), 0);
    lv_obj_align(view->footer_label, LV_ALIGN_BOTTOM_RIGHT, -14, -13);
    lv_obj_invalidate(view->footer_label);
  }

  return 1U;
}

static void wrist_home_create_bluetooth_icon(lv_obj_t *parent, WristHomeView_t *view)
{
  lv_obj_t *icon = lv_label_create(parent);

  lv_label_set_text(icon, LV_SYMBOL_BLUETOOTH);
  lv_obj_set_style_text_font(icon, &lv_font_montserrat_28, 0);
  lv_obj_set_style_text_color(icon, wrist_ui_bt_off_color(), 0);
  lv_obj_align(icon, LV_ALIGN_BOTTOM_RIGHT, -82, -8);
  view->bt_icon = icon;
}

static void wrist_ui_wait_lcd_idle(void)
{
  uint32_t start = HAL_GetTick();

  while (LCD_IsDmaTransferActive() != 0U)
  {
    LCD_ProcessDmaTransfer();
    if ((HAL_GetTick() - start) > 60U)
    {
      break;
    }
  }
}

static void wrist_ui_flush_press_feedback(void)
{
  lv_refr_now(NULL);
  wrist_ui_wait_lcd_idle();
  osDelay(60U);
  lv_refr_now(NULL);
  wrist_ui_wait_lcd_idle();
}
static void wrist_home_health_event_cb(lv_event_t *event)
{
  WristUiContext_t *ui = (WristUiContext_t *)lv_event_get_user_data(event);

  if (ui != NULL)
  {
    wrist_ui_flush_press_feedback();
    wrist_heart_request_start();
    wrist_health_create(ui);
  }
}

static void wrist_home_training_event_cb(lv_event_t *event)
{
  WristUiContext_t *ui = (WristUiContext_t *)lv_event_get_user_data(event);

  if (ui != NULL)
  {
    wrist_ui_flush_press_feedback();
    if (wrist_home_wireless_is_connected(osKernelGetTickCount()) == 0U)
    {
      wrist_link_lost_create(ui);
    }
    else
    {
      wrist_wait_arm_create(ui);
    }
  }
}

static void wrist_health_back_event_cb(lv_event_t *event)
{
  WristUiContext_t *ui = (WristUiContext_t *)lv_event_get_user_data(event);

  if (ui != NULL)
  {
    wrist_ui_flush_press_feedback();
    wrist_home_create(ui);
  }
}

static void wrist_training_back_event_cb(lv_event_t *event)
{
  WristUiContext_t *ui = (WristUiContext_t *)lv_event_get_user_data(event);

  if (ui != NULL)
  {
    wrist_ui_flush_press_feedback();
    wrist_home_create(ui);
  }
}
static void wrist_wait_arm_back_event_cb(lv_event_t *event)
{
  WristUiContext_t *ui = (WristUiContext_t *)lv_event_get_user_data(event);
  lv_obj_t *btn = lv_event_get_target(event);
  lv_obj_t *label;

  if ((ui != NULL) && (btn != NULL))
  {
    label = lv_obj_get_child(btn, 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x1877F2), 0);
    lv_obj_set_style_border_color(btn, lv_color_hex(0x003FAD), 0);
    lv_obj_set_style_shadow_width(btn, 0, 0);
    lv_obj_set_style_shadow_ofs_y(btn, 0, 0);
    if (label != NULL)
    {
      lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    }
    wrist_ui_invalidate_obj_full(btn, 8);
    lv_refr_now(NULL);
    osDelay(110U);
    wrist_home_create(ui);
  }
}

static void wrist_wait_arm_back_press_event_cb(lv_event_t *event)
{
  lv_obj_t *btn = lv_event_get_target(event);
  lv_obj_t *label;
  lv_event_code_t code = lv_event_get_code(event);

  if (btn == NULL)
  {
    return;
  }

  label = lv_obj_get_child(btn, 0);
  if (code == LV_EVENT_PRESSED)
  {
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x1877F2), 0);
    lv_obj_set_style_border_color(btn, lv_color_hex(0x003FAD), 0);
    lv_obj_set_style_shadow_width(btn, 0, 0);
    if (label != NULL)
    {
      lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    }
    wrist_ui_invalidate_obj_full(btn, 8);
    lv_refr_now(NULL);
  }
  else if (code == LV_EVENT_PRESS_LOST)
  {
    lv_obj_set_style_bg_color(btn, wrist_ui_surface_color(), 0);
    lv_obj_set_style_border_color(btn, wrist_ui_border_color(), 0);
    lv_obj_set_style_shadow_width(btn, 8, 0);
    if (label != NULL)
    {
      lv_obj_set_style_text_color(label, lv_color_hex(0x2F80ED), 0);
    }
    wrist_ui_invalidate_obj_full(btn, 8);
  }
}

static void wrist_link_lost_create(WristUiContext_t *ui)
{
  lv_obj_t *scr = lv_scr_act();
  lv_obj_t *icon;
  lv_obj_t *title;
  lv_obj_t *hint;
  lv_obj_t *btn;
  lv_obj_t *label;

  ui->page = WRIST_UI_PAGE_LINK_LOST;
  taskENTER_CRITICAL();
  g_wrist_health_mode_active = 0U;
  taskEXIT_CRITICAL();
  WristVibration_PromptAsync();
  lv_obj_clean(scr);
  lv_obj_remove_style_all(scr);
  lv_obj_set_style_bg_color(scr, wrist_ui_bg_color(), 0);
  lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
  lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);

  icon = lv_label_create(scr);
  lv_label_set_text(icon, LV_SYMBOL_BLUETOOTH);
  lv_obj_set_style_text_font(icon, &lv_font_montserrat_28, 0);
  lv_obj_set_style_text_color(icon, wrist_ui_bt_off_color(), 0);
  lv_obj_align(icon, LV_ALIGN_TOP_MID, 0, 54);

  title = lv_label_create(scr);
  lv_label_set_text(title, WRIST_TXT_BT_DISCONNECTED);
  lv_obj_set_style_text_font(title, &wrist_font_zh_16, 0);
  lv_obj_set_style_text_color(title, wrist_ui_text_color(), 0);
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 94);

  hint = lv_label_create(scr);
  lv_label_set_text(hint, WRIST_TXT_CONNECT_ARM);
  lv_obj_set_style_text_font(hint, &wrist_font_zh_14, 0);
  lv_obj_set_style_text_color(hint, wrist_ui_muted_text_color(), 0);
  lv_obj_align(hint, LV_ALIGN_TOP_MID, 0, 122);

  btn = lv_btn_create(scr);
  lv_obj_remove_style_all(btn);
  lv_obj_set_size(btn, 166, 46);
  lv_obj_set_style_radius(btn, 14, 0);
  lv_obj_set_style_bg_color(btn, lv_color_hex(0x2F80ED), 0);
  lv_obj_set_style_bg_color(btn, lv_color_hex(0x003FAD), LV_STATE_PRESSED);
  lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
  lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, LV_STATE_PRESSED);
  lv_obj_set_style_shadow_width(btn, 10, 0);
  lv_obj_set_style_shadow_width(btn, 0, LV_STATE_PRESSED);
  lv_obj_set_style_shadow_color(btn, wrist_ui_shadow_color(), 0);
  lv_obj_set_style_shadow_opa(btn, LV_OPA_40, 0);
  lv_obj_set_style_shadow_ofs_y(btn, 5, 0);
  lv_obj_set_style_shadow_ofs_y(btn, 0, LV_STATE_PRESSED);
  lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -22);
  lv_obj_add_event_cb(btn, wrist_wait_arm_back_event_cb, LV_EVENT_CLICKED, ui);

  label = lv_label_create(btn);
  lv_label_set_text(label, WRIST_TXT_BACK_HOME);
  lv_obj_set_style_text_font(label, &wrist_font_zh_16, 0);
  lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
  lv_obj_clear_flag(label, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_center(label);

  wrist_ui_page_fade_in(scr);
  wrist_ui_request_force_refresh();
}

static void wrist_wait_arm_create(WristUiContext_t *ui)
{
  lv_obj_t *scr = lv_scr_act();
  lv_obj_t *icon_bg;
  lv_obj_t *icon;
  lv_obj_t *title;
  lv_obj_t *hint_1;
  lv_obj_t *hint_2;
  lv_obj_t *btn;
  lv_obj_t *label;

  ui->page = WRIST_UI_PAGE_WAIT_ARM;
  taskENTER_CRITICAL();
  g_wrist_health_mode_active = 0U;
  taskEXIT_CRITICAL();
  WristVibration_PromptAsync();
  lv_obj_clean(scr);
  lv_obj_remove_style_all(scr);
  lv_obj_set_style_bg_color(scr, wrist_ui_bg_color(), 0);
  lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
  lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);

  icon_bg = lv_obj_create(scr);
  lv_obj_remove_style_all(icon_bg);
  lv_obj_set_size(icon_bg, 74, 74);
  lv_obj_set_style_radius(icon_bg, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(icon_bg, wrist_ui_blue_soft_color(), 0);
  lv_obj_set_style_bg_opa(icon_bg, LV_OPA_COVER, 0);
  lv_obj_align(icon_bg, LV_ALIGN_TOP_MID, 0, 44);

  icon = lv_label_create(icon_bg);
  lv_label_set_text(icon, LV_SYMBOL_BLUETOOTH);
  lv_obj_set_style_text_font(icon, &lv_font_montserrat_28, 0);
  lv_obj_set_style_text_color(icon, lv_color_hex(0x1877F2), 0);
  lv_obj_center(icon);

  title = lv_label_create(scr);
  lv_label_set_text(title, WRIST_TXT_WAIT_ARM);
  lv_obj_set_style_text_font(title, &wrist_font_zh_16, 0);
  lv_obj_set_style_text_color(title, wrist_ui_text_color(), 0);
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 132);

  hint_1 = lv_label_create(scr);
  lv_label_set_text(hint_1, WRIST_TXT_SELECT_ACTION);
  lv_obj_set_style_text_font(hint_1, &wrist_font_zh_14, 0);
  lv_obj_set_style_text_color(hint_1, lv_color_hex(0x2F80ED), 0);
  lv_obj_align(hint_1, LV_ALIGN_TOP_MID, 0, 160);

  hint_2 = lv_label_create(scr);
  lv_label_set_text(hint_2, WRIST_TXT_AUTO_START);
  lv_obj_set_style_text_font(hint_2, &wrist_font_zh_14, 0);
  lv_obj_set_style_text_color(hint_2, wrist_ui_muted_text_color(), 0);
  lv_obj_align(hint_2, LV_ALIGN_TOP_MID, 0, 182);

  btn = lv_btn_create(scr);
  lv_obj_remove_style_all(btn);
  lv_obj_set_size(btn, 166, 46);
  lv_obj_set_style_radius(btn, 14, 0);
  lv_obj_set_style_bg_color(btn, wrist_ui_surface_color(), 0);
  lv_obj_set_style_bg_color(btn, lv_color_hex(0x1877F2), LV_STATE_PRESSED);
  lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
  lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, LV_STATE_PRESSED);
  lv_obj_set_style_border_width(btn, 1, 0);
  lv_obj_set_style_border_color(btn, wrist_ui_border_color(), 0);
  lv_obj_set_style_border_color(btn, lv_color_hex(0x003FAD), LV_STATE_PRESSED);
  lv_obj_set_style_shadow_width(btn, 8, 0);
  lv_obj_set_style_shadow_width(btn, 0, LV_STATE_PRESSED);
  lv_obj_set_style_shadow_color(btn, wrist_ui_shadow_color(), 0);
  lv_obj_set_style_shadow_opa(btn, LV_OPA_30, 0);
  lv_obj_set_style_shadow_ofs_y(btn, 4, 0);
  lv_obj_set_style_shadow_ofs_y(btn, 0, LV_STATE_PRESSED);
  lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -22);
  lv_obj_add_event_cb(btn, wrist_wait_arm_back_press_event_cb, LV_EVENT_PRESSED, NULL);
  lv_obj_add_event_cb(btn, wrist_wait_arm_back_press_event_cb, LV_EVENT_PRESS_LOST, NULL);
  lv_obj_add_event_cb(btn, wrist_wait_arm_back_event_cb, LV_EVENT_CLICKED, ui);

  label = lv_label_create(btn);
  lv_label_set_text(label, WRIST_TXT_BACK_HOME);
  lv_obj_set_style_text_font(label, &wrist_font_zh_16, 0);
  lv_obj_set_style_text_color(label, lv_color_hex(0x2F80ED), 0);
  lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), LV_STATE_PRESSED);
  lv_obj_clear_flag(label, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_center(label);

  wrist_ui_page_fade_in(scr);
  wrist_ui_request_force_refresh();
}

static void wrist_health_draw_heart(lv_obj_t *parent)
{
  static uint8_t heart_buf[LV_CANVAS_BUF_SIZE_TRUE_COLOR_ALPHA(40, 36)];
  static const lv_point_t heart_points[] = {
    {20, 12}, {21, 9}, {24, 6}, {28, 5}, {33, 7}, {36, 11},
    {36, 16}, {33, 21}, {29, 25}, {25, 29}, {22, 32}, {20, 34},
    {18, 32}, {15, 29}, {11, 25}, {7, 21}, {4, 16}, {4, 11},
    {7, 7}, {12, 5}, {16, 6}, {19, 9}
  };
  static const lv_point_t ecg_points[] = {
    {7, 19}, {14, 19}, {17, 14}, {21, 25}, {25, 19}, {33, 19}
  };
  const lv_color_t red = lv_color_hex(0xF24E5E);
  const lv_color_t white = lv_color_hex(0xFFFFFF);
  lv_obj_t *canvas = lv_canvas_create(parent);
  lv_coord_t x;
  lv_coord_t y;
  size_t i;

  lv_canvas_set_buffer(canvas, heart_buf, 40, 36, LV_IMG_CF_TRUE_COLOR_ALPHA);
  lv_canvas_fill_bg(canvas, lv_color_hex(0x000000), LV_OPA_TRANSP);

  for (y = 0; y < 36; y++)
  {
    for (x = 0; x < 40; x++)
    {
      if (wrist_home_point_in_polygon(heart_points,
                                      sizeof(heart_points) / sizeof(heart_points[0]),
                                      x,
                                      y) != 0U)
      {
        lv_canvas_set_px_color(canvas, x, y, red);
        lv_canvas_set_px_opa(canvas, x, y, LV_OPA_COVER);
      }
    }
  }

  for (i = 0; i < (sizeof(ecg_points) / sizeof(ecg_points[0])) - 1U; i++)
  {
    lv_coord_t min_x = LV_MIN(ecg_points[i].x, ecg_points[i + 1U].x) - 2;
    lv_coord_t max_x = LV_MAX(ecg_points[i].x, ecg_points[i + 1U].x) + 2;
    lv_coord_t min_y = LV_MIN(ecg_points[i].y, ecg_points[i + 1U].y) - 2;
    lv_coord_t max_y = LV_MAX(ecg_points[i].y, ecg_points[i + 1U].y) + 2;
    const int32_t x1 = ecg_points[i].x;
    const int32_t y1 = ecg_points[i].y;
    const int32_t dx = ecg_points[i + 1U].x - x1;
    const int32_t dy = ecg_points[i + 1U].y - y1;
    const int32_t len2 = (dx * dx) + (dy * dy);

    if (min_x < 0) min_x = 0;
    if (min_y < 0) min_y = 0;
    if (max_x >= 40) max_x = 39;
    if (max_y >= 36) max_y = 35;

    for (y = min_y; y <= max_y; y++)
    {
      for (x = min_x; x <= max_x; x++)
      {
        const int32_t px = x - x1;
        const int32_t py = y - y1;
        int32_t t = (len2 != 0) ? (((px * dx) + (py * dy)) * 256 / len2) : 0;
        int32_t near_x;
        int32_t near_y;
        int32_t dist2;

        if (t < 0) t = 0;
        if (t > 256) t = 256;

        near_x = x1 + ((dx * t) / 256);
        near_y = y1 + ((dy * t) / 256);
        dist2 = ((x - near_x) * (x - near_x)) + ((y - near_y) * (y - near_y));
        if (dist2 <= 4)
        {
          lv_canvas_set_px_color(canvas, x, y, white);
          lv_canvas_set_px_opa(canvas, x, y, LV_OPA_COVER);
        }
      }
    }
  }

  lv_obj_center(canvas);
}

static void wrist_health_draw_o2(lv_obj_t *parent)
{
  lv_obj_t *drop = lv_obj_create(parent);
  lv_obj_t *label_o;
  lv_obj_t *label_2;

  lv_obj_remove_style_all(drop);
  lv_obj_set_size(drop, 42, 42);
  lv_obj_set_style_bg_color(drop, lv_color_hex(0x2F80ED), 0);
  lv_obj_set_style_bg_opa(drop, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(drop, LV_RADIUS_CIRCLE, 0);
  lv_obj_center(drop);

  label_o = lv_label_create(drop);
  lv_obj_set_style_text_color(label_o, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_text_font(label_o, &lv_font_montserrat_28, 0);
  lv_label_set_text(label_o, "O");
  lv_obj_align(label_o, LV_ALIGN_CENTER, -5, 1);

  label_2 = lv_label_create(drop);
  lv_obj_set_style_text_color(label_2, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_text_font(label_2, &lv_font_montserrat_14, 0);
  lv_label_set_text(label_2, "2");
  lv_obj_align(label_2, LV_ALIGN_CENTER, 12, 8);
}

static void wrist_health_create_metric_card(lv_obj_t *parent,
                                            lv_coord_t x,
                                            lv_coord_t y,
                                            lv_color_t accent,
                                            const char *title,
                                            const char *unit,
                                            lv_obj_t **value_label,
                                            uint8_t is_heart)
{
  lv_obj_t *card = lv_obj_create(parent);
  lv_obj_t *icon_box;
  lv_obj_t *title_label;
  lv_obj_t *unit_label;

  lv_obj_remove_style_all(card);
  lv_obj_set_size(card, 104, 104);
  lv_obj_set_style_bg_color(card, wrist_ui_surface_color(), 0);
  lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(card, 16, 0);
  lv_obj_set_style_border_width(card, 1, 0);
  lv_obj_set_style_border_color(card, wrist_ui_border_color(), 0);
  lv_obj_set_style_shadow_width(card, 8, 0);
  lv_obj_set_style_shadow_opa(card, LV_OPA_20, 0);
  lv_obj_set_style_shadow_color(card, wrist_ui_shadow_color(), 0);
  lv_obj_set_style_shadow_ofs_y(card, 3, 0);
  lv_obj_align(card, LV_ALIGN_TOP_LEFT, x, y);
  lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);

  icon_box = lv_obj_create(card);
  lv_obj_remove_style_all(icon_box);
  lv_obj_set_size(icon_box, 46, 46);
  lv_obj_set_style_bg_color(icon_box, (is_heart != 0U) ? wrist_ui_red_soft_color() : wrist_ui_blue_soft_color(), 0);
  lv_obj_set_style_bg_opa(icon_box, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(icon_box, LV_RADIUS_CIRCLE, 0);
  lv_obj_align(icon_box, LV_ALIGN_TOP_MID, 0, 7);
  if (is_heart != 0U)
  {
    wrist_health_draw_heart(icon_box);
  }
  else
  {
    wrist_health_draw_o2(icon_box);
  }

  title_label = lv_label_create(card);
  lv_obj_set_style_text_color(title_label, wrist_ui_text_color(), 0);
  lv_obj_set_style_text_font(title_label, &wrist_font_zh_16, 0);
  lv_label_set_text(title_label, title);
  lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 48);

  *value_label = lv_label_create(card);
  lv_obj_set_style_text_color(*value_label, wrist_ui_text_color(), 0);
  lv_obj_set_style_text_font(*value_label, &lv_font_montserrat_28, 0);
  lv_obj_set_style_bg_color(*value_label, wrist_ui_surface_color(), 0);
  lv_obj_set_style_bg_opa(*value_label, LV_OPA_COVER, 0);
  lv_obj_set_width(*value_label, 58);
  lv_obj_set_style_text_align(*value_label, LV_TEXT_ALIGN_CENTER, 0);
  lv_label_set_long_mode(*value_label, LV_LABEL_LONG_CLIP);
  lv_label_set_text(*value_label, "--");
  lv_obj_align(*value_label, LV_ALIGN_BOTTOM_LEFT, 8, -5);

  unit_label = lv_label_create(card);
  lv_obj_set_style_text_color(unit_label, wrist_ui_muted_text_color(), 0);
  lv_obj_set_style_text_font(unit_label, &lv_font_montserrat_14, 0);
  lv_label_set_text(unit_label, unit);
  lv_obj_align(unit_label, LV_ALIGN_BOTTOM_RIGHT, -12, -13);

  (void)accent;
}

static void wrist_health_update_pulse_trend(WristHealthView_t *view,
                                            const int8_t *wave,
                                            uint32_t events,
                                            uint32_t now,
                                            uint8_t heart_rate)
{
  int32_t raw_sample;
  int8_t sample = 0;
  uint8_t erase_index;
  uint8_t point_value;

  (void)heart_rate;

  if ((view == NULL) || (view->trend_chart == NULL) || (view->trend_series == NULL))
  {
    return;
  }

  if ((wave != NULL) && (events != view->last_trend_events))
  {
    memcpy(view->trend_wave, wave, sizeof(view->trend_wave));
    view->last_trend_events = events;
    view->trend_has_sample = 1U;
  }

  if (view->trend_has_sample == 0U)
  {
    return;
  }

  if ((now - view->last_trend_step_tick) < WRIST_HEALTH_PULSE_STEP_MS)
  {
    return;
  }
  view->last_trend_step_tick = now;

  raw_sample = (int32_t)view->trend_wave[view->pulse_phase % MKS142_OFF_WAVE_LEN];
  view->pulse_phase = (uint8_t)((view->pulse_phase + 1U) % MKS142_OFF_WAVE_LEN);

  if (raw_sample < -80)
  {
    raw_sample = -80;
  }
  else if (raw_sample > 80)
  {
    raw_sample = 80;
  }

  if (view->trend_has_sample != 0U)
  {
    raw_sample = (((int32_t)view->trend_last_sample) + (raw_sample * 3)) / 4;
  }

  sample = (int8_t)raw_sample;
  point_value = (uint8_t)(sample + 80);
  erase_index = (view->trend_draw_index == 0U)
                    ? (uint8_t)(WRIST_HEALTH_PULSE_TREND_POINTS - 1U)
                    : (uint8_t)(view->trend_draw_index - 1U);

  lv_chart_set_value_by_id(view->trend_chart,
                           view->trend_series,
                           view->trend_draw_index,
                           (lv_coord_t)point_value);
  lv_chart_set_value_by_id(view->trend_chart,
                           view->trend_series,
                           erase_index,
                           80);
  lv_chart_refresh(view->trend_chart);

  view->trend_prev_sample = view->trend_last_sample;
  view->trend_last_sample = sample;
  view->trend_draw_index = erase_index;
}

static uint8_t wrist_health_update(WristHealthView_t *view, uint32_t now)
{
  MKS142_Data_t heart_snapshot;
  uint32_t events;
  uint32_t last_frame_tick;
  uint8_t heart_rate;
  uint8_t spo2;
  uint8_t valid;
  uint8_t changed;

  taskENTER_CRITICAL();
  heart_snapshot = g_heart_data;
  valid = g_heart_valid;
  events = s_mks142_rx_events;
  last_frame_tick = g_heart_last_frame_tick;
  taskEXIT_CRITICAL();
  heart_rate = heart_snapshot.heart_rate;
  spo2 = heart_snapshot.spo2;

  if ((valid != 0U) && ((now - last_frame_tick) > WRIST_HEART_DATA_TIMEOUT_MS))
  {
    valid = 0U;
    heart_rate = 0U;
    spo2 = 0U;
  }

  changed = ((events != view->last_heart_events) ||
             (valid != view->last_heart_valid) ||
             (heart_rate != view->last_heart_rate) ||
             (spo2 != view->last_spo2)) ? 1U : 0U;
  if (wrist_label_set_u8_or_dash(view->hr_value_label, heart_rate) != 0U)
  {
    changed = 1U;
  }
  if (wrist_label_set_u8_or_dash(view->spo2_value_label, spo2) != 0U)
  {
    changed = 1U;
  }
  if (wrist_label_set_text_if_changed(view->status_label,
                                      (valid != 0U) ? WRIST_TXT_MONITORING : WRIST_TXT_WAIT_HEART) != 0U)
  {
    changed = 1U;
  }

  if (valid != 0U)
  {
    wrist_health_update_pulse_trend(view, heart_snapshot.wave, events, now, heart_rate);
  }

  if (view->status_dot != NULL)
  {
    uint8_t blink_phase = (valid != 0U) ? (uint8_t)((now / 1000U) & 0x01U) : 0U;
    if ((blink_phase != view->last_blink_phase) || (valid != view->last_dot_valid))
    {
      if (valid != 0U)
      {
        lv_obj_set_style_bg_color(view->status_dot, lv_color_hex(0x27AE60), 0);
        lv_obj_set_style_bg_opa(view->status_dot, (blink_phase != 0U) ? LV_OPA_COVER : LV_OPA_60, 0);
      }
      else
      {
        lv_obj_set_style_bg_color(view->status_dot, lv_color_hex(0xAAB4C2), 0);
        lv_obj_set_style_bg_opa(view->status_dot, LV_OPA_60, 0);
      }
      lv_obj_invalidate(view->status_dot);
      view->last_blink_phase = blink_phase;
      view->last_dot_valid = valid;
    }
  }
  if (changed != 0U)
  {
    view->last_heart_events = events;
    view->last_heart_valid = valid;
    view->last_heart_rate = heart_rate;
    view->last_spo2 = spo2;
  }

  return changed;
}

static void wrist_health_create(WristUiContext_t *ui)
{
  WristHealthView_t *view = &ui->health;
  lv_obj_t *scr = lv_scr_act();
  lv_obj_t *back;
  lv_obj_t *title;
  lv_obj_t *accent;
  lv_obj_t *status_card;
  lv_obj_t *status_dot;
  lv_obj_t *trend_label;
  uint8_t was_health;

  was_health = (ui->page == WRIST_UI_PAGE_HEALTH) ? 1U : 0U;
  memset(view, 0, sizeof(*view));
  view->trend_draw_index = (uint8_t)(WRIST_HEALTH_PULSE_TREND_POINTS - 1U);
  ui->page = WRIST_UI_PAGE_HEALTH;
  taskENTER_CRITICAL();
  g_wrist_health_mode_active = 1U;
  if (was_health == 0U)
  {
    g_wrist_health_enter_frames = 10U;
    g_wrist_health_exit_frames = 0U;
  }
  taskEXIT_CRITICAL();
  lv_obj_clean(scr);
  lv_obj_remove_style_all(scr);
  lv_obj_set_style_bg_color(scr, wrist_ui_bg_color(), 0);
  lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
  lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);
  back = lv_btn_create(scr);
  lv_obj_remove_style_all(back);
  lv_obj_set_size(back, 96, 76);
  lv_obj_set_ext_click_area(back, 14);
  lv_obj_set_style_bg_opa(back, LV_OPA_TRANSP, 0);
  lv_obj_set_style_bg_color(back, wrist_ui_pressed_surface_color(), LV_STATE_PRESSED);
  lv_obj_set_style_bg_opa(back, LV_OPA_COVER, LV_STATE_PRESSED);
  lv_obj_set_style_radius(back, 22, LV_STATE_PRESSED);
  lv_obj_align(back, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_add_event_cb(back, wrist_health_back_event_cb, LV_EVENT_CLICKED, ui);
  title = lv_label_create(back);
  lv_obj_set_style_text_color(title, wrist_ui_text_color(), 0);
  lv_obj_set_style_text_font(title, &lv_font_montserrat_28, 0);
  lv_label_set_text(title, "<");
  lv_obj_clear_flag(title, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_center(title);

  title = lv_label_create(scr);
  lv_obj_set_style_text_color(title, wrist_ui_text_color(), 0);
  lv_obj_set_style_text_font(title, &wrist_font_zh_16, 0);
  lv_label_set_text(title, WRIST_TXT_HEALTH_TITLE);
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 13);

  accent = lv_obj_create(scr);
  lv_obj_remove_style_all(accent);
  lv_obj_set_style_bg_color(accent, lv_color_hex(0x2F80ED), 0);
  lv_obj_set_style_bg_opa(accent, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(accent, 3, 0);
  lv_obj_set_size(accent, 42, 4);
  lv_obj_align(accent, LV_ALIGN_TOP_MID, 0, 40);

  wrist_health_create_metric_card(scr, 10, 55, lv_color_hex(0xF24E5E),
                                  WRIST_TXT_HEART, "BPM", &view->hr_value_label, 1U);
  wrist_health_create_metric_card(scr, 126, 55, lv_color_hex(0x2F80ED),
                                  WRIST_TXT_SPO2, "%", &view->spo2_value_label, 0U);

  status_card = lv_obj_create(scr);
  lv_obj_remove_style_all(status_card);
  lv_obj_set_size(status_card, 220, 84);
  lv_obj_set_style_bg_color(status_card, wrist_ui_surface_color(), 0);
  lv_obj_set_style_bg_opa(status_card, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(status_card, 16, 0);
  lv_obj_set_style_border_width(status_card, 1, 0);
  lv_obj_set_style_border_color(status_card, wrist_ui_border_color(), 0);
  lv_obj_set_style_shadow_width(status_card, 8, 0);
  lv_obj_set_style_shadow_opa(status_card, LV_OPA_20, 0);
  lv_obj_set_style_shadow_color(status_card, wrist_ui_shadow_color(), 0);
  lv_obj_set_style_shadow_ofs_y(status_card, 3, 0);
  lv_obj_align(status_card, LV_ALIGN_BOTTOM_MID, 0, -12);
  lv_obj_clear_flag(status_card, LV_OBJ_FLAG_SCROLLABLE);

  view->trend_chart = lv_chart_create(status_card);
  lv_obj_remove_style_all(view->trend_chart);
  lv_obj_set_size(view->trend_chart, 194, 42);
  lv_obj_set_pos(view->trend_chart, 13, 8);
  lv_chart_set_type(view->trend_chart, LV_CHART_TYPE_LINE);
  lv_chart_set_update_mode(view->trend_chart, LV_CHART_UPDATE_MODE_SHIFT);
  lv_chart_set_point_count(view->trend_chart, WRIST_HEALTH_PULSE_TREND_POINTS);
  lv_chart_set_range(view->trend_chart, LV_CHART_AXIS_PRIMARY_Y, 0, 160);
  lv_obj_set_style_bg_opa(view->trend_chart, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(view->trend_chart, 0, 0);
  lv_obj_set_style_pad_all(view->trend_chart, 0, 0);
  lv_obj_set_style_size(view->trend_chart, 0, LV_PART_INDICATOR);
  lv_obj_set_style_line_width(view->trend_chart, 3, LV_PART_ITEMS);
  lv_obj_set_style_line_rounded(view->trend_chart, true, LV_PART_ITEMS);
  view->trend_series = lv_chart_add_series(view->trend_chart, lv_color_hex(0xF24E5E), LV_CHART_AXIS_PRIMARY_Y);
  if (view->trend_series != NULL)
  {
    for (uint8_t i = 0U; i < WRIST_HEALTH_PULSE_TREND_POINTS; i++)
    {
      lv_chart_set_next_value(view->trend_chart, view->trend_series, 80);
    }
  }

  trend_label = lv_label_create(status_card);
  lv_obj_set_style_text_color(trend_label, wrist_ui_muted_text_color(), 0);
  lv_obj_set_style_text_font(trend_label, &wrist_font_zh_14, 0);
  lv_label_set_text(trend_label, WRIST_TXT_PULSE);
  lv_obj_align(trend_label, LV_ALIGN_TOP_RIGHT, -12, 7);

  status_dot = lv_obj_create(status_card);
  lv_obj_remove_style_all(status_dot);
  lv_obj_set_size(status_dot, 12, 12);
  lv_obj_set_style_bg_color(status_dot, lv_color_hex(0x27AE60), 0);
  lv_obj_set_style_bg_opa(status_dot, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(status_dot, LV_RADIUS_CIRCLE, 0);
  lv_obj_align(status_dot, LV_ALIGN_BOTTOM_LEFT, 18, -13);
  view->status_dot = status_dot;

  view->status_label = lv_label_create(status_card);
  lv_obj_set_style_text_color(view->status_label, wrist_ui_text_color(), 0);
  lv_obj_set_style_text_font(view->status_label, &wrist_font_zh_14, 0);
  lv_obj_set_width(view->status_label, 162);
  lv_label_set_long_mode(view->status_label, LV_LABEL_LONG_CLIP);
  lv_label_set_text(view->status_label, WRIST_TXT_MONITORING);
  lv_obj_align(view->status_label, LV_ALIGN_BOTTOM_LEFT, 42, -9);
  lv_obj_move_foreground(view->status_label);


  (void)wrist_health_update(view, osKernelGetTickCount());
  wrist_ui_page_fade_in(scr);
  wrist_ui_request_force_refresh();
}

static void wrist_training_update(WristTrainingView_t *view, uint32_t now)
{
  uint8_t phase = (uint8_t)((now / 220U) % 3U);
  static const lv_coord_t dot_y[3] = {0, -6, 0};

  if (phase == view->last_phase)
  {
    return;
  }

  view->last_phase = phase;

  lv_obj_align(view->pulse_dot_1, LV_ALIGN_TOP_MID, -24, 84 + dot_y[(phase + 0U) % 3U]);
  lv_obj_align(view->pulse_dot_2, LV_ALIGN_TOP_MID, 0, 84 + dot_y[(phase + 1U) % 3U]);
  lv_obj_align(view->pulse_dot_3, LV_ALIGN_TOP_MID, 24, 84 + dot_y[(phase + 2U) % 3U]);
}

static void wrist_training_create_icon_image(lv_obj_t *parent)
{
  lv_obj_t *img = lv_img_create(parent);

  lv_img_set_src(img, &wrist_training_icon);
  lv_obj_align(img, LV_ALIGN_TOP_MID, 0, 3);
}

static void wrist_training_create(WristUiContext_t *ui)
{
  WristTrainingView_t *view = &ui->training;
  lv_obj_t *scr = lv_scr_act();
  lv_obj_t *back;
  lv_obj_t *title;
  lv_obj_t *subtitle;
  lv_obj_t *accent;
  lv_obj_t *panel;
  lv_obj_t *label;
  lv_obj_t *hint_1;
  lv_obj_t *hint_2;
  lv_obj_t *hint_3;

  memset(view, 0, sizeof(*view));
  view->last_phase = 0xFFU;
  ui->page = WRIST_UI_PAGE_TRAINING;
  taskENTER_CRITICAL();
  g_wrist_health_mode_active = 0U;
  taskEXIT_CRITICAL();
  WristVibration_PromptAsync();

  lv_obj_clean(scr);
  lv_obj_remove_style_all(scr);
  lv_obj_set_style_bg_color(scr, wrist_ui_bg_color(), 0);
  lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
  lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);

  back = lv_btn_create(scr);
  lv_obj_remove_style_all(back);
  lv_obj_set_size(back, 36, 34);
  lv_obj_align(back, LV_ALIGN_TOP_LEFT, 4, 6);
  lv_obj_add_event_cb(back, wrist_training_back_event_cb, LV_EVENT_CLICKED, ui);
  label = lv_label_create(back);
  lv_obj_set_style_text_color(label, wrist_ui_text_color(), 0);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_28, 0);
  lv_label_set_text(label, "<");
  lv_obj_center(label);

  title = lv_label_create(scr);
  lv_obj_set_style_text_color(title, wrist_ui_text_color(), 0);
  lv_obj_set_style_text_font(title, &wrist_font_zh_16, 0);
  lv_label_set_text(title, WRIST_TXT_TRAINING_ACTIVE);
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

  subtitle = lv_label_create(scr);
  lv_obj_set_style_text_color(subtitle, lv_color_hex(0x2F80ED), 0);
  lv_obj_set_style_text_font(subtitle, &lv_font_montserrat_14, 0);
  lv_label_set_text(subtitle, "Training");
  lv_obj_align(subtitle, LV_ALIGN_TOP_MID, 0, 30);

  accent = lv_obj_create(scr);
  lv_obj_remove_style_all(accent);
  lv_obj_set_style_bg_color(accent, lv_color_hex(0x2F80ED), 0);
  lv_obj_set_style_bg_opa(accent, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(accent, 3, 0);
  lv_obj_set_size(accent, 42, 4);
  lv_obj_align(accent, LV_ALIGN_TOP_MID, 0, 49);

  panel = lv_obj_create(scr);
  lv_obj_remove_style_all(panel);
  lv_obj_set_size(panel, 190, 118);
  lv_obj_set_style_bg_color(panel, wrist_ui_surface_color(), 0);
  lv_obj_set_style_bg_opa(panel, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(panel, 18, 0);
  lv_obj_set_style_border_width(panel, 1, 0);
  lv_obj_set_style_border_color(panel, wrist_ui_border_color(), 0);
  lv_obj_set_style_shadow_width(panel, 8, 0);
  lv_obj_set_style_shadow_opa(panel, LV_OPA_20, 0);
  lv_obj_set_style_shadow_color(panel, wrist_ui_shadow_color(), 0);
  lv_obj_set_style_shadow_ofs_y(panel, 3, 0);
  lv_obj_align(panel, LV_ALIGN_TOP_MID, 0, 58);
  lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE);

  wrist_training_create_icon_image(panel);

  label = lv_label_create(panel);
  lv_obj_set_style_text_color(label, wrist_ui_text_color(), 0);
  lv_obj_set_style_text_font(label, &wrist_font_zh_16, 0);
  lv_label_set_text(label, WRIST_TXT_TRAINING_NOW);
  lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, -3);

  view->pulse_dot_1 = lv_obj_create(panel);
  view->pulse_dot_2 = lv_obj_create(panel);
  view->pulse_dot_3 = lv_obj_create(panel);
  lv_obj_t *dots[3] = {view->pulse_dot_1, view->pulse_dot_2, view->pulse_dot_3};
  for (uint8_t i = 0U; i < 3U; i++)
  {
    lv_obj_remove_style_all(dots[i]);
    lv_obj_set_size(dots[i], 8, 8);
    lv_obj_set_style_bg_color(dots[i], lv_color_hex(0x27AE60), 0);
    lv_obj_set_style_bg_opa(dots[i], LV_OPA_COVER, 0);
    lv_obj_set_style_radius(dots[i], LV_RADIUS_CIRCLE, 0);
  }

  hint_1 = lv_label_create(scr);
  lv_obj_set_style_text_color(hint_1, wrist_ui_text_color(), 0);
  lv_obj_set_style_text_font(hint_1, &wrist_font_zh_14, 0);
  lv_label_set_text(hint_1, WRIST_TXT_FOLLOW_ARM);
  lv_obj_align(hint_1, LV_ALIGN_BOTTOM_MID, 0, -61);

  hint_2 = lv_label_create(scr);
  lv_obj_set_style_text_color(hint_2, wrist_ui_text_color(), 0);
  lv_obj_set_style_text_font(hint_2, &wrist_font_zh_14, 0);
  lv_label_set_text(hint_2, WRIST_TXT_FINISH_ACTION);
  lv_obj_align(hint_2, LV_ALIGN_BOTTOM_MID, 0, -42);

  hint_3 = lv_label_create(scr);
  lv_obj_set_style_text_color(hint_3, lv_color_hex(0x27AE60), 0);
  lv_obj_set_style_text_font(hint_3, &wrist_font_zh_14, 0);
  lv_label_set_text(hint_3, WRIST_TXT_KEEP_POSTURE);
  lv_obj_align(hint_3, LV_ALIGN_BOTTOM_MID, 0, -24);

  wrist_training_update(view, osKernelGetTickCount());
  wrist_ui_page_fade_in(scr);
  wrist_ui_request_force_refresh();
}

static void wrist_home_create(WristUiContext_t *ui)
{
  WristHomeView_t *view = &ui->home;
  static lv_style_t style_screen;
  static lv_style_t style_title;
  static lv_style_t style_status;
  static lv_style_t style_card;
  static lv_style_t style_card_blue_pressed;
  static lv_style_t style_card_red_pressed;
  static lv_style_t style_card_blue;
  static lv_style_t style_card_red;
  static lv_style_t style_icon_blue;
  static lv_style_t style_icon_red;
  static lv_style_t style_card_title;
  static lv_style_t style_card_subtitle;
  static lv_style_t style_blue_text;
  static lv_style_t style_red_text;
  static lv_style_t style_footer;
  lv_obj_t *scr = lv_scr_act();
  uint8_t was_health = (ui->page == WRIST_UI_PAGE_HEALTH) ? 1U : 0U;

  memset(view, 0, sizeof(*view));
  ui->page = WRIST_UI_PAGE_HOME;
  taskENTER_CRITICAL();
  g_wrist_health_mode_active = 0U;
  if (was_health != 0U)
  {
    g_wrist_health_exit_frames = 40U;
    g_wrist_health_enter_frames = 0U;
  }
  taskEXIT_CRITICAL();
  view->last_battery_percent = 0xFFU;

  lv_obj_clean(scr);
  lv_obj_remove_style_all(scr);

  wrist_home_init_styles(&style_screen, &style_title, &style_status,
                         &style_card, &style_card_blue_pressed, &style_card_red_pressed, &style_card_blue, &style_card_red,
                         &style_icon_blue, &style_icon_red,
                         &style_card_title, &style_card_subtitle,
                         &style_blue_text, &style_red_text, &style_footer);

  lv_obj_add_style(scr, &style_screen, 0);
  lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *title = lv_label_create(scr);
  lv_obj_add_style(title, &style_title, 0);
  lv_label_set_text(title, WRIST_TXT_TITLE);
  lv_obj_align(title, LV_ALIGN_TOP_LEFT, 12, 12);

  lv_obj_t *bat = lv_obj_create(scr);
  lv_obj_remove_style_all(bat);
  lv_obj_set_style_border_width(bat, 2, 0);
  lv_obj_set_style_border_color(bat, wrist_ui_text_color(), 0);
  lv_obj_set_style_radius(bat, 4, 0);
  lv_obj_set_size(bat, 25, 14);
  lv_obj_align(bat, LV_ALIGN_TOP_RIGHT, -48, 13);

  lv_obj_t *bat_fill = lv_obj_create(bat);
  lv_obj_remove_style_all(bat_fill);
  lv_obj_set_style_bg_color(bat_fill, lv_color_hex(0x28D95B), 0);
  lv_obj_set_style_bg_opa(bat_fill, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(bat_fill, 2, 0);
  lv_obj_set_style_border_width(bat_fill, 0, 0);
  lv_obj_set_size(bat_fill, 17, 8);
  lv_obj_align(bat_fill, LV_ALIGN_LEFT_MID, 2, 0);
  view->battery_fill = bat_fill;
  view->battery_label = lv_label_create(scr);
  lv_obj_add_style(view->battery_label, &style_title, 0);
  lv_label_set_text(view->battery_label, "--%");
  lv_obj_align(view->battery_label, LV_ALIGN_TOP_RIGHT, -12, 11);
  if ((s_battery_percent_valid != 0U) || (wrist_battery_load_percent(&s_battery_percent) != 0U))
  {
    (void)wrist_home_set_battery(view, s_battery_percent);
  }

  lv_obj_t *accent = lv_obj_create(scr);
  lv_obj_remove_style_all(accent);
  lv_obj_set_style_bg_color(accent, lv_color_hex(0x1877F2), 0);
  lv_obj_set_style_bg_opa(accent, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(accent, 3, 0);
  lv_obj_set_size(accent, 42, 4);
  lv_obj_align(accent, LV_ALIGN_TOP_LEFT, 14, 36);

  wrist_home_create_card(scr, &style_card, &style_card_blue_pressed, &style_card_blue, &style_icon_blue,
                         &style_card_title, &style_card_subtitle, &style_blue_text,
                         WRIST_TXT_TRAINING, WRIST_TXT_REHAB, WRIST_HOME_ICON_TRAINING, 60,
                         wrist_home_training_event_cb, ui);

  wrist_home_create_card(scr, &style_card, &style_card_red_pressed, &style_card_red, &style_icon_red,
                         &style_card_title, &style_card_subtitle, &style_red_text,
                         WRIST_TXT_HEART, WRIST_TXT_REALTIME, WRIST_HOME_ICON_HEART, 152,
                         wrist_home_health_event_cb, ui);

  wrist_home_create_bluetooth_icon(scr, view);

  view->footer_label = lv_label_create(scr);
  lv_obj_add_style(view->footer_label, &style_footer, 0);
  lv_label_set_text(view->footer_label, WRIST_TXT_DISCONNECTED);
  lv_obj_set_style_text_color(view->footer_label, wrist_ui_muted_text_color(), 0);
  lv_obj_align(view->footer_label, LV_ALIGN_BOTTOM_RIGHT, -14, -13);
  view->wireless_connected = 0xFFU;

  (void)wrist_home_update_wireless_status(view, osKernelGetTickCount());
  wrist_ui_page_fade_in(scr);
  wrist_ui_request_force_refresh();
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
  .priority = (osPriority_t) osPriorityHigh,
  .stack_size = 512 * 4
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
  .stack_size = 512 * 4
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
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 4096 * 4
};
/* Definitions for DebugTask */
osThreadId_t DebugTaskHandle;
const osThreadAttr_t DebugTask_attributes = {
  .name = "DebugTask",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 512 * 4
};
/* Definitions for VibrationTask */
osThreadId_t VibrationTaskHandle;
const osThreadAttr_t VibrationTask_attributes = {
  .name = "VibrationTask",
  .priority = (osPriority_t) osPriorityBelowNormal,
  .stack_size = 128 * 4
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
/* Definitions for vibrationQueue */
osMessageQueueId_t vibrationQueueHandle;
const osMessageQueueAttr_t vibrationQueue_attributes = {
  .name = "vibrationQueue"
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
void StartHeartRateTask(void *argument);
void StartVibrationTask(void *argument);
/* USER CODE END FunctionPrototypes */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

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
  mks142FrameSemHandle = osSemaphoreNew(1, 0, &mks142FrameSem_attributes);
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
  vibrationQueueHandle = osMessageQueueNew(4, sizeof(uint32_t), &vibrationQueue_attributes);
  /* USER CODE END RTOS_QUEUES */
  /* defaultTask, AlgoTask, EmgTask, and UiTask are CubeMX placeholders; keep them disabled while unused. */

  /* creation of SensorTask */
  SensorTaskHandle = osThreadNew(StartSensorTask, NULL, &SensorTask_attributes);

  /* HC-05 transparent UART link to the upper-arm node. */
  WirelessTaskHandle = osThreadNew(StartWirelessTask, NULL, &WirelessTask_attributes);

  /* creation of DisplayTask */
  DisplayTaskHandle = osThreadNew(StartDisplayTask, NULL, &DisplayTask_attributes);

  /* creation of DebugTask */
#if DEBUG_ANGLE_PRINT
  DebugTaskHandle = osThreadNew(StartDebugTask, NULL, &DebugTask_attributes);
#endif

  /* USER CODE BEGIN RTOS_THREADS */
  HeartRateTaskHandle = osThreadNew(StartHeartRateTask, NULL, &HeartRateTask_attributes);
  VibrationTaskHandle = osThreadNew(StartVibrationTask, NULL, &VibrationTask_attributes);
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

static void WristVibration_PromptAsync(void)
{
  uint32_t duration_ms = WRIST_VIBRATION_PROMPT_MS;

  if (WristSettings_IsVibrationEnabled() == 0U)
  {
    return;
  }

  if ((osKernelGetState() == osKernelRunning) && (vibrationQueueHandle != NULL))
  {
    (void)osMessageQueuePut(vibrationQueueHandle, &duration_ms, 0U, 0U);
  }
  else
  {
    VibrationMotor_PulseBlocking(duration_ms);
  }
}

void StartVibrationTask(void *argument)
{
  uint32_t duration_ms;

  (void)argument;
  VibrationMotor_Init();
#if WRIST_VIBRATION_BOOT_TEST
  osDelay(500U);
  if (WristSettings_IsVibrationEnabled() != 0U)
  {
    VibrationMotor_PulseBlocking(350U);
  }
#endif

  for (;;)
  {
    if (vibrationQueueHandle == NULL)
    {
      osDelay(1000U);
      continue;
    }
    if (osMessageQueueGet(vibrationQueueHandle, &duration_ms, NULL, osWaitForever) == osOK)
    {
      VibrationMotor_PulseBlocking(duration_ms);
      while (osMessageQueueGet(vibrationQueueHandle, &duration_ms, NULL, 0U) == osOK)
      {
      }
    }
  }
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
    osDelay(1);
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
  /* This task samples and solves the wrist IMU. */
  LSM6DSR_Data_t imu_data;
  IMUProc_State_t imu_state;
  int32_t gyro_bias_raw[3] = {0};
  int32_t gyro_bias_sum[3] = {0};
  uint32_t last_error_tick = 0U;
  uint16_t calib_count = 0U;

  IMUProc_StateInit(&imu_state, 0.2f);
  IMUProc_AttitudeSetGains(&imu_state.attitude, IMU_ATTITUDE_KP, IMU_ATTITUDE_KI);

  while (LSM6DSR_Init() != HAL_OK)
  {
    if ((osKernelGetTickCount() - last_error_tick) >= 500U)
    {
      last_error_tick = osKernelGetTickCount();
      osMutexAcquire(uart2MutexHandle, osWaitForever);
//      printf("LSM start failed, WHO_AM_I=0x%02X\r\n", LSM6DSR_ReadWhoAmI());
      osMutexRelease(uart2MutexHandle);
    }
    HAL_GPIO_TogglePin(LED_STATUS_GPIO_Port, LED_STATUS_Pin);
    osDelay(100);
  }

  osMutexAcquire(uart2MutexHandle, osWaitForever);
//  printf("Wrist IMU start OK\r\n");
//  printf("Keep wrist IMU still for gyro calibration\r\n");
  osMutexRelease(uart2MutexHandle);

  for (uint16_t i = 0U; i < 100U; i++)
  {
    (void)LSM6DSR_ReadRaw(&imu_data);
    osDelay(5);
  }

  IMUProc_CalibrationReset(&imu_state.calib);
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

  osMutexAcquire(uart2MutexHandle, osWaitForever);
//  printf("Wrist IMU calib OK, gyro_bias_raw=%d,%d,%d sample_raw=%d,%d,%d\r\n",
//         (int)gyro_bias_raw[0],
//         (int)gyro_bias_raw[1],
//         (int)gyro_bias_raw[2],
//         (int)imu_data.gyro_x,
//         (int)imu_data.gyro_y,
//         (int)imu_data.gyro_z);
  osMutexRelease(uart2MutexHandle);

  /* Infinite loop */
  for(;;)
  {
    if (LSM6DSR_ReadRaw(&imu_data) == HAL_OK)
    {
      IMUProc_PrepareRawForUpdate(&imu_data, gyro_bias_raw, IMU_GYRO_DEADBAND_RAW);
      IMUProc_StateUpdate(&imu_state, &imu_data, 0.005f);

      taskENTER_CRITICAL();
      /* Publish wrist IMU data for debug, display, and wireless upload to the upper-arm node. */
      wrist_imu_raw = imu_data;
      wrist_imu_euler = imu_state.attitude.euler;
      wrist_imu_valid = 1U;
      taskEXIT_CRITICAL();
    }
    else
    {
      if ((osKernelGetTickCount() - last_error_tick) >= 500U)
      {
        last_error_tick = osKernelGetTickCount();
        osMutexAcquire(uart2MutexHandle, osWaitForever);
//        printf("LSM read failed\r\n");
        osMutexRelease(uart2MutexHandle);
      }
      HAL_GPIO_TogglePin(LED_STATUS_GPIO_Port, LED_STATUS_Pin);
    }
    osDelay(5);
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
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
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
void StartEmgTask(void *argument)
{
  /* USER CODE BEGIN EmgTask */
  (void)argument;

  /* Infinite loop */
  for(;;)
  {
    osDelay(1000);
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
void StartWirelessTask(void *argument)
{
  /* USER CODE BEGIN WirelessTask */
  uint8_t tx_buf[WIRELESS_WRIST_FRAME_SIZE];
  uint8_t health_tx_buf[WIRELESS_HEALTH_FRAME_SIZE];
  WirelessWristFrame_t wrist_frame;
  WirelessHealthFrame_t health_frame;
  LSM6DSR_Data_t imu_raw;
  IMUProc_Euler_t imu_euler;
  MKS142_Data_t health_snapshot;
  uint16_t seq = 0U;
  uint16_t health_seq = 0U;
  uint32_t last_debug_tick = 0U;
  uint32_t last_health_tx_tick = 0U;
  uint32_t tx_count = 0U;
  uint32_t health_tx_count = 0U;
  uint32_t tx_error_count = 0U;
  uint8_t imu_valid;
  uint8_t health_valid;
  uint8_t health_mode_active;
  uint32_t health_last_tick;
  HAL_StatusTypeDef rx_status;

  (void)argument;

#if MKS142_USE_WIRELESS_USART1
  osMutexAcquire(uart2MutexHandle, osWaitForever);
//  printf("Wireless USART1 TX disabled for MKS142 test\r\n");
  osMutexRelease(uart2MutexHandle);
  for (;;)
  {
    osDelay(1000);
  }
#endif

  WirelessLink_Init();

//  printf("HC05 USART1 TX task start, baud=9600\r\n");

  taskENTER_CRITICAL();
  g_wireless_last_ok_tick = 0U;
  g_wireless_last_error_tick = 0U;
  g_wireless_uart_ok = 0U;
  g_wireless_rx_enabled = 0U;
  taskEXIT_CRITICAL();

  s_wireless_ack_rx_state = 0U;
  rx_status = HAL_UART_Receive_IT(&huart1, &s_wireless_ack_rx_byte, 1U);
  taskENTER_CRITICAL();
  g_wireless_rx_enabled = (rx_status == HAL_OK) ? 1U : 0U;
  taskEXIT_CRITICAL();

  /* Infinite loop */
  for(;;)
  {
    uint32_t now_tick = osKernelGetTickCount();
    uint32_t last_ok_snapshot;
    uint8_t uart_ok_snapshot;

    taskENTER_CRITICAL();
    imu_raw = wrist_imu_raw;
    imu_euler = wrist_imu_euler;
    imu_valid = wrist_imu_valid;
    health_snapshot = g_heart_data;
    health_valid = g_heart_valid;
    health_mode_active = g_wrist_health_mode_active;
    health_last_tick = g_heart_last_frame_tick;
    last_ok_snapshot = g_wireless_last_ok_tick;
    uart_ok_snapshot = g_wireless_uart_ok;
    taskEXIT_CRITICAL();

    if ((uart_ok_snapshot != 0U) &&
        (last_ok_snapshot != 0U) &&
        ((now_tick - last_ok_snapshot) > WRIST_WIRELESS_CONNECTED_TIMEOUT_MS))
    {
      taskENTER_CRITICAL();
      g_wireless_uart_ok = 0U;
      g_wrist_train_start_request = 0U;
      g_wrist_home_request = 0U;
      g_wrist_health_start_request = 0U;
      taskEXIT_CRITICAL();
    }

    FillWristWirelessFrame(&wrist_frame,
                           seq++,
                           osKernelGetTickCount(),
                           &imu_raw,
                           &imu_euler,
                           imu_valid);

    if (WirelessLink_BuildWristFrame(tx_buf, &wrist_frame) != 0U)
    {
      HAL_StatusTypeDef tx_status;

      osMutexAcquire(uart1MutexHandle, osWaitForever);
      tx_status = HAL_UART_Transmit(&huart1, tx_buf, WIRELESS_WRIST_FRAME_SIZE, 50U);
      if (tx_status == HAL_OK)
      {
        tx_count++;
      }
      else
      {
        tx_error_count++;
        taskENTER_CRITICAL();
        g_wireless_last_error_tick = osKernelGetTickCount();
        g_wireless_uart_ok = 0U;
        taskEXIT_CRITICAL();
      }
      osMutexRelease(uart1MutexHandle);
    }

    if ((health_mode_active != 0U) &&
        (health_valid != 0U) &&
        ((now_tick - health_last_tick) <= WRIST_HEART_DATA_TIMEOUT_MS) &&
        ((now_tick - last_health_tx_tick) >= WRIST_HEALTH_WIRELESS_PERIOD_MS))
    {
      health_frame.seq = health_seq++;
      health_frame.tick = now_tick;
      health_frame.heart_rate = health_snapshot.heart_rate;
      health_frame.spo2 = health_snapshot.spo2;
      health_frame.micro_circ = health_snapshot.micro_circ;
      health_frame.fatigue = health_snapshot.fatigue;
      health_frame.sbp = health_snapshot.sbp;
      health_frame.dbp = health_snapshot.dbp;
      health_frame.sdnn = health_snapshot.sdnn;
      health_frame.rmssd = health_snapshot.rmssd;
      health_frame.status = WIRELESS_STATUS_HEART_VALID;
      if (WirelessLink_BuildHealthFrame(health_tx_buf, &health_frame) != 0U)
      {
        HAL_StatusTypeDef tx_status;

        osMutexAcquire(uart1MutexHandle, osWaitForever);
        tx_status = HAL_UART_Transmit(&huart1, health_tx_buf, WIRELESS_HEALTH_FRAME_SIZE, 50U);
        osMutexRelease(uart1MutexHandle);
        if (tx_status == HAL_OK)
        {
          health_tx_count++;
          last_health_tx_tick = now_tick;
        }
        else
        {
          tx_error_count++;
          taskENTER_CRITICAL();
          g_wireless_last_error_tick = osKernelGetTickCount();
          g_wireless_uart_ok = 0U;
          taskEXIT_CRITICAL();
        }
      }
    }

    if (huart1.RxState == HAL_UART_STATE_READY)
    {
      rx_status = HAL_UART_Receive_IT(&huart1, &s_wireless_ack_rx_byte, 1U);
      taskENTER_CRITICAL();
      g_wireless_rx_enabled = (rx_status == HAL_OK) ? 1U : 0U;
      taskEXIT_CRITICAL();
    }

    if ((osKernelGetTickCount() - last_debug_tick) >= 1000U)
    {
#if WRIST_WIRELESS_STATUS_DEBUG
      uint32_t ack_tick_snapshot;
      uint8_t ack_rx_snapshot;
      uint8_t ack_ok_snapshot;
      uint32_t now_snapshot;

      last_debug_tick = osKernelGetTickCount();
      now_snapshot = last_debug_tick;
      taskENTER_CRITICAL();
      ack_tick_snapshot = g_wireless_last_ok_tick;
      ack_rx_snapshot = g_wireless_rx_enabled;
      ack_ok_snapshot = g_wireless_uart_ok;
      taskEXIT_CRITICAL();

#if WRIST_WIRELESS_ASCII_PING_DEBUG
      {
        static const uint8_t ping_msg[] = {'P', 'I', 'N', 'G', '\r', '\n'};
        osMutexAcquire(uart1MutexHandle, osWaitForever);
        (void)HAL_UART_Transmit(&huart1, (uint8_t *)ping_msg, sizeof(ping_msg), 50U);
        osMutexRelease(uart1MutexHandle);
      }
#endif

      printf("HC05_TX count=%lu health=%lu tx_err=%lu seq=%u valid=%u ack=%u rx=%u ack_age=%lu acc_mg=%d,%d,%d gyro_x10=%d,%d,%d angle_x100=%d,%d,%d\r\n",
             (unsigned long)tx_count,
             (unsigned long)health_tx_count,
             (unsigned long)tx_error_count,
             (unsigned int)wrist_frame.seq,
             (unsigned int)imu_valid,
             (unsigned int)ack_ok_snapshot,
             (unsigned int)ack_rx_snapshot,
             (unsigned long)((ack_tick_snapshot == 0U) ? 9999U : (now_snapshot - ack_tick_snapshot)),
             (int)wrist_frame.acc_mg[0],
             (int)wrist_frame.acc_mg[1],
             (int)wrist_frame.acc_mg[2],
             (int)wrist_frame.gyro_dps_x10[0],
             (int)wrist_frame.gyro_dps_x10[1],
             (int)wrist_frame.gyro_dps_x10[2],
             (int)wrist_frame.angle_x100[0],
             (int)wrist_frame.angle_x100[1],
             (int)wrist_frame.angle_x100[2]);
#else
      last_debug_tick = osKernelGetTickCount();
#endif
    }

    osDelay(WRIST_WIRELESS_PERIOD_MS);
  }
  /* USER CODE END WirelessTask */
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
    osDelay(1);
  }
  /* USER CODE END UiTask */
}

/* USER CODE BEGIN Header_StartDisplayTask */
/**
* @brief Function implementing the DisplayTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDisplayTask */
void StartDisplayTask(void *argument)
{
  /* USER CODE BEGIN DisplayTask */
#if WRIST_ATTITUDE_LCD_DEBUG_UI
  int32_t last_roll = -2147483647 - 1;
  int32_t last_pitch = -2147483647 - 1;
  int32_t last_yaw = -2147483647 - 1;

  (void)argument;
  (void)LCD_Init();
  LCD_FillScreen(LCD_COLOR_BLACK);
  LCD_Fill(6U, 20U, 24U, 66U, LCD_COLOR_RED);
  LCD_Fill(6U, 110U, 24U, 156U, LCD_COLOR_GREEN);
  LCD_Fill(6U, 200U, 24U, 246U, LCD_COLOR_BLUE);

  for(;;)
  {
    IMUProc_Euler_t euler;
    IMUProc_EulerX10_t euler_x10;
    uint8_t valid;

    taskENTER_CRITICAL();
    euler = wrist_imu_euler;
    valid = wrist_imu_valid;
    taskEXIT_CRITICAL();

    if (valid != 0U)
    {
      IMUProc_EulerToSignedX10(&euler, IMU_ANGLE_DEADBAND_X10, &euler_x10);
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
  WristUiContext_t ui;
  uint32_t last_lv_tick;
  uint32_t last_wireless_update = 0U;
  uint32_t last_battery_update = 0U;
  uint32_t last_link_guard_update = 0U;
  uint32_t last_health_update = 0U;
  uint32_t last_training_update = 0U;
  uint32_t last_touch_activity = 0U;
  uint8_t settings_night_before_open = 0U;
  uint8_t display_sleeping = 0U;

  (void)argument;
  memset(&ui, 0, sizeof(ui));

  (void)LCD_Init();
  (void)CST816T_Init();
  LV_Port_Init();
  wrist_boot_create();
  wrist_boot_hold(WRIST_BOOT_LOGO_DURATION_MS);
  wrist_home_create(&ui);

  last_lv_tick = osKernelGetTickCount();
  last_touch_activity = last_lv_tick;

  for(;;)
  {
    uint32_t now = osKernelGetTickCount();
    uint32_t elapsed = now - last_lv_tick;
    uint8_t train_start_request;
    uint8_t home_request;
    uint8_t health_start_request;
    uint8_t force_refresh_request;
    uint8_t key_event;
    uint8_t setting_key_event;

    if (display_sleeping != 0U)
    {
      taskENTER_CRITICAL();
      train_start_request = g_wrist_train_start_request;
      health_start_request = g_wrist_health_start_request;
      if (train_start_request != 0U)
      {
        g_wrist_train_start_request = 0U;
      }
      if (health_start_request != 0U)
      {
        g_wrist_health_start_request = 0U;
      }
      taskEXIT_CRITICAL();

      if ((train_start_request != 0U) || (health_start_request != 0U))
      {
        display_sleeping = 0U;
        taskENTER_CRITICAL();
        g_wrist_sleeping = 0U;
        taskEXIT_CRITICAL();
        LCD_DisplayOn();
        LCD_SetBacklight(1U);
        wrist_user_key_reset(now);
        wrist_setting_key_reset(now);
        last_touch_activity = now;
        last_lv_tick = now;
        if (train_start_request != 0U)
        {
          wrist_training_create(&ui);
          last_training_update = now;
          last_link_guard_update = now;
        }
        else
        {
          wrist_heart_request_start();
          wrist_health_create(&ui);
          last_health_update = now;
        }
        wrist_ui_force_full_refresh();
        continue;
      }

      if ((CST816T_IsPressed() != 0U) ||
          (HAL_GPIO_ReadPin(USER_KEY_GPIO_Port, USER_KEY_Pin) == GPIO_PIN_RESET) ||
          (HAL_GPIO_ReadPin(SETTING_KEY_GPIO_Port, SETTING_KEY_Pin) == GPIO_PIN_RESET))
      {
        display_sleeping = 0U;
        taskENTER_CRITICAL();
        g_wrist_sleeping = 0U;
        taskEXIT_CRITICAL();
        LCD_DisplayOn();
        LCD_SetBacklight(1U);
        wrist_user_key_reset(now);
        wrist_setting_key_reset(now);
        last_touch_activity = now;
        last_lv_tick = now;
        wrist_home_create(&ui);
        last_wireless_update = now;
        last_battery_update = now;
        WristVibration_PromptAsync();
        wrist_ui_force_full_refresh();
      }
      osDelay(WRIST_SLEEP_POLL_MS);
      continue;
    }

    if (elapsed != 0U)
    {
      lv_tick_inc(elapsed);
      last_lv_tick = now;
    }

    if (CST816T_IsPressed() != 0U)
    {
      last_touch_activity = now;
    }

    if ((ui.page == WRIST_UI_PAGE_HOME) || (ui.page == WRIST_UI_PAGE_HEALTH))
    {
      key_event = wrist_user_key_scan(now);
    }
    else
    {
      key_event = 0U;
      wrist_user_key_reset(now);
    }

    if (ui.page == WRIST_UI_PAGE_HOME)
    {
      setting_key_event = wrist_setting_key_scan(now);
    }
    else
    {
      setting_key_event = 0U;
      wrist_setting_key_reset(now);
    }

    taskENTER_CRITICAL();
    train_start_request = g_wrist_train_start_request;
    g_wrist_train_start_request = 0U;
    home_request = g_wrist_home_request;
    g_wrist_home_request = 0U;
    health_start_request = g_wrist_health_start_request;
    g_wrist_health_start_request = 0U;
    force_refresh_request = g_wrist_force_refresh_request;
    g_wrist_force_refresh_request = 0U;
    taskEXIT_CRITICAL();

    if ((home_request != 0U) && (ui.page != WRIST_UI_PAGE_HOME))
    {
      wrist_home_create(&ui);
      last_wireless_update = now;
      last_battery_update = now;
      last_touch_activity = now;
      force_refresh_request = 1U;
    }
    else if ((train_start_request != 0U) && (ui.page != WRIST_UI_PAGE_TRAINING))
    {
      wrist_training_create(&ui);
      last_training_update = now;
      force_refresh_request = 1U;
    }
    else if ((health_start_request != 0U) && (ui.page != WRIST_UI_PAGE_HEALTH))
    {
      wrist_heart_request_start();
      wrist_health_create(&ui);
      last_health_update = now;
      last_touch_activity = now;
      force_refresh_request = 1U;
    }

    if ((ui.page == WRIST_UI_PAGE_HOME) && ((setting_key_event & WRIST_KEY_EVENT_LONG) != 0U))
    {
      if (WristSettings_IsOpen(&ui.home.settings) != 0U)
      {
        uint8_t night_before_close = settings_night_before_open;
        WristSettings_Close(&ui.home.settings);
        if (WristSettings_IsNightMode() != night_before_close)
        {
          wrist_home_create(&ui);
          last_wireless_update = now;
          last_battery_update = now;
          force_refresh_request = 1U;
        }
      }
    }
    else if ((ui.page == WRIST_UI_PAGE_HOME) && ((setting_key_event & WRIST_KEY_EVENT_SHORT) != 0U))
    {
      if (WristSettings_IsOpen(&ui.home.settings) == 0U)
      {
        settings_night_before_open = WristSettings_IsNightMode();
        WristSettings_Open(&ui.home.settings);
      }
      else
      {
        WristSettings_SelectNext(&ui.home.settings);
      }
      last_touch_activity = now;
    }

    if ((ui.page == WRIST_UI_PAGE_HOME) && (WristSettings_IsOpen(&ui.home.settings) != 0U))
    {
      if ((key_event & WRIST_KEY_EVENT_SHORT) != 0U)
      {
        WristSettings_ApplySelected(&ui.home.settings);
        last_touch_activity = now;
      }
      key_event = 0U;
    }

    if (((key_event & WRIST_KEY_EVENT_LONG) != 0U) &&
        (ui.page == WRIST_UI_PAGE_HEALTH) &&
        (WristSettings_IsHeartKeyEnabled() != 0U))
    {
      wrist_home_create(&ui);
      last_wireless_update = now;
      last_battery_update = now;
      last_touch_activity = now;
      force_refresh_request = 1U;
    }
    else if (((key_event & WRIST_KEY_EVENT_SHORT) != 0U) &&
             (ui.page == WRIST_UI_PAGE_HOME) &&
             (WristSettings_IsHeartKeyEnabled() != 0U))
    {
      wrist_heart_request_start();
      wrist_health_create(&ui);
      last_health_update = now;
      force_refresh_request = 1U;
    }

    if ((ui.page == WRIST_UI_PAGE_HOME) && ((now - last_wireless_update) >= 100U))
    {
      last_wireless_update = now;
      (void)wrist_home_update_wireless_status(&ui.home, now);
    }

    if ((ui.page == WRIST_UI_PAGE_HOME) &&
        (WristSettings_IsOpen(&ui.home.settings) == 0U) &&
        ((now - last_battery_update) >= WRIST_BATTERY_UPDATE_MS))
    {
      last_battery_update = now;
      (void)wrist_home_update_battery(&ui.home);
    }

    if ((ui.page == WRIST_UI_PAGE_HOME) &&
        (WristSettings_IsOpen(&ui.home.settings) == 0U) &&
        ((now - last_touch_activity) >= WRIST_HOME_SLEEP_TIMEOUT_MS))
    {
      LCD_ProcessDmaTransfer();
      wrist_ui_wait_lcd_idle();
      LCD_FillScreen(LCD_COLOR_BLACK);
      LCD_DisplayOff();
      LCD_SetBacklight(0U);
      taskENTER_CRITICAL();
      g_wrist_sleeping = 1U;
      g_wrist_training_mode_active = 0U;
      g_wrist_health_mode_active = 0U;
      taskEXIT_CRITICAL();
      display_sleeping = 1U;
      osDelay(WRIST_SLEEP_POLL_MS);
      continue;
    }

    if (((ui.page == WRIST_UI_PAGE_WAIT_ARM) || (ui.page == WRIST_UI_PAGE_TRAINING)) &&
        ((now - last_link_guard_update) >= 500U))
    {
      last_link_guard_update = now;
      if (wrist_home_wireless_display_is_connected(now) == 0U)
      {
        wrist_link_lost_create(&ui);
        force_refresh_request = 1U;
      }
    }

    if ((ui.page == WRIST_UI_PAGE_HEALTH) && ((now - last_health_update) >= WRIST_HEALTH_UPDATE_MS))
    {
      last_health_update = now;
      (void)wrist_health_update(&ui.health, now);
    }

    if ((ui.page == WRIST_UI_PAGE_TRAINING) && ((now - last_training_update) >= 220U))
    {
      last_training_update = now;
      wrist_training_update(&ui.training, now);
    }

    taskENTER_CRITICAL();
    g_wrist_training_mode_active = (ui.page == WRIST_UI_PAGE_TRAINING) ? 1U : 0U;
    g_wrist_health_mode_active = (ui.page == WRIST_UI_PAGE_HEALTH) ? 1U : 0U;
    taskEXIT_CRITICAL();

    LCD_ProcessDmaTransfer();
    (void)lv_timer_handler();
    if (force_refresh_request != 0U)
    {
      wrist_ui_force_full_refresh();
    }
    LCD_ProcessDmaTransfer();
    osDelay(1);
  }
#endif
  /* USER CODE END DisplayTask */
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
  (void)argument;

  /* Infinite loop */
  for(;;)
  {
#if DEBUG_ANGLE_PRINT
    LSM6DSR_Data_t imu_data;
    IMUProc_Euler_t imu_euler;
    IMUProc_EulerX10_t imu_euler_x10;
    uint8_t imu_valid;

    taskENTER_CRITICAL();
    imu_data = wrist_imu_raw;
    imu_euler = wrist_imu_euler;
    imu_valid = wrist_imu_valid;
    taskEXIT_CRITICAL();

    if (imu_valid != 0U)
    {
      int32_t ax[3];
      const char *sgn[3];
      uint32_t mag[3];
      uint8_t i;

      IMUProc_EulerToSignedX10(&imu_euler, IMU_ANGLE_DEADBAND_X10, &imu_euler_x10);
      ax[0] = imu_euler_x10.roll_x10;
      ax[1] = imu_euler_x10.pitch_x10;
      ax[2] = imu_euler_x10.yaw_x10;
      for (i = 0U; i < 3U; i++)
      {
          sgn[i] = (ax[i] < 0) ? "-" : "";
          mag[i] = (ax[i] < 0) ? (uint32_t)(-ax[i]) : (uint32_t)ax[i];
      }
      (void)imu_data;

      osMutexAcquire(uart2MutexHandle, osWaitForever);
      printf("ANGLE roll=%s%lu.%lu pitch=%s%lu.%lu yaw=%s%lu.%lu\r\n",
             sgn[0], (unsigned long)(mag[0] / 10U), (unsigned long)(mag[0] % 10U),
             sgn[1], (unsigned long)(mag[1] / 10U), (unsigned long)(mag[1] % 10U),
             sgn[2], (unsigned long)(mag[2] / 10U), (unsigned long)(mag[2] % 10U));
      osMutexRelease(uart2MutexHandle);
    }
#endif
    osDelay(100);
  }
  /* USER CODE END DebugTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/**
  * @brief  闂傚倸鍊搁…顒勫磻閸曨個娲Χ婢跺﹦鏌堥梺鍝勵槹椤戞瑩宕崨瀛樼厽闁规儳鍟块鍌炴煟閺冨倸甯堕柣蹇斿▕閺屟嗙疀閿濆懍绨绘繛瀵稿Т閻°劎鎹㈠☉姘ｅ亾閻㈡鐒鹃棄瀣渻閵堝棙鑲犻柛鏇ㄥ墮瀵潡姊虹紒妯荤叆闁告艾顑夊鏌ヮ敆娴ｇ懓寮垮┑顔筋殔濡鏅堕鍓х＜闁告瑥顦伴妵婵囨叏婵犲啯銇濇鐐存崌楠炴帡骞橀搹顐や悍濠电姷鏁搁崑娑⒙ㄩ埀顒傜磼閻樿櫕宕岄柣娑卞櫍楠炲洭寮剁捄銊モ偓鐐烘⒑閹稿孩顥嗘俊顐㈠椤ｄ粙姊婚崒娆戠獢婵炰匠鍏炬盯寮崒娑卞仺濠殿喗顭堝▔娑㈡倿閸偁浜滈柟鍝勭Ф椤︼箑鈹戦鍏煎枠闁哄矉缍侀崺濠傗枎韫囨柨顦╅梺鎼炲€曠粔褰掑蓟濞戞矮娌柛鎾楀嫬娅欓梻浣告啞閿曗晠宕滈悢鐓庤摕闁跨喓濮寸壕濂告煏閸繃鍣圭紒鐘虫そ濮婄儤娼幍顕呮М濠电姭鍋撻梺顒€绉撮悞鍨亜閹哄棗浜惧┑鐘亾闂侇剙绉寸壕濠氭煟閹邦剚鈻曟繛鎾愁煼閺屾稑鐣濋埀顒勫磻濞戞氨鐭嗙€光偓閸曨剛鍘卞┑鐐村灥瀹曨剚绂嶉幇鐗堢厱閻庯綆鍋呯亸浼存煙瀹勭増鍤囩€规洩缍侀、鏇㈡晲閸涱喛鐧侀柣搴㈩問閸犳骞冮崒鐐靛祦闁搞儺鍓﹂弫鍡椕归敐鍥┿€婇柛瀣崌閹粙宕ㄦ繝鍕箞婵犵數鍋涘Λ娆撳礉閺囥垺鍊堕柕澹懐锛滈梺鍝勫暙閸婃悂宕㈢€电硶鍋撶憴鍕闁搞劍瀵ф穱濠囨倻閽樺）銊╂煥閺囨浜鹃梺閫炲苯澧剧紓宥勭窔瀵鍩勯崘鈺侇€撻梺鍛婄箓鐎氱兘寮抽銏″€甸悷娆忓鐏忣偆绱掗懜鐢殿暡濞?
  */
void StartHeartRateTask(void *argument)
{
  MKS142_Data_t hr;
  uint32_t last_debug_tick = 0U;
  HAL_StatusTypeDef rx_status;
  const uint8_t start_cmd[] = {MKS142_CMD_ACQUIRE_ON};

  (void)argument;

  MKS142_ParserReset(&s_mks142_parser);

#if MKS142_USE_WIRELESS_USART1
  /* 濠电姷鏁搁崑鐐哄垂閸洖绠伴柟闂寸劍閸嬨倝鏌曟繛鍨姶婵炴挸顭烽弻娑樼暆閳ь剟宕戦悙鍝勭柧妞ゆ帊鐒﹂崣蹇旂節闂堟稒顥炵紒鈧径缍琓1(PA9=TX/PA10=RX) 濠电姷鏁告慨浼村垂閻撳簶鏋栨繛鎴炴皑閻捇鏌熺紒銏犳灈闁哄嫨鍎甸弻鏇＄疀閵壯咃紵闂佽崵鍠庣紞濠囧箖濡ゅ懏鏅查幖绮光偓鍐茬闂備礁鎼悮顐﹀磹濠靛钃熼柨婵嗩槸鎯熼梺闈涱槶閸庣儤瀵奸崘顔解拺闂侇偆鍋涢懟顖涙櫠椤栫偞鐓曢柡鍐ｅ亾婵炶尙鍠栭獮鍐┿偊濞嗙偓鈻岄梻浣烘嚀绾绢參宕戦妶鍛殾闁圭儤鍨熼弸搴ㄦ煙閻戞ê鐏ョ紒鐘冲灴濮婄粯鎷呴崨濠冨創闂佺顑呭Λ婵嗙暦閵忥紕顩烽悗锝庝簼閻庮剟姊洪棃鈺佺槣闁告ü绮欏鍐差煥閸涱喖鏋戦柟鑹版彧缁插潡鎯岄崼銉﹀€甸梻鍫熺⊕閸熺偤鎮楀顓炲摵闁哄矉绻濆畷鍫曞煛娴ｅ湱鈧箖姊虹紒妯诲皑闁稿鎸荤换婵堝枈濡椿娼戦梺鍓茬厛娴滎亪宕洪埀顒併亜閹烘垵鏆為柣婵愪邯閺屾稓鈧絻鍔岄崝锕傛煛?
     婵犵數濮烽。钘壩ｉ崨鏉戝瀭妞ゅ繐鐗嗛悞鍨亜閹烘垵鏆為柣婵愪邯閺?UTX闂傚倸鍊烽悞锕傚礈濮樿泛纾婚柛娑卞枟濞呯姷绱?0闂傚倸鍊风欢姘焽瑜嶈灋闁哄啫鍊绘稉宥夋煙閻㈢數鈧參姊婚崒娆戝妽闁告挻宀稿畷鎴﹀川椤旇姤鐝风紓?闂傚倸鍊风欢姘焽瑜嶈灋闁哄啫鍊婚惌鍡椼€掑锝呬壕閻庤娲╃紞渚€寮幘缁樺亹闁告瑥顦粊鍫曟⒒娴ｅ搫甯跺鐟帮躬瀹曟顫滈埀顒€鐣?.3V闂傚倸鍊风欢姘焽瑜嶈灋婵°倕鎳庣壕褰掓煙闂傚顦﹂柛灞诲妽缁绘盯骞嬪▎蹇曚患闂?0x8A闂傚倸鍊烽悞锔锯偓绗涘懐鐭欓柟杈鹃檮閸ゆ劖銇勯弽顐粶闁汇値鍣ｉ弻宥堫檨闁告挻鐩崺鈧い鎺嶇贰閸熷繘鏌涢悩鍐叉诞鐎规洘鍨块獮姗€寮妷锔绘綌闂備胶鎳撻悘婵嬪疮椤愶妇宓佹俊銈呮噺閻撴盯鎮橀悙璺盒撻柛鐘筹耿閺岋紕浠﹂懞銉ユ畻濡ょ姷鍋涘ú顓€佸Δ鍛＜婵炴垶鐟ラ弸娆撴⒒閸屾瑨鍏岄柛妯犲洤绠规い鎰剁畱绾惧綊鏌涢敂璇插箺闁哄棙绮撻弻鐔虹磼閵忕姵鐏堢紓鍌欒閺呯娀骞冪憴鍕闁告稑锕ㄧ涵鈧紓鍌欒閸?
     闂?HAL_UART_RxCpltCallback 闂傚倸鍊风粈渚€骞楀鍕弿闁圭虎鍠栫壕褰掓煕婵犲嫬娅忛柡鍡曞嵆濮婄粯鎷呴崨濠冨創濡炪倖鍨靛Λ婵嗙暦濠靛鍗抽柣鎰ㄦ櫆閻庢椽姊洪崷顓炲妺妞ゃ劌妫濆鏌ヮ敆閸屾浜鹃柛蹇擃槸娴滈箖姊洪崨濠冨闁告挻绻堝畷鎴炴媴缁洘鏂€闂佺粯蓱婢х娀宕奸妷銉у姦濡炪倖甯掗崯顐︽儍閻戞ǜ浜滈煫鍥ㄧ◥閹查箖鏌熼鑽ょ煓妞ゃ垺鐩畷鍗炍旈埀顒勵敂閳哄懏鐓冩い蹇撴閺嗩剟鏌?HR闂?*/
  (void)HAL_UART_Transmit(&huart1, (uint8_t *)start_cmd, sizeof(start_cmd), 50U);
  (void)HAL_UART_Receive_IT(&huart1, &s_mks142_u1_byte, 1U);

  for (;;)
  {
    /* 闂傚倸鍊风粈渚€骞夐敍鍕煓闊洦绋戠粈澶屸偓骞垮劚閹锋垿鎳撻崸妤佺叆闁绘柨鎼牎闂佺粯鎸搁崐濠氬焵椤掆偓缁犲秹宕曢柆宥呯疇閹艰揪绲界欢銈嗙箾瀹割喕绨奸柍閿嬪灴閺岀喓绱掑Ο铏圭懖闂佸摜濮佃摫缂佺粯绋掑鍕槈濮樿鲸娈告俊鐐€栧ú鈺冪礊娓氣偓瀹曟椽鍩€椤掍降浜滈柟鐑樺灥椤忊晝鈧懓鎲＄换鍫ュ蓟閻旇　鍋撳☉娅亞绮顑芥斀妞ゆ柨鎼悘顏呫亜閵婏絽鍔﹂柟顔界懅閹瑰嫰鎼归悷鏉跨細婵犵數濮幏鍐吋閸モ晜娈奸柣搴ゎ潐濞测晝鎹㈠┑瀣畺鐟滄柨鐣烽悡搴樻斀闁糕檧鏅滀簺闂傚倸鍊峰ù鍥Υ閳ь剟鏌涚€ｎ偅宕岄柡宀€鍠栭、娑㈡倷闊厼鏋堥梻浣告啞閸╁啴宕戦幘缁樷拻濞达綀濮ら妴鍐磼閳ь剚鎷呯憴鍕妳濠电姴锕ら幊蹇涳綖閺囩喓绡€闂傚牊绋掑婵嬫煟閻旂儤鍤€妞ゎ叀娉曢幑鍕倻濡鏋戦梻浣哄帶缂嶅﹦绮婚弽顓炵畺鐎瑰嫭澹嬮弸搴ㄧ叓閸ャ劍灏ㄩ柛瀣崌楠炴牗鎷呴悷鏉垮闂備胶顭堥惉濂稿磻濞戙垹鐭楀┑鐘插暔娴滄粓鏌″鍐ㄥ妞わ絾鐓￠弻娑欐償閳藉懓鍚梺鍝勬湰閻╊垶寮崒鐐蹭紶闁告洦鍋掗悗鎾⒒娴ｈ姤銆冪紒鈧担铏圭濠电姴鍋嗛崵鏇熴亜閹板爼妾柛瀣儔閺屾盯顢曢敐鍥╃暭闂?*/
    (void)HAL_UART_Transmit(&huart1, (uint8_t *)start_cmd, sizeof(start_cmd), 50U);

    if (huart1.RxState == HAL_UART_STATE_READY)
    {
      (void)HAL_UART_Receive_IT(&huart1, &s_mks142_u1_byte, 1U);
    }

    osMutexAcquire(uart2MutexHandle, osWaitForever);
    printf("MKS142 usart1 bytes=%lu events=%lu err=0x%08lX\r\n",
           (unsigned long)s_mks142_rx_bytes,
           (unsigned long)s_mks142_rx_events,
           (unsigned long)huart1.ErrorCode);
    osMutexRelease(uart2MutexHandle);

    osDelay(1000);
  }
#endif

  /* 婵犵數濮烽。钘壩ｉ崨鏉戝瀭妞ゅ繐鐗嗛悞鍨亜閹烘垵鏆為柣婵愪邯閺屾稓鈧絻鍔岄崝姘舵煃閽樺妯€妤犵偞顭囩划鐢告偡閺夋娼梻鍌欐祰濡椼劑鎮為敃鍌氱畺闁稿本姘ㄩ弳锕傛煏婢舵盯妾柛搴ｅ枛閺屻劌鈽夊Ο渚紑闂佷紮缍€妞村憡绌辨繝鍥ㄥ€锋い蹇撳閸嬫挸鈽夐姀鈺傛櫔閻熸粍鏌ㄩ悾鐑藉閻橆偅顫嶉梺闈涚箳婵兘宕Δ鍛厽闁绘ê寮堕幆鍫ユ煙绾板崬浜伴柟顕嗙節瀹曟帡鎮欑划瑙勫闂備礁婀辩划顖滄暜濡ゅ懎绐楁俊顖濆吹缁犳儳顭块懜鐢点€掗柣锝堥哺閵囧嫰寮撮鍡櫺滈梺绯曟杹閸嬫挸顪冮妶鍡楃瑨閻庢凹鍓涚划缁樺鐎涙鍘遍梺闈涱樈閸ㄨ泛顬婇灏栨斀?0x8A 闂傚倸鍊风粈浣虹礊婵犲偆鐒界憸蹇曞垝閺傝法鏆﹂柛銉厛濞肩喖姊虹紒姗嗘當闁绘绻橀崺銏ゅ棘鎼存挻鏂€闂佺粯蓱瑜板啴寮冲▎鎴炲仏婵鍩栭悡鐔煎箹缁懓澧查悹鎰剁節閺屾稓鈧綆鍓欐禒婊勩亜椤忓嫬鏆ｇ€规洘甯￠幃娆撴嚑椤掑楔闂傚倷绀侀幉锛勫垝瀹€鍕柈濞寸厧顕畵渚€鏌曡箛瀣偓鏍偂閺囩喓绠鹃柛鈩冣棨閼测晞濮抽柤濮愬€栭崰鎰版煛婢跺﹦浠㈤柍褜鍓欏﹢杈╁垝濞嗘劕绶為柟閭﹀墰閸旓箑顪冮妶鍡楀潑闁稿鎹囬弻宥囨嫚閺屻儱寮伴梺绯曟杺閸庨潧鐣烽妸鈺婃晬婵犲﹤瀛╁暩闂傚倸鍊搁崐椋庢閿熺姴绀堟繛鍡樻尰閸婅埖绻濋棃娑卞剰闁稿被鍔戦弻鏇㈠醇濠垫劖笑缂備胶濮伴崕鐢稿蓟閻旂厧绠氱憸蹇涘箺閻樼粯鐓熼柟鐑樻礃椤ュ鏌嶉挊澶樻█妤犵偞顭囩划鐢垫喆閸曨剚姣庢繝鐢靛У椤旀牠宕伴幘璇茬９婵犻潧娲ｇ换?*/
  (void)HAL_UART_Transmit(&huart2, (uint8_t *)start_cmd, sizeof(start_cmd), 50U);

  /* 濠电姷鏁告慨浼村垂閻撳簶鏋栨繛鎴炴皑閻捇鏌熺紒銏犳灈闁哄嫨鍎甸弻鏇＄疀閵壯咃紵闂佽崵鍠庣紞濠囧箖濡ゅ懏鏅查幖绮光偓鍐茬闂備礁鎼悮顐﹀磹濠靛钃熼柕濞垮劗濡插牊绻涢崱妤冪闁圭晫鏁诲娲焻閻愯尪瀚板褜鍨堕弻娑㈠籍閳ь剙煤閻旂厧绠栧Δ锔筋儥濡嫰姊洪悷鎵紞闁稿鍊曢悾鐑藉箳閹存梹鐎婚梺鍦劋閸ㄧ數绮婚幋锔解拻濞达絽鎲￠幆鍫ユ煕鐎ｎ亝顥㈢€规洏鍨介幃鍓т沪閻愵剚顔囬梻浣告贡閾忓酣宕伴弽顐ょ＜闁冲搫鎳忛崑锝吤归敐鍛暈閻犳劧绱曠槐鎺楀焵椤掍礁绶為柟閭﹀幘閸橀亶姊洪棃娑崇础闁告剬鍐╂啟闂傚倷鐒﹂惇褰掑礉鎼淬劌绐楅柡宥庡幖閽冪喓鈧厜鍋撻柍褜鍓氱粋鎺楁晝閸屾稑娈熼梺闈涱檧婵″洨绱欐繝姘拺闁煎鍊曢弸鎴︽煟閻旇鍚€垫澘锕ラ妶锝夊礃閳轰讲鍋撻棃娴㈠綊鎮╁顔煎壈闂傚倸鍋嗛崹閬嶅Φ閸曨垼鏁囬柣鏃堫棑椤戝倻绱撴担鎻掍壕婵犮垼鍩栭崝鏍煕閹寸偑浜滈柟鍝勬娴滈箖姊虹拠鑼缂佽鐗嗛悾?DMA 缂傚倸鍊搁崐椋庣矆娓氣偓瀵敻顢楅埀顒勨€旈崘顔藉癄濠㈠厜鏅滅粙鎺旂矉閹烘柡鍋撻敐搴濈敖妞わ富鍋婂铏瑰寲閺囩偛鈷夊銈冨妼濡瑩骞堥妸锔藉劅闁靛绠戦埀顒勬涧闇夐柨婵嗘噹閺嗘瑦銇勯妷銉ч柡灞剧洴婵＄敻宕￠悙鏉戭槱闂佺楠哥粔褰掑蓟濞戙垹鍗抽柕濞垮€楅崙瑙勭箾閹寸偞灏柛銊ょ矙楠炲啫螖閳ь剟鍩ユ径濞炬瀻婵☆垵宕甸弳顖氣攽?闂傚倸鍊搁崐鐑芥倿閿旈敮鍋撶粭娑樻处閸嬶繝鏌曢崼婵囧窛缂佲偓婵犲洦鐓曟繛鎴炵懄缂嶆垶娼?*/
  rx_status = HAL_UART_Receive_IT(&huart2, &s_mks142_u2_byte, 1U);

#if DEBUG_HR_PRINT
  osMutexAcquire(uart2MutexHandle, osWaitForever);
  printf("MKS142 start tx=0x%02X rx_status=%d uart_err=0x%08lX\r\n",
         (unsigned int)start_cmd[0],
         (int)rx_status,
         (unsigned long)huart2.ErrorCode);
  osMutexRelease(uart2MutexHandle);
#endif
  (void)rx_status;

  for (;;)
  {
    uint8_t start_request;

    if (wrist_sleep_is_active() != 0U)
    {
      osDelay(WRIST_SLEEP_POLL_MS);
      continue;
    }

    taskENTER_CRITICAL();
    start_request = g_heart_start_request;
    g_heart_start_request = 0U;
    taskEXIT_CRITICAL();

    if (start_request != 0U)
    {
      MKS142_ParserReset(&s_mks142_parser);
      osMutexAcquire(uart2MutexHandle, osWaitForever);
      (void)HAL_UART_Transmit(&huart2, (uint8_t *)start_cmd, sizeof(start_cmd), 50U);
      osMutexRelease(uart2MutexHandle);
      if (huart2.RxState == HAL_UART_STATE_READY)
      {
        (void)HAL_UART_Receive_IT(&huart2, &s_mks142_u2_byte, 1U);
      }
    }

    /* 婵犵數濮烽。钘壩ｉ崨鏉戝瀭妞ゅ繐鐗嗛悞鍨亜閹烘垵鏆為柣婵愪邯閺屾稓鈧絻鍔岄崝婊堟煙娓氬灝濮傞柡浣瑰姍瀹曘劑顢涘搴℃暯闂?1.28s 濠电姷鏁搁崑鐐哄垂閸洖绠伴柟闂寸劍閺呮繈鏌曟径娑㈡闁告瑥绻橀弻鏇㈠醇濠靛浂妫ら梺宕囩帛濡啴寮诲鍫闂佸憡鎸婚惄顖氼嚕椤愶富鏁婃繛鍡欏亾椤庡绱撻崒娆愮グ濡炴潙鎽滈幑銏ゅ礋椤栨氨顔愭俊銈忕到閸燁偄顔忓┑鍥ヤ簻闁哄啫鍊婚幗鍌涗繆瀹割喖鐏︽慨?2000ms闂? 濠电姷鏁搁崑鐐哄垂閸洖绠伴柟闂寸劍閺呮繈鏌曟径娑㈡闁告瑥绻橀弻鏇㈠醇濠垫劖笑閻庣懓鎲＄换鍫ュ蓟閺囩喓绡€闊洦绋掗宥夋⒑缁洘娅呴柛濠傛健瀵濡搁妷銏☆潔濠殿喗顨呭Λ娆擃敁閸℃瑧纾藉ù锝勭矙閸濊櫣绱掔拠鑼ⅵ鐎殿喖顭峰畷鍗炩槈濡櫣鈧厼顪冮妶鍡樷拻闁告鍛皫閻庯綆鍠楅悡鐔兼煟濡搫绾х紒浣叉櫊閺屾盯濡搁妶鍛ギ閻庤娲栫紞濠囧箖閸撗傛勃闁绘劦鍓欓崝姘舵⒒娴ｅ憡鍟炲〒姘殜瀹曟澘螖閸氳棄娲╅ˇ褰掓煛瀹€鈧崰鏍х暦瑜版帩鏁冮柨婵嗘嚇閻庢椽姊绘担鍛婃儓闁兼椿鍨扮叅闁靛牆顦扮粻鎺戔攽閻愯埖褰х紒鎻掓健閵嗗倹绂掔€ｎ亞锛?*/
    if (osSemaphoreAcquire(mks142FrameSemHandle, 2000U) == osOK)
    {
      taskENTER_CRITICAL();
      hr = g_heart_data;
      g_screen_heart_rate = hr.heart_rate;
      g_screen_spo2 = hr.spo2;
      taskEXIT_CRITICAL();

#if DEBUG_HR_PRINT
      osMutexAcquire(uart2MutexHandle, osWaitForever);
      printf("HR=%u SpO2=%u SBP=%u DBP=%u\r\n",
             (unsigned int)hr.heart_rate,
             (unsigned int)hr.spo2,
             (unsigned int)hr.sbp,
             (unsigned int)hr.dbp);
      osMutexRelease(uart2MutexHandle);
#endif
      (void)hr;
    }
    else
    {
      /* 闂傚倷鑳堕崕鐢稿礈濠靛牊鏆滈柟鐑橆殔缁犵娀鐓崶銊︽儎婵?>濠电姷鏁搁崑鐐哄垂閸洖绠伴柟缁㈠枛绾惧鏌熼悙顒佺伇婵℃彃鐗撻弻鐔煎垂椤斿吋娈ㄧ紓浣靛姂椤ユ挾妲愰幒鎳虫棃鍩€椤掑嫬纾归柛褎顨堝畵渚€骞栧ǎ顒€濡奸崬顖炴⒑闂堟侗妾х紒鐘冲灥閳诲秴鈻庨幘绮规嫼?闂傚倸鍊风粈渚€骞栭锔藉亱婵犲﹤瀚々鍙夌節闂堟侗鍎忛柡瀣╄兌閹叉悂鎮ч崼婵堢懆缂備讲鍋撻柛鈩冪⊕閻撶喐淇婇妶鍌氫壕闂佺粯顨呴敃顏堝箖閿熺姴鐒垫い鎺戝閻撶喖鏌曡箛濠冩珔闁诲氦宕甸埀顒€绠嶉崕鍗灻洪妸鈺傚仒闁靛鏅滈ˉ濠冦亜閹扳晛鐏柟鍏煎姍閺屾盯鎮㈤崨濠勫嚒濡炪倖鏌ㄧ换鎴犵紦閻ｅ瞼鐭欓柟绋挎捣閳ь剦鍓熷铏圭磼濮楀棙鐣堕梺缁橆殔濡稓鍒掓繝鍥ㄦ櫆闂佹鍨版禍楣冩偡濞嗗繐顏柟钘夊暣閹绠涢敐鍕晼闂佺懓绠嶉崹钘夌暦濮椻偓椤㈡瑩鎮℃惔锝呯秮闂傚倷鑳剁划顖毭洪弽顓炵９闁煎鍊楅惌鍡楊熆鐠轰警鍎戠紒鐘荤畺閺岀喖骞戦幇顓犲涧闂侀潻绲块崕銈囨崲濠靛鐒垫い鎺嶈兌閻熷綊鏌嶈閸撴瑩顢氶敐澶樻晝闁挎洍鍋撻梺鍗炴处缁绘繃绻濋崒娑樻閻庢稒绻堝濠氬磼濞嗘垼绐楅梺鍛婃尰閻熲晠骞冨Ο琛℃斀閻庯綆浜滈崑宥夋⒑闂堟稓澧曟い锔诲灣閻氭儳顓兼径瀣幗濠殿喗銇涢崑鎾寸箾娴ｅ啿鍟犻弸鏃堟煟濡偐甯涢柛瀣ㄥ妽閵囧嫰寮借椤ユ粓鏌涢悢璺哄祮鐎殿噮鍋婃俊鎼佸煛閸屾瀚介梻浣告啞濞诧箓宕戦崱妯绘珷婵炴垶鐟ょ换?bpm闂?*/
      uint32_t now_tick = osKernelGetTickCount();
      uint32_t last_frame_tick;

      taskENTER_CRITICAL();
      last_frame_tick = g_heart_last_frame_tick;
      if ((g_heart_valid == 0U) ||
          (last_frame_tick == 0U) ||
          ((now_tick - last_frame_tick) > WRIST_HEART_DATA_TIMEOUT_MS))
      {
        g_heart_valid = 0U;
        g_heart_data.heart_rate = 0U;
        g_heart_data.spo2 = 0U;
        g_screen_heart_rate = 0U;
        g_screen_spo2 = 0U;
      }
      taskEXIT_CRITICAL();

      /* 濠电姷鏁搁崑鐐哄垂閸洖绠伴柟缁㈠枛绾惧鏌熼崜褏甯涢柛瀣ㄥ妽閵囧嫰寮借椤ユ粍绻涚仦鍌氣偓婵嗙暦閵忊懇鍋撳☉娆樼劷缂佺娀绠栭幃姗€鎮欏▓璺ㄥ姼婵犫拃鍕弨闁哄备鍓濋幏鍛村传閵夘灝銊╂倵鐟欏嫭绀冪€光偓閹间胶宓侀煫鍥ㄦ尵閺嗗棝鏌嶈閸撴稓鍒掓繝姘唨妞ゆ挾鍠撻崣鍡椻攽閻愭潙鐏﹂柣鐔濆洢鈧倿寮婚妷锕€娈欓梺鍐叉惈閹冲繘鎮￠弴鐘冲枑閹兼番鍔婇埀顒€鍟村畷銊╂嚋椤戞寧鐫忛梻浣告惈鐞氼偊宕曢弻銉ュ惞闁告洦鍨遍悡鐘绘煙椤撶喎绗掗柛鏃€绮庨埀顒侇問閸犳骞戦崶顒€钃熸繛鎴烇供濞尖晠鏌ｉ幘铏崳濞寸姭鏅滅换婵嗏枔閸喗鐏€闂佸搫鎳愭繛鈧€殿噮鍋勯濂稿川椤栨稒鐣烽梻浣侯焾濞存岸宕规导姝ゅ洭妫冨☉姘闂佹寧绻傞ˇ顖滅不閿濆鐓ラ柡鍥殔娴滄儳鈹戦悙鍙夊櫤鐎光偓缁嬫娼栧┑鐘宠壘楠炪垺淇婇妶鍕窗婵☆偆鍋熺槐鎾存媴娴犲鎽甸梺鎼炲妼濞尖€愁嚕椤愩埄鍚嬮柛娑变簼椤秴鈹戦埥鍡楃仩闁圭⒈鍋勯埢鎾诲Ψ閳哄倵鎷洪柣鐐寸▓閳ь剙鍘栨竟鏇㈡⒑閼姐倕孝闁圭⒈鍋婇獮鏍敃閿旇棄浜楅梺绋跨灱閸嬫稓鐥閺屽秵娼幍顔煎闂佺懓鍚嬮崝娆撳蓟濞戔懇鈧箓骞嬪┑鍫滃寲缂傚倷娴囨ご鍝ユ暜閹烘鐓橀柟杈剧畱瀹告繃銇勯弽銊х煁闁哥姵甯″濠氬磼濞嗘垵濡介悷婊勬緲閸熸壆鍒掗弮鍌氼棜閻庯絻鍔嬪Ч妤呮⒑缁嬭法鐏遍柛瀣仱瀵?
         闂傚倸鍊搁崐椋庢閿熺姴绀堟繛鍡樻尰閸婅埖绻濋棃娑卞剰闁稿被鍔戦弻鏇㈠醇濠垫劖笑缂備胶濮伴崕鐢稿蓟閻旂厧绠氱憸蹇涘箺閻樼粯鐓熼柟鐑樻礀琚氭繛锝呮搐閿曨亪銆佸☉姗嗘富閻犲洩寮撴竟鏇㈡⒑閹勭闁稿瀚粋鎺楀箻椤旇В鎷哄┑顔炬嚀濞层倖淇婃總鍛婂€垫慨妯煎帶婢у瓨顨ラ悙璇ц含鐎规洖宕埥澶娢熼搹鐟扮秮闂傚倷鑳剁划顖炲蓟閵婏附娅犻幖娣妽閸嬶繝鏌熼幍顔碱暭闁绘挻鐟╁Λ鍛搭敆娴ｅ摜绁烽梺鍛婃⒐閿曘垽寮诲鍫闂佸憡鎸婚悷鈺佺暦閹版澘绠瑰ù锝呮憸閻ｆ椽姊虹憴鍕妞ゆ泦鍛暗鐎广儱顦伴悡鏇㈡煙閹规劕钄兼い鏂款樀閺屽秷顧侀柛鎾翠亢閹筋偆绱撴担姝屽鐎规洦鍓熼崺鐐哄箣閿曗偓瀹告繈鏌℃径濠勪虎缂佺姵鍨垮缁樻媴閸涘﹥鍎撻梺绋款儏濡繂鐣烽姀銈嗗仼閻忕偟鍋撳▓浼存⒑閸︻叀妾搁柛鐘崇墱缁辨帡鍩￠崨顔间化闁哄鍋炴刊浠嬵敆閻旈晲绻嗛柟缁樺笒閼稿綊妫佹径瀣瘈濠电姴鍊搁弳锝嗐亜鎼淬埄娈滈柡灞剧洴楠炲寮崫鍕闂佸綊顣﹂悞锕€顩奸妸鈺傗拺闁圭瀛╃壕鑽ょ磼閸ㄦ稈鍋撳鍕洴瀹曘劎鈧稒顭囬崢钘夆攽閳藉棗鐏犻柣蹇旂箞閹繝骞囬悧鍫㈠帗閻熸粍绮撳畷妤€鈽夐姀鈾€鎸冮悗骞垮劚椤︻垶鎮?*/
      if (s_mks142_rx_events == 0U)
      {
        (void)HAL_UART_Transmit(&huart2, (uint8_t *)start_cmd, sizeof(start_cmd), 50U);
      }
      if (huart2.RxState == HAL_UART_STATE_READY)
      {
        (void)HAL_UART_Receive_IT(&huart2, &s_mks142_u2_byte, 1U);
      }

#if DEBUG_HR_PRINT
      if ((osKernelGetTickCount() - last_debug_tick) >= 1000U)
      {
        last_debug_tick = osKernelGetTickCount();
        osMutexAcquire(uart2MutexHandle, osWaitForever);
        printf("MKS142 wait events=%lu bytes=%lu uart_err=0x%08lX\r\n",
               (unsigned long)s_mks142_rx_events,
               (unsigned long)s_mks142_rx_bytes,
               (unsigned long)huart2.ErrorCode);
        osMutexRelease(uart2MutexHandle);
      }
#endif
    }
  }
}

/**
  * @brief  UART 闂傚倸鍊搁崐椋庢閿熺姴纾婚柛娑欐綑缁€澶愭煛閸ャ儱鐏柛瀣ㄥ€楅幉鎼佸籍閸繄鐤囬梺缁樺姇閹碱偆澹曢崗鑲╃闁瑰浼濋鍩跺洭寮婚妷锔规嫼闁荤偞绋堥埀顒€鍘栨竟鏇㈡⒑閼姐倕鈻堢紓鍌涜壘閳诲秹鏁愭径濠冭緢濠殿喗顭堥崺鏍偂濞嗘挻鍊垫繛鎴烆仾椤忓懏鍙忛柛銉ｅ妿缁犻箖鏌涘▎蹇ｆШ濠⒀囦憾閺岀喐绺芥径濠佸枈閻庤娲橀…鍥箲閸曨垰惟鐟滄粌危閸楃偐鏀介柣姗嗗枛閻忚鲸绻涙径瀣€掔紒顔芥楠炴﹢顢欓崗鍏夹氶梻渚€娼ф蹇曟閺囥垺鍋＄憸鐗堝笚閸婄敻鏌ㄥ┑鍡涱€楅柛妯绘尦閺岋紕浠︾拠娴嬪亾濠靛棭娼栨繛宸憾閺佸啴鏌ㄥ┑鍡樺櫣妞ゎ偄鐭傚娲倻閳轰緡娼氶梺鎼炲劘閸斿苯鈻撻悢鍏尖拺缂佸瀵у﹢鎵磼鐎ｎ偄娴€殿喗濞婇、鏃堝醇閻旇渹缂撴俊鐐€栭悧鏇炍涘Δ鍐煋闁告挷鑳堕惌鍡涙煕閳╁啰鈯曢柣?
  *         USART2(闂傚倸鍊搁…顒勫磻閸曨個娲Χ婢跺﹦鏌堥梺鍝勵槹椤戞瑩宕崨瀛樼厽闁规儳鍟块鍌炴煟閺冨倸甯堕柣蹇斿▕閺屟嗙疀閿濆懍绨绘繛?闂傚倸鍊烽悞锔锯偓绗涘懐鐭欓柟鐑橆殕閸ゅ苯螖閿濆懎鏆欑紒鐙欏洦鐓ｉ煫鍥ㄥ嚬濞兼劙鏌涢悩鍙夘棦闁哄被鍔岄埞鎴﹀幢濮楀棙锟ラ梻渚€鈧偛鑻崢鍝ョ磼椤旂晫鎳冮柣锝囧厴楠炲海鎲撮敐鍌氫壕闁挎洖鍊哥粻鎶芥煛閸愩劌浜炴俊鑼厴濮婂宕掑顑藉亾缁嬭娑樜旈埀顒冪亱濠电偛妫楀ù姘跺吹閺囩姵鍠愰幖杈剧悼閻瑥顭块懜闈涘闁绘帒鐏氶妵鍕箳閹存績鍋撹ぐ鎺戞辈婵犲﹤鎳忛崰鎰偓骞垮劚閻楀嫭绂嶈ぐ鎺撶厱閻忕偛澧介惌瀣煛閸℃鐭掗柡灞剧洴楠炲洭鍩℃担鍓茬€辨繝鐢靛剳缂嶅棝宕抽敐澶屽祦闁哄稁鍘肩粻缁樸亜閺冨洤袚闁哄棛濞€濮婃椽宕崟顒夋￥闂佸摜濮甸悧鐘绘偘椤曗偓楠炲鏁傜紒妯绘珦闂備浇澹堥～澶婎熆濮椻偓閹敻顢涘鍛紳闂佺鏈懝楣冨几閵堝鐓曢柣妯挎珪缁€鍐磼瀹€鍐摵缂佺粯绻堝畷鎯邦槾闁绘挻鍨垮铏圭磼濡櫣浼囧┑鈽嗗亜鐎氼剟鍩㈠澶娢у璺侯儑閸?
  *         闂傚倸鍊搁崐鎼佸磹閹间礁绠犻幖杈剧稻瀹曟煡鏌熼悧鍫熺凡缂佺姵鐗楁穱濠囧Χ閸涱喖娅ら梺缁樻尭閸婂鍩€椤掆偓缁犲秹宕曢崡鐐嶆稑鈻庨幘鏉戜簵闂侀€炲苯澧柍瑙勫灴椤㈡瑩鎮惧畝鈧鍥╃磽娴ｅ搫校閻㈩嚪銈嗭紓濠电姷鏁告慨鏉懨洪妶鍥╀笉闁规儼濮ら悡蹇擃熆閼哥數娲存俊缁㈠櫍閺岋繝宕遍埡鈧柇顖炴煛鐏炶濡奸柍钘夘槸閳诲酣骞嬮弮鈧～宥嗙節閻㈤潧浠滅€殿喖鐖奸幃褍顭ㄩ崼婵堫唵濡炪倖鍔х粻鎴犵矆閸愵喗鐓忓┑鐐茬仢閸旀粓鏌￠崱鎰姦婵﹥妞藉畷銊︾節閸曨剙娅у┑鐐茬摠缁秶鍒掑鍥╃焿闁圭儤鍨熷Σ鍫熺箾閸℃ê濮囨い顐熷墲缁绘繈鎮介棃娴讹綁鏌ょ憴鍕姢闁宠绉规俊鑸靛緞鐎ｎ亙鐢绘繝鐢靛仜濡瑩鎮疯缁瑦绻濆顓犲幗闁瑰吋鐣崐鏇☆暱缂傚倷绀侀鍛此囨导鏉戠畾闁哄啫鐗嗛悘宕団偓瑙勬礀濞层倝鎯屽┑瀣拺婵懓娲ら悞娲煕閵娿倕宓嗙€规洘婢橀埥澶婎潩椤撗勭稐婵犵數濞€濞佳兠洪妸鈺佺厺闁哄啫鍊甸崑鎾荤嵁閸喖濮庨梺鐟板暱缁夊爼宕氶幒妤佸仺闁告稑锕﹂崢楣冩⒑缂佹﹩娈旈柣妤佺矊閳绘捇寮撮悢鍓佺畾闂佺粯鍔栬ぐ鍐焵椤掆偓濞硷繝鎮伴灏栨瀻闊浄绲介幃鎴︽⒑閸涘﹥澶勯柛姗€绠栭崺銏ゅ即閵忊檧鎷?
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  MKS142_Data_t frame;

  if (huart->Instance == USART2)
  {
    if (g_wrist_sleeping != 0U)
    {
      (void)HAL_UART_Receive_IT(&huart2, &s_mks142_u2_byte, 1U);
      return;
    }
    s_mks142_rx_bytes++;
    if (MKS142_FeedByte(&s_mks142_parser, s_mks142_u2_byte, &frame) != 0U)
    {
      s_mks142_rx_events++;
      g_heart_data = frame;
      g_heart_valid = 1U;
      g_heart_last_frame_tick = HAL_GetTick();
      g_screen_heart_rate = frame.heart_rate;
      g_screen_spo2 = frame.spo2;
      (void)osSemaphoreRelease(mks142FrameSemHandle);
    }
    (void)HAL_UART_Receive_IT(&huart2, &s_mks142_u2_byte, 1U);
  }
#if MKS142_USE_WIRELESS_USART1
  else if (huart->Instance == USART1)
  {
    s_mks142_rx_bytes++;
    if (MKS142_FeedByte(&s_mks142_parser, s_mks142_u1_byte, &frame) != 0U)
    {
      s_mks142_rx_events++;
      g_heart_data = frame;
      g_heart_valid = 1U;
      g_heart_last_frame_tick = HAL_GetTick();
      g_screen_heart_rate = frame.heart_rate;
      g_screen_spo2 = frame.spo2;
    }
    (void)HAL_UART_Receive_IT(&huart1, &s_mks142_u1_byte, 1U);
  }
#else
  else if (huart->Instance == USART1)
  {
    if (s_wireless_ack_rx_state == 0U)
    {
      s_wireless_ack_rx_state = (s_wireless_ack_rx_byte == WIRELESS_ACK_BYTE0) ? 1U : 0U;
    }
    else if (s_wireless_ack_rx_state == 1U)
    {
      if (s_wireless_ack_rx_byte == WIRELESS_ACK_BYTE1)
      {
        g_wireless_last_ok_tick = HAL_GetTick();
        g_wireless_uart_ok = 1U;
        s_wireless_ack_rx_state = 2U;
      }
      else
      {
        s_wireless_ack_rx_state = (s_wireless_ack_rx_byte == WIRELESS_ACK_BYTE0) ? 1U : 0U;
      }
    }
    else if (s_wireless_ack_rx_state == 2U)
    {
      if (s_wireless_ack_rx_byte == WIRELESS_LINK_MSG_BYTE2)
      {
        s_wireless_ack_rx_state = 3U;
      }
      else if (s_wireless_ack_rx_byte == WIRELESS_TRAIN_MSG_BYTE2)
      {
        s_wireless_ack_rx_state = 4U;
      }
      else if (s_wireless_ack_rx_byte == WIRELESS_HOME_MSG_BYTE2)
      {
        s_wireless_ack_rx_state = 5U;
      }
      else if (s_wireless_ack_rx_byte == WIRELESS_HEALTH_MSG_BYTE2)
      {
        s_wireless_ack_rx_state = 6U;
      }
      else
      {
        s_wireless_ack_rx_state = (s_wireless_ack_rx_byte == WIRELESS_ACK_BYTE0) ? 1U : 0U;
      }
    }
    else if (s_wireless_ack_rx_state == 3U)
    {
      if (s_wireless_ack_rx_byte == WIRELESS_LINK_MSG_BYTE3)
      {
        g_wireless_last_ok_tick = HAL_GetTick();
        g_wireless_uart_ok = 1U;
      }
      s_wireless_ack_rx_state = (s_wireless_ack_rx_byte == WIRELESS_ACK_BYTE0) ? 1U : 0U;
    }
    else
    {
      if (s_wireless_ack_rx_state == 4U)
      {
        if (s_wireless_ack_rx_byte == WIRELESS_TRAIN_MSG_BYTE3)
        {
          g_wireless_last_ok_tick = HAL_GetTick();
          g_wireless_uart_ok = 1U;
          g_wrist_train_start_request = 1U;
        }
      }
      else if ((s_wireless_ack_rx_state == 5U) && (s_wireless_ack_rx_byte == WIRELESS_HOME_MSG_BYTE3))
      {
        g_wireless_last_ok_tick = HAL_GetTick();
        g_wireless_uart_ok = 1U;
        g_wrist_home_request = 1U;
      }
      else if ((s_wireless_ack_rx_state == 6U) && (s_wireless_ack_rx_byte == WIRELESS_HEALTH_MSG_BYTE3))
      {
        g_wireless_last_ok_tick = HAL_GetTick();
        g_wireless_uart_ok = 1U;
        g_wrist_health_start_request = 1U;
      }
      s_wireless_ack_rx_state = (s_wireless_ack_rx_byte == WIRELESS_ACK_BYTE0) ? 1U : 0U;
    }
    {
      HAL_StatusTypeDef rx_status = HAL_UART_Receive_IT(&huart1, &s_wireless_ack_rx_byte, 1U);
      g_wireless_rx_enabled = (rx_status == HAL_OK) ? 1U : 0U;
    }
  }
#endif
}
/* USER CODE END Application */





























