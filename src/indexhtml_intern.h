// Not used if index.html exists in data/index.html
// Use data/index.html for development and testing, as overwriting data will erase user settings as well. 
static const char PROGMEM INDEX_HTML[] = R"rawliteral(
<!DOCTYPE html>
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Hackffm ESP32 CAM Bot</title>
  <style>
    body {
      font-family: sans-serif; margin: 0;
      padding: 10px; background: #080505; color: white; text-align: center; }

    h1 { margin: 4px 0; color: #f0ad5e; }
    h2 { color: #8bdcf3; font-size: 16px; margin: 18px 0 6px 0; }

    .section {
      border: 1px solid #333; border-radius: 8px;
      padding: 14px; margin: 16px auto;
      max-width: 700px; text-align: left; }

    /* ── Stream / joystick ── */
    #photo-container {
      position: relative; display: inline-block;
      /* width follows the scaled image; joystick stays on top unscaled */ }

    #media-wrapper { display: flex; justify-content: center;
      align-items: center; gap: 20px; flex-wrap: wrap; }

    .side-mode   { flex-direction: row; }
    .normal-mode { flex-direction: column; }

    #joystick { margin: 0; touch-action: none; transition: all 0.2s ease; z-index: 10; }

    .normal-mode #joystick { display: block; margin: 15px auto; width: 300px; height: 300px; }

    .side-mode #joystick { position: absolute; right: 10px; bottom: 10px;
      width: 210px; height: 210px; pointer-events: auto; }

    /* photo is scaled via CSS transform; container clips to natural size */
    #photo-scale-clip {
      display: inline-block; overflow: hidden;
      line-height: 0; }

    #photo {
      display: block; max-width: none;
      transform-origin: top left;
      transition: transform 0.2s ease; }

    #info { font-family: monospace; margin: 8px auto; color: #aaa;
      white-space: pre-wrap; max-width: 700px; text-align: center; }

    /* ── Stream controls row ── */
    .stream-controls {
      display: flex; align-items: center; gap: 10px;
      justify-content: center; flex-wrap: wrap;
      margin: 6px 0; font-size: 14px; color: #aaa; }

    .stream-controls label { display: flex; align-items: center; gap: 4px; }

    .stream-controls select {
      font-size: 13px; padding: 2px 4px;
      background: #1a1a1a; color: white;
      border: 1px solid #555; border-radius: 4px; }

    #btn-stream-toggle {
      padding: 4px 12px; font-size: 13px;
      border: none; border-radius: 4px;
      background-color: #333; color: #8bdcf3;
      cursor: pointer; }
    #btn-stream-toggle:hover { background-color: #555; }
    #btn-stream-toggle.running { color: #f87; }

    /* ── Sliders ── */
    .slider-container { margin-bottom: 10px; }
    .slider-container label { font-weight: bold; color: #8bdcf3; font-size: 14px; }
    .slider-row { display: flex; align-items: center; gap: 10px; margin-top: 4px; }
    input[type=range] { flex: 1; height: 35px; }
    .slider-value { min-width: 45px; font-weight: bold; color: #9b8bf3; text-align: right; }

    /* ── Gamepad ── */
    .gpds_row { display: flex; gap: 8px;
      align-items: center; flex-wrap: wrap; margin-bottom: 8px; }
    .gpds_axes { margin-top: 5px; font-family: monospace; font-size: 12px; color: #aaa; }

    input[type="text"]   { width: 90px; font-size: 12px; padding: 2px; }
    input[type="number"] { width: 55px; font-size: 12px; padding: 2px; }
    select { font-size: 12px; padding: 2px;
      background: #1a1a1a; color: white; border: 1px solid #555; border-radius: 4px; }

    button { padding: 5px 12px; font-size: 13px; border: none; border-radius: 4px;
      background-color: #333; color: #8bdcf3; cursor: pointer; }
    button:hover { background-color: #555; }

    /* ── Bottom button row ── */
    .action-btn-row {
      display: flex; gap: 10px; justify-content: center;
      flex-wrap: wrap; margin: 16px auto; max-width: 700px; }

    .settings-btn {
      padding: 8px 20px; font-size: 14px; border: none; border-radius: 6px;
      background-color: #444; color: #f0ad5e; cursor: pointer;
      text-decoration: none; display: inline-block; }
    .settings-btn:hover { background-color: #666; }

    .reconnect-btn {
      padding: 8px 16px; font-size: 14px; border: none; border-radius: 6px;
      background-color: #333; color: #8bdcf3; cursor: pointer; }
    .reconnect-btn:hover { background-color: #555; }

    footer { margin-top: 30px; padding-top: 10px; border-top: 1px solid #333; }

    a:link    { text-decoration: none; color: #f0ad5e; }
    a:visited { text-decoration: none; color: #f0ad5e; }
    a:hover   { text-decoration: underline; }
    a:active  { text-decoration: underline; }
  </style>
</head>
<body>
  <h1 id="title">Hackffm ESP32 CAM Bot</h1>

  <!-- ── Stream + overlay joystick ── -->
  <div id="media-wrapper" class="normal-mode">
    <div id="photo-container">
      <div id="photo-scale-clip">
        <img id="photo">
      </div>
      <canvas id="joystick" width="300" height="300"></canvas>
    </div>
  </div>

  <!-- ── Stream controls ── -->
  <div class="stream-controls">
    <label>
      <input type="checkbox" id="modeToggle">
      Joystick overlay
    </label>

    <label>
      Scale:
      <select id="streamScale">
        <option value="0.5">0.5×</option>
        <option value="1"   selected>1×</option>
        <option value="2">2×</option>
        <option value="4">4×</option>
      </select>
    </label>

    <button id="btn-stream-toggle" class="running">⏹ Stop stream</button>
  </div>

  <div id="info"></div>

  <!-- ── Sliders box ── -->
  <div class="section">
    <h2>Controls</h2>
    <div id="sliders"></div>
  </div>

  <!-- ── Gamepad box ── -->
  <div class="section">
    <h2>Gamepad</h2>

    <div class="gpds_row">
      <button id="searchGamepad">Search gamepad</button>
      <p id="output" style="margin:0">No gamepad detected</p>
    </div>

    <div class="gpds_row">
      <button id="searchGamepadServoUP">Set servo UP</button>
      <p id="outputGamepadServoUP" style="margin:0">No gamepad button set</p>
    </div>

    <div class="gpds_row">
      <button id="searchGamepadServoDOWN">Set servo DOWN</button>
      <p id="outputGamepadServoDOWN" style="margin:0">No gamepad button set</p>
    </div>

    <div class="gpds_row">
      <span style="color:#8bdcf3;font-size:14px;">Gamepad:</span>
      <select id="gpdsel_gamepadSelect"></select>
    </div>

    <div class="gpds_row">
      <span style="color:#8bdcf3;font-size:14px;">X Axis:</span>
      <select id="gpdsel_xAxis">
        <option value="0" selected>0</option><option value="1">1</option>
        <option value="2">2</option><option value="3">3</option>
        <option value="4">4</option><option value="5">5</option>
      </select>
      <label><input type="checkbox" id="gpdsel_invertX"> Invert</label>

      <span style="color:#8bdcf3;font-size:14px;margin-left:8px;">Y Axis:</span>
      <select id="gpdsel_yAxis">
        <option value="0">0</option><option value="1" selected>1</option>
        <option value="2">2</option><option value="3">3</option>
        <option value="4">4</option><option value="5">5</option>
      </select>
      <label><input type="checkbox" id="gpdsel_invertY"> Invert</label>
    </div>

    <div class="gpds_row">
      <span style="color:#aaa;font-size:13px;">X: <span id="gpdsel_xVal">0.00</span></span>
      <span style="color:#aaa;font-size:13px;">Y: <span id="gpdsel_yVal">0.00</span></span>
    </div>

    <div class="gpds_axes" id="gpdsel_axesDisplay"></div>
  </div>

  <!-- ── Action buttons row ── -->
  <div class="action-btn-row">
    <button id="btn-reconnect" class="reconnect-btn">🔄 WiFi Reconnect</button>
    <button id="btn-restart"   class="reconnect-btn">⟳ Restart</button>
    <a href="/settings" class="settings-btn">⚙ Settings</a>
  </div>

  <footer>
    <div style="display:flex;gap:8px;align-items:center;justify-content:center;flex-wrap:wrap;">
      <img src="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAADMAAAA/AgMAAAAwDRjCAAAADFBMVEVuAAAoKCjwrV74+Ph5Qa9/AAAAhElEQVQoz6XTSQrAIAwF0F7yr3O6HDG7ljTRUhW+0sGF+CADEd28W/u20oG2/kivszJVKpcWTKSmUy3yABv6dUIRmHAJryUGnynv2bgko4RKJBpY7EReI6MNkZUOTiUSanmDyjxGtZrBkVVw542KMDcq5BVJV7NXnXairOLv9eldP/5HJ2k/9FhNcZTRAAAAAElFTkSuQmCC"
       alt="hackffm.de" style="width:51px;height:63px;"/>
      <span>
        ESP32_CAM_Auto V1.00 on
        <a href="https://github.com/hackffm/ESP32_CAM_Auto">GitHub</a> |
        <a href="https://www.hackerspace-ffm.de/wiki/index.php?title=FPV-Roboter">&copy; 2026 Hackerspace-FFM e.V.</a>
      </span>
    </div>
  </footer>

<script>
  /* ================= CONFIG ================= */
  const LIMIT_RADIUS = 150;
  const DEADZONE_RADIUS = 0;
  const STICK_RADIUS = 25;
  const MAX_VALUE = 255;
  const UPDATE_INTERVAL = 50;
  let   STICK_COLOR = "rgba(255,165,0,0.3)";

  const STICK_X_EXPO_FACTOR = 0.35;
  const STICK_Y_EXPO_FACTOR = 0.7;

  /* ========================================== */

  const canvas       = document.getElementById("joystick");
  const ctx          = canvas.getContext("2d");
  const photo        = document.getElementById("photo");
  const scaleClip    = document.getElementById("photo-scale-clip");
  const titleElement = document.getElementById("title");

  let stick = { x: 0, y: 0 };
  let dragging = false;
  let lastSendTime = 0;
  let roboter_name_set = false;
  let sliderValues = { A:0, B:0, C:0, D:0, E:0 };

  /* ============= STREAM SCALE ============= */
  function applyScale() {
    const s = parseFloat(document.getElementById('streamScale').value);
    photo.style.transform = `scale(${s})`;
    // shrink/grow the clipping wrapper so surrounding layout follows
    if (photo.naturalWidth) {
      scaleClip.style.width  = (photo.naturalWidth  * s) + 'px';
      scaleClip.style.height = (photo.naturalHeight * s) + 'px';
    } else {
      scaleClip.style.width  = '';
      scaleClip.style.height = '';
    }
  }

  document.getElementById('streamScale').addEventListener('change', applyScale);

  // re-apply after image dimensions become known
  photo.addEventListener('load', applyScale);

  /* ============= STREAM START / STOP ============= */
  let streamRunning = false;
  const btnStreamToggle = document.getElementById('btn-stream-toggle');

  function startStream() {
    photo.src = `${window.location.origin}:81/stream?t=${Date.now()}`;
    streamRunning = true;
    btnStreamToggle.textContent = '⏹ Stop stream';
    btnStreamToggle.classList.add('running');
  }

  function stopStream() {
    photo.src = '';          // cancels the ongoing MJPEG fetch
    streamRunning = false;
    btnStreamToggle.textContent = '▶ Start stream';
    btnStreamToggle.classList.remove('running');
    // clear the clip dimensions so the layout collapses cleanly
    scaleClip.style.width  = '';
    scaleClip.style.height = '';
  }

  btnStreamToggle.addEventListener('click', () => {
    if (streamRunning) stopStream(); else startStream();
  });

  /* ============= STREAM LOAD / ERROR / WATCHDOG ============= */
  let streamRetryDelay = 1000;

  function loadStream() {
    if (!streamRunning) return;
    photo.src = `${window.location.origin}:81/stream?t=${Date.now()}`;
    updateJoystickMode();
  }

  photo.onload  = () => { streamRetryDelay = 1000; applyScale(); };
  photo.onerror = () => {
    if (!streamRunning) return;
    console.warn("Stream error, retrying in " + streamRetryDelay + "ms");
    setTimeout(loadStream, streamRetryDelay);
    streamRetryDelay = Math.min(streamRetryDelay * 2, 10000);
  };

  setInterval(() => {
    if (streamRunning && (!photo.complete || photo.naturalWidth === 0)) loadStream();
  }, 10000);

  /* ============= CREATE SLIDERS ============= */
  const slidersContainer = document.getElementById("sliders");

  ["A","B","C","D","E"].forEach(letter => {
    const containerDiv = document.createElement("div");
    containerDiv.className = "slider-container";

    const label = document.createElement("label");
    label.id = "label"+letter;
    label.textContent = letter;

    const row = document.createElement("div");
    row.className = "slider-row";

    const input = document.createElement("input");
    input.type = "range";
    input.min = -127;
    input.max = 127;
    input.value = 0;
    input.dataset.for = letter;

    const value = document.createElement("div");
    value.className = "slider-value";
    value.textContent = "0";
    if(letter === "A") input.value = 100;

    input.addEventListener("input", () => {
      sliderValues[letter] = parseInt(input.value);
      value.textContent = input.value;
      sendData(false);
    });

    row.appendChild(input);
    row.appendChild(value);
    containerDiv.appendChild(label);
    containerDiv.appendChild(row);
    slidersContainer.appendChild(containerDiv);
  });

  /* ============= DRAW JOYSTICK ============= */
  function drawJoystick() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    const center = { x: canvas.width/2, y: canvas.height/2 };

    ctx.beginPath();
    ctx.arc(center.x, center.y, LIMIT_RADIUS, 0, Math.PI*2);
    ctx.strokeStyle = "white";
    ctx.lineWidth = 1;
    ctx.stroke();

    ctx.beginPath();
    ctx.arc(center.x + stick.x, center.y + stick.y, STICK_RADIUS, 0, Math.PI*2);
    ctx.fillStyle = STICK_COLOR;
    ctx.fill();
  }

  /* ============= JOYSTICK POSITION MODE ============= */
  function updateJoystickMode() {
    const wrapper = document.getElementById("media-wrapper");
    const overlayActive = document.getElementById("modeToggle").checked;

    canvas.style.width  = "300px";
    canvas.style.height = "300px";

    if (!overlayActive) {
      wrapper.classList.remove("side-mode");
      wrapper.classList.add("normal-mode");
      canvas.style.position = "static";
    } else {
      wrapper.classList.remove("normal-mode");
      wrapper.classList.add("side-mode");
      canvas.style.position = "absolute";
    }
    drawJoystick();
  }

  /* ============= UPDATE STICK ============= */
  function updateStick(clientX, clientY) {
    const rect = canvas.getBoundingClientRect();
    const scaleX = canvas.width  / rect.width;
    const scaleY = canvas.height / rect.height;
    const xPos = (clientX - rect.left) * scaleX;
    const yPos = (clientY - rect.top)  * scaleY;
    const center = { x: canvas.width/2, y: canvas.height/2 };

    let x = xPos - center.x;
    let y = yPos - center.y;

    stick = { x, y };
    sendData();
    drawJoystick();
  }

  function resetStick() {
    dragging = false;
    stick = { x:0, y:0 };
    sendData(true);
    drawJoystick();
  }

  /* ============= AXIS CALCULATION ============= */
  function expo(val, factor) {
    return Math.sin(val * (Math.PI / 2)) * factor;
  }

  function calculateAxis(x, y) {
    x = 150 * expo(x / 150, STICK_X_EXPO_FACTOR);
    y = 150 * expo(y / 150, STICK_Y_EXPO_FACTOR);
    const dist = Math.hypot(x, y);
    if (dist <= DEADZONE_RADIUS) return { x:0, y:0 };
    const scale = (dist - DEADZONE_RADIUS) / (LIMIT_RADIUS - DEADZONE_RADIUS);
    return {
      x: Math.round((x / dist) * scale * MAX_VALUE),
      y: Math.round((y / dist) * -scale * MAX_VALUE)
    };
  }

  /* ============= SEND DATA ============= */
  let retryTimeout = null;
  function sendData(force=false) {
    const now = Date.now();
    if (!force && now - lastSendTime < UPDATE_INTERVAL) {
      if (!retryTimeout) {
        retryTimeout = setTimeout(() => { sendData(false); },
          UPDATE_INTERVAL - (now - lastSendTime));
      }
      return;
    }
    if (retryTimeout) { clearTimeout(retryTimeout); retryTimeout = null; }
    lastSendTime = now;

    const axis = calculateAxis(stick.x, stick.y);
    fetch(`/action?x=${axis.x}&y=${axis.y}` +
          `&a=${sliderValues.A}&b=${sliderValues.B}&c=${sliderValues.C}` +
          `&d=${sliderValues.D}&e=${sliderValues.E}`)
          .catch(()=>{});
  }

  /* ============= TOKEN PARSER ============= */
  function parseTokens(tokenString) {
    const regex = /([A-E]|Name)="([^"]*)"/g;
    let match;
    while ((match = regex.exec(tokenString)) !== null) {
      const key   = match[1];
      const value = match[2];
      if (key === "Name") {
        titleElement.textContent = "HackFFM-Bot: " + value;
        document.title = "HackFFM-Bot: " + value;
        if (!roboter_name_set) roboter_name_set = true;
      } else {
        const labelElement = document.getElementById("label" + key);
        if (labelElement) labelElement.textContent = value;
      }
    }
  }

  /* ============= INFO UPDATE ============= */
  async function updateInfo() {
    try {
      const res = await fetch('/info');
      const txt = await res.text();
      const parts = txt.split("|");
      document.getElementById("info").textContent = parts[0].trim();
      if (parts.length > 1) parseTokens(parts[1]);
    } catch(e) {}
    setTimeout(updateInfo, 1000);
  }

  /* ============= INPUT EVENTS ============= */
  canvas.addEventListener("mousedown", e => { dragging = true; updateStick(e.clientX, e.clientY); });
  canvas.addEventListener("mousemove", e => { if (dragging) updateStick(e.clientX, e.clientY); });
  window.addEventListener("mouseup", resetStick);

  canvas.addEventListener("touchstart", e => {
    e.preventDefault(); dragging = true;
    updateStick(e.touches[0].clientX, e.touches[0].clientY);
  }, { passive:false });

  canvas.addEventListener("touchmove", e => {
    e.preventDefault();
    if (dragging) updateStick(e.touches[0].clientX, e.touches[0].clientY);
  }, { passive:false });

  canvas.addEventListener("touchend", resetStick);

  document.getElementById('media-wrapper').addEventListener('click', () => {
    updateJoystickMode();
  });

  /* ============= RECONNECT / RESTART ============= */
  document.getElementById('btn-reconnect').addEventListener('click', () => {
    fetch('/action?reconnect=1', { method: 'GET' });
  });
  document.getElementById('btn-restart').addEventListener('click', () => {
    fetch('/action?restart=1', { method: 'GET' });
  });

  /* ============= GAMEPAD ============= */
  const gpdsel_gamepadSelect  = document.getElementById('gpdsel_gamepadSelect');
  const gpdsel_xAxisSelect    = document.getElementById('gpdsel_xAxis');
  const gpdsel_yAxisSelect    = document.getElementById('gpdsel_yAxis');
  const gpdsel_invertX        = document.getElementById('gpdsel_invertX');
  const gpdsel_invertY        = document.getElementById('gpdsel_invertY');
  const gpdsel_axesDisplay    = document.getElementById('gpdsel_axesDisplay');
  const gpdsel_xVal           = document.getElementById('gpdsel_xVal');
  const gpdsel_yVal           = document.getElementById('gpdsel_yVal');

  let selectedGamepadIndex    = null;
  let lastGamepadButtons      = null;
  let servo_up_index          = null;
  let servo_down_index        = null;
  let gpdsel_activityTimeout  = null;
  let searching               = false;
  let searchingServoUP        = false;
  let searchingServoDOWN      = false;

  function format_fix2(v) { return parseFloat(v).toFixed(2); }

  function gpdsel_populateGamepads() {
    const gamepads = navigator.getGamepads();
    gpdsel_gamepadSelect.innerHTML = '';
    const optdefault = document.createElement('option');
    optdefault.value = -1;
    optdefault.textContent = "No Gamepad";
    gpdsel_gamepadSelect.appendChild(optdefault);
    for (let i = 0; i < gamepads.length; i++) {
      if (gamepads[i]) {
        const opt = document.createElement('option');
        opt.value = i;
        opt.textContent = `${i}: ${gamepads[i].id}`;
        gpdsel_gamepadSelect.appendChild(opt);
      }
    }
  }

  function gpdsel_activityTimeoutFunction() {
    gpdsel_activityTimeout = null;
    STICK_COLOR = "rgba(255,165,0,0.3)";
    stick = { x: 0, y: 0 };
    sendData(true);
    drawJoystick();
  }

  function gpdsel_update() {
    if (selectedGamepadIndex === null) return;
    const gamepads = navigator.getGamepads();
    const gp = gamepads[selectedGamepadIndex];
    if (!gp) return;

    if (servo_up_index !== null && gp.buttons[servo_up_index] && gp.buttons[servo_up_index].pressed) {
      sliderValues.B = Math.min(127, (sliderValues.B || 0) + 5);
      sendData(true);
    }
    if (servo_down_index !== null && gp.buttons[servo_down_index] && gp.buttons[servo_down_index].pressed) {
      sliderValues.B = Math.max(-127, (sliderValues.B || 0) - 5);
      sendData(true);
    }

    const axes = gp.axes;
    gpdsel_axesDisplay.textContent = axes.map((v, i) => `A${i}: ${format_fix2(v)}`).join(' | ');

    let x = axes[gpdsel_xAxisSelect.value] || 0;
    let y = axes[gpdsel_yAxisSelect.value] || 0;
    if (gpdsel_invertX.checked) x *= -1;
    if (gpdsel_invertY.checked) y *= -1;
    gpdsel_xVal.textContent = format_fix2(x);
    gpdsel_yVal.textContent = format_fix2(y);

    if ((Math.abs(x) > 0.1) || (Math.abs(y) > 0.1)) {
      if (gpdsel_activityTimeout !== null) {
        clearTimeout(gpdsel_activityTimeout);
        gpdsel_activityTimeout = null;
      }
      gpdsel_activityTimeout = setTimeout(gpdsel_activityTimeoutFunction, 8000);
      STICK_COLOR = "rgba(255,0,165,0.3)";
    }
    if (gpdsel_activityTimeout !== null) {
      stick = { x: x * LIMIT_RADIUS, y: y * LIMIT_RADIUS };
      sendData(true);
      drawJoystick();
    }
  }

  /* --- Cookie helpers --- */
  function setCookie(name, value, days) {
    document.cookie = name + "=" + value +
      (days ? "; max-age=" + days * 86400 : "") + "; path=/";
  }

  function getCookie(cname) {
    const name = cname + "=";
    const ca = decodeURIComponent(document.cookie).split(';');
    for (let c of ca) {
      c = c.trimStart();
      if (c.indexOf(name) === 0) return c.substring(name.length);
    }
    return "";
  }

  /* --- Search gamepad --- */
  const search_gamepad_button            = document.getElementById("searchGamepad");
  const search_gamepad_output            = document.getElementById("output");
  const search_gamepad_button_servo_up   = document.getElementById("searchGamepadServoUP");
  const search_gamepad_output_servo_up   = document.getElementById("outputGamepadServoUP");
  const search_gamepad_button_servo_down = document.getElementById("searchGamepadServoDOWN");
  const search_gamepad_output_servo_down = document.getElementById("outputGamepadServoDOWN");

  search_gamepad_button.addEventListener("click", () => {
    if (!searching) {
      lastGamepadButtons = null;
      selectedGamepadIndex = null;
      searching = true;
      search_gamepad_output.textContent = "Press any button on your gamepad...";
      requestAnimationFrame(checkGamepad);
    } else {
      searching = false;
      search_gamepad_output.textContent = "No gamepad selected";
    }
  });

  search_gamepad_button_servo_up.addEventListener("click", () => {
    if (selectedGamepadIndex === null) {
      search_gamepad_output_servo_up.textContent = "Select a gamepad first!";
      return;
    }
    searchingServoUP = true;
    search_gamepad_output_servo_up.textContent = "Press the UP button...";
    requestAnimationFrame(checkGamepadServoUP);
  });

  search_gamepad_button_servo_down.addEventListener("click", () => {
    if (selectedGamepadIndex === null) {
      search_gamepad_output_servo_down.textContent = "Select a gamepad first!";
      return;
    }
    searchingServoDOWN = true;
    search_gamepad_output_servo_down.textContent = "Press the DOWN button...";
    requestAnimationFrame(checkGamepadServoDOWN);
  });

  function checkGamepad() {
    if (!searching) return;
    const gamepads = navigator.getGamepads();
    for (let i = 0; i < gamepads.length; i++) {
      const gp = gamepads[i];
      if (!gp) continue;
      for (let j = 0; j < gp.buttons.length; j++) {
        if (gp.buttons[j].pressed) {
          searching = false;
          selectedGamepadIndex = i;
          search_gamepad_output.textContent = `Gamepad ${i}: ${gp.id} (btn ${j})`;
          setCookie("gamepad", i, 30);
          return;
        }
      }
    }
    requestAnimationFrame(checkGamepad);
  }

  function checkGamepadServoUP() {
    if (!searchingServoUP) return;
    const gp = navigator.getGamepads()[selectedGamepadIndex];
    if (!gp) return;
    for (let j = 0; j < gp.buttons.length; j++) {
      if (gp.buttons[j].pressed) {
        searchingServoUP = false;
        servo_up_index = j;
        search_gamepad_output_servo_up.textContent = `Servo UP: button ${j}`;
        setCookie("gamepadBtnUp", j, 30);
        return;
      }
    }
    requestAnimationFrame(checkGamepadServoUP);
  }

  function checkGamepadServoDOWN() {
    if (!searchingServoDOWN) return;
    const gp = navigator.getGamepads()[selectedGamepadIndex];
    if (!gp) return;
    for (let j = 0; j < gp.buttons.length; j++) {
      if (gp.buttons[j].pressed) {
        searchingServoDOWN = false;
        servo_down_index = j;
        search_gamepad_output_servo_down.textContent = `Servo DOWN: button ${j}`;
        setCookie("gamepadBtnDown", j, 30);
        return;
      }
    }
    requestAnimationFrame(checkGamepadServoDOWN);
  }

  /* --- Restore cookies --- */
  (function restoreCookies() {
    const gi = getCookie("gamepad");
    if (gi !== "" && Number(gi) >= 0) selectedGamepadIndex = Number(gi);
    const bu = getCookie("gamepadBtnUp");
    if (bu !== "" && Number(bu) >= 0) servo_up_index = Number(bu);
    const bd = getCookie("gamepadBtnDown");
    if (bd !== "" && Number(bd) >= 0) servo_down_index = Number(bd);
  })();

  window.addEventListener("gamepadconnected",    gpdsel_populateGamepads);
  window.addEventListener("gamepaddisconnected", gpdsel_populateGamepads);
  gpdsel_populateGamepads();
  setInterval(gpdsel_update, 75);

  /* ============= INIT ============= */
  document.getElementById("modeToggle").addEventListener("change", updateJoystickMode);
  document.getElementById("modeToggle").checked = true;
  updateJoystickMode();
  drawJoystick();
  startStream();   // auto-start on page load
  updateInfo();
</script>
</body>
</html>
)rawliteral";

static const unsigned char favicon[] PROGMEM = {
  0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x10, 0x10, 0x00, 0x00, 0x01, 0x00, 0x20, 0x00, 0x68, 0x04,
  0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x20, 0x00,
  0x00, 0x00, 0x01, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x12, 0x0b,
  0x00, 0x00, 0x12, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf6, 0xf7,
  0xf7, 0xff, 0xbc, 0xbc, 0xbd, 0xff, 0x05, 0x04, 0x06, 0xff, 0x06, 0x05, 0x07, 0xff, 0x57, 0x57,
  0x59, 0xff, 0xc5, 0xd3, 0xde, 0xff, 0xca, 0xd7, 0xe2, 0xff, 0x90, 0x90, 0x91, 0xff, 0x90, 0x90,
  0x91, 0xff, 0xcc, 0xd9, 0xe4, 0xff, 0xc1, 0xce, 0xd8, 0xff, 0x5c, 0x5c, 0x5d, 0xff, 0x05, 0x04,
  0x06, 0xff, 0x05, 0x04, 0x06, 0xff, 0xc2, 0xc3, 0xc3, 0xff, 0xf6, 0xf7, 0xf7, 0xff, 0xf6, 0xf7,
  0xf7, 0xff, 0xbc, 0xbc, 0xbd, 0xff, 0x05, 0x04, 0x06, 0xff, 0x05, 0x04, 0x06, 0xff, 0x08, 0x07,
  0x09, 0xff, 0xde, 0xdf, 0xdf, 0xff, 0xc0, 0xc2, 0xc4, 0xff, 0x64, 0x64, 0x65, 0xff, 0x61, 0x60,
  0x61, 0xff, 0xc7, 0xc8, 0xca, 0xff, 0xd7, 0xd8, 0xd8, 0xff, 0x08, 0x07, 0x09, 0xff, 0x05, 0x04,
  0x06, 0xff, 0x05, 0x04, 0x06, 0xff, 0xc2, 0xc3, 0xc3, 0xff, 0xf6, 0xf7, 0xf7, 0xff, 0xf6, 0xf7,
  0xf7, 0xff, 0xd5, 0xd6, 0xd6, 0xff, 0x23, 0x23, 0x25, 0xff, 0x24, 0x24, 0x25, 0xff, 0x96, 0x99,
  0x9b, 0xff, 0xa3, 0xa4, 0xa4, 0xff, 0xe7, 0xe8, 0xe9, 0xff, 0xb2, 0xb3, 0xb3, 0xff, 0xb2, 0xb3,
  0xb3, 0xff, 0xe9, 0xea, 0xea, 0xff, 0x9d, 0x9d, 0x9e, 0xff, 0x9d, 0x9f, 0xa0, 0xff, 0x22, 0x21,
  0x23, 0xff, 0x25, 0x25, 0x27, 0xff, 0xd9, 0xd9, 0xda, 0xff, 0xf6, 0xf7, 0xf7, 0xff, 0xf6, 0xf7,
  0xf7, 0xff, 0xf6, 0xf7, 0xf7, 0xff, 0xdd, 0xde, 0xde, 0xff, 0xbf, 0xc0, 0xc0, 0xff, 0x2f, 0x2f,
  0x31, 0xff, 0x25, 0x24, 0x26, 0xff, 0x3f, 0x3e, 0x40, 0xff, 0x3f, 0x3e, 0x40, 0xff, 0x3f, 0x3e,
  0x40, 0xff, 0x3f, 0x3e, 0x40, 0xff, 0x24, 0x23, 0x25, 0xff, 0x30, 0x30, 0x33, 0xff, 0xc4, 0xc5,
  0xc5, 0xff, 0xde, 0xdf, 0xdf, 0xff, 0xf6, 0xf7, 0xf7, 0xff, 0xf6, 0xf7, 0xf7, 0xff, 0xf6, 0xf7,
  0xf7, 0xff, 0xf6, 0xf7, 0xf7, 0xff, 0xf6, 0xf7, 0xf7, 0xff, 0x43, 0x43, 0x44, 0xff, 0x91, 0x91,
  0x92, 0xff, 0xc5, 0xc5, 0xc6, 0xff, 0xb4, 0xb4, 0xb5, 0xff, 0xb4, 0xb4, 0xb5, 0xff, 0xb4, 0xb4,
  0xb5, 0xff, 0xb4, 0xb4, 0xb5, 0xff, 0xc8, 0xc9, 0xc9, 0xff, 0x8b, 0x8b, 0x8c, 0xff, 0x48, 0x48,
  0x49, 0xff, 0xf6, 0xf7, 0xf7, 0xff, 0xf6, 0xf7, 0xf7, 0xff, 0xf6, 0xf7, 0xf7, 0xff, 0xf6, 0xf7,
  0xf7, 0xff, 0xf6, 0xf7, 0xf7, 0xff, 0x90, 0x92, 0x95, 0xff, 0x1a, 0x20, 0x27, 0xff, 0xc2, 0xc3,
  0xc3, 0xff, 0x19, 0x18, 0x1a, 0xff, 0x05, 0x04, 0x06, 0xff, 0x05, 0x04, 0x06, 0xff, 0x05, 0x04,
  0x06, 0xff, 0x05, 0x04, 0x06, 0xff, 0x1c, 0x1c, 0x1d, 0xff, 0xc3, 0xc4, 0xc5, 0xff, 0x1b, 0x21,
  0x29, 0xff, 0x94, 0x96, 0x99, 0xff, 0xf6, 0xf7, 0xf7, 0xff, 0xf6, 0xf7, 0xf7, 0xff, 0xf6, 0xf7,
  0xf7, 0xff, 0xf6, 0xf7, 0xf7, 0xff, 0x60, 0x72, 0x84, 0xff, 0x08, 0x39, 0x67, 0xff, 0xa9, 0xaa,
  0xaa, 0xff, 0x06, 0x14, 0x25, 0xff, 0x08, 0x7f, 0xec, 0xff, 0x05, 0x04, 0x06, 0xff, 0x05, 0x04,
  0x06, 0xff, 0x08, 0x83, 0xf4, 0xff, 0x05, 0x11, 0x1e, 0xff, 0xad, 0xae, 0xb0, 0xff, 0x07, 0x3a,
  0x6b, 0xff, 0x66, 0x76, 0x84, 0xff, 0xf6, 0xf7, 0xf7, 0xff, 0xf6, 0xf7, 0xf7, 0xff, 0xf6, 0xf7,
  0xf7, 0xff, 0xf6, 0xf7, 0xf7, 0xff, 0x6b, 0x72, 0x79, 0xff, 0x0c, 0x1d, 0x2e, 0xff, 0xbb, 0xbb,
  0xbb, 0xff, 0x13, 0x14, 0x18, 0xff, 0x07, 0x18, 0x2a, 0xff, 0x05, 0x04, 0x06, 0xff, 0x05, 0x04,
  0x06, 0xff, 0x06, 0x18, 0x2a, 0xff, 0x16, 0x17, 0x1a, 0xff, 0xbe, 0xbf, 0xc0, 0xff, 0x0b, 0x1d,
  0x30, 0xff, 0x71, 0x77, 0x7d, 0xff, 0xf6, 0xf7, 0xf7, 0xff, 0xf6, 0xf7, 0xf7, 0xff, 0xf6, 0xf7,
  0xf7, 0xff, 0xf6, 0xf7, 0xf7, 0xff, 0xf6, 0xf7, 0xf7, 0xff, 0x43, 0x43, 0x44, 0xff, 0xb7, 0xb8,
  0xb9, 0xff, 0xb5, 0xb6, 0xb6, 0xff, 0x9f, 0x9f, 0xa0, 0xff, 0x9f, 0x9f, 0xa0, 0xff, 0x9f, 0x9f,
  0xa0, 0xff, 0x9f, 0x9f, 0xa0, 0xff, 0xba, 0xba, 0xbb, 0xff, 0xb2, 0xb2, 0xb3, 0xff, 0x48, 0x48,
  0x4a, 0xff, 0xf6, 0xf7, 0xf7, 0xff, 0xf6, 0xf7, 0xf7, 0xff, 0xf6, 0xf7, 0xf7, 0xff, 0xf6, 0xf7,
  0xf7, 0xff, 0xbf, 0xdd, 0xf8, 0xff, 0x9e, 0xce, 0xf9, 0xff, 0xab, 0xac, 0xae, 0xff, 0x1b, 0x1a,
  0x1c, 0xff, 0x4a, 0x49, 0x4b, 0xff, 0x4a, 0x49, 0x4b, 0xff, 0x4a, 0x49, 0x4b, 0xff, 0x4a, 0x49,
  0x4b, 0xff, 0x49, 0x49, 0x4a, 0xff, 0x49, 0x49, 0x4a, 0xff, 0x19, 0x18, 0x1a, 0xff, 0xb4, 0xb5,
  0xb5, 0xff, 0x9a, 0xcc, 0xf9, 0xff, 0xc5, 0xe0, 0xf8, 0xff, 0xf6, 0xf7, 0xf7, 0xff, 0xd2, 0xe6,
  0xf8, 0xff, 0x5e, 0xb0, 0xfa, 0xff, 0xe2, 0xed, 0xf7, 0xff, 0x81, 0xc0, 0xf8, 0xff, 0xce, 0xdb,
  0xe6, 0xff, 0xe5, 0xe5, 0xe6, 0xff, 0xe5, 0xe5, 0xe6, 0xff, 0x55, 0x55, 0x56, 0xff, 0x57, 0x57,
  0x58, 0xff, 0xe5, 0xe5, 0xe6, 0xff, 0xe5, 0xe5, 0xe6, 0xff, 0xc9, 0xd9, 0xe6, 0xff, 0x86, 0xc2,
  0xf9, 0xff, 0xe1, 0xed, 0xf7, 0xff, 0x57, 0xac, 0xfa, 0xff, 0xde, 0xec, 0xf7, 0xff, 0x5a, 0xae,
  0xfa, 0xff, 0xce, 0xe4, 0xf8, 0xff, 0x88, 0xc4, 0xf9, 0xff, 0x98, 0xcb, 0xf9, 0xff, 0xcd, 0xe4,
  0xf8, 0xff, 0x86, 0xc3, 0xf9, 0xff, 0xda, 0xdb, 0xdb, 0xff, 0x38, 0x4a, 0x5c, 0xff, 0x3c, 0x4d,
  0x5f, 0xff, 0xdc, 0xdd, 0xdd, 0xff, 0x81, 0xc0, 0xf9, 0xff, 0xd4, 0xe7, 0xf8, 0xff, 0x91, 0xc8,
  0xf9, 0xff, 0x8d, 0xc6, 0xf9, 0xff, 0xcc, 0xe3, 0xf8, 0xff, 0x5e, 0xb0, 0xfa, 0xff, 0x33, 0x9c,
  0xfb, 0xff, 0xdf, 0xec, 0xf7, 0xff, 0x46, 0xa5, 0xfb, 0xff, 0xf1, 0xf5, 0xf7, 0xff, 0x35, 0x9d,
  0xfb, 0xff, 0xed, 0xf3, 0xf7, 0xff, 0x94, 0x94, 0x95, 0xff, 0x07, 0x54, 0x9c, 0xff, 0x07, 0x51,
  0x95, 0xff, 0x9a, 0x9b, 0x9d, 0xff, 0xed, 0xf2, 0xf7, 0xff, 0x38, 0x9e, 0xfb, 0xff, 0xf1, 0xf5,
  0xf7, 0xff, 0x42, 0xa3, 0xfb, 0xff, 0xe9, 0xf1, 0xf7, 0xff, 0x35, 0x9d, 0xfb, 0xff, 0x34, 0x9c,
  0xfb, 0xff, 0xe5, 0xef, 0xf7, 0xff, 0x55, 0xac, 0xfa, 0xff, 0xe0, 0xed, 0xf7, 0xff, 0x52, 0xaa,
  0xfa, 0xff, 0xcd, 0xe4, 0xf8, 0xff, 0xee, 0xef, 0xef, 0xff, 0x5e, 0x61, 0x65, 0xff, 0x61, 0x64,
  0x69, 0xff, 0xee, 0xef, 0xef, 0xff, 0xcc, 0xe3, 0xf8, 0xff, 0x55, 0xab, 0xfa, 0xff, 0xe0, 0xec,
  0xf7, 0xff, 0x51, 0xaa, 0xfa, 0xff, 0xec, 0xf2, 0xf7, 0xff, 0x35, 0x9d, 0xfb, 0xff, 0x3c, 0xa0,
  0xfb, 0xff, 0xed, 0xf3, 0xf7, 0xff, 0x9b, 0xcd, 0xf9, 0xff, 0x92, 0xc8, 0xf9, 0xff, 0xd3, 0xe6,
  0xf8, 0xff, 0xa5, 0xd1, 0xf9, 0xff, 0xf6, 0xf7, 0xf7, 0xff, 0xf6, 0xf7, 0xf7, 0xff, 0xf6, 0xf7,
  0xf7, 0xff, 0xf6, 0xf7, 0xf7, 0xff, 0xa0, 0xcf, 0xf9, 0xff, 0xd7, 0xe8, 0xf7, 0xff, 0x90, 0xc7,
  0xf9, 0xff, 0x9f, 0xce, 0xf9, 0xff, 0xed, 0xf2, 0xf7, 0xff, 0x3f, 0xa1, 0xfb, 0xff, 0xcf, 0xe5,
  0xf8, 0xff, 0x5d, 0xaf, 0xfa, 0xff, 0xf6, 0xf7, 0xf7, 0xff, 0x8f, 0xc7, 0xf9, 0xff, 0xe2, 0xee,
  0xf7, 0xff, 0xf6, 0xf7, 0xf7, 0xff, 0xf6, 0xf7, 0xf7, 0xff, 0xf6, 0xf7, 0xf7, 0xff, 0xf6, 0xf7,
  0xf7, 0xff, 0xf6, 0xf7, 0xf7, 0xff, 0xf6, 0xf7, 0xf7, 0xff, 0xdf, 0xec, 0xf7, 0xff, 0x92, 0xc8,
  0xf9, 0xff, 0xf6, 0xf7, 0xf7, 0xff, 0x53, 0xab, 0xfa, 0xff, 0xdb, 0xeb, 0xf8, 0xff, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};