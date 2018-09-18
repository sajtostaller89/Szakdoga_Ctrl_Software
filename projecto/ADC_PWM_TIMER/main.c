#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include "F2806x_EPwm_defines.h"
#include "pheripherals.h"

//TIMER (by Mate 2018.09.16.)
__interrupt void cpu_timer0_isr(void);
__interrupt void cpu_timer1_isr(void);
__interrupt void cpu_timer2_isr(void);

//eCAP (by Mate 2018.09.16.)
__interrupt void ecap1_isr(void);
void InitECapture(void);
void InitEPwmTimer(void);
void Fail(void);

//
// Globals
//

//eCAP-hoz kellenek (by Mate 2018.09.16.)----------------------------------------------------------------------
Uint32  ECap1IntCount;
Uint32  ECap1PassCount;
Uint32  EPwm1TimerDirection; //itt valaszd ki melyik timert akarod captureolni!!!!!!!



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
    // ADCINA6 ->SOC0 ->GPIO6, ADCINA7 ->SOC1 ->GPIO7,
    //
    // Resolver measurement:
    // ADCINB0 ->SOC2 ->GPIO8, ADCINB1 ->SOC3 ->GPIO9,
    //
    // Voltage measurement:
    // ADCINB2 ->SOC4 ->GPIO10:
    InitAdcAio();


    // For this case just init GPIO pins for ePWM1
    InitEPwm1Gpio();

    //Init eCAP to catch ePWM (by Mate 2018.09.16.)
    InitECap1Gpio(); //eCAP1 is on GPIO19 szal ,ha ezzel akarod elkapni a PWMet akkor ossze kell kotnod a GPIO19-et az adott ePWM pinjevel
//
// Step 3. Clear all interrupts and initialize PIE vector table:
// Disable CPU interrupts
//
    DINT;


    // Initialize the PIE control registers to their default state.
    // The default state is all PIE interrupts disabled and flags
    // are cleared.
    InitPieCtrl();


    // Disable CPU interrupts and clear all CPU interrupt flags
    IER = 0x0000;
    IFR = 0x0000;


    // Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    // This will populate the entire table, even if the interrupt
    // is not used in this example.  This is useful for debug purposes.
    InitPieVectTable();


    // Interrupts that are used are re-mapped to
    // ISR functions found within this file.
    EALLOW;// This is needed to write to EALLOW protected registers

    PieVectTable.ADCINT1 = &adc1_isr;       //First the ADC1

    PieVectTable.EPWM1_INT = &epwm1_isr;    //Second the ePWM1

    PieVectTable.TINT0 = &cpu_timer0_isr;   //Third the cPU Timers (by Mate 2018.09.16.)
    PieVectTable.TINT1 = &cpu_timer1_isr;
    PieVectTable.TINT2 = &cpu_timer2_isr;

    PieVectTable.ECAP1_INT = &ecap1_isr;    //Fourth eCAP (by Mate 2018.09.16.)

    EDIS;                                   // This is needed to disable write to EALLOW protected registers

//
// Step 4. Initialize all the Device Peripherals:
//

    // Initialize the ADCs
    InitAdc();
    AdcOffsetSelfCal();
    Adc1_Config();


    // Initialize the ePWMs
    EPwm1_Config();

    //Init the eCAP (by Mate 2018.09.16.)---------------------------------------------------------
    // InitPeripherals();  // Not required for this example
        InitEPwmTimer();    // For this example, only initialize the ePWM Timers
        InitECapture();
        ECap1IntCount = 0;
        ECap1PassCount = 0;

    // Initialize the CPUTimers (by Mate 2018.09.16.)---------------------------------------------

        //Debughoz:
        //This example configures CPU Timer0, 1, and 2 and increments
        //! a counter each time the timer asserts an interrupt.
        //! \b Watch \b Variables \n
        //! - CpuTimer0.InterruptCount
        //! - CpuTimer1.InterruptCount
        //! - CpuTimer2.InterruptCount
        // Step 4. Initialize the Device Peripheral. This function can be
        //         found in F2806x_CpuTimers.c
        //
        InitCpuTimers();   // For this example, only initialize the Cpu Timers

        // Configure CPU-Timer 0, 1, and 2 to interrupt every second:
        // 80MHz CPU Freq, 1 second Period (in uSeconds)
        // Config CPU Timers
        ConfigCpuTimer(&CpuTimer0, 80, 110); //Akosnak interrupt 0.11ms- nkent
        ConfigCpuTimer(&CpuTimer1, 80, 200); //Benedeknek és nekem 0,2ms-enkent kell interrupt (5kHz jelado) egyik negyszog
        ConfigCpuTimer(&CpuTimer2, 80, 200); //Benedeknek és nekem 0,2ms-enkent kell interrupt (5kHz jelado) masik negyszog
