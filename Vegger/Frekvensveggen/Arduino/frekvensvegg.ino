// Program for frekvensveggen
// Av Isak Bjørn
// Mars 2018

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif


#define LEDSTRIP 6

//-------------------------------------------------------------
#define freq1 440
#define freq2 200
#define freq3 310
#define freq4 650
#define floorFreq 120
#define roofFreq 720
#define freqStep 20
#define precision 10
//-------------------------------------------------------------

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(32, LEDSTRIP, NEO_RGB + NEO_KHZ800);



boolean DEBUG = false;
boolean ACTIVE = false;

//-------------------------------------------------------------
// The following are for setting up the button with short and long keypress
int button = 3;
long buttonTimer = 0;
long longPressTime = 1000;
boolean buttonActive = false;
boolean longPressActive = false;
//-------------------------------------------------------------
int serial_data = 0;
String inString = "";
int freq = 50;

//-------------------------------------------------------------
// This sets up the relee outputs and boolean states to se if they have been found
int bulb1 = 8;
int bulb2 = 9;
int bulb3 = 11;
int bulb4 = 10;
boolean freq1_found = false;
boolean freq2_found = false;
boolean freq3_found = false;

//-------------------------------------------------------------
// For holding rnd frequensies. NOT IN USE
int rndFreq1 = 50;
int rndFreq2 = 50;
int rndFreq3 = 50;
int rndFreq4 = 50;

void setup() {
// NOT IN USE, FOR GENERATING RANDOM FREQUENCIES
  randomSeed(analogRead(0));
//-------------------------------------------------------------
// Buttons and inputs
  pinMode(bulb1, OUTPUT);
  digitalWrite(bulb1, HIGH);
  pinMode(bulb2, OUTPUT);
  digitalWrite(bulb2, HIGH);
  pinMode(bulb3, OUTPUT);
  digitalWrite(bulb3, HIGH);
  pinMode(bulb4, OUTPUT);
  digitalWrite(bulb4, HIGH);
  //The big button on the middle of the wall
  pinMode(button, INPUT);
//-------------------------------------------------------------
// Serial setup
  //initialize neopixels
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  Serial.begin(9600);
  // Venter på at serial kobler seg til
  while (!Serial) {
    // wait for serial port to connect. Needed for native USB port only
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  buttonRead();
  if (ACTIVE == true) {
    strip_clear();
    //read serial input from piand
    while (Serial.available() > 0) {
      int inChar = Serial.read();
      if (isDigit(inChar)) {
        // convert the incoming byte to a char and add it to the string:
        inString += (char)inChar;
      }
      // if you get a newline, print the string, then the string's value:
      if (inChar == '\n') {
        freq = inString.toInt();
        inString = "";
        break; // when a complete number is read
      }
    }
    equalizer(freq); // Display a frequency indicator on ledstrip
    if (freq1_found == false) {//look for freq1
      if (freq1 - precision < freq && freq < freq1 + precision) {
        digitalWrite(bulb1, LOW);
        freq1_found = true;
      }
    } else if (freq2_found == false) {//look for freq2
      if (freq2 - precision < freq && freq < freq2 + precision) {
        digitalWrite(bulb2, LOW);
        freq2_found = true;
      }
    } else if (freq3_found == false) { //look for freq3
      if (freq3 - precision < freq && freq < freq3 + precision) {
        digitalWrite(bulb3, LOW);
        freq3_found = true;
      }
    } else { //look for freq4
      if (freq4 - precision < freq && freq < freq4 + precision) {
        // GAME WON! ALL FREQUENCIES FOUND
        digitalWrite(bulb4, LOW);
        gameWon(); //woop woop
      }
    }
  } else {
    //GAME IS INACTIVE, DO EYE WATERING BLINKY STUFF
    ledOrange();
    int inChar = Serial.read(); // to keep buffer from filling up
  }
} // end of loop

void gameWon() {
  theaterChase(255, 100);
  reset();
  strip_clear();
  ACTIVE = false;
  freq = 50;
}

void equalizer(int freq) {
  strip_clear();
  for (int i = 0; i < 32; i++) {
    if (i == 31 || freq < floorFreq + i * freqStep ) {
      strip.setPixelColor(i, strip.Color(0, 255, 150));
      // set forrige pixel til 50% av lysstyrken
      if ( i != 0) {
        strip.setPixelColor(i - 1, strip.Color(0, 32, 24));
      }
      if ( i != 31) {
        strip.setPixelColor(i + 1, strip.Color(0, 32, 24));
      }
      break;
    }
  }
  strip.show();
}

void strip_clear() {
  for (int i = 0; i < 32; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  strip.show();
}

void led_test() {
  for (int i = 0; i < 32; i++) {
    strip.setPixelColor(i, strip.Color(255, 255, 255));
  }
  strip.show();
  delay(1000);
  strip_clear();
}

void ledOrange() {
  for (int i = 0; i < 32; i++) {
    strip.setPixelColor(i, strip.Color(255, 20, 0));
  }
  strip.show();
}

void reset() {
  freq = 50;
  freq1_found = false;
  freq2_found = false;
  freq3_found = false;
  digitalWrite(bulb1, HIGH);
  digitalWrite(bulb2, HIGH);
  digitalWrite(bulb3, HIGH);
  digitalWrite(bulb4, HIGH);
}

void buttonRead() {
  if (digitalRead(button) == HIGH) {
    if (buttonActive == false) {
      buttonActive = true;
      buttonTimer = millis();
    }
    if ((millis() - buttonTimer > longPressTime) && (longPressActive == false)) {
      //LONG PRESS ACTIVATED
      longPressActive = true;
      reset();
      ACTIVE = false;
    }
  } else {
    if (buttonActive == true) {
      if (longPressActive == true) {
        longPressActive = false;
      } else {
        //short press
        ACTIVE = true;
       generateFreq();
      }
      buttonActive = false;
    }
  }
} // end of button read

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j = 0; j < 32; j++) { //do 10 cycles of chasing
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, c);  //turn every third pixel on
      }
      strip.show();
      delay(wait);
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}
void generateFreq(){
  rndFreq1 = random(120, 680);
  rndFreq2 = random(120, 680);
  rndFreq3 = random(120, 680);
  rndFreq4 = random(120, 680);
  }

