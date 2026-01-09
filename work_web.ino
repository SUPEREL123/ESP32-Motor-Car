/*
 * ESP8266 NodeMCU LED Control over WiFi Demo
 *
 * https://circuits4you.com
 */
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

//ESP Web Server Library to host a web page
#include <ESP8266WebServer.h>

//---------------------------------------------------------------
//Our HTML webpage contents in program memory
const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0"/>
    <title>ESP32 Motor Controller</title>
    <style>
        :root {
            --primary: #00d4ff;
            --primary-dark: #0095b3;
            --bg-dark: #0f172a;
            --card: #1e293b;
            --text: #e2e8f0;
            --danger: #f87171;
            --success: #22c55e;
        }

        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }

        body {
            min-height: 100vh;
            background: linear-gradient(135deg, #0f172a 0%, #1e293b 100%);
            color: var(--text);
            font-family: 'Segoe UI', system-ui, sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            padding: 20px;
        }

        .controller {
            background: var(--card);
            border-radius: 24px;
            padding: 2.5rem 2rem;
            width: 100%;
            max-width: 420px;
            box-shadow: 
                0 25px 50px -12px rgba(0,0,0,0.6),
                0 0 0 1px rgba(255,255,255,0.08) inset;
            backdrop-filter: blur(10px);
            border: 1px solid rgba(255,255,255,0.07);
            position: relative;
        }

        h1 {
            text-align: center;
            font-size: 2.1rem;
            margin-bottom: 2rem;
            color: var(--primary);
            text-shadow: 0 0 20px rgba(0,212,255,0.4);
            letter-spacing: -0.5px;
        }

        .status {
            text-align: center;
            margin-bottom: 2rem;
            font-size: 1.1rem;
            opacity: 0.8;
        }

        .led-section {
            display: flex;
            justify-content: center;
            gap: 2.5rem;
            margin: 2rem 0;
        }

        .led-btn {
            padding: 1rem 2.2rem;
            font-size: 1.1rem;
            border: none;
            border-radius: 12px;
            cursor: pointer;
            transition: all 0.25s ease;
            font-weight: 600;
            min-width: 140px;
            text-decoration: none;
            text-align: center;
        }

        .led-on {
            background: #22c55e;
            color: white;
            box-shadow: 0 0 25px rgba(34,197,94,0.5);
        }

        .led-off {
            background: #ef4444;
            color: white;
            box-shadow: 0 0 25px rgba(239,68,68,0.4);
        }

        .led-btn:hover {
            transform: translateY(-2px);
            box-shadow: 0 10px 25px rgba(0,0,0,0.4);
        }

        .motor-control {
            background: rgba(0,0,0,0.2);
            padding: 1.8rem;
            border-radius: 16px;
            margin: 2rem 0;
        }

        .speed-label {
            display: flex;
            justify-content: space-between;
            margin-bottom: 0.8rem;
            font-weight: 500;
        }

        .speed-value {
            color: var(--primary);
            font-weight: bold;
            min-width: 60px;
            text-align: right;
        }

        input[type="range"] {
            width: 100%;
            height: 10px;
            -webkit-appearance: none;
            background: rgba(255,255,255,0.1);
            border-radius: 5px;
            outline: none;
        }

        input[type="range"]::-webkit-slider-thumb {
            -webkit-appearance: none;
            width: 28px;
            height: 28px;
            background: var(--primary);
            border-radius: 50%;
            cursor: pointer;
            box-shadow: 0 0 15px rgba(0,212,255,0.6);
            border: 3px solid #0f172a;
        }

        .btn-run {
            width: 100%;
            padding: 1.3rem;
            font-size: 1.3rem;
            font-weight: bold;
            background: linear-gradient(90deg, #00d4ff, #0891b2);
            color: white;
            border: none;
            border-radius: 14px;
            cursor: pointer;
            transition: all 0.3s ease;
            margin-top: 1.5rem;
            box-shadow: 0 10px 30px rgba(0,212,255,0.35);
            position: relative;
            overflow: hidden;
        }

        .btn-run:hover:not(:disabled) {
            transform: translateY(-3px);
            box-shadow: 0 15px 40px rgba(0,212,255,0.5);
        }

        .btn-run:active:not(:disabled) {
            transform: translateY(1px);
        }

        .btn-run:disabled {
            opacity: 0.7;
            cursor: not-allowed;
            background: #475569;
            box-shadow: none;
        }

        /* Subtle Finish Message */
        #finish-msg {
            position: absolute;
            bottom: -30px;
            left: 50%;
            transform: translateX(-50%);
            color: var(--success);
            font-weight: 600;
            font-size: 0.9rem;
            opacity: 0;
            transition: all 0.4s ease;
            pointer-events: none;
        }

        #finish-msg.show {
            bottom: 10px;
            opacity: 1;
        }

        .footer {
            text-align: center;
            margin-top: 2rem;
            font-size: 0.9rem;
            opacity: 0.6;
        }

        .footer a {
            color: var(--primary);
            text-decoration: none;
        }
    </style>
