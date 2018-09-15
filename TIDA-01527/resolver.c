/******************************************************************************
* TIDA-01527 Discrete Resolver Front-end
* Module: Resolver signal processing
* Author: Jiri Panacek, j-panacek@ti.com
* Version: v1.0
*
* Copyright 2018 Texas Instruments Incorporated. All rights reserved.
******************************************************************************/

#include "resolver.h"
#include "hal.h" /* hardware abstraction layer */

/******************************************************************************
 * MACROS
 *****************************************************************************/

/* Control the analog crossbar */
#define RES_DISCONNECT_CROSSBAR     HAL_INH_CLR;
#define RES_CONNECT_CROSSBAR        HAL_INH_SET;

/* Empty cycle */
#define DO_NOTHING

/******************************************************************************
 * ENUMS
 *****************************************************************************/

/******************************************************************************
 * TYPEDEFS
 *****************************************************************************/

/******************************************************************************
 * VARIABLES, CONSTANTS
 *****************************************************************************/

/******************************************************************************
 * INTERNAL FUNCTIONS PROTOTYPES
 *****************************************************************************/

/* this function calculates resolver angle */
void res_CalcAngle(volatile res_t *res_p);

/* this function calculates resolver angle in scattered mode */
void res_CalcAngleScatered(volatile res_t *res_p);

/******************************************************************************
 * INTERNAL FUNCTIONS
 *****************************************************************************/

/* this function calculates resolver angle */
void res_CalcAngle(volatile res_t *res_p)
{
    /* we use 4-quadrant arctan2 (see wiki) */
    res_p->Angle = _IQ19atan2PU(res_p->Vsin, res_p->Vcos);
    /* I do low-pass filtering here */
    res_p->AngleFlt += _IQmpy(res_p->AlphaFLT, (res_p->Angle - res_p->AngleFlt));
    /* credits: https://www.embeddedrelated.com/showarticle/779.php */
}

/* this function calculates resolver angle in scattered mode */
void res_CalcAngleScatered(volatile res_t *res_p)
{
    /* we use 4-quadrant arctan2 (see wiki) and the result from normal and ivnerted mode */
    res_p->Angle = _IQ19atan2PU((res_p->Vsin + res_p->VsinInv), (res_p->Vcos + res_p->VcosInv));
    /* I do low-pass filtering here */
    res_p->AngleFlt += _IQmpy(res_p->AlphaFLT, (res_p->Angle - res_p->AngleFlt));
    /* credits: https://www.embeddedrelated.com/showarticle/779.php */
}

/******************************************************************************
* SYSTEM INTERFACE FUNCTIONS
*****************************************************************************/

/* Module Initialization */
void res_Init(volatile res_t *res_p)
{
    /* laod default values */
    res_p->ExcGain = EXC_GAIN_DEFAULT;
    res_p->ExcAngle.iqval = _IQ(0.0);
    res_p->SigSamplAngle.ival =  _IQ(0.0);
    res_p->ExcSamplAngle.iqval = _IQ(0.0);
    res_p->AlphaFLT = RES_FLT_ALPHA;
    res_p->Flags.Mode = RES_NORMAL_MODE;
}

/* Resolver sub-System Calibration */
void res_DCoffsetCalibration(volatile res_t *res_p)
{
    Uint16 i;
    _iq tmpExcGain;
    /* Disable PWM, disconnect resolver */
    RES_DISCONNECT_CROSSBAR;
    tmpExcGain = res_p->ExcGain;
    res_p->ExcGain = _IQ(0.0); /* disable exciter amp */
    HAL_DELAY_US(1000);
    /* Reset calibration values */
    res_p->DCoffsetSIN = _IQ(0.0);
    res_p->DCoffsetCOS = _IQ(0.0);
    res_p->DCoffsetEXC = _IQ(0.0);

    /* measure zero signal multiple times and average */
    for (i = 0; i < 32; i++)
    {
        res_p->DCoffsetSIN += _IQ12toIQ(HAL_ADC_RESULT_SIN);
        res_p->DCoffsetCOS += _IQ12toIQ(HAL_ADC_RESULT_COS);
        res_p->DCoffsetEXC += _IQ12toIQ(HAL_ADC_RESULT_EXC);
        HAL_DELAY_US(1000);
    }

    /* division finalizes averageing */
    res_p->DCoffsetSIN >>= 5;
    res_p->DCoffsetCOS >>= 5;
    res_p->DCoffsetEXC >>= 5;
    /* RAW */
    res_p->DCoffsetSIN_raw = _IQtoIQ12(res_p->DCoffsetSIN);
    res_p->DCoffsetCOS_raw = _IQtoIQ12(res_p->DCoffsetCOS);
    res_p->DCoffsetEXC_raw = _IQtoIQ12(res_p->DCoffsetEXC);
    /* Restore PWM, connect resolver */
    RES_CONNECT_CROSSBAR;
    res_p->ExcGain = tmpExcGain; /* re-enable exciter amp */
}

