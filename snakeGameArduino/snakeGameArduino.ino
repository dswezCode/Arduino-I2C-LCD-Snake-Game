/*****************************************************************************
 *      _____ _   _  ___  _   __ _____  _   _                                *
 *     /  ___| \ | |/ _ \| | / /|  ___|| | | |                               *
 *     \ `--.|  \| / /_\ \ |/ / | |__  | | | |                               *
 *      `--. \ . ` |  _  |    \ |  __| | | | |                               *
 *     /\__/ / |\  | | | | |\  \| |___ |_| |_|                               *
 *     \____/\_| \_/_| |_/_| \_/\____/ (_) (_)                               *
 *                                                                           *
 * PROJECT:      ARDUINO I2C LCD 16x2 SNAKE GAME (16x4 GAMEPLAY)             *
 * AUTHOR:       github - dswezCode                                          *
 * VERSION:      8.0                                                         *
 *                                                                           *
 * DESCRIPTION:                                                              *
 * A fully playable, classic Snake game built for the Arduino Uno.           *
 * Implemented on a standard 16x2 I2C LCD, Analog joystick,passive buzzer    *
 * and a breadboard.                                                         *
 *                                                                           *
 * GAMEPLAY FEATURES:                                                        *
 * - Persistent High Scores & Wins: Powered by the Arduino's internal        *
 *   EEPROM memory, your High Score and Total Wins are saved permanently.    *
 * - Continuous play mechanic: if you win by getting to maximum              *
 *   snakes length the snake resets while keeping your score                 *
 *   allowing you to continue playing (win count grows by 1 each run you won *
 *   at least once by achieving maximum length, wins dont stack up in        *
 *   a single run)                                                           *
 * - Dynamic game speed: the longer you play the faster the snake becomes    *
 *   until it reaches max speed, if you win the game and the snake resets    *
 *   the max speed is kept.                                                  *
 * - custom tones that play on a passive buzzer to enhance                   *
 *   gameplay experience                                                     *
 *                                                                           *
 * TECHNICAL FEATURES:                                                       *
 * - Using dynamically generated custom characters during gameplay loop      *
 *   to generate more custom characters bypassing screen ram limit of only   *
 *   8 graphic characters                                                    *
 * - Using an engine that allows 16x4 res gameplay on a 16x2 screen          *
 * - Delta-rendering engine (draws new frames only if change is needed)      *
 * - Persistent high scores and win tracking (saved to Arduino EEPROM)       *
 * - State-machine audio engine (non-blocking sound effects to not cause lag)*
 * - Hardware debounce for smooth joystick controls                          *
 * - True RNG seeding using human reaction time                              *
 * - using internal arduino clock to avoid using delay(),                    *
 *   in game loop (to prevent lag)                                           *
 *                                                                           *
 *  CAUTION:                                                                 *
 * -The program uses and updates arduinos EEPROM memory to keep track        *
 *  of scores and wins, be sure that you dont need/use the specific          *
 *  slots of this memory before running the program, you also can change     *
 *  the eeprom addresses this program is using by changing the               *
 *  "eeAddressWins" and "eeAddressHighScore" variables                       *
 *  (for example eeAddressWins = 4; eeAddressHighScore=6;)                   *
 *  make sure each address is 2 steps away from the preceding one because int*
 *  variables take 2 places of memory                                        *
 *                                                                           *
 *  HOW TO PLAY:                                                             *
 * -wire everything according to the hardware connections specified below    *
 *  flash the program to the Arduino uno board, power the Arduino board      *
 *  once the main menu loads press joystick to srart the game                *
 *                                                                           *
 * HARDWARE CONNECTIONS:                                                     *
 * - Joystick 5v         -> 5v supply                                        *
 * - Joystick GND        -> GND                                              *
 * - Joystick X-Axis     -> Analog A0                                        *
 * - Joystick Y-Axis     -> Analog A1                                        *
 * - Joystick Button/SW  -> Digital Pin 8                                    *
 * - Passive Buzzer +    -> Digital Pin 9                                    *
 * - Passive Buzzer -    -> GND                                              *
 * - 16x2 I2C LCD VCC    -> 5v supply                                        *
 * - 16x2 I2C LCD GND    -> GND                                              *
 * - 16x2 I2C LCD SDA    ->Arduino SDA pin                                   *
 * - 16x2 I2C LCD SCL    ->Arduino SCL pin                                   *
 *                                                                           *
 * LIBRARIES REQUIRED:                                                       *
 * - LiquidCrystal_I2C                                                       *
 * - EEPROM (Built-in)                                                       *
 *                                                                           *
 *****************************************************************************/


