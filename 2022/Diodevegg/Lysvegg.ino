/* Versjon 1.31
   Lastet opp 9.august 2018
   av Isak Bjørn, Elias og Mikro

   Program skrevet til SimonSays veggen til Mikros arrangement Escape Velocity.
   Det kontrollerer en vegg bestående av 42 knapper med tilhørende led-dioder
   Programmet forholder seg til det vi kan kalle bruker nr, dette er en "logisk" addresering.
   Sett fra front:

      0,  1,  2,  3,  4,  5,
      6,  7,  8,  9, 10, 11,
     12, 13, 14, 15, 16, 17,
     18, 19, 20, 21, 22, 23,
     24, 25, 26, 27, 28, 29,
     30, 31, 32, 33, 34, 35,
     36, 37, 38, 39, 40, 41

     Da knappene og led-ene ikke er koblet opp på en logisk måte brukes to metoder button() og led() til å oversette
     fra brukernr til reelle nr. Så hvis man vil lese av knapp to fra venstre på øverste rad bruker man
     digitalRead( button(2) )

   Dette programmet kan fritt brukes og deles av alle som trur de kan gjøre noe kult med det.

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
#define maxSeqLength 99
#define dispTime 700 // The time each step of the sequence will light up (lower number = harder)

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
uint32_t wallLightRed = wallStrip.Color(255, 30, 15);
uint32_t wallGreen = wallStrip.Color(0, 255, 0);
uint32_t wallBlue = wallStrip.Color(0, 0, 255);
uint32_t wallPurple = wallStrip.Color(255, 0, 127);
uint32_t wallOrange = wallStrip.Color(255, 50, 0);
uint32_t wallSpeed = wallStrip.Color(0, 255, 80);
uint32_t wallOff = wallStrip.Color(0, 0, 0);
uint32_t wallGrey = wallStrip.Color(40, 40, 40);

uint32_t clockDefColor = clockStrip.Color(255, 255, 255);
uint32_t clockOrange = clockStrip.Color(255, 160, 0);
uint32_t clockRed = clockStrip.Color(255, 0, 0);
uint32_t clockCyan = clockStrip.Color(0, 255, 255);
uint32_t clockPurple = clockStrip.Color(255, 0, 70);
uint32_t clockOff = clockStrip.Color(0, 0, 0);


//Keeping the clock:
uint32_t clock_color_value;
int clock_current_num;
unsigned long refresh_timer;


// VARIABLES TO STORE GAME STATE
boolean failed = false;
int velocity = 1000; //ms
int velocityReduction = 10;

int seq[maxSeqLength];
int level = 0;

boolean memoryModeSelected = false;

//SPEED MODE
boolean speedModeSelected = false;

//FLASH MODE
boolean flashModeSelected = false;

boolean gameOfLifeModeSelected = false;
int smallMode = 0; // Divided by 6 gived row 

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
  dispNumber(0, clockDefColor);

  smallMode = numberButtons() - 1;
} // END OF SETUP


void loop() {
  //light the buttons containing a mode
  setPixel(12, wallPurple);
  setPixel(13, wallSpeed);
  setPixel(14, wallOrange);
  setPixel(15, wallGreen);
  
  //Check if a mode has been selected
  memoryModeSelected = (digitalRead(button(12)) == HIGH); // reads from button 12
  speedModeSelected = (digitalRead(button(13)) == HIGH); // reads from button 13
  flashModeSelected = (digitalRead(button(14)) == HIGH); // reads from button 14
  gameOfLifeModeSelected = (digitalRead(button(15)) == HIGH); // reads from button 15

  //If a mode has been selected, start it
  if (memoryModeSelected) memoryMode();
  else if (speedModeSelected) speedMode();
  else if (flashModeSelected) flash_mode(numberButtons()); // < Choose a number  
  else if (gameOfLifeModeSelected) gameOfLife();
  else {  //Game inactive, run some sort of animation
    random(42); //rng advancer.
    //hmm
    clock_refresh_method();
  }
} // END OF LOOP


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




//1 generate sequence
//2 display sequence
//3 check buttons
//4 if correct, green light, level++ and repeat from 2
//5 if incorrect, red animation, reset and go back to 0
