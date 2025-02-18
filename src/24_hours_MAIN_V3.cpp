//#include <mp3TF.h>
#include <Arduino.h>
//#include "DFPlayerMini_Fast.h"
#include "DFPlayerMini_Fast.h"

#include <SoftwareSerial.h>
#include <stl.h>
#include <input.h>
#include <quest.h>
//#include <Wire.h>


Input input;

/*#include <input.h>
 outputs
  23-27 -firelights 
  33 wheelcover
  7 winker
  28 back car lights
  42 car door
  32 floor 
  38 rocks
  34 35 37 torches
  36 spotlight
  39 fog
  40 finlight
  41 the end


serials:
  8,9    jungle
  10,11  transit
  12, 13 backup  
  4 5 cave
  2 3 animals
  14 15 badguys
  16 17 caveinside
  18 19 end

Buttons
  29 START
  43 statue
  44 cave remote
  45 tree button

*/        

// Buttons vars
Button startGameButton(29);
Button statue(43);
Button CaveRemote(44);
Button treeButton(45);

int quest_music = 0; // 0 - 1 quest, 100 - 2 quest, 200 -3 quest


// Relays vars
Relay carBench(22, true); 
Relay fireLights[5] = {{23, false}, {24, false}, {25, false}, {26, false}, {27, false}}; 
Relay trapRelay(31, false); //--
Relay floorShake(32, false);
Relay wheelLock(33, false); //--
Relay frontCarLightsVisher(7, false);
Relay backCarLights(28, false);
Relay closeTheCave(30, false);           //dont remember which pin
Relay carDoor(42, false);            //dont remember which pin
Relay torch1(37, false);
Relay torch2(35, false);
Relay torch3(34, false);
Relay spotLight(36, false);
Relay finLight(40, false);
Relay theEndLight(41, false);
Relay finalFogMachine(39, false);
Relay rockShelf(38, false);



// General Vars
unsigned long transitDuration = 5*60*1000L; //300; // miliseconds to print transit section is over
unsigned long delayBeforeBadGuys = 35000;
unsigned long delayTorch1 = 11400;
unsigned long delayTorch2 = 13400;
unsigned long delayTorch3 = 15600;
unsigned long delaySpot = 19200;
unsigned long delayFogOff = 21000;
unsigned long delayAllOff = 30800;
unsigned long delayKidsEnd = 47000;
unsigned long delayFloor = 89000;
unsigned long delayRocks = 105000;
unsigned long delayBigLight = 118000;

unsigned long gameStartTime;
unsigned long caveOpeningTime;
unsigned long finalSeqStartTime;
unsigned long currentTime;

long shootingDelay = 132; //60; //miliseconds until shooting shounds
long benchDelay = shootingDelay + 17; //miliseconds from shooting to bench down
long visherDelay = benchDelay + 100; // seconds from accident to car battery out



bool transitStage = true;
bool fired = false;
bool accident = false;
bool initGame = true;
bool jungleStage = true;
bool disableBench = false;
bool wheelCloseQuest = false;
bool visherStops = false;
bool startBackupSound = true;
char runQuest = ' ';
bool startTheEnding = false;
bool endAndReset = false;

bool caveOpened = false;
bool caveClosed = false;
bool badGuysStarted = false;
bool kidsGame = false;
bool runningTheEnd = false;
bool finmMusicStarted = false;
bool torch1Lit = false;
bool torch2Lit = false;
bool torch3Lit = false;
bool spotLit = false;
bool fogMachineDone = false;
bool rocksFall = false;
bool floorShaking = false;
bool carDoorOpened = false;
bool wheelOpened = false;
bool noBadGuys = false;
bool treeButtonPressed = false;

void printDetail(uint8_t, int);
void stopBackupSound();
void playBackupSound();
void stopJungleSound();
void playJungleSound();
void stopTransitSound();
void playTransitSound();
void stopCaveSound();
void playCaveSound();
void stopCaveInsidesSound();
void playCaveInsidesSound();
void stopBadGuysSound();
void playBadGuysSound();
void playAnimalsSound();
void stopFinalSound();
void playFinalSound();
void openWheel();
void resetFun();
void startGame();
void transitFunc();
void inputCheck() ;

void statueCheck();
void remoteCheck();
void caveRoutine();
void endGameRoutine();
void treeButtonCheck();


// Sounds vars
SoftwareSerial backupSerial(12, 13);
SoftwareSerial transitSerial(11, 46); 
SoftwareSerial jungleSerial(8, 48);
SoftwareSerial nearCaveSerial(5, 4);
//SoftwareSerial caveInsidesSerial(16, 17);
//SoftwareSerial badGuysSerial(14, 15);
//SoftwareSerial finalSerial(18, 19);
SoftwareSerial animalsSerial(2, 3);

