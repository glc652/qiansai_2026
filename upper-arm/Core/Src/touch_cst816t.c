#include "touch_cst816t.h"
#include "i2c.h"
#include "lcd_st7789.h"
#include <stdio.h>
#include <string.h>

#define FT6336_REG_GESTURE_ID   0x01U
#define FT6336_REG_TD_STATUS    0x02U
#define FT6336_REG_P1_XH        0x03U
#define FT6336_REG_ID_G_THGROUP 0x80U
#define FT6336_REG_ID_G_MODE    0xA4U
#define FT6336_REG_ID_G_FOCALTECH_ID 0xA8U
#define FT6336_REG_ID_G_LIB_VERSION_H 0xA1U
#define FT6336_REG_ID_G_LIB_VERSION_L 0xA2U
#define FT6336_REG_ID_G_CIPHER  0xA3U
#define FT6336_REG_ID_G_PERIODACTIVE 0x88U
#define FT6336_TIMEOUT_MS       50U
#define FT6336_DEBUG_PRINTF     0U

static HAL_StatusTypeDef FT6336_WriteReg(uint8_t reg, uint8_t value)
{
    HAL_StatusTypeDef status;

    status = HAL_I2C_Mem_Write(&hi2c1, FT6336_I2C_ADDR_8BIT, reg,
                               I2C_MEMADD_SIZE_8BIT, &value, 1U, FT6336_TIMEOUT_MS);
#if FT6336_DEBUG_PRINTF
    if (status != HAL_OK) {
        printf("FT6336 write reg=0x%02X val=0x%02X status=%d err=0x%08lX\r\n",
               (unsigned int)reg,
               (unsigned int)value,
               (int)status,
               (unsigned long)HAL_I2C_GetError(&hi2c1));
    }
#endif
    return status;
}

static HAL_StatusTypeDef FT6336_ReadReg(uint8_t reg, uint8_t *data, uint16_t len)
{
    HAL_StatusTypeDef status;

    status = HAL_I2C_Mem_Read(&hi2c1, FT6336_I2C_ADDR_8BIT, reg,
                              I2C_MEMADD_SIZE_8BIT, data, len, FT6336_TIMEOUT_MS);
#if FT6336_DEBUG_PRINTF
    if (status != HAL_OK) {
        printf("FT6336 read reg=0x%02X len=%u status=%d err=0x%08lX\r\n",
               (unsigned int)reg,
               (unsigned int)len,
               (int)status,
               (unsigned long)HAL_I2C_GetError(&hi2c1));
    }
#endif
    return status;
}

