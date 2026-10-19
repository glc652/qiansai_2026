#include "lcd_st7789.h"
#include "spi.h"
#include <stdio.h>
#include <string.h>

#define LCD_SPI_TIMEOUT_MS 500U
#define LCD_TX_BUF_SIZE    512U
#define ST7789_CMD_MADCTL  0x36U
#define ST7789_MADCTL_MY   0x80U
#define ST7789_MADCTL_MX   0x40U
#define ST7789_MADCTL_MV   0x20U
#define ST7789_MADCTL_RGB  0x00U
#define LCD_DEBUG_PRINTF   0U

static uint8_t lcd_tx_buf[2][LCD_TX_BUF_SIZE];
static uint32_t lcd_spi_tx_count;
static HAL_StatusTypeDef lcd_last_spi_status;
static volatile uint8_t lcd_dma_waiting;
static volatile HAL_StatusTypeDef lcd_dma_status;

static void LCD_DebugPrintPins(const char *tag)
{
#if LCD_DEBUG_PRINTF
    printf("LCD %s pins CS=%u DC=%u RST=%u BL=%u spi_state=%lu spi_err=0x%08lX tx=%lu last=%d\r\n",
           tag,
           (unsigned int)HAL_GPIO_ReadPin(LCD_CS_GPIO_Port, LCD_CS_Pin),
           (unsigned int)HAL_GPIO_ReadPin(LCD_DC_GPIO_Port, LCD_DC_Pin),
           (unsigned int)HAL_GPIO_ReadPin(LCD_RST_GPIO_Port, LCD_RST_Pin),
           (unsigned int)HAL_GPIO_ReadPin(LCD_BL_GPIO_Port, LCD_BL_Pin),
           (unsigned long)HAL_SPI_GetState(&hspi3),
           (unsigned long)HAL_SPI_GetError(&hspi3),
           (unsigned long)lcd_spi_tx_count,
           (int)lcd_last_spi_status);
#else
    (void)tag;
#endif
}
/**
 * @brief  选中LCD屏幕（拉低CS引脚）
 */
static void LCD_Select(void)
{
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
}

static void LCD_Unselect(void)
{
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}

static void LCD_CommandMode(void)
{
    HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET);
}

static void LCD_DataMode(void)
{
    HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);
}

static void LCD_Reset(void)
{
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(20);
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(120);
}
/**
 * @brief  通过SPI发送字节数据
 * @param  data: 指向待发送数据缓冲区的指针
 * @param  len: 待发送数据的长度
 * @retval HAL状态枚举值
 */
static void LCD_ClearSpiErrors(void)
{
    if (HAL_SPI_GetError(&hspi3) != HAL_SPI_ERROR_NONE) {
        __HAL_SPI_CLEAR_OVRFLAG(&hspi3);
        __HAL_SPI_CLEAR_FREFLAG(&hspi3);
        __HAL_SPI_CLEAR_UDRFLAG(&hspi3);
        __HAL_SPI_CLEAR_EOTFLAG(&hspi3);
        __HAL_SPI_CLEAR_TXTFFLAG(&hspi3);
    }
}

static HAL_StatusTypeDef LCD_WaitDmaDone(void)
{
    uint32_t start_tick = HAL_GetTick();

    while (lcd_dma_waiting != 0U) {
        if ((HAL_GetTick() - start_tick) > LCD_SPI_TIMEOUT_MS) {
            (void)HAL_SPI_Abort(&hspi3);
            lcd_dma_waiting = 0U;
            lcd_dma_status = HAL_TIMEOUT;
            break;
        }
    }
    lcd_last_spi_status = lcd_dma_status;
    return lcd_last_spi_status;
}

