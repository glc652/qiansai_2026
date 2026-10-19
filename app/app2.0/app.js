const state = {
  history: [],
  health: [],
  analysis: null,
  latest: null,
  modelMessages: [],
  loading: false,
  online: false,
  lastSyncAt: null,
  apiBase: localStorage.getItem("rehabApiBase") || ""
};

const DB_NAME = "rehab-band-app2";
const DB_VERSION = 1;
const DB_STORE = "snapshots";
const REALTIME_INTERVAL_MS = 2000;
const HISTORY_SYNC_INTERVAL_MS = 15000;
let realtimeTimer = null;
let historySyncTimer = null;

function openLocalDb() {
  return new Promise((resolve, reject) => {
    const request = indexedDB.open(DB_NAME, DB_VERSION);
    request.onupgradeneeded = () => {
      const db = request.result;
      if (!db.objectStoreNames.contains(DB_STORE)) db.createObjectStore(DB_STORE, { keyPath: "key" });
    };
    request.onsuccess = () => resolve(request.result);
    request.onerror = () => reject(request.error || new Error("无法打开本地数据库"));
  });
}

async function readLocal(key, fallback = null) {
  try {
    const db = await openLocalDb();
    return await new Promise((resolve, reject) => {
      const tx = db.transaction(DB_STORE, "readonly");
      const request = tx.objectStore(DB_STORE).get(key);
      request.onsuccess = () => resolve(request.result?.value ?? fallback);
      request.onerror = () => reject(request.error);
      tx.oncomplete = () => db.close();
    });
  } catch (err) {
    console.warn("读取本地数据库失败", err);
    return fallback;
  }
}

async function writeLocal(key, value) {
  try {
    const db = await openLocalDb();
    await new Promise((resolve, reject) => {
      const tx = db.transaction(DB_STORE, "readwrite");
      tx.objectStore(DB_STORE).put({ key, value, savedAt: new Date().toISOString() });
      tx.oncomplete = resolve;
      tx.onerror = () => reject(tx.error);
    });
    db.close();
  } catch (err) {
    console.warn("保存本地数据库失败", err);
  }
}

function recordKey(item, kind) {
  if (item?.id !== undefined && item?.id !== null) return `${kind}:id:${item.id}`;
  if (kind === "health" && item?.seq !== undefined) return `${kind}:seq:${item.seq}:${item.updated_at || item.time || ""}`;
  return `${kind}:${item?.date || item?.updated_at || item?.time || item?.created_at || ""}:${item?.action || ""}`;
}

function mergeRecords(localItems, remoteItems, kind) {
  const merged = new Map();
  (Array.isArray(localItems) ? localItems : []).forEach((item) => merged.set(recordKey(item, kind), item));
  (Array.isArray(remoteItems) ? remoteItems : []).forEach((item) => merged.set(recordKey(item, kind), item));
  return [...merged.values()].sort((a, b) => parseTime(b) - parseTime(a));
}

async function loadLocalData() {
  const cache = await readLocal("app-data", {});
  state.history = Array.isArray(cache.history) ? cache.history : [];
  state.health = Array.isArray(cache.health) ? cache.health : [];
  state.analysis = cache.analysis || {};
  state.latest = cache.latest || await readLocal("latest", null);
  state.lastSyncAt = cache.lastSyncAt || null;
  renderAll();
  renderRealtime();
  const count = state.history.length + state.health.length;
  setText("connectionState", count
    ? `已从本地读取 ${state.history.length} 条训练记录、${state.health.length} 条健康记录，正在连接 Flask…`
    : "本地暂无数据，正在连接 Flask…");
}

async function saveLocalData() {
  await writeLocal("app-data", {
    history: state.history,
    health: state.health,
    analysis: state.analysis,
    latest: state.latest,
    lastSyncAt: state.lastSyncAt
  });
}

const $ = (id) => document.getElementById(id);

function apiPath(path) {
  const base = state.apiBase.trim().replace(/\/$/, "");
  return base ? `${base}${path}` : `/proxy${path}`;
}

async function fetchJson(path, options = {}) {
  const resp = await fetch(apiPath(path), { cache: "no-store", ...options });
  const text = await resp.text();
  let data = {};
  try {
    data = text ? JSON.parse(text) : {};
  } catch {
    data = { error: text };
  }
  if (!resp.ok) throw new Error(data.error || `${resp.status} ${resp.statusText}`);
  return data;
}

function value(v, unit = "") {
  if (v === undefined || v === null || v === "") return "--";
  return `${v}${unit}`;
}

