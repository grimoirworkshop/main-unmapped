#include "Arduino.h"
#include "KeypadModel.h"



KeypadModel::KeypadModel(char password[passwordLength], int pin) {
  strncpy(_password, password, passwordLength);
  _pin = pin;
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, HIGH);
}

bool KeypadModel::check(char input){
  bool ret = false;
  if (input == _password[_indx]){
    _count++;
  }
  if (_indx == passwordLength-1){
    if (_count == passwordLength){
      _success = true;
      Serial.print(_password);
      Serial.println(": entered successfully");
      openLocker();
	  ret = true;
    } else {
      Serial.println("failed");
    }
    _indx = 0;
    _count = 0;
  } else {
    _indx++;
  }
  return ret;
}

void KeypadModel::openLocker(){
  digitalWrite(_pin, LOW);
  Serial.print("opened locker #");
  Serial.println(_pin);
}

void KeypadModel::lockLocker(){
  digitalWrite(_pin, HIGH);
  _success = false;
  Serial.print("locked locker #");
  Serial.println(_pin);
}

void KeypadModel::undo(){
  lockLocker();
  _indx = 0;
  _count = 0;
}

bool KeypadModel::getLockerState(){
	return _success;
}

int KeypadModel::getIndex(){
	return _indx;
}

int KeypadModel::getCount(){
	return _count;
}
