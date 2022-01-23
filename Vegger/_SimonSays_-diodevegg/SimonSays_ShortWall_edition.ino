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
boolean memoryMode = false;
boolean failed = false;
int velocity = 1000; //ms
int velocityReduction = 10;

int seq[maxSeqLength];
int level = 0;

//SPEED MODE
boolean speedMode = false;

//FLASH MODE
boolean flashMode = false;

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
  //0 check if game is active AND check gamemode
  //1 generate sequence
  //2 display sequence
  //3 check buttons
  //4 if correct, green light, level++ and repeat from 2
  //5 if incorrect, red animation, reset and go back to 0
  setPixel(12, wallPurple);
  setPixel(13, wallSpeed);
  setPixel(14, wallOrange);
  memoryMode = (digitalRead(button(12)) == HIGH); // reads from button 12
  speedMode = (digitalRead(button(13)) == HIGH); // reads from button 13
  flashMode = (digitalRead(button(14)) == HIGH); // reads from button 14

  if (memoryMode) { // wait on start buttonpress
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
            theaterChase(wallGreen, 80);
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
        failed = false;
        dispNumber(level, clockOrange);
        break;// break out of game loop
      } else { // WOOP WOOP, level completed!
        if (level == 99) {
          // YOU WON THE GAME!!!!
          dispNumber(level, clockOrange);
          rainbowCycle(10);
          failed = false;
          break; // break out of game loop
        } else {
          level += 1;
          dispNumber(level, clockDefColor);
        }
      }
    }// end of game loop
  } else if (speedMode) { //
    //Display some flashy animation
    rowFlash(100, wallSpeed);
    columnFlash(100, wallSpeed);
    dispNumber(0, clockDefColor);

    // pick a random delay time and button
    int rndDelay = random(100, 5000);
    int speedButton = random (smallMode, 42); //adjusted to bottom 4 rows.

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
  } else if (flashMode) { 
    // START OF FLASH MODE
    //flash_mode(10); // < hardcoded >> kanskje best for fadderuka?
    flash_mode(numberButtons()); // < Choose a number
    // END OF FLASH MODE
  
  } else { //Game inactive, run some sort of animation
    random(42); //rng advancer.
    //hmm
    clock_refresh_method();
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
      rnd = random(smallMode, 42); // var orginalt satt til 41? 42 er vel mer riktig? //sequence nå på 4 nederste rader 
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
  rowFlash(30);
}

// Flash_Mode functions:
//int disabled_flash = 0; //Set value to 2 to instantly enable..
void flash_mode(int num_active_buttons){ //>input< THIS DECIDES A bit, so that changing it will adjust all other settings, in a perfectly fine way.
    
    //if (disabled_flash == 0) return;
    //else if (disabled_flash == 1) {disabled_flash++; return;}
    swipeAClear(wallLightRed, 14);
    clearClock();
    clearWall();
    // CONSTANTS/SETUP 
    //int const perfect_score_here = num_active_buttons*3;
    //int const worst_score_here = -(42 - num_active_buttons)*2 - num_active_buttons*3;
    
    bool that_pattern[42]; // to map the right buttons
    int next_button = 0; //to choose a button for the pattern
    bool not_at_all_used; //to see if the button is already inside the pattern 
    int my_countdown_num = num_active_buttons/3 + 5; // countdown value
    int clock_counting = 2; // added control
    for (int i = 0; i < 42; i++) that_pattern[i] = false; // It will store true when the button is used.....


    // GAME RULES
    // right gives +3
    // wrong gives -2
    // missed right gives -2
    // point modes: (positive) (negative) (max score) (zero)
    //kind of dont alter this.......

  

    // Generate and store a random image pattern
    next_button = int(random(smallMode, 42));//int(random(42));
    for (int myI = 0; myI<num_active_buttons; myI++){
      not_at_all_used = true;
      while(that_pattern[next_button]){
        next_button = int(random(smallMode, 42));//int(random(42));
      }
      that_pattern[next_button] = true;
      setPixel(next_button, wallBlue);
      if (clock_counting == 2) {
        clock_counting = 0;
        dispNumber(my_countdown_num, clockOrange);
        my_countdown_num--;
      } else {
        clock_counting++;
      }
      delay(333);   
    }

    // Show image pattern for 20 sec with countdown
    dispNumber(my_countdown_num, clockOrange);
    for (my_countdown_num = my_countdown_num -1; my_countdown_num > -1; my_countdown_num--){
      delay(999);
      dispNumber(my_countdown_num, clockOrange);
    }
     
    // Clear an wait 30 sec with countdown >>>>> Added responsiveness (Show pressed butons);
    delay(100);
    clearClock();
    // for (int i = 6; i >= 0; i--) swipe0Clear((i%2 == 1) ? wallRed : wallBlue, i%3); //very pretty but:::
    
    randomSwiping(7, 1, 100);
    // more confusion
    clearWall();
    long current;
    for (my_countdown_num = 10; my_countdown_num > -1; my_countdown_num--){ 
      current = millis();
      while ((millis() - current) < 200) trackButtons(wallOrange);
      dispNumber(my_countdown_num, clockOrange);
    }   

    checkPattern_flash(num_active_buttons, that_pattern); 
    clearWall();
}

