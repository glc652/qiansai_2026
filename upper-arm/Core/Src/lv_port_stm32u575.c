#include "lv_port_stm32u575.h"

#include "cmsis_os2.h"
#include "lcd_st7789.h"
#include "touch_cst816t.h"
#include "lvgl.h"
#include <stddef.h>
#include <stdint.h>

#define LV_PORT_BUF_LINES LCD_HEIGHT

extern osMutexId_t spi3LcdMutexHandle;

static lv_disp_draw_buf_t s_disp_draw_buf;
static lv_color_t s_disp_buf[LCD_WIDTH * LV_PORT_BUF_LINES];
static lv_disp_drv_t s_disp_drv;
static lv_indev_drv_t s_touch_drv;
static volatile uint8_t s_touch_pressed_event;
static volatile uint32_t s_flush_fps;
static uint32_t s_flush_window_tick;
static uint32_t s_flush_window_count;

static void LV_Port_CountFlush(void)
{
    uint32_t now = osKernelGetTickCount();

    if (s_flush_window_tick == 0U) {
        s_flush_window_tick = now;
    }
    s_flush_window_count++;
    if ((now - s_flush_window_tick) >= 1000U) {
        s_flush_fps = s_flush_window_count;
        s_flush_window_count = 0U;
        s_flush_window_tick = now;
    }
}

static void LV_Port_DispFlush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    int32_t x1 = area->x1;
    int32_t y1 = area->y1;
    int32_t x2 = area->x2;
    int32_t y2 = area->y2;

    if ((x2 < 0) || (y2 < 0) || (x1 >= (int32_t)LCD_WIDTH) || (y1 >= (int32_t)LCD_HEIGHT)) {
        lv_disp_flush_ready(disp_drv);
        return;
    }

    if (x1 < 0) {
        x1 = 0;
    }
    if (y1 < 0) {
        y1 = 0;
    }
    if (x2 >= (int32_t)LCD_WIDTH) {
        x2 = (int32_t)LCD_WIDTH - 1;
    }
    if (y2 >= (int32_t)LCD_HEIGHT) {
        y2 = (int32_t)LCD_HEIGHT - 1;
    }

    if (spi3LcdMutexHandle != NULL) {
        (void)osMutexAcquire(spi3LcdMutexHandle, osWaitForever);
    }

    LCD_DrawRGB565Bitmap((uint16_t)x1,
                         (uint16_t)y1,
                         (uint16_t)(x2 - x1 + 1),
                         (uint16_t)(y2 - y1 + 1),
                         (const uint16_t *)color_p);
    LV_Port_CountFlush();

    if (spi3LcdMutexHandle != NULL) {
        (void)osMutexRelease(spi3LcdMutexHandle);
    }

    lv_disp_flush_ready(disp_drv);
}

static void LV_Port_TouchRead(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    CST816T_TouchPoint_t point;

    (void)indev_drv;

    if ((CST816T_IsPressed() != 0U) && (CST816T_ReadPoint(&point) == HAL_OK) && (point.detected != 0U)) {
        data->state = LV_INDEV_STATE_PRESSED;
        data->point.x = (lv_coord_t)point.x;
        data->point.y = (lv_coord_t)point.y;
        s_touch_pressed_event = 1U;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

uint8_t LV_Port_TouchWasPressed(void)
{
    uint8_t pressed = s_touch_pressed_event;

    s_touch_pressed_event = 0U;
    return pressed;
}

uint32_t LV_Port_GetFlushFps(void)
{
    return s_flush_fps;
}

void LV_Port_Init(void)
{
    lv_init();

    lv_disp_draw_buf_init(&s_disp_draw_buf, s_disp_buf, NULL, LCD_WIDTH * LV_PORT_BUF_LINES);

    lv_disp_drv_init(&s_disp_drv);
    s_disp_drv.hor_res = LCD_WIDTH;
    s_disp_drv.ver_res = LCD_HEIGHT;
    s_disp_drv.flush_cb = LV_Port_DispFlush;
    s_disp_drv.draw_buf = &s_disp_draw_buf;
    (void)lv_disp_drv_register(&s_disp_drv);

    lv_indev_drv_init(&s_touch_drv);
    s_touch_drv.type = LV_INDEV_TYPE_POINTER;
    s_touch_drv.read_cb = LV_Port_TouchRead;
    (void)lv_indev_drv_register(&s_touch_drv);
}



