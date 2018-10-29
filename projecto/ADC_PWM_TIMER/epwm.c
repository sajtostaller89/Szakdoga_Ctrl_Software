/*
 * epwm.c
 *
 *  Created on: 2018. szept. 18.
 *      Author: Ákos
 *      For some reason i can not make epwm1 and 3 to work :(
 */

#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include "F2806x_EPwm_defines.h"
#include "pheripherals.h"
#include "general.h"

__interrupt void epwm4_isr(void)
{
    // Update the CMPA and CMPB values
    update_compare();


    // Clear INT flag for this timer
    EPwm1Regs.ETCLR.bit.INT = 1;
    // Acknowledge this interrupt to receive more interrupts from group 3
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
    return;
}

__interrupt void epwm5_isr(void)
{
    // Update the CMPA and CMPB values
    update_compare();


    // Clear INT flag for this timer
    EPwm2Regs.ETCLR.bit.INT = 1;
    // Acknowledge this interrupt to receive more interrupts from group 3
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
    return;
}

__interrupt void epwm6_isr(void)
{
    // Update the CMPA and CMPB values
    update_compare();


    // Clear INT flag for this timer
    EPwm3Regs.ETCLR.bit.INT = 1;
    // Acknowledge this interrupt to receive more interrupts from group 3
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
    return;
}


//ePWM

void EPwm4_Config()
{
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;      //We need to disable the sync first
    EDIS;


    // Setup TBCLK
    EPwm4Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;  // Count updown
    EPwm4Regs.TBPRD = EPWM4_TIMER_TBPRD;            // Set timer period
    EPwm4Regs.TBCTL.bit.PHSEN = TB_DISABLE;         // Disable phase loading
    EPwm4Regs.TBPHS.half.TBPHS = 0x0000;            // Phase is 0
    EPwm4Regs.TBCTR = 0x0000;                       // Clear counter
    EPwm4Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;        // Clock ratio to SYSCLKOUT
    EPwm4Regs.TBCTL.bit.CLKDIV = TB_DIV1;
    EPwm4Regs.TBCTL.bit.PRDLD = TB_SHADOW;
    EPwm4Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO;     // Sync down-stream module

    // Setup shadow register load on ZERO
    EPwm4Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm4Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm4Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
    EPwm4Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;


    // Set Compare values, not using dead-band module
    EPwm4Regs.CMPA.half.CMPA = EPWM4_TIMER_TBPRD/2+250;          // Set compare A value used to be 1
    EPwm4Regs.CMPB = EPWM4_TIMER_TBPRD/2-250;          // Set Compare B value used to be 1


    // Set actions
    EPwm4Regs.AQCTLA.bit.CAU = AQ_SET;      // Set PWM1A on Zero legyen igy, hogy ne csusszunk el, annyi hogy invertalni kell majd a comparalasi szintet, mert ha kicsi ideig akarunk 3.3voltot adni, akkor magas szint kell
    EPwm4Regs.AQCTLA.bit.CAD = AQ_CLEAR;    // Clear PWM1A on event A, up count

    EPwm4Regs.AQCTLB.bit.CBD = AQ_SET;      // Set PWM1B on Zero
    EPwm4Regs.AQCTLB.bit.CBU = AQ_CLEAR;    // Clear PWM1B on event B, up count


    // Interrupt where we will change the Compare Values
    EPwm4Regs.ETSEL.bit.INTSEL = ET_CTR_PRDZERO;         // Select INT on Zero event
    EPwm4Regs.ETSEL.bit.INTEN = 1;                       // Enable INT
    EPwm4Regs.ETPS.bit.INTPRD = ET_1ST;                  // Generate INT on 1st event


    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;      //Then enable the sync
    EDIS;
}

