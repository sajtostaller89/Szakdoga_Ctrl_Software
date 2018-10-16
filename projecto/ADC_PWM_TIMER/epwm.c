/*
 * epwm.c
 *
 *  Created on: 2018. szept. 18.
 *      Author: Ákos
 */

#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include "F2806x_EPwm_defines.h"
#include "pheripherals.h"
#include "general.h"

//ePWM function call
__interrupt void epwm1_isr(void)
{
    // Update the CMPA and CMPB values
    update_compare();


    // Clear INT flag for this timer
    EPwm1Regs.ETCLR.bit.INT = 1;


    // Acknowledge this interrupt to receive more interrupts from group 3
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;

    return;
}


//ePWM

void EPwm1_Config()
{
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;      //We need to disable the sync first
    EDIS;


    // Setup TBCLK
    EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;  // Count updown
    EPwm1Regs.TBPRD = EPWM1_TIMER_TBPRD;            // Set timer period
    EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;         // Disable phase loading
    EPwm1Regs.TBPHS.half.TBPHS = 0x0000;            // Phase is 0
    EPwm1Regs.TBCTR = 0x0000;                       // Clear counter
    EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV2;        // Clock ratio to SYSCLKOUT
    EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV2;
    EPwm1Regs.TBCTL.bit.PRDLD = TB_SHADOW;
    EPwm1Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO;     // Sync down-stream module

    // Setup shadow register load on ZERO
    EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
    EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;


    // Set Compare values
    EPwm1Regs.CMPA.half.CMPA = EPWM1_TIMER_TBPRD;          // Set compare A value used to be 1
    EPwm1Regs.CMPB = EPwm1Regs.CMPA.half.CMPA - EPWM1_TIMER_TBPRD;          // Set Compare B value used to be 1


    // Set actions
    EPwm1Regs.AQCTLA.bit.CAU = AQ_SET;      // Set PWM1A on Zero
    EPwm1Regs.AQCTLA.bit.CAD = AQ_CLEAR;    // Clear PWM1A on event A, up count

    EPwm1Regs.AQCTLB.bit.CBU = AQ_SET;      // Set PWM1B on Zero
    EPwm1Regs.AQCTLB.bit.CBD = AQ_CLEAR;    // Clear PWM1B on event B, up count


    // Dead-band settings
    EPwm1Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE; // enable Dead-band module
    EPwm1Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; // Active Hi complementary
    EPwm1Regs.DBFED = 50; // FED = 50 TBCLKs
    EPwm1Regs.DBRED = 50; // RED = 50 TBCLKs


    // Interrupt where we will change the Compare Values
    EPwm1Regs.ETSEL.bit.INTSEL = ET_CTR_PRDZERO;     // Select INT on Zero event
    EPwm1Regs.ETSEL.bit.INTEN = 1;                // Enable INT
    EPwm1Regs.ETPS.bit.INTPRD = ET_1ST;           // Generate INT on 3rd event


    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;      //Then enable the sync
    EDIS;
}


void update_compare()   //ide majd ugye be kéne adni egy doulet ami az adott fázisra megadja h mennyinek kell lennie feszben
{
    EPwm1Regs.CMPA.half.CMPA    = (Uint16)(1.0 - duty_calc())*EPWM1_TIMER_TBPRD;
    EPwm1Regs.CMPB              = (EPWM1_TIMER_TBPRD - EPwm1Regs.CMPA.half.CMPA);
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
