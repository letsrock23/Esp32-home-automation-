//Arduino code.
// Created and design by Farhan Saleh. Local Home Automation device.

#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// Pin definitions
#define LED1_PIN 19//change it with your gpio pin numbers
#define LED2_PIN 18
#define LED3_PIN 23
#define LED4_PIN 5

// Display definitions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C  // Use the correct I2C address here

Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// HTML and CSS for the webpage
String htmlPage = R"=====(
<!DOCTYPE html>
<html>
<head>
  <style>
    body {
      font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
      background-color: #282c34;
      color: #ffffff;
      text-align: center;
      margin: 0;
      padding: 0;
    }
    .container {
      max-width: 800px;
      margin: 50px auto;
      padding: 20px;
    }
    .header {
      font-size: 32px;
      font-weight: bold;
      margin-bottom: 40px;
      color: #61dafb;
    }
    .grid-container {
      display: grid;
      grid-template-columns: repeat(2, 1fr);
      gap: 20px;
    }
    .device {
      background: linear-gradient(135deg, #EE4B2B, #feb47b);
      border-radius: 12px;
      box-shadow: 0 6px 12px rgba(0, 0, 0, 0.3);
      padding: 30px;
      transition: transform 0.3s ease-in-out, box-shadow 0.3s ease-in-out;
    }
    .device:hover {
      transform: translateY(-10px);
      box-shadow: 0 12px 24px rgba(0, 0, 0, 0.4);
    }
    .device-icon {
      font-size: 48px;
      margin-bottom: 10px;
      color: #ffffff;
    }
    .device-name {
      font-size: 22px;
      font-weight: bold;
      margin-bottom: 10px;
    }
    .device-control {
      font-size: 18px;
      color: #ffffff;
      background-color: #61dafb;
      padding: 10px 20px;
      border-radius: 8px;
      cursor: pointer;
      transition: background-color 0.3s ease-in-out;
      text-decoration: none;
    }
    .device-control:hover {
      background-color: #21a1f1;
    }
  </style>
</head>
<body>
  <div class="container">
    <div class="header">ESP32 Device Control</div>
    <div class="grid-container">
      <div class="device" onclick="toggleLed(1)">
        <div class="device-icon">&#x1f4a1;</div>
        <div class="device-name">Living Room Light</div>
        <a class="device-control">Toggle Light</a>
      </div>
      <div class="device" onclick="toggleLed(2)">
        <div class="device-icon">&#x1f4a1;</div>
        <div class="device-name">Bedroom Light</div>
        <a class="device-control">Toggle Light</a>
      </div>
      <div class="device" onclick="toggleLed(3)">
        <div class="device-icon">&#x1f4a1;</div>
        <div class="device-name">Kitchen Light</div>
        <a class="device-control">Toggle Light</a>
      </div>
      <div class="device" onclick="toggleLed(4)">
        <div class="device-icon">&#x1f4a1;</div>
        <div class="device-name">Garage Light</div>
        <a class="device-control">Toggle Light</a>
      </div>
    </div>
  </div>

  <script>
    function toggleLed(ledNumber) {
      var xhr = new XMLHttpRequest();
      xhr.open("GET", "/toggle?led=" + ledNumber, true);
      xhr.send();
    }
  </script>
</body>
</html>
)=====";


// WiFi setup
const char* ssid = "ESP32-Access-Point";
const char* password = "123456789";

WiFiServer server(80);

void setup() {
  // Serial and GPIO setup
  Serial.begin(115200);
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
  pinMode(LED4_PIN, OUTPUT);

  // Display initialization
  if (!display.begin(SCREEN_ADDRESS, OLED_RESET)) {
    Serial.println(F("SH110X allocation failed"));
    while (1);
  }
  display.display();
  delay(1000);
  display.clearDisplay();

  // Initialize WiFi as AP
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // Start the server
  server.begin();

  // Display the initial message
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println("ESP32 Device Control");
  display.display();

  // Print the number of connected stations (clients)
  Serial.print("Number of stations connected: ");
  Serial.println(WiFi.softAPgetStationNum());
}

void loop() {
  // Check for client connection
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New Client.");
    String request = client.readStringUntil('\r');
    Serial.println(request);
    client.flush();

    // Handle LED toggle
    if (request.indexOf("/toggle?led=1") != -1) {
      toggleLed(LED1_PIN, "Living Room Light ON");
    }
    if (request.indexOf("/toggle?led=2") != -1) {
      toggleLed(LED2_PIN, "Bedroom Light ON");
    }
    if (request.indexOf("/toggle?led=3") != -1) {
      toggleLed(LED3_PIN, "Kitchen Light ON");
    }
    if (request.indexOf("/toggle?led=4") != -1) {
      toggleLed(LED4_PIN, "Garage Light ON");
    }

    // Send the HTML response
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println();
    client.println(htmlPage);
    client.println();
    client.stop();
    Serial.println("Client Disconnected.");
  }
}

void toggleLed(int ledPin, const char* message) {
  int state = digitalRead(ledPin);
  digitalWrite(ledPin, !state);

  // Display the LED state on the OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(message);
  display.display();

  Serial.print("LED ");
  Serial.print(ledPin);
  Serial.println(state ? " OFF" : " ON");
}
