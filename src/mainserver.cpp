#include "mainserver.h"
#include <WebServer.h>
#include <WiFi.h>

bool led1_state = false;
bool led2_state = false;
bool isAPMode = true;

WebServer server(80);

unsigned long connect_start_ms = 0;
bool connecting = false;

String mainPage() {
    float hum = -1, temp = -1;
    SensorData recv;

  if (xQueuePeek(qSensorData, &recv, 0) == pdTRUE)
  {
    hum = recv.humidity;
    temp = recv.temperature;
  }

    String led1 = led1_state ? "ON" : "OFF";
    String led2 = led2_state ? "ON" : "OFF";

    return R"rawliteral(
<!DOCTYPE html>
<html lang="vi">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 Dashboard</title>


  <style>
    body {
      margin: 0;
      background: #111827;
      color: #e5e7eb;
      font-family: "Segoe UI", sans-serif;
      display: flex;
      flex-direction: column;
      align-items: center;
      padding: 20px;
    }

    h1 {
      color: #38bdf8;
      margin-bottom: 20px;
      text-align: center;
    }

    .value-box {
      display: flex;
      flex-wrap: wrap;
      gap: 25px;
      margin-bottom: 30px;
      justify-content: center;
    }

    .box {
      background: #1f2937;
      padding: 20px 30px;
      border-radius: 15px;
      text-align: center;
      box-shadow: 0 4px 15px rgba(0,0,0,0.3);
      min-width: 150px;
    }

    .box span {
      font-size: 2rem;
      color: #10b981;
      font-weight: bold;
    }

    /* LED buttons */
    .led-box {
      display: flex;
      gap: 20px;
      margin-bottom: 15px;
      flex-wrap: wrap;
      justify-content: center;
    }

    .led-btn {
      background: #38bdf8;
      border: none;
      padding: 12px 22px;
      border-radius: 12px;
      cursor: pointer;
      font-weight: bold;
      color: #000;
      transition: 0.25s;
    }

    .led-btn:hover {
      transform: scale(1.07);
      background: #0ea5e9;
    }

    /* SETTINGS BUTTON */
    .settings-btn {
      background: #facc15;
      color: #000;
      font-weight: bold;
      padding: 12px 30px;
      border-radius: 12px;
      border: none;
      cursor: pointer;
      transition: 0.25s;
      margin-bottom: 30px;
    }

    .settings-btn:hover {
      transform: scale(1.07);
      background: #eab308;
    }

    /* 2 chart bên cạnh nhau */
    .charts-row {
      display: flex;
      gap: 20px;
      width: 100%;
      max-width: 1200px;
      flex-wrap: wrap;
      justify-content: center;
    }

    .chart-container {
      background: #1f2937;
      padding: 20px;
      border-radius: 15px;
      box-shadow: 0 4px 18px rgba(0,0,0,0.4);
      flex: 1 1 45%;
      min-width: 320px;
    }

    canvas {
      width: 100% !important;
      height: 330px !important;
    }

  </style>
</head>

<body>

  <h1>📊 ESP32 Sensor Dashboard</h1>

  <!-- VALUE BOX -->
  <div class="value-box">
    <div class="box">
      🌡️ Nhiệt độ <br>
      <span id="temp">25.0</span> °C
    </div>

    <div class="box">
      💧 Độ ẩm <br>
      <span id="hum">60</span> %
    </div>

    <div class="box">
      💡 LED1 <br>
      <span id="l1" style="color:#facc15">OFF</span>
    </div>

    <div class="box">
      💡 LED2 <br>
      <span id="l2" style="color:#facc15">OFF</span>
    </div>
  </div>

  <!-- LED BUTTONS -->
  <div class="led-box">
    <button class="led-btn" onclick="toggleLED(1)">🔘 Toggle LED1</button>
    <button class="led-btn" onclick="toggleLED(2)">🔘 Toggle LED2</button>
  </div>

  <!-- SETTINGS BUTTON -->
  <button class="settings-btn" onclick="window.location='/settings'">⚙️ Cài đặt</button>

<!-- 2 CHARTS CẠNH NHAU -->
<div class="charts-row">

  <div class="chart-container">
    <h2 style="color:#f87171; margin-bottom:10px;">🌡️ Biểu đồ Nhiệt độ</h2>
    <svg id="tempSVG" width="100%" height="300"></svg>
  </div>

  <div class="chart-container">
    <h2 style="color:#60a5fa; margin-bottom:10px;">💧 Biểu đồ Độ ẩm</h2>
    <svg id="humSVG" width="100%" height="300"></svg>
  </div>

</div>



<script>

   function toggleLED(id) {
    fetch('/toggle?led=' + id)
      .then(res => res.json())
      .then(json => {
        
        // Cập nhật trạng thái LED1
        document.getElementById('l1').innerText = json.led1;
        document.getElementById('l1').style.color =
          (json.led1 === "ON") ? "#10b981" : "#facc15";

        // Cập nhật trạng thái LED2
        document.getElementById('l2').innerText = json.led2;
        document.getElementById('l2').style.color =
          (json.led2 === "ON") ? "#10b981" : "#facc15";
      })
      .catch(err => console.error("Toggle LED error:", err));
  }
const MAX_POINTS = 20;

let tempData = [];
let humData  = [];

/* ===========================================
   HÀM VẼ GRID + LABEL SỐ TRỤC Y
=========================================== */
function drawGrid(svg, minVal, maxVal) {
  const W = svg.clientWidth;
  const H = svg.clientHeight;
  svg.innerHTML = "";

  for (let i = 0; i <= 5; i++) {
    const y = (H / 5) * i;

    // Grid line
    const line = document.createElementNS("http://www.w3.org/2000/svg", "line");
    line.setAttribute("x1", 0);
    line.setAttribute("y1", y);
    line.setAttribute("x2", W);
    line.setAttribute("y2", y);
    line.setAttribute("stroke", "#374151");
    svg.appendChild(line);

    // Y-axis label
    const val = maxVal - (i * (maxVal - minVal) / 5);
    const txt = document.createElementNS("http://www.w3.org/2000/svg", "text");
    txt.setAttribute("x", 5);
    txt.setAttribute("y", y - 2);
    txt.setAttribute("fill", "#9ca3af");
    txt.setAttribute("font-size", "12");
    txt.textContent = val.toFixed(0);
    svg.appendChild(txt);
  }
}

/* ===========================================
   HÀM TÍNH PATH SMOOTH CURVE (Cubic Bézier)
=========================================== */
function smoothPath(points) {
  if (points.length < 2) return "";

  let path = `M ${points[0].x} ${points[0].y}`;

  for (let i = 1; i < points.length; i++) {
    const p0 = points[i - 1];
    const p1 = points[i];
    const cpx = (p0.x + p1.x) / 2;
    path += ` C ${cpx} ${p0.y}, ${cpx} ${p1.y}, ${p1.x} ${p1.y}`;
  }

  return path;
}

/* ===========================================
   HÀM VẼ CHART SVG
=========================================== */
function drawChart(svg, data, color, minVal, maxVal) {
  drawGrid(svg, minVal, maxVal);

  const W = svg.clientWidth;
  const H = svg.clientHeight;

  let pts = [];

  for (let i = 0; i < data.length; i++) {
    const x = (i / (MAX_POINTS - 1)) * (W - 20) + 10;
    const val = data[i];

    const y = H - ((val - minVal) / (maxVal - minVal)) * (H - 20) - 10;

    pts.push({ x, y });
  }

  if (pts.length < 2) return;

  // Create smooth curve
  const d = smoothPath(pts);

  const p = document.createElementNS("http://www.w3.org/2000/svg", "path");
  p.setAttribute("d", d);
  p.setAttribute("fill", "none");
  p.setAttribute("stroke", color);
  p.setAttribute("stroke-width", "3");
  p.setAttribute("stroke-linecap", "round");

  svg.appendChild(p);
}

/* ===========================================
   UPDATE DỮ LIỆU TỪ ESP32
=========================================== */
setInterval(() => {
  fetch("/sensors")
    .then(res => res.json())
    .then(s => {
      document.getElementById("temp").innerText = s.temp;
      document.getElementById("hum").innerText  = s.hum;

      tempData.push(parseFloat(s.temp));
      humData.push(parseFloat(s.hum));

      if (tempData.length > MAX_POINTS) tempData.shift();
      if (humData.length > MAX_POINTS) humData.shift();

      drawChart(document.getElementById("tempSVG"), tempData, "#f87171", 10, 50);
      drawChart(document.getElementById("humSVG"),  humData, "#60a5fa", 20, 90);
    });
}, 2000);
</script>

</body>
</html>
)rawliteral";
}

