// LED pins
int pin1 = 2;
int pin2 = 3;
int pin3 = 4;
int pin4 = 5;
int pin5 = 6;
int pin6 = 7;
int pin7 = 8;
int pin8 = 9;
int pin9 = 10;
int pin10 = 11;
int pin11 = A5;
int pin12 = A4;
int pin13 = A3;
int pin14 = A2;
int pin15 = A1;
int pin16 = 13;

int cols[] = {pin9, pin14, pin8, pin12, pin1,  pin7, pin2,pin5};
int rows[] = {pin16, pin15,pin11, pin6, pin10, pin4,  pin3, pin13};

int scanRow = 0;
int count = 0;


// potentiometer configurations
int potPin = A0;
int potVal;
int prevPotVal;
long potReadTime;

// button configurations
int shootButton = 12;
int readingBT;
long ReadingBTTime = 0;
long debounce = 200;

//bullet configurations
long bulletUpdateTime;
long bulletDebounceTime = 60;

//asteriods configurations
long asteriodsGeneratedTime = 0;
long asteriodsUpdateTime;
long asteriodsUpdateDebounce = 800;
long asteriodsDebounceTime = 2000;
long asteriodsCount = 0;

//player info
long score = 0;
int playerPos = 4;

//game state info
int gameState = 0;//0 for splash screen,
                  //1 for in the play
                  //2 for game over

long startScreenCount = 0; 
long startScreenDuration = 800;

//animation
byte explodeCount = 0;
byte explodePattern = 0x0;
long animationTime = 0;
byte explodeState = 0;

//display pattern
byte startScreen[8] = {0x00,0x18,0x18,0x18,0x3C,0x7E,0x18,0x00};
byte player[8] ={0x0,0x0,0x00,0x0,0x0,0x0,0x08,0x1C};
byte explode[8] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
byte asteriods[8] = {0x0,0x0,0x0,0x00,0x0,0x0,0x0,0x0};
byte initBoard[8] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
byte scoreBoard[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
byte tenDigits[10][8] = {{0xF0,0x90,0x90,0x90,0x90,0x90,0xF0,0x00},//0
                         {0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x00},//1
                         {0xF0,0x10,0x10,0xF0,0x80,0x80,0xF0,0x00},//2
                         {0xF0,0x10,0x10,0xF0,0x10,0x10,0xF0,0x00},//3
                         {0x90,0x90,0x90,0xF0,0x10,0x10,0x10,0x00},//4
                         {0xF0,0x80,0x80,0xF0,0x10,0x10,0xF0,0x00},//5
                         {0xF0,0x80,0x80,0xF0,0x90,0x90,0xF0,0x00},//6
                         {0xF0,0x10,0x10,0x10,0x10,0x10,0x10,0x00},//7
                         {0xF0,0x90,0x90,0xF0,0x90,0x90,0xF0,0x00},//8
                         {0xF0,0x90,0x90,0xF0,0x10,0x10,0xF0,0x00}};//9

byte oneDigits[10][8] = {{0x00,0x07,0x05,0x05,0x05,0x07,0x00,0x00},//0
                         {0x00,0x01,0x01,0x01,0x01,0x01,0x00,0x00},//1
                         {0x00,0x07,0x01,0x07,0x04,0x07,0x00,0x00},//2
                         {0x00,0x07,0x01,0x07,0x01,0x07,0x00,0x00},//3
                         {0x00,0x05,0x05,0x07,0x01,0x01,0x00,0x00},//4
                         {0x00,0x07,0x04,0x07,0x01,0x07,0x00,0x00},//5
                         {0x00,0x07,0x04,0x07,0x05,0x07,0x00,0x00},//6
                         {0x00,0x07,0x01,0x01,0x01,0x01,0x00,0x00},//7
                         {0x00,0x07,0x05,0x07,0x05,0x07,0x00,0x00},//8
                         {0x00,0x07,0x05,0x07,0x01,0x07,0x00,0x00}}; //9
  
                         
byte all[8] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};


void setup() {
  Serial.begin(9600);
  // assume all are HIGH at the beginning
  for(int i = 0; i< 8; i++){
    pinMode(cols[i], OUTPUT);
    digitalWrite(cols[i],HIGH);
    pinMode(rows[i], OUTPUT);
    digitalWrite(rows[i],HIGH);
  }
  pinMode(shootButton, INPUT);
  
  potVal = analogRead(potPin);
  prevPotVal = potVal;
  
}

