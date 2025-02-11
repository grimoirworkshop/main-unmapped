/*
input.h - Library for simulating inputs by serial commands
Created by Amit Arnon, February 17, 2016.
*/

#ifndef input_h
#define input_h

#include "Arduino.h"
#include "stl.h"
#include "quest.h"

class Input {
public:
	void check();
	int read();

	void handle(GenericInput *input, int state, char letter, bool hold = false);
	void handle(GenericInput *input, char letterOn, char letterOff = 0, char letterReturn = 0);
	void handle(Quest *quest, char letter);
	//static void handle(Quest *quest, char letter);
protected:
	int _readByte;
};

#endif