static HAL_StatusTypeDef LCD_StartWriteBytes(const uint8_t *data, uint16_t len)
{
    LCD_ClearSpiErrors();

    if ((hspi3.hdmatx != NULL) && (len >= 16U)) {
        lcd_dma_waiting = 1U;
        lcd_dma_status = HAL_OK;
        lcd_last_spi_status = HAL_SPI_Transmit_DMA(&hspi3, (uint8_t *)data, len);
        if (lcd_last_spi_status != HAL_OK) {
            lcd_dma_waiting = 0U;
        }
    } else {
        lcd_last_spi_status = HAL_SPI_Transmit(&hspi3, (uint8_t *)data, len, LCD_SPI_TIMEOUT_MS);
    }

    lcd_spi_tx_count++;
#if LCD_DEBUG_PRINTF
    if (lcd_last_spi_status != HAL_OK) {
        printf("LCD SPI tx failed len=%u status=%d state=%lu err=0x%08lX\r\n",
               (unsigned int)len,
               (int)lcd_last_spi_status,
               (unsigned long)HAL_SPI_GetState(&hspi3),
               (unsigned long)HAL_SPI_GetError(&hspi3));
    }
#endif
    return lcd_last_spi_status;
}

static HAL_StatusTypeDef LCD_WriteBytes(const uint8_t *data, uint16_t len)
{
    HAL_StatusTypeDef status;

    status = LCD_StartWriteBytes(data, len);
    if ((status == HAL_OK) && (lcd_dma_waiting != 0U)) {
        status = LCD_WaitDmaDone();
    }
    return status;
}

void LCD_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if ((hspi != NULL) && (hspi->Instance == SPI3) && (lcd_dma_waiting != 0U)) {
        lcd_dma_status = HAL_OK;
        lcd_dma_waiting = 0U;
    }
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    LCD_SPI_TxCpltCallback(hspi);
}

void LCD_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
    if ((hspi != NULL) && (hspi->Instance == SPI3) && (lcd_dma_waiting != 0U)) {
        lcd_dma_status = HAL_ERROR;
        lcd_dma_waiting = 0U;
    }
}
/**
 * @brief  向LCD发送单字节命令
 * @param  cmd: 命令字节
 */
static void LCD_WriteCommand(uint8_t cmd)
{
    LCD_Select();
    LCD_CommandMode();
    (void)LCD_WriteBytes(&cmd, 1);
    LCD_Unselect();
}

/**
 * @brief  向LCD发送单字节数据
 * @param  data: 数据字节
 */
static void LCD_WriteData8(uint8_t data)
{
    LCD_Select();
    LCD_DataMode();
    (void)LCD_WriteBytes(&data, 1);
    LCD_Unselect();
}
/**
 * @brief  向LCD发送多字节数据
 * @param  data: 指向数据缓冲区的指针
 * @param  len: 数据长度
 */
static void LCD_WriteData(const uint8_t *data, uint16_t len)
{
    LCD_Select();
    LCD_DataMode();
    (void)LCD_WriteBytes(data, len);
    LCD_Unselect();
}

/**
 * @brief  设置LCD显存写入地址范围
 * @param  x0: 起始X坐标
 * @param  y0: 起始Y坐标
 * @param  x1: 结束X坐标
 * @param  y1: 结束Y坐标
 * @note   自动处理坐标越界情况，并发送列地址设置(0x2A)、行地址设置(0x2B)及内存写命令(0x2C)
 */
static void LCD_SetAddress(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    uint8_t data[4];

    if (x0 >= LCD_WIDTH) {
        x0 = LCD_WIDTH - 1U;
    }
    if (x1 >= LCD_WIDTH) {
        x1 = LCD_WIDTH - 1U;
    }
    if (y0 >= LCD_HEIGHT) {
        y0 = LCD_HEIGHT - 1U;
    }
    if (y1 >= LCD_HEIGHT) {
        y1 = LCD_HEIGHT - 1U;
    }

    LCD_WriteCommand(0x2A);
    data[0] = (uint8_t)(x0 >> 8);
    data[1] = (uint8_t)x0;
    data[2] = (uint8_t)(x1 >> 8);
    data[3] = (uint8_t)x1;
    LCD_WriteData(data, sizeof(data));

    LCD_WriteCommand(0x2B);
    data[0] = (uint8_t)(y0 >> 8);
    data[1] = (uint8_t)y0;
    data[2] = (uint8_t)(y1 >> 8);
    data[3] = (uint8_t)y1;
    LCD_WriteData(data, sizeof(data));

    LCD_WriteCommand(0x2C);
}
/**
 * @brief  控制LCD背光开关
 * @param  on: 非0值开启背光，0值关闭背光
 */
