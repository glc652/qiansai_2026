#include "lcd_st7789.h"
#include "spi.h"
#include <stdint.h>
#include <string.h>

#define LCD_SPI_TIMEOUT_MS 100U
#define LCD_TX_BUF_SIZE    512U
#define LCD_DMA_CHUNK_SIZE 512U
/* ST7789V 鎺у埗鍣?GRAM 涓?240x320锛屾湰闈㈡澘鍙鍖轰负 240x280锛岃璧峰鍦?GRAM 涓湁鍋忕Щ銆?
   搴曢儴鍑虹幇鑺辨潯鏃惰皟姝ゅ€硷紙甯歌 0 鎴?20锛夛紝鍒楁柟鍚戞弧瀹?240 鏃犻渶鍋忕Щ銆?*/
#define LCD_Y_OFFSET       0U

static uint8_t lcd_tx_buf[LCD_TX_BUF_SIZE] __ALIGNED(32);
static HAL_StatusTypeDef lcd_last_status = HAL_OK;
static volatile uint8_t lcd_dma_waiting;
static volatile HAL_StatusTypeDef lcd_dma_wait_status;
static volatile uint8_t lcd_dma_active;
static volatile uint8_t lcd_dma_chunk_done;
static volatile uint8_t lcd_dma_error_pending;
static const uint16_t *lcd_dma_pixels;
static uint32_t lcd_dma_pixels_left;
static LCD_TransferDoneCallback_t lcd_dma_done_cb;
static void *lcd_dma_done_context;

static HAL_StatusTypeDef LCD_StartNextDmaChunk(void);
static void LCD_FinishDmaTransfer(HAL_StatusTypeDef status);
static void LCD_CleanDCacheForDma(const void *addr, uint32_t len);
static void LCD_CopyRGB565ToBusBytes(uint8_t *dst, const uint16_t *src, uint32_t pixels);
/**
 * @brief  閫変腑LCD灞忓箷锛堟媺浣嶤S寮曡剼锛?
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
 * @brief  閫氳繃SPI鍙戦€佸瓧鑺傛暟鎹?
 * @param  data: 鎸囧悜寰呭彂閫佹暟鎹紦鍐插尯鐨勬寚閽?
 * @param  len: 寰呭彂閫佹暟鎹殑闀垮害
 * @retval HAL鐘舵€佹灇涓惧€?
 */
static HAL_StatusTypeDef LCD_WriteBytes(const uint8_t *data, uint16_t len)
{
    uint32_t start_tick;

    if ((hspi3.hdmatx != NULL) && (len >= 16U)) {
        LCD_CleanDCacheForDma(data, len);
        lcd_dma_waiting = 1U;
        lcd_dma_wait_status = HAL_OK;
        lcd_last_status = HAL_SPI_Transmit_DMA(&hspi3, (uint8_t *)data, len);
        if (lcd_last_status == HAL_OK) {
            start_tick = HAL_GetTick();
            while (lcd_dma_waiting != 0U) {
                if (HAL_SPI_GetState(&hspi3) == HAL_SPI_STATE_READY) {
                    lcd_dma_wait_status = HAL_OK;
                    lcd_dma_waiting = 0U;
                    break;
                }
                if ((HAL_GetTick() - start_tick) > LCD_SPI_TIMEOUT_MS) {
                    (void)HAL_SPI_Abort(&hspi3);
                    lcd_dma_wait_status = HAL_TIMEOUT;
                    lcd_dma_waiting = 0U;
                    break;
                }
            }
            lcd_last_status = lcd_dma_wait_status;
        } else {
            lcd_dma_waiting = 0U;
        }
    } else {
        lcd_last_status = HAL_SPI_Transmit(&hspi3, (uint8_t *)data, len, LCD_SPI_TIMEOUT_MS);
    }

    return lcd_last_status;
}

