/*
stl.h - Standard library. Contains classes for handling buttons, sensors and all general stuff.
Created by Amit Arnon, December 12, 2015.
*/

#ifndef stl_h
#define stl_h

#include "Arduino.h"

// MACROS
#define LOG(logstr) if(DEBUG)Serial.print(logstr)
#define LOGLN(logstr) if(DEBUG)Serial.println(logstr)

// STL

#define INPUT_NOCHANGE 0
#define INPUT_OFF 1
#define INPUT_ON 2
#define OUTPUT_NOCHANGE 0
#define OUTPUT_OPEN 1
#define OUTPUT_CLOSED 2

class GenericInput {
public:
	GenericInput(int pin, bool pullUp = true, bool autoPullup = true);
	char check();
	void debug(char name[]);
	void overrideState(int state, bool once = false);	//INPUT_NOCHANGE to disable

protected:
	bool debounce();
	bool _lastState;
	int _lastButtonRead;
	int _pin;
	bool _pullUp;
	char* _debug;
	int _override;
	bool _overrideOnce;
};

class GenericOutput {
public:
	GenericOutput(int pin, bool normallyOpen);
	virtual void turnOn();
	virtual void turnOff();
	virtual void toggle();
	virtual bool isOn();
	void debug(char name[]);
	
protected:
	int _pin;
	bool _currentState;
	bool _normallyOpen;
	char* _debug;
};

class Button : public GenericInput{
public:
	Button(int pin, bool pullUp = true, bool autoPullup = true);
	bool isReleased();
	bool isPressed();
	bool isHeld();
};

class IRSensor : public GenericInput {
public:
	IRSensor(int pin);
	bool isDetected();
	bool isUndetected();
	bool isDetecting();
};

class Led : public GenericOutput {
public:
	Led(int pin);
};

class Relay : public GenericOutput {
public:
	Relay(int pin, bool normallyOpen);
};



#endif
