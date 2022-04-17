
void speedMode() {
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
}
