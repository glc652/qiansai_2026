# 上臂端工程记忆文档

更新时间：2026-07-17

## 工程位置

- 上臂端工程：`E:\qiansai\upper-arm`
- 手腕端工程：`E:\qiansai\U575`
- 参考姿态工程：`E:\qiansai\stm32u5-rehab-band-master\U575`
- 参考上臂工程：`E:\qiansai\stm32u5-rehab-band-master\upper-arm`
- Web 端工程：`E:\qiansai\web`
- 旧动作判定说明：`E:\qiansai\备赛\方案\三动作判定与评估说明.md`

## 当前重要结论

1. 上臂端 IMU 单独运行时姿态角正常，多任务一起运行时曾经角度异常。
2. 多任务姿态角异常的关键修复是启用 FreeRTOS FPU 上下文保存：
   - 文件：`Core\Inc\FreeRTOSConfig.h`
   - 配置：`#define configENABLE_FPU 1`
3. 姿态角解算目前按参考手腕端工程移植，手腕端和上臂端都使用这套解算。
4. 上臂端必须保留的任务/模块包括：
   - IMU 姿态解算
   - ADC/EMG 采样
   - LVGL 界面
   - 语音模块
   - 无线手腕端接收
   - WiFi/ESP8266 上传
5. WiFi 上传曾经影响姿态角，后来确认和发送包长度、任务运行方式有关；目前通过缩短部分上传内容、恢复合理任务调度后可工作。
6. 健康数据上传逻辑已调整：平常不主动上传健康数据，只有手腕端进入健康测量或上臂端健康页面请求时才上传。
7. 上臂外展不是只看上臂端角度，还需要上臂端和手腕端 y 轴转角共同满足条件。
8. 动作索引当前约定：
   - `0`：肘关节屈伸
   - `1`：肩部上举
   - `2`：前臂旋转
   - `3`：上臂外展

## 当前已做修改

### 1. 姿态角和任务调度

- 上臂端姿态角解算代码按参考手腕端工程同步。
- 手腕端姿态角也按参考工程调整并测试正常。
- 上臂端多任务运行角度异常时，最终确认和 FreeRTOS FPU 保存有关，已启用：
  - `Core\Inc\FreeRTOSConfig.h`
  - `configENABLE_FPU 1`
- `SensorTask` 维持固定 5ms 周期，不使用实际 dt 参与解算。
- 空转任务已关闭，避免无意义占用调度。

### 2. 上臂端任务/模块状态

当前正式运行状态应启用：

```c
#define UPPER_ALGO_TASK_ENABLE      (1U)
#define UPPER_EMG_TASK_ENABLE       (1U)
#define UPPER_WIRELESS_TASK_ENABLE  (1U)
#define UPPER_DEBUG_TASK_ENABLE     (1U)
#define UPPER_WIFI_TASK_ENABLE      (1U)
```

关闭空任务：

```c
#define UPPER_DEFAULT_TASK_ENABLE   (0U)
#define UPPER_UI_IDLE_TASK_ENABLE   (0U)
```

主要模块保持启用：

```c
#define UPPER_ADC_MODULE_ENABLE        (1U)
#define UPPER_WIRELESS_MODULE_ENABLE   (1U)
#define UPPER_VOICE_MODULE_ENABLE      (1U)
#define UPPER_VIBRATION_MODULE_ENABLE  (1U)
#define UPPER_WIFI_MODULE_ENABLE       (1U)
```

### 3. LVGL 界面

- 临时黑屏/角度调试界面已经关闭。
- 当前应使用 LVGL 正常主页和训练界面。
- 训练页面中实时角度曲线和进度条已经加回。
- 上臂端主页也曾加过实时姿态角显示用于排查。

### 4. WiFi / Web 上传

- WiFi 模块已重新启用。
- WiFi 上传周期曾经临时拉长排查，后续要求改回原来的速度。
- 训练页面曾测试关闭 WiFi 上传以排查姿态角问题。
- 目前网页连接问题已排查过，HTTP/MQTT 服务需要先开。
- 非训练上传包中健康页面标志已包含：
  - `wh`
  - `whe`
  - `whexit`
  - `hreq`
- 健康数据上传已经改为：只有手腕端健康检测或上臂端健康请求时才上传，不再平常一直传。
- 后续如果网页健康检测界面进不去，优先检查这些标志是否还在上传包里。

### 5. 正式训练动作

已重点看过：

- 肘关节屈伸
- 上臂外展
- 肩部上举
- 前臂旋转

上臂外展判断要注意：

- 是上臂带动小臂一起外展。
- 因此不能只看上臂端 y 轴。
- 当前逻辑取上臂端 y 轴变化和手腕端 y 轴变化共同约束，避免只动一端误判。

### 6. 自由训练计数

最近一次修改：自由训练模式每完成一个动作后加一次。

修改文件：

- `Core\Src\app_freertos.c`

原问题：

- 之前自由训练是识别到动作就尝试加次数。
- 如果动作锁存或动作没有干净回到 `IDLE`，次数可能不加。

当前逻辑：

1. 某个动作角度超过进入阈值，标记动作开始。
2. 角度回落到退出阈值以下，判定该动作完成一次。
3. 对应动作次数 `free_action_counts[i]++`。
4. 总次数 `free_total_reps++`。

使用的自由训练状态变量：

```c
uint8_t free_motion_active[4];
int32_t free_motion_peak_x100[4];
uint32_t free_last_count_tick[4];
```

动作阈值复用正式动作阈值：

- 肘关节屈伸：`REHAB_ELBOW_HOLD_ENTER_X100` / `REHAB_ELBOW_HOLD_EXIT_X100`
- 肩部上举：`REHAB_SHOULDER_HOLD_ENTER_X100` / `REHAB_SHOULDER_HOLD_EXIT_X100`
- 前臂旋转：`REHAB_FOREARM_ROT_HOLD_ENTER_X100` / `REHAB_FOREARM_ROT_HOLD_EXIT_X100`
- 上臂外展：`REHAB_ARM_ABD_HOLD_ENTER_X100` / `REHAB_ARM_ABD_HOLD_EXIT_X100`

修改后编译通过：

```text
"U575\U575.axf" - 0 Error(s), 0 Warning(s).
```

## 常用编译命令

```powershell
& 'E:\Keil ARM\Core\UV4\UV4.exe' -b 'E:\qiansai\upper-arm\MDK-ARM\U575.uvprojx' -j0 -o 'E:\qiansai\upper-arm\MDK-ARM\codex_build.log'
Get-Content -LiteralPath 'E:\qiansai\upper-arm\MDK-ARM\codex_build.log' | Select-Object -Last 60
```

## 下次继续这个工程时优先检查

1. 先读这个文件。
2. 再看 `Core\Src\app_freertos.c` 中：
   - `StartSensorTask`
   - `StartAlgoTask`
   - WiFi 上传任务
   - 自由训练计数逻辑
3. 如果姿态角在多任务下再次异常，优先确认：
   - `configENABLE_FPU` 是否仍为 `1`
   - IMU 任务是否固定 5ms
   - 是否有大量串口/WiFi阻塞式发送
   - 是否有调试打印重新打开
4. 如果网页或健康页面异常，优先确认：
   - HTTP/MQTT 服务是否开启
   - WiFi 上传包是否还包含页面状态标志
   - 健康数据是否只在健康检测请求时上传
5. 如果动作计数异常，优先区分：
   - 正式训练计数
   - 自由训练计数
   - 页面显示计数
   - Web 上传计数

