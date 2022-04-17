void memoryMode() {
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
      } else { //Incorrect answer
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
