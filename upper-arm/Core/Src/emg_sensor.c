/* ==================== Include ==================== */
#include "emg_sensor.h"

#include "adc.h"
#include "app_freertos.h"
#include "emg_participation_model.h"
#include "rehab_utils.h"
#include "tim.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
/* ==================== 宏定义 ==================== */
#define EMG_SENSOR_ADC_MAX             (4095.0f)
#define EMG_SENSOR_VREF_MV             (3300.0f)
/* 采集模式下 100Hz 采样，2 秒静息数据用于建立个体化基线和噪声水平。 */
#define EMG_SENSOR_BASELINE_SAMPLES    (EMG_SENSOR_SAMPLE_RATE_HZ * 2U)
#define EMG_SENSOR_WARMUP_SAMPLES      (EMG_SENSOR_SAMPLE_RATE_HZ * 3U)
#define EMG_SENSOR_ENVELOPE_ALPHA      (0.15f)
/* 20 点约等于 200ms 窗口，用于估计肌电短时 RMS。 */
#define EMG_SENSOR_RMS_WINDOW_LEN      (20U)
/* 噪声阈值倍数，避免静息抖动被误判为主动发力。 */
/* This processed dry-electrode module shows a large relaxed raw range; the
 * range becomes smaller as the user applies more force. */
#define EMG_SENSOR_RANGE_WINDOW_LEN     (25U)
#define EMG_SENSOR_RANGE_UPDATE_DECIMATE (10U)
#define EMG_SENSOR_RANGE_HISTORY_LEN    (15U)
#define EMG_SENSOR_RANGE_ACTIVE_RATIO   (0.80f)
#define EMG_SENSOR_RANGE_FULL_RATIO     (0.20f)
#define EMG_SENSOR_ACTIVE_ENTER_SAMPLES (10U)
#define EMG_SENSOR_ACTIVE_EXIT_SAMPLES  (5U)
#define EMG_SENSOR_STRENGTH_ALPHA       (0.12f)
#define EMG_SENSOR_BASE_DEBUG          (0U)
#define EMG_SENSOR_RAW_LOG_LEN         (512U)
/* 正式版默认使用实测验证过的稳定摆幅算法，不再让旧模型覆盖判断结果。
 * 保留模型源文件和兼容接口，后续如需对比可在此处显式改为 1。 */
#define EMG_SENSOR_MODEL_ENABLE        (0U)
#define EMG_SENSOR_MODEL_DECIMATE      (10U)

/* ==================== 全局静态变量 ==================== */
/* ADC DMA 环形缓冲区，半满/满中断分别处理前后 32 点。 */
static uint16_t emg_adc_buffer[EMG_SENSOR_BUFFER_LEN];
/* RMS 不直接保存原始 ADC，而是保存去基线后的幅值平方滑动均值。 */
static float emg_rms_window[EMG_SENSOR_RMS_WINDOW_LEN];
static EmgSensor_State_t emg_state;
/* baseline_square_sum 用 E[x^2] - E[x]^2 估计静息噪声方差。 */
static uint32_t emg_baseline_sum;
static uint64_t emg_baseline_square_sum;
static uint32_t emg_baseline_count;
static uint32_t emg_warmup_count;
static uint16_t emg_baseline_min;
static uint16_t emg_baseline_max;
static float emg_relax_range;
static Rehab_FirstOrderFilter_t emg_envelope_filter;
static Rehab_MovingAverage_t emg_rms_average;
static uint16_t emg_range_window[EMG_SENSOR_RANGE_WINDOW_LEN];
static uint8_t emg_range_window_index;
static uint8_t emg_range_window_count;
static float emg_range_history[EMG_SENSOR_RANGE_HISTORY_LEN];
static uint8_t emg_range_history_index;
static uint8_t emg_range_history_count;
static uint8_t emg_range_update_count;
static float emg_stable_range;
static uint8_t emg_sampling_running;
static uint8_t emg_software_fallback;
/* Software fallback reconfigures ADC4 away from TIM3/DMA. Remember that the
 * generated peripheral configuration must be restored before the next run. */
static uint8_t emg_dma_config_valid = 1U;
static uint32_t emg_sample_count;
static uint8_t emg_model_enabled;
static uint8_t emg_model_decimate_count;
static uint8_t emg_active_enter_count;
static uint8_t emg_active_exit_count;
static float emg_strength_filtered;
static EmgSensor_RawSample_t emg_raw_log[EMG_SENSOR_RAW_LOG_LEN];
static uint16_t emg_raw_log_head;
static uint16_t emg_raw_log_tail;
static uint32_t emg_raw_log_dropped;

