#ifndef __VIBRATION_MOTOR_H__
#define __VIBRATION_MOTOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void VibrationMotor_Init(void);
void VibrationMotor_On(void);
void VibrationMotor_Off(void);
void VibrationMotor_PulseBlocking(uint32_t duration_ms);
void VibrationMotor_Prompt(void);

#ifdef __cplusplus
}
#endif

#endif /* __VIBRATION_MOTOR_H__ */
