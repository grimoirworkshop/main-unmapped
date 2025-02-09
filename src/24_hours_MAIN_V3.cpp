//#include <mp3TF.h>
#include <Arduino.h>
#include "DFRobotDFPlayerMini.h"


#include <SoftwareSerial.h>
#include <stl.h>
#include <input.h>
#include <quest.h>
//#include <Wire.h>


Input input;

/*
Used Pins
relays:
  22 - bench 
  23-27 -firelights 
  31 trap
  32 wheelcover
  33 topwheelcover
  7 winker
  28 back car lights
  opening cave
  closing cave
  torches
  spotlight


serials:
  8,9    jungle
  10,11  transit
  12, 13 monkey  

Buttons
  29 START
  open cave 
  closecave
*/        

// Buttons vars
Button startGameButton(29);


int quest_music = 0; // 0 - 1 quest, 100 - 2 quest, 200 -3 quest


// Relays vars
Relay carBench(22, true); 
Relay fireLights[5] = {{23, false}, {24, false}, {25, false}, {26, false}, {27, false}}; 
Relay trapRelay(31, false); //--
Relay wheelCover(32, true);
Relay topWheelCover(33, true); //--
Relay frontCarLightsVisher(7, false);
Relay backCarLights(28, false);
Relay openTheCave();
Relay closeTheCave();
Relay torch1();
Relay torch2();
Relay torch3();
Relay spotLight();



// General Vars
int totalStages = 6;
unsigned long transitDuration = 5*60*1000L; //300; // miliseconds to print transit section is over
unsigned long jungleDuration = 30*60*1000L; //1800; // miliseconds from the game start to the end of jungle section
unsigned long caveDuration = 35*60*1000L; // miliseconds from the game start to the end of cave section
unsigned long tribeDuration = 65*60*1000L; // miliseconds from the game start to the end of tribe section
unsigned long omegaDuration = 70*60*1000L; // miliseconds from the game start to the end of omega section
unsigned long borderDuration = 85*60*1000L; // miliseconds from the game start to the end of border section
//unsigned long  = 85*60*1000L; // miliseconds from the game start to the end of border section
unsigned long gameStageDuration[6] = {transitDuration, jungleDuration, caveDuration, tribeDuration, omegaDuration, borderDuration};
long shootingDelay = 125; //60; //miliseconds until shooting shounds
long benchDelay = shootingDelay + 21; //miliseconds from shooting to bench down
long visherDelay = benchDelay + 180; // seconds from accident to car battery out
long wheelDelay = transitDuration + 10; // miliseconds from jungle started to show the wheel
long wheelStopDelay = wheelDelay + 60 ; // seconds to close wheel
long totalGames = 3;
long gamesStartTime[3] = {0,0,0}; // start time of each game
long gamesProgress[3] = {0,0,0}; // current stage of each game
long currentGame = totalGames-1;


bool transitStage = true;
bool fired = false;
bool accident = false;
bool initGame = true;
bool wheelQuest = false;
bool jungleStage = true;
bool disableBench = false;
bool wheelCloseQuest = false;
bool visherStops = false;
bool startMonkeySound = true;
char runQuest = ' ';
char gmaeStages[6][10] = {"TRANSIT","JUNGLE", "CAVE", "TRIBE", "OMEGA", "BORDER"};
bool startTheEnding = false;
bool endAndReset = false;
bool caveOpens = false;

void serialSender(char[]);
void mainSerialReader();
void printDetail(uint8_t, int);
void stopMonkeySound(int);
void setMonkeySound(int);
void stopJungleSound(int);
void setJungleSound(int);
void stopTransitSound(int);
void setTransitSound(int);
void printGameProgress(int, char[10]);
void monitorGameProgress();
void openWheel();
void resetFun();
void startGame(int sound_track);
void transitFunc();
void inputCheck() ;

void remoteCheck();
void caveRoutine();
void endGameRoutine();


// Sounds vars
SoftwareSerial monkeySerial(12, 13);
SoftwareSerial transitSerial(10, 11); 
SoftwareSerial jungleSerial(8, 9);
//mp3TF transitMp3 = mp3TF();
//mp3TF jungleMp3 = mp3TF();
DFRobotDFPlayerMini transitMp3;
DFRobotDFPlayerMini jungleMp3;
DFRobotDFPlayerMini monkeyMp3;
void printDetail(uint8_t type, int value);