void loop() {
  switch(gameState){
    //start state
    case 0:
      start();
      render(startScreen);
      if(startScreenCount > startScreenDuration){
        inverseSplashScreen();
        startScreenCount = 0;
      }
      startScreenCount++;
      break;
      
    //in game
    case 1:
      asteriodGeneration();
      asteriodMoveDown();
      checkPlayerStatus();
      movePlayer();
      shoot();
      bulletMove();
      
      if(asteriodsCount % 20 == 0 && asteriodsUpdateDebounce > 90){
        asteriodsUpdateDebounce--;
      }
      if(asteriodsCount % 20 == 0 && asteriodsDebounceTime > 1000){
        asteriodsDebounceTime-=2;
      }
      
      asteriodsCount++;
      render(asteriods);
      render(player);
      break;

    //explosion animation
    case 2:
      explodeAnimation();
      render(explode);
      break;
      
    //display score
    case 3:
      start();
      displayScore();
      break;
  }
}



////////////////////////ANIMATION//////////////////////////

//plays the animation of the explosion
void explodeAnimation(){
  //first explode: turn bits into 1
  if(millis() - animationTime > 100 && explodeState == 0){
    if(explodeCount > 3){
      explodeCount = 0;
      explodePattern = 0x0;
      explodeState = 1;
    }else{
      explodePattern = explodePattern | (0x80>>(3 - explodeCount));
      explodePattern = explodePattern | (0x01<<(3 - explodeCount));
      for(int i = 0; i < explodeCount+1; i++){
        explode[3-i] = explodePattern;
        explode[4+i] = explodePattern;
      }
      explodeCount++;
      animationTime = millis();
    }
  }
  //second explode: reverse bits back to 0
  else if(millis() - animationTime > 150 && explodeState == 1){
    if(explodeCount > 3){
      explodeCount = 0;
      explodePattern = 0xFF;
      explodeState = 2;
      gameState = 3;
    }else{
      byte temp1 = (0xE0<<explodeCount);
      byte temp2 = (0x07>>explodeCount);
      explodePattern = temp1 | temp2;
      for(int i = 0; i < explodeCount+1; i++){
        explode[3-i] = explodePattern;
        explode[4+i] = explodePattern;
      }
      explodeCount++;
      animationTime = millis();
    }
  }
}

//flips every bits in splash screen
void inverseSplashScreen(){
  for(int i = 0; i < 8; i++){
    startScreen[i] = ~startScreen[i];
  }
}


////////////////////////GAME LOGICS//////////////////////////
//when the player presses shooting button, it will change the game state to
//in game and game will start. It will initialize all the in game parameters.
void start(){
  if(digitalRead(shootButton) == HIGH && millis() - ReadingBTTime > debounce){
    byte initPlayer[8] ={0x0,0x0,0x0,0x0,0x0,0x0,0x08,0x1C};
    memcpy(player, initPlayer, sizeof(initPlayer));
    memcpy(asteriods, initBoard, sizeof(initBoard));
    memcpy(scoreBoard, initBoard, sizeof(initBoard ));
    score = 0;
    playerPos = 4;
    gameState = 1;
    explodeState = 0;
    explodePattern = 0x0;
    asteriodsUpdateDebounce = 800;
    asteriodsDebounceTime = 2000;
    ReadingBTTime = millis();
  }
}

//When the player use the potentiometer, the spaceship will move horizontally
void movePlayer(){
  potVal = analogRead(potPin);
  if(potVal > prevPotVal && potVal - prevPotVal  > 20 &&  playerPos>1){
    player[7] = (player[7] << 1);
    player[6] = (player[6] << 1);
    playerPos--;
    prevPotVal = potVal;
    potReadTime = millis();
  }
  else if(prevPotVal > potVal && prevPotVal - potVal > 20 &&  playerPos<6){
    player[7] = (player[7] >> 1);
    player[6] = (player[6] >> 1);
    playerPos++;
    prevPotVal = potVal;
    potReadTime = millis();
  } 
}

//when the player presses shoot button, the spaceship shoots a bullet.
void shoot(){
  readingBT = digitalRead(shootButton);
  if(readingBT == HIGH && millis() - ReadingBTTime > debounce){ 
    createBullet();
    ReadingBTTime = millis();
  }
}

//creates a bullet when the player shoot.
void createBullet(){
  byte bulletPos = 0x80>>playerPos;
  player[5] = bulletPos;
  hitAsteriods();
}

//move bullets in a certain amount of time.
void bulletMove(){
  if(millis() - bulletUpdateTime > bulletDebounceTime){
    for(int i = 0; i < 5; i++){
      player[i] = player[i+1];
      player[i+1] = 0x0;
    }
    hitAsteriods();
    bulletUpdateTime = millis();
  }
}

