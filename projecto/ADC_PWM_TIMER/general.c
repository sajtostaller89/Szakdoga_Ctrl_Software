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
    PieVectTable.ADCINT2 = &adc2_isr;       //First the ADC2
    PieVectTable.ADCINT3 = &adc3_isr;       //First the ADC3

    EDIS;                                   // This is needed to disable write to EALLOW protected registers

}

void ConfigEnableInterrupts(){
    // Enable CPU INT1 which is connected to ADC1 INT:
    IER |= M_INT1;                       // Enable CPU Interrupt 1
    IER |= M_INT10;                      // Enable CPU Interrupt 10 for adcint3

     // Enable CPU INT3 which is connected to EPWM1-3 INT:
     EINT;                               // Enable Global interrupt INTM
     ERTM;                               // Enable Global realtime interrupt DBGM


     // Enable ADC INTn in the PIE: Group 1 interrupt 1
     PieCtrlRegs.PIEIER1.bit.INTx1 = 1;
     PieCtrlRegs.PIEIER1.bit.INTx2 = 1;


     // Enable global Interrupts and higher priority real-time debug events
     EINT;   // Enable Global interrupt INTM
     ERTM;   // Enable Global realtime interrupt DBGM
}
