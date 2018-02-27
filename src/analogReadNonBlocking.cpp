#include "analogReadNonBlocking.h"

void analogReadNonBlocking::analogRead(uint32_t NumOfSamples, uint16_t &adcRaw, uint8_t &ArbitrationToken) {
	/*
	uint8_t pin = Valid GPIO pins on ADC1 (ADC2 not yet implemented).  Value pins are:  32, 33, 34, 35, 36, 37, 38, or 39
	uint16_t NumOfSamples = Number of samples to average together for better smoothing.
	&uint16_t adcRaw = your global variable that this routine will modify when complete
	&unit8_t &ArbitrationToken = A global variable that this routine will assign to handle arbitration between itself and other instances of this routine

	I.E. analogRead(39,100,Pin39ADCRaw, ADC1ArbitrationToken).  This will take 100 samples ( min of ~1ms @ 10us per sample)

	*/
	newValueFlag = false;
	if (adcBusy(_pin) || (ArbitrationToken != _pin && ArbitrationToken > 0)) return; //no point in doing anything if the adc is already doing something or someone else has ownership

	if (ArbitrationToken == 0) { //adc isn't busy, nor is it arbitrated, now's your chance!
		adcAttachPin(_pin);
		adcStart(_pin);
		ArbitrationToken = _pin; //take ownership of the token by setting it to your pin
		_adcSamplesTotal = 0;
		_adcCurrentSamples = 0;
		return; //no reason to continue, check for completion on the next loop
	}
	else if (ArbitrationToken == _pin) { //you must have ownershiup and the adc is done.  This really could have been just an else statement, but for completeness...
		
		_adcSamplesTotal += adcEnd(_pin); //get the adc results and add them to a total for averaging
		_adcCurrentSamples++; //increment the number of samples taken for averaging

		if (_adcCurrentSamples >= NumOfSamples) { //you have taken the requested number of samples, take the average and release arbitration
			adcRaw = _adcSamplesTotal / _adcCurrentSamples; //take the average
			ArbitrationToken = 0; //release arbitration
			newValueFlag = true;
		}
		else { //you have more samples to take, start the adc again
			adcStart(_pin);
		}
	}

}