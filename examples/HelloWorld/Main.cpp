/*
 * Main.cpp
 *
 * Created: 5/28/2014 7:14:32 AM
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



#ifndef SKETCH_H_
#include "Sketch.h"
#endif

#include <ShiftOut595.h>

const int srData	= 8;
const int srOE		= 7;
const int srLatch	= 6;
const int srClock	= 5;
const int srSRCLR	= 3;

const uint8_t numChips = 3;
ShiftOutMulti595<numChips> shiftOut595(srData, srLatch, srClock, srOE, srSRCLR);
 void setup()
 {
	 
	 /* Initialize serial interface */
	 Serial.begin(SERIAL_SPEED);
	 shiftOut595.init();
 }
 void loop()
{
	int speed = analogRead(A4) / 5;	
	for(int pin = 0; pin < numChips * 8; pin++)
	{
		shiftOut595.writePin(pin, true);
		delay(speed);
		shiftOut595.writePin(pin, false);
	}
	for(int pin = numChips * 8 - 1; pin >=0 ; pin--)
	{
		shiftOut595.writePin(pin, true);
		delay(speed / 2);
		shiftOut595.writePin(pin, false);
	}
}