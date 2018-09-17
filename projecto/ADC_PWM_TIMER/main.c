#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include "F2806x_EPwm_defines.h"

//
// Typedefs
//
typedef struct
{
    volatile struct EPWM_REGS *EPwmRegHandle;
    Uint16 EPwm_CMPA_Direction;
    Uint16 EPwm_CMPB_Direction;
    Uint16 EPwmTimerIntCount;
    Uint16 EPwmMaxCMPA;
    Uint16 EPwmMinCMPA;
    Uint16 EPwmMaxCMPB;
    Uint16 EPwmMinCMPB;
} EPWM_INFO;

//
// Function Prototypes
//

void Adc_Config(void);
void InitEPwm1Example(void);
void InitEPwm2Example(void);
void InitEPwm3Example(void);

__interrupt void adc1_isr(void);
__interrupt void epwm1_isr(void);
__interrupt void epwm2_isr(void);
__interrupt void epwm3_isr(void);

//TIMER (by Mate 2018.09.16.)
__interrupt void cpu_timer0_isr(void);
__interrupt void cpu_timer1_isr(void);
__interrupt void cpu_timer2_isr(void);

//eCAP (by Mate 2018.09.16.)
__interrupt void ecap1_isr(void);
void InitECapture(void);
void InitEPwmTimer(void);
void Fail(void);

//Update compare
void update_compare(EPWM_INFO*);

//
// Globals
//
Uint16 ConversionCount;
Uint16 Voltage1[10];
Uint16 Voltage2[10];

EPWM_INFO epwm1_info;
EPWM_INFO epwm2_info;
EPWM_INFO epwm3_info;

//
// Defines that configure the period for each timer
//
#define EPWM1_TIMER_TBPRD  3000  // Period register
#define EPWM1_MAX_CMPA     2999 //holtido benne legyen 2 orajelciklusnyi
#define EPWM1_MIN_CMPA      1
#define EPWM1_MAX_CMPB     2999
#define EPWM1_MIN_CMPB      1

#define EPWM2_TIMER_TBPRD  2000  // Period register
#define EPWM2_MAX_CMPA     1950
#define EPWM2_MIN_CMPA       50
#define EPWM2_MAX_CMPB     1950
#define EPWM2_MIN_CMPB       50

#define EPWM3_TIMER_TBPRD  2000  // Period register
#define EPWM3_MAX_CMPA      950
#define EPWM3_MIN_CMPA       50
#define EPWM3_MAX_CMPB     1950
#define EPWM3_MIN_CMPB     1050

//Defines to keep track of which way the timer value is moving (by Mate 2018.09.16.)
#define EPWM_TIMER_UP   1
#define EPWM_TIMER_DOWN 0
#define PWM1_TIMER_MIN     10
#define PWM1_TIMER_MAX     2900


// Defines to keep track of which way the compare value is moving

#define EPWM_CMP_UP   1
#define EPWM_CMP_DOWN 0