void EPwm5_Config()
{
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;      //We need to disable the sync first
    EDIS;


    // Setup TBCLK
    EPwm5Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;  // Count updown
    EPwm5Regs.TBPRD = EPWM5_TIMER_TBPRD;            // Set timer period
    EPwm5Regs.TBCTL.bit.PHSEN = TB_ENABLE;         // Disable phase loading
    EPwm5Regs.TBPHS.half.TBPHS = 0x2710; //10000;            // Phase is 120 deg = 360 * 10000/30000
    EPwm5Regs.TBCTR = 0x0000;                       // Clear counter
    EPwm5Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;        // Clock ratio to SYSCLKOUT
    EPwm5Regs.TBCTL.bit.CLKDIV = TB_DIV1;
    EPwm5Regs.TBCTL.bit.PRDLD = TB_SHADOW;
    EPwm5Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;     // Sync down-stream module

    // Setup shadow register load on ZERO
    EPwm5Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm5Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm5Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
    EPwm5Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;


    // Set Compare values, not using dead-band module
    EPwm5Regs.CMPA.half.CMPA = EPWM5_TIMER_TBPRD/2+500;          // Set compare A value, should be >=1
    EPwm5Regs.CMPB = EPWM5_TIMER_TBPRD/2-500;          // Set Compare B value, should be >=1 200 is good ie


    // Set actions
    EPwm5Regs.AQCTLA.bit.CAU = AQ_SET;      // Set PWM1A on Zero legyen igy, hogy ne csusszunk el, annyi hogy invertalni kell majd a comparalasi szintet, mert ha kicsi ideig akarunk 3.3voltot adni, akkor magas szint kell
    EPwm5Regs.AQCTLA.bit.CAD = AQ_CLEAR;    // Clear PWM1A on event A, down count

    EPwm5Regs.AQCTLB.bit.CBD = AQ_SET;      // Inverter working principle
    EPwm5Regs.AQCTLB.bit.CBU = AQ_CLEAR;


    // Interrupt where we will change the Compare Values
    EPwm5Regs.ETSEL.bit.INTSEL = ET_CTR_PRDZERO;         // Select INT on Zero event
    EPwm5Regs.ETSEL.bit.INTEN = 1;                       // Enable INT
    EPwm5Regs.ETPS.bit.INTPRD = ET_1ST;                  // Generate INT on 1st event


    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;      //Then enable the sync
    EDIS;
}

void EPwm6_Config()
{
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;      //We need to disable the sync first
    EDIS;


    // Setup TBCLK
    EPwm6Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;  // Count updown
    EPwm6Regs.TBPRD = EPWM6_TIMER_TBPRD;            // Set timer period
    EPwm6Regs.TBCTL.bit.PHSEN = TB_ENABLE;         // Disable phase loading
    EPwm6Regs.TBPHS.half.TBPHS = 20000;            // Phase is 0
    EPwm6Regs.TBCTR = 0x0000;                       // Clear counter
    EPwm6Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;        // Clock ratio to SYSCLKOUT
    EPwm6Regs.TBCTL.bit.CLKDIV = TB_DIV1;
    EPwm6Regs.TBCTL.bit.PRDLD = TB_SHADOW;
    EPwm6Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;     // Sync down-stream module

    // Setup shadow register load on ZERO
    EPwm6Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm6Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm6Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
    EPwm6Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;


    // Set Compare values, not using dead-band module
    EPwm6Regs.CMPA.half.CMPA = EPWM6_TIMER_TBPRD/2+100;          // Set compare A value used to be 1
    EPwm6Regs.CMPB = EPWM6_TIMER_TBPRD/2-100;          // Set Compare B value used to be 1


    // Set actions
    EPwm6Regs.AQCTLA.bit.CAU = AQ_SET;      // Set PWM1A on Zero legyen igy, hogy ne csusszunk el, annyi hogy invertalni kell majd a comparalasi szintet, mert ha kicsi ideig akarunk 3.3voltot adni, akkor magas szint kell
    EPwm6Regs.AQCTLA.bit.CAD = AQ_CLEAR;    // Clear PWM1A on event A, up count

    EPwm6Regs.AQCTLB.bit.CBD = AQ_SET;      // Set PWM1B on Zero
    EPwm6Regs.AQCTLB.bit.CBU = AQ_CLEAR;    // Clear PWM1B on event B, up count


    // Interrupt where we will change the Compare Values
    EPwm6Regs.ETSEL.bit.INTSEL = ET_CTR_PRDZERO;         // Select INT on Zero event
    EPwm6Regs.ETSEL.bit.INTEN = 1;                       // Enable INT
    EPwm6Regs.ETPS.bit.INTPRD = ET_1ST;                  // Generate INT on 1st event


    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;      //Then enable the sync
    EDIS;
}

