#include "vibration_motor.h"

#include "main.h"
#include "cmsis_os2.h"

static uint8_t vibration_pulse_active;
static uint32_t vibration_pulse_end_tick;

void Vibration_Init(void)
{
  vibration_pulse_active = 0U;
  vibration_pulse_end_tick = 0U;
  Vibration_Off();
}

void Vibration_On(void)
{
  HAL_GPIO_WritePin(VIBRATION_GPIO_Port, VIBRATION_Pin, GPIO_PIN_SET);
}

void Vibration_Off(void)
{
  HAL_GPIO_WritePin(VIBRATION_GPIO_Port, VIBRATION_Pin, GPIO_PIN_RESET);
}

void Vibration_Set(uint8_t enable)
{
  if (enable != 0U)
  {
    Vibration_On();
  }
  else
  {
    Vibration_Off();
  }
}

void Vibration_Pulse(uint32_t on_ms)
{
  uint32_t now;

  if (on_ms == 0U)
  {
    return;
  }

  now = osKernelGetTickCount();
  vibration_pulse_active = 1U;
  vibration_pulse_end_tick = now + on_ms;
  Vibration_On();
}

void Vibration_Update(uint32_t now)
{
  if ((vibration_pulse_active != 0U) &&
      ((int32_t)(now - vibration_pulse_end_tick) >= 0))
  {
    vibration_pulse_active = 0U;
    vibration_pulse_end_tick = 0U;
    Vibration_Off();
  }
}

void Vibration_PulseBlocking(uint32_t on_ms)
{
  if (on_ms == 0U)
  {
    return;
  }

  Vibration_On();
  osDelay(on_ms);
  Vibration_Off();
}

void Vibration_PatternBlocking(uint32_t on_ms, uint32_t off_ms, uint8_t repeat)
{
  uint8_t i;

  if ((on_ms == 0U) || (repeat == 0U))
  {
    return;
  }

  for (i = 0U; i < repeat; i++)
  {
    Vibration_On();
    osDelay(on_ms);
    Vibration_Off();

    if ((off_ms != 0U) && ((uint8_t)(i + 1U) < repeat))
    {
      osDelay(off_ms);
    }
  }
}
