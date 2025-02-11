#include "stl.h"
#include "quest.h"

Quest qu(result);

void setup()
{
	Serial.begin(9600);
	qu.registerDone(callback, 'b');		// Set the callback function when done is called
	qu.done();
}

void loop()
{
	
}

// Will be called when the quest is done
void result() {
	Serial.println("Quest done");
}

// Will be called when the quest is done
void callback(char letter) {
	Serial.println("Callback called");
	Serial.println(letter);
}
