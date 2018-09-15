/******************************************************************************
* TIDA-01527 Discrete Resolver Front-end
* Module: MAIN
* Author: Jiri Panacek, j-panacek@ti.com
* Version: v1.0
*
* Copyright 2018 Texas Instruments Incorporated. All rights reserved.
******************************************************************************/

#include "F2806x_Device.h" /* registers and types  definition */
#include "hal.h" /* Hardware Abstraction Layer */
#include "resolver.h" /* Resolver Signal Processing */

/******************************************************************************
 * MACROS
 *****************************************************************************/

/******************************************************************************
 * ENUMS
 *****************************************************************************/

/******************************************************************************
 * TYPEDEFS
 *****************************************************************************/

/******************************************************************************
 * VARIABLES, CONSTANTS
 *****************************************************************************/

volatile res_t res; /* this complex structure keeps all resolver data */

/******************************************************************************
* INTERNAL FUNCTIONS PROTOTYPES
*****************************************************************************/

/******************************************************************************
 * INTERNAL FUNCTIONS
 *****************************************************************************/

/******************************************************************************
 * MAIN FUNCTION
 *****************************************************************************/

void main(void)
{
    /* init the micro */
    hal_InitSysCtrl();
    hal_GlobalIsrEnable();
    /* init the resolver sub-system */
    res_Init(&res);
    res_DCoffsetCalibration(&res); /* auto-calibrate the system */
    res_FindSamplAngle(&res); /* pick the ideal sampling time */

    /* main loop */
    for (;;)
    {
        HAL_DELAY_US(100000); /* simple blocking delay */
        hal_toggle_LED(); /* the LED on the LaunchXL blinks */
        res_Diag(&res); /* check resolver subsystem status */
    }
}

/******************************************************************************
 * INTERRUPTS
 *****************************************************************************/

/* This ISR occurs every new PWM period */
#pragma CODE_SECTION(epwm1_isr, "ramfuncs")
__interrupt void epwm1_isr(void)
{
    res_PWMInterrupt(&res); /* resolver routine */
    hal_ClearePWMFlags(); /* clear PWM & PIE flags */
}
