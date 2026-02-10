#ifndef WEB_PAGE_H
#define WEB_PAGE_H

const char index_html[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Smart Poultry Node</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: Arial, sans-serif; margin: 0; padding: 20px; background: #f4f4f9; }
    .container { max-width: 600px; margin: auto; background: white; padding: 20px; border-radius: 8px; box-shadow: 0 2px 5px rgba(0,0,0,0.1); }
    h2 { color: #333; border-bottom: 2px solid #ddd; padding-bottom: 10px; }
    .card { background: #eee; padding: 15px; border-radius: 5px; margin-bottom: 15px; }
    .sensor-val { font-size: 1.2em; font-weight: bold; color: #007bff; }
    label { display: block; margin-top: 10px; font-weight: bold; }
    input[type=text], input[type=password] { width: 100%; padding: 8px; margin-top: 5px; border: 1px solid #ccc; border-radius: 4px; box-sizing: border-box; }
    button { background: #28a745; color: white; padding: 10px 15px; border: none; border-radius: 4px; cursor: pointer; margin-top: 15px; width: 100%; }
    button:hover { background: #218838; }
    .switch { position: relative; display: inline-block; width: 50px; height: 24px; float: right; }
    .switch input { opacity: 0; width: 0; height: 0; }
    .slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; transition: .4s; border-radius: 24px; }
    .slider:before { position: absolute; content: ""; height: 16px; width: 16px; left: 4px; bottom: 4px; background-color: white; transition: .4s; border-radius: 50%; }
    input:checked + .slider { background-color: #2196F3; }
    input:checked + .slider:before { transform: translateX(26px); }
    .row { display: flex; justify-content: space-between; align-items: center; margin-bottom: 10px; }
  </style>
</head>
<body>
  <div class="container">
    <h2>Poultry Node Status</h2>
    <div id="status-container">Loading...</div>

    <h2>Controls</h2>
    <div class="card">
      <div class="row"><span>Fan</span> <label class="switch"><input type="checkbox" id="btn-fan" onchange="toggle('Fan', this)"><span class="slider"></span></label></div>
      <div class="row"><span>Heater</span> <label class="switch"><input type="checkbox" id="btn-heater" onchange="toggle('Heater', this)"><span class="slider"></span></label></div>
      <div class="row"><span>Light</span> <label class="switch"><input type="checkbox" id="btn-light" onchange="toggle('Light', this)"><span class="slider"></span></label></div>
    </div>

    <h2>Configuration</h2>
    <div class="card">
      <h3>WiFi</h3>
      <form onsubmit="saveWifi(event)">
        <label>WiFi SSID</label>
        <input type="text" id="ssid" placeholder="Enter SSID">
        <label>WiFi Password</label>
        <input type="password" id="pass" placeholder="Enter Password">
        <button type="submit">Save & Reboot</button>
      </form>
    </div>

    <div class="card">
      <h3>LoRa</h3>
      <form onsubmit="saveLoRa(event)">
        <label>Frequency (Hz)</label>
        <input type="text" id="lora-freq" placeholder="e.g. 433000000">
        <label>Spreading Factor (6-12)</label>
        <input type="text" id="lora-sf" placeholder="e.g. 7">
        <label>Bandwidth (Hz)</label>
        <input type="text" id="lora-bw" placeholder="e.g. 125000">
        <button type="submit">Save LoRa Config</button>
      </form>
    </div>
  </div>

<script>
function fetchStatus() {
  fetch('/api/status').then(res => res.json()).then(data => {
    let html = '';
    html += `<div class="row"><span>Temperature</span> <span class="sensor-val">${data.temp.toFixed(1)} &deg;C</span></div>`;
    html += `<div class="row"><span>Humidity</span> <span class="sensor-val">${data.hum.toFixed(1)} %</span></div>`;
    html += `<div class="row"><span>Ammonia (NH3)</span> <span class="sensor-val">${data.nh3.toFixed(1)} ppm</span></div>`;
    html += `<div class="row"><span>Feed Level</span> <span class="sensor-val">${data.feed.toFixed(1)} cm</span></div>`;
    document.getElementById('status-container').innerHTML = html;
    
    document.getElementById('btn-fan').checked = data.fan;
    document.getElementById('btn-heater').checked = data.heater;
    document.getElementById('btn-light').checked = data.light;
  });
}

function fetchLoRa() {
    fetch('/api/config/lora').then(res => res.json()).then(data => {
        document.getElementById('lora-freq').value = data.freq;
        document.getElementById('lora-sf').value = data.sf;
        document.getElementById('lora-bw').value = data.bw;
    });
}

function toggle(device, el) {
  fetch('/api/control', {
    method: 'POST',
    headers: {'Content-Type': 'application/json'},
    body: JSON.stringify({device: device, action: el.checked ? 'on' : 'off'})
  });
}

function saveWifi(e) {
  e.preventDefault();
  const ssid = document.getElementById('ssid').value;
  const pass = document.getElementById('pass').value;
  if(!ssid) return alert("SSID required");
  
  fetch('/api/config/wifi', {
    method: 'POST',
    headers: {'Content-Type': 'application/json'},
    body: JSON.stringify({ssid: ssid, pass: pass})
  }).then(res => {
    alert("Saved! Rebooting...");
  });
}

function saveLoRa(e) {
    e.preventDefault();
    const freq = document.getElementById('lora-freq').value;
    const sf = document.getElementById('lora-sf').value;
    const bw = document.getElementById('lora-bw').value;
    
    fetch('/api/config/lora', {
        method: 'POST',
        headers: {'Content-Type': 'application/json'},
        body: JSON.stringify({freq: parseInt(freq), sf: parseInt(sf), bw: parseInt(bw)})
    }).then(res => {
        alert("LoRa Config Saved!");
    });
}

setInterval(fetchStatus, 2000);
fetchStatus();
fetchLoRa();
</script>
</body>
</html>
)rawliteral";

#endif // WEB_PAGE_H
