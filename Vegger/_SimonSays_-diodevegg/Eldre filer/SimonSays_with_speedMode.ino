/* Versjon 1.1
 * Lastet opp 9.april 2018
 * av Isak Bjørn og Mikro
 * 
 * Program skrevet til SimonSays veggen til Mikros arrangement Escape Velocity.
 * Det kontrollerer en vegg bestående av 42 knapper med tilhørende led-dioder
 * Programmet forholder seg til det vi kan kalle bruker nr, dette er en "logisk" addresering.
 * Sett fra front:
 *    0,  1,  2,  3,  4,  5,
 *    6,  7,  8,  9, 10, 11,
 *   12, 13, 14, 15, 16, 17,
 *   18, 19, 20, 21, 22, 23,
 *   24, 25, 26, 27, 28, 29,
 *   30, 31, 32, 33, 34, 35,
 *   36, 37, 38, 39, 40, 41
 *   
 *   Da knappene og led-ene ikke er koblet opp på en logisk måte brukes to metoder button() og led() til å oversette
 *   fra brukernr til reelle nr. Så hvis man vil lese av knapp to fra venstre på øverste rad bruker man 
 *   digitalRead( button(2) ) 
 *   
 * Dette programmet kan fritt brukes og deles av alle som trur de kan gjøre noe kult med det.
 *                        
 */ 
 
#include <Adafruit_NeoPixel.h>
//#include <utils.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define clockPin 38
#define wallPin 36
#define wallPixels 124
#define clockPixels 29
#define numButtons 42
#define maxSeqLength 42
#define dispTime 800 // The time each step of the sequence will light up (lower number = harder)

boolean DEBUG = true;

// To patch the buttons to the correct input pins
int pinPatch[numButtons] = { 5,  4,  3,  2, 44, 46,
                             6,  7,  8,  9, 10, 11,
                             17, 16, 15, 14, 13, 12,
                             18, 19, 42, 40, 22, 23,
                             31, 29, 27, 26, 25, 24,
                             33, 35, 37, 39, 41, 43,
                             48, 50, 52, 47, 49, 45
                           };
int ledPatch[numButtons] = {  5,  4,  3,  2,  1,  0,
                              6,  7,  8,  9, 10, 11,
                              17, 16, 15, 14, 13, 12,
                              18, 19, 20, 21, 22, 23,
                              29, 28, 27, 26, 25, 24,
                              30, 31, 32, 33, 34, 35,
                              41, 40, 39, 38, 37, 36
                           };
Adafruit_NeoPixel clockStrip = Adafruit_NeoPixel(clockPixels, clockPin, NEO_RGB + NEO_KHZ800);
Adafruit_NeoPixel wallStrip = Adafruit_NeoPixel(wallPixels, wallPin, NEO_RGB + NEO_KHZ800);

//--------------------------------------------------------------------------
// Groups for
int all[42];
int row[7][6];
int column[6][7];

// The following variables are for storing colour
uint32_t defColor = wallStrip.Color(30, 144, 255);
uint32_t wallRed = wallStrip.Color(255, 0, 0);
uint32_t wallGreen = wallStrip.Color(0, 255, 0);
uint32_t wallPurple = wallStrip.Color(255, 0, 127);
uint32_t wallOrange = wallStrip.Color(255, 50, 0);
uint32_t wallSpeed = wallStrip.Color(0, 255, 80);
uint32_t wallOff = wallStrip.Color(0, 0, 0);

uint32_t clockDefColor = clockStrip.Color(255, 255, 255);
uint32_t clockOrange = clockStrip.Color(255, 160, 0);
uint32_t clockCyan = clockStrip.Color(0, 255, 255);
uint32_t clockPurple = clockStrip.Color(255, 0, 70);
uint32_t clockOff = clockStrip.Color(0, 0, 0);


// VARIABLES TO STORE GAME STATE
boolean active = true;
boolean failed = false;
int velocity = 1000; //ms
int velocityReduction = 10;

int seq[maxSeqLength];
int level = 0;

//SPEED MODE
boolean speedMode = false;
void setup() {
  Serial.begin(9600); //to allow debugging
  //Set up in/out pins
  pinMode(wallPin, OUTPUT); //clock
  pinMode(clockPin, OUTPUT);
  for ( int i = 0; i < 42; i++) {
    pinMode(pinPatch[i], INPUT);
  }
  // Samples random noise on analogpin 0 to create a random seed
  randomSeed(analogRead(0));

  //Initialze leds
  wallStrip.begin();
  wallStrip.show(); // Initialize all pixels to 'off'
  clockStrip.begin();
  clockStrip.show();

  setupLedGroups();

  chaseFlash(20);
} // END OF SETUP


