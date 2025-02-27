
/*
 * ESP8266 Wifi WS2812 LEDmatrix
 * 2025 francesco.anselmo@aaschool.ac.uk
 * ESP8266 + WS2812 lighting control for dynamic lighting
 *
 * Dependencies
 * ESP8266 board manager library v2.7.4
 * FastLED v3.5.0
 */

// use the ESP and FastLED libraries
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FastLED.h>

// *** Variables and definitions to be changed for each devices ***

#ifndef STASSID
#define STASSID "digital-ecologies"
#define STAPSK "calmtechnology"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

// change DATA_PIN to the PIN used on the ESP8266
// note that sometimes the PIN number is different than what is indicated in the circuit board
#define NODE_NAME "ledmatrix1"      // change this with the name of the ESP8266 WiFi device
#define NUM_LEDS 64             // change this number of LEDs in the WS2812 LED strip
#define DATA_PIN D4             // change this to the pin connected on the ESP8266 to the WS2812 data pin
#define LED_TYPE WS2812         // change this if the LED type is different than WS2812
#define COLOR_ORDER GRB         // change this to match the LED order if the colours appear in the wrong order
#define INITIAL_BRIGHTNESS 150  // change this to modify the initial brightness
#define INITIAL_TRANSITION 1    // change this to modify the initial transition type
#define FRAMES_PER_SECOND 20    // change this to speed up (lower value) or slow down (higher value) the lighting transitions
#define DELAY 20                // change this to modify the FastLED update delay

// *** Nothing to change in the code below ***

FASTLED_USING_NAMESPACE

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

enum colour_enum { BLACK, WHITE, RED, GREEN, BLUE, YELLOW, ORANGE, PINK };
enum transition_enum { CONSTANT, SINELON, BPM, GRADIENT, RANDOM };

CRGB leds[NUM_LEDS];
CRGBPalette16 palette = PartyColors_p;

ESP8266WebServer server(80);

DEFINE_GRADIENT_PALETTE( blue_gp ) {
  0,     0,  0,  0,     //black
255,     0,  0,  255 }; //blue

DEFINE_GRADIENT_PALETTE( green_gp ) {
  0,     0,  0,  0,     //black
255,     0,  255,  0 }; //green

DEFINE_GRADIENT_PALETTE( yellow_gp ) {
  0,     0,  0,  0,       //black
255,     255,  255,  0 }; //yellow

DEFINE_GRADIENT_PALETTE( orange_gp ) {
  0,     0,  0,  0,       //black
  10,     10,  5,  0,   
  20,     20,  10,  0,   
  200,     200,  100,  0,
255,     255, 128,  0 };  //orange

DEFINE_GRADIENT_PALETTE( red_gp ) {
  0,     0,  0,  0,     //black
255,     255,  0,  0 }; //red

DEFINE_GRADIENT_PALETTE( pink_gp ) {
  0,     0,  0,  0,       //black
255,     255,  0,  128 }; //pink

DEFINE_GRADIENT_PALETTE( white_gp ) {
  0,     0,  0,  0,         //black
255,     255,  255,  255 }; //white

DEFINE_GRADIENT_PALETTE( black_gp ) {
  0,     0,  0,  0,   //black
255,     0,  0,  0 }; //black

const int signal_led = LED_BUILTIN;  // signalling LED pin, only used for showing that the microcontroller is active
uint8_t idx = 0;                     // rotating index used for the LED lighting transition

int transition = INITIAL_TRANSITION;
int brightness = INITIAL_BRIGHTNESS;
int speed = FRAMES_PER_SECOND;


// HTML and JavaScript code for the web page
const char webpage[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
  <title>ESP8266 LED Control</title>
  <style>
    body { margin: 0; overflow: hidden; }
    canvas { display: block; }
  </style>
</head>
<body>
  <ul>
  <li><a href="/red">Red</a></li>
  <li><a href="/green">Green</a></li>
  <li><a href="/blue">Blue</a></li>
  </ul>
</body>
</html>
)=====";


void testLEDs(){
  int i;
  Serial.println("Testing LEDs");
  for(int i = 0; i < NUM_LEDS; i++) {
    // set our current dot to red
    Serial.println("Red");
    leds[i] = CRGB::Red;
    FastLED.show();
    delay(DELAY);
  }
  for(int i = 0; i < NUM_LEDS; i++) {
    // set our current dot to green
    Serial.println("Green");
    leds[i] = CRGB::Green;
    FastLED.show();
    delay(DELAY);
  }
  for(int i = 0; i < NUM_LEDS; i++) {
    // set our current dot to blue
    Serial.println("Blue");
    leds[i] = CRGB::Blue;
    FastLED.show();
    delay(DELAY);
  }
}