//--------------------------------------------------------------------------------------------------

//Initialize eCAP to capture PWM (by Mate 2018.09.16.)-------------------------------------------------------------------
        //Debughoz:
        //\b Watch \b Variables \n
        //! - \b ECap1PassCount , Successful captures
        //! - \b ECap1IntCount , Interrupt counts



//--------------------------------------------------------------------------------------------------
// Step 5. User specific code, enable interrupts
//

    // Enable CPU INT1 which is connected to ADC1 INT:
    IER |= M_INT1;                      // Enable CPU Interrupt 1


    // Enable CPU INT3 which is connected to EPWM1-3 INT:
    IER |= M_INT3;                      // Enable CPU Interrupt 3
    EINT;                               // Enable Global interrupt INTM
    ERTM;                               // Enable Global realtime interrupt DBGM

    // Enable CPU INT4 which is connected to ECAP1-4 INT
    IER |= M_INT4;

    // Enable CPU INT12, INT13 and INT14 for CPU TIMERS
    IER |= M_INT12; //Timer0
    IER |= M_INT13; //Timer1
    IER |= M_INT14; //Timer2

    // Enable ADC INTn in the PIE: Group 1 interrupt 1
    PieCtrlRegs.PIEIER1.bit.INTx1 = 1;


    // Enable EPWM INTn in the PIE: Group 3 interrupt 1-3
    PieCtrlRegs.PIEIER3.bit.INTx1 = 1;
    PieCtrlRegs.PIEIER3.bit.INTx2 = 1;
    PieCtrlRegs.PIEIER3.bit.INTx3 = 1;

    // Enable eCAP INTn in the PIE: Group 3 interrupt 1-6
    PieCtrlRegs.PIEIER4.bit.INTx1 = 1;

    // Enable TIMER TINT0 in the PIE: Group 1 interrupt 7
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;


    // Enable global Interrupts and higher priority real-time debug events
    EINT;   // Enable Global interrupt INTM
    ERTM;   // Enable Global realtime interrupt DBGM

//
// Step 6. IDLE loop. Just sit and loop forever (optional)
//
    for(;;)
    {
        __asm("          NOP");
    }
}

