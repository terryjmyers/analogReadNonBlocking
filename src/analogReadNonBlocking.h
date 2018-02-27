/*
analogReadNonBlocking for ESP32
v1.0
Terry Myers
github.com/terryjmyers/analogReadNonBlocking

Parameters
	uint132_t NumOfSamples = Number of samples to average together for better smoothing.  Note that this should be limited to <= 1,048,831 (2^32 / 4095) samples due to the uint32_t size
	uint16_t &adcRaw = your global variable that this routine will modify
	unit8_t &ArbitrationToken = A global variable that each instance will use as a round robin token arbitrate the ADC

I.E. XXX.analogRead(100,TempRaw, ADC1ArbitrationToken).  This will take 100 samples ( minimum of ~1ms @ 10us per sample)

Theory of operation:
Pass in the Number of Samples to take, as well as your main program's adc raw variable, as well as a globally scoped Arbitration Token.  
If the adc isn't busy and the arbitration token is 0 (ADC is free), this library will change the token to the pin to "take ownership" of ADC1, initiate a conversion, and immediatly return to allow your code to process
Oneach subsequent call of analogRead, it will check to see the adc conversion is done.  If so, it will initate another one if NumOfSamples > 1.
If the NumOfSamples has been reached (even if you juset set to 1), it will update adcRaw and release arbitration.
This will allow the next instance of this function to take ownership and use the ADC.
Ownership and use of the ADC will thusly happen in a "round robin" fashion in the order of code execution.
*/

#ifndef ESP32
#error "analogReadNonBlock.h library only valid for ESP32"
#endif // !ESP32
#include <Arduino.h>
#ifndef analogReadNonBlocking_H
#define analogReadNonBlocking_H

class analogReadNonBlocking
{
	public:
		analogReadNonBlocking(uint8_t pin) { // uint8_t pin = Valid GPIO pins on ADC1 (ADC2 not yet implemented).  Valid pins are : 32, 33, 34, 35, 36, 37, 38, or 39
			_pin = pin;
			//An initial useless adc read is performed because when the ESP32 boots up its ADC its "busy".
			//Doing this at least once, "releases" the ADC for the first instance of this library to take ownership and start working
			adcAttachPin(_pin);
			adcStart(_pin);
		};

		void analogRead(uint32_t NumOfSamples, uint16_t &adcRaw, uint8_t &ArbitrationToken); //call this every loop
		bool newValueFlag; //Set true for one loop when the adcRaw value is updated.  Can be used to update scaling only when nessesary


		double analogCalibration(double adcRaw) {// Performs a polynomial fit to linearize the awful ADC
			double adcCalibrated;
			if (X6 != 0) adcCalibrated += X6 * pow(adcRaw, 6);
			if (X5 != 0) adcCalibrated += X5 * pow(adcRaw, 5);
			if (X4 != 0) adcCalibrated += X4 * pow(adcRaw, 4);
			if (X3 != 0) adcCalibrated += X3 * pow(adcRaw, 3);
			if (X2 != 0) adcCalibrated += X2 * pow(adcRaw, 2);
			if (X != 0) adcCalibrated += X * adcRaw;
			if (b != 0) adcCalibrated += b;
			return adcCalibrated;
		};
		//Note that these default values were the average of a couple ESP32's I had and linearized the 0-4095 12-bit raw counts.
		//They will most likely give you a useable linear response accurate to 100mV (over a 0-3.3V range, or 11dB attenuation)
		//However you can perform your own calibration with excel and a trendline to obtain the coefficients and therefore characterize each individual ESP32.  See spreadsheet provided in the library
		double X6 = 3.07344E-18;
		double X5 = -3.28432E-14;
		double X4 = 1.16038E-10;
		double X3 = -1.531075E-07;
		double X2 = 0.0000357848;
		double X = 1.06235;
		double b = 180.189;		

	private:
		uint8_t _pin;
		//Used for averaging
		uint32_t _adcSamplesTotal;
		uint32_t _adcCurrentSamples;
};
#endif