void randomSwiping(int num, int delay_num, int delay_two) {
  uint32_t a_color = wallBlue;
  for (int j = 0; j<num; j++) {
    a_color = (a_color == wallBlue)? wallRed : wallBlue;
    switch(int(random(8))){
      case 0: swipeAClear(a_color, delay_num); break; case 1: swipeBClear(a_color, delay_num); break; case 2: swipeCClear(a_color, delay_num); break; case 3: swipeDClear(a_color, delay_num); break;
      case 4: midSwipeAClear(a_color, delay_num); break; case 5: midSwipeBClear(a_color, delay_num); break; case 6: midSwipeCClear(a_color, delay_num); break; case 7: midSwipeDClear(a_color, delay_num); break;
    }
    delay(delay_two);
  }
}

int numberButtons() { // THIS IS A BIT FINICKY AND SHOULD BE REMADE..... //all good, also 18 adjusted in this script/edition...
    clearClock();
    swipeCClear(wallGrey, 1);
    int value = 0;      
    dispNumber(value, wallGrey);
    
    while(value == 0) {
      for ( int i = smallMode; i < 42; i++) {
        if ( digitalRead(button(i)) == 1) {
          setPixel(i, clockPurple);
          dispNumber(i + 1 - smallMode, clockPurple);
          setPixel((i+41)%42, wallGreen);
          setPixel((i+1)%42, wallRed);
          while((digitalRead(button((i+1)%42)) == 0) && (value == 0)){
            if (digitalRead(button((i+41)%42)) == 1) value = (i + 1 - smallMode);
          }
          clearWall();
          dispNumber(value, wallGrey);
          delay(200);
          
          
        }
      }
    }
    //if (value == 30) disabled_flash++; // select 30 once to enable flash_Mode..
    return value;
}



void storeButtons(bool allButtons[], uint32_t color) {
  for ( int i = 0; i < 42; i++) {
    if ( digitalRead(button(i)) == 1) {
      setPixel(i, color);
      allButtons[i] = true;
    } else if (!allButtons[i]) {
      setPixel(i, wallOff);
    }
  }
}

void trackButtons(uint32_t color) {
  for ( int i = 0; i < 42; i++) {
    if ( digitalRead(button(i)) == 1) {
      setPixel(i, color);
    } else {
      setPixel(i, wallOff);
    }
  }
}

void checkPattern_flash(int num_active_buttons, bool that_pattern[] ){
    // to map all the pressed buttons:
    int right = 0;
    int wrong = 0;
    int missed = 0;
    bool button_used;
    bool button_value;

    //int right_pressed[num_active_buttons];

    //int missed_pressed[num_active_buttons];
    
    //int wrong_pressed[42 - num_active_buttons];

    bool buttons1[42];
    for (int i = 0; i < 42; i++) buttons1[i] = false; // just to make sure...
    for(int i = 0; i<5; i++) storeButtons(buttons1, clockPurple); //for singleplayer, just make all checks this. but for now, it's like this because olympiaden og samarbeid + enklere kode.
    clearClock();
    delay(1000);
    // Check pattern
    for (int myI = 0; myI<42; myI++){
       button_used = that_pattern[myI];
       button_value = buttons1[myI];
       if (button_used) {
        if (button_value) {
          //right_pressed[right] = myI;
          right++;
          setPixel(myI, wallGreen);
        } else {
          //missed_pressed[missed] = myI;
          missed++;
          setPixel(myI, wallRed);
        }
       } else if (button_value) { 
        //wrong_pressed[wrong] = myI;
        wrong++;
        setPixel(myI, wallLightRed);
       } else {
        flashPixel(myI, 1, wallGrey);
       }
    }

    

    /*
    //__Score Debug__:
    int butttt = 15;
    setPixel(butttt, wallBlue);
    dispNumber(right, wallGreen);
    while(digitalRead(button(butttt)) == LOW) delay(10);
    butttt++;
    setPixel(butttt, wallBlue);
    dispNumber(missed, wallRed);
    while(digitalRead(button(butttt)) == LOW) delay(10);
    butttt++;
    setPixel(butttt, wallBlue);
    dispNumber(wrong, wallOrange);
    while(digitalRead(button(butttt)) == LOW) delay(10);
    */

    
    // Show rights / wrongs / missed and display score
    int score = (3 * right) - (2 * wrong) - (2 * missed); 

    uint32_t this_color = wallBlue;
    button_value = false;
    if (score < 0) {
      this_color = wallRed;
      score = -score;
    } else if (score > 0) {
      if (score == num_active_buttons * 3) {
        button_value = true;
      }
      this_color = wallGreen;
    }
    if (score > 99) score = 99;
    long ohNo;
    for (int myI = 0; myI < score; myI++){
      dispNumber(myI, this_color);
      ohNo =  (long)((myI * 10) +((1000 * (myI*myI + 1))/(score*score+1)));
      if (ohNo < 200) delay(200);
      else if (ohNo > 2000) delay(2000);
      else delay(ohNo);
    }
    if (button_value) dispNumber(score, wallPurple); // you have achieved max score...
    else dispNumber(score, this_color);
    delay(5000);
    swipeBClear(wallGrey, 15);
}
/// ______END FLASHMODE_______




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
  clock_color_value = clockColor;
  clock_current_num = nr;
  refresh_timer = millis();
}
/*
 * uint32_t clock_color_value;
 * int clock_current_num;
 * long refresh_timer;
 */