void loop() {
  //0 check if game is active AND check gamemode
  //1 generate sequence
  //2 display sequence
  //3 check buttons
  //4 if correct, green light, level++ and repeat from 2
  //5 if incorrect, red animation, reset and go back to 0
  setPixel(12, wallPurple);
  setPixel(13, wallSpeed);
  active = (digitalRead(button(12)) == HIGH); // reads from button 12
  speedMode = (digitalRead(button(13)) == HIGH); // reads from button 13

  if (active) { // wait on start buttonpress
    rowFlash(100);
    columnFlash(100);

    level = 0; // resets the level counter
    dispNumber(0, clockDefColor);
    generateSeq(); // generates an array of random nr between 0 and 41

    if (DEBUG) printSeq();
    while (true) { // game loop
      setColor(wallOff);
      dispSeq();

      for (int i = 0; i <= level; i++) {
        setColor(wallOff);
        if (DEBUG) {
          Serial.print("LEVEL: ");
          Serial.println(level);
          Serial.print("seq[i] = ");
          Serial.println(seq[i]);
        }
        // TODO DEBOUNCE BUTTON INPUT
        int input = readButtons(); // loops here until a button is pressed and the value is returned
        if (DEBUG) {
          Serial.print("input = ");
          Serial.println(input);
        }
        if (seq[i] == input) { //Correct button pressed!
          if (DEBUG)Serial.println("CORRECT");
          if ( i == level) {
            if (DEBUG)Serial.println("Last in Seq");
            greenFlash(input, 1);
            theaterChase(wallGreen, 100);
          } else {
            greenFlash(input, 1);
          }
        } else { //Incorrect anser
          if (DEBUG)Serial.println("FAILED");
          failed = true;
          //display failure animation
          for (int i = 0; i < 3; i++) {
            setColor(wallRed);
            delay(800);
            setColor(wallOff);
            delay(800);
          }
          break; //jump out of for loop
        }
      }
      if (failed) {
        if (DEBUG)Serial.println("JUMPING OUT OF GAME LOOP");
        active = false;
        failed = false;
        dispNumber(level, clockOrange);
        break;// break out of game loop
      } else { // WOOP WOOP, level completed!
        level += 1;
        dispNumber(level, clockDefColor);
      }
    }// end of game loop
  } else if (speedMode) { //
    //Display some flashy animation
    rowFlash(100, wallSpeed);
    columnFlash(100, wallSpeed);
    dispNumber(0, clockDefColor);

    // pick a random delay time and button

    int rndDelay = random(100, 5000);
    int speedButton = random (0, 41);

    delay(rndDelay); // wait for the random time
    //light the button and start the timer
    setPixel(speedButton, wallSpeed);
    int buttonTimer = millis();
    int timeLapsed = 0;
    while (true) { // wait for correct button to be pressed
      if (digitalRead(button(speedButton)) == HIGH) {
        timeLapsed = millis() - buttonTimer;
        break;
      }
    }
    theaterChase(wallGreen, 100);
    if (timeLapsed < 1000) {
      int tens = timeLapsed / 100;
      int hundreds = (timeLapsed - (tens * 100) ) / 10;
      String nrStr = "";
      nrStr = nrStr + tens + hundreds;
      int nr = nrStr.toInt();
      dispNumber(nr, clockCyan);
    } else {
      int sec = timeLapsed / 1000;
      int tens = ( timeLapsed - (sec * 1000) ) / 100;
      String nrStr = "";
      nrStr = nrStr + sec + tens;
      int nr = nrStr.toInt();
      dispNumber(nr, clockPurple);
    }

  } else { //Game inactive, run some sort of animation
    // TODO

  }
} // END OF LOOP
//-------------------------------------------------------
// GAME LOGIC

// For translating from user to raw/true button numbers
int button(int nr) {
  return pinPatch[nr];
}
void generateSeq() {
  int lastNr = 0;
  int rnd;
  for (int i = 0; i < maxSeqLength; i++) {
    // to ensure no numbers repeat themselves
    do {
      rnd = random(0, 41);
    } while (rnd == lastNr);
    seq[i] = rnd;
    lastNr = rnd;
  }
}

int readButtons() {
  while (true) {
    for ( int i = 0; i < 42; i++) {
      if ( digitalRead(button(i)) == 1) {
        return i;
      }
    }
  }
}

