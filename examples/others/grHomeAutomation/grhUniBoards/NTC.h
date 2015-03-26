#include <math.h>


/*
 * Inputs ADC Value from Thermistor and outputs Temperature in Celsius
 *  requires: include <math.h>
 * Utilizes the Steinhart-Hart Thermistor Equation:
 *    Temperature in Kelvin = 1 / {A + B[ln(R)] + C[ln(R)]3}
 *    where A = 0.001129148, B = 0.000234125 and C = 8.76741E-08
 *
 * These coefficients seem to work fairly universally, which is a bit of a 
 * surprise. 
 *
 * Schematic:
 *   [Ground] -- [10k-pad-resistor] -- | -- [thermistor] --[Vcc (5 or 3.3v)]
 *                                               |
 *                                          Analog Pin 0
 */

/*
class CNTC
{
	short m_sPin;
	float m_fPadResistor;
	float m_fTermResistor;

public:
	/*
	@uPin
	Analog pin used to read

	@fPadResistor 
	balance/pad resistor value as measured
	
	@fTermResistor
	thermistor nominal resistance
	*/
/*	CNTC(short sPin, float fPadResistor, float fTermResistor = 10000)
	{
		m_sPin = sPin;
		m_fPadResistor = fPadResistor;
		m_fTermResistor = fTermResistor;
	}

	float ReadTemperature()
	{
		long lRestistance = m_fPadResistor*((1024.0 / analogRead(m_sPin)) - 1);

		float fTemp = log(lRestistance); // Saving the Log(resistance) so not to calculate  it 4 times later
  		fTemp = 1 / (0.001129148 + (0.000234125 * fTemp) + (0.0000000876741 * fTemp * fTemp * fTemp));
  		return fTemp - 273.15;  // Convert Kelvin to Celsius            
	}

};
*/

inline float NTC_RawADCToCelsius(int iRawADC, float fPadResistor = 10000, float fTermResistor = 10000)
{
	long lRestistance = fPadResistor*((1024.0 / iRawADC) - 1);

	float fTemp = log(lRestistance); // Saving the Log(resistance) so not to calculate  it 4 times later
		fTemp = 1 / (0.001129148 + (0.000234125 * fTemp) + (0.0000000876741 * fTemp * fTemp * fTemp));
		return fTemp - 273.15;  // Convert Kelvin to Celsius            
}