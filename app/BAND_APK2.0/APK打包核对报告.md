# BAND_APK1.0 APK 打包核对报告

## 1. 目标

将 `E:\qiansai\app\app1.0` 的家属端移动网页 App 封装为 Android APK，供手机下载并安装。

## 2. 工具安装位置

工具安装 / 解压目录：

```text
D:\Android_Build_Tools
```

已使用的工具：

- JDK 17：`D:\Android_Build_Tools\downloads\jdk17\jdk-17.0.19+10`
- Gradle 8.7：`D:\Android_Build_Tools\downloads\gradle-8.7-bin\gradle-8.7`
- Android SDK：`D:\Android_Build_Tools\android-sdk`
- Android SDK Platform：`android-35`
- Android Build Tools：`35.0.0`

## 3. Android 工程位置

```text
E:\qiansai\app\BAND_APK1.0
```

工程类型：

- 原生 Android WebView 壳工程。
- 内置 `app1.0` 的网页资源到 `assets/www`。
- App 名称：`BAND_APK1.0`
- 包名：`com.qiansai.bandapk`
- 版本：`1.0`
- minSdk：23
- targetSdk：35

## 4. 内置网页资源

APK 内置资源来自：

```text
E:\qiansai\app\app1.0
```

已打包到：

```text
E:\qiansai\app\BAND_APK1.0\app\src\main\assets\www
```

包含：

- `index.html`
- `styles.css`
- `app.js`
- `manifest.json`

## 5. 后端连接方式

APK 内置网页在 WebView 的 `file://` 环境中运行，因此已将 APK 版本默认 API 地址设置为：

```text
http://10.146.93.211:5000
```

如果电脑局域网 IP 变化，可以在 App 的“设置”页修改 API 地址。

手机使用时需要满足：

- 手机和电脑在同一个 WiFi。
- 电脑端 Flask 后端正在运行。
- Flask 监听 `0.0.0.0:5000`。
- Windows 防火墙允许手机访问 5000 端口。

## 6. APK 输出位置

正式给手机安装的 APK 文件：

```text
E:\qiansai\app\BAND_APK1.0\BAND_APK1.0.apk
```

为方便手机下载，已复制一份到 app1.0 服务目录：

```text
E:\qiansai\app\app1.0\BAND_APK1.0.apk
```

手机浏览器下载地址：

```text
http://10.146.93.211:5174/BAND_APK1.0.apk
```

## 7. 验证结果

已完成验证：

- Gradle 构建成功：`BUILD SUCCESSFUL`
- APK 元信息检查通过：
  - application-label：`BAND_APK1.0`
  - package：`com.qiansai.bandapk`
  - versionName：`1.0`
  - minSdk：`23`
  - targetSdk：`35`
- APK 内置资源检查通过：
  - `assets/www/index.html`
  - `assets/www/styles.css`
  - `assets/www/app.js`
  - `assets/www/manifest.json`
- 下载地址测试通过：
  - `http://127.0.0.1:5174/BAND_APK1.0.apk`
  - HTTP 状态：`200`
  - 文件大小：`26407` 字节

## 8. 手机安装步骤

1. 保持电脑端 app1.0 服务运行。
2. 保持手机和电脑连接同一个 WiFi。
3. 手机浏览器打开：

```text
http://10.146.93.211:5174/BAND_APK1.0.apk
```

4. 下载 APK。
5. 打开 APK 文件安装。
6. 如果手机提示“禁止安装未知来源应用”，在系统设置中允许当前浏览器安装未知来源应用。
7. 安装后桌面应用名称为：

```text
BAND_APK1.0
```

## 9. 注意事项

- 当前 APK 是 debug 签名版本，适合比赛演示和本机安装测试。
- 如果要正式发布，需要改为 release 签名 APK。
- APK 不包含 Flask 后端，后端仍需要在电脑或服务器上运行。
- 如果手机打开 App 后没有数据，优先检查 App 设置页的 API 地址是否能访问电脑 Flask 服务。
