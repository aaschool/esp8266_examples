/*
 * ESP8266 Sensor to Cloud
 * 2025 francesco.anselmo@aaschool.ac.uk
 * ESP8266 + Analogue Sensor + Google App Script + Google Sheets
 *
 * Dependencies
 * HTTPSRedirect: https://github.com/electronicsguy/HTTPSRedirect
 *
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "HTTPSRedirect.h"

// Enter wifi credentials:
const char* ssid     = "CHANGEME-SSID";
const char* password = "CHANGEME-PASSWORD";

// Enter Google Script Deployment ID:
const char *GScriptId = "CHANGEME-GSCRIPTID";

// Enter command (insert_row or append_row) and your Google Sheets sheet name:
String payload_base =  "{\"command\": \"append_row\", \"sheet_name\": \"SensorData\", \"values\": ";
String payload = "";

// Google Sheets setup (do not edit)
const char* host = "script.google.com";
const int httpsPort = 443;
const char* fingerprint = "";
String url = String("/macros/s/") + GScriptId + "/exec";
HTTPSRedirect* client = nullptr;


void setup() {

  Serial.begin(9600);        
  delay(10);
  Serial.println('\n');
  
  // Connect to WiFi
  WiFi.begin(ssid, password);             
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());

  // Use HTTPSRedirect class to create a new TLS connection
  client = new HTTPSRedirect(httpsPort);
  client->setInsecure();
  client->setPrintResponseBody(true);
  client->setContentTypeHeader("application/json");
  
  Serial.print("Connecting to ");
  Serial.println(host);

  // Try to connect for a maximum of 5 times
  bool flag = false;
  for (int i=0; i<5; i++){ 
    int retval = client->connect(host, httpsPort);
    if (retval == 1){
       flag = true;
       Serial.println("Connected");
       break;
    }
    else
      Serial.println("Connection failed. Retrying...");
  }
  if (!flag){
    Serial.print("Could not connect to server: ");
    Serial.println(host);
    return;
  }
  delete client;    // delete HTTPSRedirect object
  client = nullptr; // delete HTTPSRedirect object
}


void loop() {

  // Read analoge value from A0 pin
  int sensorValue = analogRead(A0);

  static bool flag = false;
  if (!flag){
    client = new HTTPSRedirect(httpsPort);
    client->setInsecure();
    flag = true;
    client->setPrintResponseBody(true);
    client->setContentTypeHeader("application/json");
  }
  if (client != nullptr){
    if (!client->connected()){
      client->connect(host, httpsPort);
    }
  }
  else{
    Serial.println("Error creating client object!");
  }
  
  // Create json object string including the sensor value to send to Google Sheets
  payload = payload_base + "\"" + sensorValue + "\"}";
  
  // Publish data to Google Sheets
  Serial.println("Publishing data...");
  Serial.println(payload);
  if(client->POST(url, host, payload)){ 
    // Publish has been successful
  }
  else{
    // Publish has not been successful
    Serial.println("Error publishing data");
  }

  // wait for 10 seconds
  delay(10000);
}