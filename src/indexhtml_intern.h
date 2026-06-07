// Not used if index.html exists in data/index.html
// Use data/index.html for development and testing, as overwriting data will erase user settings as well. 
static const char PROGMEM INDEX_HTML[] = R"rawliteral(
<!DOCTYPE html>
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Hackffm ESP32 CAM Bot</title>

  <!-- ============================================================
       GLOBAL STYLES
  ============================================================ -->
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

    /* ── Sliders ── */
    .slider-container { margin-bottom: 10px; }
    .slider-container label { font-weight: bold; color: #8bdcf3; font-size: 14px; }
    .slider-row { display: flex; align-items: center; gap: 10px; margin-top: 4px; }
    input[type=range] { flex: 1; height: 35px; }
    .slider-value { min-width: 55px; font-weight: bold; color: #9b8bf3; text-align: right; }

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

  <!-- ============================================================
       STREAM + JOYSTICK STYLES
       (scoped to #media-wrapper and related elements)
  ============================================================ -->
  <style>
    /* ── Stream area ── */
    #stream-area {
      position: relative;
      display: inline-block;
      width: 640px;
      height: 480px;
      background: #000;
      overflow: hidden;
    }

    #stream-placeholder {
      position: absolute; top: 0; left: 0;
      width: 640px; height: 480px;
      display: block;
    }

    #photo-scale-clip {
      position: absolute; top: 0; left: 0;
      display: inline-block;
      line-height: 0;
      overflow: hidden;
    }

    #photo {
      display: block; max-width: none;
      transform-origin: top left;
      transition: transform 0.2s ease;
    }

    /* joystick wrapper pinned bottom-right in overlay mode */
    #joystick-wrapper {
      position: absolute;
      bottom: 10px; right: 10px;
      display: flex;
      flex-direction: column;
      align-items: center;
      z-index: 20;
      pointer-events: auto;
    }
    #joystick-wrapper.normal-pos {
      position: static;
      margin: 15px auto 0 auto;
      display: flex;
      flex-direction: column;
      align-items: center;
    }

    #joy-coords {
      font-family: monospace;
      font-size: 11px;
      color: #ff0;
      background: rgba(0,0,0,0.5);
      padding: 2px 6px;
      border-radius: 4px;
      margin-bottom: 3px;
      white-space: pre;
    }

    #joystick {
      touch-action: none;
      cursor: crosshair;
    }

    #media-wrapper {
      display: flex;
      justify-content: center;
      align-items: flex-start;
      flex-direction: column;
      align-items: center;
    }

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
  </style>
