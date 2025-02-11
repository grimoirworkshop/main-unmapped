/*
input.h - Library for simulating inputs by serial commands
Created by Amit Arnon, February 17 2016.
*/

#include "input.h"

void Input::check() {
	_readByte = 0;
	if (Serial.available() > 0) {
		// read the incoming byte:
		_readByte = Serial.read();
	}
}

int Input::read() {
	return _readByte;
}

void Input::handle(GenericInput *input, int state, char letter, bool hold) {
	if (_readByte == letter) {
		input->overrideState(state, !hold);
	}
}

void Input::handle(GenericInput *input, char letterOn, char letterOff, char letterReturn) {
	if (_readByte == letterOn) {
		input->overrideState(INPUT_ON);
	}
	if (letterOff != 0 && _readByte == letterOff) {
		input->overrideState(INPUT_OFF);
	}
	if (letterReturn != 0 && _readByte == letterReturn) {
		input->overrideState(INPUT_NOCHANGE);
	}
}

void Input::handle(Quest *q, char letter) {
	if (_readByte == letter) {
		q->done();
	}
}