DFPlayerMini_Fast transitMp3;
DFPlayerMini_Fast jungleMp3;
DFPlayerMini_Fast backupMp3;
DFPlayerMini_Fast nearCaveMp3;
DFPlayerMini_Fast caveInsidesMp3;
DFPlayerMini_Fast badGuysMp3;
DFPlayerMini_Fast finalMp3;
DFPlayerMini_Fast animalsMp3;
void printDetail(uint8_t type, int value);




void setup() {
  frontCarLightsVisher.turnOff();
  carBench.turnOff();
  trapRelay.turnOn();
  wheelLock.turnOff();
  carBench.turnOff();
  trapRelay.turnOn();
  wheelLock.turnOn();
  backCarLights.turnOn();
  carDoor.turnOn();
  closeTheCave.turnOn();
  rockShelf.turnOff();
  floorShake.turnOff();
  torch1.turnOff();
  torch2.turnOff();
  torch3.turnOff();
  finalFogMachine.turnOff();
  spotLight.turnOff();
  theEndLight.turnOff();
  finLight.turnOff();
  backCarLights.turnOn();
  Serial.begin(9600);
  delay(100);
  Serial1.begin(9600);
  delay(100);
  Serial2.begin(9600);
  delay(100);
  Serial3.begin(9600);
  delay(100);
  transitSerial.begin(9600);
  delay(100);
  jungleSerial.begin(9600);
  delay(100);
  backupSerial.begin(9600);
  delay(100);
  animalsSerial.begin(9600);
  //caveInsidesSerial.begin(9600);
  delay(100);
  nearCaveSerial.begin(9600);
  //badGuysSerial.begin(9600);
  //finalSerial.begin(9600);
  delay(100);
  transitMp3.begin(transitSerial);delay(100);
    Serial.println(F("Initializing transitMp3:"));
  delay(100);

  jungleMp3.begin(jungleSerial);delay(100);
  Serial.println(F("Initializing jungleMp3:"));
  delay(100);

  backupMp3.begin(backupSerial);delay(100);
  Serial.println(F("Initializing backupMp3:"));
  delay(100);

  animalsMp3.begin(animalsSerial);delay(100);
  Serial.println(F("Initializing animalsMp3:"));
  delay(100);

  nearCaveMp3.begin(nearCaveSerial);delay(100);
  Serial.println(F("Initializing nearCaveMp3:"));
  delay(100);

  caveInsidesMp3.begin(Serial2);delay(100);
  Serial.println(F("Initializing caveInsideMp3:"));
  delay(100);

  badGuysMp3.begin(Serial3);delay(100);
  Serial.println(F("Initializing badGuysMp3:"));
  delay(100);

  finalMp3.begin(Serial1);delay(100);
  Serial.println(F("Initializing finalMp3:"));
  delay(100);

  transitMp3.volume(25);
  delay(100);
  jungleMp3.volume(25);
  delay(100);
  backupMp3.volume(25);
  delay(100);
  animalsMp3.volume(20);
  delay(100);
  nearCaveMp3.volume(25);
  delay(100);
  caveInsidesMp3.volume(25);
  delay(100);
  badGuysMp3.volume(25);
  delay(100);
  finalMp3.volume(20);
  //backupMp3.enableLoop();
  //playAnimalsSound();
  delay(200);
  animalsMp3.loop(1);
  delay(200);
  
  
  resetFun();
}
void loop() {
  currentTime = millis();
  delay(50);
  treeButton.check();
  CaveRemote.check();
  statue.check();
  inputCheck();
  transitFunc();
  statueCheck();
  caveRoutine();
  endGameRoutine();
  treeButtonCheck();
}