function numberOf(v) {
  const n = Number(v);
  return Number.isFinite(n) ? n : 0;
}

function parseTime(item) {
  const raw = item.date || item.time || item.created_at || item.timestamp || "";
  const t = Date.parse(String(raw).replace(/-/g, "/"));
  return Number.isFinite(t) ? t : 0;
}

function setText(id, text) {
  const el = $(id);
  if (el) el.textContent = text;
}

function setRefreshLoading(loading) {
  state.loading = loading;
  const btn = $("refreshBtn");
  if (!btn) return;
  btn.classList.toggle("loading", loading);
  btn.disabled = loading;
  btn.setAttribute("aria-label", loading ? "正在刷新数据" : "刷新数据");
  btn.title = loading ? "正在刷新数据" : "刷新数据";
}

function escapeHtml(text) {
  return String(text)
    .replace(/&/g, "&amp;")
    .replace(/</g, "&lt;")
    .replace(/>/g, "&gt;")
    .replace(/"/g, "&quot;");
}

async function loadData(options = {}) {
  if (state.loading) return;
  const quiet = Boolean(options.quiet);
  state.loading = true;
  if (!quiet) setRefreshLoading(true);
  setText("connectionState", "正在同步 Flask 数据...");
  try {
    const [historyResp, healthResp, analysisResp] = await Promise.all([
      fetchJson("/api/history"),
      fetchJson("/api/health_history"),
      fetchJson("/api/analysis")
    ]);
    const oldTrainingCount = state.history.length;
    const oldHealthCount = state.health.length;
    state.history = mergeRecords(state.history, historyResp.records, "training");
    state.health = mergeRecords(state.health, healthResp.records, "health");
    state.analysis = analysisResp || {};
    state.online = true;
    state.lastSyncAt = new Date().toISOString();
    await saveLocalData();
    renderAll();
    const addedTraining = Math.max(0, state.history.length - oldTrainingCount);
    const addedHealth = Math.max(0, state.health.length - oldHealthCount);
    setText("connectionState", `在线 · 本地共 ${state.history.length} 条训练、${state.health.length} 条健康记录，本次新增 ${addedTraining + addedHealth} 条。`);
  } catch (err) {
    state.online = false;
    const cached = state.history.length + state.health.length;
    setText("connectionState", cached
      ? `离线模式 · 正在显示本地数据（${cached} 条）。连接失败：${err.message}`
      : `连接失败且本地暂无记录：${err.message}`);
    if (!cached) renderError(err);
  } finally {
    state.loading = false;
    if (!quiet) setRefreshLoading(false);
  }
}

function renderAll() {
  renderHome();
  renderFilters();
  renderHistory();
  renderHealth();
  renderTrend();
  renderModelMessages();
  setText("lastUpdate", new Date().toLocaleTimeString("zh-CN", { hour12: false }));
}

function renderRealtime() {
  const payload = state.latest || {};
  const rt = payload.realtime || {};
  const health = payload.health || {};
  const system = payload.system_state || {};
  const devices = payload.devices || {};
  const emgMode = Number(rt.emg_mode ?? -1);
  const emgClass = emgMode === 2 ? "strong" : emgMode === 1 ? "weak" : emgMode === 0 ? "relax" : "offline";

  setText("liveState", state.online ? (system.is_training ? "训练进行中" : "设备在线") : "离线 / 等待连接");
  setText("liveAction", rt.action || "等待设备数据");
  setText("liveAngleName", rt.angle_name || "动作角度");
  setText("liveAngle", value(rt.display_angle ?? rt.elbow_angle, "°"));
  setText("liveCompletion", value(rt.completion, "%"));
  setText("liveReps", `${value(rt.reps)} / ${value(rt.target_reps)}`);
  setText("liveScore", value(rt.score));
  setText("liveEmg", rt.emg_label || "--");
  setText("liveEmgStrength", value(rt.emg_strength));
  setText("liveHeart", health.valid ? value(health.heart_rate, " bpm") : "--");
  setText("liveSpo2", health.valid ? value(health.spo2, "%") : "--");
  setText("liveUpper", devices.upper?.bluetooth_connected ? "已连接" : "未连接");
  setText("liveWrist", devices.wrist?.bluetooth_connected ? "已连接" : "未连接");
  setText("liveTime", payload.time || (state.lastSyncAt ? new Date(state.lastSyncAt).toLocaleString("zh-CN") : "--"));

  const emg = $("liveEmg");
  if (emg) emg.className = `live-emg ${emgClass}`;
  const fill = $("liveCompletionFill");
  if (fill) fill.style.width = `${Math.max(0, Math.min(100, numberOf(rt.completion)))}%`;
}

async function pollRealtime() {
  try {
    state.latest = await fetchJson("/api/latest");
    state.online = true;
    renderRealtime();
    await writeLocal("latest", state.latest);
  } catch (err) {
    state.online = false;
    renderRealtime();
  }
}

function startBackgroundSync() {
  if (realtimeTimer) clearInterval(realtimeTimer);
  if (historySyncTimer) clearInterval(historySyncTimer);
  pollRealtime();
  realtimeTimer = setInterval(pollRealtime, REALTIME_INTERVAL_MS);
  historySyncTimer = setInterval(() => loadData({ quiet: true }), HISTORY_SYNC_INTERVAL_MS);
}

function renderError(err) {
  const html = `<div class="error">无法读取数据：${err.message}<br>请确认 Flask 上位机服务已启动。</div>`;
  $("historyList").innerHTML = html;
  $("healthList").innerHTML = html;
  $("actionAnalysis").innerHTML = html;
}

function latestRecord() {
  return state.history.slice().sort((a, b) => parseTime(b) - parseTime(a))[0] || null;
}

function latestHealth() {
  return state.health.slice().sort((a, b) => parseTime(b) - parseTime(a))[0] || null;
}

function renderHome() {
  const last = latestRecord();
  const health = latestHealth();
  const analysis = state.analysis || {};
  const firstAdvice = (analysis.suggestions || [])[0];
  const adviceItems = firstAdvice?.items || fallbackAdvice(last, analysis);

  setText("recoveryIndex", value(analysis.recovery_index));
  setText("recordCount", value(state.history.length));
  setText("lastScore", last ? value(last.score) : "--");
  setText("lastAction", last ? value(last.action) : "暂无");
  setText("lastReps", last ? value(last.reps, " 次") : "--");
  setText("lastCompletion", last ? value(last.completion, "%") : "--");
  setText("lastAngle", last ? value(last.max_angle || last.best_angle, "°") : "--");
  setText("mainProblem", analysis.main_problem || "暂无问题");
  setText("healthTime", health ? value(health.date || health.time || health.created_at) : "--");
  setText("heartRate", health ? value(health.heart_rate, " bpm") : "--");
  setText("spo2", health ? value(health.spo2, "%") : "--");
  setText("fatigue", health ? value(health.fatigue) : "--");

  $("homeAdvice").innerHTML = adviceItems.length
    ? adviceItems.slice(0, 4).map((item) => `<div class="advice">${item}</div>`).join("")
    : `<div class="empty">暂无建议，完成训练后会生成趋势和建议。</div>`;
}

function fallbackAdvice(last, analysis) {
  if (!last) return ["暂无训练记录，建议先完成一次标准训练后再查看康复趋势。"];
  const items = [];
  if (numberOf(last.score) < 80) items.push("最近一次评分偏低，建议放慢动作速度，优先保证动作完整。");
  if (numberOf(last.completion) < 75) items.push("完成度不足时，可减少单组次数，增加组间休息。");
  if (numberOf(last.amplitude) < 75) items.push("动作幅度偏低时，末端位置保持 2 秒，逐步扩大活动范围。");
  if (analysis.weak_action) items.push(`近期薄弱动作是${analysis.weak_action}，后续训练可适当增加该动作观察。`);
  return items.length ? items : ["近期训练表现稳定，建议保持当前训练节奏。"];
}

function renderFilters() {
  const actions = [...new Set(state.history.map((r) => r.action).filter(Boolean))];
  const current = $("actionFilter").value;
  $("actionFilter").innerHTML = `<option value="">全部动作</option>${actions.map((a) => `<option value="${a}">${a}</option>`).join("")}`;
  $("actionFilter").value = actions.includes(current) ? current : "";
}

function filteredHistory() {
  const action = $("actionFilter").value;
  const mode = $("sortMode").value;
  const items = state.history.filter((item) => !action || item.action === action);
  return items.sort((a, b) => {
    if (mode === "score_desc") return numberOf(b.score) - numberOf(a.score);
    if (mode === "score_asc") return numberOf(a.score) - numberOf(b.score);
    return parseTime(b) - parseTime(a);
  });
}

function renderHistory() {
  const items = filteredHistory();
  $("historyList").innerHTML = items.length
    ? items.map(trainingCard).join("")
    : `<div class="empty">暂无训练记录</div>`;
  document.querySelectorAll("[data-record-id]").forEach((btn) => {
    btn.addEventListener("click", () => showRecordDetail(btn.dataset.recordId));
  });
}

function trainingCard(record) {
  const id = record.id ?? `${record.date}-${record.action}`;
  return `
    <article class="record-card" data-record-id="${id}">
      <div>
        <h3>${value(record.action)}</h3>
        <p>${value(record.date)} · ${value(record.duration)}</p>
        <p>完成 ${value(record.reps, " 次")} / 目标 ${value(record.target_reps, " 次")} · 最大角度 ${value(record.max_angle || record.best_angle, "°")}</p>
      </div>
      <div class="score-pill">${value(record.score)}</div>
    </article>
  `;
}

function renderHealth() {
  setText("healthCount", `${state.health.length} 条`);
  const items = state.health.slice().sort((a, b) => parseTime(b) - parseTime(a)).slice(0, 80);
  $("healthList").innerHTML = items.length
    ? items.map((item) => `
      <article class="record-card">
        <div>
          <h3>${value(item.date || item.time || item.created_at)}</h3>
          <p>血氧 ${value(item.spo2, "%")} · 疲劳度 ${value(item.fatigue)} · 血压 ${value(item.blood_pressure || item.bp || ((item.sbp || item.dbp) ? `${item.sbp || "--"}/${item.dbp || "--"}` : "--"))}</p>
          <p>SDNN ${value(item.sdnn)} · RMSSD ${value(item.rmssd)}</p>
        </div>
        <div class="score-pill">${value(item.heart_rate)}</div>
      </article>
    `).join("")
    : `<div class="empty">暂无健康记录</div>`;
}

function renderTrend() {
  const analysis = state.analysis || {};
  setText("weakAction", analysis.weak_action || "--");
  drawScoreChart(state.history.slice().sort((a, b) => parseTime(a) - parseTime(b)).slice(-12));
  const items = analysis.action_analysis || [];
  $("actionAnalysis").innerHTML = items.length
    ? items.map((item) => `
      <article class="analysis-item">
        <header><strong>${item.action}</strong><span>${item.trend || "--"}</span></header>
        <p>记录 ${value(item.records)} 条 · 平均评分 ${value(item.avg_score)} · 完成度 ${value(item.avg_completion, "%")} · 平均角度 ${value(item.avg_angle, "°")}</p>
        <p>${(item.problems || []).join("、") || "训练表现稳定"}</p>
      </article>
    `).join("")
    : `<div class="empty">暂无趋势分析</div>`;
}

function renderModelMessages() {
  const panel = $("modelMessages");
  if (!panel) return;
  if (!state.modelMessages.length) {
    panel.innerHTML = `<div class="empty">可以选择快捷问题，或输入问题后调用 API 模型生成建议。</div>`;
    return;
  }
  panel.innerHTML = state.modelMessages
    .map((msg) => `<div class="message ${msg.role}">${escapeHtml(msg.content)}</div>`)
    .join("");
}

async function sendModelMessage(promptText) {
  const input = $("modelInput");
  const text = (promptText || input.value || "").trim();
  if (!text) return;

  input.value = "";
  state.modelMessages.push({ role: "user", content: text });
  state.modelMessages.push({ role: "assistant", content: "正在调用 API 模型生成建议..." });
  renderModelMessages();

  try {
    const historyMessages = state.modelMessages
      .filter((msg) => msg.role === "user" || msg.role === "assistant")
      .slice(-8)
      .map((msg) => ({ role: msg.role, content: msg.content }));
    const resp = await fetchJson("/api/ai_chat", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ message: text, messages: historyMessages })
    });
    state.modelMessages[state.modelMessages.length - 1] = {
      role: "assistant",
      content: resp.reply || resp.answer || "后端已响应，但没有返回建议内容。"
    };
  } catch (err) {
    state.modelMessages[state.modelMessages.length - 1] = {
      role: "assistant",
      content: `API 模型调用失败：${err.message}\n请确认后端 /api/ai_chat 可用，并已配置模型 API Key。`
    };
  }
  renderModelMessages();
}

