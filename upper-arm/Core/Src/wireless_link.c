/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    wireless_link.c
  * @brief   Wireless UART packet parser for wrist node data.
  ******************************************************************************
  */
/* USER CODE END Header */

#include "wireless_link.h"

static uint8_t rx_buf[WIRELESS_WRIST_FRAME_SIZE];
static uint8_t rx_index;
static uint8_t rx_expected_size;
static WirelessWristFrame_t latest_frame;
static WirelessHealthFrame_t latest_health_frame;
static uint8_t latest_valid;
static uint8_t latest_health_valid;
static WirelessLinkStats_t link_stats;

static uint16_t ReadU16Le(const uint8_t *buf)
{
  return (uint16_t)(((uint16_t)buf[1] << 8) | (uint16_t)buf[0]);
}

static int16_t ReadI16Le(const uint8_t *buf)
{
  return (int16_t)ReadU16Le(buf);
}

static uint32_t ReadU32Le(const uint8_t *buf)
{
  return ((uint32_t)buf[3] << 24) |
         ((uint32_t)buf[2] << 16) |
         ((uint32_t)buf[1] << 8) |
         (uint32_t)buf[0];
}

static uint8_t CalcChecksum(const uint8_t *buf, uint8_t len)
{
  uint8_t sum = 0U;

  for (uint8_t i = 0U; i < len; i++)
  {
    sum = (uint8_t)(sum + buf[i]);
  }
  return sum;
}

static void DecodeFrame(const uint8_t *buf, WirelessWristFrame_t *frame)
{
  frame->seq = ReadU16Le(&buf[1]);
  frame->tick = ReadU32Le(&buf[3]);
  frame->acc_mg[0] = ReadI16Le(&buf[7]);
  frame->acc_mg[1] = ReadI16Le(&buf[9]);
  frame->acc_mg[2] = ReadI16Le(&buf[11]);
  frame->gyro_dps_x10[0] = ReadI16Le(&buf[13]);
  frame->gyro_dps_x10[1] = ReadI16Le(&buf[15]);
  frame->gyro_dps_x10[2] = ReadI16Le(&buf[17]);
  frame->angle_x100[0] = ReadI16Le(&buf[19]);
  frame->angle_x100[1] = ReadI16Le(&buf[21]);
  frame->angle_x100[2] = ReadI16Le(&buf[23]);
  frame->heart_rate = ReadU16Le(&buf[25]);
  frame->status = buf[27];
  frame->battery_percent = buf[28];
  frame->status2 = buf[29];
}

static void DecodeHealthFrame(const uint8_t *buf, WirelessHealthFrame_t *frame)
{
  frame->seq = ReadU16Le(&buf[1]);
  frame->tick = ReadU32Le(&buf[3]);
  frame->heart_rate = buf[7];
  frame->spo2 = buf[8];
  frame->micro_circ = buf[9];
  frame->fatigue = buf[10];
  frame->sbp = buf[11];
  frame->dbp = buf[12];
  frame->sdnn = buf[13];
  frame->rmssd = buf[14];
  frame->status = buf[15];
}

void WirelessLink_Init(void)
{
  rx_index = 0U;
  rx_expected_size = 0U;
  latest_valid = 0U;
  latest_health_valid = 0U;
  link_stats.rx_bytes = 0U;
  link_stats.frames_ok = 0U;
  link_stats.health_frames_ok = 0U;
  link_stats.checksum_errors = 0U;
  link_stats.resync_count = 0U;
}

uint8_t WirelessLink_PushByte(uint8_t byte, WirelessWristFrame_t *frame)
{
  link_stats.rx_bytes++;

  if (rx_index == 0U)
  {
    if (byte == WIRELESS_FRAME_HEADER)
    {
      rx_expected_size = WIRELESS_WRIST_FRAME_SIZE;
    }
    else if (byte == WIRELESS_HEALTH_FRAME_HEADER)
    {
      rx_expected_size = WIRELESS_HEALTH_FRAME_SIZE;
    }
    else
    {
      link_stats.resync_count++;
      return 0U;
    }
  }
  rx_buf[rx_index++] = byte;

  if (rx_index < rx_expected_size)
  {
    return 0U;
  }

  rx_index = 0U;
  if (CalcChecksum(rx_buf, rx_expected_size - 1U) != rx_buf[rx_expected_size - 1U])
  {
    link_stats.checksum_errors++;
    rx_expected_size = 0U;
    return 0U;
  }

  if (rx_buf[0] == WIRELESS_HEALTH_FRAME_HEADER)
  {
    DecodeHealthFrame(rx_buf, &latest_health_frame);
    latest_health_valid = 1U;
    link_stats.health_frames_ok++;
    rx_expected_size = 0U;
    return 0U;
  }

  DecodeFrame(rx_buf, &latest_frame);
  latest_valid = 1U;
  link_stats.frames_ok++;
  rx_expected_size = 0U;

  if (frame != NULL)
  {
    *frame = latest_frame;
  }
  return 1U;
}

uint8_t WirelessLink_GetLatest(WirelessWristFrame_t *frame)
{
  if ((latest_valid == 0U) || (frame == NULL))
  {
    return 0U;
  }

  *frame = latest_frame;
  return 1U;
}

uint8_t WirelessLink_GetLatestHealth(WirelessHealthFrame_t *frame)
{
  if ((latest_health_valid == 0U) || (frame == NULL))
  {
    return 0U;
  }

  *frame = latest_health_frame;
  return 1U;
}

void WirelessLink_GetStats(WirelessLinkStats_t *stats)
{
  if (stats != NULL)
  {
    *stats = link_stats;
  }
}
