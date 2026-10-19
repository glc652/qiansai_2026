#ifndef __TOUCH_CST816T_H__
#define __TOUCH_CST816T_H__

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FT6336_I2C_ADDR_8BIT 0x70U
#define CST816T_I2C_ADDR_8BIT FT6336_I2C_ADDR_8BIT

#define FT6336_SWAP_XY   0U
#define FT6336_INVERT_X  0U
#define FT6336_INVERT_Y  0U

typedef struct {
    uint8_t detected;
    uint8_t gesture;
    uint8_t points;
    uint8_t event;
    uint8_t id;
    uint16_t x;
    uint16_t y;
} CST816T_TouchPoint_t;

HAL_StatusTypeDef FT6336_Init(void);
HAL_StatusTypeDef FT6336_ReadPoint(CST816T_TouchPoint_t *point);
uint8_t FT6336_IsPressed(void);

HAL_StatusTypeDef CST816T_Init(void);
HAL_StatusTypeDef CST816T_ReadPoint(CST816T_TouchPoint_t *point);
uint8_t CST816T_IsPressed(void);

#ifdef __cplusplus
}
#endif

#endif