//Lookup table for ePWM modulation 512 data points------------------------------------------------------
int i;
int j;
int sinelookup[512] = {1500,1518,1537,1555,1574,1592,1610,1629,
                       1647,1665,1684,1702,1720,1738,1756,1775,
                       1793,1811,1829,1847,1864,1882,1900,1918,
                       1935,1953,1971,1988,2005,2023,2040,2057,
                       2074,2091,2108,2125,2141,2158,2174,2191,
                       2207,2223,2239,2255,2271,2287,2302,2318,
                       2333,2349,2364,2379,2394,2408,2423,2437,
                       2452,2466,2480,2494,2507,2521,2534,2548,
                       2561,2574,2586,2599,2611,2624,2636,2648,
                       2660,2671,2683,2694,2705,2716,2726,2737,
                       2747,2757,2767,2777,2787,2796,2805,2814,
                       2823,2831,2840,2848,2856,2864,2871,2879,
                       2886,2893,2899,2906,2912,2918,2924,2930,
                       2935,2941,2946,2950,2955,2959,2964,2967,
                       2971,2975,2978,2981,2984,2986,2989,2991,
                       2993,2994,2996,2997,2998,2999,3000,3000,
                       3000,3000,3000,2999,2998,2997,2996,2994,
                       2993,2991,2989,2986,2984,2981,2978,2975,
                       2971,2967,2964,2959,2955,2950,2946,2941,
                       2935,2930,2924,2918,2912,2906,2899,2893,
                       2886,2879,2871,2864,2856,2848,2840,2831,
                       2823,2814,2805,2796,2787,2777,2767,2757,
                       2747,2737,2726,2716,2705,2694,2683,2671,
                       2660,2648,2636,2624,2611,2599,2586,2574,
                       2561,2548,2534,2521,2507,2494,2480,2466,
                       2452,2437,2423,2408,2394,2379,2364,2349,
                       2333,2318,2302,2287,2271,2255,2239,2223,
                       2207,2191,2174,2158,2141,2125,2108,2091,
                       2074,2057,2040,2023,2005,1988,1971,1953,
                       1935,1918,1900,1882,1864,1847,1829,1811,
                       1793,1775,1756,1738,1720,1702,1684,1665,
                       1647,1629,1610,1592,1574,1555,1537,1518,
                       1500,1482,1463,1445,1426,1408,1390,1371,
                       1353,1335,1316,1298,1280,1262,1244,1225,
                       1207,1189,1171,1153,1136,1118,1100,1082,
                       1065,1047,1029,1012,995,977,960,943,
                       926,909,892,875,859,842,826,809,
                       793,777,761,745,729,713,698,682,
                       667,651,636,621,606,592,577,563,
                       548,534,520,506,493,479,466,452,
                       439,426,414,401,389,376,364,352,
                       340,329,317,306,295,284,274,263,
                       253,243,233,223,213,204,195,186,
                       177,169,160,152,144,136,129,121,
                       114,107,101,94,88,82,76,70,
                       65,59,54,50,45,41,36,33,
                       29,25,22,19,16,14,11,9,
                       7,6,4,3,2,1,0,0,
                       0,0,0,1,2,3,4,6,
                       7,9,11,14,16,19,22,25,
                       29,33,36,41,45,50,54,59,
                       65,70,76,82,88,94,101,107,
                       114,121,129,136,144,152,160,169,
                       177,186,195,204,213,223,233,243,
                       253,263,274,284,295,306,317,329,
                       340,352,364,376,389,401,414,426,
                       439,452,466,479,493,506,520,534,
                       548,563,577,592,606,621,636,651,
                       667,682,698,713,729,745,761,777,
                       793,809,826,842,859,875,892,909,
                       926,943,960,977,995,1012,1029,1047,
                       1065,1082,1100,1118,1136,1153,1171,1189,
                       1207,1225,1244,1262,1280,1298,1316,1335,
                       1353,1371,1390,1408,1426,1445,1463,1482};

//eCAP-hoz kellenek (by Mate 2018.09.16.)----------------------------------------------------------------------
Uint32  ECap1IntCount;
Uint32  ECap1PassCount;
Uint32  EPwm1TimerDirection; //itt valaszd ki melyik timert akarod captureolni!!!!!!!
//-------------------------------------------------------------------------------------------------------------

