
#include <analogReadNonBlocking.h>

//instantiate as many as you need
	analogReadNonBlocking pin36(36);

	analogReadNonBlocking pin39(39);

	uint8_t ArbitrationToken; //must have only one of these 

//The raw values you will use in the rest of the code
	uint16_t pin36Raw;
	uint16_t pin39Raw;

//loop counter to show that the code is non-blocking
	uint32_t loopcounter;

void setup() {
	Serial.begin(115200);

	/*
	//Setup your calibration data somewhere...from EEPROM, a JSON on SPIFFS, manually edit the default values in analogReadNonBlocking.h, whatever
	pin36.X6 = 3.07344E-18;
	pin36.X5 = -3.28432E-14;
	pin36.X4 = 1.16038E-10;
	pin36.X3 = -1.531075E-07;
	pin36.X2 = 0.0000357848;
	pin36.X = 1.06235;
	pin36.b = 180.189;
	*/
	
}

void loop()
{	
	pin36.analogRead(100000, pin36Raw, ArbitrationToken); 
	pin39.analogRead(100000, pin39Raw, ArbitrationToken);

	//100,000 samples @ ~10us per sample gives a ~1sample per second rate which is useful for this example to not flood the serial output
	//In this example, I'm using the default calibration data and converting it to 3.3V for the serial output
	if (pin36.newValueFlag){
	  Serial.print("GPIO36 raw counts = ");	  Serial.print(pin36Raw);
	  Serial.print(", voltage= ");	  Serial.print((double)pin36Raw / 4095 * 3.3, 3);
	  Serial.print("V, 'calibrated' counts = ");	  Serial.print(pin36.analogCalibration((double)pin36Raw));
	  Serial.print(", 'calibrated' voltage = ");	  Serial.print(pin36.analogCalibration((double)pin36Raw) / 4095 * 3.3, 3);
	  Serial.println("V");
	}
	if (pin39.newValueFlag){
		Serial.print("GPIO39 raw counts = ");	  Serial.print(pin39Raw);
		Serial.print(", voltage= ");	  Serial.print((double)pin39Raw / 4095 * 3.3, 3);
		Serial.print("V, 'calibrated' counts = ");	  Serial.print(pin36.analogCalibration((double)pin39Raw));
		Serial.print(", 'calibrated' voltage = ");	  Serial.print(pin36.analogCalibration((double)pin39Raw) / 4095 * 3.3, 3);
		Serial.println("V");
		Serial.print("loopcounter= ");  Serial.println(loopcounter);
		Serial.println();
	}
	
	//more code...
	loopcounter++;
}