void LCD_SetBacklight(uint8_t on)
{
    HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin,
                      on ? LCD_BL_ACTIVE_LEVEL : (GPIO_PinState)(!LCD_BL_ACTIVE_LEVEL));
#if LCD_DEBUG_PRINTF
    printf("LCD backlight %u level=%u\r\n",
           (unsigned int)on,
           (unsigned int)HAL_GPIO_ReadPin(LCD_BL_GPIO_Port, LCD_BL_Pin));
#endif
}

void LCD_SetRotation(uint8_t rotation)
{
    uint8_t madctl;

    switch (rotation & 0x03U) {
    default:
    case 0:
        madctl = ST7789_MADCTL_RGB;
        break;
    case 1:
        madctl = ST7789_MADCTL_MY | ST7789_MADCTL_MX | ST7789_MADCTL_RGB;
        break;
    case 2:
        madctl = ST7789_MADCTL_MY | ST7789_MADCTL_MV | ST7789_MADCTL_RGB;
        break;
    case 3:
        madctl = ST7789_MADCTL_MX | ST7789_MADCTL_MV | ST7789_MADCTL_RGB;
        break;
    }

    LCD_WriteCommand(ST7789_CMD_MADCTL);
    LCD_WriteData8(madctl);
}
/**
 * @brief  初始化LCD屏幕
 * @note   执行硬件复位，发送ST7789初始化序列配置参数，最后清屏并开启背光
 */
void LCD_Init(void)
{
#if LCD_DEBUG_PRINTF
    printf("LCD init begin %ux%u hspi3_state=%lu hspi3_err=0x%08lX\r\n",
           (unsigned int)LCD_WIDTH,
           (unsigned int)LCD_HEIGHT,
           (unsigned long)HAL_SPI_GetState(&hspi3),
           (unsigned long)HAL_SPI_GetError(&hspi3));
#endif
    lcd_spi_tx_count = 0U;
    lcd_last_spi_status = HAL_OK;
    LCD_Unselect();
    LCD_CommandMode();
    LCD_SetBacklight(0);
    LCD_DebugPrintPins("before_reset");
    LCD_Reset();
    LCD_DebugPrintPins("after_reset");

    LCD_WriteCommand(0x11);
#if LCD_DEBUG_PRINTF
    printf("LCD cmd SLPOUT 0x11 status=%d tx=%lu\r\n",
           (int)lcd_last_spi_status,
           (unsigned long)lcd_spi_tx_count);
#endif
    HAL_Delay(120);

    LCD_WriteCommand(0x36);
    LCD_WriteData8(0x00);

    LCD_WriteCommand(0x3A);
    LCD_WriteData8(0x05);

    LCD_WriteCommand(0xB2);
    LCD_WriteData8(0x0C);
    LCD_WriteData8(0x0C);
    LCD_WriteData8(0x00);
    LCD_WriteData8(0x33);
    LCD_WriteData8(0x33);

    LCD_WriteCommand(0xB7);
    LCD_WriteData8(0x35);

    LCD_WriteCommand(0xBB);
    LCD_WriteData8(0x28);

    LCD_WriteCommand(0xC0);
    LCD_WriteData8(0x2C);

    LCD_WriteCommand(0xC2);
    LCD_WriteData8(0x01);

    LCD_WriteCommand(0xC3);
    LCD_WriteData8(0x0B);

    LCD_WriteCommand(0xC4);
    LCD_WriteData8(0x20);

    LCD_WriteCommand(0xC6);
    LCD_WriteData8(0x0F);

    LCD_WriteCommand(0xD0);
    LCD_WriteData8(0xA4);
    LCD_WriteData8(0xA1);

    LCD_WriteCommand(0xE0);
    {
        const uint8_t p_gamma[] = {0xD0,0x01,0x08,0x0F,0x11,0x2A,0x36,0x55,0x44,0x3A,0x0B,0x06,0x11,0x20};
        LCD_WriteData(p_gamma, sizeof(p_gamma));
    }

    LCD_WriteCommand(0xE1);
    {
        const uint8_t n_gamma[] = {0xD0,0x02,0x07,0x0A,0x0B,0x18,0x34,0x43,0x4A,0x2B,0x1B,0x1C,0x22,0x1F};
        LCD_WriteData(n_gamma, sizeof(n_gamma));
    }

    LCD_WriteCommand(0x29);
#if LCD_DEBUG_PRINTF
    printf("LCD cmd DISPON 0x29 status=%d tx=%lu\r\n",
           (int)lcd_last_spi_status,
           (unsigned long)lcd_spi_tx_count);
#endif
    HAL_Delay(20);

    LCD_SetBacklight(1);
    LCD_DebugPrintPins("before_clear");
    LCD_FillScreen(LCD_COLOR_BLACK);
    LCD_DebugPrintPins("after_clear");
#if LCD_DEBUG_PRINTF
    printf("LCD init end tx=%lu last=%d spi_err=0x%08lX\r\n",
           (unsigned long)lcd_spi_tx_count,
           (int)lcd_last_spi_status,
           (unsigned long)HAL_SPI_GetError(&hspi3));
#endif
}
/**
 * @brief  在指定矩形区域内填充颜色
 * @param  x0: 起始X坐标
 * @param  y0: 起始Y坐标
 * @param  x1: 结束X坐标
 * @param  y1: 结束Y坐标
 * @param  color: 填充颜色 (RGB565格式)
 * @note   使用内部缓冲区分批发送数据以优化大量像素写入性能
 */