void update_compare()   //ide majd ugye be kéne adni egy doulet ami az adott fázisra megadja h mennyinek kell lennie feszben
{
    //EPwm1Regs.CMPA.half.CMPA    = (Uint16)(1.0 - duty_calc())*EPWM1_TIMER_TBPRD;
    //EPwm1Regs.CMPB              = (EPWM1_TIMER_TBPRD - EPwm1Regs.CMPA.half.CMPA);
    return;
}

//Maybe the code below would work fine:
//=====================================================================
// Configuration
//=====================================================================
// Initialization Time
//========================
// EPWM Module 1 config
/*EPwm1Regs.TBPRD = 800; // Period = 1600 TBCLK counts
EPwm1Regs.TBPHS.half.TBPHS = 0; // Set Phase register to zero
EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Symmetrical mode
EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE; // Master module
EPwm1Regs.TBCTL.bit.PRDLD = TB_SHADOW;
EPwm1Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO; // Sync down-stream module
EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO; // load on CTR=Zero
EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO; // load on CTR=Zero
EPwm1Regs.AQCTLA.bit.CAU = AQ_SET; // set actions for EPWM1A
EPwm1Regs.AQCTLA.bit.CAD = AQ_CLEAR;
EPwm1Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE; // enable Dead-band module
EPwm1Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; // Active Hi complementary
EPwm1Regs.DBFED = 50; // FED = 50 TBCLKs
EPwm1Regs.DBRED = 50; // RED = 50 TBCLKs

// EPWM Module 2 config
EPwm2Regs.TBPRD = 800; // Period = 1600 TBCLK counts
EPwm2Regs.TBPHS.half.TBPHS = 0; // Set Phase register to zero
EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Symmetrical mode
EPwm2Regs.TBCTL.bit.PHSEN = TB_ENABLE; // Slave module
EPwm2Regs.TBCTL.bit.PRDLD = TB_SHADOW;
EPwm2Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN; // sync flow-through
EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
EPwm2Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO; // load on CTR=Zero
EPwm2Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO; // load on CTR=Zero
EPwm2Regs.AQCTLA.bit.CAU = AQ_SET; // set actions for EPWM2A
EPwm2Regs.AQCTLA.bit.CAD = AQ_CLEAR;
EPwm2Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE; // enable Dead-band module
EPwm2Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; // Active Hi complementary
EPwm2Regs.DBFED = 50; // FED = 50 TBCLKs
EPwm2Regs.DBRED = 50; // RED = 50 TBCLKs

// EPWM Module 3 config
EPwm3Regs.TBPRD = 800; // Period = 1600 TBCLK counts
EPwm3Regs.TBPHS.half.TBPHS = 0; // Set Phase register to zero
EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Symmetrical mode
EPwm3Regs.TBCTL.bit.PHSEN = TB_ENABLE; // Slave module
EPwm3Regs.TBCTL.bit.PRDLD = TB_SHADOW;
EPwm3Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN; // sync flow-through
EPwm3Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
EPwm3Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
EPwm3Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO; // load on CTR=Zero
EPwm3Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO; // load on CTR=Zero
EPwm3Regs.AQCTLA.bit.CAU = AQ_SET; // set actions for EPWM3A
EPwm3Regs.AQCTLA.bit.CAD = AQ_CLEAR;
EPwm3Regs.DBCTL.bitMODE = DB_FULL_ENABLE; // enable Dead-band module
EPwm3Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; // Active Hi complementary
EPwm3Regs.DBFED = 50; // FED = 50 TBCLKs
EPwm3Regs.DBRED = 50; // RED = 50 TBCLKs

// Run Time (Note: Example execution of one run-time instant)
//=========================================================
EPwm1Regs.CMPA.half.CMPA = 500; // adjust duty for output EPWM1A
EPwm2Regs.CMPA.half.CMPA = 600; // adjust duty for output EPWM2A
EPwm3Regs.CMPA.half.CMPA = 700; // adjust duty for output EPWM3A*/
