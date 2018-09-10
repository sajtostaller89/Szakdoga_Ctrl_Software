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
void InitEPwm1Example(void);
void InitEPwm2Example(void);
void InitEPwm3Example(void);
__interrupt void epwm1_isr(void);
__interrupt void epwm2_isr(void);
__interrupt void epwm3_isr(void);
void update_compare(EPWM_INFO*);

//
// Globals
//
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

//
// Defines to keep track of which way the compare value is moving
//
#define EPWM_CMP_UP   1
#define EPWM_CMP_DOWN 0



//
// Main
//

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

void main(void)
{

    i=0;
    j=511;
    //
    // Step 1. Initialize System Control:
    // PLL, WatchDog, enable Peripheral Clocks
    // This example function is found in the F2806x_SysCtrl.c file.
    //
    InitSysCtrl();

    //
    // Step 2. Initalize GPIO:
    // This example function is found in the F2806x_Gpio.c file and
    // illustrates how to set the GPIO to it's default state.
    //
    // InitGpio();  // Skipped for this example

    //
    // For this case just init GPIO pins for ePWM1, ePWM2, ePWM3
    // These functions are in the F2806x_EPwm.c file
    //
     InitEPwm1Gpio();
    /*InitEPwm2Gpio();
    InitEPwm3Gpio();*/

    //
    // Step 3. Clear all interrupts and initialize PIE vector table:
    // Disable CPU interrupts
    //
    DINT;

    //
    // Initialize the PIE control registers to their default state.
    // The default state is all PIE interrupts disabled and flags
    // are cleared.
    // This function is found in the F2806x_PieCtrl.c file.
    //
    InitPieCtrl();

    //
    // Disable CPU interrupts and clear all CPU interrupt flags
    //
    IER = 0x0000;
    IFR = 0x0000;

    //
    // Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    // This will populate the entire table, even if the interrupt
    // is not used in this example.  This is useful for debug purposes.
    // The shell ISR routines are found in F2806x_DefaultIsr.c.
    // This function is found in F2806x_PieVect.c.
    //
    InitPieVectTable();

    //
    // Interrupts that are used in this example are re-mapped to
    // ISR functions found within this file.
    //
    EALLOW;  // This is needed to write to EALLOW protected registers
    PieVectTable.EPWM1_INT = &epwm1_isr;
    /*PieVectTable.EPWM2_INT = &epwm2_isr;
    PieVectTable.EPWM3_INT = &epwm3_isr;*/
    EDIS;    // This is needed to disable write to EALLOW protected registers

    //
    // Step 4. Initialize all the Device Peripherals:
    // This function is found in F2806x_InitPeripherals.c
    //
    // InitPeripherals();  // Not required for this example

    //
    // For this example, only initialize the ePWM
    //
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
    EDIS;

    InitEPwm1Example();
    /*InitEPwm2Example();
    InitEPwm3Example();*/

    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;

    //
    // Step 5. User specific code, enable interrupts
    //

    //
    // Enable CPU INT3 which is connected to EPWM1-3 INT:
    //
    IER |= M_INT3;

    //
    // Enable EPWM INTn in the PIE: Group 3 interrupt 1-3
    //
    PieCtrlRegs.PIEIER3.bit.INTx1 = 1;
    PieCtrlRegs.PIEIER3.bit.INTx2 = 1;
    PieCtrlRegs.PIEIER3.bit.INTx3 = 1;

    //
    // Enable global Interrupts and higher priority real-time debug events
    //
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

//
// epwm1_isr -
//
__interrupt void
epwm1_isr(void)
{
    //
    // Update the CMPA and CMPB values
    //
    update_compare(&epwm1_info);

    //
    // Clear INT flag for this timer
    //
    EPwm1Regs.ETCLR.bit.INT = 1;

    //
    // Acknowledge this interrupt to receive more interrupts from group 3
    //
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}

//
// epwm2_isr -
//
/*__interrupt void
epwm2_isr(void)
{
    //
    // Update the CMPA and CMPB values
    //
    update_compare(&epwm2_info);

    //
    // Clear INT flag for this timer
    //
    EPwm2Regs.ETCLR.bit.INT = 1;

    //
    // Acknowledge this interrupt to receive more interrupts from group 3
    //
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}

//
// epwm3_isr -
//
__interrupt void
epwm3_isr(void)
{
    //
    // Update the CMPA and CMPB values
    //
    update_compare(&epwm3_info);

    //
    // Clear INT flag for this timer
    //
    EPwm3Regs.ETCLR.bit.INT = 1;

    //
    // Acknowledge this interrupt to receive more interrupts from group 3
    //
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}*/

//
// InitEPwm1Example -
//
void
InitEPwm1Example()
{
    //
    // Setup TBCLK
    //
    EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count updown
    EPwm1Regs.TBPRD = EPWM1_TIMER_TBPRD;       // Set timer period
    EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;    // Disable phase loading
    EPwm1Regs.TBPHS.half.TBPHS = 0x0000;       // Phase is 0
    EPwm1Regs.TBCTR = 0x0000;                  // Clear counter
    EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV2;   // Clock ratio to SYSCLKOUT
    EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV2;

    //
    // Setup shadow register load on ZERO
    //
    EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
    EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

    //
    // Set Compare values
    //
    EPwm1Regs.CMPA.half.CMPA = EPWM1_MIN_CMPA;    // Set compare A value
    EPwm1Regs.CMPB = EPWM1_MIN_CMPB;              // Set Compare B value

    //
    // Set actions
    //
    EPwm1Regs.AQCTLA.bit.CAU = AQ_SET;      // Set PWM1A on Zero
    EPwm1Regs.AQCTLA.bit.CAD = AQ_CLEAR;    // Clear PWM1A on event A, up count

    EPwm1Regs.AQCTLB.bit.CBU = AQ_SET;      // Set PWM1B on Zero
    EPwm1Regs.AQCTLB.bit.CBD = AQ_CLEAR;    // Clear PWM1B on event B, up count

    //
    // Interrupt where we will change the Compare Values
    //
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

//
// InitEPwm2Example -
//
/*void
InitEPwm2Example()
{
    //
    // Setup TBCLK
    //
    EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // Count up
    EPwm2Regs.TBPRD = EPWM2_TIMER_TBPRD;       // Set timer period
    EPwm2Regs.TBCTL.bit.PHSEN = TB_DISABLE;    // Disable phase loading
    EPwm2Regs.TBPHS.half.TBPHS = 0x0000;       // Phase is 0
    EPwm2Regs.TBCTR = 0x0000;                  // Clear counter
    EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV2;   // Clock ratio to SYSCLKOUT
    EPwm2Regs.TBCTL.bit.CLKDIV = TB_DIV2;

    //
    // Setup shadow register load on ZERO
    //
    EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm2Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
    EPwm2Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

    //
    // Set Compare values
    //
    EPwm2Regs.CMPA.half.CMPA = EPWM2_MIN_CMPA;       // Set compare A value
    EPwm2Regs.CMPB = EPWM2_MAX_CMPB;                 // Set Compare B value

    //
    // Set actions
    //
    EPwm2Regs.AQCTLA.bit.PRD = AQ_CLEAR;     // Clear PWM2A on Period
    EPwm2Regs.AQCTLA.bit.CAU = AQ_SET;       // Set PWM2A on event A, up count

    EPwm2Regs.AQCTLB.bit.PRD = AQ_CLEAR;     // Clear PWM2B on Period
    EPwm2Regs.AQCTLB.bit.CBU = AQ_SET;       // Set PWM2B on event B, up count

    //
    // Interrupt where we will change the Compare Values
    //
    EPwm2Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;    // Select INT on Zero event
    EPwm2Regs.ETSEL.bit.INTEN = 1;               // Enable INT
    EPwm2Regs.ETPS.bit.INTPRD = ET_3RD;          // Generate INT on 3rd event

    //
    // Information this example uses to keep track of the direction the
    // CMPA/CMPB values are moving, the min and max allowed values and
    // a pointer to the correct ePWM registers
    //

    //
    // Start by increasing CMPA and decreasing CMPB
    //
    epwm2_info.EPwm_CMPA_Direction = EPWM_CMP_UP;
    epwm2_info.EPwm_CMPB_Direction = EPWM_CMP_DOWN;
    epwm2_info.EPwmTimerIntCount = 0;         // Zero the interrupt counter
    epwm2_info.EPwmRegHandle = &EPwm2Regs;// Set the pointer to the ePWM module
    epwm2_info.EPwmMaxCMPA = EPWM2_MAX_CMPA;  // Setup min/max CMPA/CMPB values
    epwm2_info.EPwmMinCMPA = EPWM2_MIN_CMPA;
    epwm2_info.EPwmMaxCMPB = EPWM2_MAX_CMPB;
    epwm2_info.EPwmMinCMPB = EPWM2_MIN_CMPB;
}

//
// InitEPwm3Example -
//
void
InitEPwm3Example(void)
{
    //
    // Setup TBCLK
    //
    EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // Count up
    EPwm3Regs.TBPRD = EPWM3_TIMER_TBPRD;       // Set timer period
    EPwm3Regs.TBCTL.bit.PHSEN = TB_DISABLE;    // Disable phase loading
    EPwm3Regs.TBPHS.half.TBPHS = 0x0000;       // Phase is 0
    EPwm3Regs.TBCTR = 0x0000;                  // Clear counter
    EPwm3Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;   // Clock ratio to SYSCLKOUT
    EPwm3Regs.TBCTL.bit.CLKDIV = TB_DIV1;

    //
    // Setup shadow register load on ZERO
    //
    EPwm3Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm3Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm3Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
    EPwm3Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

    //
    // Set Compare values
    //
    EPwm3Regs.CMPA.half.CMPA = EPWM3_MIN_CMPA; // Set compare A value
    EPwm3Regs.CMPB = EPWM3_MAX_CMPB;           // Set Compare B value

    //
    // Set Actions
    //
    EPwm3Regs.AQCTLA.bit.CAU = AQ_SET;    // Set PWM3A on event B, up count
    EPwm3Regs.AQCTLA.bit.CBU = AQ_CLEAR;  // Clear PWM3A on event B, up count

    EPwm3Regs.AQCTLB.bit.ZRO = AQ_TOGGLE; // Toggle EPWM3B on Zero

    //
    // Interrupt where we will change the Compare Values
    //
    EPwm3Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;     // Select INT on Zero event
    EPwm3Regs.ETSEL.bit.INTEN = 1;                // Enable INT
    EPwm3Regs.ETPS.bit.INTPRD = ET_3RD;           // Generate INT on 3rd event

    //
    // Start by increasing the compare A and decreasing compare B
    //
    epwm3_info.EPwm_CMPA_Direction = EPWM_CMP_UP;
    epwm3_info.EPwm_CMPB_Direction = EPWM_CMP_DOWN;

    //
    // Start the cout at 0
   //

    epwm3_info.EPwmTimerIntCount = 0;
    epwm3_info.EPwmRegHandle = &EPwm3Regs;
    epwm3_info.EPwmMaxCMPA = EPWM3_MAX_CMPA;
    epwm3_info.EPwmMinCMPA = EPWM3_MIN_CMPA;
    epwm3_info.EPwmMaxCMPB = EPWM3_MAX_CMPB;
    epwm3_info.EPwmMinCMPB = EPWM3_MIN_CMPB;
}*/

//
// update_compare -
//
void
update_compare(EPWM_INFO *epwm_info)
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

        /*epwm_info->EPwmTimerIntCount = 0;

        //
        // If we were increasing CMPA, check to see if we reached the max value
        // If not, increase CMPA else, change directions and decrease CMPA
        //
        if(epwm_info->EPwm_CMPA_Direction == EPWM_CMP_UP)
        {
            if(epwm_info->EPwmRegHandle->CMPA.half.CMPA <
               epwm_info->EPwmMaxCMPA)
            {
                epwm_info->EPwmRegHandle->CMPA.half.CMPA++;
            }
            else
            {
                epwm_info->EPwm_CMPA_Direction = EPWM_CMP_DOWN;
                epwm_info->EPwmRegHandle->CMPA.half.CMPA--;
            }
        }

        //
        // If we were decreasing CMPA, check to see if we reached the min value
        // If not, decrease CMPA else, change directions and increase CMPA
        //
        else
        {
            if(epwm_info->EPwmRegHandle->CMPA.half.CMPA ==
               epwm_info->EPwmMinCMPA)
            {
                epwm_info->EPwm_CMPA_Direction = EPWM_CMP_UP;
                epwm_info->EPwmRegHandle->CMPA.half.CMPA++;
            }
            else
            {
                epwm_info->EPwmRegHandle->CMPA.half.CMPA--;
            }
        }

        //
        // If we were increasing CMPB, check to see if we reached the max value
        // If not, increase CMPB else, change directions and decrease CMPB
        //
        if(epwm_info->EPwm_CMPB_Direction == EPWM_CMP_UP)
        {
            if(epwm_info->EPwmRegHandle->CMPB < epwm_info->EPwmMaxCMPB)
            {
                epwm_info->EPwmRegHandle->CMPB++;
            }
            else
            {
                epwm_info->EPwm_CMPB_Direction = EPWM_CMP_DOWN;
                epwm_info->EPwmRegHandle->CMPB--;
            }
        }

        //
        // If we were decreasing CMPB, check to see if we reached the min value
        // If not, decrease CMPB else, change directions and increase CMPB
        //
        else
        {
            if(epwm_info->EPwmRegHandle->CMPB == epwm_info->EPwmMinCMPB)
            {
                epwm_info->EPwm_CMPB_Direction = EPWM_CMP_UP;
                epwm_info->EPwmRegHandle->CMPB++;
            }
            else
            {
                epwm_info->EPwmRegHandle->CMPB--;
            }
        }
    }
    */
    else
    {
        epwm_info->EPwmTimerIntCount++;
    }

    return;
}

//
// End of File
//
