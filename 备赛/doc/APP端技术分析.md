# APP端技术分析

> 分析对象：`F:\qiansai\app`，重点为 `app2.0` 和 `BAND_APK2.0`。本文依据实际 HTML、JavaScript、Node 服务端、Android WebView 容器和构建配置整理。APP 不是 MCU 固件；无法由 APP 工程确认的硬件采集、底层算法和无线链路信息均标记为【无法确认】。

## 模块定位

APP 2.0 是智能康复臂带的家属端/移动展示端，主要承担训练记录、健康记录、实时状态、趋势分析、AI 建议、离线缓存和设置后端地址等功能。

工程分为两层：

- `app2.0`：HTML/CSS/JavaScript 页面和 Node 静态服务器。Node 服务监听 5175，提供静态文件和 `/proxy/api/*` 反向代理。
- `BAND_APK2.0`：原生 Android WebView 壳，将 `app2.0` 的 `index.html`、`styles.css`、`app.js`、`manifest.json` 放入 `assets/www`，启动时加载 `file:///android_asset/www/index.html`。

因此 APP 在系统中是“数据消费端和人机交互端”，不负责上臂/手腕传感器采样，也不负责 MCU 动作识别或健康信号原始处理。其上游通常是 Web/Flask 服务，但该 APP 工程本身没有设备直连代码。

源码证据：`app2.0/app.js:1`；`app2.0/server.js:1`；`BAND_APK2.0/app/src/main/java/com/qiansai/bandapk/MainActivity.java:1`。

## 硬件组成

- APP 本地硬件：Android 手机或浏览器设备；具体手机型号【无法确认】。
- Android 容器：原生 `Activity + WebView`，包名 `com.qiansai.bandapk2`，版本号 2.0，`minSdk 23`、`targetSdk 35`。
- 数据源硬件：页面显示上臂端、手腕端连接状态和动作/健康字段，但 APP 工程没有对应传感器驱动。
- 网络：手机通过 HTTP 访问 Flask 后端；APK manifest 允许 INTERNET 和明文 HTTP。

MCU 型号、IMU/角度传感器、肌电传感器、心率/血氧传感器、ADC、蓝牙芯片、Wi-Fi 芯片、采样率和引脚均不在 APP 代码中，属于【无法确认】。

源码证据：`BAND_APK2.0/app/build.gradle`；`BAND_APK2.0/app/src/main/AndroidManifest.xml`；`BAND_APK2.0/app/src/main/res/xml/network_security_config.xml`。

## 软件架构

1. 展示层：`index.html` 定义首页、实时、历史、健康、趋势、AI 建议、设置页面；`styles.css` 负责样式和响应式布局。
2. 状态层：`app.js` 的 `state` 保存训练历史、健康历史、分析结果、最新实时数据、在线状态和 AI 对话。
3. 本地缓存层：IndexedDB 数据库 `rehab-band-app2`，对象仓库 `snapshots`，保存 `app-data` 和 `latest`。
4. 数据访问层：`fetchJson()` 统一请求；默认访问相对路径 `/proxy/api/...`，也可在设置中使用 `localStorage` 保存完整后端地址。
5. 同步层：启动时先读本地，再请求 Flask；实时数据每 2 秒请求 `/api/latest`，历史/健康/分析每 15 秒同步。
6. Node 适配层：`server.js` 提供静态文件和后端 API 代理，不解析康复数据。
7. Android 容器层：`MainActivity.onCreate()` 配置 WebView 后加载本地资源，不包含原生业务逻辑。

## 上电初始化流程

### 浏览器/Node 版本

1. 执行 `npm start`，Node 运行 `server.js`。
2. `server.js` 读取 `PORT`，默认监听 `0.0.0.0:5175`；读取 `REHAB_API_BASE`，默认 `http://127.0.0.1:5000`；设置代理超时，默认 45 秒。
3. 浏览器访问 `/`，Node 将其映射为 `index.html`，通过 `serveStatic()` 返回页面资源。
4. `app.js` 创建状态对象，读取 `rehabApiBase`。
5. 设置 API 输入框，绑定页面切换、刷新、筛选、详情弹窗、AI 发送、设置保存等事件。
6. `initializeApp()` 先执行 `loadLocalData()` 读取 IndexedDB，再执行 `loadData()` 并行请求训练历史、健康历史和趋势分析。
7. 成功后 `startBackgroundSync()` 立即读一次实时数据，并建立 2 秒实时定时器和 15 秒历史同步定时器。

### APK 版本