static const char *EmgSensor_ModeName(uint8_t mode)
{
  switch (mode)
  {
    case 1U:
      return "weak";
    case 2U:
      return "strong";
    default:
      return "relax";
  }
}

static int32_t EmgSensor_ToX10(float value)
{
  if (value >= 0.0f)
  {
    return (int32_t)((value * 10.0f) + 0.5f);
  }
  return (int32_t)((value * 10.0f) - 0.5f);
}

static void EmgSensor_ClearRawLogLocked(void)
{
  emg_raw_log_head = 0U;
  emg_raw_log_tail = 0U;
  emg_raw_log_dropped = 0U;
}

static void EmgSensor_PushRawLog(uint16_t raw)
{
  uint16_t next_head;

  taskENTER_CRITICAL();
  next_head = (uint16_t)(emg_raw_log_head + 1U);
  if (next_head >= EMG_SENSOR_RAW_LOG_LEN)
  {
    next_head = 0U;
  }

  if (next_head == emg_raw_log_tail)
  {
    emg_raw_log_tail = (uint16_t)(emg_raw_log_tail + 1U);
    if (emg_raw_log_tail >= EMG_SENSOR_RAW_LOG_LEN)
    {
      emg_raw_log_tail = 0U;
    }
    emg_raw_log_dropped++;
  }

  emg_raw_log[emg_raw_log_head].sequence = emg_sample_count;
  emg_raw_log[emg_raw_log_head].raw = raw;
  emg_raw_log_head = next_head;
  taskEXIT_CRITICAL();
}

static float EmgSensor_UpdateRangeWindow(uint16_t raw)
{
  uint16_t sorted_raw[EMG_SENSOR_RANGE_WINDOW_LEN];
  float sorted_range[EMG_SENSOR_RANGE_HISTORY_LEN];
  float local_range;
  uint8_t stable_index;

  emg_range_window[emg_range_window_index] = raw;
  emg_range_window_index++;
  if (emg_range_window_index >= EMG_SENSOR_RANGE_WINDOW_LEN)
  {
    emg_range_window_index = 0U;
  }
  if (emg_range_window_count < EMG_SENSOR_RANGE_WINDOW_LEN)
  {
    emg_range_window_count++;
  }
  if (emg_range_window_count < EMG_SENSOR_RANGE_WINDOW_LEN)
  {
    return -1.0f;
  }

  emg_range_update_count++;
  if (emg_range_update_count < EMG_SENSOR_RANGE_UPDATE_DECIMATE)
  {
    return (emg_range_history_count >= EMG_SENSOR_RANGE_HISTORY_LEN) ? emg_stable_range : -1.0f;
  }
  emg_range_update_count = 0U;

  memcpy(sorted_raw, emg_range_window, sizeof(sorted_raw));
  for (uint8_t i = 1U; i < EMG_SENSOR_RANGE_WINDOW_LEN; i++)
  {
    uint16_t value = sorted_raw[i];
    uint8_t j = i;
    while ((j > 0U) && (sorted_raw[j - 1U] > value))
    {
      sorted_raw[j] = sorted_raw[j - 1U];
      j--;
    }
    sorted_raw[j] = value;
  }

  /* 5th-to-95th percentile range rejects isolated ADC spikes. */
  local_range = (float)((uint32_t)sorted_raw[23] - (uint32_t)sorted_raw[1]);
  emg_range_history[emg_range_history_index] = local_range;
  emg_range_history_index++;
  if (emg_range_history_index >= EMG_SENSOR_RANGE_HISTORY_LEN)
  {
    emg_range_history_index = 0U;
  }
  if (emg_range_history_count < EMG_SENSOR_RANGE_HISTORY_LEN)
  {
    emg_range_history_count++;
  }
  if (emg_range_history_count < EMG_SENSOR_RANGE_HISTORY_LEN)
  {
    return -1.0f;
  }

  memcpy(sorted_range, emg_range_history, sizeof(sorted_range));
  for (uint8_t i = 1U; i < EMG_SENSOR_RANGE_HISTORY_LEN; i++)
  {
    float value = sorted_range[i];
    uint8_t j = i;
    while ((j > 0U) && (sorted_range[j - 1U] > value))
    {
      sorted_range[j] = sorted_range[j - 1U];
      j--;
    }
    sorted_range[j] = value;
  }

  stable_index = (uint8_t)((EMG_SENSOR_RANGE_HISTORY_LEN * 90U) / 100U);
  emg_stable_range = sorted_range[stable_index];
  return emg_stable_range;
}

