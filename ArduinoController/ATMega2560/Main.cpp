/* -*- c++ -*- */

/*
    Reprap firmware based on Sprinter and grbl.
 Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 This firmware is a mashup between Sprinter and grbl.
  (https://github.com/kliment/Sprinter)
  (https://github.com/simen/grbl/tree)

 It has preliminary support for Matthew Roberts advance algorithm
    http://reprap.org/pipermail/reprap-dev/2011-May/003323.html
 */
#include "fastio.h"
#include "Serial.h"

//===========================================================================
//=============================imported variables============================
//===========================================================================


//===========================================================================
//=============================public variables=============================
//===========================================================================

//===========================================================================
//=============================private variables=============================
//===========================================================================

//===========================================================================
//=============================ROUTINES=============================
//===========================================================================

#define SETUP_BUTTON_PIN(nr) do { SET_INPUT(nr); WRITE(nr, 1); } while(0)
#define SETUP_LED_PIN(nr) do { SET_OUTPUT(nr); WRITE(nr, 1); } while(0)
int main()
{
    uint16_t counter = 0;
    
    Serial.begin(115200);
    SET_OUTPUT(13);

    SETUP_BUTTON_PIN(54);
    SETUP_BUTTON_PIN(56);
    SETUP_BUTTON_PIN(58);
    SETUP_BUTTON_PIN(60);
    SETUP_BUTTON_PIN(62);
    SETUP_BUTTON_PIN(65);
    SETUP_BUTTON_PIN(68);
    
    SETUP_BUTTON_PIN(49);
    SETUP_BUTTON_PIN(43);
    SETUP_BUTTON_PIN(37);
    SETUP_BUTTON_PIN(31);
    SETUP_BUTTON_PIN(25);
    
    SETUP_LED_PIN(7);
    SETUP_LED_PIN(5);
    SETUP_LED_PIN(2);
    SETUP_LED_PIN(14);
    SETUP_LED_PIN(16);
    SETUP_LED_PIN(18);
    SETUP_LED_PIN(20);

    while(1)
    {
        WRITE(13, 1);
        
        uint8_t buttons = 0;
        if (!READ(54)) buttons |= 0x01;
        if (!READ(56)) buttons |= 0x02;
        if (!READ(58)) buttons |= 0x04;
        if (!READ(60)) buttons |= 0x08;
        
        Serial.write(uint8_t(0x5F));
        Serial.write(uint8_t(0x00));
        Serial.write(uint8_t(buttons));

        WRITE(13, 0);
        buttons = 0;
        if (!READ(62)) buttons |= 0x01;
        if (!READ(65)) buttons |= 0x02;
        if (!READ(68)) buttons |= 0x04;
        if (!READ(49)) buttons |= 0x08;
        if (!READ(43)) buttons |= 0x10;
        if (!READ(37)) buttons |= 0x20;
        if (!READ(31)) buttons |= 0x40;
        if (buttons & 0x40)
        {
            counter++;
            if (counter > 2000)
            {
                buttons &=~0x40;
                buttons |= 0x80;
            }
        }else{
            counter = 0;
        }

        Serial.write(uint8_t(0x5F));
        Serial.write(uint8_t(0x01));
        Serial.write(uint8_t(buttons));
    }
}