/* Find the ideal sampling angle (resolver dependent) */
void res_FindSamplAngle(volatile res_t *res_p)
{
    Uint16 i, newSigSamplAngle, newExcSamplAngle;
    _iq newSigMax = _IQ(0.0);
    _iq newExcMax = _IQ(0.0);

    /* walk through all sampling angles */
    for (i = 0; i < EXC_STEPS_PER_PERIOD; i++)
    {
        _iq tmpVal;
        res_p->SigSamplAngle.ival = i;
        res_p->ExcSamplAngle.ival = i;
        res_p->Flags.ADCsampleTaken = FALSE;

        /* wait until the sample is taken */
        while (FALSE == res_p->Flags.ADCsampleTaken) {DO_NOTHING};

        /* pick the signal with higher amplitude */
        if (_IQabs(res_p->Vsin) > _IQabs(res_p->Vcos)) { tmpVal = _IQabs(res_p->Vsin);}
        else {tmpVal = _IQabs(res_p->Vcos);}

        /* compare the amplitude with historical maximum */
        if (tmpVal > newSigMax)
        {
            newSigMax = tmpVal;
            newSigSamplAngle = i;
        }

        /* same thing for the excitation signal */
        tmpVal = _IQabs(res_p->Vexc);

        if (tmpVal > newExcMax)
        {
            newExcMax = tmpVal;
            newExcSamplAngle = i;
        }
    }

    /* save the new preferred sampling angle */
    res_p->SigSamplAngle.ival = newSigSamplAngle;
    res_p->ExcSamplAngle.ival = newExcSamplAngle;
}

/* Diagnostics */
void res_Diag(volatile res_t *res_p)
{
    /* DC offsets */
    /* Excitation Amplifier */
    /* Voltage References */
    /* Sorry, will be implemented in v2.0 */
}

/* Interrupt service routine executed every new PWM period */
#pragma CODE_SECTION(res_PWMInterrupt, "ramfuncs")
void res_PWMInterrupt(volatile res_t * res_p)
{
    _iq duty;
    /* Generate sinewave -> update PWM dutycycle */
    /* higher Q format (GLOBAL_Q) reduces truncation error */
    duty = _IQmpy((_IQ(1) + _IQmpy(_IQsinPU(_IQ6toIQ(res_p->ExcAngle.iqval)), res_p->ExcGain)), _IQ(0.5)); /* D=<0..1> */
    /* scale duty <0..1> to match PWM resolution <0..255> */
    HAL_SET_DUTY(_IQint(_IQmpy(_IQ(EXC_DUTY_MAX), duty)));

    /* Resolver signal sampling */
    if (res_p->SigSamplAngle.ival == res_p->ExcAngle.ival)
    {
        /* normal mode */
        if (res_p->Flags.Mode == RES_NORMAL_MODE)
        {
            /* get ADC RAW values */
            res_p->Vsin_raw = HAL_ADC_RESULT_SIN;
            res_p->Vcos_raw = HAL_ADC_RESULT_COS;
            /* convert to bipolar signals in IQ format */
            res_p->Vsin = (_IQ12toIQ(res_p->Vsin_raw) - res_p->DCoffsetSIN);
            res_p->Vcos = (_IQ12toIQ(res_p->Vcos_raw) - res_p->DCoffsetCOS);
            /* You can either calculate the angle immediately or later in the background loop */
            res_CalcAngle(res_p);
            /* clear the flag */
            res_p->Flags.ADCsampleTaken = TRUE;
            /* todo: smarter solution for situation when transfering from scatter to normal mode: */
            HAL_CTRL_CLR;
        }
        /* scattered mode */
        else
        {
            /* direct (normal) connection */
            if (!HAL_CTRL_STAT)
            {
                /* get ADC RAW values */
                res_p->Vsin_raw = HAL_ADC_RESULT_SIN;
                res_p->Vcos_raw = HAL_ADC_RESULT_COS;
                /* convert to bipolar signals in IQ format */
                res_p->Vsin = (_IQ12toIQ(res_p->Vsin_raw) - res_p->DCoffsetSIN);
                res_p->Vcos = (_IQ12toIQ(res_p->Vcos_raw) - res_p->DCoffsetCOS);
                /**/
                HAL_CTRL_SET;
            }
            /* twisted connection */
            else
            {
                /* get ADC RAW values */
                res_p->VcosInv_raw = HAL_ADC_RESULT_SIN;
                res_p->VsinInv_raw = HAL_ADC_RESULT_COS;
                /* convert to bipolar signals in IQ format */
                res_p->VcosInv = (_IQ12toIQ(res_p->VcosInv_raw) - res_p->DCoffsetSIN);
                res_p->VsinInv = (_IQ12toIQ(res_p->VsinInv_raw) - res_p->DCoffsetCOS);
                /**/
                HAL_CTRL_CLR;
            }

            /* calculate the angle in scattered mode */
            /* todo: the calculation now may use old samples, implement system which performs
             * calulcation every second time. */
            res_CalcAngleScatered(res_p);
        }
    }

    /* Excitation amplifier signal sampling */
    if (res_p->ExcSamplAngle.ival == res_p->ExcAngle.ival)
    {
        /* RAW */
        res_p->Vexc_raw = HAL_ADC_RESULT_EXC;
        /* IQ */
        res_p->Vexc = (_IQ12toIQ(res_p->Vexc_raw) - res_p->DCoffsetEXC);
    }

    /* Excitation angle update */
    if (res_p->ExcAngle.ival < (EXC_STEPS_PER_PERIOD - 1))
    {   ++res_p->ExcAngle.ival;}
    else
    {   res_p->ExcAngle.ival = 0;}

    /* Synchronization flags */
    res_p->Flags.ePWMisrExecuted = TRUE;
}
