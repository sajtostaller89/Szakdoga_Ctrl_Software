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
#include <MotorCtrl_FOC/Field_Oriented_Motor_Control.h>
#include <MotorCtrl_FOC/Field_Oriented_Motor_Control_private.h>
#include <MotorCtrl_FOC/Field_Oriented_Motor_Control_types.h>

Uint16 duty4, duty5, duty6;
float dclink_voltage = 60.0;
Uint16 deadtime = 4; // 40ns mert ez csak a fele


__interrupt void epwm4_isr(void)
{
    // Update the CMPA and CMPB values for every phase(epwm 4-6)
    //update_compare(Outputs);


    // Clear INT flag for this timer
    EPwm1Regs.ETCLR.bit.INT = 1;
    // Acknowledge this interrupt to receive more interrupts from group 3
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
    return;
}

/*__interrupt void epwm5_isr(void)
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
}*/


//ePWM
void EPwm2_Config()
{
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;      //We need to disable the sync first
    EDIS;

    // Setup TBCLK
    EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;  // Count updown, we trigger on both PRD and ZERO
    EPwm2Regs.TBPRD = EPWM2_TIMER_TBPRD;            // Set timer period
    EPwm2Regs.TBCTL.bit.PHSEN = TB_DISABLE;         // Disable phase loading
    EPwm2Regs.TBPHS.half.TBPHS = 0x0000;            // Phase is 0
    EPwm2Regs.TBCTR = 0x0000;                       // Clear counter
    EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;        // Clock ratio to SYSCLKOUT
    EPwm2Regs.TBCTL.bit.CLKDIV = TB_DIV1;
    EPwm2Regs.TBCTL.bit.PRDLD = TB_SHADOW;

    // Setup shadow register load on ZERO
    EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm2Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
    EPwm2Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

    // Assumes ePWM2 clock is already enabled in InitSysCtrl(); ePWM2 triggers resolver measurement
    EPwm2Regs.ETSEL.bit.SOCAEN  = 1;            // Enable SOC on A group
    EPwm2Regs.ETSEL.bit.SOCASEL = 3;            // Select SOC on CTR=0 OR CTR=PRD
    EPwm2Regs.ETPS.bit.SOCAPRD  = 1;            // Generate pulse on every 1st event

    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;      //Then enable the sync
    EDIS;
}

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
    EPwm4Regs.CMPA.half.CMPA = EPWM4_TIMER_TBPRD/2+deadtime;          // Set compare A value used to be 1
    EPwm4Regs.CMPB = EPWM4_TIMER_TBPRD/2-deadtime;          // Set Compare B value used to be 1

    // Set actions
    EPwm4Regs.AQCTLA.bit.CAU = AQ_SET;      // Set PWM1A on Zero legyen igy, hogy ne csusszunk el, annyi hogy invertalni kell majd a comparalasi szintet, mert ha kicsi ideig akarunk 3.3voltot adni, akkor magas szint kell
    EPwm4Regs.AQCTLA.bit.CAD = AQ_CLEAR;    // Clear PWM1A on event A, up count

    EPwm4Regs.AQCTLB.bit.CBD = AQ_SET;      // Set PWM1B on Zero
    EPwm4Regs.AQCTLB.bit.CBU = AQ_CLEAR;    // Clear PWM1B on event B, up count

    // Interrupt where we will change the Compare Values or we should change the values inside the adc interrupt right
    //EPwm4Regs.ETSEL.bit.INTSEL = ET_CTR_PRDZERO;         // Select INT on Zero event
    //EPwm4Regs.ETSEL.bit.INTEN = 1;                       // Enable INT
    //EPwm4Regs.ETPS.bit.INTPRD = ET_1ST;                  // Generate INT on 1st event

    // ePWM4 triggers current measurement with 6kHz
    EPwm4Regs.ETSEL.bit.SOCAEN  = 1;            // Enable SOC on A group
    EPwm4Regs.ETSEL.bit.SOCASEL = 3;            // Select SOC on CTR=0 OR CTR=PRD
    EPwm4Regs.ETPS.bit.SOCAPRD  = 1;            // Generate pulse on every 1st event

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
    EPwm5Regs.TBCTL.bit.PHSEN = TB_ENABLE;          // Disable phase loading
    EPwm5Regs.TBPHS.half.TBPHS = 0x0000; //10000;   // Phase is 120 deg = 360 * 10000/30000 nope, the control will do this
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
    EPwm5Regs.CMPA.half.CMPA = EPWM5_TIMER_TBPRD/2+deadtime;          // Set compare A value, should be >=1
    EPwm5Regs.CMPB = EPWM5_TIMER_TBPRD/2-deadtime;          // Set Compare B value, should be >=1 200 is good ie

    // Set actions
    EPwm5Regs.AQCTLA.bit.CAU = AQ_SET;      // Set PWM1A on Zero legyen igy, hogy ne csusszunk el, annyi hogy invertalni kell majd a comparalasi szintet, mert ha kicsi ideig akarunk 3.3voltot adni, akkor magas szint kell
    EPwm5Regs.AQCTLA.bit.CAD = AQ_CLEAR;    // Clear PWM1A on event A, down count

    EPwm5Regs.AQCTLB.bit.CBD = AQ_SET;      // Inverter working principle
    EPwm5Regs.AQCTLB.bit.CBU = AQ_CLEAR;

    /*// Interrupt where we will change the Compare Values
    EPwm5Regs.ETSEL.bit.INTSEL = ET_CTR_PRDZERO;         // Select INT on Zero event
    EPwm5Regs.ETSEL.bit.INTEN = 1;                       // Enable INT
    EPwm5Regs.ETPS.bit.INTPRD = ET_1ST;                  // Generate INT on 1st event*/

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
    EPwm6Regs.TBPHS.half.TBPHS = 0x0000;            // Phase is 0
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
    EPwm6Regs.CMPA.half.CMPA = EPWM6_TIMER_TBPRD/2+deadtime;          // Set compare A value used to be 1
    EPwm6Regs.CMPB = EPWM6_TIMER_TBPRD/2-deadtime;          // Set Compare B value used to be 1

    // Set actions
    EPwm6Regs.AQCTLA.bit.CAU = AQ_SET;      // Set PWM1A on Zero legyen igy, hogy ne csusszunk el, annyi hogy invertalni kell majd a comparalasi szintet, mert ha kicsi ideig akarunk 3.3voltot adni, akkor magas szint kell
    EPwm6Regs.AQCTLA.bit.CAD = AQ_CLEAR;    // Clear PWM1A on event A, up count

    EPwm6Regs.AQCTLB.bit.CBD = AQ_SET;      // Set PWM1B on Zero
    EPwm6Regs.AQCTLB.bit.CBU = AQ_CLEAR;    // Clear PWM1B on event B, up count

    /*// Interrupt where we will change the Compare Values
    EPwm6Regs.ETSEL.bit.INTSEL = ET_CTR_PRDZERO;         // Select INT on Zero event
    EPwm6Regs.ETSEL.bit.INTEN = 1;                       // Enable INT
    EPwm6Regs.ETPS.bit.INTPRD = ET_1ST;                  // Generate INT on 1st event*/

    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;      //Then enable the sync
    EDIS;
}