void setup() {
  frontCarLightsVisher.turnOff();
  carBench.turnOff();
  trapRelay.turnOn();
  wheelCover.turnOn();
  topWheelCover.turnOff();
  backCarLights.turnOn();
  Serial.begin(9600);
  transitSerial.begin(9600);
  jungleSerial.begin(9600);
  monkeySerial.begin(9600);
  if (!transitMp3.begin(transitSerial)) {
    Serial.println(F("Initializing transitMp3:"));
  }
  if (!jungleMp3.begin(jungleSerial)) {
    Serial.println(F("Initializing jungleMp3:"));
  }
  if (!monkeyMp3.begin(monkeySerial)) {
    Serial.println(F("Initializing monkeyMp3:"));
  }
  transitMp3.volume(25);
  jungleMp3.volume(25);
  monkeyMp3.volume(25);
  Serial2.begin(9600);
  Serial3.begin(9600);
  resetFun();
}
void loop() {
//  delay(100);
//  backToNormalButton.debug("backToNormalButton");
  inputCheck();
  mainSerialReader();
  transitFunc();
  monitorGameProgress();
}






// ------------ FUNCTIONS ----------
void resetFun(){
  Serial.println("Initiating setup mode");
  // ---- stop Mp3s
  stopTransitSound(-1);
  stopJungleSound(-1);
  stopMonkeySound(-1);
  // ---- init vars


  for (int i=0; i < 5; i++){
      fireLights[i].turnOff();
  }
  startMonkeySound = true;
  visherStops = false;
  transitStage = true;
  wheelQuest = false;
  initGame = true;
  fired = false;
  accident = false;
  jungleStage = true;
  disableBench = false;
  startTheEnding = false;
  endAndReset = false;
  caveOpens = false;
  frontCarLightsVisher.turnOff();
  carBench.turnOff();
  trapRelay.turnOn();
  wheelCover.turnOn();
  topWheelCover.turnOff();
  backCarLights.turnOn();
  //Selecting an audio track, 1 - 00XX 2 - 01XX.
  Serial.println("Selecting an audio track, 1 - 00XX 2 - 01XX.");
  input.check();
  char buf = input.read();
  while ((buf != '1') && (buf != '2')&& (buf != '3')) {
    input.check();
    buf = input.read();
  }
    switch (buf)
    {
    case '1':
        Serial.println("Adult Version chosen");
        quest_music = 0;
        break;    
    case '2':
        Serial.println("Kid Version chosen");
        quest_music = 100;
        break;
    case '3':
        Serial.println("English Version chosen");
        quest_music = 200;
        break;
    
    default:   
        break;
    }

 
  
  // ---- wait to game to start
  Serial.println("Press s to start, at least 5 sec before play.");
  input.check();
  while (input.read() != 's') {
    input.check();
    monitorGameProgress();
  }
  
  Serial.println("You can start the game in 5 seconds.\n");
  
  // ---- select sound type
  while (initGame == true) {
    monitorGameProgress();
    runQuest = input.read();
    if (runQuest == 'm'){
      startGame(1);
      Serial.println("playing man record! ===---");
    }
  
    if (runQuest == 'w'){
      startGame(2);
      Serial.println("playing woman record! ===---");
    }
    input.check();
  }
}




void startGame(int sound_track){
  Serial.println("Press button to start game");
  bool startGamePressed = false;
  while (startGamePressed == false) {
    startGameButton.check();
    if (startGameButton.isPressed()) {
      startGamePressed = true;
    }
  }
  if (currentGame == totalGames-1){
    currentGame = 0;
  } else {
    currentGame++;
  }
  gamesStartTime[currentGame] = millis();
  gamesProgress[currentGame] = 0;
  setTransitSound(sound_track);
  initGame = false;
  Serial.print("---=== GAME ");
  Serial.print(currentGame);
  Serial.print(" STARTED!! - ");
}


