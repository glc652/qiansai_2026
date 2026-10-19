#include "asr_iic_slave.h"
#include <string.h>

uint8_t iic_reg_buf[0x60] = {0};
char asr_result_buf[STR_BUF_MAX] = {0};

// IO引脚定义（SDA=PA0 SCL=PA1，可自行修改）
#define SDA_PIN     PA0
#define SCL_PIN     PA1

#define SDA_OUT()   gpio_set_mode(SDA_PIN, GPIO_MODE_OUT_PP)
#define SDA_IN()    gpio_set_mode(SDA_PIN, GPIO_MODE_IN_PU)
#define SCL_OUT()   gpio_set_mode(SCL_PIN, GPIO_MODE_OUT_PP)

static void SDA_H(void)  { gpio_write(SDA_PIN, 1); }
static void SDA_L(void)  { gpio_write(SDA_PIN, 0); }
static void SCL_H(void)  { gpio_write(SCL_PIN, 1); }
static void SCL_L(void)  { gpio_write(SCL_PIN, 0); }
static uint8_t Read_SDA(void) { return gpio_read(SDA_PIN); }

// IIC从机初始化
void SoftIIC_Slave_Init(void)
{
    gpio_init(SDA_PIN, GPIO_MODE_IN_PU);
    gpio_init(SCL_PIN, GPIO_MODE_OUT_PP);
    SDA_H();
    SCL_H();
    memset(iic_reg_buf, 0, sizeof(iic_reg_buf));
    memset(asr_result_buf, 0, sizeof(asr_result_buf));
}

// 等待IIC起始信号，匹配本机地址，返回读写标记
static uint8_t IIC_Slave_Wait_Addr(uint8_t *rw_bit)
{
    uint8_t addr = 0, i;
    SDA_IN();
    while(1)
    {
        // 等待START：SCL高，SDA下降沿
        while(Read_SDA() || !gpio_read(SCL_PIN));
        delay_us(1);
        while(!Read_SDA() && gpio_read(SCL_PIN));

        // 读取7位设备地址
        addr = 0;
        for(i = 0; i < 7; i++)
        {
            while(!gpio_read(SCL_PIN));
            addr <<= 1;
            if(Read_SDA()) addr |= 1;
            while(gpio_read(SCL_PIN));
        }
        // 读写位
        while(!gpio_read(SCL_PIN));
        *rw_bit = Read_SDA();
        while(gpio_read(SCL_PIN));

        // 匹配本机地址，回复ACK
        if(addr == ASR_DEV_ADDR)
        {
            SDA_OUT();
            SDA_L();
            delay_us(1);
            SCL_H();
            delay_us(1);
            SCL_L();
            SDA_H();
            return 1;
        }
    }
}

// 主机写寄存器流程
static void IIC_Slave_Write_Reg(void)
{
    uint8_t reg_addr, dat, i;
    SDA_IN();

    // 读取寄存器地址
    reg_addr = 0;
    for(i = 0; i < 8; i++)
    {
        while(!gpio_read(SCL_PIN));
        reg_addr <<= 1;
        if(Read_SDA()) reg_addr |= 1;
        while(gpio_read(SCL_PIN));
    }
    // ACK应答
    SDA_OUT(); SDA_L(); delay_us(1); SCL_H(); delay_us(1); SCL_L(); SDA_H();

    // 读取写入数据
    dat = 0;
    for(i = 0; i < 8; i++)
    {
        while(!gpio_read(SCL_PIN));
        dat <<= 1;
        if(Read_SDA()) dat |= 1;
        while(gpio_read(SCL_PIN));
    }
    SDA_OUT(); SDA_L(); delay_us(1); SCL_H(); delay_us(1); SCL_L(); SDA_H();

    // 写入寄存器缓存
    if(reg_addr < sizeof(iic_reg_buf))
        iic_reg_buf[reg_addr] = dat;
}

// 主机读寄存器流程
static void IIC_Slave_Read_Reg(void)
{
    uint8_t reg_addr, dat, i;
    SDA_IN();

    // 获取寄存器地址
    reg_addr = 0;
    for(i = 0; i < 8; i++)
    {
        while(!gpio_read(SCL_PIN));
        reg_addr <<= 1;
        if(Read_SDA()) reg_addr |= 1;
        while(gpio_read(SCL_PIN));
    }
    SDA_OUT(); SDA_L(); delay_us(1); SCL_H(); delay_us(1); SCL_L(); SDA_H();

    // 等待重复起始，重新匹配读地址
    uint8_t rw;
    while(!IIC_Slave_Wait_Addr(&rw) || rw != 1);

    // 连续输出寄存器数据，主机NACK停止
    while(1)
    {
        dat = (reg_addr < sizeof(iic_reg_buf)) ? iic_reg_buf[reg_addr] : 0;
        SDA_OUT();
        // 输出单字节
        for(i = 0; i < 8; i++)
        {
            if(dat & 0x80) SDA_H(); else SDA_L();
            dat <<= 1;
            delay_us(1);
            SCL_H();
            delay_us(1);
            SCL_L();
        }
        SDA_IN();
        // 读取主机应答
        while(!gpio_read(SCL_PIN));
        uint8_t ack = Read_SDA();
        while(gpio_read(SCL_PIN));
        if(ack == 1) break; // NACK结束传输
        reg_addr++;
    }
}

// IIC从机主处理函数，轮询调用
void IIC_Slave_Task(void)
{
    uint8_t rw_bit;
    if(IIC_Slave_Wait_Addr(&rw_bit))
    {
        if(rw_bit == 0)
            IIC_Slave_Write_Reg();
        else
            IIC_Slave_Read_Reg();
    }
}

// 更新识别状态寄存器
void Update_ASR_Status(uint8_t sta)
{
    iic_reg_buf[REG_STATUS] = sta;
}

// 设置唤醒标志寄存器
void Set_Wake_Flag(uint8_t flag)
{
    iic_reg_buf[REG_WAKE_FLAG] = flag;
}

// 将识别文本存入IIC寄存器缓存
void Save_Recog_String(const char *str)
{
    uint8_t len = strlen(str);
    if(len >= STR_BUF_MAX) len = STR_BUF_MAX - 1;

    iic_reg_buf[REG_STR_LEN] = len;
    memset(&iic_reg_buf[REG_DATA_BUF], 0, STR_BUF_MAX);

    for(uint8_t i = 0; i < len; i++)
    {
        iic_reg_buf[REG_DATA_BUF + i] = str[i];
        asr_result_buf[i] = str[i];
    }
    asr_result_buf[len] = '\0';
}