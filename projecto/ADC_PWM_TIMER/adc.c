/*
 * adc.c
 *
 *  Created on: 2018. szept. 18.
 *      Author: Ákos
 *
 *
 * Current measurement:
 * ADCINA1 ->SOC0 ->PIN29, ADCINB1 ->SOC1 ->PIN24,
 *
 * Resolver measurement:
 * ADCINA2 ->SOC2 ->PIN25, ADCINB2 ->SOC3 ->PIN26,
 *
 * Voltage measurement:
 * ADCINA4 ->SOC4 ->PIN69:
 *
 */

#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include "F2806x_EPwm_defines.h"
#include "pheripherals.h"
#include "math.h"

// Interrupt variables
Uint16 ConversionCount;
float32 Voltage0[100];
float32 Voltage1[100];
float32 value0 = 1.0;
float32 value1 = 0.0;

// Resolver measurement variables
int i,j,lowest0,lowest1, highest0, highest1 = 0;
float32 Angle00, Angle01, Angle10, Angle11 = 0.0;
volatile float32 Theta = 0.0, Theta_out = 0.0;

// Control loop variables
float32 integral1 = 0.0, integral2 = 0.0;
float32 Kp = 50.0, Ki = 100.0, Ts = 0.001; //1kHz
float32 angle_error = 0.0;
float32 speed = 0.0;
float32 Theta_in = 0.0;

float32 control_loop(float32 angle_in){
    angle_error = angle_in-Theta;
    integral1 += Ts*angle_error*Ki;
    if (integral1 > 320) {
        integral1 = 320;
    }
    speed = angle_error*Kp + integral1;
    integral2 += speed*Ts;
    if (integral2 > 6.2832) {
        integral2 = 6.2832;
    }
    return integral2;
}

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
    AdcRegs.ADCSOC0CTL.bit.TRIGSEL      = 11;    // ADCTRIG11 = ePWM4.ADCSOCA, thats what we will set up
    AdcRegs.ADCSOC0CTL.bit.ACQPS        = 6;    // set SOC0 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)
    AdcRegs.ADCSOC1CTL.bit.ACQPS        = 6;    // set SOC1 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)
    EDIS;
}

void Adc2_Config(){
    //Configuration for the resolver signal measurement:
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
}

void Adc4_Config(){
    //Configuration for the voltage measurement:
    EALLOW;
    AdcRegs.INTSEL3N4.bit.INT3E         = 1;    // Enabled ADCINT3
    AdcRegs.INTSEL3N4.bit.INT3CONT      = 0;    // Disable ADCINT3 Continuous mode
    AdcRegs.INTSEL3N4.bit.INT3SEL       = 4;    // setup EOC4 to trigger ADCINT3 to fire, this way an interrupt occurs when both samples are ready
    AdcRegs.ADCSOC4CTL.bit.CHSEL        = 4;    // SOC4 TO A4
    AdcRegs.ADCSOC4CTL.bit.CHSEL        = 4;    // set SOC4 channel select to ADCINA4
    AdcRegs.ADCSOC4CTL.bit.TRIGSEL      = 11;   // SOC4 triggered by ePWM4.ADCSOCA
    AdcRegs.ADCSOC4CTL.bit.ACQPS        = 6;    // set SOC4 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)
    EDIS;
}


//ADC function call
__interrupt void adc1_isr(void)
{

    /*
    value0 = (float32)AdcResult.ADCRESULT0*3.3/4096;
    if(((value0 > 3.3) || (value0 < 0)) && (ConversionCount > 0))
        value0 = Voltage1[ConversionCount-1];
    Voltage0[ConversionCount] = value0;

    value1 = (float32)AdcResult.ADCRESULT1*3.3/4096;
    if(((value1 > 3.3) || (value1 < 0)) && (ConversionCount > 0))
        value1 = Voltage1[ConversionCount-1];
    Voltage1[ConversionCount] = value1;


    // If 2000 conversions have been logged, start over
    if(ConversionCount == 999)
    {
        ConversionCount = 0;
    }
    else
    {
        ConversionCount++;
    }

*/
    // Clear ADCINT1 flag reinitialize for next SOC
    AdcRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;   // Acknowledge interrupt to PIE

    return;
}

//TODO: create the other interrupts, consider if we should do a lot of these stepts here or only set a global and then the control algortihm will do the calculations
__interrupt void adc2_isr(void)
{
    // Change the digital values back to voltage values, and subtract the common mode offset
    value0 = (float32)AdcResult.ADCRESULT2*3.3/4096;
    if(((value0 > 3.3) || (value0 < 0)) && (ConversionCount > 0))
        value0 = Voltage1[ConversionCount-1]+1.65;
    Voltage0[ConversionCount] = value0-1.65;             // cosine

    value1 = (float32)AdcResult.ADCRESULT3*3.3/4096-1.65;       //Consider which way to use
    if(((value1 > 3.3) || (value1 < 0)) && (ConversionCount > 0))
        value1 = Voltage1[ConversionCount-1];
    Voltage1[ConversionCount] = value1;

    // If 100 conversions have been logged for each channel, start over and get the highest and lowest value's indexes
    if(ConversionCount == 99)
    {
        // Search for the lowest and highest value indexes
        for(i = 0; i < 99; i++){
            if (Voltage0[i]<Voltage0[lowest0])
                lowest0 = i;
            if (Voltage0[i]>Voltage0[highest0])
                highest0 = i;
        }
        for(i = 0; i < 99; i++){
            if (Voltage1[i]<Voltage1[lowest1])
                lowest1 = i;
            if (Voltage1[i]>Voltage1[highest1])
                highest1 = i;
        }

        // First method, calculate four angle values, then average them
        Angle00 = atan(Voltage0[lowest0]/Voltage1[lowest0]);  //amuyg ugyan az csak voltage1 a tomb
        Angle01 = atan(Voltage0[highest0]/Voltage1[highest0]);
        Angle10 = atan(Voltage0[lowest1]/Voltage1[lowest1]);
        Angle11 = atan(Voltage0[highest1]/Voltage1[highest1]);

        Theta_in = (Angle00 + Angle01 + Angle10 + Angle11) / 4;
        //Theta_in = (Angle00 + Angle01) / 2;

        Theta_out = control_loop(Theta_in);

        ConversionCount = 0;
    }
    else
    {
        ConversionCount++;
    }

    Theta = Theta_out;
    // Clear ADCINT1 flag reinitialize for next SOC
    AdcRegs.ADCINTFLGCLR.bit.ADCINT2 = 1;

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;   // Acknowledge interrupt to PIE

    return;
}

__interrupt void adc3_isr(void)
{
    // Clear ADCINT1 flag reinitialize for next SOC
    AdcRegs.ADCINTFLGCLR.bit.ADCINT3 = 1;

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP10;   // Acknowledge interrupt to PIE

    return;
}