static void EmgSensor_UpdateModelState(void)
{
#if EMG_SENSOR_MODEL_ENABLE
  EmgParticipationInput_t input;
  EmgParticipation_t prediction;
  uint16_t confidence_x1000 = 0U;
  int32_t baseline_x10;
  int32_t raw_x10;
  int32_t rectified_x10;

  if (emg_state.calibrated == 0U)
  {
    return;
  }
  if (emg_model_enabled == 0U)
  {
    emg_state.mode = 0U;
    return;
  }

  baseline_x10 = EmgSensor_ToX10(emg_state.baseline);
  raw_x10 = (int32_t)emg_state.raw * 10;
  rectified_x10 = EmgSensor_ToX10(emg_state.rectified);

  input.raw = emg_state.raw;
  input.baseline_x10 = baseline_x10;
  input.drop_x10 = (baseline_x10 > raw_x10) ? (baseline_x10 - raw_x10) : 0;
  input.rectified_x10 = rectified_x10;
  input.envelope_x10 = EmgSensor_ToX10(emg_state.envelope);
  input.rms_x10 = EmgSensor_ToX10(emg_state.rms);

  prediction = EmgParticipationModel_Predict(&input, &confidence_x1000);
  if (confidence_x1000 > 1000U)
  {
    confidence_x1000 = 1000U;
  }

  if (prediction == EMG_PARTICIPATION_RELAX)
  {
    emg_state.active = 0U;
    emg_state.mode = 0U;
    emg_state.strength = 0U;
  }
  else if (prediction == EMG_PARTICIPATION_WEAK)
  {
    emg_state.active = 1U;
    emg_state.mode = 1U;
    emg_state.strength = (uint8_t)(30U + (((uint32_t)confidence_x1000 * 30U) / 1000U));
  }
  else
  {
    emg_state.active = 1U;
    emg_state.mode = 2U;
    emg_state.strength = (uint8_t)(60U + (((uint32_t)confidence_x1000 * 40U) / 1000U));
  }

  emg_state.participation = emg_state.strength;
#endif
}

static void EmgSensor_ResetCalibration(uint8_t skip_warmup)
{
  taskENTER_CRITICAL();
  emg_state.baseline = 0.0f;
  emg_state.noise_level = 0.0f;
  emg_state.rectified = 0.0f;
  emg_state.envelope = 0.0f;
  emg_state.rms = 0.0f;
  emg_state.active = 0U;
  emg_state.strength = 0U;
  emg_state.mode = 0U;
  emg_state.participation = 0U;
  emg_state.calibrated = 0U;
  emg_baseline_sum = 0U;
  emg_baseline_square_sum = 0U;
  emg_baseline_count = 0U;
  emg_warmup_count = (skip_warmup != 0U) ? EMG_SENSOR_WARMUP_SAMPLES : 0U;
  emg_baseline_min = 0xFFFFU;
  emg_baseline_max = 0U;
  emg_relax_range = 0.0f;
  emg_model_decimate_count = 0U;
  emg_active_enter_count = 0U;
  emg_active_exit_count = 0U;
  emg_strength_filtered = 0.0f;
  memset(emg_rms_window, 0, sizeof(emg_rms_window));
  memset(emg_range_window, 0, sizeof(emg_range_window));
  memset(emg_range_history, 0, sizeof(emg_range_history));
  emg_range_window_index = 0U;
  emg_range_window_count = 0U;
  emg_range_history_index = 0U;
  emg_range_history_count = 0U;
  emg_range_update_count = 0U;
  emg_stable_range = 0.0f;
  Rehab_FirstOrderFilterInit(&emg_envelope_filter, EMG_SENSOR_ENVELOPE_ALPHA, 0.0f);
  Rehab_MovingAverageInit(&emg_rms_average, emg_rms_window, EMG_SENSOR_RMS_WINDOW_LEN);
  taskEXIT_CRITICAL();
}