void setColor(int color) {
  int i;
  Serial.println(color);
  CRGB c;
  switch (color) { 
    case WHITE:
      c = CRGB::White;
      palette = white_gp;
      break;
    case RED:
      c = CRGB::Red;
      palette = red_gp;
      break;
    case GREEN:
      c = CRGB::Green;
      palette = green_gp;
      break;
    case BLUE:
      c = CRGB::Blue;
      palette = blue_gp;
      break;
    case YELLOW:
      c = CRGB::Yellow;
      palette = yellow_gp;
      break;
    case ORANGE:
      c = CRGB::Orange;
      palette = orange_gp;
      break;
    case PINK:
      c = CRGB::Pink;
      palette = pink_gp;
      break;
    case BLACK:
      c = CRGB::Black;
      palette = black_gp;
      break;
  }
    delay(DELAY);
}

void setBrightness(int newBrightness) 
{
  brightness = newBrightness;
  FastLED.setBrightness(brightness);
  Serial.print("Brightness: ");
  Serial.println(brightness);
}

void off()
{
  brightness=0;
  FastLED.setBrightness(brightness);
  Serial.print("Brightness: ");
  Serial.println(brightness);
}

void brighten()
{
  brightness+=20;
  if (brightness>250) brightness=250;
  FastLED.setBrightness(brightness);
  Serial.print("Brightness: ");
  Serial.println(brightness);
}

void dim()
{
  brightness-=20;
  if (brightness<0) brightness=0;
  FastLED.setBrightness(brightness);
  Serial.print("Brightness: ");
  Serial.println(brightness);
}

void setSpeed(int newSpeed) 
{
  speed = newSpeed;
  FastLED.delay(1000/speed); 
  Serial.print("Speed (frames/second): ");
  Serial.println(speed);
}

void faster()
{
  speed+=10;
  if (speed>3000) speed=3000;
  FastLED.delay(1000/speed); 
  Serial.print("Speed (frames/second): ");
  Serial.println(speed);
}

void slower()
{
  speed-=10;
  if (speed<=10) speed=10;
  FastLED.delay(1000/speed); 
  Serial.print("Speed (frames/second): ");
  Serial.println(speed);
}

void t_sinelon()
{
  // a coloured dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 2);
  int pos = beatsin16(13,0,NUM_LEDS);
  leds[pos] = ColorFromPalette(palette, idx, 255);
}


void t_bpm()
{
  // colored stripes pulsing at a defined beats-per-minute (BPM)
  uint8_t BeatsPerMinute = int(speed/2);
  uint8_t beat = beatsin8( BeatsPerMinute, 32, 255);
  for( int i = 0; i < NUM_LEDS; i++) { 
    leds[i] = ColorFromPalette(palette, idx+(i*2), beat-idx+(i*10));
  }
}

void t_random() 
{
  // random colored dots that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] = ColorFromPalette(palette, idx + random8(64), 255);
}

void t_gradient()
{
    // all the LEDs smoothly fade up and down
    for( int i = 0; i < NUM_LEDS; i++) {
      leds[i] = ColorFromPalette( palette, idx, brightness, LINEARBLEND);
    }
}

void t_constant()
{
    // constant coloured light for all the LEDs in the strip
    for( int i = 0; i < NUM_LEDS; i++) {
      leds[i] = ColorFromPalette( palette, idx, brightness);
    }
}

