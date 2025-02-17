//#include <mp3TF.h>
#include <Arduino.h>
#include "DFRobotDFPlayerMini.h"


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
Relay floorShake(32, true);
Relay wheelLock(33, true); //--
Relay frontCarLightsVisher(7, false);
Relay backCarLights(28, false);
Relay closeTheCave(30, true);           //dont remember which pin
Relay carDoor(42, true);            //dont remember which pin
Relay torch1(34, true);
Relay torch2(35, true);
Relay torch3(37, true);
Relay spotLight(36, true);
Relay finLight(40, true);
Relay theEndLight(41, true);
Relay finalFogMachine(39, true);
Relay rockShelf(38, true);



// General Vars
unsigned long transitDuration = 5*60*1000L; //300; // miliseconds to print transit section is over
unsigned long delayBeforeBadGuys = 25000;
unsigned long delayTorch1 = 15000;
unsigned long delayTorch2 = 17000;
unsigned long delayTorch3 = 19000;
unsigned long delaySpot = 25000;
unsigned long delayFogOff = 26500;
unsigned long delayAllOff = 000;
unsigned long delayKidsEnd = 000;
unsigned long delayFloor = 000;
unsigned long delayRocks = 000;
unsigned long delayTorch3 = 000;
unsigned long delayTheEnd = 000;
unsigned long delayBigLight = 000;

unsigned long gameStartTime;
unsigned long caveOpeningTime;
unsigned long finalSeqStartTime;

long shootingDelay = 125; //60; //miliseconds until shooting shounds
long benchDelay = shootingDelay + 21; //miliseconds from shooting to bench down
long visherDelay = benchDelay + 180; // seconds from accident to car battery out



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
bool fogMachineOn = false;
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
SoftwareSerial transitSerial(10, 11); 
SoftwareSerial jungleSerial(8, 9);
SoftwareSerial nearCaveSerial(4, 5);
SoftwareSerial caveInsidesSerial(16, 17);
SoftwareSerial badGuysSerial(14, 15);
SoftwareSerial finalSerial(18, 19);
SoftwareSerial animalsSerial(2, 3);

DFRobotDFPlayerMini transitMp3;
DFRobotDFPlayerMini jungleMp3;
DFRobotDFPlayerMini backupMp3;
DFRobotDFPlayerMini nearCaveMp3;
DFRobotDFPlayerMini caveInsidesMp3;
DFRobotDFPlayerMini badGuysMp3;
DFRobotDFPlayerMini finalMp3;
DFRobotDFPlayerMini animalsMp3;
void printDetail(uint8_t type, int value);




void setup() {
  frontCarLightsVisher.turnOff();
  carBench.turnOff();
  trapRelay.turnOn();
  wheelLock.turnOff();
  backCarLights.turnOn();
  Serial.begin(9600);
  transitSerial.begin(9600);
  jungleSerial.begin(9600);
  backupSerial.begin(9600);
  animalsSerial.begin(9600);
  caveInsidesSerial.begin(9600);
  nearCaveSerial.begin(9600);
  badGuysSerial.begin(9600);
  finalSerial.begin(9600);
  if (!transitMp3.begin(transitSerial)) {
    Serial.println(F("Initializing transitMp3:"));
  }
  if (!jungleMp3.begin(jungleSerial)) {
    Serial.println(F("Initializing jungleMp3:"));
  }
  if (!backupMp3.begin(backupSerial)) {
    Serial.println(F("Initializing monkeyMp3:"));
  }
  if (!animalsMp3.begin(animalsSerial)) {
    Serial.println(F("Initializing monkeyMp3:"));
  }
  if (!nearCaveMp3.begin(nearCaveSerial)) {
    Serial.println(F("Initializing monkeyMp3:"));
  }
  if (!caveInsidesMp3.begin(caveInsidesSerial)) {
    Serial.println(F("Initializing monkeyMp3:"));
  }
  if (!badGuysMp3.begin(badGuysSerial)) {
    Serial.println(F("Initializing monkeyMp3:"));
  }
  if (!finalMp3.begin(finalSerial)) {
    Serial.println(F("Initializing monkeyMp3:"));
  }
  transitMp3.volume(25);
  jungleMp3.volume(25);
  backupMp3.volume(25);
  animalsMp3.volume(10);
  nearCaveMp3.volume(25);
  caveInsidesMp3.volume(25);
  badGuysMp3.volume(25);
  finalMp3.volume(25);
  backupMp3.enableLoop();
  animalsMp3.enableLoop();
  resetFun();
}
void loop() {
  treeButton.check();
  CaveRemote.check();
  statue.check();
  inputCheck();
  transitFunc();
  statueCheck();
  remoteCheck();
  caveRoutine();
  endGameRoutine();
  treeButtonCheck();
}






