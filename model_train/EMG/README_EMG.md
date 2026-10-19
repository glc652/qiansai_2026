# 肌电数据采集、曲线查看与训练说明

## 1. 板端串口输出格式

当前上臂端通过调试串口 `UART4` 输出肌电数据，波特率：

```text
115200
```

当前打印格式：

```text
EMG,raw,baseline_x10,rectified_x10,rms_x10,calibrated,strength
```

示例：

```text
EMG,1822,19358,1138,772,1,6
```

字段含义：

| 字段 | 含义 |
| --- | --- |
| `EMG` | 固定前缀，脚本用来识别肌电数据行 |
| `raw` | ADC 原始采样值，范围约 `0-4095` |
| `baseline_x10` | 静息基线乘以 10，例如 `19358` 表示 baseline 为 `1935.8` |
| `rectified_x10` | `abs(raw - baseline) * 10`，表示去基线后的肌电幅值 |
| `rms_x10` | RMS 特征乘以 10，用于表示一段时间内的稳定肌电强度 |
| `calibrated` | 静息标定是否完成，`1` 才能开始采集有效数据 |
| `strength` | 发力强度百分比，范围 `0-100` |

上电后需要先等待静息标定完成：

```text
约 3 秒 warmup + 2 秒 baseline
```

采集前请保持手臂完全放松，直到 `calibrated=1`。

## 2. 接线与串口

使用 USB-TTL 连接调试串口：

| 板子 | USB-TTL |
| --- | --- |
| `UART4_TX / PA0` | `RX` |
| `GND` | `GND` |

注意：

- USB-TTL 必须是 `3.3V TTL` 电平。
- 不要把 USB-TTL 的 `5V` 接到板子。
- 如果只采集板端输出，通常只接 `TX -> RX` 和 `GND` 即可。

## 3. 实时曲线查看

脚本：

```text
E:\qiansai\model_train\EMG\plot_emg_live.py
```

运行：

```powershell
python E:\qiansai\model_train\EMG\plot_emg_live.py --port COM7
```

如果不知道串口号：

```powershell
python E:\qiansai\model_train\EMG\plot_emg_live.py
```

脚本会列出可用串口并让你选择。

实时曲线包含：

| 曲线 | 含义 |
| --- | --- |
| `raw` | ADC 原始值 |
| `baseline` | 静息基线 |
| `rectified` | 去基线幅值 |
| `rms` | RMS 强度特征 |
| `strength` | 发力强度百分比 |

建议先用这个脚本确认：

1. 放松时 `rectified/rms/strength` 较低。
2. 轻微发力时 `rectified/rms/strength` 有明显上升。
3. 正常发力时曲线进一步上升。
4. 如果放松时曲线大幅抖动，优先检查电极贴合、绑带稳定和 GND 共地。

## 4. 数据采集

脚本：

```text
E:\qiansai\model_train\EMG\collect_emg_data.py
```

运行：

```powershell
python E:\qiansai\model_train\EMG\collect_emg_data.py --port COM7 --out E:\qiansai\model_train\EMG\emg_dataset.csv
```

如果不知道串口号：

```powershell
python E:\qiansai\model_train\EMG\collect_emg_data.py --out E:\qiansai\model_train\EMG\emg_dataset.csv
```

默认采集设置：

| 参数 | 默认值 | 含义 |
| --- | --- | --- |
| `--rounds` | `5` | 每个标签采集 5 轮 |
| `--duration` | `30` | 每轮采集 30 秒 |
| `--prepare` | `5` | 每轮开始前准备倒计时 5 秒 |
| `--rest` | `5` | 每轮之间休息 5 秒 |
| `--labels` | `relax weak active` | 三分类标签 |

可自定义：

```powershell
python E:\qiansai\model_train\EMG\collect_emg_data.py --port COM7 --out E:\qiansai\model_train\EMG\emg_dataset.csv --rounds 3 --duration 20
```

采集标签含义：

| 标签 | 采集方式 |
| --- | --- |
| `relax` | 手臂完全放松，不主动用力 |
| `weak` | 轻微发力，保持稳定，不要抖动 |
| `active` | 正常训练发力，不需要最大用力 |

采集脚本会等待 `calibrated=1` 后才保存数据。

推荐采集量：

```text
relax   5 轮 x 30 秒
weak    5 轮 x 30 秒
active  5 轮 x 30 秒
```

采集注意：

- 上电后先保持放松，等待标定完成。
- `weak` 不是一下一下用力，而是轻微收缩并稳定保持。
- `active` 是正常训练发力，不是最大爆发用力。
- 每次重新佩戴电极后，建议重新采一轮数据。
- 如果中途电极松动，这一轮数据建议废弃。

## 5. 模型训练

训练脚本：

```text
E:\qiansai\model_train\EMG\train_emg_model.py
```

安装训练依赖：

```powershell
pip install pandas numpy scikit-learn joblib
```

训练命令：

```powershell
python E:\qiansai\model_train\EMG\train_emg_model.py E:\qiansai\model_train\EMG\emg_dataset.csv --out-dir E:\qiansai\model_train\EMG\outputs
```

输出文件：

| 文件 | 含义 |
| --- | --- |
| `emg_participation_rf.joblib` | PC 端随机森林模型 |
| `emg_participation_report.json` | 准确率、混淆矩阵、分类报告 |
| `emg_participation_model.c` | 可移植到 STM32 的 C 模型 |
| `emg_participation_model.h` | C 模型头文件 |

训练类别：

```text
relax
weak
active
```

训练脚本使用的归一化特征：

```text
raw / baseline
(raw - baseline) / baseline
drop / baseline
rectified / baseline
envelope / baseline
rms / baseline
```

## 6. 采集前检查清单

开始正式采集前确认：

1. 调试串口能看到 `EMG,...`。
2. `calibrated` 能在上电约 5 秒后变成 `1`。
3. 放松时 `strength` 较低。
4. 发力时 `rectified_x10`、`rms_x10`、`strength` 会升高。
5. USB-TTL 是 3.3V 电平。
6. 肌电传感器和开发板 GND 已共地。
7. 电极贴在肌腹位置，绑带稳定。

## 7. 当前板端采样设置

当前采集固件为降低阻塞风险，采用软件轮询 ADC：

```text
ADC 采样周期：10 ms，约 100 Hz
串口输出周期：100 ms，约 10 Hz
```

当前用于采集 `relax/weak/active` 三类稳定发力状态是够用的。后续如果要恢复训练正式版，可以再把采集模式和息屏配置切回正式设置。
