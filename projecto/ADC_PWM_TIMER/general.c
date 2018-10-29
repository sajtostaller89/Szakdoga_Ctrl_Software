/*
 * gpio.c
 *
 *  Created on: 2018. szept. 18.
 *      Author: Ákos
 */

#include "F2806x_Device.h"     // F2806x Headerfile Include File
#include "F2806x_Examples.h"   // F2806x Examples Include File
#include "pheripherals.h"
#include "general.h"

void ConfigPieVectTable(){

    // Interrupts that are used are re-mapped to
    // ISR functions found within this file.
    EALLOW;// This is needed to write to EALLOW protected registers

    PieVectTable.ADCINT1 = &adc1_isr;       //First the ADC1

    PieVectTable.EPWM1_INT = &epwm4_isr;    //Second the ePWM1
    PieVectTable.EPWM2_INT = &epwm5_isr;    //Second the ePWM1
    PieVectTable.EPWM3_INT = &epwm6_isr;    //Second the ePWM1

    PieVectTable.TINT0 = &cpu_timer0_isr;   //Third the CPU Timers (by Mate 2018.09.16.)
    PieVectTable.TINT1 = &cpu_timer1_isr;
    PieVectTable.TINT2 = &cpu_timer2_isr;

    PieVectTable.ECAP1_INT = &ecap1_isr;    //Fourth eCAP (by Mate 2018.09.16.)

    EDIS;                                   // This is needed to disable write to EALLOW protected registers

}

void ConfigEnableInterrupts(){
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
}

double duty_calc(){         //general.h should contain all the necessary values for dclink, etc.

    //return (phase_voltage/dclink_voltage);
    return 2.0;
}