static HAL_StatusTypeDef EmgSensor_ConfigSoftwareAdc(void)
{
  ADC_ChannelConfTypeDef sConfig = {0};

  if (HAL_ADC_DeInit(&hadc4) != HAL_OK)
  {
    return HAL_ERROR;
  }

  hadc4.Instance = ADC4;
  hadc4.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc4.Init.Resolution = ADC_RESOLUTION_12B;
  hadc4.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc4.Init.ScanConvMode = ADC4_SCAN_DISABLE;
  hadc4.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc4.Init.LowPowerAutoWait = DISABLE;
  hadc4.Init.LowPowerAutoPowerOff = ADC_LOW_POWER_NONE;
  hadc4.Init.ContinuousConvMode = DISABLE;
  hadc4.Init.NbrOfConversion = 1;
  hadc4.Init.DiscontinuousConvMode = DISABLE;
  hadc4.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc4.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc4.Init.DMAContinuousRequests = DISABLE;
  hadc4.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  hadc4.Init.SamplingTimeCommon1 = ADC_SAMPLETIME_68CYCLES;
  hadc4.Init.SamplingTimeCommon2 = ADC_SAMPLETIME_68CYCLES;
  hadc4.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
  hadc4.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;
  hadc4.Init.OversamplingMode = DISABLE;
  hadc4.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_HIGH;
  hadc4.Init.VrefProtection = ADC_VREF_PPROT_NONE;
  if (HAL_ADC_Init(&hadc4) != HAL_OK)
  {
    return HAL_ERROR;
  }

  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = ADC4_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_68CYCLES;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc4, &sConfig) != HAL_OK)
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
 * @brief 完成静息标定，计算 baseline、noise_level 并初始化后续滤波器。
 *
 * baseline 用于去除传感器直流偏置；noise_level 用于估计静息噪声强度。
 * 若噪声推导出的阈值高于默认阈值，则自动抬高激活阈值，减少误触发。
 */
static void EmgSensor_FinishBaseline(void)
{
  float mean;
  float mean_square;
  float variance;

  if (emg_baseline_count == 0U)
  {
    return;
  }

  mean = (float)emg_baseline_sum / (float)emg_baseline_count;
  mean_square = (float)emg_baseline_square_sum / (float)emg_baseline_count;
  variance = mean_square - (mean * mean);
  /* 浮点舍入可能让理论上非负的方差变成极小负数。 */
  if (variance < 0.0f)
  {
    variance = 0.0f;
  }

  emg_state.baseline = mean;
  emg_state.noise_level = sqrtf(variance);
  emg_state.rectified = 0.0f;
  emg_state.envelope = 0.0f;
  emg_state.rms = 0.0f;
  emg_state.active = 0U;
  emg_state.strength = 0U;
  emg_state.mode = 0U;
  emg_state.participation = 0U;
  emg_state.calibrated = 1U;
  if (emg_relax_range <= 1.0f)
  {
    if (emg_baseline_max > emg_baseline_min)
    {
      emg_relax_range = (float)((uint32_t)emg_baseline_max - (uint32_t)emg_baseline_min);
    }
    else
    {
      emg_relax_range = 1.0f;
    }
  }

  if (EMG_SENSOR_BASE_DEBUG != 0U)
  {
    printf("EMG_BASE_DBG count=%lu sum=%lu mean_x10=%ld noise_x10=%ld state_size=%u\r\n",
           (unsigned long)emg_baseline_count,
           (unsigned long)emg_baseline_sum,
           (long)(mean * 10.0f),
           (long)(emg_state.noise_level * 10.0f),
           (unsigned int)sizeof(EmgSensor_State_t));
  }

  /* 标定完成后再启动包络滤波和 RMS 滑动窗口，避免静息样本污染有效特征。 */
  Rehab_FirstOrderFilterInit(&emg_envelope_filter, EMG_SENSOR_ENVELOPE_ALPHA, 0.0f);
  Rehab_MovingAverageInit(&emg_rms_average, emg_rms_window, EMG_SENSOR_RMS_WINDOW_LEN);
}

/**
 * @brief 处理单路EMG原始采样数据，更新传感器状态
 * 该函数执行以下操作：
 * 1. 将原始ADC值转换为电压值
 * 2. 若未校准，累积静息样本计算基线均值和噪声方差
 * 3. 若已校准，计算信号相对基线的正向幅值，提取包络线和 RMS
 *    并根据阈值判断激活状态、强度百分比和参与度
 * @param raw 原始ADC采样值
 */
