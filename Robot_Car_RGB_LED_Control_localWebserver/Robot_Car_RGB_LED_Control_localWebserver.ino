#include <WiFi.h>
#include <WebServer.h>
#include <FastLED.h>

const int in1 = 26;
const int in2 = 27;
const int in3 = 14;
const int in4 = 12;

const int MOTOR_ENABLE_PIN = 32;

#define LED_PIN 15
#define NUM_LEDS 2
CRGB leds[NUM_LEDS];

const char* ssid = "ESP32_CAR";
const char* password = "12345678";

WebServer server(80);

int redValue = 0;
int greenValue = 0;
int blueValue = 0;
bool lightsOn = false;

void handleRoot();
void forward();
void backward();
void left();
void right();
void stopCar();
void handleLights();
void handleRGB();
void handleStatus();
void setRGB(int r, int g, int b);
void toggleLights();


void setup() {
  Serial.begin(115200);

  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(MOTOR_ENABLE_PIN, OUTPUT);
  digitalWrite(MOTOR_ENABLE_PIN, HIGH);

  FastLED.addLeds<WS2812B, LED_PIN, RGB>(leds, NUM_LEDS);
  setRGB(0, 0, 0);

  WiFi.softAP(ssid, password);
  Serial.println("WiFi AP Started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/F", forward);
  server.on("/B", backward);
  server.on("/L", left);
  server.on("/R", right);
  server.on("/S", stopCar);
  server.on("/lights", handleLights);
  server.on("/rgb", handleRGB);
  server.on("/status", handleStatus);

  server.begin();
}

void loop() {
  server.handleClient();
}


void setRGB(int r, int g, int b) {
  redValue = r;
  greenValue = g;
  blueValue = b;

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(r, g, b);
  }

  FastLED.show();
}

void toggleLights() {
  lightsOn = !lightsOn;
  if (lightsOn) {
    if (redValue == 0 && greenValue == 0 && blueValue == 0) {
      setRGB(255, 255, 255);
    } else {
      setRGB(redValue, greenValue, blueValue);
    }
  } else {
    setRGB(0, 0, 0);
  }
}

void handleLights() {
  String response = "";
  if (server.hasArg("state")) {
    String state = server.arg("state");
    if (state == "on") {
      if (redValue == 0 && greenValue == 0 && blueValue == 0) {
        setRGB(255, 255, 255);
      } else {
        setRGB(redValue, greenValue, blueValue);
      }
      lightsOn = true;
      response = "Lights ON";
    } else {
      setRGB(0, 0, 0);
      lightsOn = false;
      response = "Lights OFF";
    }
  } else {
    toggleLights();
    response = lightsOn ? "Lights ON" : "Lights OFF";
  }
  server.send(200, "text/plain", response);
}

void handleRGB() {
  if (server.hasArg("r") && server.hasArg("g") && server.hasArg("b")) {
    int r = server.arg("r").toInt();
    int g = server.arg("g").toInt();
    int b = server.arg("b").toInt();

    setRGB(r, g, b);
    lightsOn = (r > 0 || g > 0 || b > 0);

    String response = "RGB set to: " + String(r) + "," + String(g) + "," + String(b);
    server.send(200, "text/plain", response);
  } else {
    server.send(400, "text/plain", "Missing RGB parameters");
  }
}


void forward() {

  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  server.send(200, "text/plain", "Forward");
}

void backward() {

  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  server.send(200, "text/plain", "Backward");
}

void left() {

  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  server.send(200, "text/plain", "Left");
}

void right() {

  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  server.send(200, "text/plain", "Right");
}

void stopCar() {

  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  server.send(200, "text/plain", "Stop");
}


void handleStatus() {
  String json = "{";
  json += "\"ip\":\"" + WiFi.softAPIP().toString() + "\",";
  json += "\"clients\":" + String(WiFi.softAPgetStationNum()) + ",";
  json += "\"lights\":" + String(lightsOn ? "true" : "false") + ",";
  json += "\"rgb\":[" + String(redValue) + "," + String(greenValue) + "," + String(blueValue) + "]";
  json += "}";

  server.send(200, "application/json", json);
}