1. Android 启动 `MainActivity.onCreate()`。
2. 创建 WebView 并设为 Activity 内容。
3. 开启 JavaScript、DOM Storage、WebView 数据库、文件访问和跨文件 URL 访问；允许混合内容。
4. 设置 `WebViewClient`，加载 `file:///android_asset/www/index.html`。
5. 内置网页随后执行与浏览器版本相同的 `initializeApp()` 和后台同步流程。
6. 返回键优先调用 WebView 历史回退，否则结束 Activity。

APK 核对报告文件内容仍写有 `BAND_APK1.0` 等旧版本信息，与 `BAND_APK2.0` 的实际 Gradle 配置不一致；报告不能作为 2.0 当前实现的唯一证据。

源码证据：`app2.0/server.js:5-61`；`app2.0/app.js:552-562`；`BAND_APK2.0/.../MainActivity.java:15-42`。

## 实时任务/中断

### RTOS、MCU 中断和底层外设

APP 没有 RTOS、任务优先级、定时器中断、串口中断、DMA、ADC、I2C、SPI、PWM 或传感器驱动。APP 端通过 HTTP 拉取已处理结果；设备端采样和算法实现【无法确认】。

### JavaScript/Node 周期机制

| 机制 | 周期/触发 | 职责 | 数据关系 |
|---|---|---|---|
| `initializeApp` | 页面加载 | 本地恢复后联网同步 | 初始化全部 UI 状态 |
| `pollRealtime` | 每 2000 ms | 请求 `/api/latest` | 写 `state.latest`、更新实时页、缓存 latest |
| `loadData({quiet:true})` | 每 15000 ms | 请求历史、健康、分析 | 增量合并后写 IndexedDB |
| 页面重新可见 | `visibilitychange` | 立即实时和历史同步 | 防止后台期间数据过旧 |
| Node HTTP 请求 | 请求触发 | 静态文件或代理 API | 代理不改变业务 payload |
| Android 返回键 | 用户触发 | WebView 回退或退出 | 不涉及设备数据 |

这些是应用层定时器，不是硬实时机制；2 秒刷新延迟、丢包行为和后台系统节流效果【无法确认】。

## 传感器数据处理链

### 训练数据

设备传感器采集 → 设备端预处理/算法 → Flask/Web 后端生成统一 `realtime` 对象 → APP `pollRealtime()` 请求 `/api/latest` → `state.latest` → `renderRealtime()` → 显示动作、角度、肌电、完成度、次数、评分和设备连接状态。

APP 只使用后端已经归一化的字段，例如 `rt.action`、`rt.display_angle`、`rt.emg_label`、`rt.emg_strength`、`rt.completion`、`rt.reps`、`rt.score` 和 `devices.upper/wrist`。APP 没有从原始 ADC/PPG/IMU/EMG 波形计算这些结果的代码。

### 健康数据

手腕传感器采集 → 手腕端健康算法 → Web 后端 `/api/health_history` 和 `/api/latest` → APP `state.health`/`state.latest.health` → `renderHealth()`/`renderHome()` → 显示心率、血氧、疲劳、血压、SDNN/RMSSD 等字段。

### 本地缓存与增量合并

`loadLocalData()` 从 IndexedDB 恢复历史；`loadData()` 并行拉取远端数据；`mergeRecords()` 用 `recordKey()` 生成训练/健康记录键，合并本地和远端后按时间倒序排列；`saveLocalData()` 再把合并结果写回 IndexedDB。

离线时保留本地历史，实时页显示离线/等待连接；恢复前台时立即刷新。该离线能力只针对 APP 已经缓存过的数据，不表示 APP 可以脱离后端接收设备实时数据。

源码证据：`app2.0/app.js:21-101`、`app2.0/app.js:161-198`、`app2.0/app.js:208-255`。

## 核心算法

1. **本地数据去重与增量合并**：`app.js:64` `recordKey` 和 `app.js:71` `mergeRecords`。优先使用 `id`；健康记录无 ID 时使用 `seq + 时间`；其他记录使用日期/时间与动作组合。通过 Map 覆盖同键记录，再按 `parseTime` 倒序。
2. **时间排序**：`app.js:132` `parseTime` 将 `date/time/created_at/timestamp` 转成时间戳；无法解析时返回 0。
3. **实时状态映射**：`app.js:208` `renderRealtime` 将后端统一字段映射为页面文本，并依据 `emg_mode` 选择 strong/weak/relax/offline 样式；完成度限制由显示 CSS 宽度使用 0~100 的数值。
4. **训练趋势图**：`app.js:428` `drawScoreChart` 使用 Canvas 绘制网格、评分折线、渐变和节点，绘制最近 12 条训练评分。它是可视化算法，不是训练评分算法。
5. **建议兜底规则**：`app.js:298` `fallbackAdvice` 根据最近评分、完成度、幅度和薄弱动作生成简单文本；正式趋势数据来自后端 `/api/analysis`。
6. **AI 对话适配**：`app.js:395` `sendModelMessage` 只负责截取最近 8 条对话并请求 `/api/ai_chat`，不会在手机本地运行模型，也不保存模型 Key。
7. **后端分析的使用**：APP 展示 `recovery_index`、`weak_action`、`action_analysis` 等后端结果；恢复指数的实际公式位于 Web 工程而非 APP 工程，因此 APP 端无法单独证明其医学含义。

