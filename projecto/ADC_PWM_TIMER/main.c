#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include "F2806x_EPwm_defines.h"
#include "pheripherals.h"
#include "general.h"
#include "math.h"


//Main
void main(void)
{
// Step 1. Initialize System Control: PLL, WatchDog, enable all Peripheral Clocks TODO:Enable only the ones we use------------------------

    InitSysCtrl();


// Step 2. Initalize GPIO:----------------------------------------------------------------------------------------------------------------

    InitAdcAio();   //not necessary maybe

    // Init GPIO pins for ePWM1
    InitEPwm4Gpio();
    InitEPwm5Gpio();
    InitEPwm6Gpio();


// Step 3. Clear all interrupts and initialize PIE vector table:--------------------------------------------------------------------------

    // Disable CPU interrupts
    DINT;

    // Initialize the PIE control registers to their default state. The default state is all PIE interrupts disabled and flags are cleared.
    InitPieCtrl();

    // Disable CPU interrupts and clear all CPU interrupt flags
    IER = 0x0000;
    IFR = 0x0000;

    // Initialize the PIE vector table with pointers to the shell Interrupt Service Routines (ISR).
    // This will populate the entire table, even if the interrupt is not used in this example.
    InitPieVectTable();
    ConfigPieVectTable();


// Step 4. Initialize all the Device Peripherals:-----------------------------------------------------------------------------------------

    // Initialize the ADCs
    InitAdc();          // General initialization
    AdcOffsetSelfCal(); // Offset compensation
    Adc_Config();       // General settings
    Adc1_Config();      // Current measurement
    Adc2_Config();      // Resolver measurement
    Adc4_Config();      // Voltage measurement

    // Initialize an ePWM for the ADC2 triggering
    EPwm2_Config();     // Triggers adc2


    // Initialize the ePWMs for the phases
    EPwm4_Config();     // Triggers adc1 and adc4
    EPwm5_Config();
    EPwm6_Config();


// Step 5. User specific code, enable interrupts------------------------------------------------------------------------------------------

    ConfigEnableInterrupts();
    Field_Oriented_Motor_Control_initialize();


// Step 6. IDLE loop. Just sit and loop forever (optional)--------------------------------------------------------------------------------

    for(;;)
    {
        __asm("          NOP");
    }
}