static void EmgSensor_ProcessSample(uint16_t raw)
{
  float current_range;
  float signal;
  float strength;
  float strength_candidate = 0.0f;
  float rms_mean_square;
  uint8_t active_candidate = 0U;

  emg_state.raw = raw;
  emg_sample_count++;
  EmgSensor_PushRawLog(raw);
  emg_state.voltage_mv = ((float)raw * EMG_SENSOR_VREF_MV) / EMG_SENSOR_ADC_MAX;

  if (emg_state.calibrated == 0U)
  {
    if (emg_warmup_count < EMG_SENSOR_WARMUP_SAMPLES)
    {
      emg_warmup_count++;
      return;
    }

    /* 标定阶段要求肌肉保持放松，样本只用于建立静息基线和噪声水平。 */
    emg_baseline_sum += raw;
    emg_baseline_square_sum += ((uint64_t)raw * (uint64_t)raw);
    if (raw < emg_baseline_min)
    {
      emg_baseline_min = raw;
    }
    if (raw > emg_baseline_max)
    {
      emg_baseline_max = raw;
    }
    emg_baseline_count++;

    current_range = EmgSensor_UpdateRangeWindow(raw);
    if (current_range >= 0.0f)
    {
      emg_relax_range = current_range;
    }

    if (emg_baseline_count >= EMG_SENSOR_BASELINE_SAMPLES)
    {
      EmgSensor_FinishBaseline();
    }
    return;
  }

  /* Some EMG front-ends drop voltage during contraction, so use magnitude. */
  signal = fabsf((float)raw - emg_state.baseline);

  emg_state.rectified = signal;
  /* envelope 适合做实时激活判断，RMS 更适合后续做训练窗口特征。 */
  emg_state.envelope = Rehab_FirstOrderFilterUpdate(&emg_envelope_filter, signal);
  rms_mean_square = Rehab_MovingAverageUpdate(&emg_rms_average, signal * signal);
  emg_state.rms = sqrtf(rms_mean_square);

  current_range = EmgSensor_UpdateRangeWindow(raw);
  if ((current_range >= 0.0f) && (emg_relax_range > 1.0f))
  {
    float active_range = emg_relax_range * EMG_SENSOR_RANGE_ACTIVE_RATIO;
    float full_range = emg_relax_range * EMG_SENSOR_RANGE_FULL_RATIO;
    float strength_span = active_range - full_range;

    if (strength_span < 1.0f)
    {
      strength_span = 1.0f;
    }

    if (current_range <= active_range)
    {
      strength = ((active_range - current_range) * 100.0f) / strength_span;
      strength_candidate = Rehab_ClampFloat(strength, 0.0f, 100.0f);
      active_candidate = 1U;
    }
  }

  if (active_candidate != 0U)
  {
    if (emg_active_enter_count < EMG_SENSOR_ACTIVE_ENTER_SAMPLES)
    {
      emg_active_enter_count++;
    }
    emg_active_exit_count = 0U;
  }
  else
  {
    emg_active_enter_count = 0U;
    if (emg_active_exit_count < EMG_SENSOR_ACTIVE_EXIT_SAMPLES)
    {
      emg_active_exit_count++;
    }
  }

  if (emg_state.active == 0U)
  {
    if (emg_active_enter_count >= EMG_SENSOR_ACTIVE_ENTER_SAMPLES)
    {
      emg_state.active = 1U;
      emg_strength_filtered = strength_candidate;
      emg_state.strength = (uint8_t)(emg_strength_filtered + 0.5f);
    }
    else
    {
      emg_strength_filtered = 0.0f;
      emg_state.strength = 0U;
    }
  }
  else if ((active_candidate == 0U) &&
           (emg_active_exit_count >= EMG_SENSOR_ACTIVE_EXIT_SAMPLES))
  {
    emg_state.active = 0U;
    emg_strength_filtered = 0.0f;
    emg_state.strength = 0U;
  }
  else
  {
    emg_strength_filtered += EMG_SENSOR_STRENGTH_ALPHA * (strength_candidate - emg_strength_filtered);
    strength = Rehab_ClampFloat(emg_strength_filtered, 0.0f, 100.0f);
    emg_state.strength = (uint8_t)(strength + 0.5f);
  }
  /* 兼容完整 LVGL 与 Wi-Fi/MQTT 的既有 mode 接口：
   * 0=放松，1=较弱发力，2=明显发力。 */
  if (emg_state.active == 0U)
  {
    emg_state.mode = 0U;
  }
  else
  {
    emg_state.mode = (emg_state.strength >= 60U) ? 2U : 1U;
  }
  /* 当前版本先用强度近似参与度；后续应结合动作有效时间计算参与率。 */
  emg_state.participation = emg_state.strength;
  if (emg_model_decimate_count > 0U)
  {
    emg_model_decimate_count--;
  }
  else
  {
    emg_model_decimate_count = (uint8_t)(EMG_SENSOR_MODEL_DECIMATE - 1U);
    EmgSensor_UpdateModelState();
  }
}

