#include "stl.h"

#define DEBUG 1

Button button1(7);
Button button2(8, false);	// false means it's not INPUT_PULLUP, just INPUT. It's still considered pullup for LOW/HIGH designation.
Led led (13);
Relay relay (12, false);	// The second parameters is "normallyOpen". false means normally closed.
IRSensor sensor(23);

void setup()
{
	Serial.begin(9600);
	button1.debug("Button 1");
}

void loop()
{
	button1.overrideState(INPUT_ON);
	button1.check();
	button1.overrideState(INPUT_OFF);
	button1.check();

	/*sensor.check();
	if (sensor.isDetected()) {
		led.toggle();
	}*/

	/*
	button1.check();
	if (button1.isPressed()) {
		led.toggle();
	}*/
	/*
	Button::State button2state = button2.check();
	switch (button2state) {
	case Button::Down:
		Serial.println("Button Pressed");
		break;
	case Button::Up:
		Serial.println("Button Released");
		break;
	}*/
}
