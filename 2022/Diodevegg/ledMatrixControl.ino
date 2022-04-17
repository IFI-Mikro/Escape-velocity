// LED CONTROL METHODS
// For translating from user to raw/true led numbers
// This is because we only use every third led, and in addition we want all
// rows to start on the left hand side
int led(int nr) {
  return ledPatch[nr] * 3;
}
int led(int x, int y){
  return ledPatch[x + 6*y] * 3;
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
