#include <quest.h>
#include <Wire.h>
#include "stl.h"
#include "input.h"

Input input;
Button button1(3);
Button button2(4);
Quest quest(result);

void setup()
{
	Serial.begin(9600);
	button1.debug("Button 1");
	button2.debug("Button 2");
}

void loop()
{
	input.check();
	input.handle(&button1, INPUT_ON, '1');
	input.handle(&button2, INPUT_ON, '2');
	input.handle(&button1, INPUT_ON, 'q', true);
	input.handle(&button2, INPUT_ON, 'w', true);
	input.handle(&button1, INPUT_OFF, 'a', true);
	input.handle(&button2, INPUT_OFF, 's', true);
	input.handle(&quest, 'z');
	button1.check();
	button2.check();
}

// Will be called when the quest is done
void result() {
	Serial.println("Quest done");
}