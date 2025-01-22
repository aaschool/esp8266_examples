/*
  AA Digital Ecologies ESP8266 LED Blink 
  Blink the blue LED on the ESP8266 NodeMCU module
  This example code is in the public domain

  The blue LED on the ESP8266 NodeMCU module is connected to GPIO1
  This is also the TXD (serial transmit) pin, so we cannot use 
  the Serial.print() function at the same time

  In this sketch we use the LED_BUILTIN constant to find the pin with the internal LED
*/

// the setup function runs only once at the beginning
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);  // Initialize the LED_BUILTIN pin as an output
}

// the loop function runs forever
void loop() {
  digitalWrite(LED_BUILTIN, LOW);  // Turn the LED on 
  // LOW is the voltage level for the LED to be ON
  // because it is "active low" on the ESP8266 NodeMCU
  delay(1000);                      // Wait for a second
  digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
  delay(2000);                      // Wait for two seconds
}
