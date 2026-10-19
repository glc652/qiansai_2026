const http = require("http");
const fs = require("fs");
const path = require("path");

const ROOT = __dirname;
const PORT = Number(process.env.PORT || 5175);
const API_BASE = (process.env.REHAB_API_BASE || "http://127.0.0.1:5000").replace(/\/$/, "");
const PROXY_TIMEOUT_MS = Number(process.env.PROXY_TIMEOUT_MS || 45000);

const MIME = {
  ".html": "text/html; charset=utf-8",
  ".css": "text/css; charset=utf-8",
  ".js": "application/javascript; charset=utf-8",
  ".json": "application/json; charset=utf-8",
  ".svg": "image/svg+xml"
};

function send(res, status, body, headers = {}) {
  res.writeHead(status, { "Cache-Control": "no-store", ...headers });
  res.end(body);
}

function proxyApi(req, res) {
  const target = new URL(req.url.replace(/^\/proxy/, ""), API_BASE);
  const headers = {
    Accept: req.headers.accept || "application/json,text/plain,*/*"
  };
  if (req.headers["content-type"]) headers["Content-Type"] = req.headers["content-type"];
  if (req.headers["content-length"]) headers["Content-Length"] = req.headers["content-length"];

  const proxyReq = http.request(
    target,
    {
      method: req.method,
      headers,
      timeout: PROXY_TIMEOUT_MS
    },
    (proxyRes) => {
      const chunks = [];
      proxyRes.on("data", (chunk) => chunks.push(chunk));
      proxyRes.on("end", () => {
        send(res, proxyRes.statusCode || 200, Buffer.concat(chunks), {
          "Content-Type": proxyRes.headers["content-type"] || "application/json; charset=utf-8"
        });
      });
    }
  );

  proxyReq.on("timeout", () => proxyReq.destroy(new Error("backend timeout")));
  proxyReq.on("error", (err) => {
    send(res, 502, JSON.stringify({ ok: false, error: `无法连接后端：${err.message}` }), {
      "Content-Type": "application/json; charset=utf-8"
    });
  });
  req.pipe(proxyReq);
}

function serveStatic(req, res) {
  const url = new URL(req.url, "http://localhost");
  const safePath = url.pathname === "/" ? "/index.html" : decodeURIComponent(url.pathname);
  const filePath = path.normalize(path.join(ROOT, safePath));

  if (!filePath.startsWith(ROOT)) {
    send(res, 403, "Forbidden", { "Content-Type": "text/plain; charset=utf-8" });
    return;
  }

  fs.readFile(filePath, (err, data) => {
    if (err) {
      send(res, 404, "Not found", { "Content-Type": "text/plain; charset=utf-8" });
      return;
    }
    send(res, 200, data, { "Content-Type": MIME[path.extname(filePath)] || "application/octet-stream" });
  });
}

http.createServer((req, res) => {
  if (req.url.startsWith("/proxy/api/")) {
    proxyApi(req, res);
    return;
  }
  serveStatic(req, res);
}).listen(PORT, "0.0.0.0", () => {
  console.log(`Rehab family app: http://localhost:${PORT}`);
  console.log(`Backend API base: ${API_BASE}`);
});