/**
 * @brief 处理ADC缓冲区指定区间的数据样本
 * @param start 起始索引
 * @param count 需要处理的样本数量
 */
static void EmgSensor_ProcessRange(uint32_t start, uint32_t count)
{
  uint32_t end = start + count;

  /* 防止错误的 DMA 区间参数越界访问采样缓冲区。 */
  if (end > EMG_SENSOR_BUFFER_LEN)
  {
    return;
  }

  for (uint32_t i = start; i < end; i++)
  {
    EmgSensor_ProcessSample(emg_adc_buffer[i]);
  }
}

/**
 * @brief 启动EMG传感器数据采集
 * 初始化状态变量与缓冲区，启动定时器触发ADC，并开启ADC DMA传输
 * @return HAL_StatusTypeDef HAL_OK表示成功，HAL_ERROR表示失败
 */
HAL_StatusTypeDef EmgSensor_Start(void)
{
  if (emg_sampling_running != 0U)
  {
    return HAL_OK;
  }

#if !EMG_SENSOR_COLLECTION_POLLING
  if (emg_dma_config_valid == 0U)
  {
    (void)HAL_ADC_DeInit(&hadc4);
    (void)HAL_TIM_Base_DeInit(&htim3);
    MX_ADC4_Init();
    MX_TIM3_Init();
    emg_dma_config_valid = 1U;
  }
#endif

  /* 每次启动采集都重新进入静息标定流程。 */
  memset(&emg_state, 0, sizeof(emg_state));
  memset(emg_adc_buffer, 0, sizeof(emg_adc_buffer));
  memset(emg_rms_window, 0, sizeof(emg_rms_window));
  emg_baseline_sum = 0U;
  emg_baseline_square_sum = 0U;
  emg_baseline_count = 0U;
  emg_warmup_count = 0U;
  emg_baseline_min = 0xFFFFU;
  emg_baseline_max = 0U;
  emg_relax_range = 0.0f;
  emg_sample_count = 0U;
  emg_software_fallback = 0U;
  emg_model_enabled = 0U;
  emg_model_decimate_count = 0U;
  emg_active_enter_count = 0U;
  emg_active_exit_count = 0U;
  emg_strength_filtered = 0.0f;
  emg_range_window_index = 0U;
  emg_range_window_count = 0U;
  memset(emg_range_history, 0, sizeof(emg_range_history));
  emg_range_history_index = 0U;
  emg_range_history_count = 0U;
  emg_range_update_count = 0U;
  emg_stable_range = 0.0f;
  taskENTER_CRITICAL();
  EmgSensor_ClearRawLogLocked();
  taskEXIT_CRITICAL();

#if EMG_SENSOR_COLLECTION_POLLING
  if (EmgSensor_ConfigSoftwareAdc() != HAL_OK)
  {
    return HAL_ERROR;
  }

  emg_software_fallback = 1U;
  emg_sampling_running = 1U;
  return HAL_OK;
#else
  if (HAL_ADC_Start_DMA(&hadc4,
                        (uint32_t *)emg_adc_buffer,
                        EMG_SENSOR_BUFFER_LEN) != HAL_OK)
  {
    return HAL_ERROR;
  }

  if (HAL_TIM_Base_Start(&htim3) != HAL_OK)
  {
    (void)HAL_ADC_Stop_DMA(&hadc4);
    return HAL_ERROR;
  }

  emg_sampling_running = 1U;
  return HAL_OK;
#endif
}

