/*
  AA Digital Ecologies ESP8266 Button LED

  Reads a digital input on pin D1, prints the result to the Serial Monitor
  and turns on and off an LED connected on D7

*/

int pushButton = D1; // digital pin D1 for the push button
int ledPin = D7; // digital pin D7 for the LED

// the setup function runs only once at the beginning
void setup() {
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  
  pinMode(pushButton, INPUT); // make the pushbutton's pin an input
  pinMode(ledPin, OUTPUT); // make the LED's pin an output
}

// the loop function runs forever
void loop() {
  // read the digital input pin
  int buttonState = digitalRead(pushButton);

  // print out the state of the button (pressed = 0, released = 1)
  Serial.println(buttonState);

  if (buttonState == 1) {
    digitalWrite(ledPin, HIGH); // light off when button released
  } else {
    digitalWrite(ledPin, LOW); // light on when button pressed
  }
  
  delay(1);  // small delay between reads for stability

}
