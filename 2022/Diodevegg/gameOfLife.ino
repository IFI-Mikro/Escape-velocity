/**
   Conways Game of life
   Hilmar ville også ha noe på veggen,
   så da laget han det første han kom på.
   Veggen kan nå også brukes som fasit til
   den siste in1000 obligen

   Når moden er i gang, velger man hvilke
   leds som skal starte som levende. slik
   det er satt opp nå kan alle leds starte
   levende, bortsett fra 41, da denne starter
   simulasjonen

   Bruksanvisning:
    1. Velg game of life i hovedmenyen
    2. trykk på knappene du vil skal starte levende
    3. trykk in 41 for å starte simulasjonen


*/
#define for_x for (int x = 0; x < 6; x++)
#define for_y for (int y = 0; y < 7; y++)
#define for_xy for_x for_y

void show(int board[6][7]) {
  setColor(wallOff);
  int numberOfLiveCells = 0;
  for_y {
    for_x {
      if (board[x][y]) {
        setPixel(x + 6 * y, wallGreen);
        numberOfLiveCells++;
      }
    }
  }
  for_x {
    for_y{
      Serial.print(board[x][y]);
    }
    Serial.println();
  }
  dispNumber(numberOfLiveCells, wallGreen);
}
//returnerer true om det finnes en levende celle
//returnerer false om alle celler er døde

boolean evolve(int board[6][7]) {
  boolean boardNotDead = false;
  unsigned newBoard[6][7];

  for_x for_y {
    int n = 0;
    for (int i = -1; i <= 1 ; i++)
      for (int j = - 1; j <= 1; j++)
        if (board[( x + i + 6) % 6][(y + j + 7) % 7]) n++;

    if (board[x][y]) n--;
    newBoard[x][y] = (n == 3 || (n == 2 && board[x][y]));
  }
  for_y for_x {
    board[x][y] = newBoard[x][y];
    if (board[x][y]) {
      boardNotDead = true;
      Serial.println("hei, jeg lecer!");
    }
  }
  return boardNotDead; //

}

void getSeed(int board[6][7]) {
  Serial.println("getting seed input");
  delay(300);//debounce

  for_xy board[x][y] = 0;

  while (!digitalRead(button(41))) {
    for (int i = 0; i < 41; i++) {
      int x = i / 6;
      int y = i % 6;
      if (digitalRead(button(i))) {
        board[x][y] = 1;
        Serial.print("setting ("); Serial.print(x); Serial.print(", "); Serial.print(y); Serial.println(")");
        setPixel(i, wallGreen);
        delay(100);
      }
    }
  }
  Serial.println();

  delay(300);//debounce


}
void gameOfLife() {
  Serial.println("game of life started");
  setColor(wallOff);
  int board[6][7];

  getSeed(board);
  delay(300);//debounce
  show(board);

  while (!buttonsPressed() && evolve(board)) {
    show(board);
    Serial.println("\n");
    delay(1000);

  }
  setColor(wallOff);

}

void printBoard(int board[6][7]) {
  for_x{
    for_y Serial.print(board[x][y]);
    Serial.println();
  }
}