function drawScoreChart(items) {
  const canvas = $("scoreChart");
  const ctx = canvas.getContext("2d");
  const w = canvas.width;
  const h = canvas.height;
  ctx.clearRect(0, 0, w, h);
  ctx.fillStyle = "#fbfeff";
  ctx.fillRect(0, 0, w, h);
  ctx.strokeStyle = "#d9e8ec";
  ctx.lineWidth = 1;
  for (let i = 0; i <= 4; i++) {
    const y = 28 + i * 58;
    ctx.beginPath();
    ctx.moveTo(44, y);
    ctx.lineTo(w - 18, y);
    ctx.stroke();
  }
  ctx.fillStyle = "#66808a";
  ctx.font = "22px system-ui";
  if (!items.length) {
    ctx.fillText("暂无训练评分数据", 230, 155);
    return;
  }
  const points = items.map((item, idx) => {
    const x = 54 + idx * ((w - 94) / Math.max(1, items.length - 1));
    const y = h - 34 - numberOf(item.score) * 2.35;
    return { x, y, score: numberOf(item.score) };
  });
  const gradient = ctx.createLinearGradient(44, 0, w - 18, 0);
  gradient.addColorStop(0, "#0a84ff");
  gradient.addColorStop(1, "#ff375f");
  ctx.strokeStyle = gradient;
  ctx.lineWidth = 4;
  ctx.beginPath();
  points.forEach((p, idx) => idx ? ctx.lineTo(p.x, p.y) : ctx.moveTo(p.x, p.y));
  ctx.stroke();
  points.forEach((p, idx) => {
    ctx.fillStyle = "#ffffff";
    ctx.beginPath();
    ctx.arc(p.x, p.y, 7, 0, Math.PI * 2);
    ctx.fill();
    ctx.strokeStyle = idx % 2 ? "#ff375f" : "#0a84ff";
    ctx.lineWidth = 3;
    ctx.stroke();
    ctx.fillStyle = "#17313b";
    ctx.font = "18px system-ui";
    ctx.fillText(String(p.score), p.x - 11, p.y - 12);
  });
}

