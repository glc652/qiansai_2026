# Web端技术分析

## 模块定位

`F:\qiansai\web` 是康复臂带系统的服务端与浏览器仪表盘，负责设备数据接入、协议解析、状态归一化、历史保存、训练分析、实时推送和 AI 建议。它不是 MCU 固件，不负责传感器采样或底层动作识别。

源码：`server.py:793` `ingest_device_payload`、`server.py:966` `start_mqtt_subscriber`、`server.py:1030` `index`。

## 硬件组成

本工程没有本地 MCU、传感器或嵌入式外设。外部数据中出现上臂端、手腕端、电量、蓝牙状态和健康字段；具体 MCU、传感器型号、采样率、ADC/I2C/SPI 参数【无法确认】。服务端默认监听 HTTP `0.0.0.0:5000`，MQTT 默认连接 `127.0.0.1:1883`。

## 软件架构

Flask 路由接收 HTTP/MQTT；`parse_kv_payload` 解析键值报文；`normalize_realtime`、`normalize_devices`、`normalize_health` 统一字段；JSON 文件保存历史；`build_analysis_payload` 计算训练分析；`/events` 通过 SSE 推送；`dashboard.html` 负责页面和图表。

## 上电初始化流程

`server.py` 导入并读取 `.env`，初始化全局状态和数据路径；底部调用 `load_training_history()`、`load_health_history()` 和 `start_mqtt_subscriber()`；直接运行时 `app.run(..., port=5000, debug=False, use_reloader=False)`。浏览器打开 `/` 后加载仪表盘，调用 `/api/latest`，建立 SSE，失败时以轮询回退。

## 实时任务/中断

没有 RTOS、MCU 中断、DMA、ADC、I2C、SPI 或串口驱动。Web 侧有 paho MQTT 网络循环、Flask 请求、SSE 条件变量、浏览器 400 ms 轮询回退和动画帧。MQTT 异常断开使用 1~30 秒自动重连；SSE 等待状态版本变化，超时 3 秒。

## 传感器数据处理链

外部设备采集/算法【不在本工程】→ HTTP `/upload` 或 MQTT → `ingest_device_payload` → `parse_kv_payload` → 运动/健康分流 → 归一化 → `latest_data`/`latest_health` → JSON 历史和 SSE → `dashboard.html` 的 `applyRealtime`、`renderHealth`。Web 只处理已经上传的数值，不处理原始波形。

## 核心算法

- 键值解析与字段别名兼容：`server.py:97` `parse_kv_payload`。
- 动作角度选择、0~180 限幅、完成度计算：`server.py:157` `normalize_realtime`。
- 训练恢复指数：`server.py:640` `build_analysis_payload`，按平均评分、完成度、幅度和活跃天数加权。
- 健康阈值规则与前端显示平滑：`dashboard.html` 的 `evaluateHealthIndicator`、`healthScore`、`predictedAngle`、`updateSmoothHeartRate`。
- AI 上下文及调用：`server.py:716` `build_ai_context`、`server.py:751` `call_dashscope_chat`。

## 通信协议

设备到 Web 支持 `POST /upload`，JSON/表单/原始文本均可，核心载荷是逗号或 `&` 分隔的 `key=value`，无 Web 侧 CRC、固定帧头或长度校验。MQTT 默认订阅 `rehab/upper/#`、QoS 0；payload 可以是键值字符串或带 `data` 字段的 JSON。Web 到浏览器提供 `/api/latest`、历史/分析 API 和 `/events` SSE。具体设备帧头、CRC、序号、重传和无线芯片【无法确认】。

## 与其他端的数据交互

上臂端提供动作、角度、次数、评分、肌电和电量；手腕端提供健康数据和状态标志；Web 统一汇总后返回浏览器。浏览器可调用 `/api/start`、`/api/stop`，但这两个接口源码只修改 Web 状态，是否实际控制设备【无法确认】。AI 请求会把训练上下文发送到外部模型接口。

## 关键函数索引

`server.py:97 parse_kv_payload`；`server.py:157 normalize_realtime`；`server.py:261 normalize_devices`；`server.py:285 normalize_health`；`server.py:389 add_history_record`；`server.py:640 build_analysis_payload`；`server.py:793 ingest_device_payload`；`server.py:835 upload_data`；`server.py:866 realtime_events`；`server.py:966 start_mqtt_subscriber`；`dashboard.html:1416 applyRealtime`。

## 技术亮点

统一接入入口、字段归一化、HTTP/MQTT 双通道、MQTT 自动重连、SSE 实时推送、轮询降级、历史去重和临时文件替换保存体现了模块化与可靠性。需要诚实说明：匿名 MQTT、明文 HTTP、缺少鉴权/TLS/CRC/数据库事务等生产能力在源码中未见。

## 答辩值得讲的内容

重点讲 `ingest_device_payload → normalize_* → build_latest_payload → /events → applyRealtime` 的端到端数据闭环，以及设备端算法与 Web 展示层的职责边界。训练分析采用明确公式，便于解释和复现；AI 只作为辅助建议。

## 可能被评委追问的问题

实际端到端延迟如何测量？QoS 0 丢包如何处理？HTTP/MQTT 重复上传如何去重？角度是否为真实关节角度？健康阈值是否经过医学验证？`/api/start` 是否真正控制设备？生产环境如何解决匿名 MQTT、明文 HTTP、鉴权和隐私问题？以上未被源码完整证明的内容均为【无法确认】。

## 无法从代码确认的信息

MCU 和传感器型号、采样与滤波算法、实际设备发送方、完整帧校验协议、生产部署、认证授权、TLS、AI 医学准确性和临床验证均【无法确认】。