// ------------ FUNCTIONS ----------
void resetFun(){
  Serial.println("Initiating setup mode");
  // ---- stop Mp3s
  stopTransitSound();
  stopJungleSound();
  stopBackupSound();
  // ---- init vars


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
  torch1Lit = false;
  torch2Lit = false;
  torch3Lit = false;
  spotLit = false;
  fogMachineOn = false;
  rocksFall = false;
  floorShaking = false;
  carDoorOpened = false;
  wheelOpened = false;
  noBadGuys = false;
  treeButtonPressed = false;
  badGuysStarted = false;
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
  finLight.turnOn();
  

  //Selecting an audio track, 1 - 00XX 2 - 01XX.
  Serial.println("Selecting an audio track");
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
        kidsGame = true;
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
  }
  
  Serial.println("You can start the game in 5 seconds.\n");
  
  
}




void startGame(){
  Serial.println("Press button to start game");
  bool startGamePressed = false;
  while (startGamePressed == false) {
    startGameButton.check();
    if (startGameButton.isPressed()) {
      startGamePressed = true;
    }
  }
  
  gameStartTime = millis();
  playTransitSound();
  initGame = false;
  Serial.print("---=== GAME ");
  
  Serial.println(" STARTED!! - ");
}


void transitFunc(){
  if ((fired == false) && (gameStartTime + shootingDelay*1000 <= millis())){
    Serial.println("shooting has been started");

    for (int i=0; i < 5; i++){
      if (!kidsGame) fireLights[i].turnOn();
      delay(2000);
    }
    fired = true;
  }
  if ((accident == false) && (gameStartTime + benchDelay*1000 <= millis())) {
    Serial.println("Accident occurs");
    if (disableBench == false) {
      frontCarLightsVisher.turnOn();
      playJungleSound();
      carBench.turnOn(); 
      backCarLights.turnOff();
    }
    accident = true;
  }
  if ((accident == true) && (!carDoorOpened) && (gameStartTime + benchDelay*1000 + 9000 <= millis())){
    Serial.println("open the door");
    carDoorOpened = true;
    carDoor.turnOff();
  }
  

  if ((visherStops == false) && (gameStartTime + visherDelay*1000 <= millis())) {
    visherStops = true;
//    jungleMp3.volume(20);
    Serial.println("Start doing the wishers");
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
  }    
}


void openWheel(){
  wheelLock.turnOff();
}
void statueCheck(){
  if (!wheelOpened && statue.isHeld()){
    Serial.println("Statue in place, opening the wheel");
    openWheel();
  }
}
void treeButtonCheck(){
  if (treeButtonPressed) return;
  if (treeButton.isHeld()){
    Serial.println("Tree button pressed");
    treeButtonPressed = true;
    playCaveSound();
  }
}
void caveRoutine(){
  if (!treeButtonPressed) return;
  if (!caveOpened && CaveRemote.isHeld()){
    Serial.println("Opening the cave");
    caveOpened = true;
    playCaveInsidesSound();
    caveOpeningTime = millis();
    closeTheCave.turnOff();
    return;
  }
  if (caveOpened && (caveOpeningTime < millis()) && !badGuysStarted){
    badGuysStarted = true;
    if (noBadGuys or kidsGame){
      Serial.println("bad guys are NOT coming");
      return;
    }
    Serial.println("bad guys are cumming...");
    playBadGuysSound();    
  }
  if (badGuysStarted && !caveClosed && CaveRemote.isHeld()){
    Serial.println("closing the cave and filling final space with fog");
    caveClosed = true;
    playCaveInsidesSound();
    finalFogMachine.turnOn();
    delay(3000);
    finalFogMachine.turnOff();
  }

}
void endGameRoutine(){
if (!startTheEnding) return;
if (!runningTheEnd){
  Serial.println("staring the final sequence");
  playFinalSound();
  runningTheEnd = true;
  finalSeqStartTime = millis();
  Serial.print("Game duration was  ");
  Serial.print((finalSeqStartTime-gameStartTime)/60000);
  Serial.println(" minutes");

  return;
}
if (runningTheEnd && !torch1.isOn() && (millis()>finalSeqStartTime+delayTorch1) ){
  torch1.turnOn();
  Serial.println("torch 1 lit");
}
if (runningTheEnd && !torch2.isOn() && (millis()>finalSeqStartTime+delayTorch2) ){
  torch2.turnOn();
  Serial.println("torch 1 lit");
}
if (runningTheEnd && !torch3.isOn() && (millis()>finalSeqStartTime+delayTorch3) ){
  torch3.turnOn();
  Serial.println("torch 3 lit");
}
if (runningTheEnd && !spotLight.isOn() && (millis()>finalSeqStartTime+delaySpot) ){
  spotLight.turnOn();
  finalFogMachine.turnOn();
  Serial.println("spot lit, fog on");
}
if (runningTheEnd && finalFogMachine.isOn() && (millis()>finalSeqStartTime+delayFogOff) ){
  torch1.turnOn();
  Serial.println("fog machine off");
}
if (kidsGame){
  if (runningTheEnd && torch1.isOn() && (millis()>finalSeqStartTime+delayKidsEnd) ){
    torch1.turnOff();
    torch2.turnOff();
    torch3.turnOff();
    finLight.turnOn();
    Serial.println("that's all, folks");
  }

}
else{
  if (runningTheEnd && spotLight.isOn() && (millis()>finalSeqStartTime+delayAllOff) ){
    torch1.turnOff();
    torch2.turnOff();
    torch3.turnOff();
    spotLight.turnOff();
    Serial.println("turn all off");
  }
  if (runningTheEnd && !floorShake.isOn() && (millis()>finalSeqStartTime+delayFloor) ){
    floorShake.turnOn();
    theEndLight.turnOn();
    Serial.println("floor + the end sign");
  }
  if (runningTheEnd && !rockShelf.isOn() && (millis()>finalSeqStartTime+delayRocks) ){
    rockShelf.turnOn();
    floorShake.turnOff();
    theEndLight.turnOff();
    Serial.println("rocks fall, shaking ends");
  }
  if (runningTheEnd && rockShelf.isOn() && (millis()>finalSeqStartTime+delayTheEnd) ){
    finLight.turnOn();
    spotLight.turnOn();
    rockShelf.turnOff();
    Serial.println("that's all, folks");
  }
}
}

