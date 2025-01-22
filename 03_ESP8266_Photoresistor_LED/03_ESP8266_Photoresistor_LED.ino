/*
  AA Digital Ecologies Photoresistor input and LED and Serial output

  Circuit:
  - photoresistor connected to analog pin A0 and 4k ohm resistor
  - LED connected from digital pin D7 to ground through 220 ohm resistor

*/

const int analogInPin = A0;  // Photoresistor pin
const int analogOutPin = D7;  // LED pin

int sensorValue = 0;  // value read from the photoresistor
int outputValue = 0;  // value output using pulse width modulation (PWM) for the LED

void setup() {
  // initialize serial communications at 115200 bps
  Serial.begin(115200);
}

void loop() {
  // read the analog input value
  sensorValue = analogRead(analogInPin);
  // map it to the range of the analog output
  outputValue = map(sensorValue, 0, 1023, 0, 255);
  // actuate the analog output value to the LED
  analogWrite(analogOutPin, outputValue);

  // print the results to the Serial Monitor
  Serial.print("sensor = ");
  Serial.print(sensorValue);
  Serial.print("\t output = ");
  Serial.println(outputValue);

  delay(2); // small delay between reads for stability
}
