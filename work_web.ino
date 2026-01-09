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
<html>
<body>
<center>
<h1>WiFi LED on off demo: 1</h1><br>
Ciclk to turn <a href="ledOn">LED ON</a><br>
Ciclk to turn <a href="ledOff">LED OFF</a><br>



<label for="speed">Enter speed (e.g., 0-255):</label><br>
<input type="number" id="speed" name="speed" min="0" max="255" value="100"><br><br>

<button onclick="runMotor()">Turn Motor ON</button>

<script>
function runMotor() {
    // Get the value from the input box
    let speed = document.getElementById('speed').value;
    
    // Optional: basic validation
    if (speed === '' || speed < 0 || speed > 255) {
        alert('Please enter a valid speed between 0 and 255');
        return;
    }
    
    // Redirect to /run?speed=XXX
    window.location.href = '/run?speed=' + speed;
}
</script>


<hr>
<a href="https://circuits4you.com">circuits4you.com</a>
</center>

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

// make move for func


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
 server.send(200, "text/html", "LED is ON"); //Send ADC value only to client ajax request

}

void handleLEDoff() { 
 Serial.println("LED off page");
 digitalWrite(LED,HIGH); //LED off
 server.send(200, "text/html", "LED is OFF"); //Send ADC value only to client ajax request
}



void moveMotorA() {

  if (server.hasArg("speed")) {
    String speed_string = server.arg("speed");
    int speed = speed_string.toInt();
    analogWrite(ENA, speed); // set speed to 200 out of possible range 0~255
  } else {
    digitalWrite(ENA, HIGH); // set speed to 200 out of possible range 0~255
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

 server.send(200, "text/html", "run once is done"); //Send ADC value only to client ajax request

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
    //////
  }


  

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
 
  server.on("/", handleRoot);      //Which routine to handle at root location. This is display page
  server.on("/ledOn", handleLEDon); //as Per  <a href="ledOn">, Subroutine to be called
  server.on("/ledOff", handleLEDoff);
  server.on("/run",moveMotorA);

  server.begin();                  //Start server
  // Serial.println("HTTP server started");
}



//==============================================================
//                     LOOP
//==============================================================
void loop(void){
  server.handleClient();          //Handle client requests

}