String settingsPage()
{
  return R"rawliteral(
<!DOCTYPE html>
<html lang="vi">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Cài đặt Wi-Fi</title>

  <style>
    body {
      margin: 0;
      padding: 0;
      font-family: "Segoe UI", sans-serif;
      background: #0f172a;
      color: #e2e8f0;
      display: flex;
      flex-direction: column;
      align-items: center;
      padding-top: 40px;
    }

    h1 {
      color: #38bdf8;
      font-size: 2rem;
      margin-bottom: 25px;
    }

    .card {
      background: #1e293b;
      padding: 30px 35px;
      border-radius: 18px;
      width: 90%;
      max-width: 420px;
      box-shadow: 0 4px 25px rgba(0,0,0,0.35);
    }

    input[type=text],
    input[type=password] {
      width: 100%;
      background: #0f172a;
      color: #e2e8f0;
      padding: 12px;
      margin-bottom: 18px;
      border-radius: 12px;
      border: 1px solid #334155;
      outline: none;
      font-size: 1rem;
      transition: 0.25s;
    }

    input:focus {
      border-color: #38bdf8;
      box-shadow: 0 0 0 3px rgba(56,189,248,0.35);
    }

    .btn {
      width: 100%;
      padding: 12px;
      border: none;
      border-radius: 12px;
      cursor: pointer;
      font-size: 1rem;
      font-weight: bold;
      transition: 0.25s;
      margin-bottom: 12px;
    }

    .btn-connect {
      background: #38bdf8;
      color: #0f172a;
    }

    .btn-connect:hover {
      background: #0ea5e9;
      transform: scale(1.04);
    }

    .btn-back {
      background: #334155;
      color: #e2e8f0;
    }

    .btn-back:hover {
      background: #475569;
      transform: scale(1.04);
    }

    #msg {
      margin-top: 12px;
      text-align: center;
      font-size: 1.1rem;
      font-weight: 500;
    }
  </style>