//libraries included
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>


LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- HARDWARE PINS ---
const int xAxisPin = A0;    
const int yAxisPin = A1;    
const int zAxisPin = 8;     
const int buzzerPin = 9;  


// --- GRID CELL STATES ---
#define CELL_EMPTY 0
#define CELL_BODY  1
#define CELL_APPLE 2
#define CELL_HEAD  3

// --- AUDIO FREQUENCIES ---
#define NOTE_DS5 622  
#define NOTE_D5  587  
#define NOTE_CS5 554  
#define NOTE_C5  523
#define NOTE_E5  659
#define NOTE_G5  784
#define NOTE_C6  1047
#define NOTE_C7  2093 
#define NOTE_E7  2637 
#define NOTE_G7  3136 
#define NOTE_E6  1319
#define NOTE_G6  1568

// ==========================================
// --- INTRO SCREEN CUSTOM CHARACTERS ---
// ==========================================
//those chars are made to create the snake in the intro scene 

// Top Row 1: Far left back curve
byte top0[8] = {B00000, B00000, B00001, B00001,B00011, B00011, B00111, B00111};
// Top Row 2: Left side of the head/arch
byte top1[8] = {B00111, B11111, B11111, B11110,B11100, B11000, B10000, B10000};
// Top Row 3: Right side of head and Left Eye
byte top2[8] = {B11000, B11110, B11111, B10110, B11111, B01111, B00111, B00010};
// Top Row 4: Snout and Right Eye
byte top3[8] = {B00000, B00000, B00000, B00000, B00000, B00000, B00000, B00000};
// Bottom Row 1: The tail
byte bot0[8] = {B00111, B00111, B00011, B00011,B00001, B00000, B00111, B11111};
// Bottom Row 2: Thick body base
byte bot1[8] = {B10000, B10000, B11000, B11111,B11111, B11111, B11111, B11111};
// Bottom Row 3: Neck swooping down into base
byte bot2[8] = {B00000, B00000, B00000, B11110,B11111, B11111, B11111, B11111};
// Bottom Row 4: Base extending right
byte bot3[8] = {B00000, B00000, B00000, B00000,B10000, B11100, B11110, B11111};

// ==========================================
// --- 16x4 ENGINE CUSTOM CHARACTERS ---
// ==========================================
//all possible graphic chars in a 16x4 engine

byte snake_top[8]   = {B01110, B10001, B10001, B01110, B00000, B00000, B00000, B00000}; 
byte snake_bot[8]   = {B00000, B00000, B00000, B00000, B01110, B10001, B10001, B01110}; 
byte snake_full[8]  = {B01110, B10001, B10001, B01110, B01110, B10001, B10001, B01110}; 
byte apple_top[8]   = {B00100, B01110, B01110, B00000, B00000, B00000, B00000, B00000}; 
byte apple_bot[8]   = {B00000, B00000, B00000, B00000, B00000, B00100, B01110, B01110}; 
byte apple_top_snake_bot[8] = {B00100, B01110, B01110, B00000, B01110, B10001, B10001, B01110}; 
byte snake_top_apple_bot[8] = {B01110, B10001, B10001, B01110, B00000, B00100, B01110, B01110}; 

// --- BASE 4-ROW GRAPHICS ---
//made for rendering the head dynamically ( to bypass screen 8 char limit )
byte head_half[4]  = {B01010, B11111, B11111, B01010}; 
byte body_half[4]  = {B01110, B10001, B10001, B01110}; 
byte apple_half[4] = {B00100, B01110, B01110, B00000}; 
byte empty_half[4] = {B00000, B00000, B00000, B00000}; 

// Directions
const int DIR_UP    = 0;
const int DIR_RIGHT = 1;
const int DIR_DOWN  = 2;
const int DIR_LEFT  = 3;