function showRecordDetail(id) {
  const record = state.history.find((item) => String(item.id ?? `${item.date}-${item.action}`) === String(id));
  if (!record) return;
  setText("detailTitle", record.action || "训练详情");
  $("detailBody").innerHTML = [
    ["训练时间", record.date],
    ["训练时长", record.duration],
    ["综合评分", record.score],
    ["完成次数", record.reps],
    ["目标次数", record.target_reps],
    ["完成度", value(record.completion, "%")],
    ["幅度评分", value(record.amplitude, "%")],
    ["最大角度", value(record.max_angle || record.best_angle, "°")],
    ["平均角度", value(record.avg_angle, "°")]
  ].map(([k, v]) => `<div class="detail-row"><span>${k}</span><strong>${value(v)}</strong></div>`).join("");
  $("detailDialog").showModal();
}

function bindEvents() {
  document.querySelectorAll(".tab").forEach((btn) => {
    btn.addEventListener("click", () => {
      document.querySelectorAll(".tab").forEach((item) => item.classList.remove("active"));
      document.querySelectorAll(".page").forEach((item) => item.classList.remove("active"));
      btn.classList.add("active");
      const page = $(btn.dataset.page);
      page.classList.add("active");
      setText("pageTitle", page.dataset.title || "家属端");
    });
  });
  $("refreshBtn").addEventListener("click", loadData);
  $("actionFilter").addEventListener("change", renderHistory);
  $("sortMode").addEventListener("change", renderHistory);
  $("closeDialogBtn").addEventListener("click", () => $("detailDialog").close());
  $("sendModelBtn").addEventListener("click", () => sendModelMessage());
  $("modelInput").addEventListener("keydown", (event) => {
    if (event.key === "Enter" && (event.ctrlKey || event.metaKey)) {
      sendModelMessage();
    }
  });
  document.querySelectorAll(".quick-prompt").forEach((btn) => {
    btn.addEventListener("click", () => sendModelMessage(btn.dataset.prompt));
  });
  $("saveSettingsBtn").addEventListener("click", () => {
    state.apiBase = $("apiBaseInput").value.trim();
    localStorage.setItem("rehabApiBase", state.apiBase);
    loadData();
    pollRealtime();
  });
  document.addEventListener("visibilitychange", () => {
    if (!document.hidden) {
      pollRealtime();
      loadData({ quiet: true });
    }
  });
  bindTapEffects();
}

function bindTapEffects() {
  document.addEventListener("pointerdown", (event) => {
    const target = event.target.closest("button, .record-card, .metric, .track-card");
    if (!target || target.disabled) return;
    const rect = target.getBoundingClientRect();
    const ripple = document.createElement("span");
    ripple.className = "tap-ripple";
    ripple.style.left = `${event.clientX - rect.left}px`;
    ripple.style.top = `${event.clientY - rect.top}px`;
    target.appendChild(ripple);
    ripple.addEventListener("animationend", () => ripple.remove(), { once: true });
  });
}

$("apiBaseInput").value = state.apiBase;
bindEvents();

async function initializeApp() {
  await loadLocalData();
  await loadData();
  startBackgroundSync();
}

initializeApp();