</head>

<body>

  <h1>⚙️ Cấu hình Wi-Fi (Preview)</h1>

  <div class="card">
    <form id="wifiForm">
      <input type="text" id="ssid" name="ssid" placeholder="Tên Wi-Fi (SSID)" required>
      <input type="password" id="pass" name="password" placeholder="Mật khẩu (không bắt buộc)">

      <button type="submit" class="btn btn-connect">Kết nối</button>
      <button type="button" class="btn btn-back" onclick="window.location='/'">⬅ Quay lại</button>
    </form>

    <div id="msg"></div>
  </div>

  <script>
document.getElementById('wifiForm').onsubmit = function(e){
        e.preventDefault();
        let ssid = document.getElementById('ssid').value;
        let pass = document.getElementById('pass').value;
        fetch('/connect?ssid='+encodeURIComponent(ssid)+'&pass='+encodeURIComponent(pass))
          .then(r=>r.text())
          .then(msg=>{
            document.getElementById('msg').innerText = msg;
          });
      };
    </script>

</body>
</html>

  )rawliteral";
}

// ========== Handlers ==========
void handleRoot() { server.send(200, "text/html", mainPage()); }

void handleToggle() {
    int led = server.arg("led").toInt();
    ToggleData data;
    xQueuePeek(qToggleState, &data, 0);

    if (led == 1) {
        data.toggleStateLed1 = !data.toggleStateLed1;
    } else if (led == 2) {
        data.toggleStateLed2 = !data.toggleStateLed2;
    }
    xQueueOverwrite(qToggleState, &data);

    server.send(200, "application/json",
                "{\"led1\":\"" + String(data.toggleStateLed1 ? "ON" : "OFF") +
                    "\",\"led2\":\"" + String(data.toggleStateLed2 ? "ON" : "OFF") + "\"}");
}