void LCD_Fill(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    uint32_t pixels;
    uint32_t bytes_left;
    uint16_t chunk;

    if ((x0 >= LCD_WIDTH) || (y0 >= LCD_HEIGHT)) {
#if LCD_DEBUG_PRINTF
        printf("LCD fill reject start x0=%u y0=%u x1=%u y1=%u\r\n",
               (unsigned int)x0, (unsigned int)y0, (unsigned int)x1, (unsigned int)y1);
#endif
        return;
    }
    if (x1 >= LCD_WIDTH) {
        x1 = LCD_WIDTH - 1U;
    }
    if (y1 >= LCD_HEIGHT) {
        y1 = LCD_HEIGHT - 1U;
    }
    if ((x1 < x0) || (y1 < y0)) {
#if LCD_DEBUG_PRINTF
        printf("LCD fill reject range x0=%u y0=%u x1=%u y1=%u\r\n",
               (unsigned int)x0, (unsigned int)y0, (unsigned int)x1, (unsigned int)y1);
#endif
        return;
    }

    pixels = ((uint32_t)(x1 - x0 + 1U)) * ((uint32_t)(y1 - y0 + 1U));
    bytes_left = pixels * 2U;

    for (uint16_t i = 0; i < LCD_TX_BUF_SIZE; i += 2U) {
        lcd_tx_buf[0][i] = (uint8_t)(color >> 8);
        lcd_tx_buf[0][i + 1U] = (uint8_t)color;
    }

    LCD_SetAddress(x0, y0, x1, y1);
#if LCD_DEBUG_PRINTF
    if ((x0 == 0U) && (y0 == 0U)) {
        printf("LCD fill x0=%u y0=%u x1=%u y1=%u color=0x%04X bytes=%lu\r\n",
               (unsigned int)x0,
               (unsigned int)y0,
               (unsigned int)x1,
               (unsigned int)y1,
               (unsigned int)color,
               (unsigned long)bytes_left);
    }
#endif
    LCD_Select();
    LCD_DataMode();
    while (bytes_left > 0U) {
        chunk = (bytes_left > LCD_TX_BUF_SIZE) ? LCD_TX_BUF_SIZE : (uint16_t)bytes_left;
        (void)LCD_WriteBytes(lcd_tx_buf[0], chunk);
        bytes_left -= chunk;
    }
    LCD_Unselect();
}
/**
 * @brief  全屏填充指定颜色
 * @param  color: 填充颜色 (RGB565格式)
 */
void LCD_FillScreen(uint16_t color)
{
    LCD_Fill(0, 0, LCD_WIDTH - 1U, LCD_HEIGHT - 1U, color);
}

