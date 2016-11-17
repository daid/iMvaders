#include "fastio.h"
#include "Serial.h"
#include "pn532.h"

#define ID 0

#define SETUP_BUTTON_PIN(nr) do { SET_INPUT(nr); WRITE(nr, 1); } while(0)
#define SETUP_LED_PIN(nr) do { SET_OUTPUT(nr); WRITE(nr, 1); } while(0)

#define LED_PIN_1 7
#define LED_PIN_2 5
#define LED_PIN_3 2
#define LED_PIN_4 14
#define LED_PIN_5 16
#define LED_PIN_6 18
#define LED_PIN_7 19

#define BUTTON_1_PIN 62
#define BUTTON_2_PIN 65
#define BUTTON_3_PIN 68
#define BUTTON_4_PIN 49
#define BUTTON_5_PIN 43
#define BUTTON_6_PIN 37

#define START_BUTTON_PIN 31

void writePacket(uint8_t id, uint8_t data)
{
    Serial.write(uint8_t(0x5F));
    Serial.write(id);
    Serial.write(data);
    Serial.write(id ^ data);
}

int main()
{
    uint16_t counter = 0;
    uint8_t uid[7];
    uint8_t uid_len;
    
    pn532Init();
    Serial.begin(115200);
    sei();
    SET_OUTPUT(13);

    SETUP_BUTTON_PIN(54);
    SETUP_BUTTON_PIN(56);
    SETUP_BUTTON_PIN(58);
    SETUP_BUTTON_PIN(60);
    
    SETUP_BUTTON_PIN(BUTTON_1_PIN);
    SETUP_BUTTON_PIN(BUTTON_2_PIN);
    SETUP_BUTTON_PIN(BUTTON_3_PIN);
    SETUP_BUTTON_PIN(BUTTON_4_PIN);
    SETUP_BUTTON_PIN(BUTTON_5_PIN);
    SETUP_BUTTON_PIN(BUTTON_6_PIN);
    
    SETUP_BUTTON_PIN(START_BUTTON_PIN);
    SETUP_BUTTON_PIN(25);
    
    SETUP_LED_PIN(LED_PIN_1);
    SETUP_LED_PIN(LED_PIN_2);
    SETUP_LED_PIN(LED_PIN_3);
    SETUP_LED_PIN(LED_PIN_4);
    SETUP_LED_PIN(LED_PIN_5);
    SETUP_LED_PIN(LED_PIN_6);
    SETUP_LED_PIN(LED_PIN_7);

    while(1)
    {
        uint8_t buttons = 0;
        if (!READ(54)) buttons |= 0x01;
        if (!READ(56)) buttons |= 0x02;
        if (!READ(58)) buttons |= 0x04;
        if (!READ(60)) buttons |= 0x08;
        
        writePacket(0x00, (ID == 0) ? buttons : 0);
        writePacket(0x02, (ID == 1) ? buttons : 0);

        buttons = 0;
        if (!READ(BUTTON_1_PIN)) buttons |= 0x01;
        if (!READ(BUTTON_2_PIN)) buttons |= 0x02;
        if (!READ(BUTTON_3_PIN)) buttons |= 0x04;
        if (!READ(BUTTON_4_PIN)) buttons |= 0x08;
        if (!READ(BUTTON_5_PIN)) buttons |= 0x10;
        if (!READ(BUTTON_6_PIN)) buttons |= 0x20;
        if (!READ(START_BUTTON_PIN)) buttons |= 0x40;
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

        writePacket(0x01, (ID == 0) ? buttons : 0);
        writePacket(0x03, (ID == 1) ? buttons : 0);
        
        if (pn532readUid(uid, &uid_len))
        {
            for(uint8_t n=0; n<uid_len; n++)
            {
                writePacket(0x10, uid[n]);
            }
            writePacket(0x11, ID);
        }
        
        while(Serial.available())
        {
            uint8_t data = Serial.read();
            static uint8_t read_state = 0;
            static uint8_t read_command;
            static uint8_t read_data;
            switch(read_state)
            {
            case 0:
                if (data == 0x5F)
                {
                    WRITE(13, 1);
                    read_state = 1;
                }
                break;
            case 1:
                read_command = data;
                read_state = 2;
                break;
            case 2:
                read_data = data;
                read_state = 3;
                break;
            case 3:
                read_state = 0;
                if ((read_command ^ read_data) == data)
                {
                    switch(read_command)
                    {
                    case 0x01:
                        if (read_data & 0x01) WRITE(LED_PIN_1, 1); else WRITE(LED_PIN_1, 0); 
                        if (read_data & 0x02) WRITE(LED_PIN_2, 1); else WRITE(LED_PIN_2, 0); 
                        if (read_data & 0x04) WRITE(LED_PIN_3, 1); else WRITE(LED_PIN_3, 0); 
                        if (read_data & 0x08) WRITE(LED_PIN_4, 1); else WRITE(LED_PIN_4, 0); 
                        if (read_data & 0x10) WRITE(LED_PIN_5, 1); else WRITE(LED_PIN_5, 0); 
                        if (read_data & 0x20) WRITE(LED_PIN_6, 1); else WRITE(LED_PIN_6, 0); 
                        if (read_data & 0x40) WRITE(LED_PIN_7, 1); else WRITE(LED_PIN_7, 0); 
                        break;
                    }
                    break;
                }
            }
        }
    }
}
