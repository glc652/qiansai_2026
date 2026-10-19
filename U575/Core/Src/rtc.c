#include "rtc.h"

#define RTC_INIT_TIMEOUT_MS 1000U
#define RTC_LSI_TIMEOUT_MS  1000U

static uint8_t rtc_ready;

static uint8_t rtc_bcd_to_u8(uint32_t tens, uint32_t units)
{
  return (uint8_t)((tens * 10U) + units);
}

static uint8_t rtc_u8_to_bcd(uint8_t value)
{
  return (uint8_t)(((value / 10U) << 4) | (value % 10U));
}

static uint32_t rtc_wait_flag_set(volatile uint32_t *reg, uint32_t mask, uint32_t timeout_ms)
{
  uint32_t start = HAL_GetTick();

  while (((*reg) & mask) == 0U)
  {
    if ((HAL_GetTick() - start) > timeout_ms)
    {
      return 0U;
    }
  }
  return 1U;
}

static void rtc_unlock_write_protection(void);
static void rtc_lock_write_protection(void);
static uint32_t rtc_enter_init_mode(void);
static void rtc_exit_init_mode(void);

uint8_t RTC_SetTime(uint8_t hour, uint8_t minute, uint8_t second)
{
  uint8_t hour_bcd;
  uint8_t minute_bcd;
  uint8_t second_bcd;

  if ((hour > 23U) || (minute > 59U) || (second > 59U) ||
      (rtc_ready == 0U) ||
      ((RTC->ICSR & RTC_ICSR_INITS) == 0U))
  {
    return 0U;
  }

  hour_bcd = rtc_u8_to_bcd(hour);
  minute_bcd = rtc_u8_to_bcd(minute);
  second_bcd = rtc_u8_to_bcd(second);

  rtc_unlock_write_protection();
  if (rtc_enter_init_mode() == 0U)
  {
    rtc_lock_write_protection();
    return 0U;
  }

  RTC->TR = (((uint32_t)((hour_bcd >> 4) & 0x03U) << RTC_TR_HT_Pos) |
             ((uint32_t)(hour_bcd & 0x0FU) << RTC_TR_HU_Pos) |
             ((uint32_t)((minute_bcd >> 4) & 0x07U) << RTC_TR_MNT_Pos) |
             ((uint32_t)(minute_bcd & 0x0FU) << RTC_TR_MNU_Pos) |
             ((uint32_t)((second_bcd >> 4) & 0x07U) << RTC_TR_ST_Pos) |
             ((uint32_t)(second_bcd & 0x0FU) << RTC_TR_SU_Pos));
  RTC->DR = ((2U << RTC_DR_YT_Pos) |
             (6U << RTC_DR_YU_Pos) |
             (1U << RTC_DR_WDU_Pos) |
             (1U << RTC_DR_MU_Pos) |
             (1U << RTC_DR_DU_Pos));

  rtc_exit_init_mode();
  rtc_lock_write_protection();
  return 1U;
}

static void rtc_unlock_write_protection(void)
{
  RTC->WPR = 0xCAU;
  RTC->WPR = 0x53U;
}

static void rtc_lock_write_protection(void)
{
  RTC->WPR = 0xFFU;
}

static uint32_t rtc_enter_init_mode(void)
{
  RTC->ICSR |= RTC_ICSR_INIT;
  return rtc_wait_flag_set(&RTC->ICSR, RTC_ICSR_INITF, RTC_INIT_TIMEOUT_MS);
}

static void rtc_exit_init_mode(void)
{
  RTC->ICSR &= ~RTC_ICSR_INIT;
}

void MX_RTC_Init(void)
{
  uint32_t rtcsel;

  rtc_ready = 0U;
  __HAL_RCC_PWR_CLK_ENABLE();
  PWR->DBPR |= PWR_DBPR_DBP;

  RCC->BDCR |= RCC_BDCR_LSION;
  if (rtc_wait_flag_set(&RCC->BDCR, RCC_BDCR_LSIRDY, RTC_LSI_TIMEOUT_MS) == 0U)
  {
    return;
  }

  rtcsel = RCC->BDCR & RCC_BDCR_RTCSEL;
  if (((RCC->BDCR & RCC_BDCR_RTCEN) == 0U) || (rtcsel != RCC_BDCR_RTCSEL_1))
  {
    RCC->BDCR |= RCC_BDCR_BDRST;
    RCC->BDCR &= ~RCC_BDCR_BDRST;
    RCC->BDCR |= RCC_BDCR_LSION;
    if (rtc_wait_flag_set(&RCC->BDCR, RCC_BDCR_LSIRDY, RTC_LSI_TIMEOUT_MS) == 0U)
    {
      return;
    }
    RCC->BDCR = (RCC->BDCR & ~RCC_BDCR_RTCSEL) | RCC_BDCR_RTCSEL_1;
    RCC->BDCR |= RCC_BDCR_RTCEN;
  }

  rtc_unlock_write_protection();
  if ((RTC->ICSR & RTC_ICSR_INITS) == 0U)
  {
    if (rtc_enter_init_mode() == 0U)
    {
      rtc_lock_write_protection();
      return;
    }

    RTC->CR &= ~RTC_CR_FMT;
    RTC->PRER = ((127U << RTC_PRER_PREDIV_A_Pos) | (249U << RTC_PRER_PREDIV_S_Pos));
    RTC->TR = 0U;
    RTC->DR = ((2U << 20) | (6U << 16) | (1U << 13) | (1U << 8) | 1U);
    rtc_exit_init_mode();
  }
  rtc_lock_write_protection();
  rtc_ready = 1U;
}

uint8_t RTC_GetHourMinute(uint8_t *hour, uint8_t *minute)
{
  uint32_t tr;

  if ((hour == NULL) || (minute == NULL) || (rtc_ready == 0U) ||
      ((RTC->ICSR & RTC_ICSR_INITS) == 0U))
  {
    return 0U;
  }

  tr = RTC->TR;
  *hour = rtc_bcd_to_u8((tr & RTC_TR_HT) >> RTC_TR_HT_Pos,
                        (tr & RTC_TR_HU) >> RTC_TR_HU_Pos);
  *minute = rtc_bcd_to_u8((tr & RTC_TR_MNT) >> RTC_TR_MNT_Pos,
                          (tr & RTC_TR_MNU) >> RTC_TR_MNU_Pos);
  return 1U;
}
