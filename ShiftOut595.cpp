/*
 * ShiftOut595.cpp
 *
 * Created: 5/28/2014 7:12:43 AM
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

#include "ShiftOut595.h"


ShiftOut595::ShiftOut595(int dataPin, int latchPin, int clockPin, int outputEnablePin, int resetPin)
	:
	m_dataPin(dataPin),
	m_latchPin(latchPin),
	m_clockPin(clockPin),
	m_outputEnablePin(outputEnablePin),
	m_resetPin(resetPin),
	m_current(0)
{}

void ShiftOut595::init()
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

uint16_t ShiftOut595::allOff(bool latchIt)
{
	// clear everything out
	if(latchIt) digitalWrite(m_latchPin, LOW);
	for(int i = 0; i < 16; i++)
	{
		digitalWrite(m_dataPin, LOW);
		digitalWrite(m_clockPin, HIGH);
		delay(1);
		digitalWrite(m_clockPin, LOW);
	}
	if(latchIt) 
	{
		digitalWrite(m_latchPin, HIGH);
		m_current = 0;
	}
	return m_current;
}
void ShiftOut595::write(uint8_t val, uint8_t bitOrder)
{
	digitalWrite(m_latchPin, LOW);
	for (uint8_t  i = 0; i < 8; i++)  
	{
		if (bitOrder == LSBFIRST)
		{
			digitalWrite(m_dataPin, (val & (1 << i)));
		}
		else
		{
			digitalWrite(m_dataPin, (val & (1 << (7 - i))));
		}
		digitalWrite(m_clockPin, HIGH);
		//delay(1);
		digitalWrite(m_clockPin, LOW);
	}
	digitalWrite(m_latchPin, HIGH);
}


// Tested OK
void ShiftOut595::write16(uint16_t val, uint8_t bitOrder)
{
	m_current = 0;
	digitalWrite(m_latchPin, LOW);
	for (uint8_t  i = 0; i < 16; i++)
	{
		uint16_t bitVal = (LSBFIRST == bitOrder) 
			?
			val & (1 << i)
			:
			val & (1 << (15 - i));
		digitalWrite(m_dataPin, bitVal ? HIGH : LOW);
		m_current |= bitVal;
					
		digitalWrite(m_clockPin, HIGH);
		delay(1);
		digitalWrite(m_clockPin, LOW);
	}
	digitalWrite(m_latchPin, HIGH);
}

// Tested OK
void ShiftOut595::writePin(uint8_t pin, bool on)
{
	digitalWrite(m_latchPin, LOW);
	for (uint8_t  i = 0; i < 16; i++)
	{
		if(i == pin)
		{
			if(on)
			{
				digitalWrite(m_dataPin, HIGH);
				m_current |= (1 << pin);
			}
			else
			{
				digitalWrite(m_dataPin, LOW);
				m_current &= ~(1 << pin);
			}
		}
		else
		{
			if(m_current & (1 << i))
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
	digitalWrite(m_latchPin, HIGH);
}

