/*	2014-01-07 by lisper <leyapin@gmail.com>
 *	control mp3 module by seirial
 *	use mp3TF.h	
 *	test on UNO
 */

#include <SoftwareSerial.h>
#include <mp3TF.h>
#include <input.h>
#include <stl.h>
#include <quest.h>

Input input;
mp3TF mp3tf = mp3TF ();
Button but(12);
int track = 1;

//
void setup () {
	Serial.println("1");
	mp3tf.init (&Serial2);
	Serial2.begin (9600);
	Serial.begin(9600);
	//Serial1.begin (9600);
	delay(500);
	mp3tf.play();
	Serial.println("2");
}

//
void loop () {
	input.check();

	if (input.read() == '1') {
		Serial.print("Playing ");
		Serial.println(track);
		mp3tf.play(track);
		track++;
		if (track == 6) track = 1;
	}
}

/****************************************************************
 *		mp3TF mp3tf = mp3TF ();
 *	UNO:
 *		Serial.begin (9600);
 *		mp3tf.init (&Serial);
 *	LEONARDO:
 *		Serial1.begin (9600);
 *		mp3tf.init (&Serial1);
 *	
 *	function:
 *		mp3tf.play ();		play once
 *		mp3tf.play (1);		play first
 *		mp3tf.play (3);		play third
 *		mp3tf.pause ();		pause
 *		mp3tf.stop ();		stop (на самом деле это таже функция что и пауза)
 *		mp3tf.next ();		next
 *		mp3tf.previous ();	previous
 *		mp3tf.upVolume ();	upVolume
 *		mp3tf.downVolume ();	downVolume
 *		mp3tf.setVolume (20);	setVolume 20
 *****************************************************************/