void printSeq() {
  Serial.println("Printing seq");
  for (int i = 0; i < maxSeqLength; i++) {
    Serial.print(i);
    Serial.print(" : ");
    Serial.println(seq[i]);
  }
}

void dispSeq() {
  for (int i = 0; i <= level; i++) {
    flashPixel(seq[i], dispTime, defColor);
  }
  rowFlash(50);
}
//-------------------------------------------------------------
// LED CONTROL METHODS
// For translating from user to raw/true led numbers
// This is because we only use every third led, and in addition we want all
// rows to start on the left hand side
int led(int nr) {
  return ledPatch[nr] * 3;
}

void greenFlash(int nr, int nrFlashes) {
  for (int i = 0; i < nrFlashes; i++) {
    flashPixel(nr, 400, wallGreen);
    delay(400);
  }
}

//Flashes a group of LEDs
void flashGroup(int group[], int arrLength, int delay_time, uint32_t color) {
  for (int i = 0; i < arrLength; i ++) {
    wallStrip.setPixelColor(led(group[i]), color);
  }
  wallStrip.show();
  delay(delay_time);
  for (int i = 0; i < arrLength; i ++) {
    wallStrip.setPixelColor(led(group[i]), wallOff);
  }
  wallStrip.show();
}

void rowFlash(int delayTime) {
  for (int i = 0; i < 7; i++) {
    flashGroup(row[i], 6, delayTime, defColor);
  }
}
void rowFlash(int delayTime, uint32_t color) {
  for (int i = 0; i < 7; i++) {
    flashGroup(row[i], 6, delayTime, color);
  }
}
void columnFlash(int delayTime) {
  for (int i = 0; i < 6; i++) {
    flashGroup(column[i], 7, delayTime, defColor);
  }
}
void columnFlash(int delayTime, uint32_t color) {
  for (int i = 0; i < 6; i++) {
    flashGroup(column[i], 7, delayTime, color);
  }
}
void chaseFlash(int delayTime) {
  for (int i = 0; i < 42; i++) {
    flashPixel(i, delayTime, defColor);
  }
}

//Flashes a single LED
void flashPixel(int nr, int delay_time, uint32_t color ) {
  setPixel(nr, color);
  delay(delay_time);
  setPixel(nr, wallOff);
}

// Turns on a single pixel of the led wall with a color
// use the color "off" to turn off a pixel
void setPixel(int nr, uint32_t color) {
  wallStrip.setPixelColor(led(nr), color);
  wallStrip.show();
}

// Turns on a single pixel of the led wall with a color
// use the color "off" to turn off a pixel
void setGroup(int group[], int arrLength, uint32_t color) {
  for (int i = 0; i < arrLength; i++) {
    wallStrip.setPixelColor(led(i), color);
  }
  wallStrip.show();
}

void setupLedGroups() {
  for (int i = 0; i < 42; i ++) {
    all[i] = i;
  }
  // Setup rows
  for (int i = 0; i < 7; i ++) {
    for (int j = 0; j < 6; j++) {
      row[i][j] = j + (i * 6);
    }
  }
  // Setup columns
  for (int i = 0; i < 6; i ++) {
    for (int j = 0; j < 7; j++) {
      column[i][j] = i + (6 * j);
    }
  }
}
void clearWall() {
  for (int i = 0; i < 42; i++) {
    wallStrip.setPixelColor(led(i), wallOff);
  }
  wallStrip.show();
}

