// added stream auto-reconnect, dynamic labels for sliders, joystick movement 3
// Joystick nun nur über dem Bild und kleiner
static const char PROGMEM INDEX_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32-CAM Robot</title>

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
      background: #080505;
      color: white;
      text-align: center;
    }

    h1 {
      margin: 4px 0;
      color: #ea903c;
    }

    /* Make photo-container the positioning context for absolute joystick overlay */
    #photo-container {
      position: relative;
      display: inline-block;
      max-width: 100%;
    }

    #media-wrapper {
      display: flex;
      justify-content: center;
      align-items: center;   
      gap: 20px;
      flex-wrap: wrap;       
    }

    /* Side-by-side mode for larger screens */
    .side-mode {
      flex-direction: row;
    }

    /* Normal stacked mode */
    .normal-mode {
      flex-direction: column;
    }

    /* Joystick base styles */
    #joystick {
      margin: 0;
      touch-action: none;
      transition: all 0.2s ease;
      z-index: 10;
    }

    /* Joystick in normal mode - centered below image */
    .normal-mode #joystick {
      display: block;
      margin: 15px auto;
      width: 300px;
      height: 300px;
    }

    /* Joystick overlay mode - absolute positioned over image */
    .side-mode #joystick {
      position: absolute;
      right: 10px;
      bottom: 10px;
      /* Size calculated dynamically in JS, but start with reasonable size */
      width: 210px;
      height: 210px;
      pointer-events: auto;
    }

    #photo {
      display: block;
      max-width: 100%;
      height: auto;
    }

    #info {
      font-family: monospace;
      margin-top: 6px;
      color: #aaa;
      white-space: pre-wrap;
    }

    .mode-switch {
      margin: 10px 0;
    }

    .sliders-wrapper {
      display: flex;
      flex-direction: column;
      gap: 8px;
      width: 95%;
      max-width: 450px;
      margin: 15px auto;
      text-align: left;
    }

    .slider-container label {
      font-weight: bold;
      color: #8bdcf3;
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
      color: #9b8bf3;
    }
  </style>
</head>