static void FT6336_Reset(void)
{
#if FT6336_DEBUG_PRINTF
    printf("FT6336 reset begin INT=%u RST=%u i2c_state=%lu err=0x%08lX\r\n",
           (unsigned int)HAL_GPIO_ReadPin(TP_INT_GPIO_Port, TP_INT_Pin),
           (unsigned int)HAL_GPIO_ReadPin(TP_RST_GPIO_Port, TP_RST_Pin),
           (unsigned long)HAL_I2C_GetState(&hi2c1),
           (unsigned long)HAL_I2C_GetError(&hi2c1));
#endif
    HAL_GPIO_WritePin(TP_RST_GPIO_Port, TP_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(20);
    HAL_GPIO_WritePin(TP_RST_GPIO_Port, TP_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(300);
#if FT6336_DEBUG_PRINTF
    printf("FT6336 reset end INT=%u RST=%u\r\n",
           (unsigned int)HAL_GPIO_ReadPin(TP_INT_GPIO_Port, TP_INT_Pin),
           (unsigned int)HAL_GPIO_ReadPin(TP_RST_GPIO_Port, TP_RST_Pin));
#endif
}

static void FT6336_Transform(uint16_t raw_x, uint16_t raw_y, uint16_t *x, uint16_t *y)
{
    uint16_t tx = raw_x;
    uint16_t ty = raw_y;

#if FT6336_SWAP_XY
    uint16_t tmp = tx;
    tx = ty;
    ty = tmp;
#endif

#if FT6336_INVERT_X
    if (tx < LCD_WIDTH) {
        tx = (uint16_t)(LCD_WIDTH - 1U - tx);
    }
#endif

#if FT6336_INVERT_Y
    if (ty < LCD_HEIGHT) {
        ty = (uint16_t)(LCD_HEIGHT - 1U - ty);
    }
#endif

    *x = tx;
    *y = ty;
}

HAL_StatusTypeDef FT6336_Init(void)
{
    HAL_StatusTypeDef status;
    uint8_t cipher = 0U;
    uint8_t focal_id = 0U;
    uint8_t lib_h = 0U;
    uint8_t lib_l = 0U;

#if FT6336_DEBUG_PRINTF
    printf("FT6336 init begin addr8=0x%02X swap=%u invx=%u invy=%u\r\n",
           (unsigned int)FT6336_I2C_ADDR_8BIT,
           (unsigned int)FT6336_SWAP_XY,
           (unsigned int)FT6336_INVERT_X,
           (unsigned int)FT6336_INVERT_Y);
#endif

    FT6336_Reset();

    status = HAL_I2C_IsDeviceReady(&hi2c1, FT6336_I2C_ADDR_8BIT, 3U, FT6336_TIMEOUT_MS);
#if FT6336_DEBUG_PRINTF
    printf("FT6336 ready status=%d state=%lu err=0x%08lX\r\n",
           (int)status,
           (unsigned long)HAL_I2C_GetState(&hi2c1),
           (unsigned long)HAL_I2C_GetError(&hi2c1));
#endif
    if (status != HAL_OK) {
        return status;
    }

    (void)FT6336_ReadReg(FT6336_REG_ID_G_CIPHER, &cipher, 1U);
    (void)FT6336_ReadReg(FT6336_REG_ID_G_FOCALTECH_ID, &focal_id, 1U);
    (void)FT6336_ReadReg(FT6336_REG_ID_G_LIB_VERSION_H, &lib_h, 1U);
    (void)FT6336_ReadReg(FT6336_REG_ID_G_LIB_VERSION_L, &lib_l, 1U);
#if FT6336_DEBUG_PRINTF
    printf("FT6336 ids cipher=0x%02X focal=0x%02X lib=0x%02X%02X\r\n",
           (unsigned int)cipher,
           (unsigned int)focal_id,
           (unsigned int)lib_h,
           (unsigned int)lib_l);
#endif

    (void)FT6336_WriteReg(FT6336_REG_ID_G_MODE, 0x00U);
    (void)FT6336_WriteReg(FT6336_REG_ID_G_THGROUP, 0x16U);
    (void)FT6336_WriteReg(FT6336_REG_ID_G_PERIODACTIVE, 0x0CU);

#if FT6336_DEBUG_PRINTF
    printf("FT6336 init end\r\n");
#endif

    return HAL_OK;
}

HAL_StatusTypeDef FT6336_ReadPoint(CST816T_TouchPoint_t *point)
{
    uint8_t data[6];
    uint8_t count;
    uint16_t raw_x;
    uint16_t raw_y;
    HAL_StatusTypeDef status;

    if (point == NULL) {
        return HAL_ERROR;
    }

    memset(point, 0, sizeof(*point));

    status = FT6336_ReadReg(FT6336_REG_GESTURE_ID, data, 2U);
    if (status != HAL_OK) {
        return status;
    }

    point->gesture = data[0];
    count = data[1] & 0x0FU;
    if ((count == 0U) || (count > 2U)) {
        return HAL_OK;
    }
    point->points = count;

    status = FT6336_ReadReg(FT6336_REG_P1_XH, data, 6U);
    if (status != HAL_OK) {
        return status;
    }

    point->event = (data[0] >> 6) & 0x03U;
    point->id = (data[2] >> 4) & 0x0FU;
    raw_x = (uint16_t)(((uint16_t)(data[0] & 0x0FU) << 8) | data[1]);
    raw_y = (uint16_t)(((uint16_t)(data[2] & 0x0FU) << 8) | data[3]);

    FT6336_Transform(raw_x, raw_y, &point->x, &point->y);
#if FT6336_DEBUG_PRINTF
    printf("FT6336 touch cnt=%u event=%u raw=(%u,%u) xy=(%u,%u) int=%u\r\n",
           (unsigned int)point->points,
           (unsigned int)point->event,
           (unsigned int)raw_x,
           (unsigned int)raw_y,
           (unsigned int)point->x,
           (unsigned int)point->y,
           (unsigned int)HAL_GPIO_ReadPin(TP_INT_GPIO_Port, TP_INT_Pin));
#endif
    if ((point->x < LCD_WIDTH) && (point->y < LCD_HEIGHT) &&
        ((point->event == 0U) || (point->event == 2U))) {
        point->detected = 1U;
    }

    return HAL_OK;
}

uint8_t FT6336_IsPressed(void)
{
    return (HAL_GPIO_ReadPin(TP_INT_GPIO_Port, TP_INT_Pin) == GPIO_PIN_RESET) ? 1U : 0U;
}

HAL_StatusTypeDef CST816T_Init(void)
{
    return FT6336_Init();
}

HAL_StatusTypeDef CST816T_ReadPoint(CST816T_TouchPoint_t *point)
{
    return FT6336_ReadPoint(point);
}

uint8_t CST816T_IsPressed(void)
{
    return FT6336_IsPressed();
}