void LCD_DrawRGB565Bitmap(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *pixels)
{
    uint32_t total_pixels;
    uint32_t sent_pixels = 0U;
    uint8_t active_dma = 0U;
    uint8_t buf_index = 0U;

    if ((pixels == NULL) || (w == 0U) || (h == 0U) || (x >= LCD_WIDTH) || (y >= LCD_HEIGHT)) {
        return;
    }

    if ((uint32_t)x + w > LCD_WIDTH) {
        w = (uint16_t)(LCD_WIDTH - x);
    }
    if ((uint32_t)y + h > LCD_HEIGHT) {
        h = (uint16_t)(LCD_HEIGHT - y);
    }

    total_pixels = ((uint32_t)w) * ((uint32_t)h);

    LCD_SetAddress(x, y, (uint16_t)(x + w - 1U), (uint16_t)(y + h - 1U));
    LCD_Select();
    LCD_DataMode();
    while (sent_pixels < total_pixels) {
        uint32_t remain = total_pixels - sent_pixels;
        uint16_t chunk_pixels = (remain > (LCD_TX_BUF_SIZE / 2U)) ? (LCD_TX_BUF_SIZE / 2U) : (uint16_t)remain;
        uint8_t *tx_buf = lcd_tx_buf[buf_index];

        for (uint16_t i = 0U; i < chunk_pixels; i++) {
            uint16_t color = pixels[sent_pixels + i];
            tx_buf[(uint32_t)i * 2U] = (uint8_t)(color >> 8);
            tx_buf[((uint32_t)i * 2U) + 1U] = (uint8_t)color;
        }

        if (active_dma != 0U) {
            if (LCD_WaitDmaDone() != HAL_OK) {
                break;
            }
            active_dma = 0U;
        }

        if (LCD_StartWriteBytes(tx_buf, (uint16_t)(chunk_pixels * 2U)) != HAL_OK) {
            break;
        }
        active_dma = (lcd_dma_waiting != 0U) ? 1U : 0U;
        sent_pixels += chunk_pixels;
        buf_index ^= 1U;
    }
    if (active_dma != 0U) {
        (void)LCD_WaitDmaDone();
    }
    LCD_Unselect();
}
/**
 * @brief  在指定坐标绘制一个像素点
 * @param  x: X坐标
 * @param  y: Y坐标
 * @param  color: 像素颜色 (RGB565格式)
 */
void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color)
{
    uint8_t data[2];

    if ((x >= LCD_WIDTH) || (y >= LCD_HEIGHT)) {
        return;
    }

    data[0] = (uint8_t)(color >> 8);
    data[1] = (uint8_t)color;
    LCD_SetAddress(x, y, x, y);
    LCD_WriteData(data, sizeof(data));
}
/**
 * @brief  绘制空心矩形
 * @param  x: 左上角X坐标
 * @param  y: 左上角Y坐标
 * @param  w: 矩形宽度
 * @param  h: 矩形高度
 * @param  color: 边框颜色 (RGB565格式)
 * @note   通过绘制四条边线实现
 */
void LCD_DrawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    if ((w == 0U) || (h == 0U)) {
        return;
    }

    LCD_Fill(x, y, x + w - 1U, y, color);
    LCD_Fill(x, y + h - 1U, x + w - 1U, y + h - 1U, color);
    LCD_Fill(x, y, x, y + h - 1U, color);
    LCD_Fill(x + w - 1U, y, x + w - 1U, y + h - 1U, color);
}

static const uint8_t lcd_seg7_table[10] = {
    0x3FU, 0x06U, 0x5BU, 0x4FU, 0x66U, 0x6DU, 0x7DU, 0x07U, 0x7FU, 0x6FU
};