//total wins variable
unsigned int wins = 0;
unsigned int highScore = 0;

//the eeprom memory address for the wins variable
int eeAddressWins = 0;
int eeAddressHighScore = 2;


/*** SETUP OF THE GAME ***/
void setup() {
  
  //init screen
  lcd.init();                
  lcd.backlight();          

  // --- 1. LOAD INTRO SCREEN CHARS---
  // The I2C LCD can only hold 8 custom characters at a time.
  lcd.createChar(0, top0);
  lcd.createChar(1, top1);
  lcd.createChar(2, top2);
  lcd.createChar(3, top3);
  lcd.createChar(4, bot0);
  lcd.createChar(5, bot1);
  lcd.createChar(6, bot2);
  lcd.createChar(7, bot3);

  lcd.clear();
  
  // Draw the Big Snake in the intro screen using the chars above
  lcd.setCursor(6, 0); 
  lcd.write(byte(0)); lcd.write(byte(1)); lcd.write(byte(2)); lcd.write(byte(3));
  lcd.setCursor(6, 1);
  lcd.write(byte(4)); lcd.write(byte(5)); lcd.write(byte(6)); lcd.write(byte(7));
  
  // Add logo text
  lcd.setCursor(0, 0); lcd.print("SNAKE");
  lcd.setCursor(11, 1); lcd.print("GAME!");

  // Wait so the player can see it
  delay(4000);

  // --- 2. OVERWRITE LCD MEMORY WITH ACTUAL GAME GRAPHICS ---
  // The intro snake is wiped from CGRAM memory, replaced by game engine shapes
  // that will be used in the game loop

  lcd.createChar(0, snake_top); 
  lcd.createChar(1, snake_bot);    
  lcd.createChar(2, snake_full);    
  lcd.createChar(3, apple_top);    
  lcd.createChar(4, apple_bot);    
  lcd.createChar(5, apple_top_snake_bot);    
  lcd.createChar(6, snake_top_apple_bot);    

  // Setup hardware
  pinMode(zAxisPin, INPUT_PULLUP);  
  pinMode(buzzerPin, OUTPUT);  

  // Get variables from arduino eeprom memory
  EEPROM.get(eeAddressWins, wins);
  EEPROM.get(eeAddressHighScore, highScore);

  // --- EEPROM FIRST-TIME SETUP CHECK ---
  // A completely blank/new EEPROM returns 65535 (0xFFFF) for unsigned ints.
  // If we detect this, it means the game has never been played on this Arduino.
  // We must reset the variables to 0 and format the EEPROM.
  
  if (wins == 65535) {
    wins = 0;
    EEPROM.put(eeAddressWins, wins);
  }
  
  if (highScore == 65535) {
    highScore = 0;
    EEPROM.put(eeAddressHighScore, highScore);
  }
  
}

