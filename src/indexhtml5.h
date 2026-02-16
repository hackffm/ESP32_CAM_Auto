// added stream auto-reconnect and small screen support
static const char PROGMEM INDEX_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32-CAM Robot</title>

  <!-- Allow manual zooming -->
  <meta name="viewport"
        content="width=device-width,
                 initial-scale=1.0,
                 minimum-scale=0.5,
                 maximum-scale=5.0,
                 user-scalable=yes">

  <style>
    body {
      font-family: Arial, sans-serif;
      margin: 0;
      padding: 10px;
      background: #080505ff;
      color: white;
      overflow: auto; /* Allow scrolling if needed */
    }

    h1 {
      text-align: center;
      margin: 4px 0;
      color: #ea903cff;
    }

    /* Main layout container */
    #main-wrapper {
      display: flex;
      justify-content: center;
      align-items: flex-start;
      gap: 20px;
      flex-wrap: nowrap; /* Prevent wrapping */
      overflow-x: auto;  /* Allow horizontal scroll if needed */
    }

    #photo-container img {
      display: block;
      max-height: 80vh; /* Prevent image from exceeding screen height */
      height: auto;
      width: auto;
    }

    /* Joystick container */
    #joystick {
      width: 260px;
      height: 260px;
      touch-action: none; /* Disable gestures only for joystick */
      flex-shrink: 0;     /* Prevent shrinking in flex layout */
    }

    #info {
      font-family: monospace;
      margin-top: 6px;
      text-align: center;
      color: #aaa;
    }

    .sliders-wrapper {
      display: flex;
      flex-direction: column;
      gap: 8px;
      width: 95%;
      max-width: 400px;
      margin: 15px auto;
    }

    .slider-container label {
      font-weight: bold;
      color: #8bdcf3ff;
      font-size: 14px;
    }

    .slider-row {
      display: flex;
      align-items: center;
      gap: 10px;
    }

    input[type=range] {
      flex: 1;
      height: 35px;
    }

    .slider-value {
      min-width: 45px;
      font-weight: bold;
      color: #9b8bf3ff;
    }
  </style>
</head>

<body>

<h1>ESP32-CAM Robot</h1>

<div id="main-wrapper">
  <div id="photo-container">
    <img id="photo">
  </div>

  <canvas id="joystick" width="300" height="300"></canvas>
</div>

<div id="info"></div>

<div class="sliders-wrapper">

  <div class="slider-container">
    <label>A</label>
    <div class="slider-row">
      <input id="sliderA" type="range" min="-127" max="127" value="0">
      <div class="slider-value" id="valueA">0</div>
    </div>
  </div>

  <div class="slider-container">
    <label>B</label>
    <div class="slider-row">
      <input id="sliderB" type="range" min="-127" max="127" value="0">
      <div class="slider-value" id="valueB">0</div>
    </div>
  </div>

  <div class="slider-container">
    <label>C</label>
    <div class="slider-row">
      <input id="sliderC" type="range" min="-127" max="127" value="0">
      <div class="slider-value" id="valueC">0</div>
    </div>
  </div>

</div>

<script>

/* ================= CONFIG ================= */

const LIMIT_RADIUS = 150;
const DEADZONE_RADIUS = 40;
const STICK_RADIUS = 25;
const MAX_VALUE = 127;
const UPDATE_INTERVAL = 50;

/* ========================================== */

const canvas = document.getElementById("joystick");
const ctx = canvas.getContext("2d");
const photo = document.getElementById("photo");

const center = { x: canvas.width / 2, y: canvas.height / 2 };

let stick = { x: 0, y: 0 };
let dragging = false;
let lastSendTime = 0;

let sliderA = 0, sliderB = 0, sliderC = 0;

/* ============= DRAW JOYSTICK ============= */

function drawJoystick() {
  ctx.clearRect(0, 0, canvas.width, canvas.height);

  ctx.beginPath();
  ctx.arc(center.x, center.y, LIMIT_RADIUS, 0, Math.PI * 2);
  ctx.strokeStyle = "white";
  ctx.lineWidth = 3;
  ctx.stroke();

  ctx.beginPath();
  ctx.arc(center.x, center.y, DEADZONE_RADIUS, 0, Math.PI * 2);
  ctx.lineWidth = 1;
  ctx.stroke();

  ctx.beginPath();
  ctx.arc(center.x + stick.x, center.y + stick.y,
          STICK_RADIUS, 0, Math.PI * 2);
  ctx.fillStyle = "#FFA500";
  ctx.fill();
}