void LCD_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if ((hspi != NULL) && (hspi->Instance == SPI3)) {
        if (lcd_dma_active != 0U) {
            lcd_dma_chunk_done = 1U;
        } else if (lcd_dma_waiting != 0U) {
            lcd_dma_wait_status = HAL_OK;
            lcd_dma_waiting = 0U;
        } else {
            /* No LCD DMA transfer is waiting. */
        }
    }
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    LCD_SPI_TxCpltCallback(hspi);
}

void LCD_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
    if ((hspi != NULL) && (hspi->Instance == SPI3)) {
        if (lcd_dma_active != 0U) {
            lcd_dma_error_pending = 1U;
        } else if (lcd_dma_waiting != 0U) {
            lcd_dma_wait_status = HAL_ERROR;
            lcd_dma_waiting = 0U;
        } else {
            /* No LCD DMA transfer is waiting. */
        }
    }
}

static void LCD_FinishDmaTransfer(HAL_StatusTypeDef status)
{
    LCD_TransferDoneCallback_t done_cb = lcd_dma_done_cb;
    void *context = lcd_dma_done_context;

    lcd_last_status = status;
    lcd_dma_active = 0U;
    lcd_dma_chunk_done = 0U;
    lcd_dma_error_pending = 0U;
    lcd_dma_pixels = NULL;
    lcd_dma_pixels_left = 0U;
    lcd_dma_done_cb = NULL;
    lcd_dma_done_context = NULL;
    LCD_Unselect();

    if (done_cb != NULL) {
        done_cb(context);
    }
}

static HAL_StatusTypeDef LCD_StartNextDmaChunk(void)
{
    uint32_t chunk_pixels;
    uint32_t max_pixels;

    if (lcd_dma_pixels_left == 0U) {
        LCD_FinishDmaTransfer(HAL_OK);
        return HAL_OK;
    }

    max_pixels = LCD_DMA_CHUNK_SIZE / 2U;
    chunk_pixels = (lcd_dma_pixels_left > max_pixels) ? max_pixels : lcd_dma_pixels_left;
    LCD_CopyRGB565ToBusBytes(lcd_tx_buf, lcd_dma_pixels, chunk_pixels);

    lcd_dma_pixels += chunk_pixels;
    lcd_dma_pixels_left -= chunk_pixels;
    lcd_dma_chunk_done = 0U;

    LCD_CleanDCacheForDma(lcd_tx_buf, chunk_pixels * 2U);
    lcd_last_status = HAL_SPI_Transmit_DMA(&hspi3, lcd_tx_buf, (uint16_t)(chunk_pixels * 2U));
    if (lcd_last_status != HAL_OK) {
        LCD_FinishDmaTransfer(lcd_last_status);
    }

    return lcd_last_status;
}

static void LCD_CleanDCacheForDma(const void *addr, uint32_t len)
{
#if defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    uintptr_t start = ((uintptr_t)addr) & ~((uintptr_t)31U);
    uintptr_t end = (((uintptr_t)addr) + (uintptr_t)len + 31U) & ~((uintptr_t)31U);

    SCB_CleanDCache_by_Addr((uint32_t *)start, (int32_t)(end - start));
#else
    (void)addr;
    (void)len;
#endif
    __DSB();
}

static void LCD_CopyRGB565ToBusBytes(uint8_t *dst, const uint16_t *src, uint32_t pixels)
{
    uint32_t pairs = pixels / 2U;

    for (uint32_t i = 0U; i < pairs; i++) {
        uint32_t packed = ((const uint32_t *)src)[i];
        ((uint32_t *)dst)[i] = __REV16(packed);
    }

    if ((pixels & 1U) != 0U) {
        uint16_t color = src[pixels - 1U];
        uint32_t offset = (pixels - 1U) * 2U;
        dst[offset] = (uint8_t)(color >> 8);
        dst[offset + 1U] = (uint8_t)color;
    }
}

void LCD_ProcessDmaTransfer(void)
{
    if (lcd_dma_active == 0U) {
        return;
    }

    if (lcd_dma_error_pending != 0U) {
        LCD_FinishDmaTransfer(HAL_ERROR);
        return;
    }

    if ((lcd_dma_chunk_done == 0U) && (HAL_SPI_GetState(&hspi3) == HAL_SPI_STATE_READY)) {
        lcd_dma_chunk_done = 1U;
    }

    if (lcd_dma_chunk_done != 0U) {
        lcd_dma_chunk_done = 0U;
        if (lcd_dma_pixels_left == 0U) {
            LCD_FinishDmaTransfer(HAL_OK);
        } else {
            (void)LCD_StartNextDmaChunk();
        }
    }
}

