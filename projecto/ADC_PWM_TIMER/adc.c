/*
 * adc.c
 *
 *  Created on: 2018. szept. 18.
 *      Author: Ákos
 */

#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include "F2806x_EPwm_defines.h"
#include "pheripherals.h"


Uint16 ConversionCount;
Uint16 Voltage1[10];
Uint16 Voltage2[10];

void Adc1_Config(){
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
