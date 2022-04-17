// 7 Segment display logic
/*Method for updating the 7 segment displays.
  args: nr - integer 0-99
  Updates the leds with the colour in the clockColor variable
*/

/*
 * uint32_t clock_color_value;
 * int clock_current_num;
 * long refresh_timer;
 */
void clock_refresh_method(){
  if ((millis() - refresh_timer) < 600000UL) return;
  dispNumber(clock_current_num, clock_color_value); 
}


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


void clearClock() {
  for (int i = 0; i < 29; i++) {
    clockStrip.setPixelColor(i, clockOff);
  }
  clockStrip.show();
}
