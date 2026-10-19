#include "system.h"
#include "asr_api.h"
#include "asr_iic_slave.h"

// 语音唤醒回调函数
void wakeup_callback(void)
{
    Set_Wake_Flag(1);
    Update_ASR_Status(1);
}

// 语音识别完成回调函数
void recog_callback(uint16_t cmd_id, const char *str)
{
    Save_Recog_String(str);
    Update_ASR_Status(2);

    delay_ms(800);
    Update_ASR_Status(0);
    Set_Wake_Flag(0);
}

int main(void)
{
    // 系统初始化
    system_init();
    // IIC从机初始化
    SoftIIC_Slave_Init();

    // 注册语音事件回调
    asr_set_wakeup_cb(wakeup_callback);
    asr_set_recog_cb(recog_callback);
    // 启动离线语音识别
    asr_start();

    while(1)
    {
        IIC_Slave_Task();   // 轮询处理IIC主机读写
        asr_loop();         // 语音后台处理（必须调用，否则语音失效）
        delay_ms(10);
    }
}