// ------------ SOUND SECTION --------------


void setTransitSound() {
  Serial.print("play transit sound - ");
  switch (quest_music)
  {
  case 0:
    Serial.println("Adult");
    break;
  case 100:
    Serial.println("Kids");
    break;
   case 200:
    Serial.println("English");
    break;
  
  default:
    Serial.println("Something went wrong - OOPS. call Misha");    
    break;
  }
  transitMp3.playMp3Folder(1 + quest_music);
}
void stopTransitSound() {
  Serial.println("stop transit track");
  transitMp3.stop();
}


void playJungleSound() {
  Serial.println("play Jungle sound ");
  jungleMp3.playMp3Folder(1);
}
void stopJungleSound() {
  Serial.println("stop Jungle track");
  jungleMp3.stop();
}


void playBackup() {
  Serial.print("play animal backup ");\
  backupMp3.playMp3Folder(1);
}
void stopBackup() {
  Serial.println("stop animal backup");
  backupMp3.stop();
}

void stopCaveSound(){
  Serial.println("stop nearCave track");
  jungleMp3.stop();
}
void playCaveSound(){
  Serial.println("play nearCave sound ");
  jungleMp3.playMp3Folder(1);
}

void stopCaveInsidesSound(){
  Serial.println("stop Jungle track");
  caveInsidesMp3.stop();
}
void playCaveInsidesSound(){
  if (caveOpened && !caveClosed){
    Serial.println("playing cave opening sound");
    caveInsidesMp3.playMp3Folder(1);  
  }
  else if (caveClosed && caveOpened)
  {
    Serial.println("playing cave closing + inside sound");
    caveInsidesMp3.playMp3Folder(2);
  }
}

void stopBadGuysSound(){
  Serial.println("stop bad guys track");
  badGuysMp3.stop();
}
void playBadGuysSound(){
  Serial.println("play badguys sound ");
  badGuysMp3.playMp3Folder(1);
}

void playAnimalsSound(){
  Serial.println("play animal sounds");
  animalsMp3.playMp3Folder(1);
}

void stopFinalSound(){
  Serial.println("stop final track");
  finalMp3.stop();
}
void playFinalSound(){
  Serial.print("play final track");
  if (kidsGame){
    finalMp3.playMp3Folder(101);
    Serial.println(": Kids");
  }
  else{
    finalMp3.playMp3Folder(1+quest_music);
    Serial.println(": Adults");
  }
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
    if (noBadGuys) Serial.println("OFF");
    else Serial.println("ON");
  }
  if (runQuest == 'k') {
    if (startBackupSound == true) {
      playBackupSound();  
      startBackupSound = false;
    } else {
      stopBackupSound();
      startBackupSound = true;
      
    }
    printDetail(backupMp3.readType(), backupMp3.read());
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

/*
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
  */