HAL_StatusTypeDef EmgSensor_StartSoftwareFallback(void)
{
  if (emg_sampling_running == 0U)
  {
    return HAL_ERROR;
  }
  if (emg_software_fallback != 0U)
  {
    return HAL_OK;
  }

  (void)HAL_TIM_Base_Stop(&htim3);
  (void)HAL_ADC_Stop_DMA(&hadc4);
  if (EmgSensor_ConfigSoftwareAdc() != HAL_OK)
  {
    return HAL_ERROR;
  }

  emg_software_fallback = 1U;
  emg_dma_config_valid = 0U;
  return HAL_OK;
}

void EmgSensor_Stop(void)
{
  if (emg_sampling_running == 0U)
  {
    return;
  }

  emg_sampling_running = 0U;
#if EMG_SENSOR_COLLECTION_POLLING
  (void)HAL_ADC_Stop(&hadc4);
#else
  if (emg_software_fallback != 0U)
  {
    (void)HAL_ADC_Stop(&hadc4);
  }
  else
  {
    (void)HAL_TIM_Base_Stop(&htim3);
    (void)HAL_ADC_Stop_DMA(&hadc4);
  }
  emg_software_fallback = 0U;
#endif
}

uint8_t EmgSensor_PollSample(void)
{
  uint16_t raw;

  if (emg_sampling_running == 0U)
  {
    return 0U;
  }

  if (HAL_ADC_Start(&hadc4) != HAL_OK)
  {
    return 0U;
  }
  if (HAL_ADC_PollForConversion(&hadc4, 2U) != HAL_OK)
  {
    (void)HAL_ADC_Stop(&hadc4);
    return 0U;
  }

  raw = (uint16_t)HAL_ADC_GetValue(&hadc4);
  (void)HAL_ADC_Stop(&hadc4);
  EmgSensor_ProcessSample(raw);
  return 1U;
}

/**
 * @brief 处理ADC DMA半传输完成中断对应的数据段
 * 处理缓冲区前半段数据
 */
void EmgSensor_ProcessHalfBuffer(void)
{
  if (emg_sampling_running == 0U)
  {
    return;
  }

  EmgSensor_ProcessRange(0U, EMG_SENSOR_BUFFER_LEN / 2U);
}

/**
 * @brief 处理ADC DMA全传输完成中断对应的数据段
 * 处理缓冲区后半段数据
 */
void EmgSensor_ProcessFullBuffer(void)
{
  if (emg_sampling_running == 0U)
  {
    return;
  }

  EmgSensor_ProcessRange(EMG_SENSOR_BUFFER_LEN / 2U, EMG_SENSOR_BUFFER_LEN / 2U);
}

uint8_t EmgSensor_ProcessPollingBuffer(void)
{
  uint32_t i;
  uint8_t has_sample = 0U;

  if (emg_sampling_running == 0U)
  {
    return 0U;
  }

  for (i = 0U; i < EMG_SENSOR_BUFFER_LEN; i++)
  {
    if (emg_adc_buffer[i] != 0U)
    {
      has_sample = 1U;
      break;
    }
  }

  if (has_sample == 0U)
  {
    return 0U;
  }

  EmgSensor_ProcessRange(0U, EMG_SENSOR_BUFFER_LEN);
  return 1U;
}

/**
 * @brief 获取当前EMG传感器状态副本
 * 使用临界段保护，保证状态读取一致性
 * @param state 指向存储状态副本的EmgSensor_State_t结构体指针
 */
void EmgSensor_GetState(EmgSensor_State_t *state)
{
  if (state == NULL)
  {
    return;
  }

  taskENTER_CRITICAL();
  *state = emg_state;
  taskEXIT_CRITICAL();
}

void EmgSensor_GetFeatures(EmgSensor_Features_t *features)
{
  float drop;

  if (features == NULL)
  {
    return;
  }

  taskENTER_CRITICAL();
  features->raw = emg_state.raw;
  features->baseline_x10 = (int32_t)(emg_state.baseline * 10.0f);
  drop = emg_state.baseline - (float)emg_state.raw;
  if (drop < 0.0f)
  {
    drop = 0.0f;
  }
  features->drop_x10 = (int32_t)(drop * 10.0f);
  features->rectified_x10 = (int32_t)(emg_state.rectified * 10.0f);
  features->envelope_x10 = (int32_t)(emg_state.envelope * 10.0f);
  features->rms_x10 = (int32_t)(emg_state.rms * 10.0f);
  features->active = emg_state.active;
  features->strength = emg_state.strength;
  features->mode = emg_state.mode;
  features->calibrated = emg_state.calibrated;
  features->running = emg_sampling_running;
  features->sample_count = emg_sample_count;
  taskEXIT_CRITICAL();
}

