/*
 * ShiftOut595.h
 *
 * Created: 5/28/2014 7:15:56 AM
 *  Author: Ketil Wright
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */



/*
 *	595 pinout
 *		
 *			------\/------
 *		Qb  | 1        16| Vcc
 *		Qc	| 2        15| Qa
 *		Qd	| 3        14| data
 *		Qe	| 4        13| output enable (OE active low)
 *		Qf	| 5        12| latch (RCLK)
 *		Qg	| 6        11| clock
 *		Qh	| 7        10| reset active low (empties Shift register if pulled low)
 *		GND	| 8         9| overflow (serial out)
 *			--------------
 *
 *	Minimal connections:	
 *   1) data  from a MCU digital output
 *	 2) latch from a MCU digital output
 *   3) clock from a MCU digital output
 *   4) output enable to GND (active low): eg output is always available on Qa -> Qh
 *   5) reset to Vcc: eg never in reset
 *   6) Qa -> Qh -> parallel outputs
 *  
 *	Further control is available if output and reset are connected.
 */

#pragma once
#include <stdint.h>
#include <Arduino.h>

#define noConnect -1

template<uint8_t numChips>
class ShiftOutMulti595
{
	const int m_dataPin;
	const int m_latchPin;
	const int m_clockPin;
	const int m_outputEnablePin;
	const int m_resetPin;
	uint8_t  m_current[numChips];
	
	typedef uint8_t currentState[numChips];
public:	
	ShiftOutMulti595(int dataPin,
		int latchPin,
		int clockPin,
		int outputEnablePin = noConnect,
		int resetPin = noConnect)
		:
			m_dataPin(dataPin),
			m_latchPin(latchPin),
			m_clockPin(clockPin),
			m_outputEnablePin(outputEnablePin),
			m_resetPin(resetPin)
	{
		for(int i = 0; i < numChips; i++) m_current[i] = 0;
	}
	void init();
	void write(uint8_t  values[numChips], uint8_t bitOrder);
	void writePin(uint32_t pin, bool on);
	currentState const& allOff(bool latchIt);
	currentState & state() const { return m_current;}
};

template<uint8_t numChips>
inline void ShiftOutMulti595<numChips>::init()
{
	pinMode(m_dataPin, OUTPUT);
	digitalWrite(m_dataPin, LOW);
	pinMode(m_latchPin, OUTPUT);
	digitalWrite(m_latchPin, LOW);
	pinMode(m_clockPin, OUTPUT);
	digitalWrite(m_clockPin, LOW);
	if(noConnect != m_outputEnablePin)
	{
		pinMode(m_outputEnablePin, OUTPUT);
		digitalWrite(m_outputEnablePin, HIGH); // turn off output (active low)
	}
	if(noConnect != m_resetPin)
	{
		pinMode(m_resetPin, OUTPUT);
		digitalWrite(m_resetPin, HIGH); // reset everything
		delay(10);
		digitalWrite(m_resetPin, LOW); // reset everything
		
		delay(10);
		digitalWrite(m_resetPin, HIGH); // reset everything
		
	}
	// clear everything out
	allOff(true);
	if(noConnect != m_outputEnablePin)
	{
		pinMode(m_outputEnablePin, OUTPUT);
		digitalWrite(m_outputEnablePin, LOW); // turn on output (active low)
	}
}
template<uint8_t numChips>
inline void ShiftOutMulti595<numChips>::write(uint8_t  values[numChips], uint8_t bitOrder)
{
	m_current = 0;
	digitalWrite(m_latchPin, LOW);
	
	for(int part = 0; part < numChips; part++)
	{
		uint8_t & val = values[part];
		for (uint8_t  i = 0; i < 8; i++)
		{
			bool setIt = false;
			
			uint16_t bitVal = (LSBFIRST == bitOrder)
			?
			val & (1 << i)
			:
			val & (1 << (15 - i));
			digitalWrite(m_dataPin, bitVal ? HIGH : LOW);
			m_current[part] |= bitVal;
			
			digitalWrite(m_clockPin, HIGH);
			delay(1);
			digitalWrite(m_clockPin, LOW);
		}
	}
	digitalWrite(m_latchPin, HIGH);

}
template<uint8_t numChips>
inline void ShiftOutMulti595<numChips>::writePin(uint32_t pin, bool on)
{
	if(pin > (numChips * 8)) return;
	digitalWrite(m_latchPin, LOW);
	
	for(int part = 0; part < numChips; part++)
	{
		for (uint8_t  i = 0; i < 8; i++)
		{
			if(((8* part) + i) == pin)
			{
				if(on)
				{
					digitalWrite(m_dataPin, HIGH);
					m_current[part] |= (1 << (pin - part * 8));
				}
				else
				{
					digitalWrite(m_dataPin, LOW);
					m_current[part] &= ~(1 << (pin - part * 8));
				}
			}
			else
			{
				if(m_current[part] & (1 << i))
				{
					digitalWrite(m_dataPin, HIGH);
				}
				else
				{
					digitalWrite(m_dataPin, LOW);
				}
			}
			digitalWrite(m_clockPin, HIGH);
			//delay(1);
			// delayMicroseconds(10);
			digitalWrite(m_clockPin, LOW);
		}
	}
	digitalWrite(m_latchPin, HIGH);
	//delay(75);
}


template<uint8_t numChips>
inline typename ShiftOutMulti595<numChips>::currentState const& ShiftOutMulti595<numChips>::allOff(bool latchIt)
{
	// clear everything out
	if(latchIt) digitalWrite(m_latchPin, LOW);
	for(int part = 0; part < numChips; part++)
	{
		for(int i = 0; i < 8; i++)
		{
			digitalWrite(m_dataPin, LOW);
			digitalWrite(m_clockPin, HIGH);
			delay(1);
			digitalWrite(m_clockPin, LOW);
		}
	}
	
	if(latchIt)
	{
		digitalWrite(m_latchPin, HIGH);
		//m_current = 0;
		memset(m_current, 0, sizeof(m_current));
	}
	return m_current;
}