uint8_t LCD_IsDmaTransferActive(void)
{
    return lcd_dma_active;
}

void LCD_AbortDmaTransfer(void)
{
    if (lcd_dma_active != 0U) {
        (void)HAL_SPI_Abort(&hspi3);
        LCD_FinishDmaTransfer(HAL_TIMEOUT);
    }
}
/**
 * @brief  鍚慙CD鍙戦€佸崟瀛楄妭鍛戒护
 * @param  cmd: 鍛戒护瀛楄妭
 */
static void LCD_WriteCommand(uint8_t cmd)
{
    LCD_Select();
    LCD_CommandMode();
    (void)LCD_WriteBytes(&cmd, 1);
    LCD_Unselect();
}

/**
 * @brief  鍚慙CD鍙戦€佸崟瀛楄妭鏁版嵁
 * @param  data: 鏁版嵁瀛楄妭
 */
static void LCD_WriteData8(uint8_t data)
{
    LCD_Select();
    LCD_DataMode();
    (void)LCD_WriteBytes(&data, 1);
    LCD_Unselect();
}
/**
 * @brief  鍚慙CD鍙戦€佸瀛楄妭鏁版嵁
 * @param  data: 鎸囧悜鏁版嵁缂撳啿鍖虹殑鎸囬拡
 * @param  len: 鏁版嵁闀垮害
 */
static void LCD_WriteData(const uint8_t *data, uint16_t len)
{
    LCD_Select();
    LCD_DataMode();
    (void)LCD_WriteBytes(data, len);
    LCD_Unselect();
}

/**
 * @brief  璁剧疆LCD鏄惧瓨鍐欏叆鍦板潃鑼冨洿
 * @param  x0: 璧峰X鍧愭爣
 * @param  y0: 璧峰Y鍧愭爣
 * @param  x1: 缁撴潫X鍧愭爣
 * @param  y1: 缁撴潫Y鍧愭爣
 * @note   鑷姩澶勭悊鍧愭爣瓒婄晫鎯呭喌锛屽苟鍙戦€佸垪鍦板潃璁剧疆(0x2A)銆佽鍦板潃璁剧疆(0x2B)鍙婂唴瀛樺啓鍛戒护(0x2C)
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

    y0 += LCD_Y_OFFSET;
    y1 += LCD_Y_OFFSET;
    LCD_WriteCommand(0x2B);
    data[0] = (uint8_t)(y0 >> 8);
    data[1] = (uint8_t)y0;
    data[2] = (uint8_t)(y1 >> 8);
    data[3] = (uint8_t)y1;
    LCD_WriteData(data, sizeof(data));

    LCD_WriteCommand(0x2C);
}
/**
 * @brief  鎺у埗LCD鑳屽厜寮€鍏?
 * @param  on: 闈?鍊煎紑鍚儗鍏夛紝0鍊煎叧闂儗鍏?
 */