void update_compare(Output FOC_voltages)
{
    // Duty calculation
    duty4 = (Uint16)(1.0 - (FOC_voltages.va/dclink_voltage))*EPWM4_TIMER_TBPRD;
    duty5 = (Uint16)(1.0 - (FOC_voltages.vb/dclink_voltage))*EPWM5_TIMER_TBPRD;
    duty6 = (Uint16)(1.0 - (FOC_voltages.vc/dclink_voltage))*EPWM6_TIMER_TBPRD;

    // Duty corrections in case of invalid values, regarding the deadtime corrections' nature
    if(duty4 > EPWM4_TIMER_TBPRD - deadtime)
        duty4 = EPWM4_TIMER_TBPRD - deadtime;
    if(duty4 < deadtime)
        duty4 = deadtime;

    if(duty5 > EPWM5_TIMER_TBPRD - deadtime)
        duty5 = EPWM5_TIMER_TBPRD - deadtime;
    if(duty5 < deadtime)
        duty5 = deadtime;

    if(duty6 > EPWM6_TIMER_TBPRD - deadtime)
        duty6 = EPWM6_TIMER_TBPRD - deadtime;
    if(duty6 < deadtime)
        duty6 = deadtime;

    // Updating compare values and effectuating with the deadtime
    EPwm4Regs.CMPA.half.CMPA    = duty4 + deadtime;
    EPwm4Regs.CMPB              = duty4 - deadtime;

    EPwm5Regs.CMPA.half.CMPA    = duty5 + deadtime;
    EPwm5Regs.CMPB              = duty5- deadtime;

    EPwm6Regs.CMPA.half.CMPA    = duty6 + deadtime;
    EPwm6Regs.CMPB              = duty6 - deadtime;

    return;
}
