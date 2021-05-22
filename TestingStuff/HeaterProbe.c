#include "msp.h"
#include "ValveController.h"
#include "PumpController.h"
#include "HeaterController.h"
//#include "Thermocouple.h"

/**
 * Created on: May 15, 2021
 *      Author: Jacob Everest
 */
int HPCount;        //variable that records how many times the heater Probe has turned
int Done;           //Flag that tells our system when the whole thing is done
int AmountOfCycles; //Variable that stores the upper limit of total cycles we intend to go through
int CycleCount;     //Variable that counts the circulation cycles that we've gone through (water jet, recirculation to clear the suction hole, sucking out water)
int lion;           //Variable used for a "delay" function when recirculation water for the heater probe
int Temp;           //Variable used to store the data from the temperature sensor to determine how hot our cartridge heater is

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	//Initital code to be run when the probe is in the ground
	Heater1On();       //Start by turning the heater on to melt the ice
    PumpForward();     //Start pumping, can run dry so it isn't a problem
    Valve1Close();     //Close the outlet valve to begin with, just want it to circulate water in the water jet
    Valve2Open();      //Open the water jet valve
    Valve3Close();     //Close the valve that we use to dump out the system
    HPCount = 0;       //Reset the heater probe counter
    Done = 0;          //Start by making sure the system doesn't thing we're done
    AmountOfCycles = 1;//Change this to however many cycles we want to use
    CycleCount = 0;    //To begin with, we haven't gone through any cycles


	while(1)
	{
	    int cat;                          //Code for testing, to be replaced by recording turns of the motor in the interrupt that turns the motor
        for (cat=0; cat < 500000; cat++)  //
        {                                 //
        }                                 //
        HPCount = HPCount + 1;            //

//	    //Temperature module, must be run through first each time for safety (module refered to as "State 1")
//	    Temp = readvalue(temp reading pin); //Not the actual code for reading the thermocouple, just an assumption of the code and placeholder
//
//      Temp = MAX31855ReadTemperature();
//	    if (Temp > set value) //If temperature of the cartridge heater too high
//	    {
//	        Heater1Off();	//Turn off the cartridge heater
//	    }
//	    else //If temperature of the cartridge heater isn't too high
//	    {
//	        Heater1On();  //Turn on the cartridge heater
//	    }

	    if (HPCount < 3 && CycleCount < AmountOfCycles && Done == 0) //State 2 Water jet function
	    {
	        Valve1Close();
	        Valve2Open();
	        Heater2On();
	    }
	    else if (HPCount == 3 && CycleCount < AmountOfCycles && Done == 0) //recirc state. Base coming here on doing so a certain number of times
	        //when implemented in actual code. This should be done for a few minutes and then skipped to give time to sucking out water.
	    {
	        Valve1Close();
	        Valve2Open();
	        Heater2Off();
	        PumpBackward();
            for (lion = 0; lion < 100000; lion++)
            {
            }
	    }
        else if (HPCount == 4 && CycleCount < AmountOfCycles && Done == 0) //State 3 Extract Water
        {
            PumpForward();
            Valve1Open();
            Valve2Close();
            Heater2Off();
        }
	    else if (HPCount > 4 && HPCount < 7 && CycleCount < AmountOfCycles && Done == 0) //State 4 Water jet function
	    {
	        Valve1Close();
            Valve2Open();
            Heater2On();
	    }
	    else if (HPCount == 7 && CycleCount < AmountOfCycles && Done == 0) //Recirc state
	    {
            Valve1Close();
            Valve2Open();
            Heater2Off();
            PumpBackward();
            for (lion = 0; lion < 100000; lion++)
            {
            }
	    }
        else if (HPCount == 8 && CycleCount < AmountOfCycles && Done == 0) //State 5 Extract Water
        {
            PumpForward();
            Valve1Open();
            Valve2Close();
            Heater2Off();
            HPCount = 0;
            CycleCount += 1;
        }
	    else if (HPCount == 1 && CycleCount >= AmountOfCycles && Done == 0) //Recirc state
	    {
            Valve1Close();
            Valve2Open();
            Heater2Off();
            PumpBackward();
            for (lion = 0; lion < 100000; lion++)
            {
            }
	    }
        else if (HPCount < 4 && CycleCount >= AmountOfCycles && Done == 0) //State 6 Final suction state, Can adjust HPCount to make it longer or shorter
        {
            PumpForward();
            Valve1Open();
            Valve2Close();
            Heater2Off();
        }
	    else if (HPCount >= 4 && CycleCount >= AmountOfCycles && Done == 0) //State 7 clear out system
	    {
	        PumpBackward();
	        Valve1Close();
	        Valve2Close();
	        Valve3Open();
	        Heater1Off();
	        Done = 1;
	    }
	    else if (Done == 1) //State 8 break free from this cruel world
	    {
	        break;
	    }
	}
}