// sets the ledwall to a given color.
// use the color "off" to turn of the leds
void setColor(uint32_t color) {
  for (int i = 0; i < 42; i++) {
    wallStrip.setPixelColor(led(i), color);
  }
  wallStrip.show();
}
void clearClock() {
  for (int i = 0; i < 29; i++) {
    clockStrip.setPixelColor(i, clockOff);
  }
  clockStrip.show();
}
//---------------------------------------------------
// 7 Segment display logic
/*Method for updating the 7 segment displays.
  args: nr - integer 0-99
  Updates the leds with the colour in the clockColor variable
*/
void dispNumber(int nr, uint32_t clockColor ) {
  // Set all numbers to black
  clearClock();
  int nr_split[] = {nr % 10, nr / 10};
  for (int i = 0; i < 2; i++) {
    switch (nr_split[i]) {
      case 0:
        clockStrip.setPixelColor(0 + (15 * i), clockColor);
        clockStrip.setPixelColor(1 + (15 * i), clockColor);
        clockStrip.setPixelColor(2 + (15 * i), clockColor);
        clockStrip.setPixelColor(3 + (15 * i), clockColor);
        clockStrip.setPixelColor(4 + (15 * i), clockColor);
        clockStrip.setPixelColor(5 + (15 * i), clockColor);
        clockStrip.setPixelColor(8 + (15 * i), clockColor);
        clockStrip.setPixelColor(9 + (15 * i), clockColor);
        clockStrip.setPixelColor(10 + (15 * i), clockColor);
        clockStrip.setPixelColor(11 + (15 * i), clockColor);
        clockStrip.setPixelColor(12 + (15 * i), clockColor);
        clockStrip.setPixelColor(13 + (15 * i), clockColor);
        break;
      case 1:
        clockStrip.setPixelColor(0 + (15 * i), clockColor);
        clockStrip.setPixelColor(1 + (15 * i), clockColor);
        clockStrip.setPixelColor(8 + (15 * i), clockColor);
        clockStrip.setPixelColor(9 + (15 * i), clockColor);
        break;
      case 2:
        clockStrip.setPixelColor(2 + (15 * i), clockColor);
        clockStrip.setPixelColor(3 + (15 * i), clockColor);
        clockStrip.setPixelColor(4 + (15 * i), clockColor);
        clockStrip.setPixelColor(5 + (15 * i), clockColor);
        clockStrip.setPixelColor(6 + (15 * i), clockColor);
        clockStrip.setPixelColor(7 + (15 * i), clockColor);
        clockStrip.setPixelColor(8 + (15 * i), clockColor);
        clockStrip.setPixelColor(9 + (15 * i), clockColor);
        clockStrip.setPixelColor(10 + (15 * i), clockColor);
        clockStrip.setPixelColor(11 + (15 * i), clockColor);
        break;
      case 3:
        clockStrip.setPixelColor(0 + (15 * i), clockColor);
        clockStrip.setPixelColor(1 + (15 * i), clockColor);
        clockStrip.setPixelColor(2 + (15 * i), clockColor);
        clockStrip.setPixelColor(3 + (15 * i), clockColor);
        clockStrip.setPixelColor(6 + (15 * i), clockColor);
        clockStrip.setPixelColor(7 + (15 * i), clockColor);
        clockStrip.setPixelColor(8 + (15 * i), clockColor);
        clockStrip.setPixelColor(9 + (15 * i), clockColor);
        clockStrip.setPixelColor(10 + (15 * i), clockColor);
        clockStrip.setPixelColor(11 + (15 * i), clockColor);
        break;
      case 4:
        clockStrip.setPixelColor(0 + (15 * i), clockColor);
        clockStrip.setPixelColor(1 + (15 * i), clockColor);
        clockStrip.setPixelColor(6 + (15 * i), clockColor);
        clockStrip.setPixelColor(7 + (15 * i), clockColor);
        clockStrip.setPixelColor(8 + (15 * i), clockColor);
        clockStrip.setPixelColor(9 + (15 * i), clockColor);
        clockStrip.setPixelColor(12 + (15 * i), clockColor);
        clockStrip.setPixelColor(13 + (15 * i), clockColor);
        break;
      case 5:
        clockStrip.setPixelColor(0 + (15 * i), clockColor);
        clockStrip.setPixelColor(1 + (15 * i), clockColor);
        clockStrip.setPixelColor(2 + (15 * i), clockColor);
        clockStrip.setPixelColor(3 + (15 * i), clockColor);
        clockStrip.setPixelColor(6 + (15 * i), clockColor);
        clockStrip.setPixelColor(7 + (15 * i), clockColor);
        clockStrip.setPixelColor(10 + (15 * i), clockColor);
        clockStrip.setPixelColor(11 + (15 * i), clockColor);
        clockStrip.setPixelColor(12 + (15 * i), clockColor);
        clockStrip.setPixelColor(13 + (15 * i), clockColor);
        break;

      case 6:
        clockStrip.setPixelColor(0 + (15 * i), clockColor);
        clockStrip.setPixelColor(1 + (15 * i), clockColor);
        clockStrip.setPixelColor(2 + (15 * i), clockColor);
        clockStrip.setPixelColor(3 + (15 * i), clockColor);
        clockStrip.setPixelColor(4 + (15 * i), clockColor);
        clockStrip.setPixelColor(5 + (15 * i), clockColor);
        clockStrip.setPixelColor(6 + (15 * i), clockColor);
        clockStrip.setPixelColor(7 + (15 * i), clockColor);
        clockStrip.setPixelColor(10 + (15 * i), clockColor);
        clockStrip.setPixelColor(11 + (15 * i), clockColor);
        clockStrip.setPixelColor(12 + (15 * i), clockColor);
        clockStrip.setPixelColor(13 + (15 * i), clockColor);
        break;
      case 7:
        clockStrip.setPixelColor(0 + (15 * i), clockColor);
        clockStrip.setPixelColor(1 + (15 * i), clockColor);
        clockStrip.setPixelColor(8 + (15 * i), clockColor);
        clockStrip.setPixelColor(9 + (15 * i), clockColor);
        clockStrip.setPixelColor(10 + (15 * i), clockColor);
        clockStrip.setPixelColor(11 + (15 * i), clockColor);
        break;

      case 8:
        clockStrip.setPixelColor(0 + (15 * i), clockColor);
        clockStrip.setPixelColor(1 + (15 * i), clockColor);
        clockStrip.setPixelColor(2 + (15 * i), clockColor);
        clockStrip.setPixelColor(3 + (15 * i), clockColor);
        clockStrip.setPixelColor(4 + (15 * i), clockColor);
        clockStrip.setPixelColor(5 + (15 * i), clockColor);
        clockStrip.setPixelColor(6 + (15 * i), clockColor);
        clockStrip.setPixelColor(7 + (15 * i), clockColor);
        clockStrip.setPixelColor(8 + (15 * i), clockColor);
        clockStrip.setPixelColor(9 + (15 * i), clockColor);
        clockStrip.setPixelColor(10 + (15 * i), clockColor);
        clockStrip.setPixelColor(11 + (15 * i), clockColor);
        clockStrip.setPixelColor(12 + (15 * i), clockColor);
        clockStrip.setPixelColor(13 + (15 * i), clockColor);
        break;

      case 9:
        clockStrip.setPixelColor(0 + (15 * i), clockColor);
        clockStrip.setPixelColor(1 + (15 * i), clockColor);
        clockStrip.setPixelColor(6 + (15 * i), clockColor);
        clockStrip.setPixelColor(7 + (15 * i), clockColor);
        clockStrip.setPixelColor(8 + (15 * i), clockColor);
        clockStrip.setPixelColor(9 + (15 * i), clockColor);
        clockStrip.setPixelColor(10 + (15 * i), clockColor);
        clockStrip.setPixelColor(11 + (15 * i), clockColor);
        clockStrip.setPixelColor(12 + (15 * i), clockColor);
        clockStrip.setPixelColor(13 + (15 * i), clockColor);
        break;
    }
  }
  clockStrip.show();
}

