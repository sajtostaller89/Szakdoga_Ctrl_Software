#ifndef _RESOLVER_H_
#define _RESOLVER_H_
/******************************************************************************
* TIDA-01527 Discrete Resolver Front-end
* Module: Resolver signal processing
* Author: Jiri Panacek, j-panacek@ti.com
* Version: v1.0
*
* Copyright 2018 Texas Instruments Incorporated. All rights reserved.
******************************************************************************/
/*note: GLOBAL_Q needs to be defined here or changed in the library */
#define GLOBAL_Q                19 /* range from -4096 to 4095.999 998 093 */

#include "IQmathLib.h" /* Fixed point Q-numbers based math libarry */
#include "F2806x_Device.h" /* registers and types  definition */

/******************************************************************************
 * MACROS
 *****************************************************************************/
/* Simple true/false definition */
#define FALSE 0
#define TRUE !(FALSE)

/* Excitation Amplifier */
#define EXC_STEPS_PER_PERIOD    64  /* 2^6 -> 6-bit resolution per EXC period */
#define EXC_DUTY_MAX            255 /* 2^8 -> 0..255 for duty resolution */

/* Default parameters */
#define EXC_GAIN_DEFAULT        _IQ(0.90)
#define SIG_SAMPL_ANGLE         17
#define RES_FLT_ALPHA           _IQ(0.15)
#define RES_NORMAL_MODE         0
#define RES_SCATTERED_MODE      1

/******************************************************************************
 * ENUMS
 *****************************************************************************/

/******************************************************************************
 * TYPEDEFS
 *****************************************************************************/
 typedef struct
 {
     /* Excitation Angle */
     volatile union
     {
         Uint32 ival; /* integer for cycles */
         _iq6 iqval; /* IQ number for calculations */
     } ExcAngle;

     /* Sample Angle */
     volatile union
     {
         Uint32 ival; /* integer for cycles & comparison */
         _iq6 iqval; /* IQ number for calculations & user input */
     } SigSamplAngle;

     /* Exciter Sample Angle */
     volatile union
     {
         Uint32 ival; /* integer for cycles & comparison */
         _iq6 iqval; /* IQ number for calculations & user input */
     } ExcSamplAngle;

     /* ePW duty cycle modulation index (gain)  */
     _iq ExcGain;

     /* Output Angle */
     volatile _iq Angle; /* Actual value */
     volatile _iq AngleFlt; /* Low-pass filtered */

     /* ADC data scaled to 1 (+/- 0.5) */
     _iq Vsin; /* SIN winding amplitude with DC offset elimination (bipolar) */
     _iq Vcos; /* COS winding amplitude with DC offset elimination (bipolar) */
     _iq Vexc; /* EXC winding amplitude with DC offset elimination (bipolar) */
     /**/
     _iq VsinInv; /* SIN winding amplitude in inverted mode (bipolar) */
     _iq VcosInv; /* COS winding amplitude in inverted mode (bipolar) */

     /* RAW ADC data (practically for debug purposes only) */
     Uint16 Vsin_raw;
     Uint16 Vcos_raw;
     Uint16 Vexc_raw;
     Uint16 VsinInv_raw;
     Uint16 VcosInv_raw;
     Uint16 DCoffsetSIN_raw;
     Uint16 DCoffsetCOS_raw;
     Uint16 DCoffsetEXC_raw;

     /* DC offset ADC data scaled to 1 (0..1) */
     _iq DCoffsetSIN; /* SIN winding */
     _iq DCoffsetCOS; /* COS winding*/
     _iq DCoffsetEXC; /* EXC winding */

     /* Single-pole low-pass filter settings */
     /* read more here: https://www.embeddedrelated.com/showarticle/779.php */
     _iq AlphaFLT;

     /* Flags & configs */
     struct
     {
     volatile Uint16 ePWMisrExecuted :1 ; /* every PWM ISR sets this to one */
     volatile Uint16 ADCsampleTaken  :1 ; /* every ADC read-out sets this to one */
     volatile Uint16 Mode            :1 ; /* defines normal or scattered mode */
     }Flags;
 } res_t;
/******************************************************************************
 * VARIABLES, CONSTANTS
 *****************************************************************************/
 
/******************************************************************************
 * SYSTEM INTERFACE FUNCTIONS PROTOTYPES
 *****************************************************************************/

/* Module Initialization */
void res_Init (volatile res_t *res_p);

/* Resolver sub-System Calibration */
void res_DCoffsetCalibration(volatile res_t *res_p);

/* Find the ideal sampling angle (time) (resolver dependent) */
void res_FindSamplAngle(volatile res_t *res_p);

/* Diagnostics */
void res_Diag (volatile res_t *res_p);

/* Interrupt service routine executed every new PWM period */
#pragma CODE_SECTION(res_PWMInterrupt, "ramfuncs")
void res_PWMInterrupt(volatile res_t *res_p);

#endif