/*** MAIN LOOP OF THE GAME ***/
void loop() {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print(" Play SNAKE!  ");
  
  // Dynamic Main Menu Text
  lcd.setCursor(0, 1);
  lcd.print(" HS:"); lcd.print(highScore);
  lcd.setCursor(9, 1);
  lcd.print("W:"); lcd.print(wins);
  

  // --- WAIT FOR JOYSTICK PRESS TO BEGIN THE GAME ---
  while (digitalRead(zAxisPin) == HIGH) delay(10);

  //use user time to press joy stick for generating random seed for the gaeme
  randomSeed(micros());

  delay(50); // hardware debounce
  while (digitalRead(zAxisPin) == LOW) delay(10); 
  delay(50); // hardware debounce
  
  //game final score variable
  int finalScore = playSnake();
  
  lcd.clear();
  
  //win condition
  if (finalScore >= 61) { // 64 total spots - 3 starting length = 61 maximum score
    wins++;
    EEPROM.put(eeAddressWins, wins);
    lcd.setCursor(1, 0); lcd.print("YOU WIN!");

    if (finalScore > highScore)
    {
      highScore = finalScore;
      EEPROM.put(eeAddressHighScore, highScore); // Save new high score to EEPROM
    
      lcd.setCursor(1, 1); lcd.print("New HS: "); lcd.print(finalScore);
    }
    else
    {
      lcd.setCursor(1, 1); lcd.print("score: "); lcd.print(finalScore);
    }
    
    // Happy "Victory" Music
    tone(buzzerPin, NOTE_G5, 150); delay(150);
    tone(buzzerPin, NOTE_C6, 150); delay(150);
    tone(buzzerPin, NOTE_E6, 300); delay(300);
    tone(buzzerPin, NOTE_C6, 150); delay(150);
    tone(buzzerPin, NOTE_G6, 600);

    delay(5000); 

  }
  // --- 2. NEW HIGH SCORE CONDITION ---
  // (Only happens if they haven't won the game yet AND they beat the high score)
  else if (finalScore > highScore) {
    highScore = finalScore;
    EEPROM.put(eeAddressHighScore, highScore); // Save new high score to EEPROM
    
    lcd.setCursor(1, 0); lcd.print("NEW HIGH SCORE!");
    lcd.setCursor(1, 1); lcd.print("Score: "); lcd.print(finalScore);
    
    // Exciting "High Score" Music
    tone(buzzerPin, NOTE_C5, 150); delay(170);
    tone(buzzerPin, NOTE_E5, 150); delay(170);
    tone(buzzerPin, NOTE_G5, 150); delay(170);
    tone(buzzerPin, NOTE_C6, 600);

    delay(3000); 
  }
  //normal game over condition (not wining)
  else {
    lcd.setCursor(1, 0); lcd.print("GAME OVER!");
    lcd.setCursor(1, 1); lcd.print("Score: "); lcd.print(finalScore);
    
    // Sad "Game Over" Music
    tone(buzzerPin, NOTE_DS5, 300); delay(350);
    tone(buzzerPin, NOTE_D5,  300); delay(350);
    tone(buzzerPin, NOTE_CS5, 300); delay(350);
    tone(buzzerPin, NOTE_C5,  800); 

    delay(3000); 
  }
}



/****** the game function that runs the game when called and returns game score in int  *******/

