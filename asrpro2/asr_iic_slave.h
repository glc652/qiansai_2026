#ifndef __ASR_IIC_SLAVE_H
#define __ASR_IIC_SLAVE_H

#include "system.h"
#include "gpio.h"
#include "delay.h"
#include "asr_api.h"

// IIC从机7位地址，和STM32主机统一
#define ASR_DEV_ADDR    0x60

// 寄存器映射，与STM32主机一一对应
#define REG_STATUS      0x00    // 识别状态：0空闲/1识别中/2识别完成
#define REG_STR_LEN     0x01    // 识别字符串有效长度
#define REG_WAKE_FLAG   0x50    // 唤醒标志：1唤醒，0未唤醒
#define REG_DATA_BUF    0x10    // 识别文本缓存起始地址

#define STR_BUF_MAX     64      // 最大识别字符串长度

// 全局寄存器内存池
extern uint8_t iic_reg_buf[0x60];
// 语音字符串缓存
extern char asr_result_buf[STR_BUF_MAX];

// 初始化软件IIC从机
void SoftIIC_Slave_Init(void);
// IIC从机轮询处理函数，放while(1)
void IIC_Slave_Task(void);

// 对外业务接口（语音事件调用）
void Update_ASR_Status(uint8_t sta);
void Set_Wake_Flag(uint8_t flag);
void Save_Recog_String(const char *str);

#endif