void handleRoot() {
  String ipAddress = WiFi.softAPIP().toString();
  String html = R"rawliteral(
    <!DOCTYPE html><html>
    <head>
      <title>ESP32 Car Control</title>
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body {
          font-family: sans-serif;
          background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
          min-height: 100vh;
          padding: 20px;
          color: #333;
        }
        .container {
          max-width: 500px; margin: 0 auto; background: white;
          border-radius: 20px; padding: 30px; box-shadow: 0 10px 30px rgba(0,0,0,0.3);
        }
        h1 { text-align: center; margin-bottom: 10px; color: #4a5568; font-size: 28px; }
        .subtitle { text-align: center; color: #718096; margin-bottom: 30px; font-size: 14px; }
        .control-section {
          margin-bottom: 30px; padding: 20px; background: #f7fafc;
          border-radius: 15px; border: 2px solid #e2e8f0;
        }
        .section-title { font-size: 18px; font-weight: 600; margin-bottom: 15px; color: #4a5568; text-align: center; }
        .control-grid {
          display: grid; grid-template-columns: 1fr 1fr 1fr;
          grid-template-rows: auto auto; gap: 10px; margin-bottom: 15px;
        }
        .control-button {
          padding: 15px; border: none; border-radius: 12px; font-size: 16px;
          font-weight: 600; cursor: pointer; transition: all 0.15s ease;
          color: white; box-shadow: 0 4px 6px rgba(0,0,0,0.1);
          text-decoration: none; display: flex; align-items: center; justify-content: center;
        }
        .control-button:active { transform: translateY(2px); box-shadow: 0 2px 3px rgba(0,0,0,0.2); }
        .forward { background: linear-gradient(135deg, #48bb78, #38a169); grid-column: 2; }
        .backward { background: linear-gradient(135deg, #e53e3e, #c53030); grid-column: 2; }
        .left { background: linear-gradient(135deg, #ed8936, #dd6b20); grid-column: 1; grid-row: 2; }
        .right { background: linear-gradient(135deg, #4299e1, #3182ce); grid-column: 3; grid-row: 2; }
        .stop { background: linear-gradient(135deg, #a0aec0, #718096); grid-column: 2; grid-row: 2; }
        .rgb-controls { display: flex; flex-direction: column; gap: 15px; }
        .color-slider { width: 100%; margin: 5px 0; }
        .color-preview {
          width: 100%; height: 60px; border-radius: 10px;
          border: 2px solid #e2e8f0; margin: 10px 0; transition: background-color 0.3s ease;
        }
        .light-button {
          background: linear-gradient(135deg, #805ad5, #6b46c1); color: white;
          padding: 15px; border: none; border-radius: 12px; font-size: 16px;
          font-weight: 600; cursor: pointer; transition: all 0.15s ease;
          margin-top: 10px; box-shadow: 0 4px 6px rgba(0,0,0,0.1);
        }
        .light-button:active { transform: translateY(2px); box-shadow: 0 2px 3px rgba(0,0,0,0.2); }
        .status {
          background: #edf2f7; padding: 15px; border-radius: 10px;
          text-align: center; margin-top: 20px; font-size: 14px; color: #4a5568;
        }
        .ip-address { font-weight: 600; color: #2d3748; }
        /* Style for range inputs */
        input[type=range] {
          -webkit-appearance: none;
          width: 100%;
          height: 10px;
          border-radius: 5px;
          background: #d3d3d3;
          outline: none;
          opacity: 0.7;
          -webkit-transition: .2s;
          transition: opacity .2s;
        }
        input[type=range]::-webkit-slider-thumb {
          -webkit-appearance: none;
          appearance: none;
          width: 20px;
          height: 20px;
          border-radius: 50%;
          background: #4a5568;
          cursor: pointer;
        }
      </style>
      <script>
        let currentColor = { r: 0, g: 0, b: 0 };
        let lightsOn = false;
        
        function sendCommand(cmd) {
          fetch("/" + cmd).catch(err => console.log('Command error:', err));
        }
        
        function updateColorPreview() {
          const preview = document.getElementById('colorPreview');
          preview.style.backgroundColor = 'rgb(' + currentColor.r + ',' + currentColor.g + ',' + currentColor.b + ')';
        }
        
        function updateColor(value, color) {
          currentColor[color] = parseInt(value);
          document.getElementById(color + 'Value').textContent = value;
          updateColorPreview();
          
          // Send RGB command on input change
          fetch(`/rgb?r=${currentColor.r}&g=${currentColor.g}&b=${currentColor.b}`)
            .then(response => response.text())
            .then(data => {
                if (currentColor.r > 0 || currentColor.g > 0 || currentColor.b > 0) {
                    lightsOn = true;
                    updateLightButton(true);
                }
            })
            .catch(err => console.log('RGB error:', err));
        }
        
        function updateLightButton(state) {
            const button = document.getElementById('lightToggle');
            lightsOn = state;
            button.textContent = lightsOn ? 'Turn Lights Off' : 'Turn Lights On';
            button.style.background = lightsOn ? 
                'linear-gradient(135deg, #e53e3e, #c53030)' : 
                'linear-gradient(135deg, #805ad5, #6b46c1)';
        }
        
        function toggleLights() {
          const newState = !lightsOn;
          const url = '/lights?state=' + (newState ? 'on' : 'off');
          
          fetch(url)
            .then(response => response.text())
            .then(data => {
                updateLightButton(newState);
                if (newState) {
                    // When turning ON via toggle, set to white if RGB values are currently 0
                    if (currentColor.r === 0 && currentColor.g === 0 && currentColor.b === 0) {
                        setSlidersAndValues(255, 255, 255);
                    }
                } else {
                    // When turning OFF, reset sliders and color to 0,0,0
                    setSlidersAndValues(0, 0, 0);
                }
                updateColorPreview();
            })
            .catch(err => console.log('Lights error:', err));
        }
        
        function setSlidersAndValues(r, g, b) {
            currentColor.r = r;
            currentColor.g = g;
            currentColor.b = b;
            
            document.getElementById('rSlider').value = r;
            document.getElementById('gSlider').value = g;
            document.getElementById('bSlider').value = b;
            
            document.getElementById('rValue').textContent = r;
            document.getElementById('gValue').textContent = g;
            document.getElementById('bValue').textContent = b;
        }
        
        function setupButton(id, command) {
          const btn = document.getElementById(id);
          // Use 'mousedown'/'touchstart' to start, and 'mouseup'/'touchend'/'mouseleave' to stop
          btn.addEventListener('mousedown', () => sendCommand(command));
          btn.addEventListener('mouseup', () => sendCommand('S'));
          btn.addEventListener('mouseleave', () => sendCommand('S'));
          btn.addEventListener('touchstart', (e) => {
            e.preventDefault();
            sendCommand(command);
          });
          btn.addEventListener('touchend', (e) => {
            e.preventDefault();
            sendCommand('S');
          });
        }
        
        function loadStatus() {
            fetch('/status')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('ipAddress').textContent = data.ip;
                    document.getElementById('clients').textContent = data.clients;
                    
                    // Set initial RGB state from server
                    const [r, g, b] = data.rgb;
                    setSlidersAndValues(r, g, b);
                    
                    updateLightButton(data.lights);
                    updateColorPreview();
                })
                .catch(err => console.log('Status load error:', err));
        }
        
        window.onload = () => {
          setupButton("forward", "F");
          setupButton("backward", "B");
          setupButton("left", "L");
          setupButton("right", "R");
          setupButton("stop", "S");
          
          loadStatus();
        };
      </script>
    </head>
    <body>
      <div class="container">
        <h1>ESP32 Smart Car</h1>
        <div class="subtitle">Wireless Control Interface - IP: <span class="ip-address">)rawliteral"
                + ipAddress + R"rawliteral(</span></div>
        
        <div class="control-section">
          <div class="section-title">Movement Controls</div>
          <div class="control-grid">
            <button class="control-button forward" id="forward">Forward</button>
            <button class="control-button backward" id="backward">Backward</button>
            <button class="control-button left" id="left">Left</button>
            <button class="control-button stop" id="stop">Stop</button>
            <button class="control-button right" id="right">Right</button>
          </div>
          <div style="text-align: center; font-size: 12px; color: #718096;">
          </div>
        </div>
        
        <div class="control-section">
          <div class="section-title">RGB Lighting (Pin )rawliteral"
                + String(LED_PIN) + R"rawliteral()</div>
          <div class="rgb-controls">
            <div>
              <label>Red: <span id="rValue">0</span></label>
              <input type="range" min="0" max="255" value="0" class="color-slider" id="rSlider"
                     oninput="updateColor(this.value, 'r')">
            </div>
            <div>
              <label>Green: <span id="gValue">0</span></label>
              <input type="range" min="0" max="255" value="0" class="color-slider" id="gSlider"
                     oninput="updateColor(this.value, 'g')">
            </div>
            <div>
              <label>Blue: <span id="bValue">0</span></label>
              <input type="range" min="0" max="255" value="0" class="color-slider" id="bSlider"
                     oninput="updateColor(this.value, 'b')">
            </div>
            
            <div id="colorPreview" class="color-preview"></div>
            
            <button class="light-button" id="lightToggle" onclick="toggleLights()">
              Turn Lights On
            </button>
          </div>
        </div>
        
        <div class="status">
          <div>AP IP: <span class="ip-address" id="ipAddress">)rawliteral"
                + ipAddress + R"rawliteral(</span></div>
          <div>Connected clients: <span class="ip-address" id="clients">0</span></div>
        </div>
      </div>
    </body>
    </html>
  )rawliteral";

  server.send(200, "text/html", html);
}