void LCD_DrawSeg7Digit(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t t,
                       uint8_t digit, uint16_t color, uint16_t bg)
{
    uint16_t vlen;
    uint16_t ym;
    uint8_t seg;

    if ((h < (3U * t)) || (w < (2U * t))) {
        return;
    }

    seg = (digit > 9U) ? 0x00U : lcd_seg7_table[digit];
    vlen = (uint16_t)((h - 3U * t) / 2U);
    ym = (uint16_t)(y + t + vlen);

    LCD_Fill(x + t, y, x + w - 1U - t, y + t - 1U, ((seg & 0x01U) != 0U) ? color : bg);
    LCD_Fill(x + w - t, y + t, x + w - 1U, ym - 1U, ((seg & 0x02U) != 0U) ? color : bg);
    LCD_Fill(x + w - t, ym + t, x + w - 1U, y + h - 1U - t, ((seg & 0x04U) != 0U) ? color : bg);
    LCD_Fill(x + t, y + h - t, x + w - 1U - t, y + h - 1U, ((seg & 0x08U) != 0U) ? color : bg);
    LCD_Fill(x, ym + t, x + t - 1U, y + h - 1U - t, ((seg & 0x10U) != 0U) ? color : bg);
    LCD_Fill(x, y + t, x + t - 1U, ym - 1U, ((seg & 0x20U) != 0U) ? color : bg);
    LCD_Fill(x + t, ym, x + w - 1U - t, ym + t - 1U, ((seg & 0x40U) != 0U) ? color : bg);
}

uint16_t LCD_DrawNumberX10(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t t,
                           int32_t value_x10, uint16_t color, uint16_t bg)
{
    uint16_t gap = t;
    uint16_t sw = (uint16_t)(t * 2U);
    uint16_t cx = x;
    uint8_t neg;
    uint32_t mag;
    uint32_t ip;
    uint8_t frac;
    uint8_t hundreds;
    uint8_t tens;
    uint8_t ones;

    neg = (value_x10 < 0) ? 1U : 0U;
    mag = (uint32_t)(neg ? -value_x10 : value_x10);
    ip = mag / 10U;
    frac = (uint8_t)(mag % 10U);
    hundreds = (uint8_t)((ip / 100U) % 10U);
    tens = (uint8_t)((ip / 10U) % 10U);
    ones = (uint8_t)(ip % 10U);

    LCD_Fill(cx, y + (h - t) / 2U, cx + sw - 1U, y + (h - t) / 2U + t - 1U, (neg != 0U) ? color : bg);
    cx = (uint16_t)(cx + sw + gap);

    LCD_DrawSeg7Digit(cx, y, w, h, t, (ip < 100U) ? 0xFFU : hundreds, color, bg);
    cx = (uint16_t)(cx + w + gap);
    LCD_DrawSeg7Digit(cx, y, w, h, t, (ip < 10U) ? 0xFFU : tens, color, bg);
    cx = (uint16_t)(cx + w + gap);
    LCD_DrawSeg7Digit(cx, y, w, h, t, ones, color, bg);
    cx = (uint16_t)(cx + w + gap);

    LCD_Fill(cx, y + h - t, cx + t - 1U, y + h - 1U, color);
    cx = (uint16_t)(cx + sw + gap);

    LCD_DrawSeg7Digit(cx, y, w, h, t, frac, color, bg);
    cx = (uint16_t)(cx + w + gap);

    return cx;
}

uint16_t LCD_DrawNumberU(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t t,
                         uint32_t value, uint16_t color, uint16_t bg)
{
    uint16_t gap = t;
    uint16_t cx = x;
    uint8_t hundreds;
    uint8_t tens;
    uint8_t ones;

    if (value > 999U)
    {
        value = 999U;
    }
    hundreds = (uint8_t)((value / 100U) % 10U);
    tens = (uint8_t)((value / 10U) % 10U);
    ones = (uint8_t)(value % 10U);

    LCD_DrawSeg7Digit(cx, y, w, h, t, (value < 100U) ? 0xFFU : hundreds, color, bg);
    cx = (uint16_t)(cx + w + gap);
    LCD_DrawSeg7Digit(cx, y, w, h, t, (value < 10U) ? 0xFFU : tens, color, bg);
    cx = (uint16_t)(cx + w + gap);
    LCD_DrawSeg7Digit(cx, y, w, h, t, ones, color, bg);
    cx = (uint16_t)(cx + w + gap);

    return cx;
}