//---------------------------------------------------
//strandtest-metodene:
// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < wallStrip.numPixels(); i++) {
    wallStrip.setPixelColor(i, c);
    wallStrip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < wallStrip.numPixels(); i++) {
      wallStrip.setPixelColor(i, Wheel((i + j) & 255));
    }
    wallStrip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < wallStrip.numPixels(); i++) {
      wallStrip.setPixelColor(i, Wheel(((i * 256 / wallStrip.numPixels()) + j) & 255));
    }
    wallStrip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j = 0; j < 4; j++) { //do 10 cycles of chasing
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < wallStrip.numPixels(); i = i + 3) {
        wallStrip.setPixelColor(led(i + q), c);  //turn every third pixel on
      }
      wallStrip.show();
      delay(wait);
      for (uint16_t i = 0; i < wallStrip.numPixels(); i = i + 3) {
        wallStrip.setPixelColor(led(i + q ), 0);      //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j = 0; j < 256; j++) {   // cycle all 256 colors in the wheel
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < wallStrip.numPixels(); i = i + 3) {
        wallStrip.setPixelColor(i + q, Wheel( (i + j) % 255)); //turn every third pixel on
      }
      wallStrip.show();

      delay(wait);

      for (uint16_t i = 0; i < wallStrip.numPixels(); i = i + 3) {
        wallStrip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return wallStrip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return wallStrip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return wallStrip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

//-----------------------------------------------------------
// TESTMETODER
void seg7Test() {
  for (int i = 0; i < 100; i++) {
    dispNumber(i, clockDefColor);
    delay(500);
  }
}
void checkButtons() {
  for ( int i = 0; i < 42; i++) {
    if ( digitalRead(button(i)) == 1) {
      setPixel(i, defColor);
    } else {
      setPixel(i, wallOff);
    }
  }
}
