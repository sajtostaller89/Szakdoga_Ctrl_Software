/*
 * ecap.c
 *
 *  Created on: 2018. szept. 18.
 *      Author: Máté
 */

#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include "F2806x_EPwm_defines.h"
#include "pheripherals.h"

Uint32  ECap1IntCount = 0;
Uint32  ECap1PassCount = 0;
Uint32  EPwm1TimerDirection;

// InitEPwmTimer
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


// InitECapture
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


// ecap1_isr
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


// Fail
void Fail()
    {
        __asm("   ESTOP0");
    }


