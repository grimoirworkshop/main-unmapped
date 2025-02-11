/*        2014-01-07 by lisper <leyapin@gmail.com>
 *        control mp3 module by seirial
 */

#include <Arduino.h>
#include <SoftwareSerial.h>

class mp3TF{         
       HardwareSerial* mySerial;
	   SoftwareSerial* mySoftSerial;
	   bool hard = true;
public :
	void init (HardwareSerial*);
	void init (SoftwareSerial*);
	void sendCmd (uint8_t *); 
    void play(); 
	void play (uint16_t index);
	void play (int index);
    void pause(); 
    void stop(); 
    void next (); 
    void previous(); 
	void upVolume();
	void downVolume();
	void setVolume(uint16_t volume);
	void setVolume(int volume);
};