// ------------ FUNCTIONS ----------
void resetFun(){
  Serial.println("Initiating setup mode");
  delay(100);
 


  for (int i=0; i < 5; i++){
      fireLights[i].turnOff();
  }
  startBackupSound = true;
  visherStops = false;
  transitStage = true;
  initGame = true;
  fired = false;
  accident = false;
  jungleStage = true;
  disableBench = false;
  startTheEnding = false;
  endAndReset = false;
  caveOpened = false;
  caveClosed = false;
  kidsGame = false;
  runningTheEnd = false;
  finmMusicStarted = false;
  carDoorOpened = false;
  wheelOpened = false;
  noBadGuys = false;
  treeButtonPressed = false;
  badGuysStarted = false;
  Serial.println("all booleans reset");
  delay(100);
  frontCarLightsVisher.turnOff();
  carBench.turnOff();
  trapRelay.turnOn();
  wheelLock.turnOn();
  backCarLights.turnOn();
  carDoor.turnOn();
  closeTheCave.turnOn();
  rockShelf.turnOff();
  floorShake.turnOff();
  torch1.turnOff();
  torch2.turnOff();
  torch3.turnOff();
  finalFogMachine.turnOff();
  spotLight.turnOff();
  theEndLight.turnOff();
  finLight.turnOff();
   // ---- stop Mp3s
  stopTransitSound();
  delay(50);
  stopJungleSound();
  delay(50);
  stopBackupSound();
  delay(50);
  stopBadGuysSound();
  delay(50);
  stopCaveInsidesSound();
  delay(50);
  stopCaveSound();
  delay(200);
  
  // ---- init vars
  

  //Selecting an audio track, 1 - 00XX 2 - 01XX.
  Serial.println("Selecting an audio track");
  delay(100);
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
        delay(100);
        quest_music = 0;
        break;    
    case '2':
        Serial.println("Kid Version chosen");
        delay(100);
        quest_music = 100;
        kidsGame = true;
        break;
    case '3':
        Serial.println("English Version chosen");
        delay(100);
        quest_music = 200;
        break;
    
    default:   
        break;
    }

 
  
  // ---- wait to game to start
  Serial.println("Press s to start, at least 5 sec before play.");
  delay(100);
  input.check();
  while (input.read() != 's') {
    input.check();
  }
  
  Serial.println("You can start the game in 5 seconds.\n");
  delay(100);
  startGame();
  
}




void startGame(){
  Serial.println("Press button to start game");
  delay(100);
  bool startGamePressed = false;
  while (startGamePressed == false) {
    startGameButton.check();
    if (startGameButton.isPressed()) {
      startGamePressed = true;
    }
  }
  
  gameStartTime = millis();
  playTransitSound();
  delay(200);
  finLight.turnOn();
  initGame = false;
  Serial.print("---=== GAME ");
  delay(100);

  Serial.println(" STARTED!! - ");
  delay(100);
}



void transitFunc(){
  if ((fired == false) && (gameStartTime + shootingDelay*1000 <= currentTime)){
    Serial.println("shooting has been started");
delay(100);

    for (int i=0; i < 5; i++){
      if (!kidsGame) fireLights[i].turnOn();
      delay(2000);
    }
    fired = true;
  }
  if ((accident == false) && ((gameStartTime + benchDelay*1000 )<= currentTime)) {
    Serial.println("Accident occurs");
    delay(100);
    if (disableBench == false) {
      frontCarLightsVisher.turnOn();
      playJungleSound();
      carBench.turnOn(); 
      backCarLights.turnOff();
    }
    accident = true;
  }
  if ((accident == true) && (!carDoorOpened) && ((gameStartTime + benchDelay*1000 + 9000 )<= currentTime)){
    Serial.println("open the door");
    delay(100);
    carDoorOpened = true;
    carDoor.turnOff();
  }
  

  if ((visherStops == false) && ((gameStartTime + visherDelay*1000) <= currentTime)){
    visherStops = true;
//    jungleMp3.volume(20);
    Serial.println("Start doing the wishers");
    delay(100);
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
    Serial.println("Stop doing the wishers");
  delay(100);} 

}