//CPU Timer function call (by Mate 2018.09.16.)---------------------------------------------------------------
__interrupt void
cpu_timer0_isr(void)
{
    CpuTimer0.InterruptCount++;

    //
    // Acknowledge this interrupt to receive more interrupts from group 1
    //
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

//
// cpu_timer1_isr -
//
__interrupt void
cpu_timer1_isr(void)
{
    CpuTimer1.InterruptCount++;

    //
    // The CPU acknowledges the interrupt
    //
    EDIS;
}

//
// cpu_timer2_isr -
//
__interrupt void
cpu_timer2_isr(void)
{
    EALLOW;
    CpuTimer2.InterruptCount++;

    //
    // The CPU acknowledges the interrupt.
    //
    EDIS;
}
//----------------------------------------------------------------------------------------------


//eCAP function call ----------------------------------------------------------------------------------------
    // InitEPwmTimer -
    //Erre ra kellene nezni skacok mert nem vagom pontosan ,hogy ez most mi a loturo ,de mivel erre hivatkozik az eCAP enelkul nem fordul
    //ez most az a timer amire a PWM komparal vagy ez csak szamolja a PWM ciklust és erre jatszik ra az eCAP?
    void
    InitEPwmTimer()
    {
        EALLOW;
        SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
        EDIS;

        EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // Count up
        EPwm1Regs.TBPRD = PWM1_TIMER_MIN;
        EPwm1Regs.TBPHS.all = 0x00000000;
        EPwm1Regs.AQCTLA.bit.PRD = AQ_TOGGLE;      // Toggle on PRD

        //
        // TBCLK = SYSCLKOUT
        //
        EPwm1Regs.TBCTL.bit.HSPCLKDIV = 1;
        EPwm1Regs.TBCTL.bit.CLKDIV = 0;

        EPwm1TimerDirection = EPWM_TIMER_UP;

        EALLOW;
        SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
        EDIS;
    }

    //

    // InitECapture -
    //
    void
    InitECapture()
    {
        ECap1Regs.ECEINT.all = 0x0000;          // Disable all capture interrupts
        ECap1Regs.ECCLR.all = 0xFFFF;           // Clear all CAP interrupt flags
        ECap1Regs.ECCTL1.bit.CAPLDEN = 0;       // Disable CAP1-CAP4 register loads
        ECap1Regs.ECCTL2.bit.TSCTRSTOP = 0;     // Make sure the counter is stopped

        //
        // Configure peripheral registers
        //
        ECap1Regs.ECCTL2.bit.CONT_ONESHT = 1;   // One-shot
        ECap1Regs.ECCTL2.bit.STOP_WRAP = 3;     // Stop at 4 events
        ECap1Regs.ECCTL1.bit.CAP1POL = 1;       // Falling edge
        ECap1Regs.ECCTL1.bit.CAP2POL = 0;       // Rising edge
        ECap1Regs.ECCTL1.bit.CAP3POL = 1;       // Falling edge
        ECap1Regs.ECCTL1.bit.CAP4POL = 0;       // Rising edge
        ECap1Regs.ECCTL1.bit.CTRRST1 = 1;       // Difference operation
        ECap1Regs.ECCTL1.bit.CTRRST2 = 1;       // Difference operation
        ECap1Regs.ECCTL1.bit.CTRRST3 = 1;       // Difference operation
        ECap1Regs.ECCTL1.bit.CTRRST4 = 1;       // Difference operation
        ECap1Regs.ECCTL2.bit.SYNCI_EN = 1;      // Enable sync in
        ECap1Regs.ECCTL2.bit.SYNCO_SEL = 0;     // Pass through
        ECap1Regs.ECCTL1.bit.CAPLDEN = 1;       // Enable capture units

        ECap1Regs.ECCTL2.bit.TSCTRSTOP = 1;     // Start Counter
        ECap1Regs.ECCTL2.bit.REARM = 1;         // arm one-shot
        ECap1Regs.ECCTL1.bit.CAPLDEN = 1;       // Enable CAP1-CAP4 register loads
        ECap1Regs.ECEINT.bit.CEVT4 = 1;         // 4 events = interrupt
    }

    //
    // ecap1_isr -
    //
    __interrupt void
    ecap1_isr(void)
    {
        //
        // Cap input is syc'ed to SYSCLKOUT so there may be
        // a +/- 1 cycle variation
        //
        if(ECap1Regs.CAP2 > EPwm1Regs.TBPRD*2+1 ||
           ECap1Regs.CAP2 < EPwm1Regs.TBPRD*2-1)
        {
            Fail();
        }

        if(ECap1Regs.CAP3 > EPwm1Regs.TBPRD*2+1 ||
           ECap1Regs.CAP3 < EPwm1Regs.TBPRD*2-1)
        {
            Fail();
        }

        if(ECap1Regs.CAP4 > EPwm1Regs.TBPRD*2+1 ||
           ECap1Regs.CAP4 < EPwm1Regs.TBPRD*2-1)
        {
            Fail();
        }

        ECap1IntCount++;

        if(EPwm1TimerDirection == EPWM_TIMER_UP)
        {
            if(EPwm1Regs.TBPRD < PWM1_TIMER_MAX)
            {
                EPwm1Regs.TBPRD++;
            }
            else
            {
                EPwm1TimerDirection = EPWM_TIMER_DOWN;
                EPwm1Regs.TBPRD--;
            }
        }
        else
        {
            if(EPwm1Regs.TBPRD > PWM1_TIMER_MIN)
            {
                EPwm1Regs.TBPRD--;
            }
            else
            {
                EPwm1TimerDirection = EPWM_TIMER_UP;
                EPwm1Regs.TBPRD++;
            }
        }

        ECap1PassCount++;

        ECap1Regs.ECCLR.bit.CEVT4 = 1;
        ECap1Regs.ECCLR.bit.INT = 1;
        ECap1Regs.ECCTL2.bit.REARM = 1;

        //
        // Acknowledge this interrupt to receive more interrupts from group 4
        //
        PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;
    }

    //
    // Fail -
    //
    void Fail()
    {
        __asm("   ESTOP0");
    }


