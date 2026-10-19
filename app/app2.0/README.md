# 智能康复臂带家属端 app2.0

这是独立于 app1.0 的第二版手机端，不修改 Flask 后端和原有历史数据文件。

## 2.0 数据策略

- App 启动时先从 IndexedDB 本地数据库读取训练、健康、分析及最近实时数据。
- Flask 在线时，将 `/api/history` 和 `/api/health_history` 返回的记录按 ID 增量合并到本地。
- Flask 暂时离线时，不清空页面，仍可浏览已经保存的全部历史数据。
- 历史和健康数据每 15 秒同步一次；App 回到前台时立即同步。
- “实时”页面每 2 秒读取 `/api/latest`，显示动作、角度、肌电、完成度、次数、评分及健康状态。
- 本地数据随 App 保留，只有卸载 App 或在系统设置中清除应用数据时才会删除。

## 功能

- Apple Health 风格界面：浅灰背景、白色健康卡片、大数字指标和轻量层次。
- 双主题色：训练记录使用 Apple 蓝，健康记录使用 Apple 红。
- 流动背景：加入低透明度蓝红动态背景，增强高级感，并支持系统减少动态效果设置。
- 比赛主题词：顶部展示“AI赋能设计，设计点亮AI”，不显示“赛事主题词”标签，避免占用作品功能区域。
- 首页：最近训练、康复指数、健康摘要、重点建议。
- 历史训练：读取 `/api/history`。
- 健康记录：读取 `/api/health_history`。
- 趋势分析：读取 `/api/analysis`，并在前端绘制评分趋势。
- AI建议：调用 `/api/ai_chat`，让后端已配置的 API 模型生成康复训练建议。
- 设置：可配置 Flask 服务端地址。

## API 模型说明

App 端不直接保存模型 Key，也不直接调用外部模型服务。当前流程是：

```text
手机 App -> app1.0 本地代理 -> Flask /api/ai_chat -> 后端模型 API
```

如果页面提示模型调用失败，需要检查 `E:\qiansai\web\.env` 中的模型 API Key 是否已经配置，并确认 Flask 服务重新启动后读取到了环境变量。

## 运行

先启动原来的 Flask 上位机服务，再运行：

```powershell
cd E:\qiansai\app\app1.0
npm start
```

电脑打开：

```text
  http://localhost:5175
```

手机访问时，手机和电脑需要在同一个 WiFi 下，地址一般为：

```text
  http://电脑局域网IP:5175
```

默认后端地址是 `http://127.0.0.1:5000`。如果 Flask 不在本机 5000 端口，可用环境变量指定：

```powershell
$env:REHAB_API_BASE="http://192.168.1.8:5000"; npm start
```

模型接口可能响应较慢，app1.0 代理默认等待 45 秒。需要调整时可设置：

```powershell
$env:PROXY_TIMEOUT_MS="60000"; npm start
```