void handleSensors() {
    SensorData data;
    float t = 0, h = 0;

  // Lấy dữ liệu cảm biến mới nhất từ queue (nếu có)
  if (xSemaphoreTake(semSensorData, portMAX_DELAY) == pdTRUE)
  {
    if (xQueuePeek(qSensorData, &data, 0) == pdTRUE)
    {
      xSemaphoreGive(semSensorData);
      t = data.temperature;
      h = data.humidity;
    }
    // Tạo JSON trả về
    String json = "{\"temp\":" + String(t) + ",\"hum\":" + String(h) + "}";
    server.send(200, "application/json", json);
}
}

void handleSettings() { server.send(200, "text/html", settingsPage()); }

void handleConnect() {
    wifi_ssid = server.arg("ssid");
    wifi_password = server.arg("pass");
    server.send(200, "text/plain", "Connecting....");
    isAPMode = false;
    connecting = true;
    connect_start_ms = millis();
    connectToWiFi();
}

// ========== WiFi ==========
void setupServer() {
    server.on("/", HTTP_GET, handleRoot);
    server.on("/toggle", HTTP_GET, handleToggle);
    server.on("/sensors", HTTP_GET, handleSensors);
    server.on("/settings", HTTP_GET, handleSettings);
    server.on("/connect", HTTP_GET, handleConnect);
    server.begin();
}

void startAP() {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid.c_str(), password.c_str());
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
    isAPMode = true;
    connecting = false;
}

void connectToWiFi() {
    WiFi.mode(WIFI_STA);
    if (wifi_password.isEmpty()) {
        WiFi.begin(wifi_ssid.c_str());
    } else {
        WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());
    }
    Serial.print("Connecting to: ");
    Serial.print(wifi_ssid.c_str());

    Serial.print(" Password: ");
    Serial.print(wifi_password.c_str());
}

// ========== Main task ==========
void main_server_task(void *pvParameters) {
    pinMode(BOOT_PIN, INPUT_PULLUP);

    startAP();
    setupServer();

    while (1) {
        server.handleClient();

        // BOOT Button to switch to AP Mode
        if (digitalRead(BOOT_PIN) == LOW) {
            vTaskDelay(100);
            if (digitalRead(BOOT_PIN) == LOW) {
                if (!isAPMode) {
                    startAP();
                    setupServer();
                }
            }
        }

        // STA Mode
        if (connecting) {
            if (WiFi.status() == WL_CONNECTED) {
                Serial.print("STA IP address: ");
                Serial.println(WiFi.localIP());
                isWifiConnected = true; // Internet access

                xSemaphoreGive(xBinarySemaphoreInternet);

                isAPMode = false;
                connecting = false;
            } else if (millis() - connect_start_ms > 10000) { // timeout 10s
                Serial.println("WiFi connect failed! Back to AP.");
                startAP();
                setupServer();
                connecting = false;
                isWifiConnected = false;
            }
        }

        vTaskDelay(20); // avoid watchdog reset
    }
}