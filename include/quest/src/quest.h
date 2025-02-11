/*
stl.h - Standard library. Contains classes for handling buttons, sensors and all general stuff.
Created by Amit Arnon, December 12, 2015.
*/

#ifndef quest_h
#define quest_h

#include "Arduino.h"
#include <string.h>

typedef void(*QuestCallback)(char);
typedef void(*QuestResults)();
typedef bool(*QuestConditions)();

class Quest {
public:
	Quest();
	Quest(QuestResults callback);
	Quest(QuestConditions conditions, QuestResults callback);
	Quest(QuestResults callback, QuestConditions conditions);
	void check();
	void done();
	void undo();
	bool isDone();

	void registerDone(QuestCallback callback, char letter);	// This is used by the sender
protected:
	QuestCallback _doneCallback;
	QuestResults _resultCallback;
	QuestConditions _conditionFunction;
	bool _status;
	
	char _letter; // This is for transmission use - TODO: See if we can move it to the sender
};

#endif