## 通信协议

### APP → Web 后端

APP 使用 HTTP `GET` 请求：`/api/latest`、`/api/history`、`/api/health_history`、`/api/analysis`；使用 HTTP `POST /api/ai_chat`，请求体为 JSON，包含 `message` 和 `messages`。

默认路径为 `/proxy` 加 API 路径，例如 `/proxy/api/latest`。Node `proxyApi()` 去掉 `/proxy`，根据 `API_BASE` 拼接目标 URL，转发 HTTP 方法、Content-Type、Content-Length 和请求体，并把后端状态码和 Content-Type 返回给 APP。

### APP → Node 代理

- 入口：`/proxy/api/*`。
- 帧头/固定长度/校验：没有 APP 自定义二进制帧；HTTP 负责长度和传输，无 CRC/校验和。
- 超时：Node 请求超时默认为 45 秒，超时返回 502 JSON。
- 安全：当前代理没有鉴权、签名或加密层；是否由外部反向代理提供 HTTPS【无法确认】。

### Web 后端 → APP

训练与健康记录通过 JSON API 返回；APP 没有 WebSocket、SSE、MQTT 或蓝牙接收实现。实时数据是每 2 秒主动拉取，而不是服务端主动推送。

### APP → AI 服务

APP 不直接调用 AI 服务；`sendModelMessage()` 调用后端 `/api/ai_chat`。API Key 和模型调用由 Flask 后端处理，APP 工程不包含密钥。实际模型、超时和安全策略【无法从 APP 工程确认】。

## 与其他端的数据交互

- 设备/嵌入式端 → Web/Flask：设备数据如何到达 Flask、采用 HTTP 还是 MQTT、完整字段和校验方式不在 APP 工程，属于【无法确认】。
- Web/Flask → APP：返回统一的训练、健康、分析和实时 JSON；APP 只消费这些结果。
- APP → Web/Flask：查询最新状态、历史、健康和分析；发送 AI 问题。
- APP ↔ Node：浏览器运行时由 Node 提供静态页面并代理 API；若设置页填写完整 `apiBase`，则可绕过 Node 代理直接请求该地址。
- APK → Web/Flask：WebView 加载本地资源，仍通过网络请求后端；APK 不内置 Flask 后端。

## 关键函数索引

| 文件路径 + 函数 | 作用 |
|---|---|
| `app2.0/server.js` `send` | 统一返回状态码、禁止缓存和响应头 |
| `app2.0/server.js` `proxyApi` | 将 `/proxy/api/*` 转发到 Flask |
| `app2.0/server.js` `serveStatic` | 安全拼接路径并提供静态资源 |
| `app2.0/app.js` `openLocalDb` | 初始化 IndexedDB 数据库和对象仓库 |
| `app2.0/app.js` `readLocal`/`writeLocal` | 本地读取和保存快照 |
| `app2.0/app.js` `recordKey` | 生成训练/健康记录去重键 |
| `app2.0/app.js` `mergeRecords` | 本地与远端增量合并 |
| `app2.0/app.js` `fetchJson` | 统一 API 请求和 JSON/错误处理 |
| `app2.0/app.js` `loadData` | 并行拉取历史、健康和趋势分析 |
| `app2.0/app.js` `renderRealtime` | 实时动作和健康状态渲染 |
| `app2.0/app.js` `pollRealtime` | 2 秒轮询最新状态 |
| `app2.0/app.js` `startBackgroundSync` | 启动实时与历史同步定时器 |
| `app2.0/app.js` `fallbackAdvice` | 无/弱分析数据时生成前端兜底建议 |
| `app2.0/app.js` `drawScoreChart` | Canvas 评分趋势绘制 |
| `app2.0/app.js` `sendModelMessage` | AI 对话请求和消息展示 |
| `app2.0/app.js` `initializeApp` | APP 启动总流程 |
| `BAND_APK2.0/.../MainActivity.java` `onCreate` | 创建和配置 WebView |
| `BAND_APK2.0/.../MainActivity.java` `onBackPressed` | WebView 回退/Activity 退出 |

## 技术亮点