void openWheel(){
  wheelLock.turnOff();
}
void statueCheck(){
  if (!wheelOpened && statue.isHeld()){
    Serial.println("Statue in place, opening the wheel");
    wheelOpened = true;
    delay(100);
    openWheel();
  }
}
void treeButtonCheck(){
  if (treeButtonPressed) return;
  if (treeButton.isHeld()){
    Serial.println("Tree button pressed");
    delay(100);
    treeButtonPressed = true;
    playCaveSound();
    stopJungleSound();
  }
}
void caveRoutine(){
  if (!treeButtonPressed) return;
  if (!caveOpened && CaveRemote.isHeld()){
    Serial.println("Opening the cave");
    delay(100);
    caveOpened = true;
    playCaveInsidesSound();
    caveOpeningTime = currentTime;
    closeTheCave.turnOff();
    return;
  }
  if (caveOpened && (caveOpeningTime + delayBeforeBadGuys < currentTime) && !badGuysStarted){
    badGuysStarted = true;
    if (noBadGuys or kidsGame){
      Serial.println("bad guys are NOT coming");
      delay(100);
      return;
    }
    Serial.println("bad guys are cumming...");
    delay(100);
    playBadGuysSound();    
  }
  if (badGuysStarted && !caveClosed && CaveRemote.isHeld()){
    Serial.println("closing the cave and filling final space with fog");
    delay(100);
    caveClosed = true;
    playCaveInsidesSound();
    closeTheCave.turnOn();
    finalFogMachine.turnOn();
    delay(3000);
    finalFogMachine.turnOff();
    Serial.println("fog machine off");
  }

}
void endGameRoutine(){
if (!startTheEnding) return;
if (!runningTheEnd){
  Serial.println("staring the final sequence");
  delay(100);
  playFinalSound();
  runningTheEnd = true;
  finalSeqStartTime = currentTime+4000;
  Serial.print("Game duration was  ");
  delay(100);
  Serial.print((finalSeqStartTime-gameStartTime)/60000);
  delay(100);
  Serial.println(" minutes");
delay(100);

  return;
}
if (runningTheEnd && !torch1.isOn() && (currentTime>(finalSeqStartTime+delayTorch1)) &&(currentTime<delayAllOff+finalSeqStartTime)){
  torch1.turnOn();
  Serial.println("torch 1 lit");
delay(100);}

if (runningTheEnd && !torch2.isOn() && (currentTime>(finalSeqStartTime+delayTorch2) &&(currentTime<delayAllOff+finalSeqStartTime))){
  torch2.turnOn();
  Serial.println("torch 1 lit");
delay(100);}

if (runningTheEnd && !torch3.isOn() && (currentTime>(finalSeqStartTime+delayTorch3) &&(currentTime<delayAllOff+finalSeqStartTime))){
  torch3.turnOn();
  Serial.println("torch 3 lit");
delay(100);}

if (!fogMachineDone && !spotLight.isOn() && (currentTime>(finalSeqStartTime+delaySpot)&&(currentTime<delayAllOff+finalSeqStartTime) )){
  spotLight.turnOn();
  finalFogMachine.turnOn();
  fogMachineDone = true;
  Serial.println("spot lit, fog on");
delay(100);}

if (runningTheEnd && finalFogMachine.isOn() && (currentTime>(finalSeqStartTime+delayFogOff))&&(currentTime<delayAllOff+finalSeqStartTime) ){
  finalFogMachine.turnOff();
  Serial.println("fog machine off");
delay(100);}

if (kidsGame){
  if (runningTheEnd && torch1.isOn() && (currentTime>(finalSeqStartTime+delayKidsEnd)) ){
    torch1.turnOff();
    torch2.turnOff();
    torch3.turnOff();
    finLight.turnOn();
    Serial.println("that's all, folks");
  delay(100);}


}
else{
  if (runningTheEnd && spotLight.isOn() && (currentTime>(finalSeqStartTime+delayAllOff)) &&(currentTime<(finalSeqStartTime+delayFloor))){
    torch1.turnOff();
    torch2.turnOff();
    torch3.turnOff();
    spotLight.turnOff();
    Serial.println("turn all off");
  delay(100);}

  if (runningTheEnd && !floorShake.isOn() &&(!rockShelf.isOn())&& (currentTime>(finalSeqStartTime+delayFloor))&&(currentTime<(finalSeqStartTime+delayRocks)) ){
    floorShake.turnOn();
    theEndLight.turnOn();
    Serial.println("floor + the end sign");
  delay(100);}

  if (runningTheEnd && !rockShelf.isOn() && (currentTime>(finalSeqStartTime+delayRocks) )&& (currentTime<(finalSeqStartTime+delayBigLight) )){
    rockShelf.turnOn();
    floorShake.turnOff();
    theEndLight.turnOff();
    Serial.println("rocks fall, shaking ends");
    delay(100);
  }

  if (runningTheEnd && rockShelf.isOn() && (currentTime>(finalSeqStartTime+delayBigLight) &&!spotLight.isOn())){
    finLight.turnOff();
    spotLight.turnOn();
    Serial.println("that's all, folks");
    delay(100);
  }

}
}

// ------------ SOUND SECTION --------------


void playTransitSound() {
  Serial.print("play transit sound - ");
  delay(100);
  switch (quest_music)
  {
  case 0:
    Serial.println("Adult");
    delay(100);
    break;
  case 100:
    Serial.println("Kids");
    delay(100);
    break;
   case 200:
    Serial.println("English");
    delay(100);
    break;
  
  default:
    Serial.println("Something went wrong - OOPS. call Misha");    
    delay(100);
    break;
  }
  delay(200);
  transitMp3.playFromMP3Folder(1+quest_music);
  delay(200);
}
void stopTransitSound() {
  Serial.println("stop transit track");
  delay(100);
  transitMp3.pause();
  delay(100);
}


