// Not used if index.html exists in data/index.html
// Use data/index.html for development and testing, as overwriting data will erase user settings as well. 
static const char PROGMEM INDEX_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html lang="de">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no, viewport-fit=cover">
  <meta name="apple-mobile-web-app-capable" content="yes">
  <meta name="mobile-web-app-capable" content="yes">
  <meta name="apple-mobile-web-app-status-bar-style" content="black-translucent">
  <meta name="theme-color" content="#000000">
  <meta name="apple-mobile-web-app-title" content="HackFFM Bot">
  <link rel="apple-touch-icon" href="data:image/png;base64,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">
  <link rel="icon" href="/favicon.ico">
  <title>Hackffm ESP32 CAM Bot</title>

  <style>
    /* ============================================================
       iOS-Style Fullscreen FPV UI  —  fully self-contained
    ============================================================ */
    :root {
      --accent:        #0a84ff;
      --accent-orange: #ff9f0a;
      --green:         #30d158;
      --red:           #ff453a;
      --text:          #ffffff;
      --text2:         #98989f;
      --sheet-bg:      #1c1c1e;
      --card:          #2c2c2e;
      --sep:           rgba(84,84,88,0.6);
      --fill:          rgba(120,120,128,0.24);
      --topbar-h:      44px;
    }

    * { box-sizing: border-box; -webkit-tap-highlight-color: transparent; }

    html, body {
      margin: 0; height: 100%; overflow: hidden; background: #000;
      color: var(--text);
      font-family: -apple-system, BlinkMacSystemFont, "SF Pro Text", "Segoe UI", Roboto, sans-serif;
      -webkit-user-select: none; user-select: none;
    }
    #app { display: flex; flex-direction: column; height: 100vh; height: 100dvh; }
    .hidden { display: none !important; }

    .ic { width: 26px; height: 26px; display: block; }

    /* ── Top bar ── */
    #topbar {
      flex: none; z-index: 30;
      height: calc(var(--topbar-h) + env(safe-area-inset-top));
      padding-top: env(safe-area-inset-top);
      padding-left: calc(12px + env(safe-area-inset-left));
      padding-right: calc(8px + env(safe-area-inset-right));
      display: flex; align-items: center; gap: 10px;
      background: rgba(0,0,0,0.82);
      -webkit-backdrop-filter: blur(20px); backdrop-filter: blur(20px);
      border-bottom: 0.5px solid rgba(255,255,255,0.12);
    }
    #topbar .logo { height: 30px; width: auto; flex: none; display: block; }
    #title {
      flex: 0 1 auto; min-width: 0;
      font-size: 16px; font-weight: 600; color: #fff;
      white-space: nowrap; overflow: hidden; text-overflow: ellipsis;
    }
    #topstatus {
      flex: 1 1 auto; min-width: 0;
      font-size: 13px; color: var(--text2);
      white-space: nowrap; overflow: hidden; text-overflow: ellipsis;
    }
    .gear-btn {
      width: 40px; height: 40px; flex: none; border: none; background: none;
      color: #fff; display: flex; align-items: center; justify-content: center;
    }
    /* Gamepad-Suche-Dialog */
    .dialog-overlay {
      position: fixed; inset: 0; z-index: 80;
      background: rgba(0,0,0,0.55);
      -webkit-backdrop-filter: blur(4px); backdrop-filter: blur(4px);
      display: flex; align-items: center; justify-content: center; padding: 24px;
    }
    .dialog {
      width: 100%; max-width: 320px; background: var(--card);
      border-radius: 16px; padding: 24px 20px; text-align: center;
    }
    .dialog-emoji { font-size: 34px; margin-bottom: 10px; }
    .dialog-title { font-size: 18px; font-weight: 700; margin-bottom: 8px; }
    .dialog-text { font-size: 15px; color: var(--text2); margin-bottom: 20px; line-height: 1.4; }
    .dialog-btn {
      width: 100%; border: none; border-radius: 12px; padding: 13px;
      font-size: 16px; font-weight: 600; background: var(--fill); color: var(--accent);
    }

    /* Vollbild-/Immersiv-Modus: Topbar aus, schwebende Steuerung oben rechts */
    #fs-controls { display: none; }
    body.fullscreen-mode #topbar { display: none; }
    body.fullscreen-mode #fs-controls {
      display: flex; gap: 8px; position: fixed; z-index: 40;
      top: calc(8px + env(safe-area-inset-top));
      right: calc(8px + env(safe-area-inset-right));
    }
    #fs-controls .gear-btn {
      width: 44px; height: 44px; border-radius: 50%;
      background: rgba(20,20,22,0.55); border: 0.5px solid rgba(255,255,255,0.18);
      -webkit-backdrop-filter: blur(14px); backdrop-filter: blur(14px);
    }

    /* Smartphone im Querformat: automatisch immersiv (Topbar aus) */
    @media (orientation: landscape) and (max-height: 500px) and (pointer: coarse) {
      #topbar { display: none; }
      #fs-controls {
        display: flex; gap: 8px; position: fixed; z-index: 40;
        top: calc(8px + env(safe-area-inset-top));
        right: calc(8px + env(safe-area-inset-right));
      }
    }

    /* ── Camera stage (fills remaining height, video letterboxed) ── */
    #stage { position: relative; flex: 1 1 0; min-height: 0; background: #000; overflow: hidden; }
    #stream-area { position: absolute; inset: 0; background: #000; overflow: hidden; }
    #photo { width: 100%; height: 100%; object-fit: contain; object-position: top center; display: block; }
    #stream-placeholder { position: absolute; inset: 0; width: 100%; height: 100%; object-fit: contain; object-position: top center; }

    /* ── Left overlay: light + servo arrows ── */
    #overlay-left {
      position: absolute; z-index: 15;
      top: 14px;
      left: calc(14px + env(safe-area-inset-left));
      display: flex; flex-direction: column; gap: 16px; align-items: center;
    }
    .ov-btn {
      width: 52px; height: 52px; border-radius: 50%; padding: 0;
      border: 0.5px solid rgba(255,255,255,0.2);
      background: rgba(22,22,24,0.5);
      -webkit-backdrop-filter: blur(16px); backdrop-filter: blur(16px);
      color: #fff; display: flex; align-items: center; justify-content: center;
      transition: background .12s, color .12s, box-shadow .12s;
    }
    .ov-btn:active { background: rgba(70,70,74,0.6); transform: scale(0.96); }
    .ov-btn.sm { width: 46px; height: 46px; }
    .ov-btn.sm .ic { width: 24px; height: 24px; }

    #ov-light-low, #ov-light-boost { width: 56px; height: 56px; }
    #ov-light-low .ic, #ov-light-boost .ic { width: 28px; height: 28px; }
    #ov-light-low.active {
      background: rgba(255,159,10,0.9); border-color: rgba(255,205,130,0.9); color: #2a1c00;
    }
    /* Boost-Status: Kreis füllt sich von unten nach oben */
    #ov-light-boost { position: relative; overflow: hidden; }
    #ov-light-boost .ic { position: relative; z-index: 1; }
    .boost-fill {
      position: absolute; left: 0; right: 0; bottom: 0; height: 0%;
      background: #30d158; z-index: 0;
      transition: height .4s linear, background .4s linear;
    }
    #ov-turtle.active {
      background: rgba(48,209,88,0.9); border-color: rgba(130,235,160,0.9); color: #042110;
    }

    #ov-s1-toggle.active, #ov-s2-toggle.active {
      background: rgba(10,132,255,0.85); border-color: rgba(130,185,255,0.9); color: #00112a;
    }
    .ov-btn.down .ic { transform: rotate(180deg); }

    .servo-pad { display: flex; flex-direction: column; align-items: center; gap: 5px; }
    .ov-lbl {
      font-size: 11px; font-weight: 700; color: rgba(255,255,255,0.9);
      text-shadow: 0 1px 3px rgba(0,0,0,0.7); letter-spacing: 0.3px;
    }

    /* ── Joystick (bottom-right) ── */
    #joystick-wrapper {
      position: absolute; z-index: 15;
      bottom: calc(18px + env(safe-area-inset-bottom));
      right: calc(18px + env(safe-area-inset-right));
      display: flex; flex-direction: column; align-items: center;
    }
    #joystick { touch-action: none; }
    #joy-coords { display: none; }

    /* ============================================================
       Settings panel — Kamera bleibt oben live, Settings scrollen
    ============================================================ */
    #settings-panel {
      flex: 0 0 0; min-height: 0; overflow: hidden;
      background: var(--sheet-bg);
      display: flex; flex-direction: column;
      border-top: 0.5px solid rgba(255,255,255,0.14);
    }
    body.settings-open #settings-panel { flex: 2 1 0; }
    body.settings-open #overlay-left { transform: scale(0.6); transform-origin: top left; }
    body.settings-open #joystick-wrapper { transform: scale(0.55); transform-origin: bottom right; }

    .sheet-header {
      display: flex; align-items: center; justify-content: space-between;
      padding: 6px 16px 8px;
    }
    .sheet-title { font-size: 18px; font-weight: 700; }
    .hdr-btn { border: none; background: none; color: var(--accent);
      font-size: 17px; font-weight: 500; padding: 4px 2px; white-space: nowrap; }
    .hdr-btn.save { font-weight: 700; }
    .hdr-btn.saved { color: var(--green); }

    /* segmented control */
    .segmented {
      display: flex; gap: 3px; background: rgba(118,118,128,0.36);
      border-radius: 10px; padding: 3px; margin: 4px 16px 16px;
    }
    .seg {
      flex: 1; border: none; background: transparent; color: var(--text2);
      font-size: 15px; font-weight: 600; padding: 9px 0; border-radius: 8px;
      transition: background .15s, color .15s;
    }
    .seg.active { background: #6c6c70; color: #fff; box-shadow: 0 1px 3px rgba(0,0,0,0.4); }

    .sheet-body {
      flex: 1 1 auto; min-height: 0; overflow-y: auto; -webkit-overflow-scrolling: touch;
      padding-bottom: calc(24px + env(safe-area-inset-bottom));
      width: 100%; max-width: 640px; margin: 0 auto;
    }

    /* grouped list */
    .group-title {
      font-size: 13px; text-transform: uppercase; letter-spacing: 0.4px;
      color: var(--text2); margin: 20px 16px 6px;
    }
    .group { background: var(--card); border-radius: 12px; margin: 0 16px; overflow: hidden; }
    .row {
      padding: 12px 16px; display: flex; align-items: center;
      justify-content: space-between; gap: 12px;
      border-top: 0.5px solid var(--sep);
    }
    .group .row:first-child { border-top: none; }
    .row.col { flex-direction: column; align-items: stretch; gap: 9px; }
    .row.btns { flex-wrap: wrap; justify-content: flex-start; }
    .row-head { display: flex; align-items: center; justify-content: space-between; gap: 10px; }
    .row-head > span { font-weight: 600; font-size: 15px; }
    .sub { display: flex; justify-content: space-between; font-size: 13px; }
    .muted { color: var(--text2); }
    .val { color: var(--text); font-variant-numeric: tabular-nums; }
    a.link { color: var(--accent); text-decoration: none; font-size: 15px; }

    /* range sliders */
    input[type=range] {
      -webkit-appearance: none; appearance: none;
      width: 100%; height: 28px; background: transparent; margin: 0;
    }
    input[type=range]::-webkit-slider-runnable-track {
      height: 5px; border-radius: 3px; background: var(--fill);
    }
    input[type=range]::-webkit-slider-thumb {
      -webkit-appearance: none; width: 26px; height: 26px; border-radius: 50%;
      background: #fff; margin-top: -10.5px; box-shadow: 0 1px 4px rgba(0,0,0,0.45);
    }
    input[type=range]::-moz-range-track { height: 5px; border-radius: 3px; background: var(--fill); }
    input[type=range]::-moz-range-thumb {
      width: 26px; height: 26px; border: none; border-radius: 50%;
      background: #fff; box-shadow: 0 1px 4px rgba(0,0,0,0.45);
    }

    /* tinted buttons */
    .ctrl-btn {
      border: none; border-radius: 9px; padding: 9px 14px;
      font-size: 15px; font-weight: 600;
      background: rgba(10,132,255,0.18); color: var(--accent);
    }
    .ctrl-btn.active { background: var(--accent); color: #fff; }
    .ctrl-btn.running { background: rgba(255,69,58,0.18); color: var(--red); }
    .ctrl-btn.mini { padding: 4px 10px; font-size: 12px; }

    .sys-btn {
      border: none; border-radius: 9px; padding: 10px 14px;
      font-size: 15px; font-weight: 600;
      background: var(--fill); color: var(--text);
    }
    .sys-btn.warn   { background: rgba(255,159,10,0.18); color: var(--accent-orange); }
    .sys-btn.danger { background: rgba(255,69,58,0.18);  color: var(--red); }

    /* selects */
    select {
      -webkit-appearance: none; appearance: none;
      background: var(--fill); color: var(--text); border: none;
      border-radius: 8px; padding: 7px 12px; font-size: 14px;
    }

    /* iOS switches (checkboxes) */
    input[type=checkbox] {
      -webkit-appearance: none; appearance: none; flex: none;
      width: 48px; height: 30px; border-radius: 15px; position: relative;
      background: var(--fill); transition: background .2s; vertical-align: middle;
    }
    input[type=checkbox]:checked { background: var(--green); }
    input[type=checkbox]::after {
      content: ""; position: absolute; top: 2px; left: 2px;
      width: 26px; height: 26px; border-radius: 50%; background: #fff;
      box-shadow: 0 1px 3px rgba(0,0,0,0.4); transition: transform .2s;
    }
    input[type=checkbox]:checked::after { transform: translateX(18px); }

    .gp-out { font-size: 13px; color: var(--text2); margin: 0; }
    .telemetry {
      font-family: ui-monospace, Menlo, monospace; font-size: 12px;
      color: var(--text2); white-space: pre-wrap; margin: 16px;
    }
    .footer-note {
      display: flex; align-items: flex-end; justify-content: center; gap: 10px;
      text-align: left; color: var(--text2); font-size: 12px; margin: 8px 16px 4px;
    }
    .footer-note .logo { height: 48px; width: auto; flex: none; }
  </style>
</head>
<body>
<div id="app">

  <!-- ══════════════ TOP BAR ══════════════ -->
  <div id="topbar">
    <img class="logo" alt="hackffm.de" src="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAADMAAAA/AgMAAAAwDRjCAAAADFBMVEVuAAAoKCjwrV74+Ph5Qa9/AAAAhElEQVQoz6XTSQrAIAwF0F7yr3O6HDG7ljTRUhW+0sGF+CADEd28W/u20oG2/kivszJVKpcWTKSmUy3yABv6dUIRmHAJryUGnynv2bgko4RKJBpY7EReI6MNkZUOTiUSanmDyjxGtZrBkVVw542KMDcq5BVJV7NXnXairOLv9eldP/5HJ2k/9FhNcZTRAAAAAElFTkSuQmCC">
    <div id="title">Hackffm ESP32 CAM Bot</div>
    <div id="topstatus"></div>
    <button id="btn-fullscreen" class="gear-btn" aria-label="Vollbild">
      <svg class="ic" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.7" stroke-linecap="round" stroke-linejoin="round">
        <path d="M8 3H5a2 2 0 0 0-2 2v3M16 3h3a2 2 0 0 1 2 2v3M21 16v3a2 2 0 0 1-2 2h-3M3 16v3a2 2 0 0 0 2 2h3"/>
      </svg>
    </button>
    <button id="btn-gamepad" class="gear-btn" aria-label="Gamepad suchen">
      <svg class="ic" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.6" stroke-linecap="round" stroke-linejoin="round">
        <path d="M9 11.5H6M7.5 10v3"/>
        <circle cx="16.4" cy="10.6" r="0.9" fill="currentColor" stroke="none"/>
        <circle cx="18" cy="13" r="0.9" fill="currentColor" stroke="none"/>
        <path d="M7.6 7.5h8.8a4.5 4.5 0 0 1 4.4 5.45l-.7 3.1a2.4 2.4 0 0 1-4.4.7l-.9-1.3a2 2 0 0 0-1.65-.85h-2.6a2 2 0 0 0-1.65.85l-.9 1.3a2.4 2.4 0 0 1-4.4-.7l-.7-3.1A4.5 4.5 0 0 1 7.6 7.5Z"/>
      </svg>
    </button>
    <button id="btn-settings" class="gear-btn" aria-label="Einstellungen">
      <svg class="ic" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.7" stroke-linecap="round" stroke-linejoin="round">
        <circle cx="12" cy="12" r="3.2"/>
        <path d="M19.4 12.9a7.6 7.6 0 0 0 0-1.8l2-1.6-2-3.4-2.4 1a7.3 7.3 0 0 0-1.5-.9l-.4-2.6H10.9l-.4 2.6a7.3 7.3 0 0 0-1.5.9l-2.4-1-2 3.4 2 1.6a7.6 7.6 0 0 0 0 1.8l-2 1.6 2 3.4 2.4-1a7.3 7.3 0 0 0 1.5.9l.4 2.6h4.2l.4-2.6a7.3 7.3 0 0 0 1.5-.9l2.4 1 2-3.4z"/>
      </svg>
    </button>
  </div>

  <!-- ══════════════ CAMERA STAGE + OVERLAYS ══════════════ -->
  <div id="stage">
  <div id="stream-area">
    <canvas id="stream-placeholder"></canvas>
    <img id="photo" alt="">

    <!-- Left overlay: light + servos -->
    <div id="overlay-left">
      <button id="ov-light-low" class="ov-btn" aria-label="Schwachlicht (Schalter)">
        <svg class="ic" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.8" stroke-linecap="round" stroke-linejoin="round">
          <circle cx="12" cy="12" r="4.2"/>
          <path d="M12 2.5v2M12 19.5v2M2.5 12h2M19.5 12h2M5 5l1.4 1.4M17.6 17.6 19 19M19 5l-1.4 1.4M6.4 17.6 5 19"/>
        </svg>
      </button>

      <button id="ov-light-boost" class="ov-btn" aria-label="Hell (Taster), Füllung = Boost-Restzeit">
        <span id="boostFill" class="boost-fill"></span>
        <svg class="ic" viewBox="0 0 24 24" fill="currentColor" stroke="none"><path d="M13 2 4 14h6l-1 8 9-12h-6z"/></svg>
      </button>

      <button id="ov-turtle" class="ov-btn" aria-label="Langsam-Modus (Schildkröte)">
        <svg class="ic" viewBox="0 0 24 24" fill="currentColor">
          <ellipse cx="12" cy="12.5" rx="6" ry="4.6"/>
          <circle cx="12" cy="5.6" r="1.9"/>
          <circle cx="6.4" cy="8.2" r="1.5"/>
          <circle cx="17.6" cy="8.2" r="1.5"/>
          <circle cx="6.4" cy="16.8" r="1.5"/>
          <circle cx="17.6" cy="16.8" r="1.5"/>
        </svg>
      </button>

      <div class="servo-pad">
        <button id="ov-s1-toggle" class="ov-btn down" aria-label="Servo 1 umschalten">
          <svg class="ic" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.4" stroke-linecap="round" stroke-linejoin="round"><path d="M6 15l6-6 6 6"/></svg>
        </button>
      </div>

      <div class="servo-pad hidden" id="servo2-pad">
        <button id="ov-s2-toggle" class="ov-btn down" aria-label="Servo 2 umschalten">
          <svg class="ic" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.4" stroke-linecap="round" stroke-linejoin="round"><path d="M9 6l6 6-6 6"/></svg>
        </button>
        <span class="ov-lbl">S2</span>
      </div>
    </div>

    <!-- Joystick steering pad -->
    <div id="joystick-wrapper">
      <div id="joy-coords">x:0.00 y:0.00  ml:0 mr:0</div>
      <canvas id="joystick" width="300" height="300"></canvas>
    </div>
  </div>
  </div><!-- end #stage -->

  <!-- ══════════════ SETTINGS PANEL (Kamera bleibt oben live) ══════════════ -->
  <div id="settings-panel">
      <div class="sheet-header">
        <button id="btn-settings-back" class="hdr-btn">‹ Zurück</button>
        <div class="sheet-title">Einstellungen</div>
        <button id="btn-settings-save" class="hdr-btn save">Speichern</button>
      </div>

      <div class="segmented">
        <button class="seg active" data-tab="control">Steuerung</button>
        <button class="seg" data-tab="gamepad">Gamepad</button>
      </div>

      <div class="sheet-body">

        <!-- ───────── TAB: STEUERUNG ───────── -->
        <div id="tab-control">

          <div class="group-title">Licht</div>
          <div class="group">
            <div class="row col">
              <div class="row-head">
                <span id="label-light">💡 Licht (0–100%)</span>
                <span style="display:flex;gap:8px;">
                  <button class="ctrl-btn" id="btn-light-offlow">Aus / Schwach</button>
                  <button class="ctrl-btn" id="btn-light-boost">Boost</button>
                </span>
              </div>
              <input type="range" id="slider-light" min="0" max="100" value="0">
              <div class="sub"><span class="muted">Helligkeit</span><span class="val" id="val-light">0%</span></div>
            </div>
            <div class="row col">
              <div class="sub"><span class="muted">Schwach</span><span class="val" id="val-lightLow">12%</span></div>
              <input type="range" id="slider-lightLow" min="0" max="100" value="12">
            </div>
            <div class="row col">
              <div class="sub"><span class="muted">Hell</span><span class="val" id="val-lightHigh">100%</span></div>
              <input type="range" id="slider-lightHigh" min="0" max="100" value="100">
            </div>
          </div>

          <div class="group-title">Servo 1</div>
          <div class="group">
            <div class="row col">
              <div class="row-head">
                <span id="label-servo1">Servo 1</span>
                <button class="ctrl-btn" id="btn-servo1-toggle">Hoch / Runter</button>
              </div>
              <input type="range" id="slider-servo1" min="-255" max="255" value="0">
              <div class="sub"><span class="muted">Position</span><span class="val" id="val-servo1">0</span></div>
            </div>
            <div class="row col">
              <div class="sub">
                <span style="display:flex;gap:8px;align-items:center;"><span class="muted">Runter</span><button class="ctrl-btn mini" id="cap-servo1Low">Position übernehmen</button></span>
                <span class="val" id="val-servo1Low">-30</span>
              </div>
              <input type="range" id="slider-servo1Low" min="-255" max="255" value="-30">
            </div>
            <div class="row col">
              <div class="sub">
                <span style="display:flex;gap:8px;align-items:center;"><span class="muted">Hoch</span><button class="ctrl-btn mini" id="cap-servo1High">Position übernehmen</button></span>
                <span class="val" id="val-servo1High">150</span>
              </div>
              <input type="range" id="slider-servo1High" min="-255" max="255" value="150">
            </div>
          </div>

          <div id="servo2-config" class="hidden">
          <div class="group-title">Servo 2</div>
          <div class="group">
            <div class="row col">
              <div class="row-head">
                <span id="label-servo2">Servo 2</span>
                <button class="ctrl-btn" id="btn-servo2-toggle">Hoch / Runter</button>
              </div>
              <input type="range" id="slider-servo2" min="-255" max="255" value="0">
              <div class="sub"><span class="muted">Position</span><span class="val" id="val-servo2">0</span></div>
            </div>
            <div class="row col">
              <div class="sub">
                <span style="display:flex;gap:8px;align-items:center;"><span class="muted">Runter</span><button class="ctrl-btn mini" id="cap-servo2Low">Position übernehmen</button></span>
                <span class="val" id="val-servo2Low">-30</span>
              </div>
              <input type="range" id="slider-servo2Low" min="-255" max="255" value="-30">
            </div>
            <div class="row col">
              <div class="sub">
                <span style="display:flex;gap:8px;align-items:center;"><span class="muted">Hoch</span><button class="ctrl-btn mini" id="cap-servo2High">Position übernehmen</button></span>
                <span class="val" id="val-servo2High">150</span>
              </div>
              <input type="range" id="slider-servo2High" min="-255" max="255" value="150">
            </div>
          </div>
          </div><!-- end #servo2-config -->

          <div class="group-title">Langsam-Modus</div>
          <div class="group">
            <div class="row col">
              <div class="sub"><span class="muted">🐢 Schildkröten-Geschwindigkeit</span><span class="val" id="val-turtle">40%</span></div>
              <input type="range" id="slider-turtle" min="5" max="100" value="40">
            </div>
          </div>

          <div class="group-title">Joysticksensivität</div>
          <div class="group">
            <div class="row col">
              <div class="sub"><span class="muted">Joystick X (Lenkung)</span><span class="val" id="val-sensX">1.00</span></div>
              <input type="range" id="slider-sensX" min="0.25" max="3" step="0.05" value="1">
              <div class="sub"><span class="muted">sanft</span><span class="muted">stark</span></div>
            </div>
            <div class="row col">
              <div class="sub"><span class="muted">Joystick Y (Geschwindigkeit)</span><span class="val" id="val-sensY">1.00</span></div>
              <input type="range" id="slider-sensY" min="0.25" max="3" step="0.05" value="1">
              <div class="sub"><span class="muted">sanft</span><span class="muted">stark</span></div>
            </div>
          </div>

          <div class="group-title">Kamera</div>
          <div class="group">
            <div class="row">
              <span>Stream</span>
              <button id="btn-stream-toggle" class="ctrl-btn running">⏹ Stream stoppen</button>
            </div>
            <div class="row">
              <span>Auflösung</span>
              <select id="cam-size">
                <option value="0">QVGA 320×240</option>
                <option value="1">CIF 400x296</option>
                <option value="2">VGA 640×480</option>
                <option value="3">SVGA 800×600</option>
                <option value="4">XGA 1024×768</option>
                <option value="5">SXGA 1280×1024</option>
                <option value="6">UXGA 1600×1200</option>
              </select>
            </div>
            <div class="row">
              <span>Bildrate</span>
              <select id="cam-fps">
                <option value="0">Auto</option>
                <option value="1">2 fps</option>
                <option value="2">5 fps</option>
                <option value="3">10 fps</option>
                <option value="4">15 fps</option>
                <option value="5">20 fps</option>
                <option value="6">25 fps</option>
                <option value="7">30 fps</option>
              </select>
            </div>
            <div class="row">
              <span>Qualität</span>
              <select id="cam-quality">
                <option value="0">Auto</option>
                <option value="1">Sehr niedrig</option>
                <option value="2">Niedrig</option>
                <option value="3">Mittel</option>
                <option value="4">Gut</option>
                <option value="5">Sehr gut</option>
              </select>
            </div>
            <div class="row">
              <span>180° drehen</span>
              <input type="checkbox" id="cam-rotation">
            </div>
          </div>

          <div class="group-title">System</div>
          <div class="group">
            <div class="row">
              <span>Auto-Aus</span>
              <span style="display:flex;align-items:center;gap:8px;justify-content:flex-end;">
                <span id="powerdown-status" class="muted" style="text-align:right;">Min.</span>
                <input type="number" id="input-powerdown" min="0" max="1440" step="1" value="60"
                  style="-webkit-appearance:none;appearance:none;background:var(--fill);color:var(--text);border:none;border-radius:8px;padding:7px 10px;font-size:15px;width:78px;text-align:right;">
              </span>
            </div>
            <div class="row btns">
              <button id="btn-reconnect" class="sys-btn">WLAN neu verbinden</button>
              <button id="btn-trigger"   class="sys-btn">WLAN Roaming</button>
              <button id="btn-restart"   class="sys-btn warn">Neustart</button>
              <button id="btn-shutdown"  class="sys-btn danger">Ausschalten</button>
            </div>
            <a class="row link" href="/settings">Erweiterte Einstellungen (WLAN, Name, Pins) ›</a>
          </div>

          <div id="info" class="telemetry"></div>
          <div class="footer-note">
            <img class="logo" alt="hackffm.de" src="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAADMAAAA/AgMAAAAwDRjCAAAADFBMVEVuAAAoKCjwrV74+Ph5Qa9/AAAAhElEQVQoz6XTSQrAIAwF0F7yr3O6HDG7ljTRUhW+0sGF+CADEd28W/u20oG2/kivszJVKpcWTKSmUy3yABv6dUIRmHAJryUGnynv2bgko4RKJBpY7EReI6MNkZUOTiUSanmDyjxGtZrBkVVw542KMDcq5BVJV7NXnXairOLv9eldP/5HJ2k/9FhNcZTRAAAAAElFTkSuQmCC">
            <span>ESP32_CAM_Auto <span id="version-display">V1.00</span> · <a class="link" href="https://github.com/hackffm/ESP32_CAM_Auto">GitHub</a> · <a class="link" href="https://www.hackerspace-ffm.de/wiki/index.php?title=FPV-Roboter">© 2026 Hackerspace-FFM e.V.</a></span>
          </div>
        </div>

        <!-- ───────── TAB: GAMEPAD ───────── -->
        <div id="tab-gamepad" class="hidden">

          <div class="group-title">Gamepad</div>
          <div class="group">
            <div class="row">
              <span>Gamepad</span>
              <select id="gpdsel_gamepadSelect"></select>
            </div>
            <div class="row col">
              <button id="searchGamepad" class="sys-btn">Gamepad suchen</button>
              <p id="output" class="gp-out">Kein Gamepad erkannt</p>
            </div>
          </div>

          <div class="group-title">Achsen</div>
          <div class="group">
            <div class="row">
              <span>X-Achse</span>
              <span style="display:flex;align-items:center;gap:10px;">
                <select id="gpdsel_xAxis">
                  <option value="0" selected>0</option><option value="1">1</option>
                  <option value="2">2</option><option value="3">3</option>
                  <option value="4">4</option><option value="5">5</option>
                </select>
                <label style="display:flex;align-items:center;gap:6px;font-size:13px;color:var(--text2);">Inv.<input type="checkbox" id="gpdsel_invertX"></label>
              </span>
            </div>
            <div class="row">
              <span>Y-Achse</span>
              <span style="display:flex;align-items:center;gap:10px;">
                <select id="gpdsel_yAxis">
                  <option value="0">0</option><option value="1" selected>1</option>
                  <option value="2">2</option><option value="3">3</option>
                  <option value="4">4</option><option value="5">5</option>
                </select>
                <label style="display:flex;align-items:center;gap:6px;font-size:13px;color:var(--text2);">Inv.<input type="checkbox" id="gpdsel_invertY"></label>
              </span>
            </div>
            <div class="row">
              <span class="muted">Werte</span>
              <span class="muted" style="font-variant-numeric:tabular-nums;">X: <span id="gpdsel_xVal">0.00</span>&nbsp;&nbsp;Y: <span id="gpdsel_yVal">0.00</span></span>
            </div>
            <div class="row"><span class="telemetry" id="gpdsel_axesDisplay" style="margin:0;"></span></div>
          </div>

          <div class="group-title">Tastenbelegung</div>
          <div class="group">
            <div class="row col">
              <button id="searchGamepadServo1" class="sys-btn">Taste für Servo 1 setzen</button>
              <p id="outputGamepadServo1" class="gp-out">Keine Taste gesetzt</p>
            </div>
            <div class="row col">
              <button id="searchGamepadServo2" class="sys-btn">Taste für Servo 2 setzen</button>
              <p id="outputGamepadServo2" class="gp-out">Keine Taste gesetzt</p>
            </div>
            <div class="row col">
              <button id="searchGamepadLowLight" class="sys-btn">Taste für Schwachlicht setzen</button>
              <p id="outputGamepadLowLight" class="gp-out">Keine Taste gesetzt</p>
            </div>
            <div class="row col">
              <button id="searchGamepadFlashLight" class="sys-btn">Taste für Blitzlicht setzen</button>
              <p id="outputGamepadFlashLight" class="gp-out">Keine Taste gesetzt</p>
            </div>
            <div class="row col">
              <button id="searchGamepadTurtle" class="sys-btn">Taste für Langsam-Modus setzen</button>
              <p id="outputGamepadTurtle" class="gp-out">Keine Taste gesetzt</p>
            </div>
          </div>

        </div>

      </div><!-- end sheet-body -->
  </div><!-- end #settings-panel -->
</div><!-- end #app -->

  <!-- ══════════════ SCHWEBENDE STEUERUNG (Vollbild) ══════════════ -->
  <div id="fs-controls">
    <button id="fs-gamepad" class="gear-btn" aria-label="Gamepad suchen">
      <svg class="ic" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.6" stroke-linecap="round" stroke-linejoin="round">
        <path d="M9 11.5H6M7.5 10v3"/>
        <circle cx="16.4" cy="10.6" r="0.9" fill="currentColor" stroke="none"/>
        <circle cx="18" cy="13" r="0.9" fill="currentColor" stroke="none"/>
        <path d="M7.6 7.5h8.8a4.5 4.5 0 0 1 4.4 5.45l-.7 3.1a2.4 2.4 0 0 1-4.4.7l-.9-1.3a2 2 0 0 0-1.65-.85h-2.6a2 2 0 0 0-1.65.85l-.9 1.3a2.4 2.4 0 0 1-4.4-.7l-.7-3.1A4.5 4.5 0 0 1 7.6 7.5Z"/>
      </svg>
    </button>
    <button id="fs-settings" class="gear-btn" aria-label="Einstellungen">
      <svg class="ic" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.7" stroke-linecap="round" stroke-linejoin="round">
        <circle cx="12" cy="12" r="3.2"/>
        <path d="M19.4 12.9a7.6 7.6 0 0 0 0-1.8l2-1.6-2-3.4-2.4 1a7.3 7.3 0 0 0-1.5-.9l-.4-2.6H10.9l-.4 2.6a7.3 7.3 0 0 0-1.5.9l-2.4-1-2 3.4 2 1.6a7.6 7.6 0 0 0 0 1.8l-2 1.6 2 3.4 2.4-1a7.3 7.3 0 0 0 1.5.9l.4 2.6h4.2l.4-2.6a7.3 7.3 0 0 0 1.5-.9l2.4 1 2-3.4z"/>
      </svg>
    </button>
    <button id="fs-exit" class="gear-btn" aria-label="Vollbild beenden">
      <svg class="ic" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.7" stroke-linecap="round" stroke-linejoin="round">
        <path d="M8 3v3a2 2 0 0 1-2 2H3M21 8h-3a2 2 0 0 1-2-2V3M16 21v-3a2 2 0 0 1 2-2h3M3 16h3a2 2 0 0 1 2 2v3"/>
      </svg>
    </button>
  </div>

  <!-- ══════════════ iOS „ZUM HOME-BILDSCHIRM" HINWEIS ══════════════ -->
  <div id="ios-hint" class="dialog-overlay hidden">
    <div class="dialog">
      <div class="dialog-emoji">📲</div>
      <div class="dialog-title">Vollbild auf dem iPhone</div>
      <div class="dialog-text">Safari erlaubt kein echtes Vollbild für Webseiten.<br><br>Tippe unten auf das <b>Teilen-Symbol</b> und dann auf <b>„Zum Home-Bildschirm"</b>. Danach startet der Rover randlos – mit dem HackFFM-Logo als App-Icon.</div>
      <button id="ios-hint-ok" class="dialog-btn">Verstanden</button>
    </div>
  </div>

  <!-- ══════════════ GAMEPAD-SUCHE DIALOG ══════════════ -->
  <div id="gp-dialog" class="dialog-overlay hidden">
    <div class="dialog">
      <div class="dialog-emoji">🎮</div>
      <div class="dialog-title">Gamepad suchen</div>
      <div class="dialog-text">Beliebige Taste am Gamepad drücken!</div>
      <button id="gp-dialog-cancel" class="dialog-btn">Abbrechen</button>
    </div>
  </div>

  <!-- ==============================================================
       JAVASCRIPT
  ============================================================== -->

  <!-- ============================================================
       JS BLOCK 0 — SHARED STATE & NETWORK LAYER
  ============================================================ -->
  <script>
  /* ── Command variables ── */
  let cmd_ml     = 0;
  let cmd_mr     = 0;
  let cmd_light  = 0;
  let cmd_servo1 = 0;
  let cmd_servo2 = 0;

  /* ── Parsed state from robot ── */
  let robotState = {
    Name: "camBot",
    Status: "",
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

  window.applyParsedState = function() {
    titleElement.textContent = "HackFFM-Bot: " + robotState.Name;
    document.title           = "HackFFM-Bot: " + robotState.Name;
    roboter_name_set = true;
    document.getElementById("topstatus").textContent = robotState.Status || "";
    document.getElementById("version-display").textContent = robotState.Version;
  };

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
       JS BLOCK 1 — STREAM (fullscreen) + JOYSTICK
  ============================================================ -->
  <script>
  (function() {

    /* ── Joystick constants ── */
    const JOY_W = 300, JOY_H = 300, STICK_RADIUS = 30;
    let   STICK_COLOR = "rgba(10,132,255,0.85)";

    const joystickCanvas = document.getElementById("joystick");
    const ctx            = joystickCanvas.getContext("2d");
    let   stick          = { x: 0, y: 0 };

    function updateCmdFromStick() {
      const nx =  stick.x / (JOY_W / 2);
      const ny = -stick.y / (JOY_H / 2);
      /* Sensitivität: Slider-Wert 0.25..3 → Exponent 1/Wert (links sanft, rechts stark) */
      const exponentY = 1 / (window.sensY || 1);
      const exponentX = 1 / (window.sensX || 1);
      const speed = Math.sign(ny) * Math.pow(Math.abs(ny), exponentY);
      const turn  = Math.sign(nx) * Math.pow(Math.abs(nx), exponentX);
      /* Langsam-Modus (Schildkröte): skaliert die Ausgabe */
      const tf = window.turtleActive ? (window.turtleSpeed || 40) / 100 : 1;
      let   ml    = Math.round((speed + turn) * 255 * tf);
      let   mr    = Math.round((speed - turn) * 255 * tf);
      ml = Math.max(-255, Math.min(255, ml));
      mr = Math.max(-255, Math.min(255, mr));
      cmd_ml = ml; cmd_mr = mr;
      document.getElementById('joy-coords').textContent =
        `x:${nx.toFixed(2)} y:${ny.toFixed(2)}   ml:${cmd_ml} mr:${cmd_mr}`;
    }

    function drawJoystick() {
      const cx = JOY_W / 2, cy = JOY_H / 2, R = cx - 6;
      ctx.clearRect(0, 0, JOY_W, JOY_H);
      /* outer disc */
      ctx.beginPath(); ctx.arc(cx, cy, R, 0, Math.PI * 2);
      ctx.fillStyle = "rgba(0,0,0,0.30)"; ctx.fill();
      ctx.lineWidth = 2; ctx.strokeStyle = "rgba(255,255,255,0.22)"; ctx.stroke();
      /* crosshair */
      ctx.strokeStyle = "rgba(255,255,255,0.12)"; ctx.lineWidth = 1;
      ctx.beginPath(); ctx.moveTo(cx, cy - R); ctx.lineTo(cx, cy + R); ctx.stroke();
      ctx.beginPath(); ctx.moveTo(cx - R, cy); ctx.lineTo(cx + R, cy); ctx.stroke();
      /* stick */
      ctx.beginPath(); ctx.arc(cx + stick.x, cy + stick.y, STICK_RADIUS, 0, Math.PI * 2);
      ctx.fillStyle = STICK_COLOR; ctx.fill();
      ctx.lineWidth = 3; ctx.strokeStyle = "rgba(255,255,255,0.85)"; ctx.stroke();
    }

    function clampStick(x, y) {
      const R = JOY_W / 2 - 6;
      const d = Math.hypot(x, y);
      if (d > R) { x = x * R / d; y = y * R / d; }
      return { x, y };
    }

    /* ── Joystick pointer events ── */
    let activePointerId = null;

    function onPointerDown(e) {
      joystickCanvas.setPointerCapture(e.pointerId);
      activePointerId = e.pointerId;
      joystickActive  = true;
      zeroSendCount   = 0;
      STICK_COLOR     = "rgba(10,132,255,1)";
      startHeartbeat();
      const rect = joystickCanvas.getBoundingClientRect();
      stick = clampStick(((e.clientX - rect.left) / rect.width)  * JOY_W - JOY_W / 2,
                         ((e.clientY - rect.top)  / rect.height) * JOY_H - JOY_H / 2);
      updateCmdFromStick();
      sendData(true);
      drawJoystick();
    }

    function onPointerMove(e) {
      if (e.pointerId !== activePointerId) return;
      const rect = joystickCanvas.getBoundingClientRect();
      stick = clampStick(((e.clientX - rect.left) / rect.width)  * JOY_W - JOY_W / 2,
                         ((e.clientY - rect.top)  / rect.height) * JOY_H - JOY_H / 2);
      updateCmdFromStick();
      sendData(false);
      drawJoystick();
    }

    function onPointerUp(e) {
      if (e.pointerId !== activePointerId) return;
      activePointerId = null;
      joystickActive  = false;
      zeroSendCount   = 0;
      STICK_COLOR     = "rgba(10,132,255,0.85)";
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
      if (gamepadActivityTimeout) { clearTimeout(gamepadActivityTimeout); }
      gamepadActivityTimeout = setTimeout(() => {
        gamepadActivityTimeout = null;
        STICK_COLOR = "rgba(10,132,255,0.85)";
        stick = { x: 0, y: 0 };
        updateCmdFromStick();
        sendData(false);
        drawJoystick();
      }, 8000);
      STICK_COLOR = "rgba(255,55,165,0.6)";
      stick = { x: nx * (JOY_W / 2), y: -ny * (JOY_H / 2) };
      updateCmdFromStick();
      sendData(false);
      drawJoystick();
    };

    window.triggerStickRelease = function() {
      if (gamepadActivityTimeout) { clearTimeout(gamepadActivityTimeout); gamepadActivityTimeout = null; }
      STICK_COLOR = "rgba(10,132,255,0.85)";
      stick = { x: 0, y: 0 };
      updateCmdFromStick();
      sendData(false);
      drawJoystick();
    };

    /* ── Stream placeholder ── */
    const placeholder = document.getElementById("stream-placeholder");
    placeholder.width  = 640;
    placeholder.height = 480;
    (function drawPlaceholder() {
      const pc = placeholder.getContext("2d");
      pc.fillStyle = "#000";   pc.fillRect(0, 0, 640, 480);
      pc.strokeStyle = "#1a1a1a"; pc.lineWidth = 1;
      for (let x = 0; x <= 640; x += 40) { pc.beginPath(); pc.moveTo(x, 0); pc.lineTo(x, 480); pc.stroke(); }
      for (let y = 0; y <= 480; y += 40) { pc.beginPath(); pc.moveTo(0, y); pc.lineTo(640, y); pc.stroke(); }
      pc.fillStyle = "#444"; pc.font = "20px -apple-system, sans-serif"; pc.textAlign = "center";
      pc.fillText("Kein Stream", 320, 240);
    })();

    /* ── Stream start/stop (fullscreen, object-fit handles scaling) ── */
    const photo = document.getElementById("photo");
    const btnStreamToggle = document.getElementById('btn-stream-toggle');
    let streamRunning    = false;
    let streamRetryDelay = 1000;

    function streamUrl() {
      return `${location.protocol}//${location.hostname}:81/stream?t=${Date.now()}`;
    }

    photo.addEventListener('load', () => {
      placeholder.style.display = "none";
      streamRetryDelay = 1000;
    });
    photo.addEventListener('error', () => {
      placeholder.style.display = "block";
      if (streamRunning) {
        setTimeout(() => { if (streamRunning) photo.src = streamUrl(); }, streamRetryDelay);
        streamRetryDelay = Math.min(streamRetryDelay * 2, 10000);
      }
    });

    function startStream() {
      streamRunning = true; streamRetryDelay = 1000;
      photo.src = streamUrl();
      btnStreamToggle.textContent = '⏹ Stream stoppen';
      btnStreamToggle.classList.add('running');
    }
    function stopStream() {
      streamRunning = false; photo.src = '';
      placeholder.style.display = "block";
      btnStreamToggle.textContent = '▶ Stream starten';
      btnStreamToggle.classList.remove('running');
    }
    btnStreamToggle.addEventListener('click', () => { streamRunning ? stopStream() : startStream(); });

    /* auto-start */
    startStream();
    drawJoystick();

  })();
  </script>

  <!-- ============================================================
       JS BLOCK 2 — DRIVE & LIGHT CONTROLS (in settings sheet)
  ============================================================ -->
  <script>
  (function() {

    const sliderLight     = document.getElementById("slider-light");
    const sliderServo1    = document.getElementById("slider-servo1");
    const sliderServo2    = document.getElementById("slider-servo2");
    const valLight        = document.getElementById("val-light");
    const valServo1       = document.getElementById("val-servo1");
    const valServo2       = document.getElementById("val-servo2");

    const sliderLightLow  = document.getElementById("slider-lightLow");
    const sliderLightHigh = document.getElementById("slider-lightHigh");
    const valLightLow     = document.getElementById("val-lightLow");
    const valLightHigh    = document.getElementById("val-lightHigh");

    const sliderS1Low     = document.getElementById("slider-servo1Low");
    const sliderS1High    = document.getElementById("slider-servo1High");
    const valS1Low        = document.getElementById("val-servo1Low");
    const valS1High       = document.getElementById("val-servo1High");

    const sliderS2Low     = document.getElementById("slider-servo2Low");
    const sliderS2High    = document.getElementById("slider-servo2High");
    const valS2Low        = document.getElementById("val-servo2Low");
    const valS2High       = document.getElementById("val-servo2High");

    let timerLight = null, timerS1 = null, timerS2 = null;

    const _origApply = window.applyParsedState || null;
    let rangeInitDone = false;

    function initRangesFromState() {
      if (rangeInitDone) return;
      rangeInitDone = true;
      const lowPct  = Math.round(robotState.lightLowValue  / 255 * 100);
      const highPct = Math.round(robotState.lightHighValue / 255 * 100);
      sliderLightLow.value  = lowPct;   valLightLow.textContent  = lowPct + "%";
      sliderLightHigh.value = highPct;  valLightHigh.textContent = highPct + "%";
      sliderS1Low.value  = robotState.Servo1LowValue;     valS1Low.textContent  = robotState.Servo1LowValue;
      sliderS1High.value = robotState.Servo1HighValue;    valS1High.textContent = robotState.Servo1HighValue;
      sliderS2Low.value  = robotState.Servo2LowValue;     valS2Low.textContent  = robotState.Servo2LowValue;
      sliderS2High.value = robotState.Servo2HighValue;    valS2High.textContent = robotState.Servo2HighValue;
    }

    function updateLightLabel() {
      const lo  = Math.round(robotState.lightLimitLowValue  / 255 * 100);
      const hi  = Math.round(robotState.lightLimitHighValue / 255 * 100);
      const bt  = robotState.lightBoostTime;
      const btm = robotState.lightBoostTimeMax;
      document.getElementById("label-light").textContent =
        `💡 Licht (0%…${lo}%…${hi}%, Boost: ${bt}s von ${btm}s)`;
    }

    window.applyParsedState = function() {
      if (_origApply) _origApply();
      initRangesFromState();
      updateLightLabel();
    };

    function scheduleLight() {
      clearTimeout(timerLight);
      timerLight = setTimeout(() => {
        robotState.lightLowValue  = Math.round(parseInt(sliderLightLow.value)  / 100 * 255);
        robotState.lightHighValue = Math.round(parseInt(sliderLightHigh.value) / 100 * 255);
        fetch('/action?lightLowValue=' + robotState.lightLowValue +
                      '&lightHighValue=' + robotState.lightHighValue).catch(() => {});
      }, 1000);
    }
    function scheduleS1() {
      clearTimeout(timerS1);
      timerS1 = setTimeout(() => {
        robotState.Servo1LowValue  = parseInt(sliderS1Low.value);
        robotState.Servo1HighValue = parseInt(sliderS1High.value);
        fetch('/action?Servo1LowValue=' + robotState.Servo1LowValue +
                      '&Servo1HighValue=' + robotState.Servo1HighValue).catch(() => {});
      }, 1000);
    }
    function scheduleS2() {
      clearTimeout(timerS2);
      timerS2 = setTimeout(() => {
        robotState.Servo2LowValue  = parseInt(sliderS2Low.value);
        robotState.Servo2HighValue = parseInt(sliderS2High.value);
        fetch('/action?Servo2LowValue=' + robotState.Servo2LowValue +
                      '&Servo2HighValue=' + robotState.Servo2HighValue).catch(() => {});
      }, 1000);
    }

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

    sliderLightLow.addEventListener("input", () => { valLightLow.textContent = sliderLightLow.value + "%"; scheduleLight(); });
    sliderLightHigh.addEventListener("input", () => { valLightHigh.textContent = sliderLightHigh.value + "%"; scheduleLight(); });
    sliderS1Low.addEventListener("input", () => { valS1Low.textContent = sliderS1Low.value; scheduleS1(); });
    sliderS1High.addEventListener("input", () => { valS1High.textContent = sliderS1High.value; scheduleS1(); });
    sliderS2Low.addEventListener("input", () => { valS2Low.textContent = sliderS2Low.value; scheduleS2(); });
    sliderS2High.addEventListener("input", () => { valS2High.textContent = sliderS2High.value; scheduleS2(); });

    /* ── Light: Off/Low toggle ── */
    let lightOffLowState = false;
    const btnOffLow = document.getElementById("btn-light-offlow");
    const btnBoost  = document.getElementById("btn-light-boost");

    function applyLightValue(raw255) {
      const pct = Math.round(raw255 / 255 * 100);
      sliderLight.value     = pct;
      valLight.textContent  = pct + "%";
      cmd_light             = raw255;
      robotState.lightValue = raw255;
      sendData(false);
    }

    btnOffLow.addEventListener("click", () => {
      lightOffLowState = !lightOffLowState;
      if (lightOffLowState) { btnOffLow.classList.add("active");  applyLightValue(robotState.lightLowValue); }
      else                  { btnOffLow.classList.remove("active"); applyLightValue(0); }
    });

    function boostOn()  { applyLightValue(robotState.lightHighValue); }
    function boostOff() { applyLightValue(lightOffLowState ? robotState.lightLowValue : 0); }
    btnBoost.addEventListener("pointerdown",  boostOn);
    btnBoost.addEventListener("pointerup",    boostOff);
    btnBoost.addEventListener("pointerleave", boostOff);
    btnBoost.addEventListener("pointercancel",boostOff);

    /* ── Servo 1 / 2: toggle Hoch/Runter ── */
    let servo1UpState = false, servo2UpState = false;
    function applyServo1Value(val) {
      sliderServo1.value = val; valServo1.textContent = val;
      cmd_servo1 = val; robotState.Servo1Value = val; sendData(false);
    }
    function applyServo2Value(val) {
      sliderServo2.value = val; valServo2.textContent = val;
      cmd_servo2 = val; robotState.Servo2Value = val; sendData(false);
    }

    const btnS1Toggle = document.getElementById("btn-servo1-toggle");
    const btnS2Toggle = document.getElementById("btn-servo2-toggle");

    function setServo1(up) {
      servo1UpState = up;
      applyServo1Value(up ? robotState.Servo1HighValue : robotState.Servo1LowValue);
      btnS1Toggle.classList.toggle("active", up);
      if (window.uiSyncServo1) window.uiSyncServo1(up);
    }
    function setServo2(up) {
      servo2UpState = up;
      applyServo2Value(up ? robotState.Servo2HighValue : robotState.Servo2LowValue);
      btnS2Toggle.classList.toggle("active", up);
      if (window.uiSyncServo2) window.uiSyncServo2(up);
    }

    window.uiServo1Toggle = () => { setServo1(!servo1UpState); return servo1UpState; };
    window.uiServo2Toggle = () => { setServo2(!servo2UpState); return servo2UpState; };
    btnS1Toggle.addEventListener("click", window.uiServo1Toggle);
    btnS2Toggle.addEventListener("click", window.uiServo2Toggle);

    /* ── Turtle (Langsam-Modus) Geschwindigkeit ── */
    const sliderTurtle = document.getElementById("slider-turtle");
    const valTurtle    = document.getElementById("val-turtle");
    let   turtleSaveTimer = null;
    window.turtleSpeed = parseInt(sliderTurtle.value) || 40;
    sliderTurtle.addEventListener("input", () => {
      window.turtleSpeed = parseInt(sliderTurtle.value);
      valTurtle.textContent = window.turtleSpeed + "%";
      clearTimeout(turtleSaveTimer);
      turtleSaveTimer = setTimeout(() => { fetch('/action?configDataWrite=1,' + window.turtleSpeed).catch(() => {}); }, 800);
    });
    fetch('/action?configDataRead=1').then(r => r.text()).then(t => {
      const v = parseInt(t.trim());
      if (!isNaN(v) && v > 0) { window.turtleSpeed = v; sliderTurtle.value = v; valTurtle.textContent = v + "%"; }
    }).catch(() => {});

    /* ── Joystick-Sensitivität X / Y (0.25 … 3.00, Default 1.00) ── */
    const sliderSensX = document.getElementById("slider-sensX");
    const sliderSensY = document.getElementById("slider-sensY");
    const valSensX    = document.getElementById("val-sensX");
    const valSensY    = document.getElementById("val-sensY");
    let   sensSaveTimer = null;
    window.sensX = parseFloat(sliderSensX.value) || 1;
    window.sensY = parseFloat(sliderSensY.value) || 1;
    function sensSave() {
      clearTimeout(sensSaveTimer);
      sensSaveTimer = setTimeout(() => {
        fetch('/action?configDataWrite=2,' + (window.sensX).toFixed(2) + ',' + (window.sensY).toFixed(2)).catch(() => {});
      }, 800);
    }
    sliderSensX.addEventListener("input", () => { window.sensX = parseFloat(sliderSensX.value); valSensX.textContent = window.sensX.toFixed(2); sensSave(); });
    sliderSensY.addEventListener("input", () => { window.sensY = parseFloat(sliderSensY.value); valSensY.textContent = window.sensY.toFixed(2); sensSave(); });
    fetch('/action?configDataRead=2').then(r => r.text()).then(t => {
      const p = t.trim().split(',');
      const sx = parseFloat(p[0]), sy = parseFloat(p[1]);
      if (!isNaN(sx) && sx > 0) { window.sensX = sx; sliderSensX.value = sx; valSensX.textContent = sx.toFixed(2); }
      if (!isNaN(sy) && sy > 0) { window.sensY = sy; sliderSensY.value = sy; valSensY.textContent = sy.toFixed(2); }
    }).catch(() => {});

    /* ── Servo: aktuelle Position in Hoch/Runter übernehmen ── */
    document.getElementById("cap-servo1Low").addEventListener("click", () => {
      sliderS1Low.value = sliderServo1.value; valS1Low.textContent = sliderS1Low.value; scheduleS1();
    });
    document.getElementById("cap-servo1High").addEventListener("click", () => {
      sliderS1High.value = sliderServo1.value; valS1High.textContent = sliderS1High.value; scheduleS1();
    });
    document.getElementById("cap-servo2Low").addEventListener("click", () => {
      sliderS2Low.value = sliderServo2.value; valS2Low.textContent = sliderS2Low.value; scheduleS2();
    });
    document.getElementById("cap-servo2High").addEventListener("click", () => {
      sliderS2High.value = sliderServo2.value; valS2High.textContent = sliderS2High.value; scheduleS2();
    });

    /* ── Public API for the fullscreen overlay light controls ── */
    window.uiLightSetRaw = applyLightValue;
    window.uiBoostOn   = boostOn;
    window.uiBoostOff  = boostOff;
    window.uiToggleLow = () => { btnOffLow.click(); return lightOffLowState; };
    window.uiIsLowOn   = () => lightOffLowState;

  })();
  </script>

  <!-- ============================================================
       JS BLOCK 3 — GAMEPAD
  ============================================================ -->
  <script>
  (function() {

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
    const search_gamepad_button_turtle       = document.getElementById('searchGamepadTurtle');
    const search_gamepad_output_turtle       = document.getElementById('outputGamepadTurtle');

    let selectedGamepadIndex   = null;
    let lastGamepadButtons     = null;
    let servo_1_index          = null;
    let servo_2_index          = null;
    let low_light_index        = null;
    let flash_light_index      = null;
    let turtle_index           = null;
    let gpdsel_activityTimeout = null;
    let searching              = false;
    let searchingServo1        = false;
    let searchingServo2        = false;
    let searchingLowLight      = false;
    let searchingFlashLight    = false;
    let searchingTurtle        = false;
    let configLoaded           = false;
    let gamepadSearchDone      = null;

    function format_fix2(v) { return parseFloat(v).toFixed(2); }

    function saveConfig() {
      if (!configLoaded) return;
      const parts = [
        gpdsel_xAxisSelect.value !== '' ? gpdsel_xAxisSelect.value : '0',
        gpdsel_yAxisSelect.value !== '' ? gpdsel_yAxisSelect.value : '0',
        gpdsel_invertX.checked ? '1' : '0',
        gpdsel_invertY.checked ? '1' : '0',
        servo_1_index     !== null ? servo_1_index     : '-1',
        servo_2_index     !== null ? servo_2_index     : '-1',
        low_light_index   !== null ? low_light_index   : '-1',
        flash_light_index !== null ? flash_light_index : '-1',
        turtle_index      !== null ? turtle_index      : '-1'
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
            const xAxis = parseInt(parts[0]); const yAxis = parseInt(parts[1]);
            const invX = parts[2] === '1';    const invY = parts[3] === '1';
            const s1 = parseInt(parts[4]);    const s2 = parseInt(parts[5]);
            const lowL = parseInt(parts[6]);  const flashL = parseInt(parts[7]);
            if (!isNaN(xAxis)) gpdsel_xAxisSelect.value = xAxis;
            if (!isNaN(yAxis)) gpdsel_yAxisSelect.value = yAxis;
            gpdsel_invertX.checked = invX; gpdsel_invertY.checked = invY;
            if (!isNaN(s1) && s1 >= 0) { servo_1_index = s1; search_gamepad_output_servo_1.textContent = 'Taste für Servo 1: ' + s1; }
            if (!isNaN(s2) && s2 >= 0) { servo_2_index = s2; search_gamepad_output_servo_2.textContent = 'Taste für Servo 2: ' + s2; }
            if (!isNaN(lowL) && lowL >= 0) { low_light_index = lowL; search_gamepad_output_low_light.textContent = 'Taste für Schwachlicht: ' + lowL; }
            if (!isNaN(flashL) && flashL >= 0) { flash_light_index = flashL; search_gamepad_output_flash_light.textContent = 'Taste für Blitzlicht: ' + flashL; }
            const turt = parseInt(parts[8]);
            if (!isNaN(turt) && turt >= 0) { turtle_index = turt; search_gamepad_output_turtle.textContent = 'Taste für Langsam-Modus: ' + turt; }
          }
        })
        .catch(err => console.warn('configDataRead failed:', err))
        .finally(() => { configLoaded = true; });
    }

    gpdsel_xAxisSelect.addEventListener('change', saveConfig);
    gpdsel_yAxisSelect.addEventListener('change', saveConfig);
    gpdsel_invertX.addEventListener('change',     saveConfig);
    gpdsel_invertY.addEventListener('change',     saveConfig);

    function rebuildGamepadDropdown() {
      const gamepads = navigator.getGamepads();
      gpdsel_gamepadSelect.innerHTML = '<option value="">-- wählen --</option>';
      for (let i = 0; i < gamepads.length; i++) {
        if (gamepads[i]) {
          const opt = document.createElement('option');
          opt.value = i; opt.textContent = gamepads[i].id || ('Gamepad ' + i);
          if (i === selectedGamepadIndex) opt.selected = true;
          gpdsel_gamepadSelect.appendChild(opt);
        }
      }
    }

    gpdsel_gamepadSelect.addEventListener('change', () => {
      const v = gpdsel_gamepadSelect.value;
      if (v === '') { selectedGamepadIndex = null; search_gamepad_output.textContent = 'Kein Gamepad gewählt'; }
      else {
        selectedGamepadIndex = parseInt(v);
        const gp = navigator.getGamepads()[selectedGamepadIndex];
        search_gamepad_output.textContent = 'Gewählt: ' + (gp ? gp.id : 'Gamepad ' + selectedGamepadIndex);
      }
    });

    window.addEventListener('gamepadconnected',    rebuildGamepadDropdown);
    window.addEventListener('gamepaddisconnected', rebuildGamepadDropdown);

    function gamepadLoop() {
      const gamepads = navigator.getGamepads();
      if (selectedGamepadIndex === null) { requestAnimationFrame(gamepadLoop); return; }
      const gp = gamepads[selectedGamepadIndex];
      if (!gp) { requestAnimationFrame(gamepadLoop); return; }

      const axes = gp.axes;
      gpdsel_axesDisplay.textContent = axes.map((v, i) => 'A' + i + ':' + format_fix2(v)).join('  ');

      let gx = axes[parseInt(gpdsel_xAxisSelect.value)] || 0;
      let gy = axes[parseInt(gpdsel_yAxisSelect.value)] || 0;
      if (gpdsel_invertX.checked) gx *= -1;
      if (gpdsel_invertY.checked) gy *= -1;
      gpdsel_xVal.textContent = format_fix2(gx);
      gpdsel_yVal.textContent = format_fix2(gy);

      if (Math.abs(gx) > 0.1 || Math.abs(gy) > 0.1) {
        if (!gpdsel_activityTimeout) {
          gpdsel_activityTimeout = setTimeout(() => {
            gpdsel_activityTimeout = null; window.triggerStickRelease();
          }, 8000);
        }
      }
      if (gpdsel_activityTimeout !== null) { window.setStickFromGamepad(gx, gy); }

      const buttons = gp.buttons;
      if (lastGamepadButtons) {
        for (let j = 0; j < gp.buttons.length; j++) {
          const pressed   = gp.buttons[j].pressed;
          const wasPessed = lastGamepadButtons[j];
          if (pressed && !wasPessed) {
            if (j === servo_1_index && window.uiServo1Toggle) window.uiServo1Toggle();
            if (j === servo_2_index && window.uiServo2Toggle) window.uiServo2Toggle();
            if (j === low_light_index) document.getElementById("btn-light-offlow").click();
            if (j === turtle_index) document.getElementById("ov-turtle").click();
          }
          if (j === flash_light_index) {
            if (pressed && !wasPessed) document.getElementById("btn-light-boost").dispatchEvent(new PointerEvent("pointerdown"));
            if (!pressed && wasPessed) document.getElementById("btn-light-boost").dispatchEvent(new PointerEvent("pointerup"));
          }
        }
      }
      lastGamepadButtons = buttons.map(b => b.pressed);
      requestAnimationFrame(gamepadLoop);
    }
    requestAnimationFrame(gamepadLoop);

    search_gamepad_button.addEventListener('click', () => {
      searching = true; search_gamepad_output.textContent = 'Beliebige Taste am Gamepad drücken …';
      requestAnimationFrame(checkGamepad);
    });
    function checkGamepad() {
      if (!searching) return;
      const gamepads = navigator.getGamepads();
      for (let i = 0; i < gamepads.length; i++) {
        const gp = gamepads[i]; if (!gp) continue;
        for (let j = 0; j < gp.buttons.length; j++) {
          if (gp.buttons[j].pressed) {
            searching = false; selectedGamepadIndex = i;
            search_gamepad_output.textContent = 'Gefunden: ' + gp.id;
            rebuildGamepadDropdown();
            gpdsel_gamepadSelect.value = String(i);
            gpdsel_gamepadSelect.dispatchEvent(new Event("change"));
            if (gamepadSearchDone) { const cb = gamepadSearchDone; gamepadSearchDone = null; cb(gp.id); }
            return;
          }
        }
      }
      requestAnimationFrame(checkGamepad);
    }

    search_gamepad_button_servo_1.addEventListener('click', () => {
      if (selectedGamepadIndex === null) { search_gamepad_output_servo_1.textContent = 'Erst Gamepad wählen!'; return; }
      searchingServo1 = true; search_gamepad_output_servo_1.textContent = 'Taste für Servo 1 drücken …';
      requestAnimationFrame(checkGamepadServo1);
    });
    function checkGamepadServo1() {
      if (!searchingServo1) return;
      const gp = navigator.getGamepads()[selectedGamepadIndex]; if (!gp) return;
      for (let j = 0; j < gp.buttons.length; j++) {
        if (gp.buttons[j].pressed) {
          searchingServo1 = false; servo_1_index = j;
          search_gamepad_output_servo_1.textContent = 'Taste für Servo 1: ' + j; saveConfig(); return;
        }
      }
      requestAnimationFrame(checkGamepadServo1);
    }

    search_gamepad_button_servo_2.addEventListener('click', () => {
      if (selectedGamepadIndex === null) { search_gamepad_output_servo_2.textContent = 'Erst Gamepad wählen!'; return; }
      searchingServo2 = true; search_gamepad_output_servo_2.textContent = 'Taste für Servo 2 drücken …';
      requestAnimationFrame(checkGamepadServo2);
    });
    function checkGamepadServo2() {
      if (!searchingServo2) return;
      const gp = navigator.getGamepads()[selectedGamepadIndex]; if (!gp) return;
      for (let j = 0; j < gp.buttons.length; j++) {
        if (gp.buttons[j].pressed) {
          searchingServo2 = false; servo_2_index = j;
          search_gamepad_output_servo_2.textContent = 'Taste für Servo 2: ' + j; saveConfig(); return;
        }
      }
      requestAnimationFrame(checkGamepadServo2);
    }

    search_gamepad_button_low_light.addEventListener('click', () => {
      if (selectedGamepadIndex === null) { search_gamepad_output_low_light.textContent = 'Erst Gamepad wählen!'; return; }
      searchingLowLight = true; search_gamepad_output_low_light.textContent = 'Taste für Schwachlicht drücken …';
      requestAnimationFrame(checkGamepadLowLight);
    });
    function checkGamepadLowLight() {
      if (!searchingLowLight) return;
      const gp = navigator.getGamepads()[selectedGamepadIndex]; if (!gp) return;
      for (let j = 0; j < gp.buttons.length; j++) {
        if (gp.buttons[j].pressed) {
          searchingLowLight = false; low_light_index = j;
          search_gamepad_output_low_light.textContent = 'Taste für Schwachlicht: ' + j; saveConfig(); return;
        }
      }
      requestAnimationFrame(checkGamepadLowLight);
    }

    search_gamepad_button_flash_light.addEventListener('click', () => {
      if (selectedGamepadIndex === null) { search_gamepad_output_flash_light.textContent = 'Erst Gamepad wählen!'; return; }
      searchingFlashLight = true; search_gamepad_output_flash_light.textContent = 'Taste für Blitzlicht drücken …';
      requestAnimationFrame(checkGamepadFlashLight);
    });
    function checkGamepadFlashLight() {
      if (!searchingFlashLight) return;
      const gp = navigator.getGamepads()[selectedGamepadIndex]; if (!gp) return;
      for (let j = 0; j < gp.buttons.length; j++) {
        if (gp.buttons[j].pressed) {
          searchingFlashLight = false; flash_light_index = j;
          search_gamepad_output_flash_light.textContent = 'Taste für Blitzlicht: ' + j; saveConfig(); return;
        }
      }
      requestAnimationFrame(checkGamepadFlashLight);
    }

    search_gamepad_button_turtle.addEventListener('click', () => {
      if (selectedGamepadIndex === null) { search_gamepad_output_turtle.textContent = 'Erst Gamepad wählen!'; return; }
      searchingTurtle = true; search_gamepad_output_turtle.textContent = 'Taste für Langsam-Modus drücken …';
      requestAnimationFrame(checkGamepadTurtle);
    });
    function checkGamepadTurtle() {
      if (!searchingTurtle) return;
      const gp = navigator.getGamepads()[selectedGamepadIndex]; if (!gp) return;
      for (let j = 0; j < gp.buttons.length; j++) {
        if (gp.buttons[j].pressed) {
          searchingTurtle = false; turtle_index = j;
          search_gamepad_output_turtle.textContent = 'Taste für Langsam-Modus: ' + j; saveConfig(); return;
        }
      }
      requestAnimationFrame(checkGamepadTurtle);
    }

    window.uiGamepadSearch = function(onDone) {
      gamepadSearchDone = onDone || null;
      searching = true;
      search_gamepad_output.textContent = 'Beliebige Taste am Gamepad drücken …';
      requestAnimationFrame(checkGamepad);
    };
    window.uiGamepadSearchCancel = function() { searching = false; gamepadSearchDone = null; };

    window.uiSaveGamepad = saveConfig;
    loadConfig();

  })();
  </script>

  <!-- ============================================================
       JS BLOCK 4 — ACTIONS, SETTINGS SHEET, OVERLAY CONTROLS
  ============================================================ -->
  <script>
  (function() {

    /* ── System action buttons ── */
    document.getElementById('btn-reconnect').addEventListener('click', () => fetch('/action?reconnect=1').catch(()=>{}));
    document.getElementById('btn-trigger').addEventListener('click',   () => fetch('/action?trig').catch(()=>{}));
    document.getElementById('btn-restart').addEventListener('click',   () => { if (confirm('Rover neu starten?')) fetch('/action?restart=1').catch(()=>{}); });
    document.getElementById('btn-shutdown').addEventListener('click',  () => { if (confirm('Rover ausschalten?')) window.location.href = '/action?shutdown'; });

    /* ── Settings panel open/close (Kamera bleibt oben live) ── */
    document.getElementById('btn-settings').addEventListener('click', () => document.body.classList.add('settings-open'));
    document.getElementById('btn-settings-back').addEventListener('click', () => document.body.classList.remove('settings-open'));

    /* ── Topbar: Gamepad-Suche-Dialog (analog zur Suche in den Einstellungen) ── */
    const gpDialog = document.getElementById('gp-dialog');
    document.getElementById('btn-gamepad').addEventListener('click', () => {
      gpDialog.classList.remove('hidden');
      if (window.uiGamepadSearch) window.uiGamepadSearch(() => gpDialog.classList.add('hidden'));
    });
    document.getElementById('gp-dialog-cancel').addEventListener('click', () => {
      if (window.uiGamepadSearchCancel) window.uiGamepadSearchCancel();
      gpDialog.classList.add('hidden');
    });

    /* ── Vollbild / Immersiv-Modus ── */
    function enterFs() {
      document.body.classList.add('fullscreen-mode');
      const el = document.documentElement;
      const req = el.requestFullscreen || el.webkitRequestFullscreen;
      if (req) { try { req.call(el); } catch (e) {} }
    }
    function exitFs() {
      document.body.classList.remove('fullscreen-mode');
      const ex = document.exitFullscreen || document.webkitExitFullscreen;
      if (ex && (document.fullscreenElement || document.webkitFullscreenElement)) { try { ex.call(document); } catch (e) {} }
    }
    const isIOS = /iPad|iPhone|iPod/.test(navigator.userAgent) || (navigator.platform === 'MacIntel' && navigator.maxTouchPoints > 1);
    const isStandalone = (window.navigator.standalone === true) || window.matchMedia('(display-mode: standalone)').matches;
    const iosHint = document.getElementById('ios-hint');
    document.getElementById('btn-fullscreen').addEventListener('click', () => {
      if (isIOS && !isStandalone) iosHint.classList.remove('hidden');
      else enterFs();
    });
    document.getElementById('ios-hint-ok').addEventListener('click', () => iosHint.classList.add('hidden'));
    document.getElementById('fs-exit').addEventListener('click', exitFs);
    document.getElementById('fs-gamepad').addEventListener('click', () => document.getElementById('btn-gamepad').click());
    document.getElementById('fs-settings').addEventListener('click', () => document.getElementById('btn-settings').click());
    document.addEventListener('fullscreenchange', () => { if (!document.fullscreenElement) document.body.classList.remove('fullscreen-mode'); });
    document.addEventListener('webkitfullscreenchange', () => { if (!document.webkitFullscreenElement) document.body.classList.remove('fullscreen-mode'); });

    /* ── Tab switching ── */
    document.querySelectorAll('.seg').forEach(btn => {
      btn.addEventListener('click', () => {
        document.querySelectorAll('.seg').forEach(b => b.classList.remove('active'));
        btn.classList.add('active');
        const tab = btn.dataset.tab;
        document.getElementById('tab-control').classList.toggle('hidden', tab !== 'control');
        document.getElementById('tab-gamepad').classList.toggle('hidden', tab !== 'gamepad');
      });
    });

    /* ── Camera configuration (rotation, size, fps, quality) ── */
    const camSize     = document.getElementById('cam-size');
    const camFps      = document.getElementById('cam-fps');
    const camQuality  = document.getElementById('cam-quality');
    const camRotation = document.getElementById('cam-rotation');
    let camLoaded = false;

    function camLoad() {
      fetch('/action?cameraConfigRead')
        .then(r => r.text())
        .then(t => {
          const p = t.trim().split(',');   /* rotation,size,fps,quality */
          if (p.length >= 4) {
            camRotation.checked = p[0] === '1';
            camSize.value = p[1]; camFps.value = p[2]; camQuality.value = p[3];
          }
          camLoaded = true;
        })
        .catch(() => { camLoaded = true; });
    }
    function camSave() {
      if (!camLoaded) return;
      const v = `${camRotation.checked ? 1 : 0},${camSize.value},${camFps.value},${camQuality.value}`;
      fetch('/action?cameraConfigWrite=' + v).catch(() => {});
    }
    camSize.addEventListener('change', camSave);
    camFps.addEventListener('change', camSave);
    camQuality.addEventListener('change', camSave);
    camRotation.addEventListener('change', camSave);
    camLoad();

    /* ── Servo 2: only show config + overlay button if a pin is defined ── */
    fetch('/action?pwmThingRead=3')
      .then(r => r.text())
      .then(t => {
        const p = t.split(',');          /* name,pinA,pinB,thingType,inverted,... */
        const pinA = parseInt(p[1]), pinB = parseInt(p[2]);
        const available = (pinA >= 0) || (pinB >= 0);
        const cfg = document.getElementById('servo2-config');
        const pad = document.getElementById('servo2-pad');
        if (cfg) cfg.classList.toggle('hidden', !available);
        if (pad) pad.classList.toggle('hidden', !available);
      })
      .catch(() => {});

    /* ── Auto-Power-Down (Minuten) lesen/schreiben ── */
    const inPowerdown = document.getElementById('input-powerdown');
    const pdStatus = document.getElementById('powerdown-status');
    function updatePowerdownUI() {
      const v = parseInt(inPowerdown.value);
      if (v === 0) { pdStatus.textContent = 'deaktiviert'; pdStatus.style.color = 'var(--accent-orange)'; }
      else { pdStatus.textContent = 'Min.'; pdStatus.style.color = 'var(--text2)'; }
    }
    fetch('/action?powerDownRead')
      .then(r => r.text())
      .then(t => { const v = parseInt(t.trim()); if (!isNaN(v) && v >= 0) inPowerdown.value = v; updatePowerdownUI(); })
      .catch(() => {});
    inPowerdown.addEventListener('input', updatePowerdownUI);
    inPowerdown.addEventListener('change', () => {
      const v = parseInt(inPowerdown.value);
      if (!isNaN(v) && v >= 0) fetch('/action?powerDownWrite=' + v).catch(() => {});
      updatePowerdownUI();
    });
    updatePowerdownUI();

    /* ── Save button: persist all settings to the robot ── */
    const btnSave = document.getElementById('btn-settings-save');
    btnSave.addEventListener('click', () => {
      const g = id => document.getElementById(id).value;
      const lightLow255  = Math.round(parseInt(g('slider-lightLow'))  / 100 * 255);
      const lightHigh255 = Math.round(parseInt(g('slider-lightHigh')) / 100 * 255);
      fetch(`/action?lightLowValue=${lightLow255}&lightHighValue=${lightHigh255}`).catch(() => {});
      fetch(`/action?Servo1LowValue=${g('slider-servo1Low')}&Servo1HighValue=${g('slider-servo1High')}`).catch(() => {});
      fetch(`/action?Servo2LowValue=${g('slider-servo2Low')}&Servo2HighValue=${g('slider-servo2High')}`).catch(() => {});
      fetch(`/action?configDataWrite=1,${window.turtleSpeed || 40}`).catch(() => {});
      fetch(`/action?configDataWrite=2,${(window.sensX || 1).toFixed(2)},${(window.sensY || 1).toFixed(2)}`).catch(() => {});
      const pdMin = parseInt(g('input-powerdown'));
      if (!isNaN(pdMin) && pdMin >= 0) fetch('/action?powerDownWrite=' + pdMin).catch(() => {});
      camSave();
      if (window.uiSaveGamepad) window.uiSaveGamepad();
      const prev = btnSave.textContent;
      btnSave.textContent = 'Gespeichert ✓';
      btnSave.classList.add('saved');
      setTimeout(() => { btnSave.textContent = prev; btnSave.classList.remove('saved'); }, 1500);
    });

    /* ── Overlay light: switch "Schwach" (toggle) + "Hell" (momentary) ── */
    const ovLow = document.getElementById('ov-light-low');
    ovLow.addEventListener('click', () => {
      const on = window.uiToggleLow ? window.uiToggleLow() : false;
      ovLow.classList.toggle('active', !!on);
    });

    const ovBoost = document.getElementById('ov-light-boost');
    const boostStart = (e) => { e.preventDefault(); if (window.uiBoostOn) window.uiBoostOn(); ovBoost.classList.add('boost'); };
    const boostEnd   = ()  => { if (window.uiBoostOff) window.uiBoostOff(); ovBoost.classList.remove('boost'); };
    ovBoost.addEventListener('pointerdown',  boostStart);
    ovBoost.addEventListener('pointerup',    boostEnd);
    ovBoost.addEventListener('pointerleave', boostEnd);
    ovBoost.addEventListener('pointercancel',boostEnd);

    /* ── Overlay: servo toggles (switch Hoch/Runter) ── */
    const ovS1 = document.getElementById('ov-s1-toggle');
    const ovS2 = document.getElementById('ov-s2-toggle');
    window.uiSyncServo1 = (up) => { ovS1.classList.toggle('active', up); ovS1.classList.toggle('down', !up); };
    window.uiSyncServo2 = (up) => { ovS2.classList.toggle('active', up); ovS2.classList.toggle('down', !up); };
    ovS1.addEventListener('click', () => window.uiServo1Toggle && window.uiServo1Toggle());
    ovS2.addEventListener('click', () => window.uiServo2Toggle && window.uiServo2Toggle());

    /* ── Overlay: turtle (Langsam-Modus) toggle ── */
    window.turtleActive = false;
    const ovTurtle = document.getElementById('ov-turtle');
    ovTurtle.addEventListener('click', () => {
      window.turtleActive = !window.turtleActive;
      ovTurtle.classList.toggle('active', window.turtleActive);
    });

    /* ── Boost gauge: bolt icon fills by remaining boost time ── */
    const boostFill = document.getElementById('boostFill');
    function updateBoostGauge() {
      if (!boostFill) return;
      const max = robotState.lightBoostTimeMax || 30;
      let f = max > 0 ? robotState.lightBoostTime / max : 0;
      f = Math.max(0, Math.min(1, f));
      /* round button fills from the bottom up; colour by remaining boost time */
      boostFill.style.height = (f * 100).toFixed(1) + '%';
      boostFill.style.background = f > 0.5 ? '#30d158' : f > 0.2 ? '#ffd60a' : '#ff453a';
    }
    setInterval(updateBoostGauge, 500);
    updateBoostGauge();

  })();
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