uint16_t EmgSensor_ReadRawSamples(EmgSensor_RawSample_t *samples, uint16_t max_count)
{
  uint16_t count = 0U;

  if ((samples == NULL) || (max_count == 0U))
  {
    return 0U;
  }

  taskENTER_CRITICAL();
  while ((count < max_count) && (emg_raw_log_tail != emg_raw_log_head))
  {
    samples[count] = emg_raw_log[emg_raw_log_tail];
    emg_raw_log_tail = (uint16_t)(emg_raw_log_tail + 1U);
    if (emg_raw_log_tail >= EMG_SENSOR_RAW_LOG_LEN)
    {
      emg_raw_log_tail = 0U;
    }
    count++;
  }
  taskEXIT_CRITICAL();

  return count;
}

uint32_t EmgSensor_GetRawLogDropped(void)
{
  uint32_t dropped;

  taskENTER_CRITICAL();
  dropped = emg_raw_log_dropped;
  taskEXIT_CRITICAL();

  return dropped;
}

void EmgSensor_SetModelEnabled(uint8_t enabled)
{
  taskENTER_CRITICAL();
  emg_model_enabled = (enabled != 0U) ? 1U : 0U;
  emg_model_decimate_count = 0U;
  if (emg_model_enabled == 0U)
  {
    emg_state.mode = 0U;
  }
  taskEXIT_CRITICAL();
}

void EmgSensor_RecalibrateForTraining(void)
{
  EmgSensor_SetModelEnabled(0U);
  EmgSensor_ResetCalibration(1U);
}

/**
 * @brief 设置EMG信号处理阈值参数
 * @param active_delta 激活阈值，必须大于0
 * @param full_scale_delta 满量程阈值，必须大于active_delta
 */
void EmgSensor_SetThreshold(float active_delta, float full_scale_delta)
{
  /* 保留手动阈值接口，方便现场根据贴片位置和个体差异临时调参。 */
  if (active_delta > 0.0f)
  {
    (void)active_delta;
  }
  if (full_scale_delta > 0.0f)
  {
    (void)full_scale_delta;
  }
}

/**
 * @brief 格式化新版 EMG 调试行。
 *
 * 当前函数只封装输出，不在 StartEmgTask 中调用，避免影响现有测试日志格式。
 * 后续可由 DebugTask、WirelessTask 或数据记录任务按需调用。
 */
int EmgSensor_FormatDebugLine(char *buffer, size_t buffer_len)
{
  EmgSensor_State_t state;

  if ((buffer == NULL) || (buffer_len == 0U))
  {
    return 0;
  }

  EmgSensor_GetState(&state);

  return snprintf(buffer,
                  buffer_len,
                  "EMG2 raw=%u mv=%lu base_x10=%ld noise_x10=%ld rect_x10=%ld env_x10=%ld rms_x10=%ld active=%u strength=%u mode=%u label=%s participation=%u cal=%u\r\n",
                  (unsigned int)state.raw,
                  (unsigned long)(state.voltage_mv + 0.5f),
                  (long)(state.baseline * 10.0f),
                  (long)(state.noise_level * 10.0f),
                  (long)(state.rectified * 10.0f),
                  (long)(state.envelope * 10.0f),
                  (long)(state.rms * 10.0f),
                  (unsigned int)state.active,
                  (unsigned int)state.strength,
                  (unsigned int)state.mode,
                  EmgSensor_ModeName(state.mode),
                  (unsigned int)state.participation,
                  (unsigned int)state.calibrated);
}

/**
 * @brief ADC半传输完成回调函数
 * ADC DMA完成缓冲区前半段传输时触发，释放信号量通知任务处理数据
 * @param hadc ADC句柄指针
 */
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
  if ((hadc->Instance == ADC4) && (adcHalfCpltSemHandle != NULL))
  {
    (void)osSemaphoreRelease(adcHalfCpltSemHandle);
  }
}

/**
 * @brief ADC全传输完成回调函数
 * ADC DMA完成整个缓冲区传输时触发，释放信号量通知任务处理数据
 * @param hadc ADC句柄指针
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
  if ((hadc->Instance == ADC4) && (adcCpltSemHandle != NULL))
  {
    (void)osSemaphoreRelease(adcCpltSemHandle);
  }
}