/* ============= STICK UPDATE ============= */

function updateStick(clientX, clientY) {

  const rect = canvas.getBoundingClientRect();

  const scaleX = canvas.width / rect.width;
  const scaleY = canvas.height / rect.height;

  const xPos = (clientX - rect.left) * scaleX;
  const yPos = (clientY - rect.top) * scaleY;

  let x = xPos - center.x;
  let y = yPos - center.y;

  const dist = Math.hypot(x, y);

  if (dist > LIMIT_RADIUS) {
    x = x * LIMIT_RADIUS / dist;
    y = y * LIMIT_RADIUS / dist;
  }

  stick.x = x;
  stick.y = y;

  sendData();
  drawJoystick();
}

function resetStick() {
  dragging = false;
  stick = { x: 0, y: 0 };
  sendData(true);
  drawJoystick();
}

/* ============= AXIS CALCULATION ============= */

function calculateAxis(x, y) {

  const dist = Math.hypot(x, y);
  if (dist <= DEADZONE_RADIUS) return { x: 0, y: 0 };

  const scale = (dist - DEADZONE_RADIUS) /
                (LIMIT_RADIUS - DEADZONE_RADIUS);

  return {
    x: Math.round((x / dist) * scale * MAX_VALUE),
    y: Math.round((y / dist) * -scale * MAX_VALUE)
  };
}

/* ============= SEND DATA ============= */

function sendData(force = false) {

  const now = Date.now();
  if (!force && now - lastSendTime < UPDATE_INTERVAL) return;
  lastSendTime = now;

  const axis = calculateAxis(stick.x, stick.y);

  fetch(`/action?x=${axis.x}&y=${axis.y}&a=${sliderA}&b=${sliderB}&c=${sliderC}`)
    .catch(() => {});
}

/* ============= INPUT EVENTS ============= */

canvas.addEventListener("mousedown", e => {
  dragging = true;
  updateStick(e.clientX, e.clientY);
});

canvas.addEventListener("mousemove", e => {
  if (dragging) updateStick(e.clientX, e.clientY);
});

window.addEventListener("mouseup", resetStick);

canvas.addEventListener("touchstart", e => {
  e.preventDefault();
  dragging = true;
  updateStick(e.touches[0].clientX, e.touches[0].clientY);
}, { passive: false });

canvas.addEventListener("touchmove", e => {
  e.preventDefault();
  if (dragging)
    updateStick(e.touches[0].clientX, e.touches[0].clientY);
}, { passive: false });

canvas.addEventListener("touchend", resetStick);

/* ============= SLIDERS ============= */

function setupSlider(id, valueId, setter) {
  const slider = document.getElementById(id);
  const valueLabel = document.getElementById(valueId);

  slider.addEventListener("input", () => {
    setter(parseInt(slider.value));
    valueLabel.textContent = slider.value;
    sendData(true);
  });
}

setupSlider("sliderA", "valueA", v => sliderA = v);
setupSlider("sliderB", "valueB", v => sliderB = v);
setupSlider("sliderC", "valueC", v => sliderC = v);

/* ============= STREAM HANDLING ============= */

/*
   Robust stream loader:
   - Adds timestamp to bypass caching
   - Retries automatically if loading fails
   - Periodically refreshes stream to keep it alive
*/

function loadStream() {
  const base = window.location.origin;
  photo.src = `${base}:81/stream?t=${Date.now()}`;
}

photo.onerror = function() {
  console.log("Stream error - retrying...");
  setTimeout(loadStream, 2000);
};

/* Periodic refresh every 60 seconds */
setInterval(loadStream, 60000);

/* ============= INIT ============= */

drawJoystick();
loadStream();

/* Fetch device info every second */
async function updateInfo() {
  try {
    const res = await fetch('/info');
    const txt = await res.text();
    document.getElementById("info").textContent = txt;
  } catch(e) {}
  setTimeout(updateInfo, 1000);
}
updateInfo();

</script>
</body>
</html>
)rawliteral";