void setup() {
  // code that runs at startup only

  // turn on the onboard
  pinMode(signal_led, OUTPUT);
  digitalWrite(signal_led, HIGH);

  delay(3000);  // 3 second delay for device recovery

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(brightness);

  testLEDs(); // run the initial RGB light test to help running a visual check
  
  setColor(RED);

  // serial connection to the Arduino running the encoder
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

  if (MDNS.begin("esp8266")) { 
    Serial.println("MDNS responder started"); 
  }

  // setup the API enpoints and match them with changes of lighting conditions as required

  server.on("/", []() {
    server.send(200, "text/html", webpage);
  });


  // set the colour to black, effectively switching off all the LEDs
  server.on("/off", []() {
    setColor(0);
    server.send(200, "text/html", webpage);
  });

  // increase brightness
  server.on("/brighten", []() {
    brighten();
    server.send(200, "text/html", webpage);
  });

  // decrease brightness
  server.on("/dim", []() {
    dim();
    server.send(200, "text/html", webpage);

  });

  // increase speed
  server.on("/faster", []() {
    faster();
    server.send(200, "text/html", webpage);
  });

  // decrease speed
  server.on("/slower", []() {
    slower();
    server.send(200, "text/html", webpage);
  });

  // set specific speed values
  server.on("/s10", []() {
    setSpeed(10);
    server.send(200, "text/html", webpage);
  });
  server.on("/s20", []() {
    setSpeed(20);
    server.send(200, "text/html", webpage);
  });
  server.on("/s50", []() {
    setSpeed(50);
    server.send(200, "text/html", webpage);
  });
  server.on("/s100", []() {
    setSpeed(100);
    server.send(200, "text/html", webpage);
  });
  server.on("/s500", []() {
    setSpeed(150);
    server.send(200, "text/html", webpage);
  });
  server.on("/s1000", []() {
    setSpeed(150);
    server.send(200, "text/html", webpage);
  });
  server.on("/s1500", []() {
    setSpeed(150);
    server.send(200, "text/html", webpage);
  });
  server.on("/s2000", []() {
    setSpeed(150);
    server.send(200, "text/html", webpage);
  });

  // set LED colour to white
  server.on("/white", []() {
    setColor(WHITE);
    server.send(200, "text/html", webpage);
  });
  // set LED colour to red
  server.on("/red", []() {
    setColor(RED);
    server.send(200, "text/html", webpage);
  });
  // set LED colour to green
  server.on("/green", []() {
    setColor(GREEN);
    server.send(200, "text/html", webpage);
  });
  // set LED colour to blue
  server.on("/blue", []() {
    setColor(BLUE);
    server.send(200, "text/html", webpage);
  });
  // set LED colour to yellow
  server.on("/yellow", []() {
    setColor(YELLOW);
    server.send(200, "text/html", webpage);
  });
  // set LED colour to orange
  server.on("/orange", []() {
    setColor(ORANGE);
    server.send(200, "text/html", webpage);
  });
  // set LED colour to pink
  server.on("/pink", []() {
    setColor(PINK);
    server.send(200, "text/html", webpage);
  });
  // set LED colour to black, which is the same as switching the LEDs off
  server.on("/black", []() {
    setColor(BLACK);
    server.send(200, "text/html", webpage);
  });

  // change transition to sinelon
  server.on("/sinelon", []() {
    transition = SINELON;
    server.send(200, "text/html", webpage);
  });
  // change transition to BPM
  server.on("/bpm", []() {
    transition = BPM;
    server.send(200, "text/html", webpage);
  });
  // change transition to gradient
  server.on("/gradient", []() {
    transition = GRADIENT;
    server.send(200, "text/html", webpage);
  });
  // change transition to random
  server.on("/random", []() {
    transition = RANDOM;
    server.send(200, "text/html", webpage);
  });
  // change transition to constant
  server.on("/constant", []() {
    transition = CONSTANT;
    server.send(200, "text/html", webpage);
  });
  
  server.onNotFound([]() {                              // If the client requests any URI
      handleNotFound();
  });

  
  server.begin();
  
  Serial.println("HTTP server started");
  digitalWrite(signal_led, LOW);
}

void loop() {
  // codes that runs continuously in a loop

  server.handleClient();
  MDNS.update();
  
  // run transition
  if (transition == SINELON) t_sinelon();
  else if (transition == BPM) t_bpm();
  else if (transition == GRADIENT) t_gradient();
  else if (transition == RANDOM) t_random();
  else if (transition == CONSTANT) t_constant();

  // send the LEDs array out to the actual LED strip
  FastLED.show();
  // insert a delay to keep the framerate within the microcontroller capability
  FastLED.delay(1000 / speed);
  // update the LED index
  EVERY_N_MILLISECONDS(5000 / speed) {
    idx++;
  }  
}

void handleNotFound() {
    digitalWrite ( signal_led, HIGH );
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for ( uint8_t i = 0; i < server.args(); i++ ) {
        message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
    }
    server.send ( 404, "text/plain", message );
    digitalWrite ( signal_led, LOW );
}


