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

void Adc_Config(){
    EALLOW;
    AdcRegs.ADCCTL2.bit.ADCNONOVERLAP   = 1;    // Enable non-overlap mode sample and conversion is not overlapped
    AdcRegs.ADCCTL1.bit.INTPULSEPOS     = 1;    // ADCINT1 trips after AdcResults latch this configures the EOC: INT pulse generation occurs 1 cycle prior to ADC result latching into its result register
    EDIS;
}

void Adc1_Config(){

    //Configuration for the current measurement:
    EALLOW;
    AdcRegs.ADCSAMPLEMODE.bit.SIMULEN0  = 1;    // Simultaneous mode for current measurement is set
    AdcRegs.INTSEL1N2.bit.INT1E         = 1;    // Enabled ADCINT1
    AdcRegs.INTSEL1N2.bit.INT1CONT      = 0;    // Disable ADCINT1 Continuous mode
    AdcRegs.INTSEL1N2.bit.INT1SEL       = 1;    // setup EOC1 to trigger ADCINT1 to fire, this way an interrupt occurs when both samples are ready
    AdcRegs.ADCSOC0CTL.bit.CHSEL        = 1;    // In simultaneous mode we only need to set SOC0 channel select to ADCINA1 and ADCINB1
    AdcRegs.ADCSOC0CTL.bit.TRIGSEL      = 5;    // ADCTRIG5 = ePWM1.ADCSOCA, thats what we will set up
    AdcRegs.ADCSOC0CTL.bit.ACQPS        = 6;    // set SOC0 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)
    AdcRegs.ADCSOC1CTL.bit.ACQPS        = 6;    // set SOC1 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)
    EDIS;

    // Assumes ePWM1 clock is already enabled in InitSysCtrl(); ePWM1 triggers current measurement
    EPwm1Regs.ETSEL.bit.SOCAEN  = 1;            // Enable SOC on A group
    EPwm1Regs.ETSEL.bit.SOCASEL = 3;            // Select SOC on CTR=0 OR CTR=PRD TODO:or this should be cmpa?
    EPwm1Regs.ETPS.bit.SOCAPRD  = 1;            // Generate pulse on 1st event
}

void Adc2_Config(){
//
//Configuration for the resolver signal measurement:
//

    EALLOW;
    AdcRegs.ADCSAMPLEMODE.bit.SIMULEN2  = 1;    // SOC2 & SOC3 sampled at the same time
    AdcRegs.INTSEL1N2.bit.INT2E         = 1;    // Enabled ADCINT2
    AdcRegs.INTSEL1N2.bit.INT2CONT      = 0;    // Disable ADCINT2 Continuous mode
    AdcRegs.INTSEL1N2.bit.INT2SEL       = 3;    // setup EOC3 to trigger ADCINT2 to fire, this way an interrupt occurs when both samples are ready
    AdcRegs.ADCSOC2CTL.bit.CHSEL        = 2;    // set SOC2 channel select to ADCINA2 and ADCINB2
    AdcRegs.ADCSOC2CTL.bit.TRIGSEL      = 7;    // SOC2 triggered by ePWM2.ADCSOCA
    AdcRegs.ADCSOC2CTL.bit.ACQPS        = 6;    // set SOC2 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)
    AdcRegs.ADCSOC3CTL.bit.ACQPS        = 6;    // set SOC3 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)
    EDIS;

    // Assumes ePWM2 clock is already enabled in InitSysCtrl(); ePWM2 triggers resolver measurement
    EPwm2Regs.ETSEL.bit.SOCAEN  = 1;            // Enable SOC on A group
    EPwm2Regs.ETSEL.bit.SOCASEL = 3;            // Select SOC on CTR=0 OR CTR=PRD
    EPwm2Regs.ETPS.bit.SOCAPRD  = 1;            // Generate pulse on 1st event
}

void Adc4_Config(){
//
//Configuration for the voltage measurement:
//

    EALLOW;
    AdcRegs.ADCSAMPLEMODE.bit.SIMULEN2 = 1;     // SOC2 & SOC3

    AdcRegs.ADCSOC4CTL.bit.CHSEL    = 4;        //SOC4 TO A4 & b4

    // set SOC4 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)
    AdcRegs.ADCSOC4CTL.bit.ACQPS    = 6;

    EDIS;
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