//MAIN---------------------------------------------------------------------------------------------------------
void main(void)
{

    i=0;
    j=511;
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
    Adc_Config();


    // Initialize the ePWMs
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;      //We need to disable the sync first
    EDIS;

    InitEPwm1Example();

    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;      //Then enable the sync
    EDIS;

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
//ePWM function call
__interrupt void epwm1_isr(void)
{
    // Update the CMPA and CMPB values
    update_compare(&epwm1_info);


    // Clear INT flag for this timer
    EPwm1Regs.ETCLR.bit.INT = 1;


    // Acknowledge this interrupt to receive more interrupts from group 3
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;

    return;
}

//ADC function call
__interrupt void adc1_isr(void)
{
    Voltage1[ConversionCount] = AdcResult.ADCRESULT0;
    Voltage2[ConversionCount] = AdcResult.ADCRESULT1;


    // If 20 conversions have been logged, start over
    if(ConversionCount == 9)
    {
        ConversionCount = 0;
    }
    else
    {
        ConversionCount++;
    }


    // Clear ADCINT1 flag reinitialize for next SOC
    AdcRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;   // Acknowledge interrupt to PIE

    return;
}


void Adc_Config(){
//
//Configuration for the current measurement:
//

    EALLOW;
    AdcRegs.ADCCTL2.bit.ADCNONOVERLAP = 1; // Enable non-overlap mode sample and conversion is not overlapped

    //If needed simultaneous mode for resolver or current measurement is set:
    // AdcRegs.ADCSAMPLEMODE.bit.SIMULEN0 = 1;


    // ADCINT1 trips after AdcResults latch
    AdcRegs.ADCCTL1.bit.INTPULSEPOS = 1;

    AdcRegs.INTSEL1N2.bit.INT1E     = 1;  // Enabled ADCINT1
    AdcRegs.INTSEL1N2.bit.INT1CONT  = 0;  // Disable ADCINT1 Continuous mode


    // setup EOC1 to trigger ADCINT1 to fire
    AdcRegs.INTSEL1N2.bit.INT1SEL   = 1;

    //In simultaneous mode we only need: AdcRegs.ADCSOC0CTL.bit.CHSEL    = 2; // set SOC0 channel select to ADCINA2 and ADCINB2
    AdcRegs.ADCSOC0CTL.bit.CHSEL    = 4;  // set SOC0 channel select to ADCINA4
    AdcRegs.ADCSOC1CTL.bit.CHSEL    = 2;  // set SOC1 channel select to ADCINA2


    // set SOC0 start trigger on EPWM1A, due to round-robin SOC0 converts
    // first then SOC1
    AdcRegs.ADCSOC0CTL.bit.TRIGSEL  = 5;    //This should be 0 if we use software interrupts only


    // set SOC1 start trigger on EPWM1A, due to round-robin SOC0 converts
    // first then SOC1
    AdcRegs.ADCSOC1CTL.bit.TRIGSEL  = 5;


    // set SOC0 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)
    AdcRegs.ADCSOC0CTL.bit.ACQPS    = 6;


    // set SOC1 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)
    AdcRegs.ADCSOC1CTL.bit.ACQPS    = 6;
    EDIS;


    // Assumes ePWM1 clock is already enabled in InitSysCtrl();
    EPwm1Regs.ETSEL.bit.SOCAEN  = 1;        // Enable SOC on A group
    EPwm1Regs.ETSEL.bit.SOCASEL = 4;        // Select SOC from CMPA on upcount
    EPwm1Regs.ETPS.bit.SOCAPRD  = 1;        // Generate pulse on 1st event
}

//ePWM example
void
InitEPwm1Example()
{
    // Setup TBCLK
    EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count updown
    EPwm1Regs.TBPRD = EPWM1_TIMER_TBPRD;       // Set timer period
    EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;    // Disable phase loading
    EPwm1Regs.TBPHS.half.TBPHS = 0x0000;       // Phase is 0
    EPwm1Regs.TBCTR = 0x0000;                  // Clear counter
    EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV2;   // Clock ratio to SYSCLKOUT
    EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV2;


    // Setup shadow register load on ZERO
    EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
    EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;


    // Set Compare values
    EPwm1Regs.CMPA.half.CMPA = sinelookup[0];       // Set compare A value used to be 1
    EPwm1Regs.CMPB = sinelookup[511];               // Set Compare B value used to be 1


    // Set actions
    EPwm1Regs.AQCTLA.bit.CAU = AQ_SET;      // Set PWM1A on Zero
    EPwm1Regs.AQCTLA.bit.CAD = AQ_CLEAR;    // Clear PWM1A on event A, up count

    EPwm1Regs.AQCTLB.bit.CBU = AQ_SET;      // Set PWM1B on Zero
    EPwm1Regs.AQCTLB.bit.CBD = AQ_CLEAR;    // Clear PWM1B on event B, up count


    // Interrupt where we will change the Compare Values
    EPwm1Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;     // Select INT on Zero event
    EPwm1Regs.ETSEL.bit.INTEN = 1;                // Enable INT
    EPwm1Regs.ETPS.bit.INTPRD = ET_3RD;           // Generate INT on 3rd event


    //
    // Information this example uses to keep track of the direction the
    // CMPA/CMPB values are moving, the min and max allowed values and
    // a pointer to the correct ePWM registers
    //

    //
    // Start by increasing CMPA & CMPB
    //
    epwm1_info.EPwm_CMPA_Direction = EPWM_CMP_UP;
    epwm1_info.EPwm_CMPB_Direction = EPWM_CMP_UP;

    epwm1_info.EPwmTimerIntCount = 0;         // Zero the interrupt counter
    epwm1_info.EPwmRegHandle = &EPwm1Regs;// Set the pointer to the ePWM module
    epwm1_info.EPwmMaxCMPA = EPWM1_MAX_CMPA;  // Setup min/max CMPA/CMPB values
    epwm1_info.EPwmMinCMPA = EPWM1_MIN_CMPA;
    epwm1_info.EPwmMaxCMPB = EPWM1_MAX_CMPB;
    epwm1_info.EPwmMinCMPB = EPWM1_MIN_CMPB;
}


void update_compare(EPWM_INFO *epwm_info)
{
    //
    // Every 10'th interrupt, change the CMPA/CMPB values
    //
    if(epwm_info->EPwmTimerIntCount == 1000)
    {
        i++;
        j--;
        epwm_info->EPwmRegHandle->CMPA.half.CMPA = sinelookup[i];
        epwm_info->EPwmRegHandle->CMPB = sinelookup[j];
        if (i==511)
            i=0;
        if (j==0)
            j=511;
    }
    else
    {
        epwm_info->EPwmTimerIntCount++;
    }
}

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

//-----------------------------------------------------------------------------------------------------------

   // return;
//}