</head>
<body>

  <h1 id="title">Hackffm ESP32 CAM Bot</h1>

  <!-- ##############################################################
       BLOCK 1 — STREAM AREA + JOYSTICK + STREAM CONTROLS + INFO
       Self-contained visual block. Interacts with the rest of the
       page only through:
         • WRITE: cmd_ml, cmd_mr  (joystick output)
         • READ:  nothing from other blocks at runtime
       Public API used by Gamepad block:
         • setStickFromGamepad(nx, ny)  — mirror gamepad axes into stick
         • triggerStickRelease()        — release stick from gamepad
  ############################################################## -->
  <div id="block-stream">

    <!-- Stream image + joystick canvas -->
    <div id="media-wrapper">
      <div id="stream-area">
        <!-- black grid placeholder shown when no stream is active -->
        <canvas id="stream-placeholder"></canvas>
        <!-- actual stream image -->
        <div id="photo-scale-clip">
          <img id="photo">
        </div>
        <!-- joystick overlay (default: pinned bottom-right of stream-area) -->
        <div id="joystick-wrapper">
          <div id="joy-coords">x:0.00 y:0.00  ml:0 mr:0</div>
          <canvas id="joystick" width="300" height="300"></canvas>
        </div>
      </div>
    </div>

    <!-- Stream controls bar (scale, overlay toggle, start/stop) -->
    <div class="stream-controls">
      <label>
        <input type="checkbox" id="modeToggle" checked>
        Joystick overlay
      </label>
      <label>
        Scale:
        <select id="streamScale">
          <option value="0.5">0.5×</option>
          <option value="1" selected>1×</option>
          <option value="1.5">1.5×</option>
          <option value="2">2×</option>
          <option value="4">4×</option>
        </select>
      </label>
      <button id="btn-stream-toggle" class="running">⏹ Stop stream</button>
    </div>

    <!-- Info line polled from /info -->
    <div id="info"></div>

  </div><!-- end #block-stream -->

  <!-- ##############################################################
       BLOCK 2 — DRIVE & LIGHT CONTROLS
       Writes: cmd_light, cmd_servo1, cmd_servo2
       Reads:  nothing from other blocks
  ############################################################## -->
  <div id="block-drive" class="section">
    <h2>Drive &amp; Light Controls</h2>

    <!-- Light slider -->
    <div class="slider-container">
      <label id="label-light">💡 Light (0–100%)</label>
      <div class="slider-row">
        <input type="range" id="slider-light" min="0" max="100" value="0">
        <div class="slider-value" id="val-light">0%</div>
      </div>
    </div>

    <!-- Servo 1 slider -->
    <div class="slider-container">
      <label id="label-servo1">Servo 1</label>
      <div class="slider-row">
        <input type="range" id="slider-servo1" min="-255" max="255" value="0">
        <div class="slider-value" id="val-servo1">0</div>
      </div>
    </div>

    <!-- Servo 2 slider -->
    <div class="slider-container">
      <label id="label-servo2">Servo 2</label>
      <div class="slider-row">
        <input type="range" id="slider-servo2" min="-255" max="255" value="0">
        <div class="slider-value" id="val-servo2">0</div>
      </div>
    </div>
  </div><!-- end #block-drive -->

  <!-- ##############################################################
       BLOCK 3 — GAMEPAD
       Reads:   nothing from other blocks directly
       Writes (via public API only):
         • setStickFromGamepad(nx, ny)  → Block 1 joystick
         • triggerStickRelease()        → Block 1 joystick
       Future button actions (servo up/down) will call Block 2 API.
  ############################################################## -->
  <div id="block-gamepad" class="section">
    <h2>Gamepad</h2>

    <div class="gpds_row">
      <span style="color:#8bdcf3;font-size:14px;">Gamepad:</span>
      <select id="gpdsel_gamepadSelect"></select>
    </div>

    <div class="gpds_row">
      <button id="searchGamepad">Search gamepad</button>
      <p id="output" style="margin:0">No gamepad detected</p>
    </div>

    <div class="gpds_row">
      <button id="searchGamepadServo1">Set button to toggle Servo 1</button>
      <p id="outputGamepadServo1" style="margin:0">No gamepad button set</p>
    </div>

    <div class="gpds_row">
      <button id="searchGamepadServo2">Set button to toggle Servo 2</button>
      <p id="outputGamepadServo2" style="margin:0">No gamepad button set</p>
    </div>

    <div class="gpds_row">
      <button id="searchGamepadLowLight">Set button to toggle low light</button>
      <p id="outputGamepadLowLight" style="margin:0">No gamepad button set</p>
    </div>

    <div class="gpds_row">
      <button id="searchGamepadFlashLight">Set button to turn on flash light</button>
      <p id="outputGamepadFlashLight" style="margin:0">No gamepad button set</p>
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
  </div><!-- end #block-gamepad -->

  <!-- ##############################################################
       BLOCK 4 — ACTION BUTTONS + FOOTER
       No dependencies on other blocks except triggering /action URLs.
  ############################################################## -->
  <div id="block-actions">

    <div class="action-btn-row">
      <button id="btn-reconnect" class="reconnect-btn">🔄 WiFi Reconnect</button>
      <button id="btn-restart"   class="reconnect-btn">⟳ Restart</button>
      <button id="btn-shutdown"  class="reconnect-btn">⏻ Power off</button>
      <button id="btn-trigger"   class="reconnect-btn">📷 Trigger</button>
      <button id="btn-settings"  class="reconnect-btn">⚙ Settings</button>
    </div>

    <footer>
      <div style="display:flex;gap:8px;align-items:center;justify-content:center;flex-wrap:wrap;">
        <img src="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAADMAAAA/AgMAAAAwDRjCAAAADFBMVEVuAAAoKCjwrV74+Ph5Qa9/AAAAhElEQVQoz6XTSQrAIAwF0F7yr3O6HDG7ljTRUhW+0sGF+CADEd28W/u20oG2/kivszJVKpcWTKSmUy3yABv6dUIRmHAJryUGnynv2bgko4RKJBpY7EReI6MNkZUOTiUSanmDyjxGtZrBkVVw542KMDcq5BVJV7NXnXairOLv9eldP/5HJ2k/9FhNcZTRAAAAAElFTkSuQmCC"
         alt="hackffm.de" style="width:51px;height:63px;"/>
        <span>
          ESP32_CAM_Auto <span id="version-display">V1.00</span> on
          <a href="https://github.com/hackffm/ESP32_CAM_Auto">GitHub</a> |
          <a href="https://www.hackerspace-ffm.de/wiki/index.php?title=FPV-Roboter">&copy; 2026 Hackerspace-FFM e.V.</a>
        </span>
      </div>
    </footer>

  </div><!-- end #block-actions -->


  <!-- ==============================================================
       JAVASCRIPT — split into four matching script blocks
  ============================================================== -->

  <!-- ============================================================
       JS BLOCK 0 — SHARED STATE & NETWORK LAYER
       Owns: cmd_* variables, sendData(), heartbeat, robotState,
             /info polling, applyParsedState().
       Everything else calls sendData() and writes cmd_* here.
  ============================================================ -->
  <script>
  /* ── Command variables (written by all blocks, read by sendData) ── */
  let cmd_ml     = 0;   // left  motor  -255 … +255
  let cmd_mr     = 0;   // right motor  -255 … +255
  let cmd_light  = 0;   // light        0   … 255
  let cmd_servo1 = 0;   // servo 1      -255 … +255
  let cmd_servo2 = 0;   // servo 2      -255 … +255

  /* ── Parsed state from robot ── */
  let robotState = {
    Name: "camBot",
    lightValue: 0,
    lightLowValue: 30,
    lightHighValue: 255,
    lightLimitLowValue: 50,
    lightLimitHighValue: 200,
    lightBoostTime: 30,
    lightBoostTimeMax: 30,
    Servo1Value: 0,
    Servo1LowValue: -30,
    Servo1HighValue: 150,
    Servo1RawValue: 4760,
    Servo2Value: 0,
    Servo2LowValue: -30,
    Servo2HighValue: 150,
    Servo2RawValue: 0,
    Version: "V1.00"
  };

  /* ── Network send ── */
  const UPDATE_INTERVAL = 50;
  let lastSendTime  = 0;
  let retryTimeout  = null;

  let joystickActive   = false;
  let zeroSendCount    = 0;
  const ZERO_SEND_MAX  = 10;
  let heartbeatTimer   = null;

  function _doSend() {
    fetch(`/action?ml=${cmd_ml}&mr=${cmd_mr}&light=${cmd_light}` +
          `&servo1=${cmd_servo1}&servo2=${cmd_servo2}`)
      .catch(() => {});
  }

  function sendData(force = false) {
    const now = Date.now();
    if (!force && (now - lastSendTime) < UPDATE_INTERVAL) {
      if (!retryTimeout) {
        retryTimeout = setTimeout(() => { retryTimeout = null; sendData(false); },
          UPDATE_INTERVAL - (now - lastSendTime));
      }
      return;
    }
    if (retryTimeout) { clearTimeout(retryTimeout); retryTimeout = null; }
    lastSendTime = now;
    _doSend();
  }

  function heartbeatTick() {
    if (joystickActive) {
      _doSend();
    } else {
      if (zeroSendCount < ZERO_SEND_MAX) {
        _doSend();
        zeroSendCount++;
      } else {
        clearInterval(heartbeatTimer);
        heartbeatTimer = null;
      }
    }
  }

  function startHeartbeat() {
    if (heartbeatTimer) return;
    heartbeatTimer = setInterval(heartbeatTick, 1000);
  }

  /* ── /info polling ── */
  const titleElement = document.getElementById("title");
  let roboter_name_set = false;

  function parseTokens(tokenString) {
    const regex = /([A-Za-z0-9]+)="([^"]*)"/g;
    let match;
    while ((match = regex.exec(tokenString)) !== null) {
      const key   = match[1];
      const value = match[2];
      if (robotState.hasOwnProperty(key)) {
        const numVal = parseInt(value);
        robotState[key] = !isNaN(numVal) ? numVal : value;
      }
    }
    applyParsedState();
  }

  function applyParsedState() {
    titleElement.textContent = "HackFFM-Bot: " + robotState.Name;
    document.title           = "HackFFM-Bot: " + robotState.Name;
    roboter_name_set = true;
    document.getElementById("version-display").textContent = robotState.Version;
  }

  async function updateInfo() {
    try {
      const res  = await fetch('/info');
      const txt  = await res.text();
      const parts = txt.split("|");
      document.getElementById("info").textContent = parts[0].trim();
      if (parts.length > 1) parseTokens(parts[1]);
    } catch(e) {}
    setTimeout(updateInfo, 1000);
  }

  /* start info polling immediately */
  updateInfo();
  </script>

  <!-- ============================================================
       JS BLOCK 1 — STREAM AREA + JOYSTICK
       Owns: stream start/stop, placeholder canvas, photo scaling,
             joystick canvas drawing & input, overlay/normal mode.
       Exposes (called by Gamepad block):
         • setStickFromGamepad(nx, ny)
         • triggerStickRelease()
  ============================================================ -->
  <script>
  (function() {   /* IIFE so local vars don't leak */

    /* ── Joystick constants ── */
    const JOY_W        = 300;
    const JOY_H        = 300;
    const STICK_RADIUS = 28;
    let   STICK_COLOR  = "rgba(255,165,0,0.7)";

    const joystickCanvas = document.getElementById("joystick");
    const ctx            = joystickCanvas.getContext("2d");
    let   stick          = { x: 0, y: 0 };

    /* ── Compute motor values from stick position ── */
    function updateCmdFromStick() {
      const nx =  stick.x / (JOY_W / 2);
      const ny = -stick.y / (JOY_H / 2);

      const speed = ny;
      const turn  = nx;
      let   ml    = Math.round((speed + turn) * 255);
      let   mr    = Math.round((speed - turn) * 255);
      ml = Math.max(-255, Math.min(255, ml));
      mr = Math.max(-255, Math.min(255, mr));

      cmd_ml = ml;
      cmd_mr = mr;

      document.getElementById('joy-coords').textContent =
        `x:${nx.toFixed(2)} y:${ny.toFixed(2)}   ml:${cmd_ml} mr:${cmd_mr}`;
    }

    /* ── Draw joystick canvas ── */
    function drawJoystick() {
      ctx.clearRect(0, 0, JOY_W, JOY_H);
      const cx = JOY_W / 2, cy = JOY_H / 2;

      ctx.fillStyle = "rgba(0,0,0,0.35)";
      ctx.fillRect(0, 0, JOY_W, JOY_H);

      ctx.strokeStyle = "rgba(255,255,255,0.4)";
      ctx.lineWidth = 1;
      ctx.strokeRect(0, 0, JOY_W, JOY_H);

      ctx.strokeStyle = "rgba(255,255,255,0.15)";
      ctx.lineWidth = 1;
      ctx.beginPath(); ctx.moveTo(cx, 0);   ctx.lineTo(cx, JOY_H); ctx.stroke();
      ctx.beginPath(); ctx.moveTo(0,  cy);  ctx.lineTo(JOY_W, cy); ctx.stroke();

      ctx.beginPath();
      ctx.arc(cx + stick.x, cy + stick.y, STICK_RADIUS, 0, Math.PI * 2);
      ctx.fillStyle = STICK_COLOR;
      ctx.fill();
      ctx.strokeStyle = "rgba(255,255,255,0.6)";
      ctx.lineWidth = 2;
      ctx.stroke();
    }

    function clampStick(x, y) {
      const hw = JOY_W / 2, hh = JOY_H / 2;
      return {
        x: Math.max(-hw, Math.min(hw, x)),
        y: Math.max(-hh, Math.min(hh, y))
      };
    }

    /* ── Joystick pointer events ── */
    let activePointerId = null;

    function onPointerDown(e) {
      joystickCanvas.setPointerCapture(e.pointerId);
      activePointerId = e.pointerId;
      joystickActive  = true;
      zeroSendCount   = 0;
      STICK_COLOR     = "rgba(255,165,0,0.9)";
      startHeartbeat();

      const rect = joystickCanvas.getBoundingClientRect();
      stick = clampStick(e.clientX - rect.left - JOY_W / 2,
                         e.clientY - rect.top  - JOY_H / 2);
      updateCmdFromStick();
      sendData(true);
      drawJoystick();
    }

    function onPointerMove(e) {
      if (e.pointerId !== activePointerId) return;
      const rect = joystickCanvas.getBoundingClientRect();
      stick = clampStick(e.clientX - rect.left - JOY_W / 2,
                         e.clientY - rect.top  - JOY_H / 2);
      updateCmdFromStick();
      sendData(false);
      drawJoystick();
    }

    function onPointerUp(e) {
      if (e.pointerId !== activePointerId) return;
      activePointerId = null;
      joystickActive  = false;
      zeroSendCount   = 0;
      STICK_COLOR     = "rgba(255,165,0,0.7)";
      stick           = { x: 0, y: 0 };
      cmd_ml = 0; cmd_mr = 0;
      updateCmdFromStick();
      sendData(true);
      drawJoystick();
      startHeartbeat();
    }

    joystickCanvas.addEventListener("pointerdown", onPointerDown);
    joystickCanvas.addEventListener("pointermove", onPointerMove);
    joystickCanvas.addEventListener("pointerup",   onPointerUp);
    joystickCanvas.addEventListener("pointercancel", onPointerUp);

    /* ── Public API for Gamepad block ── */
    let gamepadActivityTimeout = null;

    window.setStickFromGamepad = function(nx, ny) {
      /* nx, ny in [-1, +1]; y-axis: positive = forward */
      if (gamepadActivityTimeout) { clearTimeout(gamepadActivityTimeout); }
      gamepadActivityTimeout = setTimeout(() => {
        gamepadActivityTimeout = null;
        STICK_COLOR = "rgba(255,165,0,0.7)";
        stick = { x: 0, y: 0 };
        updateCmdFromStick();
        sendData(true);
        drawJoystick();
      }, 8000);

      STICK_COLOR = "rgba(255,0,165,0.5)";
      stick = {
        x:  nx * (JOY_W / 2),
        y: -ny * (JOY_H / 2)
      };
      updateCmdFromStick();
      sendData(true);
      drawJoystick();
    };

    window.triggerStickRelease = function() {
      if (gamepadActivityTimeout) { clearTimeout(gamepadActivityTimeout); gamepadActivityTimeout = null; }
      STICK_COLOR = "rgba(255,165,0,0.7)";
      stick = { x: 0, y: 0 };
      updateCmdFromStick();
      sendData(true);
      drawJoystick();
    };

    /* ── Joystick overlay mode toggle ── */
    const joyWrapper  = document.getElementById("joystick-wrapper");
    const streamArea  = document.getElementById("stream-area");

    function updateJoystickMode() {
      const overlay = document.getElementById("modeToggle").checked;
      if (overlay) {
        if (!streamArea.contains(joyWrapper)) streamArea.appendChild(joyWrapper);
        joyWrapper.classList.remove("normal-pos");
        joyWrapper.style.position = "absolute";
      } else {
        const mediaWrapper = document.getElementById("media-wrapper");
        mediaWrapper.appendChild(joyWrapper);
        joyWrapper.classList.add("normal-pos");
        joyWrapper.style.position = "static";
      }
      drawJoystick();
    }
    document.getElementById("modeToggle").addEventListener("change", updateJoystickMode);

    /* ── Stream placeholder ── */
    const placeholder = document.getElementById("stream-placeholder");
    placeholder.width  = 640;
    placeholder.height = 480;
    (function drawPlaceholder() {
      const pc = placeholder.getContext("2d");
      pc.fillStyle = "#000";
      pc.fillRect(0, 0, 640, 480);
      pc.strokeStyle = "#1a1a1a";
      pc.lineWidth = 1;
      for (let x = 0; x <= 640; x += 40) {
        pc.beginPath(); pc.moveTo(x, 0); pc.lineTo(x, 480); pc.stroke();
      }
      for (let y = 0; y <= 480; y += 40) {
        pc.beginPath(); pc.moveTo(0, y); pc.lineTo(640, y); pc.stroke();
      }
      pc.fillStyle = "#333";
      pc.font = "18px monospace";
      pc.textAlign = "center";
      pc.fillText("No stream", 320, 235);
      pc.fillText("640 × 480", 320, 258);
    })();

    /* ── Stream start/stop + scaling ── */
    const photo     = document.getElementById("photo");
    const scaleClip = document.getElementById("photo-scale-clip");

    const TARGET_W = 640;
    const TARGET_H = 480;

    let streamRunning    = false;
    let streamRetryDelay = 1000;

    function applyPhotoScale() {
      const userScale = parseFloat(document.getElementById('streamScale').value);
      const natW = photo.naturalWidth  || TARGET_W;
      const natH = photo.naturalHeight || TARGET_H;

      const scaleToFit = Math.max(TARGET_W / natW, TARGET_H / natH);
      const finalScale = Math.max(scaleToFit, userScale);

      photo.style.transform      = `scale(${finalScale})`;
      photo.style.transformOrigin = "top left";

      const displayW = Math.round(natW * finalScale);
      const displayH = Math.round(natH * finalScale);

      scaleClip.style.width  = displayW + "px";
      scaleClip.style.height = displayH + "px";

      streamArea.style.width  = Math.max(TARGET_W, displayW) + "px";
      streamArea.style.height = Math.max(TARGET_H, displayH) + "px";
    }

    document.getElementById('streamScale').addEventListener('change', applyPhotoScale);

    photo.addEventListener('load', () => {
      placeholder.style.display = "none";
      scaleClip.style.display   = "inline-block";
      streamRetryDelay = 1000;
      applyPhotoScale();
    });
    photo.addEventListener('error', () => {
      scaleClip.style.display   = "none";
      placeholder.style.display = "block";
      if (streamRunning) {
        setTimeout(() => { if (streamRunning) photo.src = `${window.location.origin}:81/stream?t=${Date.now()}`; },
          streamRetryDelay);
        streamRetryDelay = Math.min(streamRetryDelay * 2, 10000);
      }
    });

    const btnStreamToggle = document.getElementById('btn-stream-toggle');

    function startStream() {
      streamRunning = true;
      streamRetryDelay = 1000;
      photo.src = `${window.location.origin}:81/stream?t=${Date.now()}`;
      btnStreamToggle.textContent = '⏹ Stop stream';
      btnStreamToggle.classList.add('running');
    }

    function stopStream() {
      streamRunning = false;
      photo.src = '';
      scaleClip.style.display   = "none";
      placeholder.style.display = "block";
      btnStreamToggle.textContent = '▶ Start stream';
      btnStreamToggle.classList.remove('running');
    }

    btnStreamToggle.addEventListener('click', () => {
      if (streamRunning) stopStream(); else startStream();
    });

    /* auto-start stream */
    startStream();

    /* initial draw */
    drawJoystick();

  })(); /* end IIFE block 1 */
  </script>

  <!-- ============================================================
       JS BLOCK 2 — DRIVE & LIGHT CONTROLS
       Owns: light slider, servo1 slider, servo2 slider.
       Writes: cmd_light, cmd_servo1, cmd_servo2 (shared layer).
  ============================================================ -->
  <script>
  (function() {

    const sliderLight  = document.getElementById("slider-light");
    const sliderServo1 = document.getElementById("slider-servo1");
    const sliderServo2 = document.getElementById("slider-servo2");
    const valLight     = document.getElementById("val-light");
    const valServo1    = document.getElementById("val-servo1");
    const valServo2    = document.getElementById("val-servo2");

    sliderLight.addEventListener("input", () => {
      const pct = parseInt(sliderLight.value);
      valLight.textContent = pct + "%";
      cmd_light = Math.round(pct / 100 * 255);
      sendData(false);
    });

    sliderServo1.addEventListener("input", () => {
      cmd_servo1 = parseInt(sliderServo1.value);
      valServo1.textContent = cmd_servo1;
      sendData(false);
    });

    sliderServo2.addEventListener("input", () => {
      cmd_servo2 = parseInt(sliderServo2.value);
      valServo2.textContent = cmd_servo2;
      sendData(false);
    });

  })(); /* end IIFE block 2 */
  </script>

