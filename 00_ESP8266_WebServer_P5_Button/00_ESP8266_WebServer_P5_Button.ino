#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#ifndef STASSID
#define STASSID "change me with the wifi ssid"
#define STAPSK "change me with the wifi password"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

const int ledPin = LED_BUILTIN; // Built-in LED pin on ESP8266

ESP8266WebServer server(80);

// HTML and JavaScript code for the web page
const char* webpage = R"=====(
<!DOCTYPE html>
<html>
<head>
  <title>ESP8266 LED Control</title>
  <script src="https://cdn.jsdelivr.net/npm/p5@1.4.0/lib/p5.js"></script>
  <style>
    body { margin: 0; overflow: hidden; }
    canvas { display: block; }
  </style>
</head>
<body>
  <script>
    function setup() {
      createCanvas(windowWidth, windowHeight);
      background(220);
      let button = createButton('Toggle LED');
      button.position(width / 2 - button.width / 2, height / 2 - button.height / 2);
      button.mousePressed(toggleLED);
    }

    function toggleLED() {
      httpGet('/toggle', function(response) {
        console.log(response);
      });
    }
  </script>
</body>
</html>
)=====";

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH); // Initially turn off the LED

  Serial.begin(115200);
  delay(10);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) { Serial.println("MDNS responder started"); }

  server.on("/", []() {
    server.send(200, "text/html", webpage);
  });

  server.on("/toggle", []() {
    digitalWrite(ledPin, !digitalRead(ledPin)); // Toggle the LED state
    server.send(200, "text/plain", "LED toggled");
  });

  server.begin();
  Serial.println("HTTP Server started");
}

void loop() {
  server.handleClient();
  MDNS.update();
}