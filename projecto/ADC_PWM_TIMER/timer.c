/*
 * timer.c
 *
 *  Created on: 2018. szept. 18.
 *      Author: Máté
 */

#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include "F2806x_EPwm_defines.h"
#include "pheripherals.h"


// cpu_timer0_isr
__interrupt void
cpu_timer0_isr(void)
{
    CpuTimer0.InterruptCount++;

    //
    // Acknowledge this interrupt to receive more interrupts from group 1
    //
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}


// cpu_timer1_isr
__interrupt void
cpu_timer1_isr(void)
{
    CpuTimer1.InterruptCount++;

    //
    // The CPU acknowledges the interrupt
    //
    EDIS;
}


// cpu_timer2_isr
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