<!-- ============================================================
       JS BLOCK 3 — GAMEPAD
       Owns: gamepad detection, axis mapping, button mapping.
       Output (only via public API, no direct variable writes):
         • setStickFromGamepad(nx, ny)  → Block 1
         • triggerStickRelease()        → Block 1
  ============================================================ -->
  <script>
  (function() {

    /* ── DOM refs ── */
    const gpdsel_gamepadSelect = document.getElementById('gpdsel_gamepadSelect');
    const gpdsel_xAxisSelect   = document.getElementById('gpdsel_xAxis');
    const gpdsel_yAxisSelect   = document.getElementById('gpdsel_yAxis');
    const gpdsel_invertX       = document.getElementById('gpdsel_invertX');
    const gpdsel_invertY       = document.getElementById('gpdsel_invertY');
    const gpdsel_axesDisplay   = document.getElementById('gpdsel_axesDisplay');
    const gpdsel_xVal          = document.getElementById('gpdsel_xVal');
    const gpdsel_yVal          = document.getElementById('gpdsel_yVal');

    const search_gamepad_button              = document.getElementById('searchGamepad');
    const search_gamepad_output              = document.getElementById('output');
    const search_gamepad_button_servo_1      = document.getElementById('searchGamepadServo1');
    const search_gamepad_output_servo_1      = document.getElementById('outputGamepadServo1');
    const search_gamepad_button_servo_2      = document.getElementById('searchGamepadServo2');
    const search_gamepad_output_servo_2      = document.getElementById('outputGamepadServo2');
    const search_gamepad_button_low_light    = document.getElementById('searchGamepadLowLight');
    const search_gamepad_output_low_light    = document.getElementById('outputGamepadLowLight');
    const search_gamepad_button_flash_light  = document.getElementById('searchGamepadFlashLight');
    const search_gamepad_output_flash_light  = document.getElementById('outputGamepadFlashLight');

    /* ── State ── */
    let selectedGamepadIndex   = null;
    let lastGamepadButtons     = null;
    let servo_1_index          = null;
    let servo_2_index          = null;
    let low_light_index        = null;
    let flash_light_index      = null;
    let gpdsel_activityTimeout = null;
    let searching              = false;
    let searchingServo1        = false;
    let searchingServo2        = false;
    let searchingLowLight      = false;
    let searchingFlashLight    = false;

    /* Flag to suppress saving during initial load */
    let configLoaded           = false;

    function format_fix2(v) { return parseFloat(v).toFixed(2); }

    /* ── Config persistence ──
       Order: xAxisIdx, yAxisIdx, invertX, invertY,
              servo_1_index, servo_2_index, low_light_index, flash_light_index
    */
    function saveConfig() {
      if (!configLoaded) return;  /* don't save while loading */
      const parts = [
        gpdsel_xAxisSelect.value !== '' ? gpdsel_xAxisSelect.value : '0',
        gpdsel_yAxisSelect.value !== '' ? gpdsel_yAxisSelect.value : '0',
        gpdsel_invertX.checked ? '1' : '0',
        gpdsel_invertY.checked ? '1' : '0',
        servo_1_index     !== null ? servo_1_index     : '-1',
        servo_2_index     !== null ? servo_2_index     : '-1',
        low_light_index   !== null ? low_light_index   : '-1',
        flash_light_index !== null ? flash_light_index : '-1'
      ];
      const url = '/action?configDataWrite=0,' + parts.join(',');
      fetch(url).catch(err => console.warn('configDataWrite failed:', err));
    }

    function loadConfig() {
      fetch('/action?configDataRead=0')
        .then(r => r.text())
        .then(txt => {
          const parts = txt.trim().split(',');
          if (parts.length >= 8) {
            const xAxis    = parseInt(parts[0]);
            const yAxis    = parseInt(parts[1]);
            const invX     = parts[2] === '1';
            const invY     = parts[3] === '1';
            const s1       = parseInt(parts[4]);
            const s2       = parseInt(parts[5]);
            const lowL     = parseInt(parts[6]);
            const flashL   = parseInt(parts[7]);

            if (!isNaN(xAxis)) gpdsel_xAxisSelect.value = xAxis;
            if (!isNaN(yAxis)) gpdsel_yAxisSelect.value = yAxis;
            gpdsel_invertX.checked = invX;
            gpdsel_invertY.checked = invY;

            if (!isNaN(s1) && s1 >= 0) {
              servo_1_index = s1;
              search_gamepad_output_servo_1.textContent = 'Button to toggle Servo 1: ' + s1;
            }
            if (!isNaN(s2) && s2 >= 0) {
              servo_2_index = s2;
              search_gamepad_output_servo_2.textContent = 'Button to toggle Servo 2: ' + s2;
            }
            if (!isNaN(lowL) && lowL >= 0) {
              low_light_index = lowL;
              search_gamepad_output_low_light.textContent = 'Button to toggle Low Light: ' + lowL;
            }
            if (!isNaN(flashL) && flashL >= 0) {
              flash_light_index = flashL;
              search_gamepad_output_flash_light.textContent = 'Button to turn on Flash Light: ' + flashL;
            }
          }
        })
        .catch(err => console.warn('configDataRead failed:', err))
        .finally(() => { configLoaded = true; });
    }

    /* Trigger save when axis/invert UI controls change */
    gpdsel_xAxisSelect.addEventListener('change', saveConfig);
    gpdsel_yAxisSelect.addEventListener('change', saveConfig);
    gpdsel_invertX.addEventListener('change',     saveConfig);
    gpdsel_invertY.addEventListener('change',     saveConfig);

    /* ── Rebuild the gamepad dropdown from currently connected gamepads ── */
    function rebuildGamepadDropdown() {
      const gamepads = navigator.getGamepads();
      gpdsel_gamepadSelect.innerHTML = '<option value="">-- select --</option>';
      for (let i = 0; i < gamepads.length; i++) {
        if (gamepads[i]) {
          const opt = document.createElement('option');
          opt.value = i;
          opt.textContent = gamepads[i].id || ('Gamepad ' + i);
          if (i === selectedGamepadIndex) opt.selected = true;
          gpdsel_gamepadSelect.appendChild(opt);
        }
      }
    }

    /* User picks from dropdown manually */
    gpdsel_gamepadSelect.addEventListener('change', () => {
      const v = gpdsel_gamepadSelect.value;
      if (v === '') {
        selectedGamepadIndex = null;
        search_gamepad_output.textContent = 'No gamepad selected';
      } else {
        selectedGamepadIndex = parseInt(v);
        const gp = navigator.getGamepads()[selectedGamepadIndex];
        search_gamepad_output.textContent = 'Selected: ' + (gp ? gp.id : 'Gamepad ' + selectedGamepadIndex);
      }
      /* NOTE: gamepad index/name intentionally NOT persisted */
    });

    /* Keep dropdown fresh when browser adds/removes gamepads */
    window.addEventListener('gamepadconnected',    rebuildGamepadDropdown);
    window.addEventListener('gamepaddisconnected', rebuildGamepadDropdown);

    /* ── Main gamepad poll loop ── */
    function gamepadLoop() {
      const gamepads = navigator.getGamepads();
      if (selectedGamepadIndex === null) { requestAnimationFrame(gamepadLoop); return; }

      const gp = gamepads[selectedGamepadIndex];
      if (!gp)                           { requestAnimationFrame(gamepadLoop); return; }

      /* update axes display */
      const axes = gp.axes;
      gpdsel_axesDisplay.textContent = axes.map((v, i) => 'A' + i + ':' + format_fix2(v)).join('  ');

      /* read selected axes */
      let gx = axes[parseInt(gpdsel_xAxisSelect.value)] || 0;
      let gy = axes[parseInt(gpdsel_yAxisSelect.value)] || 0;
      if (gpdsel_invertX.checked) gx *= -1;
      if (gpdsel_invertY.checked) gy *= -1;
      gpdsel_xVal.textContent = format_fix2(gx);
      gpdsel_yVal.textContent = format_fix2(gy);

      /* activity detection → forward to Block 1 via public API */
      if (Math.abs(gx) > 0.1 || Math.abs(gy) > 0.1) {
        if (!gpdsel_activityTimeout) {
          gpdsel_activityTimeout = setTimeout(() => {
            gpdsel_activityTimeout = null;
            window.triggerStickRelease();
          }, 8000);
        }
      }

      if (gpdsel_activityTimeout !== null) {
        window.setStickFromGamepad(gx, gy);
      }

      /* button handling */
      const buttons = gp.buttons;
      if (lastGamepadButtons) {
        for (let i = 0; i < buttons.length; i++) {
          const pressed    = buttons[i].pressed;
          const wasPressed = lastGamepadButtons[i];
          if (pressed && !wasPressed) {
            if (i === servo_1_index) {
              /* TODO: call Block 2 public API, e.g. adjustServo1(+step) */
            }
            if (i === servo_2_index) {
              /* TODO: call Block 2 public API, e.g. adjustServo1(-step) */
            }
            if (i === low_light_index) {
              /* TODO: call Block 2 public API, e.g. toggleLowLight() */
            }
            if (i === flash_light_index) {  
              /* TODO: call Block 2 public API, e.g. toggleFlashLight() */
            }
          }
        }
      }
      lastGamepadButtons = buttons.map(b => b.pressed);

      requestAnimationFrame(gamepadLoop);
    }
    requestAnimationFrame(gamepadLoop);

    /* ── Gamepad search — press any button to auto-detect & select ── */
    search_gamepad_button.addEventListener('click', () => {
      searching = true;
      search_gamepad_output.textContent = 'Press any button on the gamepad...';
      requestAnimationFrame(checkGamepad);
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
            search_gamepad_output.textContent = 'Found: ' + gp.id;
            rebuildGamepadDropdown();          /* update dropdown to show & select it */
            /* NOTE: gamepad index/name intentionally NOT persisted */
            return;
          }
        }
      }
      requestAnimationFrame(checkGamepad);
    }

    /* ── Servo 1 search ── */
    search_gamepad_button_servo_1.addEventListener('click', () => {
      if (selectedGamepadIndex === null) {
        search_gamepad_output_servo_1.textContent = 'Select a gamepad first!'; return;
      }
      searchingServo1 = true;
      search_gamepad_output_servo_1.textContent = 'Press the button to toggle Servo 1...';
      requestAnimationFrame(checkGamepadServo1);
    });

    function checkGamepadServo1() {
      if (!searchingServo1) return;
      const gp = navigator.getGamepads()[selectedGamepadIndex]; if (!gp) return;
      for (let j = 0; j < gp.buttons.length; j++) {
        if (gp.buttons[j].pressed) {
          searchingServo1 = false; servo_1_index = j;
          search_gamepad_output_servo_1.textContent = 'Button to toggle Servo 1: ' + j;
          saveConfig();
          return;
        }
      }
      requestAnimationFrame(checkGamepadServo1);
    }

    /* ── Servo 2 search ── */
    search_gamepad_button_servo_2.addEventListener('click', () => {
      if (selectedGamepadIndex === null) {
        search_gamepad_output_servo_2.textContent = 'Select a gamepad first!'; return;
      }
      searchingServo2 = true;
      search_gamepad_output_servo_2.textContent = 'Press the button to toggle Servo 2...';
      requestAnimationFrame(checkGamepadServo2);
    });

    function checkGamepadServo2() {
      if (!searchingServo2) return;
      const gp = navigator.getGamepads()[selectedGamepadIndex]; if (!gp) return;
      for (let j = 0; j < gp.buttons.length; j++) {
        if (gp.buttons[j].pressed) {
          searchingServo2 = false; servo_2_index = j;
          search_gamepad_output_servo_2.textContent = 'Button to toggle Servo 2: ' + j;
          saveConfig();
          return;
        }
      }
      requestAnimationFrame(checkGamepadServo2);
    }

    /* ── Low Light search ── */
    search_gamepad_button_low_light.addEventListener('click', () => {
      if (selectedGamepadIndex === null) {
        search_gamepad_output_low_light.textContent = 'Select a gamepad first!'; return;
      }
      searchingLowLight = true;
      search_gamepad_output_low_light.textContent = 'Press the button to toggle Low Light...';
      requestAnimationFrame(checkGamepadLowLight);
    });

    function checkGamepadLowLight() {
      if (!searchingLowLight) return;
      const gp = navigator.getGamepads()[selectedGamepadIndex]; if (!gp) return;
      for (let j = 0; j < gp.buttons.length; j++) {
        if (gp.buttons[j].pressed) {
          searchingLowLight = false; low_light_index = j;
          search_gamepad_output_low_light.textContent = 'Button to toggle Low Light: ' + j;
          saveConfig();
          return;
        }
      }
      requestAnimationFrame(checkGamepadLowLight);
    }

    /* ── Flash Light search ── */
    search_gamepad_button_flash_light.addEventListener('click', () => {
      if (selectedGamepadIndex === null) {
        search_gamepad_output_flash_light.textContent = 'Select a gamepad first!'; return;
      }
      searchingFlashLight = true;
      search_gamepad_output_flash_light.textContent = 'Press the button to turn on Flash Light...';
      requestAnimationFrame(checkGamepadFlashLight);
    });

    function checkGamepadFlashLight() {
      if (!searchingFlashLight) return;
      const gp = navigator.getGamepads()[selectedGamepadIndex]; if (!gp) return;
      for (let j = 0; j < gp.buttons.length; j++) {
        if (gp.buttons[j].pressed) {
          searchingFlashLight = false; flash_light_index = j;
          search_gamepad_output_flash_light.textContent = 'Button to turn on Flash Light: ' + j;
          saveConfig();
          return;
        }
      }
      requestAnimationFrame(checkGamepadFlashLight);
    }

    /* ── Load persisted config on page load ── */
    loadConfig();

  })(); /* end IIFE block 3 */
  </script>



  <!-- ============================================================
       JS BLOCK 4 — ACTION BUTTONS
       No dependencies on other blocks; fires simple /action fetches.
  ============================================================ -->
  <script>
  (function() {

    document.getElementById('btn-reconnect').addEventListener('click', () => {
      fetch('/action?reconnect=1');
    });
    document.getElementById('btn-restart').addEventListener('click', () => {
      fetch('/action?restart=1');
    });
    document.getElementById('btn-shutdown').addEventListener('click', () => {
      window.location.href = '/action?shutdown';
    });
    document.getElementById('btn-trigger').addEventListener('click', () => {
      window.location.href = '/action?trig';
    });
    document.getElementById('btn-settings').addEventListener('click', () => {
      window.location.href = '/settings';
    });

  })(); /* end IIFE block 4 */
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