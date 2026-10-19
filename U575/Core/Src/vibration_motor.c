#include "vibration_motor.h"

#include "main.h"
#include "cmsis_os2.h"
#include "stm32u5xx_hal.h"

#define VIBRATION_ACTIVE_LEVEL    GPIO_PIN_SET
#define VIBRATION_INACTIVE_LEVEL  GPIO_PIN_RESET
#define VIBRATION_PROMPT_MS       250U

static void VibrationMotor_Delay(uint32_t duration_ms)
{
    osKernelState_t state = osKernelGetState();

    if ((state == osKernelRunning) || (state == osKernelLocked)) {
        osDelay(duration_ms);
    } else {
        HAL_Delay(duration_ms);
    }
}

void VibrationMotor_Init(void)
{
    VibrationMotor_Off();
}

void VibrationMotor_On(void)
{
    HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, VIBRATION_ACTIVE_LEVEL);
}

void VibrationMotor_Off(void)
{
    HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, VIBRATION_INACTIVE_LEVEL);
}

void VibrationMotor_PulseBlocking(uint32_t duration_ms)
{
    if (duration_ms == 0U) {
        return;
    }

    VibrationMotor_On();
    VibrationMotor_Delay(duration_ms);
    VibrationMotor_Off();
}

void VibrationMotor_Prompt(void)
{
    VibrationMotor_PulseBlocking(VIBRATION_PROMPT_MS);
}
