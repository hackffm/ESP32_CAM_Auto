// Not used if index.html exists in data/index.html
// Use data/index.html for development and testing, as overwriting data will erase user settings as well. 
static const char PROGMEM SETTINGS_HTML[] = R"rawliteral(
<!DOCTYPE html>
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Bot Settings</title>
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

    .form-row {
      display: flex; align-items: center; gap: 10px;
      flex-wrap: nowrap; margin-bottom: 10px; }

    .form-row label {
      white-space: nowrap; min-width: 160px;
      color: #8bdcf3; font-size: 14px; }

    .form-row input[type="text"],
    .form-row input[type="password"] {
      flex: 1; min-width: 0; font-size: 14px;
      padding: 4px 6px; background: #1a1a1a;
      border: 1px solid #555; border-radius: 4px; color: white; }

    .form-row button, .form-row input[type="submit"] {
      white-space: nowrap; padding: 5px 12px;
      font-size: 13px; border: none; border-radius: 4px;
      background-color: #333; color: #8bdcf3;
      cursor: pointer; }
    .form-row button:hover, .form-row input[type="submit"]:hover {
      background-color: #555; }

    /* password toggle button — sits flush after the input */
    #btn_pw_toggle {
      padding: 5px 9px; font-size: 15px; line-height: 1;
      border: none; border-radius: 4px;
      background-color: #333; color: #8bdcf3;
      cursor: pointer; flex-shrink: 0; }
    #btn_pw_toggle:hover { background-color: #555; }

    /* ── WiFi scan table ── */
    #wifi_scan_results {
      display: none; width: 100%;
      background: #1a1a1a; border: 1px solid #555;
      border-radius: 4px; max-height: 280px; overflow-y: auto;
      margin-top: 4px; box-sizing: border-box; }

    #wifi_scan_results table {
      width: 100%; border-collapse: collapse;
      table-layout: fixed; font-size: 13px; }

    #wifi_scan_results thead tr { background: #252525; }

    #wifi_scan_results th,
    #wifi_scan_results td {
      padding: 6px 8px; text-align: left;
      border-bottom: 1px solid #2a2a2a;
      overflow: hidden; text-overflow: ellipsis; white-space: nowrap; }

    #wifi_scan_results th {
      color: #8bdcf3; font-size: 11px;
      text-transform: uppercase; letter-spacing: 0.04em; }

    #wifi_scan_results col.col-ssid  { width: 38%; }
    #wifi_scan_results col.col-bssid { width: 30%; }
    #wifi_scan_results col.col-rssi  { width: 16%; }
    #wifi_scan_results col.col-enc   { width: 16%; }

    #wifi_scan_results tbody tr { cursor: pointer; }
    #wifi_scan_results tbody tr:hover { background: #2a2a2a; }
    #wifi_scan_results td.td-ssid  { color: white; font-weight: bold; }
    #wifi_scan_results td.td-bssid { color: #7ec8e3; font-family: monospace; font-size: 11px; }
    #wifi_scan_results td.td-rssi  { color: #aaa; }
    #wifi_scan_results td.td-enc   { color: #f0ad5e; }

    #scan-status { font-size: 12px; color: #aaa; margin-top: 4px; min-height: 16px; }

    /* Restart hint */
    #wifi_restart_hint {
      display: none; margin-top: 10px; padding: 10px 14px;
      background: #1a1200; border: 1px solid #f0ad5e;
      border-radius: 6px; font-size: 13px; color: #f0ad5e; }
    #wifi_restart_hint .hint-buttons {
      display: flex; gap: 10px; margin-top: 8px; flex-wrap: wrap; }
    #wifi_restart_hint button {
      padding: 5px 14px; font-size: 13px; border: none;
      border-radius: 4px; cursor: pointer; }
    #btn_do_restart   { background: #c0392b; color: white; }
    #btn_do_restart:hover   { background: #e74c3c; }
    #btn_dismiss_hint { background: #444; color: #ccc; }
    #btn_dismiss_hint:hover { background: #666; }

    /* ── Camera config ── */
    .cam-grid {
      display: grid;
      grid-template-columns: max-content 1fr;
      gap: 8px 14px;
      align-items: center;
      max-width: 420px; }

    .cam-grid label {
      color: #8bdcf3; font-size: 14px; white-space: nowrap; }

    .cam-grid select {
      font-size: 13px; background: #1a1a1a;
      border: 1px solid #555; border-radius: 4px;
      color: white; padding: 3px 6px; width: 100%; }

    #cam_status { font-size: 12px; color: #aaa; margin-top: 8px; min-height: 16px; }

    /* ── PT (motor/servo) config ── */
    .pt_container {
      border: 1px solid #444; border-radius: 6px;
      padding: 10px; margin-bottom: 10px; }

    .pt_title {
      font-weight: bold; color: #f0ad5e;
      margin-bottom: 8px; font-size: 14px; }

    /*
      Strict 4-column grid: [label | input | label | input]
      All four columns are uniform — no span tricks.
    */
    .pt_grid {
      display: grid;
      grid-template-columns: max-content minmax(80px,1fr) max-content minmax(80px,1fr);
      gap: 6px 14px;
      align-items: center; }

    .pt_grid label {
      color: #8bdcf3; font-size: 12px;
      white-space: nowrap; text-align: right; }

    .pt_grid select,
    .pt_grid input[type="number"] {
      width: 100%; font-size: 12px;
      background: #1a1a1a; border: 1px solid #555;
      border-radius: 3px; color: white; padding: 2px 4px;
      box-sizing: border-box; }

    .pt_grid input[type="checkbox"] { width: auto; justify-self: start; }

    .pt_buttons {
      display: flex; gap: 10px; justify-content: center; margin-top: 10px; }

    .pt_buttons button {
      padding: 6px 20px; font-size: 13px; border: none;
      border-radius: 4px; background-color: #333;
      color: #8bdcf3; cursor: pointer; }
    .pt_buttons button:hover { background-color: #555; }

    #pt_status {
      font-size: 12px; color: #aaa;
      text-align: center; margin-top: 6px; min-height: 16px; }

    /* System */
    .danger-btn {
      padding: 7px 18px; font-size: 13px; border: none;
      border-radius: 4px; cursor: pointer; margin: 4px; }
    #btn_restart   { background: #c0392b; color: white; }
    #btn_restart:hover   { background: #e74c3c; }
    #btn_reconnect { background: #1a5276; color: #7ec8e3; }
    #btn_reconnect:hover { background: #2471a3; }

    .back-btn {
      display: inline-block; margin: 12px 6px; padding: 10px 22px;
      font-size: 16px; border: none; border-radius: 6px;
      background-color: #444; color: #f0ad5e; cursor: pointer;
      text-decoration: none; }
    .back-btn:hover { background-color: #666; }

    footer {
      margin-top: 30px; padding-top: 10px;
      border-top: 1px solid #333; }

    a:link    { text-decoration: none; color: #f0ad5e; }
    a:visited { text-decoration: none; color: #f0ad5e; }
    a:hover   { text-decoration: underline; }
    a:active  { text-decoration: underline; }
  </style>
</head>
<body>
  <h1 id="title">Bot Settings</h1>

  <a href="/" class="back-btn">← Back to Control</a>

  <!-- ── Bot Name ── -->
  <div class="section">
    <h2>Bot Name</h2>
    <form action="/action" method="GET">
      <div class="form-row">
        <label for="roboter_name">Bot Name:</label>
        <input type="text" id="roboter_name" name="roboter_name">
        <input type="submit" value="Save Name">
      </div>
    </form>
  </div>

  <!-- ── WiFi Configuration ── -->
  <div class="section">
    <h2>WiFi Configuration</h2>

    <div class="form-row">
      <label for="wifi_ssid">WiFi Name (SSID):</label>
      <input type="text" id="wifi_ssid" autocomplete="off"
             placeholder="Select via scan or type manually">
      <button type="button" id="btn_wifi_scan">🔍 Scan</button>
    </div>

    <div id="wifi_scan_results"></div>
    <div id="scan-status"></div>

    <div class="form-row" style="margin-top:10px;">
      <label for="wifi_password">Password:</label>
      <input type="password" id="wifi_password" autocomplete="off"
             placeholder="Leave blank to keep current">
      <button type="button" id="btn_pw_toggle" title="Show / hide password">👁</button>
    </div>

    <div class="form-row">
      <label></label>
      <button type="button" id="btn_wifi_save">💾 Save WiFi</button>
    </div>

    <div id="wifi_restart_hint">
      ⚠️ <strong>WiFi settings saved.</strong>
      The new settings will only take effect after a restart or power cycle.
      <div class="hint-buttons">
        <button id="btn_do_restart">🔄 Restart now</button>
        <button id="btn_dismiss_hint">Dismiss</button>
      </div>
    </div>
  </div>

  <!-- ── Camera Configuration ── -->
  <div class="section">
    <h2>Camera Configuration</h2>
    <div class="cam-grid">

      <label for="cam_rotation">Rotation:</label>
      <select id="cam_rotation">
        <option value="0">0°</option>
        <option value="1">180°</option>
      </select>

      <label for="cam_size">Picture Size:</label>
      <select id="cam_size">
        <option value="0">320×240</option>
        <option value="1">640×480</option>
        <option value="2">800×600</option>
        <option value="3">1024×768</option>
        <option value="4">1280×1024</option>
      </select>

      <label for="cam_fps">FPS Limit:</label>
      <select id="cam_fps">
        <option value="0">auto</option>
        <option value="1">2 fps</option>
        <option value="2">5 fps</option>
        <option value="3">10 fps</option>
        <option value="4">15 fps</option>
        <option value="5">20 fps</option>
        <option value="6">25 fps</option>
        <option value="7">30 fps</option>
      </select>

      <label for="cam_quality">Compression Quality:</label>
      <select id="cam_quality">
        <option value="0">auto</option>
        <option value="1">very poor</option>
        <option value="2">poor</option>
        <option value="3">medium</option>
        <option value="4">good</option>
        <option value="5">very good</option>
      </select>

    </div>
    <div id="cam_status"></div>
  </div>

  <!-- ── Motor / Servo Pin Config ── -->
  <div class="section">
    <h2>Motor and Servo Pin Configuration</h2>
    <p style="font-size:12px;color:#aaa;margin:0 0 10px 0;">
      Settings are loaded automatically on page open.
      Click Store to apply changes.</p>
    <div id="pt_configs"></div>
    <div id="pt_status">Loading…</div>
    <div class="pt_buttons">
      <button onclick="pt_retrievePwmThings()">⬇ Retrieve</button>
      <button onclick="pt_storePwmThings()">⬆ Store</button>
    </div>
  </div>

  <!-- ── System ── -->
  <div class="section">
    <h2>System</h2>
    <div style="display:flex;gap:10px;flex-wrap:wrap;align-items:center;">
      <button id="btn_reconnect" class="danger-btn">📶 WiFi Reconnect</button>
      <button id="btn_restart"   class="danger-btn">🔄 Restart</button>
    </div>
  </div>

  <a href="/" class="back-btn">← Back to Control</a>

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
  /* ═══════════════════════════════════════════════
     TITLE / BOT NAME
  ═══════════════════════════════════════════════ */
  async function updateTitle() {
    try {
      const res  = await fetch('/info');
      const txt  = await res.text();
      const parts = txt.split("|");
      if (parts.length > 1) {
        const match = /Name="([^"]*)"/.exec(parts[1]);
        if (match) {
          document.getElementById('title').textContent = "Settings: " + match[1];
          document.title = "Settings: " + match[1];
          const f = document.getElementById('roboter_name');
          if (f && !f.dataset.filled) { f.value = match[1]; f.dataset.filled = "1"; }
        }
      }
    } catch(e) {}
  }

  /* ═══════════════════════════════════════════════
     PASSWORD VISIBILITY TOGGLE
  ═══════════════════════════════════════════════ */
  const pwInput     = document.getElementById('wifi_password');
  const btnPwToggle = document.getElementById('btn_pw_toggle');

  btnPwToggle.addEventListener('click', () => {
    const show = pwInput.type === 'password';
    pwInput.type          = show ? 'text' : 'password';
    btnPwToggle.textContent = show ? '🙈' : '👁';
    btnPwToggle.title       = show ? 'Hide password' : 'Show password';
  });

  /* ═══════════════════════════════════════════════
     WIFI SCAN
  ═══════════════════════════════════════════════ */
  /*
    GET /action?wifi_scan  →  JSON array sorted by RSSI descending:
    [
      {
        "ssid":   "MyNetwork",
        "bssid":  "AA:BB:CC:DD:EE:FF",
        "rssi":   -55,
        "auth":   3          // wifi_auth_mode_t:
                             //  0=OPEN 1=WEP 2=WPA_PSK 3=WPA2_PSK
                             //  4=WPA_WPA2_PSK 5=WPA2_ENTERPRISE
                             //  6=WPA3_PSK 7=WPA2_WPA3_PSK 8=WAPI_PSK
      }, …
    ]

    ESP32 Arduino example:
      int n = WiFi.scanNetworks();
      String j = "[";
      for (int i = 0; i < n; i++) {
        if (i) j += ",";
        j += "{\"ssid\":\""  + WiFi.SSID(i)    + "\"";
        j += ",\"bssid\":\"" + WiFi.BSSIDstr(i) + "\"";
        j += ",\"rssi\":"    + String(WiFi.RSSI(i));
        j += ",\"auth\":"    + String((int)WiFi.encryptionType(i));
        j += "}";
      }
      j += "]";
      request->send(200, "application/json", j);
  */

  const btnScan       = document.getElementById('btn_wifi_scan');
  const scanResults   = document.getElementById('wifi_scan_results');
  const scanStatus    = document.getElementById('scan-status');
  const wifiSsidInput = document.getElementById('wifi_ssid');
  const btnWifiSave   = document.getElementById('btn_wifi_save');
  const restartHint   = document.getElementById('wifi_restart_hint');

  function authLabel(auth) {
    switch (Number(auth)) {
      case 0: return '🔓 Open';
      case 1: return '🔒 WEP';
      case 2: return '🔒 WPA';
      case 3: return '🔒 WPA2';
      case 4: return '🔒 WPA/WPA2';
      case 5: return '👤 WPA2-ENT';
      case 6: return '🔒 WPA3';
      case 7: return '🔒 WPA2/WPA3';
      case 8: return '👤 WAPI';
      default: return '? Unknown';
    }
  }

  btnScan.addEventListener('click', async () => {
    scanResults.style.display = 'none';
    scanResults.innerHTML     = '';
    scanStatus.textContent    = '⏳ Scanning…';
    btnScan.disabled          = true;

    try {
      const res  = await fetch('/action?wifi_scan');
      const list = await res.json();

      if (!Array.isArray(list) || list.length === 0) {
        scanStatus.textContent = 'No networks found.';
        btnScan.disabled = false;
        return;
      }

      const tbl = document.createElement('table');

      const colgroup = document.createElement('colgroup');
      ['col-ssid','col-bssid','col-rssi','col-enc'].forEach(cls => {
        const c = document.createElement('col');
        c.className = cls;
        colgroup.appendChild(c);
      });
      tbl.appendChild(colgroup);

      const thead = tbl.createTHead();
      const hrow  = thead.insertRow();
      ['SSID','BSSID','RSSI','Encryption'].forEach(txt => {
        const th = document.createElement('th');
        th.textContent = txt;
        hrow.appendChild(th);
      });

      const tbody = tbl.createTBody();
      list.forEach(net => {
        const tr = tbody.insertRow();

        const tdSsid = tr.insertCell();
        tdSsid.className   = 'td-ssid';
        tdSsid.textContent = net.ssid || '(hidden)';

        const tdBssid = tr.insertCell();
        tdBssid.className   = 'td-bssid';
        tdBssid.textContent = net.bssid || '—';

        const tdRssi = tr.insertCell();
        tdRssi.className   = 'td-rssi';
        tdRssi.textContent = (net.rssi !== undefined) ? net.rssi + ' dBm' : '—';

        const tdEnc = tr.insertCell();
        tdEnc.className   = 'td-enc';
        tdEnc.textContent = authLabel(net.auth);

        tr.addEventListener('click', () => {
          wifiSsidInput.value       = net.ssid || '';
          scanResults.style.display = 'none';
          scanStatus.textContent    = '';          // ← clear the "x found" message
          document.getElementById('wifi_password').focus();
        });
      });

      scanResults.appendChild(tbl);
      scanResults.style.display = 'block';
      scanStatus.textContent    = list.length + ' network(s) found — click a row to select.';

    } catch(e) {
      scanStatus.textContent = '❌ Scan failed: ' + e.message;
    }

    btnScan.disabled = false;
  });

  document.addEventListener('click', e => {
    if (!scanResults.contains(e.target) && e.target !== btnScan)
      scanResults.style.display = 'none';
  });

  btnWifiSave.addEventListener('click', () => {
    const ssid = wifiSsidInput.value.trim();
    const pw   = document.getElementById('wifi_password').value;
    if (!ssid) {
      scanStatus.textContent = '⚠ Please enter or select an SSID first.';
      return;
    }
    fetch(`/action?wifi_ssid=${encodeURIComponent(ssid)}&wifi_password=${encodeURIComponent(pw)}`)
      .then(() => {
        scanStatus.textContent    = '';
        restartHint.style.display = 'block';
      })
      .catch(() => {
        scanStatus.textContent = '❌ Save failed — is the robot reachable?';
      });
  });

  document.getElementById('btn_do_restart').addEventListener('click', () => {
    fetch('/action?restart=1').catch(() => {});
    restartHint.innerHTML =
      '🔄 Restart command sent. Reconnect to the robot after a few seconds.';
  });

  document.getElementById('btn_dismiss_hint').addEventListener('click', () => {
    restartHint.style.display = 'none';
  });

  /* ═══════════════════════════════════════════════
     CAMERA CONFIGURATION
  ═══════════════════════════════════════════════ */
  /*
    GET /action?cameraConfigRead
      → comma-separated integers, field order: rotation, size, fps, quality
        e.g.  0,1,0,0   (0° / 640×480 / auto fps / auto quality)

    GET /action?cameraConfigWrite=0,1,0,0
      → any 200 response (body ignored)

    Value tables (matching option value= in HTML):
      rotation : 0=0°  1=180°
      size     : 0=320×240  1=640×480  2=800×600  3=1024×768  4=1280×1024
      fps      : 0=auto  1=2fps  2=5fps  3=10fps  4=15fps  5=20fps  6=25fps  7=30fps
      quality  : 0=auto  1=very poor  2=poor  3=medium  4=good  5=very good
  */

  const camSelIds = ['cam_rotation', 'cam_size', 'cam_fps', 'cam_quality'];
  const camStatus = document.getElementById('cam_status');

  async function cam_load() {
    try {
      const res  = await fetch('/action?cameraConfigRead');
      const text = await res.text();
      const vals = text.trim().split(',').map(s => parseInt(s.trim(), 10));
      camSelIds.forEach((id, i) => {
        if (!isNaN(vals[i])) {
          const sel = document.getElementById(id);
          if (sel) sel.value = vals[i];
        }
      });
      camStatus.textContent = '✅ Camera config loaded.';
    } catch(e) {
      camStatus.textContent = '⚠ Could not load camera config.';
    }
  }

  async function cam_save() {
    const vals = camSelIds.map(id => document.getElementById(id).value);
    const csv  = vals.join(',');
    try {
      await fetch(`/action?cameraConfigWrite=${csv}`);
      camStatus.textContent = '✅ Saved (' + csv + ')';
    } catch(e) {
      camStatus.textContent = '❌ Camera config save failed.';
    }
  }

  camSelIds.forEach(id => {
    document.getElementById(id).addEventListener('change', cam_save);
  });

  /* ═══════════════════════════════════════════════
     PWM THINGS — Motor / Servo
  ═══════════════════════════════════════════════ */
  const pt_COUNT = 4;

  const pt_thingTypes = [
    "pwmOut", "pwmOutGamma", "halfBridge",
    "halfBridgeIdleHigh", "servoMotor", "servoMotor0Stop"
  ];

  let pt_availablePins = null;

  async function pt_loadAvailablePins() {
    /*
      GET /action?pwmThingAvailablePins
        → CSV of GPIO integers, e.g.:  -1,0,2,4,12,13,14,15
          -1  = "none/disabled" sentinel → shown as "— none —"
          Any value < 0 is treated as the none-sentinel and is
          NOT shown as "GPIO -1" (filtered out of the GPIO list).
          0 is a valid GPIO and is shown normally as "GPIO 0".
    */
    try {
      const res  = await fetch('/action?pwmThingAvailablePins');
      const text = await res.text();
      const all  = text.trim().split(',')
                       .map(s => parseInt(s.trim(), 10))
                       .filter(n => !isNaN(n));
      const hasNone = all.some(n => n < 0);
      pt_availablePins = { hasNone, pins: all.filter(n => n >= 0) };
    } catch(e) {
      console.warn('Could not load available pins, falling back to number input.', e);
      pt_availablePins = null;
    }
  }

  function pt_makePinSelect(id) {
    if (!pt_availablePins) {
      const inp = document.createElement('input');
      inp.type = 'number'; inp.id = id; inp.value = 0;
      return inp;
    }
    const sel = document.createElement('select');
    sel.id = id;
    if (pt_availablePins.hasNone) {
      sel.appendChild(new Option('— none —', -1));
    }
    pt_availablePins.pins.forEach(pin => {
      sel.appendChild(new Option('GPIO ' + pin, pin));
    });
    return sel;
  }

  function pt_createUI() {
    const container = document.getElementById('pt_configs');
    container.innerHTML = '';

    for (let i = 0; i < pt_COUNT; i++) {
      const wrap = document.createElement('div');
      wrap.className = 'pt_container';

      const title = document.createElement('div');
      title.className   = 'pt_title';
      title.id          = `pt_name${i}`;
      title.textContent = `Channel ${i}`;
      wrap.appendChild(title);

      const grid = document.createElement('div');
      grid.className = 'pt_grid';

      // helper: append a label + element as the next two grid cells
      function cell(labelText, el) {
        const lbl = document.createElement('label');
        lbl.textContent = labelText;
        lbl.htmlFor     = el.id || '';
        grid.appendChild(lbl);
        grid.appendChild(el);
      }

      // Row 1: Pin A | Pin B
      cell('Pin A', pt_makePinSelect(`pt_pinA${i}`));
      cell('Pin B', pt_makePinSelect(`pt_pinB${i}`));

      // Row 2: Type | Invert  — both use the same single-value column width
      const selType = document.createElement('select');
      selType.id = `pt_type${i}`;
      pt_thingTypes.forEach((t, idx) => selType.appendChild(new Option(t, idx)));
      cell('Type', selType);

      const chkInv = document.createElement('input');
      chkInv.type = 'checkbox'; chkInv.id = `pt_inv${i}`;
      cell('Invert', chkInv);

      // Row 3: Min | Zero
      const inMin = document.createElement('input');
      inMin.type = 'number'; inMin.id = `pt_min${i}`; inMin.value = 0;
      cell('Min', inMin);

      const inZero = document.createElement('input');
      inZero.type = 'number'; inZero.id = `pt_zero${i}`; inZero.value = 0;
      cell('Zero', inZero);

      // Row 4: Max | (empty)
      const inMax = document.createElement('input');
      inMax.type = 'number'; inMax.id = `pt_max${i}`; inMax.value = 0;
      cell('Max', inMax);

      // empty pair so the grid row is complete
      grid.appendChild(document.createElement('span'));
      grid.appendChild(document.createElement('span'));

      wrap.appendChild(grid);
      container.appendChild(wrap);
    }
  }

  function pt_applyConfig(index, csv) {
    const p = csv.split(',');
    if (p.length < 8) return;
    document.getElementById(`pt_name${index}`).textContent = p[0] || `Channel ${index}`;
    const pa = document.getElementById(`pt_pinA${index}`);
    const pb = document.getElementById(`pt_pinB${index}`);
    if (pa) pa.value = p[1];
    if (pb) pb.value = p[2];
    document.getElementById(`pt_type${index}`).value  = p[3];
    document.getElementById(`pt_inv${index}`).checked = Number(p[4]) !== 0;
    document.getElementById(`pt_min${index}`).value   = p[5];
    document.getElementById(`pt_zero${index}`).value  = p[6];
    document.getElementById(`pt_max${index}`).value   = p[7];
  }

  function pt_collectConfig(index) {
    const name = document.getElementById(`pt_name${index}`).textContent;
    const pinA = document.getElementById(`pt_pinA${index}`).value || 0;
    const pinB = document.getElementById(`pt_pinB${index}`).value || 0;
    const type = document.getElementById(`pt_type${index}`).value;
    const inv  = document.getElementById(`pt_inv${index}`).checked ? 1 : 0;
    const min  = document.getElementById(`pt_min${index}`).value  || 0;
    const zero = document.getElementById(`pt_zero${index}`).value || 0;
    const max  = document.getElementById(`pt_max${index}`).value  || 0;
    return [name, pinA, pinB, type, inv, min, zero, max].join(',');
  }

  async function pt_retrievePwmThings() {
    document.getElementById('pt_status').textContent = '⏳ Retrieving…';
    let ok = 0;
    for (let i = 0; i < pt_COUNT; i++) {
      try {
        const res  = await fetch(`/action?pwmThingRead=${i}`);
        const text = await res.text();
        pt_applyConfig(i, text.trim());
        ok++;
      } catch(e) { console.warn('pt_retrieve failed for', i, e); }
    }
    document.getElementById('pt_status').textContent =
      ok === pt_COUNT ? '✅ All channels loaded.' : `⚠ Loaded ${ok}/${pt_COUNT} channels.`;
  }

  async function pt_storePwmThings() {
    document.getElementById('pt_status').textContent = '⏳ Storing…';
    let ok = 0;
    for (let i = 0; i < pt_COUNT; i++) {
      try {
        await fetch(`/action?pwmThingWrite=${i},${pt_collectConfig(i)}`);
        ok++;
      } catch(e) { console.warn('pt_store failed for', i, e); }
    }
    document.getElementById('pt_status').textContent =
      ok === pt_COUNT ? '✅ All channels stored.' : `⚠ Stored ${ok}/${pt_COUNT} channels.`;
  }

  /* ═══════════════════════════════════════════════
     SYSTEM BUTTONS
  ═══════════════════════════════════════════════ */
  document.getElementById('btn_reconnect').addEventListener('click', () => {
    if (confirm('Trigger a WiFi reconnect now?'))
      fetch('/action?reconnect=1').catch(() => {});
  });

  document.getElementById('btn_restart').addEventListener('click', () => {
    if (confirm('Restart the robot now?'))
      fetch('/action?restart=1').catch(() => {});
  });

  /* ═══════════════════════════════════════════════
     PAGE INIT
  ═══════════════════════════════════════════════ */
  (async function init() {
    updateTitle();
    cam_load();
    await pt_loadAvailablePins();
    pt_createUI();
    await pt_retrievePwmThings();
  })();
</script>
</body>
</html>
)rawliteral";