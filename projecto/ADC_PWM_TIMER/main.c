#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include "F2806x_EPwm_defines.h"
#include "pheripherals.h"


//Main
void main(void)
{
//
// Step 1. Initialize System Control:
// PLL, WatchDog, enable all Peripheral Clocks TODO:Enable only the ones we use
//
    InitSysCtrl();


//
// Step 2. Initalize GPIO:
//
    // Current measurement:
    // ADCINA1 ->SOC0 ->PIN29, ADCINB1 ->SOC1 ->PIN23,
    //
    // Resolver measurement:
    // ADCINA2 ->SOC2 ->PIN17, ADCINB2 ->SOC3 ->PIN24,
    //
    // Voltage measurement:
    // ADCINA4 ->SOC4 ->PIN16:
    InitAdcAio();   //not necessary maybe


    // Init GPIO pins for ePWM1
    InitEPwm1Gpio();


    //Init eCAP to catch ePWM
    InitECap1Gpio(); //eCAP1 is on GPIO19
//
// Step 3. Clear all interrupts and initialize PIE vector table:
// Disable CPU interrupts
//
    DINT;


    // Initialize the PIE control registers to their default state. The default state is all PIE interrupts disabled and flags are cleared.
    InitPieCtrl();

    // Disable CPU interrupts and clear all CPU interrupt flags
    IER = 0x0000;
    IFR = 0x0000;


    // Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    // This will populate the entire table, even if the interrupt is not used in this example.
    InitPieVectTable();
    ConfigPieVectTable(); //TODO: The section below should be moved into this function


    // Interrupts that are used are re-mapped to
    // ISR functions found within this file.
    EALLOW;// This is needed to write to EALLOW protected registers

    PieVectTable.ADCINT1 = &adc1_isr;       //First the ADC1

    PieVectTable.EPWM1_INT = &epwm1_isr;    //Second the ePWM1

    PieVectTable.TINT0 = &cpu_timer0_isr;   //Third the CPU Timers (by Mate 2018.09.16.)
    PieVectTable.TINT1 = &cpu_timer1_isr;
    PieVectTable.TINT2 = &cpu_timer2_isr;

    PieVectTable.ECAP1_INT = &ecap1_isr;    //Fourth eCAP (by Mate 2018.09.16.)

    EDIS;                                   // This is needed to disable write to EALLOW protected registers

//
// Step 4. Initialize all the Device Peripherals:
//

    // Initialize the ADCs
    InitAdc();          // General initialization
    AdcOffsetSelfCal(); // Offset compensation
    Adc_Config();       // General settings
    Adc1_Config();      // Current measurement
    Adc2_Config();      // Resolver measurement
    Adc4_Config();      // Voltage measurement


    // Initialize the ePWMs
    EPwm1_Config();


    //Initialize the CPUTimers
    InitCpuTimers();
    // Configure CPU-Timer 0, 1, and 2 to interrupt every second:
    // 80MHz CPU Freq, 1 second Period (in uSeconds)
    ConfigCpuTimer(&CpuTimer0, 80, 110); //Interrupt 0.11ms
    ConfigCpuTimer(&CpuTimer1, 80, 200); //Interrupt 0.2ms
    ConfigCpuTimer(&CpuTimer2, 80, 200); //Interrupt 0.2ms


    // Initialize the eCAP
    // InitPeripherals();
    InitEPwmTimer();
    InitECapture();

// Step 5. User specific code, enable interrupts
//

    // Enable CPU INT1 which is connected to ADC1 INT:
    IER |= M_INT1;                      // Enable CPU Interrupt 1


    // Enable CPU INT3 which is connected to EPWM1-3 INT:
    IER |= M_INT3;                      // Enable CPU Interrupt 3
    EINT;                               // Enable Global interrupt INTM
    ERTM;                               // Enable Global realtime interrupt DBGM


    // Enable CPU INT12, INT13 and INT14 for CPU TIMERS
    IER |= M_INT12; //Timer0
    IER |= M_INT13; //Timer1
    IER |= M_INT14; //Timer2


    // Enable CPU INT4 which is connected to ECAP1-4 INT
    IER |= M_INT4;

    // Enable ADC INTn in the PIE: Group 1 interrupt 1
    PieCtrlRegs.PIEIER1.bit.INTx1 = 1;


    // Enable EPWM INTn in the PIE: Group 3 interrupt 1-3
    PieCtrlRegs.PIEIER3.bit.INTx1 = 1;
    PieCtrlRegs.PIEIER3.bit.INTx2 = 1;
    PieCtrlRegs.PIEIER3.bit.INTx3 = 1;


    // Enable TIMER TINT0 in the PIE: Group 1 interrupt 7
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;


    // Enable eCAP INTn in the PIE: Group 3 interrupt 1-6
    PieCtrlRegs.PIEIER4.bit.INTx1 = 1;


    // Enable global Interrupts and higher priority real-time debug events
    EINT;   // Enable Global interrupt INTM
    ERTM;   // Enable Global realtime interrupt DBGM

// Step 6. IDLE loop. Just sit and loop forever (optional)
//

    for(;;)
    {
        __asm("          NOP");
    }
}