</head>
<body>

<div class="controller">
    <h1>ESP32 Motor Controller</h1>
    
    <div class="status">Motor & LED Control</div>

    <!-- LED Control -->
    <div class="led-section">
        <a href="ledOn" class="led-btn led-on">LED ON</a>
        <a href="ledOff" class="led-btn led-off">LED OFF</a>
    </div>

    <!-- Motor Control -->
    <div class="motor-control">
        <div class="speed-label">
            <span>Speed</span>
            <span class="speed-value" id="speedValue">100</span>
        </div>

        <input type="range" 
               id="speed" 
               min="0" 
               max="255" 
               value="100"
               oninput="document.getElementById('speedValue').textContent = this.value">

        <button id="runBtn" class="btn-run" onclick="runMotor()">START MOTOR</button>
        <div id="finish-msg">✓ Finish</div>
    </div>

    <div class="footer">
        <a href="https://circuits4you.com" target="_blank">circuits4you.com</a>
    </div>
</div>

<script>
function runMotor() {
    const btn = document.getElementById('runBtn');
    const msg = document.getElementById('finish-msg');
    const speed = document.getElementById('speed').value;
    
    // Disable button and show running state
    btn.disabled = true;
    btn.innerText = "RUNNING...";
    msg.classList.remove('show');

    // Use fetch to call the /run endpoint without reloading the page
    fetch(`/run?speed=${speed}`)
        .then(response => {
            // Re-enable button
            btn.disabled = false;
            btn.innerText = "START MOTOR";
            
            if (response.ok) {
                // Show subtle finish message
                msg.classList.add('show');
                // Hide it after 3 seconds
                setTimeout(() => {
                    msg.classList.remove('show');
                }, 3000);
            }
        })
        .catch(err => {
            console.error('Error:', err);
            btn.disabled = false;
            btn.innerText = "START MOTOR";
        });
}
</script>

</body>
</html>
)=====";
//---------------------------------------------------------------
//On board LED Connected to GPIO2
#define LED 2  

//SSID and Password of your WiFi router
const char* ssid = "Sakuragi10";
const char* password = "daddymummy123";

/// set var
/// Motor A
int ENA = 4;
int IN1 = 0;
int IN2 = 2;

/// Motor B
int ENB = 5;   // PWM pin for speed control of Motor B
int IN3 = 7;   // Direction pin 1 for Motor B
int IN4 = 8;   // Direction pin 2 for Motor B

//Declare a global object variable from the ESP8266WebServer class.
ESP8266WebServer server(80); //Server on port 80

//===============================================================
// This routine is executed when you open its IP in browser
//===============================================================
void handleRoot() {
 Serial.println("You called root page");
 String s = MAIN_page; //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}

void handleLEDon() { 
 Serial.println("LED on page");
 digitalWrite(LED,LOW); //LED is connected in reverse
 server.send(200, "text/html", "LED is ON"); 
}

void handleLEDoff() { 
 Serial.println("LED off page");
 digitalWrite(LED,HIGH); //LED off
 server.send(200, "text/html", "LED is OFF"); 
}

void moveMotorA() {
  if (server.hasArg("speed")) {
    String speed_string = server.arg("speed");
    int speed = speed_string.toInt();
    analogWrite(ENA, speed); 
  } else {
    digitalWrite(ENA, HIGH); 
  }

  // turn on motor
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  
  delay(2000); // now change motor directions
  
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  
  delay(2000); // now turn off motors
  
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  
  // Send a simple OK response for the AJAX request
  server.send(200, "text/plain", "OK"); 
}

//==============================================================
//                  SETUP
//==============================================================
void setup(void){
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");

  //Onboard LED port Direction output
  pinMode(LED,OUTPUT); 
  //Power on LED state off
  digitalWrite(LED,HIGH);
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

    ///// motor project
    pinMode(ENA, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
 
  server.on("/", handleRoot);      
  server.on("/ledOn", handleLEDon); 
  server.on("/ledOff", handleLEDoff);
  server.on("/run", moveMotorA);

  server.begin();                  
}

//==============================================================
//                     LOOP
//==============================================================
void loop(void){
  server.handleClient();          
}