//if the bullet hits asteroids, destroy the asteroid.
void destroyAsteriods(int row){
  //destory asteriod
  for(int i = 0; i < 8; i++){
    byte asteriodBit = (asteriods[row]>>(7-i) ) & 0x01;
    byte bulletBit = (player[row]>>(7-i)) & 0x01;
    if(asteriodBit & bulletBit == 1){
      //Serial.print("destory");
      byte mask = ~(0x80>>i);
      asteriods[row] = asteriods[row] & mask;
    }
  }
  //cancel out bullet
  player[row] = 0x0;
  score++;
}

//check if bullets hit asteroids.
bool hitAsteriods(){
  for(int i = 0; i < 6; i++){
    if(asteriods[i] & player[i]){
      //Serial.print("hit ");
      destroyAsteriods(i);
    }
    clearPassingAsteriod(i);
  }
}

//because concurrency issues some asteroids will pass the bullet 
//and wont be destroyed, the function will clear the passing asteroids.
void clearPassingAsteriod(int row){
  int col = 0;
  for(int i = 0; i < 8; i++){
    byte bulletBit = (player[row]>>(7-i)) & 0x01;
    if(bulletBit == 1) {
      col = i;
      break;
    }
  }
  byte asteriodBit = (asteriods[row+1]>>(7-col)) & 0x01;
  if(asteriodBit == 1){
    Serial.print("missing ");
    byte mask = ~(0x80>>col);
    asteriods[row+1] = asteriods[row+1] & mask;
    player[row] = 0x0;
    score++;
  }
}

//procedurally generate different types of asteroids in different positions.
void asteriodGeneration(){
  if(millis() - asteriodsGeneratedTime > asteriodsDebounceTime){  
    asteriodsGeneratedTime = millis();
    long shift = 0;
    long randomType = random(0,3);
    byte asteriodsPart = 0x0;
    byte asteriodsPart2 = 0x0;
    switch(randomType){
      case 0:      
        shift = random(1, 6);
        asteriodsPart = 0xC0 >> shift;
        asteriods[0] = asteriods[0] | asteriodsPart;
        asteriods[1] = asteriods[1] | asteriodsPart;
        break;
      case 1:
        shift = random(1, 7);      
        asteriodsPart = 0x80 >> shift;
        asteriods[0] = asteriods[0] | asteriodsPart;
        break;
      case 2:      
        shift = random(1, 6);
        asteriodsPart = 0x80 >> shift;
        asteriodsPart2 = 0xC0 >> shift;
        asteriods[0] = asteriods[0] | asteriodsPart;
        asteriods[1] = asteriods[1] | asteriodsPart2;
        break;
    }
  }
}

//move asteroids down in a certain amount of time.
void asteriodMoveDown(){
  if(millis() - asteriodsUpdateTime > asteriodsUpdateDebounce){
    for(int i = 0; i < 7; i++){
      asteriods[7-i] = asteriods[6-i];
    }
    asteriods[0] = 0x0;
    asteriodsUpdateTime = millis();
  }
}

//checks if the spaceship got hit. If the spaceship got hit, 
//it changes the game state to play explosion animation.
void checkPlayerStatus(){
  for(int i = 0; i < 2; i++){
    if(asteriods[7-i] & player[7-i]){
      //Serial.print("PlayerGetHit");
      gameState = 2;
    }
  } 
}

////////////////////////RENDERING//////////////////////////

//when the game state changes to display score, 
//shows the score player got in the game.
void displayScore(){
  byte ten[8];
  byte one[8];
  int tenDigit;
  int oneDigit;
  if(score < 100){
    tenDigit = score / 10;
    oneDigit = score % 10;
    memcpy(ten, tenDigits[tenDigit], sizeof(ten));
    memcpy(one, oneDigits[oneDigit], sizeof(one));
  }
  else if(score >=100){
    memcpy(ten, tenDigits[9], sizeof(ten));
    memcpy(one, oneDigits[9], sizeof(one));
  }
  for(int i = 0; i<8; i++){
    scoreBoard[i] = ten[i] | one[i];
  }
  render(scoreBoard);
}

//based on 2d bytes array, controls each LED in the matrix on and off.
void render(byte contents[]){
  byte current_content;
  
  for(int i = 0; i<8; i++){
    digitalWrite(cols[i], HIGH);
    digitalWrite(rows[i], HIGH);
  }
  current_content = contents[scanRow];
  for(int col = 0; col < 8; col++){
    digitalWrite(cols[7-col],current_content & 0x01);
    current_content = current_content>>1;   
  }
  digitalWrite(rows[scanRow], LOW);
  delay(1);
  digitalWrite(rows[scanRow], HIGH);

  if(scanRow < 8){scanRow++;}
  else{scanRow = 0;}
}




