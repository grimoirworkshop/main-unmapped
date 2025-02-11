/*
stl.cpp - Standard library. Contains classes for handling buttons, sensors and all general stuff.
Created by Amit Arnon, December 12, 2015.
*/

#include "Arduino.h"
#include "quest.h"

Quest::Quest()
{
	_doneCallback = NULL;
	_status = false;
}

Quest::Quest(QuestResults results)
{
	_resultCallback = results;
}

Quest::Quest(QuestResults results, QuestConditions conditions)
{
	_resultCallback = results;
	_conditionFunction = conditions;
}

Quest::Quest(QuestConditions conditions, QuestResults results)
{
	_resultCallback = results;
	_conditionFunction = conditions;
}

void Quest::check()
{
	if (!isDone() && _conditionFunction()) {
		done();
	}
}

void Quest::done()
{
	if (_status == false) {
		_status = true;
		if (_resultCallback != NULL) {
			_resultCallback();
		}
		if (_doneCallback != NULL) {
			_doneCallback(_letter);
		}
	}
}

void Quest::undo()
{
	_status = false;
}

bool Quest::isDone()
{
	return _status;
}

void Quest::registerDone(QuestCallback callback, char letter) {
	_doneCallback = callback;
	_letter = letter;
}