# APP端数据流

## 1. 总体链路

```text
设备端传感器与算法
        ↓
Flask/Web 后端统一 JSON
        ↓ HTTP GET
Node /proxy 或 APP 直接 API_BASE
        ↓
app.js fetchJson
        ↓
state.latest / state.history / state.health / state.analysis
        ↓
实时页、首页、历史、健康、趋势、AI 建议
```

APK 只是在 Android WebView 中加载同一套网页资源，未发现原生蓝牙、串口或传感器采集逻辑。

## 2. 启动与同步流

`initializeApp` → `loadLocalData` 读取 IndexedDB → `loadData` 并行请求 `/api/history`、`/api/health_history`、`/api/analysis` → `mergeRecords` 增量合并 → `saveLocalData` → `startBackgroundSync`。

实时数据由 `pollRealtime` 每 2 秒请求 `/api/latest`；历史、健康和分析每 15 秒同步；页面回到前台时立即刷新。网络失败时保留本地数据并显示离线状态。

## 3. Node 与 APK 流

浏览器：浏览器 → Node `server.js` → `/proxy/api/*` → Flask。

APK：`MainActivity.onCreate` → `file:///android_asset/www/index.html` → 内置 `app.js` → 后端地址。APK 不包含 Flask 后端，仍需要可访问的服务器。

## 4. 展示处理

`renderRealtime` 映射动作、角度、肌电、完成度、次数、评分、健康和设备连接状态；`renderHistory` 支持动作筛选/评分排序；`drawScoreChart` 用 Canvas 绘制评分趋势；`sendModelMessage` 请求 `/api/ai_chat`，手机端不运行模型。

## 5. 可靠性与限制

IndexedDB 提供离线历史浏览和增量去重；`recordKey` 以 ID、健康序号或时间/动作生成键。没有离线设备数据上行队列、蓝牙重连、数据加密或用户鉴权代码；后端算法、传感器采集和实际无线协议【无法确认】。