void LCD_SetBacklight(uint8_t on)
{
    HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, on ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

void LCD_DisplayOn(void)
{
    LCD_WriteCommand(0x29);
}

void LCD_DisplayOff(void)
{
    LCD_WriteCommand(0x28);
}
/**
 * @brief  鑾峰彇鏈€杩戜竴娆PI浼犺緭鐨凥AL鐘舵€?
 * @retval 鏈€杩戜竴娆CD_WriteBytes璁板綍鐨凥AL鐘舵€佹灇涓惧€?
 */
HAL_StatusTypeDef LCD_GetLastStatus(void)
{
    return lcd_last_status;
}

/**
 * @brief  鍒濆鍖朙CD灞忓箷
 * @note   鎵ц纭欢澶嶄綅锛屽彂閫丼T7789鍒濆鍖栧簭鍒楅厤缃弬鏁帮紝鏈€鍚庡紑鍚儗鍏?
 */
HAL_StatusTypeDef LCD_Init(void)
{
    lcd_last_status = HAL_OK;
    LCD_Unselect();
    LCD_CommandMode();
    LCD_SetBacklight(0);
    LCD_Reset();

    LCD_WriteCommand(0x11);
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
    LCD_WriteData8(0x32);

    LCD_WriteCommand(0xC2);
    LCD_WriteData8(0x01);

    LCD_WriteCommand(0xC3);
    LCD_WriteData8(0x15);

    LCD_WriteCommand(0xC4);
    LCD_WriteData8(0x20);

    LCD_WriteCommand(0xC6);
    LCD_WriteData8(0x0F);

    LCD_WriteCommand(0xD0);
    LCD_WriteData8(0xA4);
    LCD_WriteData8(0xA1);

    LCD_WriteCommand(0xE0);
    {
        const uint8_t p_gamma[] = {0xD0,0x08,0x0E,0x09,0x09,0x05,0x31,0x33,0x48,0x17,0x14,0x15,0x31,0x34};
        LCD_WriteData(p_gamma, sizeof(p_gamma));
    }

    LCD_WriteCommand(0xE1);
    {
        const uint8_t n_gamma[] = {0xD0,0x08,0x0E,0x09,0x09,0x15,0x31,0x33,0x48,0x17,0x14,0x15,0x31,0x34};
        LCD_WriteData(n_gamma, sizeof(n_gamma));
    }

    LCD_WriteCommand(0x21);
    LCD_WriteCommand(0x29);
    HAL_Delay(20);

    LCD_SetBacklight(1);
    return lcd_last_status;
}
/**
 * @brief  鍦ㄦ寚瀹氱煩褰㈠尯鍩熷唴濉厖棰滆壊
 * @param  x0: 璧峰X鍧愭爣
 * @param  y0: 璧峰Y鍧愭爣
 * @param  x1: 缁撴潫X鍧愭爣
 * @param  y1: 缁撴潫Y鍧愭爣
 * @param  color: 濉厖棰滆壊 (RGB565鏍煎紡)
 * @note   浣跨敤鍐呴儴缂撳啿鍖哄垎鎵瑰彂閫佹暟鎹互浼樺寲澶ч噺鍍忕礌鍐欏叆鎬ц兘
 */
void LCD_Fill(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    uint32_t pixels;
    uint32_t bytes_left;
    uint16_t chunk;

    if ((x0 >= LCD_WIDTH) || (y0 >= LCD_HEIGHT)) {
        return;
    }
    if (x1 >= LCD_WIDTH) {
        x1 = LCD_WIDTH - 1U;
    }
    if (y1 >= LCD_HEIGHT) {
        y1 = LCD_HEIGHT - 1U;
    }
    if ((x1 < x0) || (y1 < y0)) {
        return;
    }

    pixels = ((uint32_t)(x1 - x0 + 1U)) * ((uint32_t)(y1 - y0 + 1U));
    bytes_left = pixels * 2U;

    for (uint16_t i = 0; i < LCD_TX_BUF_SIZE; i += 2U) {
        lcd_tx_buf[i] = (uint8_t)(color >> 8);
        lcd_tx_buf[i + 1U] = (uint8_t)color;
    }

    LCD_SetAddress(x0, y0, x1, y1);
    LCD_Select();
    LCD_DataMode();
    while (bytes_left > 0U) {
        chunk = (bytes_left > LCD_TX_BUF_SIZE) ? LCD_TX_BUF_SIZE : (uint16_t)bytes_left;
        (void)LCD_WriteBytes(lcd_tx_buf, chunk);
        bytes_left -= chunk;
    }
    LCD_Unselect();
}
/**
 * @brief  鍏ㄥ睆濉厖鎸囧畾棰滆壊
 * @param  color: 濉厖棰滆壊 (RGB565鏍煎紡)
 */
void LCD_FillScreen(uint16_t color)
{
    LCD_Fill(0, 0, LCD_WIDTH - 1U, LCD_HEIGHT - 1U, color);
}
/**
 * @brief  鍦ㄦ寚瀹氬潗鏍囩粯鍒朵竴涓儚绱犵偣
 * @param  x: X鍧愭爣
 * @param  y: Y鍧愭爣
 * @param  color: 鍍忕礌棰滆壊 (RGB565鏍煎紡)
 */
void LCD_DrawRGB565Bitmap(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *pixels)
{
    uint32_t total_pixels;
    uint32_t sent_pixels = 0U;

    if ((pixels == NULL) || (w == 0U) || (h == 0U) || (x >= LCD_WIDTH) || (y >= LCD_HEIGHT)) {
        return;
    }

    if ((uint32_t)x + w > LCD_WIDTH) {
        w = (uint16_t)(LCD_WIDTH - x);
    }
    if ((uint32_t)y + h > LCD_HEIGHT) {
        h = (uint16_t)(LCD_HEIGHT - y);
    }

    total_pixels = (uint32_t)w * (uint32_t)h;
    LCD_SetAddress(x, y, (uint16_t)(x + w - 1U), (uint16_t)(y + h - 1U));
    LCD_Select();
    LCD_DataMode();

    while (sent_pixels < total_pixels) {
        uint32_t remain = total_pixels - sent_pixels;
        uint16_t chunk_pixels = (remain > (LCD_TX_BUF_SIZE / 2U)) ? (LCD_TX_BUF_SIZE / 2U) : (uint16_t)remain;

        LCD_CopyRGB565ToBusBytes(lcd_tx_buf, &pixels[sent_pixels], chunk_pixels);

        (void)LCD_WriteBytes(lcd_tx_buf, (uint16_t)(chunk_pixels * 2U));
        sent_pixels += chunk_pixels;
    }

    LCD_Unselect();
}

void LCD_DrawRGB565BitmapBytes(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint8_t *bytes)
{
    uint32_t bytes_left;
    uint16_t chunk;

    if ((bytes == NULL) || (w == 0U) || (h == 0U) || (x >= LCD_WIDTH) || (y >= LCD_HEIGHT)) {
        return;
    }

    if ((uint32_t)x + w > LCD_WIDTH) {
        w = (uint16_t)(LCD_WIDTH - x);
    }
    if ((uint32_t)y + h > LCD_HEIGHT) {
        h = (uint16_t)(LCD_HEIGHT - y);
    }

    bytes_left = (uint32_t)w * (uint32_t)h * 2U;
    LCD_SetAddress(x, y, (uint16_t)(x + w - 1U), (uint16_t)(y + h - 1U));
    LCD_Select();
    LCD_DataMode();

    while (bytes_left > 0U) {
        chunk = (bytes_left > 65535U) ? 65535U : (uint16_t)bytes_left;
        (void)LCD_WriteBytes(bytes, chunk);
        bytes += chunk;
        bytes_left -= chunk;
    }

    LCD_Unselect();
}

HAL_StatusTypeDef LCD_DrawRGB565BitmapAsync(uint16_t x,
                                            uint16_t y,
                                            uint16_t w,
                                            uint16_t h,
                                            const uint16_t *pixels,
                                            LCD_TransferDoneCallback_t done_cb,
                                            void *context)
{
    uint32_t total_pixels;

    if ((pixels == NULL) || (w == 0U) || (h == 0U) || (x >= LCD_WIDTH) || (y >= LCD_HEIGHT)) {
        if (done_cb != NULL) {
            done_cb(context);
        }
        return HAL_OK;
    }
    if (lcd_dma_active != 0U) {
        return HAL_BUSY;
    }
    if (hspi3.hdmatx == NULL) {
        LCD_DrawRGB565Bitmap(x, y, w, h, pixels);
        if (done_cb != NULL) {
            done_cb(context);
        }
        return HAL_OK;
    }

    if ((uint32_t)x + w > LCD_WIDTH) {
        w = (uint16_t)(LCD_WIDTH - x);
    }
    if ((uint32_t)y + h > LCD_HEIGHT) {
        h = (uint16_t)(LCD_HEIGHT - y);
    }

    total_pixels = (uint32_t)w * (uint32_t)h;
    lcd_dma_pixels = pixels;
    lcd_dma_pixels_left = total_pixels;
    lcd_dma_done_cb = done_cb;
    lcd_dma_done_context = context;
    lcd_dma_chunk_done = 0U;
    lcd_dma_error_pending = 0U;
    lcd_dma_active = 1U;

    LCD_SetAddress(x, y, (uint16_t)(x + w - 1U), (uint16_t)(y + h - 1U));
    LCD_Select();
    LCD_DataMode();

    (void)LCD_StartNextDmaChunk();
    return HAL_OK;
}

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
 * @brief  缁樺埗绌哄績鐭╁舰
 * @param  x: 宸︿笂瑙扻鍧愭爣
 * @param  y: 宸︿笂瑙扽鍧愭爣
 * @param  w: 鐭╁舰瀹藉害
 * @param  h: 鐭╁舰楂樺害
 * @param  color: 杈规棰滆壊 (RGB565鏍煎紡)
 * @note   閫氳繃缁樺埗鍥涙潯杈圭嚎瀹炵幇
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

/* 7 娈垫暟鐮佺娈典綅锛坆it0=a 椤舵í, bit1=b 鍙充笂, bit2=c 鍙充笅, bit3=d 搴曟í,
   bit4=e 宸︿笅, bit5=f 宸︿笂, bit6=g 涓í锛夊搴旀暟瀛?0-9 鐨勪寒鐏〃 */
static const uint8_t lcd_seg7_table[10] = {
    0x3FU, 0x06U, 0x5BU, 0x4FU, 0x66U, 0x6DU, 0x7DU, 0x07U, 0x7FU, 0x6FU
};

/**
 * @brief  鐢讳竴涓?7 娈垫暟鐮佺鏁板瓧
 * @param  x: 鏁板瓧妗嗗乏涓婅 X
 * @param  y: 鏁板瓧妗嗗乏涓婅 Y
 * @param  w: 鏁板瓧妗嗗搴?
 * @param  h: 鏁板瓧妗嗛珮搴?
 * @param  t: 娈电矖
 * @param  digit: 0-9 鏄剧ず瀵瑰簲鏁板瓧锛屽叾瀹冨€硷紙濡?0xFF锛夊彧娓呰儗鏅樉绀虹┖鐧?
 * @param  color: 娈甸鑹?
 * @param  bg: 鑳屾櫙鑹?
 */
void LCD_DrawSeg7Digit(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t t,
                       uint8_t digit, uint16_t color, uint16_t bg)
{
    uint16_t vlen;
    uint16_t ym;
    uint8_t seg;

    if ((h < (3U * t)) || (w < (2U * t))) {
        return;
    }

    /* digit>9 瑙嗕负绌虹櫧锛堝叏娈电唲鐏級锛涙闂撮殭鍖烘亽涓鸿儗鏅紝鐢遍甯ф竻灞忎繚璇侊紝鏈嚱鏁颁笉鍐嶆暣鍧楅娓咃紝
       閫愭鐩存帴鍐欍€屼寒=color / 鐏?bg銆嶏紝閬垮厤姣忓抚鍑虹幇鍏ㄩ粦涓棿鎬侀€犳垚鐨勫埛鏂伴棯鐑?*/
    seg = (digit > 9U) ? 0x00U : lcd_seg7_table[digit];

    vlen = (uint16_t)((h - 3U * t) / 2U);
    ym = (uint16_t)(y + t + vlen);

    LCD_Fill(x + t, y, x + w - 1U - t, y + t - 1U, ((seg & 0x01U) != 0U) ? color : bg);          /* a */
    LCD_Fill(x + w - t, y + t, x + w - 1U, ym - 1U, ((seg & 0x02U) != 0U) ? color : bg);         /* b */
    LCD_Fill(x + w - t, ym + t, x + w - 1U, y + h - 1U - t, ((seg & 0x04U) != 0U) ? color : bg); /* c */
    LCD_Fill(x + t, y + h - t, x + w - 1U - t, y + h - 1U, ((seg & 0x08U) != 0U) ? color : bg);  /* d */
    LCD_Fill(x, ym + t, x + t - 1U, y + h - 1U - t, ((seg & 0x10U) != 0U) ? color : bg);         /* e */
    LCD_Fill(x, y + t, x + t - 1U, ym - 1U, ((seg & 0x20U) != 0U) ? color : bg);                 /* f */
    LCD_Fill(x + t, ym, x + w - 1U - t, ym + t - 1U, ((seg & 0x40U) != 0U) ? color : bg);        /* g */
}

/**
 * @brief  鏄剧ず涓€涓甫绗﹀彿銆? 浣嶅皬鏁扮殑瀹氱偣鏁帮紙杈撳叆涓?脳10 鏁存暟锛?
 * @param  x: 鏁板€煎尯宸︿笂瑙?X
 * @param  y: 鏁板€煎尯宸︿笂瑙?Y
 * @param  w: 鍗曚釜鏁板瓧瀹藉害
 * @param  h: 鍗曚釜鏁板瓧楂樺害
 * @param  t: 娈电矖
 * @param  value_x10: 瀹為檯鍊?脳10锛屽 1234 琛ㄧず 123.4
 * @param  color: 棰滆壊
 * @param  bg: 鑳屾櫙鑹?
 * @retval 缁樺埗缁撴潫鍚庣殑鍙宠竟鐣?X锛屼究浜庣户缁帓鐗?
 * @note   鍥哄畾甯冨眬銆岀鍙?+ 3 浣嶆暣鏁?+ 灏忔暟鐐?+ 1 浣嶅皬鏁般€嶏紝鍓嶅 0 鏄剧ず涓虹┖鐧斤紱
 *         閫愪綅瑕嗙洊鑳屾櫙锛岄伩鍏嶄綅鏁板彉鍖栨椂娈嬪奖銆?
 */
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

    /* 绗﹀彿浣嶏細鍙湪妯潬鍖哄啓浜?鐏紝鍏朵綑鎭掍负鑳屾櫙锛堥甯у凡娓呭睆锛屾棤闇€姣忓抚鏁村潡娓咃級 */
    LCD_Fill(cx, y + (h - t) / 2U, cx + sw - 1U, y + (h - t) / 2U + t - 1U, (neg != 0U) ? color : bg);
    cx = (uint16_t)(cx + sw + gap);

    /* 鐧句綅锛?100 鐣欑┖锛?*/
    LCD_DrawSeg7Digit(cx, y, w, h, t, (ip < 100U) ? 0xFFU : hundreds, color, bg);
    cx = (uint16_t)(cx + w + gap);
    /* 鍗佷綅锛?10 鐣欑┖锛?*/
    LCD_DrawSeg7Digit(cx, y, w, h, t, (ip < 10U) ? 0xFFU : tens, color, bg);
    cx = (uint16_t)(cx + w + gap);
    /* 涓綅 */
    LCD_DrawSeg7Digit(cx, y, w, h, t, ones, color, bg);
    cx = (uint16_t)(cx + w + gap);

    /* 灏忔暟鐐癸紙甯镐寒锛岀偣澶栧尯鍩熸亽涓鸿儗鏅紝鏃犻渶姣忓抚鏁村潡娓咃級 */
    LCD_Fill(cx, y + h - t, cx + t - 1U, y + h - 1U, color);
    cx = (uint16_t)(cx + sw + gap);

    /* 灏忔暟浣?*/
    LCD_DrawSeg7Digit(cx, y, w, h, t, frac, color, bg);
    cx = (uint16_t)(cx + w + gap);

    return cx;
}

/* 鐢?3 浣嶆棤绗﹀彿鏁存暟锛堢櫨/鍗?涓級锛岄珮浣嶅墠瀵奸浂鐣欑┖锛屼釜浣嶆亽鐢汇€傜敤浜庡績鐜囩瓑鏁存暟鍊笺€?*/
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






