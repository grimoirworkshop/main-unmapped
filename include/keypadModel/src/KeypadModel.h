#ifndef KeypadModel_h
#define KeypadModel_h

#include "Arduino.h"

const int passwordLength = 4;

class KeypadModel {
    char _password[passwordLength];
  private:
    int _pin;
    int _output;
    bool _success = false;
    int _count = 0;
    int _indx = 0;
  public:
    KeypadModel(char password[passwordLength], int pin);
    bool check(char input);
    void openLocker();
    void lockLocker();
	void setFalse();
	bool getLockerState();
	int getIndex();
	int getCount();
	void undo();
};

#endif