static const char PROGMEM INDEX_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32-CAM Robot</title>
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0">

  <style>
    body {
      font-family: Arial, sans-serif;
      text-align: center;
      margin: 0;
      padding: 10px;
      user-select: none;
      touch-action: none;
      background: #080505ff;
    }

    h1 {
      margin: 10px 0;
      color: #ea903cff;
    }

    img {
      max-width: 100%;
      height: auto;
      margin-bottom: 10px;
    }

    canvas {
      display: block;
      margin: 0 auto 20px auto;
      max-width: 100%;
      height: auto;
    }

    .slider-container {
      width: 90%;
      max-width: 500px;
      margin: 15px auto;
      text-align: left;
    }

    .slider-container label {
      font-weight: bold;
      display: block;
      margin-bottom: 5px;
      color: #8bdcf3ff;
    }

    .slider-container input[type=range] {
      width: 100%;
      height: 40px;
    }

    .slider-value {
      text-align: right;
      font-size: 14px;
      color: #9b8bf3ff;
    }
  </style>
</head>

<body>
  <h1>ESP32-CAM Robot</h1>
  <img src="" id="photo">
  <div id="info" style="font-family: monospace; color: #666;"></div>

  <!-- Joystick canvas -->
  <canvas id="joystick" width="500" height="500"></canvas>

  <!-- Sliders -->
  <div class="slider-container">
    <label for="sliderA">A</label>
    <input id="sliderA" type="range" min="-127" max="127" value="0">
    <div class="slider-value" id="valueA">0</div>
  </div>

  <div class="slider-container">
    <label for="sliderB">B</label>
    <input id="sliderB" type="range" min="-127" max="127" value="0">
    <div class="slider-value" id="valueB">0</div>
  </div>

  <div class="slider-container">
    <label for="sliderC">C</label>
    <input id="sliderC" type="range" min="-127" max="127" value="0">
    <div class="slider-value" id="valueC">0</div>
  </div>

<script>
/* ===================== CONFIGURATION ===================== */
const LIMIT_RADIUS = 200;     // Outer joystick radius
const DEADZONE_RADIUS = 40;   // Center deadzone radius
const STICK_RADIUS = 25;      // Blue stick radius
const MAX_VALUE = 127;
const UPDATE_INTERVAL = 50;   // 20 Hz update rate
/* ========================================================= */

const canvas = document.getElementById("joystick");
const ctx = canvas.getContext("2d");

const center = {
  x: canvas.width / 2,
  y: canvas.height / 2
};

let stick = { x: 0, y: 0 };
let dragging = false;

// Slider values
let sliderA = 0;
let sliderB = 0;
let sliderC = 0;

// Throttling
let lastSendTime = 0;

/* ===================== DRAWING ===================== */
function drawJoystick() {
  ctx.clearRect(0, 0, canvas.width, canvas.height);

  // Outer limit circle
  ctx.beginPath();
  ctx.arc(center.x, center.y, LIMIT_RADIUS, 0, Math.PI * 2);
  ctx.strokeStyle = "white";
  ctx.lineWidth = 3;
  ctx.stroke();

  // Deadzone circle
  ctx.beginPath();
  ctx.arc(center.x, center.y, DEADZONE_RADIUS, 0, Math.PI * 2);
  ctx.strokeStyle = "white";
  ctx.lineWidth = 1;
  ctx.stroke();

  // Joystick knob
  ctx.beginPath();
  ctx.arc(center.x + stick.x, center.y + stick.y, STICK_RADIUS, 0, Math.PI * 2);
  ctx.fillStyle = "#FFA500";
  ctx.fill();
}

/* ===================== JOYSTICK LOGIC ===================== */
function updateStick(clientX, clientY) {
  const rect = canvas.getBoundingClientRect();
  let x = clientX - rect.left - center.x;
  let y = clientY - rect.top - center.y;

  const distance = Math.hypot(x, y);

  // Limit movement to outer circle
  if (distance > LIMIT_RADIUS) {
    x = x * LIMIT_RADIUS / distance;
    y = y * LIMIT_RADIUS / distance;
  }

  stick.x = x;
  stick.y = y;

  sendDataThrottled();
  drawJoystick();
}

function resetStick() {
  dragging = false;
  stick.x = 0;
  stick.y = 0;
  sendDataThrottled(true);
  drawJoystick();
}

/* ===================== DATA HANDLING ===================== */
function calculateAxis(x, y) {
  const distance = Math.hypot(x, y);
  if (distance <= DEADZONE_RADIUS) {
    return { x: 0, y: 0 };
  }

  const scale = (distance - DEADZONE_RADIUS) / (LIMIT_RADIUS - DEADZONE_RADIUS);
  return {
    x: Math.round((x / distance) * scale * MAX_VALUE),
    y: Math.round((y / distance) * -scale * MAX_VALUE)
  };
}

function sendData(force = false) {
  const now = Date.now();
  if (!force && now - lastSendTime < UPDATE_INTERVAL) return;
  lastSendTime = now;

  const axis = calculateAxis(stick.x, stick.y);

  const xhr = new XMLHttpRequest();
  xhr.open(
    "GET",
    `/action?x=${axis.x}&y=${axis.y}&a=${sliderA}&b=${sliderB}&c=${sliderC}`,
    true
  );
  xhr.send();
}

function sendDataThrottled(force = false) {
  sendData(force);
}

/* ===================== EVENTS ===================== */
// Mouse
canvas.addEventListener("mousedown", e => {
  dragging = true;
  updateStick(e.clientX, e.clientY);
});

canvas.addEventListener("mousemove", e => {
  if (dragging) updateStick(e.clientX, e.clientY);
});

window.addEventListener("mouseup", resetStick);

// Touch
canvas.addEventListener("touchstart", e => {
  dragging = true;
  updateStick(e.touches[0].clientX, e.touches[0].clientY);
});

canvas.addEventListener("touchmove", e => {
  updateStick(e.touches[0].clientX, e.touches[0].clientY);
});

canvas.addEventListener("touchend", resetStick);
canvas.addEventListener("touchcancel", resetStick);

/* ===================== SLIDERS ===================== */
function setupSlider(id, valueId, setter) {
  const slider = document.getElementById(id);
  const valueLabel = document.getElementById(valueId);

  slider.addEventListener("input", () => {
    setter(parseInt(slider.value));
    valueLabel.textContent = slider.value;
    sendDataThrottled(true);
  });
}

setupSlider("sliderA", "valueA", v => sliderA = v);
setupSlider("sliderB", "valueB", v => sliderB = v);
setupSlider("sliderC", "valueC", v => sliderC = v);

/* ===================== INIT ===================== */
drawJoystick();

window.onload = () => {
  document.getElementById("photo").src =
    window.location.href.slice(0, -1) + ":81/stream";

  // Fetch info every second
  async function updateInfo() {
    try {
      const res = await fetch('/info');
      const info = await res.text();
      document.getElementById("info").textContent = info;
    } catch(e) { console.log('Info:', e); }
    setTimeout(updateInfo, 1000);
  }
  updateInfo();
};
</script>
</body>
</html>

)rawliteral";