void clock_refresh_method(){
  if ((millis() - refresh_timer) < 600000UL) return;
  dispNumber(clock_current_num, clock_color_value); 
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

void midSwipeAClear(uint32_t color, int delNum) {
  for (int i = 0; i<8; i++) {
    setASwipe(6 + i, color, delNum);
    setASwipe(5 - i, color, delNum);
    if (i>1) continue;
    setASwipe(4 + i, wallOff, delNum);
    setASwipe(7 - i, wallOff, delNum);
  }
  for (int i = 7; i>-1; i--) {  
    if (i>1) {
      setASwipe(4 + i, color, delNum);
      setASwipe(7 - i, color, delNum);
    }
    setASwipe(6 + i, wallOff, delNum);
    setASwipe(5 - i, wallOff, delNum); 
  }
}

void midSwipeBClear(uint32_t color, int delNum) {
   for (int i = 7; i>0; i--) {  
    if (i>1) {
      setASwipe(4 + i, color, delNum);
      setASwipe(7 - i, color, delNum);
    }
    setASwipe(6 + i, wallOff, delNum);
    setASwipe(5 - i, wallOff, delNum); 
  }
  for (int i = 1; i<8; i++) {
    setASwipe(6 + i, color, delNum);
    setASwipe(5 - i, color, delNum);
    if (i>1) continue;
    setASwipe(4 + i, wallOff, delNum);
    setASwipe(7 - i, wallOff, delNum);
  }
}

void midSwipeCClear(uint32_t color, int delNum) {
  for (int i = 0; i<8; i++) {
    setBSwipe(6 + i, color, delNum);
    setBSwipe(5 - i, color, delNum);
    if (i>1) continue;
    setBSwipe(4 + i, wallOff, delNum);
    setBSwipe(7 - i, wallOff, delNum);
  }
  for (int i = 6; i>-1; i--) {
    if (i>1) {
      setBSwipe(4 + i, color, delNum);
      setBSwipe(7 - i, color, delNum);
    }
    setBSwipe(6 + i, wallOff, delNum);
    setBSwipe(5 - i, wallOff, delNum); 
  }
}

void midSwipeDClear(uint32_t color, int delNum) {
  for (int i = 7; i>0; i--) {
    if (i>1) {
      setBSwipe(4 + i, color, delNum);
      setBSwipe(7 - i, color, delNum);
    }
    setBSwipe(6 + i, wallOff, delNum);
    setBSwipe(5 - i, wallOff, delNum); 
  }
  for (int i = 1; i<8; i++) {
    setBSwipe(6 + i, color, delNum);
    setBSwipe(5 - i, color, delNum);
    if (i>1) continue;
    setBSwipe(4 + i, wallOff, delNum);
    setBSwipe(7 - i, wallOff, delNum);
  }
}

void swipeAClear(uint32_t color, int delNum){
  for (int i = 0; i<14; i++) {
    setASwipe(i, color, delNum);
    setASwipe(i-2, wallOff, delNum);
  }
}

void swipeBClear(uint32_t color, int delNum) {
  for (int i = 13; i > -1; i--) {
    setASwipe(i-2, color, delNum);
    setASwipe(i, wallOff, delNum);
  }
}

void swipeCClear(uint32_t color, int delNum){
  for (int i = 0; i<14; i++) {
    setBSwipe(i, color, delNum);
    setBSwipe(i-2, wallOff, delNum);
  }
}

void swipeDClear(uint32_t color, int delNum) {
  for (int i = 13; i > -1; i--) {
    setBSwipe(i-2, color, delNum);
    setBSwipe(i, wallOff, delNum);
  }
}

void setASwipe(int i, uint32_t color, int delNum) {
  if (i < 0) return;
  if (i > 11) return;
  if (i < 6) {
    for (int j = 0; j<=i; j++) {
        setPixel(i + j*5, color);
        delay(delNum);
    }
    return;
  }
  int val = (i - 6) * 6 + 11;
  for (int j = 11 - i; j >= 0; j--) {
        setPixel(val + j*5, color);
        delay(delNum);
  }
}

void setBSwipe(int i, uint32_t color, int delNum) {
  if (i < 0) return;
  if (i > 11) return;
  int val;
  if (i < 6) {
    val = 5-i;
    for (int j = 0; j<=i; j++) {
        setPixel(val + j*7, color);
        delay(delNum);
    }
    return;
  }
  val = (i - 6) * 6 + 6;
  for (int j = 11 - i; j >= 0; j--) {
        setPixel(val + j*7, color);
        delay(delNum);
  }
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