<body>
  <h1 id="title">ESP32-CAM Robot</h1>

  <div id="media-wrapper" class="normal-mode">
    <div id="photo-container">
      <img id="photo">
      <!-- Joystick now inside photo-container for proper absolute positioning -->
      <canvas id="joystick" width="300" height="300"></canvas>
    </div>
  </div>

  <div class="mode-switch">
    <label>
      <input type="checkbox" id="modeToggle">
      Joystick overlay (right aligned)
    </label>
  </div>

  <div id="info"></div>

  <div class="sliders-wrapper" id="sliders"></div>

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
    const titleElement = document.getElementById("title");

    let stick = { x: 0, y: 0 };
    let dragging = false;
    let lastSendTime = 0;

    let sliderValues = { A:0, B:0, C:0, D:0, E:0 };

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

      const value = document.createElement("div");
      value.className = "slider-value";
      value.textContent = "0";

      input.addEventListener("input", () => {
        sliderValues[letter] = parseInt(input.value);
        value.textContent = input.value;
        sendData(true);
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

      /* Outer circle (1px) */
      ctx.beginPath();
      ctx.arc(center.x, center.y, LIMIT_RADIUS, 0, Math.PI*2);
      ctx.strokeStyle = "white";
      ctx.lineWidth = 1;
      ctx.stroke();

      /* Semi-transparent knob */
      ctx.beginPath();
      ctx.arc(center.x + stick.x,
              center.y + stick.y,
              STICK_RADIUS,
              0,
              Math.PI*2);
      ctx.fillStyle = "rgba(255,165,0,0.3)";
      ctx.fill();
    }

    /* ============= JOYSTICK POSITION MODE ============= */
    function updateJoystickMode() {
      const wrapper = document.getElementById("media-wrapper");
      const overlayActive = document.getElementById("modeToggle").checked;

      if (!overlayActive) {
        /* Normal mode: joystick below image */
        wrapper.classList.remove("side-mode");
        wrapper.classList.add("normal-mode");

        canvas.style.position = "static";
        canvas.style.width = "300px";
        canvas.style.height = "300px";

      } else {
        /* Overlay mode: joystick over image, bottom-right corner */
        wrapper.classList.remove("normal-mode");
        wrapper.classList.add("side-mode");

        /* Calculate responsive size based on image/container dimensions */
        const containerRect = document.getElementById("photo-container").getBoundingClientRect();
        const availableWidth = containerRect.width;
        const size = Math.min(availableWidth * 0.35, 250, window.innerWidth * 0.4); // Responsive size

        canvas.style.position = "absolute";
        canvas.style.width = size + "px";
        canvas.style.height = size + "px";
      }

      /* Redraw joystick after resize */
      drawJoystick();
    }

    window.addEventListener("resize", updateJoystickMode);
    photo.onload = updateJoystickMode;
    photo.onerror = updateJoystickMode; // Also trigger on error

    /* ============= STICK HANDLING ============= */
    function updateStick(clientX, clientY) {
      const rect = canvas.getBoundingClientRect();
      const scaleX = canvas.width / rect.width;
      const scaleY = canvas.height / rect.height;

      const xPos = (clientX - rect.left) * scaleX;
      const yPos = (clientY - rect.top) * scaleY;

      const center = { x: canvas.width/2, y: canvas.height/2 };

      let x = xPos - center.x;
      let y = yPos - center.y;

      const dist = Math.hypot(x, y);

      if (dist > LIMIT_RADIUS) {
        x = x * LIMIT_RADIUS / dist;
        y = y * LIMIT_RADIUS / dist;
      }

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
    function calculateAxis(x, y) {
      const dist = Math.hypot(x, y);
      if (dist <= DEADZONE_RADIUS) return { x:0, y:0 };

      const scale = (dist - DEADZONE_RADIUS) /
                    (LIMIT_RADIUS - DEADZONE_RADIUS);

      return {
        x: Math.round((x / dist) * scale * MAX_VALUE),
        y: Math.round((y / dist) * -scale * MAX_VALUE)
      };
    }

    /* ============= SEND DATA ============= */
    function sendData(force=false) {
      const now = Date.now();
      if (!force && now - lastSendTime < UPDATE_INTERVAL) return;
      lastSendTime = now;

      const axis = calculateAxis(stick.x, stick.y);

      fetch(`/action?x=${axis.x}&y=${axis.y}` +
            `&a=${sliderValues.A}` +
            `&b=${sliderValues.B}` +
            `&c=${sliderValues.C}` +
            `&d=${sliderValues.D}` +
            `&e=${sliderValues.E}`)
            .catch(()=>{});
    }

    /* ============= TOKEN PARSER ============= */
    function parseTokens(tokenString) {
      const regex = /([A-E]|Name)="([^"]*)"/g;
      let match;

      while ((match = regex.exec(tokenString)) !== null) {
        const key = match[1];
        const value = match[2];

        if (key === "Name") {
          titleElement.textContent = value;
        } else {
          const labelElement = document.getElementById("label"+key);
          if (labelElement)
            labelElement.textContent = value;
        }
      }
    }

    /* ============= INFO UPDATE ============= */
    async function updateInfo() {
      try {
        const res = await fetch('/info');
        const txt = await res.text();

        const parts = txt.split("|");

        const visibleText = parts[0].trim();
        document.getElementById("info").textContent = visibleText;

        if (parts.length > 1) {
          parseTokens(parts[1]);
        }
      } catch(e) {}

      setTimeout(updateInfo, 1000);
    }

    /* ============= STREAM ============= */
    function loadStream() {
      photo.src = `${window.location.origin}:81/stream?t=${Date.now()}`;
    }

    photo.onerror = () => setTimeout(loadStream, 2000);
    setInterval(loadStream, 60000);

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
    }, { passive:false });

    canvas.addEventListener("touchmove", e => {
      e.preventDefault();
      if (dragging)
        updateStick(e.touches[0].clientX, e.touches[0].clientY);
    }, { passive:false });

    canvas.addEventListener("touchend", resetStick);

    /* ============= INIT ============= */
    document.getElementById("modeToggle")
            .addEventListener("change", updateJoystickMode);

    drawJoystick();
    loadStream();
    updateInfo();
  </script>
</body>
</html>
)rawliteral";