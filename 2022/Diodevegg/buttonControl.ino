//wait for a buttonpress and return which button was pressed
int readButtons() {
  while (true) {
    for ( int i = 0; i < 42; i++) {
      if ( digitalRead(button(i)) == 1) {
        return i;
      }
    }
  }
}
//check if any buttons have been pressed
boolean buttonsPressed() {
  for ( int i = 0; i < 42; i++) {
    if ( digitalRead(button(i)) == 1) {
      return true;
    }
  }
  return false;
}

// For translating from user to raw/true button numbers
int button(int nr) {
  return pinPatch[nr];
}
int button(int x, int y){
  return pinPatch[6*y + x];
}


//function used to select active buttons at startup
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
