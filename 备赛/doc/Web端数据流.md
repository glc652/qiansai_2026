# Web端数据流

## 1. 设备到服务端

```text
上臂/手腕设备
  ├─ HTTP POST /upload
  └─ MQTT rehab/upper/#
          ↓
upload_data / start_mqtt_subscriber.on_message
          ↓
ingest_device_payload
          ↓
parse_kv_payload
          ↓
实时数据分支或健康数据分支
```

输入可以是 `key=value,key=value` 或 `&` 分隔文本，也可为含 `data` 字段的 JSON。Web 侧没有实现帧头、固定长度、CRC 或签名校验。

## 2. 归一化与存储

实时分支进入 `normalize_realtime`，形成动作、角度、次数、评分、完成度、肌电和电量；设备分支进入 `normalize_devices`；健康分支进入 `normalize_health`，并由 `add_health_history_record` 去重保存。训练结果由 `maybe_add_training_history_from_payload` 和 `add_history_record` 形成记录。

## 3. 服务端到浏览器/APP

```text
latest_data/latest_health
        ├─ GET /api/latest → 浏览器或 APP
        ├─ GET /api/history、/api/health_history
        ├─ GET /api/analysis
        └─ GET /events SSE → dashboard.html
```

Web 仪表盘优先使用 SSE，失败时轮询；APP 2.0 主动每 2 秒轮询 `/api/latest`，每 15 秒同步历史、健康和分析。

## 4. 分析与 AI

训练历史 → `build_analysis_payload` → 平均评分/完成度/幅度/活跃天数 → 恢复指数、薄弱动作和建议；`build_ai_context` 再把统计和最近记录交给 `call_dashscope_chat`。AI 是外部服务，输出不等于设备端医学算法。

## 5. 关键风险

MQTT 默认 QoS 0，配置中存在匿名 Broker；Web 侧无鉴权、TLS、CRC 和数据库事务。真实丢包、延迟、重传和生产安全策略【无法确认】。