void transitFunc(){
  if ((fired == false) && (gamesStartTime[currentGame] + shootingDelay*1000 <= millis())){
    Serial.println("shooting has been started");

    for (int i=0; i < 5; i++){
      fireLights[i].turnOn();
      delay(2000);
    }
    fired = true;
  }
  if ((accident == false) && (gamesStartTime[currentGame] + benchDelay*1000 <= millis())) {
    Serial.println("Accident occurs");
    if (disableBench == false) {
      frontCarLightsVisher.turnOn();
      setJungleSound(1);
      carBench.turnOn(); 
      backCarLights.turnOff();
    }
    accident = true;
  }
  if ((wheelQuest == false) && (gamesStartTime[currentGame] + wheelDelay*1000 <= millis())) {
    Serial.println("---- NEED TO BLUR LIGHTS ----");
    Serial.println("---- NEED TO OPEN WHEEL ----- (PRESS g) ------");
    wheelQuest = true;
  }
//  if ((wheelCloseQuest == false) && (gamesStartTime[currentGame] + wheelDelay*1000 <= millis())) {
//    Serial.println("wheel closed");
//    topWheelCover.turnOff();
//    wheelCloseQuest = true;
//  
//  }
  if ((visherStops == false) && (gamesStartTime[currentGame] + visherDelay*1000 <= millis())) {
    visherStops = true;
//    jungleMp3.volume(20);
    frontCarLightsVisher.turnOff();
    delay(2000);
    frontCarLightsVisher.turnOn();
    delay(3000);
    frontCarLightsVisher.turnOff();
    delay(2000);
    frontCarLightsVisher.turnOn();
    delay(3000);
    frontCarLightsVisher.turnOff();
    delay(2000);
    frontCarLightsVisher.turnOn();
    delay(3000);
    frontCarLightsVisher.turnOff();
    delay(2000);
  }    
}


void openWheel(){
  wheelCover.turnOff();
  topWheelCover.turnOn();
}


void monitorGameProgress(){
  for (int i=0; i < totalGames; i++) {
    if ((gamesStartTime[i] == 0) || (gamesProgress[i] >= totalStages)) {
      continue;
    }
    if (gamesStartTime[i] + gameStageDuration[gamesProgress[i]] <= millis()){
      printGameProgress(i, gmaeStages[gamesProgress[i]]);
      gamesProgress[i]++;
    }
  }
}








void printGameProgress(int game_num, char stage[10]){
  Serial.print("---=== GAME ");
  Serial.print(game_num);
  Serial.print(" ");
  Serial.print(stage);
  Serial.println(" STAGE FINISHED ===---");
}




// ------------ SOUND SECTION --------------


void setTransitSound(int track) {
  Serial.print("play transit sound ");
  Serial.println(track + quest_music);
  transitMp3.playMp3Folder(track + quest_music);
}
void stopTransitSound(int track) {
  Serial.println("stop transit track");
  transitMp3.stop();
}


void setJungleSound(int track) {
  Serial.print("play Jungle sound ");
  Serial.println(track + quest_music);
  jungleMp3.playMp3Folder(track + quest_music);
}
void stopJungleSound(int track) {
  Serial.println("stop Jungle track");
  jungleMp3.stop();
}


void setMonkeySound(int track) {
  Serial.print("play monkey sound ");
  Serial.println(track + quest_music);
  monkeyMp3.playMp3Folder(track + quest_music);
}
void stopMonkeySound(int track) {
  Serial.println("stop monkey track");
  monkeyMp3.stop();
}


void inputCheck() {
  input.check();
  runQuest = input.read();
  if (runQuest == 'f') {
    startTheEnding = true;
  }
  if (runQuest == 'e') {
    resetFun();
  }
  if ((runQuest == 'b') && (disableBench == false)) {
    carBench.turnOn();
  }
  if (runQuest == 'l') {
    setJungleSound(1);
  }
  if (runQuest == 't') {
    trapRelay.turnOff();
  }
  if (runQuest == 'g') {
    openWheel();
  }
  if (runQuest == 'p') {
    setMonkeySound(2);
    delay(10000);
    for (int i=0; i < 7; i++) {
      Serial.println("car lights turn on");
      frontCarLightsVisher.turnOn();
      delay(2000);
      Serial.println("car lights turn off");
      frontCarLightsVisher.turnOff(); 
      delay(1000);
    }
  }
  if (runQuest == 'k') {
    if (startMonkeySound == true) {
      setMonkeySound(1);  
      startMonkeySound = false;
    } else {
      stopMonkeySound(-1);
      startMonkeySound = true;
      
    }
    printDetail(monkeyMp3.readType(), monkeyMp3.read());
  }
  if (runQuest == 'j') {
    setJungleSound(1);
    printDetail(jungleMp3.readType(), jungleMp3.read());
  }
  if (runQuest == 'd') {
    disableBench = true;
  }
}


// ----------- SOUND PRINT --------------
void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}


// ---------- ARDUINO COMMUNICATION SECTION ------------


void mainSerialReader(){
    while (Serial3.available()){
      char c = Serial3.read();
      if (c == '|'){
        Serial.println();
      } else {
          Serial.print(c);
      }
      delay(10);
    } 
}

void serialSender(char data[]){
  Serial.print("sending to second arduino ");
  Serial.println(data);
  Serial3.write(data);
}
void remoteCheck(){

}
void caveRoutine(){

}
void endGameRoutine(){

}