- **模块化**：数据访问、缓存、合并、渲染、图表、AI 和 Android 壳分开；同一网页资源可在浏览器和 APK 中复用。
- **离线可用**：IndexedDB 保存训练、健康、分析和最近实时快照；后端不可用时仍能浏览已保存记录。
- **增量同步**：通过记录键合并本地/远端数据，避免简单覆盖本地历史。
- **实时交互**：每 2 秒刷新最新数据，页面回到前台会立即刷新；实时页同时展示动作、角度、肌电、完成度、评分和健康指标。
- **可解释展示**：历史按动作筛选和评分排序，趋势用 Canvas 绘制，训练详情提供完成次数、目标次数、角度和幅度等字段。
- **跨端复用**：APK 是 WebView 壳，网页资源与 `app2.0` hash 一致，业务逻辑不需要重复维护。
- **工程化风险**：APK 允许明文 HTTP、混合内容、文件 URL 跨域访问；默认 API 地址依赖局域网服务；没有看到登录鉴权、TLS 证书校验、数据加密和原生蓝牙权限/实现。

## 答辩值得讲的内容

1. **“一套前端，多种运行形态”**：同一份 `app.js/index.html/styles.css` 既能在浏览器运行，也能打包进 Android WebView，减少重复开发。
2. **离线优先与增量同步**：启动先读 IndexedDB，联网后用 `mergeRecords` 合并远端数据；断网时历史仍可看，恢复前台立即同步。
3. **端到端数据链路**：`设备 → Flask → APP pollRealtime → state.latest → renderRealtime`；强调 APP 负责消费和交互，不冒充设备端传感器算法。
4. **家属端信息组织**：首页给出康复指数、最近评分、健康摘要和建议；实时页看当前状态；历史/趋势用于长期观察；AI 页负责自然语言解释。
5. **轻量部署**：Node 仅承担静态服务和 API 代理，APK 内不携带 Flask 后端，后端可独立部署和更换。

## 可能被评委追问的问题

1. APP 为什么不是蓝牙直连？源码没有蓝牙 API、扫描、配对、GATT/串口服务；当前实现依赖 Web/Flask HTTP，直连能力【无法确认】。
2. 2 秒刷新能否称为实时？这是轮询周期，不是硬实时；端到端延迟、后台节流和丢包情况下的实际延迟【无法确认】。
3. 断网后能否继续训练并回传？当前代码只缓存已获取数据，没有设备采集和离线上传队列；完整离线回传【无法确认】。
4. IndexedDB 如何防止重复和冲突？依赖 `recordKey`；相同键会覆盖，跨设备同时间同动作的冲突策略【无法确认】。
5. 训练评分和康复指数如何计算？APP 只展示后端结果；评分真实来源和医学验证【无法从 APP 确认】。
6. AI 是否在手机本地运行？不是；APP 请求 `/api/ai_chat`，后端模型配置和数据发送范围【无法从 APP 确认】。
7. APK 是否真正独立？不是。APK 只包含 WebView 和静态资源，仍需可访问 Flask 后端；局域网 IP、端口和服务部署【无法确认】。
8. 明文 HTTP 是否安全？manifest 和 network security config 明确允许明文流量；生产环境应使用 HTTPS、鉴权和证书校验。
9. APK 2.0 是否已严格对应 2.0 文档？实际 assets 与 `app2.0` 文件一致，但 `APK打包核对报告.md` 内容仍写 1.0，报告应更新或重新核验。
10. Android 是否申请了蓝牙、定位、通知等权限？manifest 只看到 INTERNET；原生蓝牙功能【无法确认】。
11. 页面如何处理恶意后端字段？部分文本通过 `textContent`，但训练/健康列表使用模板 `innerHTML`；服务端数据可信边界和 XSS 防护【无法确认】。
12. 数据是否加密、是否有用户账户隔离？APP 工程没有登录、权限、加密存储和多用户字段，均【无法确认】。

## 无法从代码确认的信息

- MCU/主控型号、传感器型号、底层采样链、滤波和动作识别算法。
- 上臂端、手腕端到 Flask 的真实通信接口、MQTT/HTTP 选择、主题、帧头、长度、CRC、序号和重传机制。
- 后端 `realtime`、`health`、`analysis` 字段的最终生成算法和临床验证结果。
- 实际运行时 Flask 地址、局域网 IP、服务器部署方式、反向代理和公网访问策略。
- APP 与设备是否存在未提交、未纳入本工程的原生桥接模块。
- 2 秒轮询在不同 Android 版本、浏览器和后台状态下的真实功耗与延迟。
- IndexedDB 的实际容量、清理策略、跨浏览器迁移和崩溃恢复结果。
- AI 模型名称、API Key 管理、数据隐私、模型准确率和医疗责任边界。
- APK release 签名、正式发布渠道和商店合规状态；当前构建元数据显示为 debug variant。
- `APK打包核对报告.md` 所描述的旧版 1.0 信息是否已被完全更新。