void playJungleSound() {
  Serial.println("play Jungle sound ");
  delay(100);
  jungleMp3.playFromMP3Folder(1);
  delay(100);
}
void stopJungleSound() {
  Serial.println("stop Jungle track");
  delay(100);
  jungleMp3.pause();
  delay(100);
}


void playBackupSound() {
  Serial.print("play animal backup ");\
  delay(100);
  backupMp3.loop(1);
  delay(100);
}
void stopBackupSound() {
  Serial.println("stop animal backup");
  delay(100);
  backupMp3.pause();
  delay(100);
}

void stopCaveSound(){
  Serial.println("stop nearCave track");
  delay(100);
  nearCaveMp3.pause();
  delay(100);
}
void playCaveSound(){
  Serial.println("play nearCave sound ");
  delay(100);
  nearCaveMp3.playFromMP3Folder(1);
  delay(100);
}

void stopCaveInsidesSound(){
  Serial.println("stop Jungle track");
  delay(100);
  caveInsidesMp3.pause();
  delay(100);
}
void playCaveInsidesSound(){
  if (caveOpened && !caveClosed){
    Serial.println("playing cave opening sound");
    delay(100);
    caveInsidesMp3.playFromMP3Folder(1);
    delay(100);  
  }
  if (caveClosed && caveOpened)
  {
    Serial.println("playing cave closing + inside sound");
    delay(100);
    caveInsidesMp3.pause();
    delay(100);
    caveInsidesMp3.playFromMP3Folder(2);
    delay(100);
  }
}

void stopBadGuysSound(){
  Serial.println("stop bad guys track");
  delay(100);
  badGuysMp3.pause();
  delay(100);
}
void playBadGuysSound(){
  Serial.println("play badguys sound ");
  delay(100);
  badGuysMp3.playFromMP3Folder(1);
  delay(100);
}

void playAnimalsSound(){
  Serial.println("play animal sounds");
  delay(100);
  //animalsMp3.enableLoop();
  animalsMp3.playFromMP3Folder(1);
  delay(100);
}

void stopFinalSound(){
  Serial.println("stop final track");
  delay(100);
  finalMp3.pause();
  delay(100);
}
void playFinalSound(){
  Serial.print("play final track");
  delay(100);
  if (kidsGame){
    finalMp3.playFromMP3Folder(101);
    delay(100);
    Serial.println(": Kids");
  delay(100);}

  else{
    finalMp3.playFromMP3Folder(1+quest_music);
    delay(100);
    Serial.println(": Adults");
  delay(100);}

}

void inputCheck() {
  input.check();
  runQuest = input.read();
  if (runQuest == 'f') {
    startTheEnding = true;
    kidsGame = false;
  }
  if (runQuest == 'd') {
    startTheEnding = true;
    kidsGame = true;
  }
  if (runQuest == 'e') {
    resetFun();
  }
  if ((runQuest == 'b') && (disableBench == false)) {
    carBench.turnOn();
  }
  if (runQuest == 'l') {
    carDoor.turnOff();
  }
  if (runQuest == 'g') {
    openWheel();
  }
  if (runQuest == 'p') {
    noBadGuys = !noBadGuys;
    Serial.print("bad Guys  sound is ");
    delay(100);
    if (noBadGuys) {
      Serial.println("OFF");
      delay(100);
    }
    else {
      Serial.println("ON");
      delay(100);
      }
  }
  if (runQuest == 'k') {
    if (startBackupSound == true) {
      playBackupSound();  
      startBackupSound = false;
    } else {
      stopBackupSound();
      startBackupSound = true;
      
    }
    //printDetail(backupMp3.readType(), backupMp3.read());
  }
}


// ----------- SOUND PRINT --------------
/*
void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      delay(100);
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      delay(100);
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      delay(100);
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      delay(100);
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      delay(100);
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      delay(100);
      Serial.print(value);
      delay(100);
      Serial.println(F(" Play Finished!"));
      delay(100);
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      delay(100);
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          delay(100);
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          delay(100);
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          delay(100);
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          delay(100);
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          delay(100);
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          delay(100);
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          delay(100);
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

*/
// ---------- ARDUINO COMMUNICATION SECTION ------------

/*
void mainSerialReader(){
    while (Serial3.available()){
      char c = Serial3.read();
      if (c == '|'){
        Serial.println();
      delay(100);} 
      else {
          Serial.print(c);
      delay(100);}

      delay(10);
    } 
}

void serialSender(char data[]){
  Serial.print("sending to second arduino ");
  delay(100);
  Serial.println(data);
  delay(100);
  Serial3.write(data);
}
  */

