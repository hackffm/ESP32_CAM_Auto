static const char PROGMEM INDEX_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32-CAM Robot</title>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">

  <style>
    body {
      font-family: Arial, sans-serif;
      text-align: center;
      margin: 0;
      padding: 10px;
      user-select: none;
      background: #080505ff;
      overflow-y: auto; /* Allow vertical scrolling */
    }

    h1 {
      margin: 4px 0;
      color: #ea903cff;
    }

    /* Main layout: image + joystick side by side */
    #main-wrapper {
      display: flex;
      justify-content: center;
      align-items: flex-start;
      gap: 20px;
      flex-wrap: wrap; /* Stack vertically on small screens */
    }

    #photo-container {
      position: relative;
    }

    img {
      max-width: 100%;
      height: auto;
      display: block;
    }

    /* Joystick positioned to the right of the image */
    #joystick {
      width: 280px;
      height: 280px;
      max-width: 40vw;
      max-height: 40vw;
      touch-action: none; /* Disable browser gestures only for joystick */
    }

    #info {
      font-family: monospace;
      color: #666;
      margin: 4px 0;
    }

    .sliders-wrapper {
      display: flex;
      flex-direction: column;
      gap: 8px;
      width: 90%;
      max-width: 400px;
      margin: 20px auto;
    }

    .slider-container {
      display: flex;
      flex-direction: column;
      text-align: left;
    }

    .slider-container label {
      font-weight: bold;
      margin-bottom: 3px;
      color: #8bdcf3ff;
      font-size: 14px;
    }

    .slider-row {
      display: flex;
      align-items: center;
      gap: 10px;
    }

    .slider-container input[type=range] {
      flex: 1;
      height: 35px;
      margin: 0;
    }

    .slider-value {
      font-size: 16px;
      color: #9b8bf3ff;
      min-width: 45px;
      font-weight: bold;
    }
  </style>
</head>

<body>

  <h1>ESP32-CAM Robot</h1>

  <div id="main-wrapper">
    <div id="photo-container">
      <img src="" id="photo">
    </div>

    <canvas id="joystick" width="300" height="300"></canvas>
  </div>

  <div id="info"></div>

  <div class="sliders-wrapper">

    <div class="slider-container">
      <label for="sliderA">A</label>
      <div class="slider-row">
        <input id="sliderA" type="range" min="-127" max="127" value="0">
        <div class="slider-value" id="valueA">0</div>
      </div>
    </div>

    <div class="slider-container">
      <label for="sliderB">B</label>
      <div class="slider-row">
        <input id="sliderB" type="range" min="-127" max="127" value="0">
        <div class="slider-value" id="valueB">0</div>
      </div>
    </div>

    <div class="slider-container">
      <label for="sliderC">C</label>
      <div class="slider-row">
        <input id="sliderC" type="range" min="-127" max="127" value="0">
        <div class="slider-value" id="valueC">0</div>
      </div>
    </div>

  </div>

<script>

/* ===================== CONFIGURATION ===================== */
const LIMIT_RADIUS = 150;     
const DEADZONE_RADIUS = 40;   
const STICK_RADIUS = 25;      
const MAX_VALUE = 127;
const UPDATE_INTERVAL = 50;   // 20 Hz
/* ========================================================= */

const canvas = document.getElementById("joystick");
const ctx = canvas.getContext("2d");

const center = {
  x: canvas.width / 2,
  y: canvas.height / 2
};

let stick = { x: 0, y: 0 };
let dragging = false;

let sliderA = 0;
let sliderB = 0;
let sliderC = 0;

let lastSendTime = 0;


/* ===================== DRAWING ===================== */
function drawJoystick() {

  ctx.clearRect(0, 0, canvas.width, canvas.height);

  // Outer movement limit
  ctx.beginPath();
  ctx.arc(center.x, center.y, LIMIT_RADIUS, 0, Math.PI * 2);
  ctx.strokeStyle = "white";
  ctx.lineWidth = 3;
  ctx.stroke();

  // Deadzone circle
  ctx.beginPath();
  ctx.arc(center.x, center.y, DEADZONE_RADIUS, 0, Math.PI * 2);
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

  // Calculate scaling between CSS size and actual canvas resolution
  const scaleX = canvas.width / rect.width;
  const scaleY = canvas.height / rect.height;

  const xPos = (clientX - rect.left) * scaleX;
  const yPos = (clientY - rect.top) * scaleY;

  let x = xPos - center.x;
  let y = yPos - center.y;

  const distance = Math.hypot(x, y);

  // Limit stick to outer circle
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


/* ===================== AXIS CALCULATION ===================== */
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


/* ===================== DATA TRANSMISSION ===================== */
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


/* ===================== INPUT EVENTS ===================== */

// Mouse support
canvas.addEventListener("mousedown", e => {
  dragging = true;
  updateStick(e.clientX, e.clientY);
});

canvas.addEventListener("mousemove", e => {
  if (dragging) updateStick(e.clientX, e.clientY);
});

window.addEventListener("mouseup", resetStick);

// Touch support (prevent default only on joystick)
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
canvas.addEventListener("touchcancel", resetStick);


/* ===================== SLIDER SETUP ===================== */
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


/* ===================== INITIALIZATION ===================== */
drawJoystick();

window.onload = () => {

  // Load camera stream
  document.getElementById("photo").src =
    window.location.href.slice(0, -1) + ":81/stream";

  // Poll system info every second
  async function updateInfo() {
    try {
      const res = await fetch('/info');
      const info = await res.text();
      document.getElementById("info").textContent = info;
    } catch(e) {
      console.log('Info error:', e);
    }
    setTimeout(updateInfo, 1000);
  }

  updateInfo();
};

</script>
</body>
</html>

)rawliteral";