#ifndef __EMG_SENSOR_H__
#define __EMG_SENSOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stddef.h>
#include <stdint.h>

#define EMG_SENSOR_BUFFER_LEN           (64U)
#define EMG_SENSOR_SAMPLE_RATE_HZ       (100U)
/* Match the verified jidian_ceshi acquisition path: TIM3-triggered ADC4 DMA.
 * StartEmgTask retains a timeout fallback to 100 Hz polling if DMA produces
 * no samples, so the full product remains robust. */
#define EMG_SENSOR_COLLECTION_POLLING   (0U)

typedef struct
{
  uint16_t raw;
  float baseline;
  float noise_level;
  float rectified;
  float envelope;
  float rms;
  float voltage_mv;
  uint8_t active;
  uint8_t strength;
  uint8_t mode;
  uint8_t participation;
  uint8_t calibrated;
} EmgSensor_State_t;

typedef struct
{
  uint16_t raw;
  int32_t baseline_x10;
  int32_t drop_x10;
  int32_t rectified_x10;
  int32_t envelope_x10;
  int32_t rms_x10;
  uint8_t active;
  uint8_t strength;
  uint8_t mode;
  uint8_t calibrated;
  uint8_t running;
  uint32_t sample_count;
} EmgSensor_Features_t;

typedef struct
{
  uint32_t sequence;
  uint16_t raw;
} EmgSensor_RawSample_t;

HAL_StatusTypeDef EmgSensor_Start(void);
HAL_StatusTypeDef EmgSensor_StartSoftwareFallback(void);
void EmgSensor_Stop(void);
uint8_t EmgSensor_PollSample(void);
void EmgSensor_ProcessHalfBuffer(void);
void EmgSensor_ProcessFullBuffer(void);
uint8_t EmgSensor_ProcessPollingBuffer(void);
void EmgSensor_GetState(EmgSensor_State_t *state);
void EmgSensor_GetFeatures(EmgSensor_Features_t *features);
uint16_t EmgSensor_ReadRawSamples(EmgSensor_RawSample_t *samples, uint16_t max_count);
uint32_t EmgSensor_GetRawLogDropped(void);
void EmgSensor_SetModelEnabled(uint8_t enabled);
void EmgSensor_RecalibrateForTraining(void);
int EmgSensor_FormatDebugLine(char *buffer, size_t buffer_len);

#ifdef __cplusplus
}
#endif

#endif /* __EMG_SENSOR_H__ */