int playSnake() {
  //two arrays declared so that each snake body part (max 64) has an X and Y cordinate, head is index 0
  byte snakeX[64];
  byte snakeY[64];

  //snakes start settings:
  int snakeLen = 3; 
  int score = 0;    
  int dir = DIR_RIGHT; 
  int pendingDir = DIR_RIGHT; 

  //snake start location
  snakeX[0] = 3; snakeY[0] = 1; 
  snakeX[1] = 2; snakeY[1] = 1; 
  snakeX[2] = 1; snakeY[2] = 1; 
  
  int appleX, appleY;
  bool needNewApple = true;

  // --- AUDIO STATE MACHINE VARIABLES ---
  byte chimeState = 0; 
  unsigned long lastChimeMillis = 0;
  const int chimeInterval = 40;  
  const int noteDuration = 20;   

  // --- DELTA RENDERING CACHE ---
  // Stores the current frame to prevent clearing the LCD and causing screen flicker
  byte oldScreen[16][2];
  for(int x=0; x<16; x++) { oldScreen[x][0] = 255; oldScreen[x][1] = 255; }
  lcd.clear();
  
  //variable to help us calculate the frames/ticks
  unsigned long lastGameTick = 0;

  // --- JOYSTICK DEBOUNCE VARIABLES ---
  //to not use delay()
  unsigned long debounceStartMillis = 0;
  int potentialDir = pendingDir; 

  while (true) {
    unsigned long currentMillis = millis();

    // --- NON-BLOCKING AUDIO CHIME ---
    // this code executes when the apple is eaten ( chimeState changes to 1 ) 

    if (chimeState > 0 && (currentMillis - lastChimeMillis >= chimeInterval)) {
      lastChimeMillis += chimeInterval; // Strict tempo
      if (chimeState == 1) { 
        tone(buzzerPin, NOTE_E7, noteDuration); 
        chimeState = 2; 
      }
      else if (chimeState == 2) { 
        tone(buzzerPin, NOTE_G7, noteDuration); 
        chimeState = 0; // Turn off chime state machine
      }
    }

    // --- JOYSTICK READING (NON-BLOCKING NOISE FILTER) ---
    int xVal = analogRead(xAxisPin);
    int yVal = analogRead(yAxisPin);
    
    // Default to the currently pending direction if the joystick is centered
    int rawDir = pendingDir; 
    if      (xVal < 50)  rawDir = DIR_LEFT;
    else if (xVal > 950) rawDir = DIR_RIGHT;
    else if (yVal < 50)  rawDir = DIR_UP;
    else if (yVal > 950) rawDir = DIR_DOWN;

    // 1. If the instantaneous reading is different from our potential direction, 
    // it means it's bouncing/moving. Reset the timer.
    if (rawDir != potentialDir) {
      potentialDir = rawDir;
      debounceStartMillis = currentMillis;
    }

    // 2. If the reading has been STABLE for at least 5ms, we process it!
    if ((currentMillis - debounceStartMillis >= 5) && (potentialDir != pendingDir)) {
      
      // Prevent the snake from reversing back into itself
      if      (potentialDir == DIR_LEFT  && dir != DIR_RIGHT) pendingDir = DIR_LEFT;
      else if (potentialDir == DIR_RIGHT && dir != DIR_LEFT)  pendingDir = DIR_RIGHT;
      else if (potentialDir == DIR_UP    && dir != DIR_DOWN)  pendingDir = DIR_UP;
      else if (potentialDir == DIR_DOWN  && dir != DIR_UP)    pendingDir = DIR_DOWN;
    }

    // --- GAME TICK ENGINE ---
    // made to make the game run at a given speed

    //you can change this to change the speed of the game
    int GAME_SPEED = max(125, (250 - (score * 4)));     // Milliseconds per game tick
    

    //checks when the next frame/tick of the game needs to execute
    if (currentMillis - lastGameTick >= GAME_SPEED) {
      lastGameTick = currentMillis;
      dir = pendingDir; 
      

      // Spawn Apple
      if (needNewApple) {
        bool valid = false;
        while (!valid) {
          appleX = random(0, 16);
          appleY = random(0, 4);
          valid = true;
          for (int i = 0; i < snakeLen; i++) {
            if (appleX == snakeX[i] && appleY == snakeY[i]) valid = false;
          }
        }
        needNewApple = false;
      }

      // Calculate Next Position
      int nextX = snakeX[0];
      int nextY = snakeY[0];
      if (dir == DIR_UP)         nextY--;
      else if (dir == DIR_DOWN)  nextY++;
      else if (dir == DIR_LEFT)  nextX--;
      else if (dir == DIR_RIGHT) nextX++;

      // Check Wall Collisions
      if (nextX < 0 || nextX > 15 || nextY < 0 || nextY > 3) break; 
      
      // Check Self Collisions
      bool selfCollision = false;
      for (int i = 0; i < snakeLen - 1; i++) {
        if (nextX == snakeX[i] && nextY == snakeY[i]) selfCollision = true;
      }
      if (selfCollision) break; 

      // --- EAT APPLE ---
      if (nextX == appleX && nextY == appleY) {
        score++;
        needNewApple = true;
        if (snakeLen < 64)
        { 
          snakeLen++;
        }

        //reset game
        if(snakeLen == 64)
        {
          // Add a quick "Level Up" / Prestige sound and message
          lcd.clear();
          lcd.setCursor(1, 0); lcd.print("BOARD CLEARED!");
          lcd.setCursor(1, 1); lcd.print("KEEP GOING!!");
          
          tone(buzzerPin, NOTE_C6, 200); delay(200);
          tone(buzzerPin, NOTE_E6, 200); delay(200);
          tone(buzzerPin, NOTE_G6, 400); delay(3000); // 3 seconds pause


          // 1. Reset the snake back to its starting state
          snakeLen = 3;
          snakeX[0] = 3; snakeY[0] = 1;
          snakeX[1] = 2; snakeY[1] = 1;
          snakeX[2] = 1; snakeY[2] = 1;
          dir = DIR_RIGHT;
          pendingDir = DIR_RIGHT;
          
          // 2. Clear the screen cache so the rendering engine redraws the empty board
          for(int x=0; x<16; x++) { oldScreen[x][0] = 255; oldScreen[x][1] = 255; }
          lcd.clear();
          
          // 3. Trigger a new apple for the new board
          needNewApple = true; 
          
          // 4. Skip the movement/rendering code below for this tick, 
          //    and start the next tick fresh!
          continue; 
        }
        
        // Start the state machine chime
        tone(buzzerPin, NOTE_C7, noteDuration);
        chimeState = 1;               
        lastChimeMillis = currentMillis;   
      }

      // Move Snake Array (each body part moves to the preceding body part possition)
      for (int i = snakeLen - 1; i > 0; i--) {
        snakeX[i] = snakeX[i - 1];
        snakeY[i] = snakeY[i - 1];
      }
      snakeX[0] = nextX;
      snakeY[0] = nextY;

      // --- 16x4 RENDERING ENGINE ---
      // Map the logical 16x4 grid onto the physical 16x2 screen
      //there is a 16x2 actual screen and the 16x4 "virtual" screen so that
      //we can bypass screen 16x2 limit and play on a "16x4" screen
      
      //first the engine takes the actual positions of the apple and snakes head,body
      byte gridState[16][4] = {CELL_EMPTY}; 
      gridState[appleX][appleY] = CELL_APPLE;
      for (int i = 1; i < snakeLen; i++) gridState[snakeX[i]][snakeY[i]] = CELL_BODY;
      gridState[snakeX[0]][snakeY[0]] = CELL_HEAD; 
      
      //loop thru each actual screen position (16x2)
      for (int lcdX = 0; lcdX < 16; lcdX++) {
        for (int lcdY = 0; lcdY < 2; lcdY++) {
          //and split it into two "virtual" positions top and bot
          int topY = lcdY * 2;       
          int botY = (lcdY * 2) + 1; 
          
          int topState = gridState[lcdX][topY];
          int botState = gridState[lcdX][botY];
          byte charToDraw = 255; 

          // If the head is in this block, dynamically generate Custom Character #7
          //basicly it generates a custom screen char on the spot where the head of the snake is located on the screen
          //the head char is generated on the spot because of the screen ram limit of only 8 chars (so we are bypassing the limit like that)
          if (topState == CELL_HEAD || botState == CELL_HEAD) {
            //the custome char
            byte dynamicHead[8];
            //genrate top half of the char
            for(int i=0; i<4; i++) {
              if (topState == CELL_HEAD)       dynamicHead[i] = head_half[i];
              else if (topState == CELL_BODY)  dynamicHead[i] = body_half[i];
              else if (topState == CELL_APPLE) dynamicHead[i] = apple_half[i];
              else                             dynamicHead[i] = empty_half[i];
            }
            //generate bot half of the char
            for(int i=0; i<4; i++) {
              if (botState == CELL_HEAD)       dynamicHead[i+4] = head_half[i];
              else if (botState == CELL_BODY)  dynamicHead[i+4] = body_half[i];
              else if (botState == CELL_APPLE) dynamicHead[i+4] = apple_half[i];
              else                             dynamicHead[i+4] = empty_half[i];
            }

            //now when we chose top and bot halfs of the char its time to generate it
            lcd.createChar(7, dynamicHead); 
            charToDraw = 7;
          } 
          // Otherwise, use the pre-loaded dictionary of block combinations
          else if (topState == CELL_BODY  && botState == CELL_EMPTY) charToDraw = 0;
          else if (topState == CELL_EMPTY && botState == CELL_BODY)  charToDraw = 1;
          else if (topState == CELL_BODY  && botState == CELL_BODY)  charToDraw = 2;
          else if (topState == CELL_APPLE && botState == CELL_EMPTY) charToDraw = 3;
          else if (topState == CELL_EMPTY && botState == CELL_APPLE) charToDraw = 4;
          else if (topState == CELL_APPLE && botState == CELL_BODY)  charToDraw = 5;
          else if (topState == CELL_BODY  && botState == CELL_APPLE) charToDraw = 6;

          // --- DELTA RENDERING ---
          // Only draw if the character has actually changed since the last tick/frame
          if (oldScreen[lcdX][lcdY] != charToDraw) {
            lcd.setCursor(lcdX, lcdY);
            if (charToDraw == 255) lcd.print(" ");
            else lcd.write(charToDraw);
            oldScreen[lcdX][lcdY] = charToDraw; // Update memory cache
          }
        }
      }
    } 
  }
  return score;
}