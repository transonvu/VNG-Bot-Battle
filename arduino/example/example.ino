/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mbed.h"
#include "ble/BLE.h"
#include "ble/services/UARTService.h"
#include "Serial.h"

DigitalOut  led1(p7);
DigitalOut  Relay_1A(p28);
DigitalOut  Relay_1B(p25);
DigitalOut  Relay_2A(p24);
DigitalOut  Relay_2B(p23);
DigitalOut  Relay_3A(p22);
DigitalOut  Relay_3B(p21);
DigitalOut  Relay_4A(p9);
DigitalOut  Relay_4B(p16);
DigitalOut  Relay_1(p17);
DigitalOut  Relay_2(p18);
DigitalOut  Relay_3(p19);
DigitalOut  Relay_4(p20);

mbed::Serial pc(p10, p11);
BLEDevice   ble;
UARTService *uartServicePtr;

//// Other
#define     ON          0
#define     OFF         1
uint8_t g_cmd;
////////////////////////////////////////////////////////////////////
void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *params)
{
    ble.startAdvertising();
}

void onDataWritten(const GattWriteCallbackParams *params)
{
    if ((uartServicePtr != NULL) && (params->handle == uartServicePtr->getTXCharacteristicHandle())) {
        uint16_t bytesRead = params->len;
        g_cmd = params->data[0];
        ble.updateCharacteristicValue(uartServicePtr->getRXCharacteristicHandle(), params->data, bytesRead);
    }
}

/////////////////////////////////////////////////////////////////////
void periodicCallback(void)
{
    led1 = !led1;
}
//////////////////////////////////////////////////////////////////////
void up(void)       //su dung motor 1 & 2
{
    Relay_1A = 0;
    Relay_1B = 1;
    Relay_2A = 0;
    Relay_2B = 1;
}
void down(void)     //su dung motor 1 & 2
{
    Relay_1A = 1;
    Relay_1B = 0;
    Relay_2A = 1;
    Relay_2B = 0;
}
void left(void)     //su dung motor 1 & 2
{
    Relay_1A = 0;
    Relay_1B = 1;
    Relay_2A = 1;
    Relay_2B = 0;
}
void right(void)    //su dung motor 1 & 2
{
    Relay_1A = 1;
    Relay_1B = 0;
    Relay_2A = 0;
    Relay_2B = 1;
}
void stop(void)    //su dung motor 1 & 2
{
    Relay_1A = 1;
    Relay_1B = 1;
    Relay_2A = 1;
    Relay_2B = 1;
}
/////////////////////////////////////////
void motor3_up(void)
{
    Relay_3A = 0;
    Relay_3B = 1;
}
void motor3_down(void)
{
    Relay_3A = 1;
    Relay_3B = 0;
}
void motor3_stop(void)
{
    Relay_3A = 1;
    Relay_3B = 1;
}
/////////////////////////////////////////
void motor4_up(void)
{
    Relay_4A = 0;
    Relay_4B = 1;
}
void motor4_down(void)
{
    Relay_4A = 1;
    Relay_4B = 0;
}
void motor4_stop(void)
{
    Relay_4A = 1;
    Relay_4B = 1;
}
//////////////////////////////////////////////////////////////////////
void test_relay(void)
{
    Relay_1A = 0; wait(1); Relay_1A = 1; wait(2);
    Relay_1B = 0; wait(1); Relay_1B = 1; wait(2);
    Relay_2A = 0; wait(1); Relay_2A = 1; wait(2);
    Relay_2B = 0; wait(1); Relay_2B = 1; wait(2);
    Relay_3A = 0; wait(1); Relay_3A = 1; wait(2);
    Relay_3B = 0; wait(1); Relay_3B = 1; wait(2);
    Relay_4A = 0; wait(1); Relay_4A = 1; wait(2);
    Relay_4B = 0; wait(1); Relay_4B = 1; wait(2);
    Relay_1 = 0;  wait(1); Relay_1 = 1;  wait(2);
    Relay_2 = 0;  wait(1); Relay_2 = 1;  wait(2);
    Relay_3 = 0;  wait(1); Relay_3 = 1;  wait(2);
    Relay_4 = 0;  wait(1); Relay_4 = 1;  wait(2);

    Relay_1 = 0;
    Relay_2 = 0;
    Relay_3 = 0;
    Relay_4 = 0;
    Relay_1A = 1; Relay_1B = 0;
    Relay_2A = 1; Relay_2B = 0;
    Relay_3A = 1; Relay_3B = 0;
    Relay_4A = 1; Relay_4B = 0;
    wait(0.5);
    Relay_1 = 1;
    Relay_2 = 1;
    Relay_3 = 1;
    Relay_4 = 1;
    Relay_1A = 0; Relay_1B = 1;
    Relay_2A = 0; Relay_2B = 1;
    Relay_3A = 0; Relay_3B = 1;
    Relay_4A = 0; Relay_4B = 1;
    wait(2);
}

//////////////////////////////////////////////////////////////////////
int main(void)
{
    //Init hardware
    led1 = OFF;
    Relay_1A = 1;
    Relay_1B = 1;
    Relay_2A = 1;
    Relay_2B = 1;
    Relay_3A = 1;
    Relay_3B = 1;
    Relay_4A = 1;
    Relay_4B = 1;
    Relay_1 = 1;
    Relay_2 = 1;
    Relay_3 = 1;
    Relay_4 = 1;
    //Init UART
    pc.baud(115200);
    pc.printf("\n\r# BOT_BATTLE\n\r");
    //Init timer
    Ticker ticker;
    ticker.attach(periodicCallback, 0.1); //sec
    //Init BLE
    ble.init();
    ble.onDisconnection(disconnectionCallback);
    ble.onDataWritten(onDataWritten);
    /* setup advertising */
    ble.accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED);
    ble.setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    ble.accumulateAdvertisingPayload(GapAdvertisingData::SHORTENED_LOCAL_NAME,
                                     (const uint8_t *)"BOT_BATTLE", sizeof("BOT_BATTLE") - 1);
    ble.accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_128BIT_SERVICE_IDS,
                                     (const uint8_t *)UARTServiceUUID_reversed, sizeof(UARTServiceUUID_reversed));
    ble.setAdvertisingInterval(200); /* 1000ms; in multiples of 0.625ms. */
    ble.startAdvertising();
    UARTService uartService(ble);
    uartServicePtr = &uartService;

  while (true) {
        ble.waitForEvent();
        //test_relay();

        if     (g_cmd == 1) {up();      }
        else if(g_cmd == 2) {left();    }
        else if(g_cmd == 3) {down();    }
        else if(g_cmd == 4) {right();   }
        else if(g_cmd == 5) {stop();    }

        else if(g_cmd == 6) {motor3_up();    }
        else if(g_cmd == 7) {motor3_down();  }
        else if(g_cmd == 8) {motor3_stop();  }
        else if(g_cmd == 9)  {motor4_up();   }
        else if(g_cmd == 10) {motor4_down(); }
        else if(g_cmd == 11) {motor4_stop(); }

        else if(g_cmd == 12) {Relay_1 = ON;  }
        else if(g_cmd == 13) {Relay_1 = OFF; }
        else if(g_cmd == 14) {Relay_2 = ON;  }
        else if(g_cmd == 15) {Relay_2 = OFF; }
        else if(g_cmd == 16) {Relay_3 = ON;  }
        else if(g_cmd == 17) {Relay_3 = OFF; }
        else if(g_cmd == 18) {Relay_4 = ON;  }
        else if(g_cmd == 19) {Relay_4 = OFF; }

        g_cmd = 0;
  }
}

