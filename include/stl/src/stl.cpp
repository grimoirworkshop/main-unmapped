/*
stl.cpp - Standard library. Contains classes for handling buttons, sensors and all general stuff.
Created by Amit Arnon, December 12, 2015.
*/

#include "Arduino.h"
#include "stl.h"

GenericInput::GenericInput(int pin, bool pullUp, bool autoPullup)
{
	pinMode(pin, (autoPullup ? INPUT_PULLUP : INPUT));
	_pin = pin;
	_lastState = (pullUp ? HIGH : LOW);
	_pullUp = pullUp;
	_debug = NULL;
	_override = INPUT_NOCHANGE;
	_overrideOnce = false;
}

void GenericInput::debug(char name[]) {
	_debug = name;
}

void GenericInput::overrideState(int state, bool once) {
	_override = state;
	_overrideOnce = once;
}

/// Gets the last switch state
/// Returns the actual debounced state if changed
bool GenericInput::debounce() {
	bool curButton = digitalRead(_pin);
	if (_lastState != curButton) {
		delay(5);
		curButton = digitalRead(_pin);
	}
	return curButton;
}

// True if the switch was pressed (changed state since last check)
char GenericInput::check() {
	char pressed = INPUT_NOCHANGE;
	bool curState;
	
	if (_override == INPUT_NOCHANGE)
	{
		curState = debounce();
	}
	else {
		if (_override == INPUT_ON)
			curState = (_pullUp ? LOW : HIGH);
		else
			curState = (_pullUp ? HIGH : LOW);

		if (_overrideOnce) {
			_override = INPUT_NOCHANGE;
			_overrideOnce = false;
		}
	}

	// We use pullups so switch is LOW when pressed
	if (curState == LOW && _lastState == HIGH) {
		pressed = (_pullUp ? INPUT_ON : INPUT_OFF);
		if (_debug) {
			Serial.print(_debug);
			Serial.print(" is ");
			Serial.println(pressed == INPUT_ON ? "ON" : "off");
		}
	}
	else if (curState == HIGH && _lastState == LOW) {
		pressed = (_pullUp ? INPUT_OFF : INPUT_ON);
		if (_debug) {
			Serial.print(_debug);
			Serial.print(" is ");
			Serial.println(pressed == INPUT_ON ? "ON" : "off");
		}
	}

	_lastState = curState;
	_lastButtonRead = pressed;
	return pressed;
}

/// Note - button is always on a PULLUP pin
Button::Button(int pin, bool pullUp, bool autoPullup) : GenericInput(pin, pullUp, autoPullup) {}

bool Button::isPressed() {
	return (_lastButtonRead == INPUT_ON);
}

bool Button::isReleased() {
	return (_lastButtonRead == INPUT_OFF);
}

bool Button::isHeld() {
	return (_pullUp ? !_lastState : _lastState) ;
}

IRSensor::IRSensor(int pin) : GenericInput(pin, true, false) {};

bool IRSensor::isDetected() {
	return (_lastButtonRead == INPUT_ON);
}

bool IRSensor::isUndetected() {
	return (_lastButtonRead == INPUT_OFF);
}

bool IRSensor::isDetecting() {
	return !_lastState;
}

GenericOutput::GenericOutput(int pin, bool normallyOpen)
{
	pinMode(pin, OUTPUT);
	_pin = pin;
	_currentState = false;
	_normallyOpen = normallyOpen;
}

void GenericOutput::debug(char name[]) {
	_debug = name;
}

void GenericOutput::turnOn() {
	digitalWrite(_pin, (_normallyOpen ? HIGH : LOW));
	if (_debug && _currentState == false) {
		Serial.print(_debug);
		Serial.println(" is ON");
	}
	_currentState = true;
}

void GenericOutput::turnOff() {
	digitalWrite(_pin, (_normallyOpen ? LOW : HIGH));
	if (_debug && _currentState == true) {
		Serial.print(_debug);
		Serial.println(" is off");
	}
	_currentState = false;
}

void GenericOutput::toggle() {
	if (isOn()) turnOff();
	else turnOn();
}

bool GenericOutput::isOn() {
	return _currentState;
}

Led::Led(int pin) : GenericOutput(pin, true) {}

Relay::Relay(int pin, bool normallyOpen) : GenericOutput(pin, normallyOpen) {}