/*
 * melting.c
 *
 *  Created on: Sep 11, 2021
 *      Author: skyry
 */
#include <stdio.h>
#include "ValveController.h"
#include "Thermocouple.h"
#include "PumpController.h"
#include "HeaterController.h"
#include "melting.h"
#include "delay.h"
#include "limitSwitch.h"
#include "motorDriver.h"
#include "augerRot.h"

void initialMelt() {//the initial melt stage that gathers a pool
    int i;
    Heater1On();
    delay_ms(180000, 24);//let it get hot - 180 seconds
    for (i = 0; i < 15; i++) {//10 is an experimental value
        setAugerRotSpeed(-1000);//clockwise from aerial view
        setAugerSpeed(-1000);//downwards
        delay_ms(1800, 24);//approx 0.2in downward
        if (getError()) {
            return;
        }
        setAugerRotSpeed(0);
        setAugerSpeed(0);
        delay_ms(12000, 24);//12 sec to melt ice
    }//overall heater is on for 30 + 20x10 = 230sec
    //Heater1Off();
    extraction();
}

void waterJet(uint32_t time) {//to shoot water out of the auger and expand melt pool
    //Valve1Close();
    Valve2Close();
    Valve3Open();
    Heater2On();
    PumpForward();
    delay_ms(time, 24);//How long to be in this stage for
}

void extraction(uint32_t time) {//to collect water for recirculation and for keeping
    //Valve1Close();
    Valve2Open();
    Valve3Close();//open to allow for addition flow to be routed back through, so no pressure buildup
    PumpForward();
    delay_ms(time, 24);//How long to be in this stage for
}

void filterDump(uint32_t time) {//to clear clogs that may form. Don't want to lose out on water, so use sparingly
    Valve1Open();
    Valve2Close();
    Valve3Close();
    PumpBackward();
    delay_ms(time, 24);
    PumpOff();
    Valve1Close();
}

int readTemp() {
    int temp = MAX31855ReadTemperature();

    //printf("%d\n", temp);
    return temp;
}
