#ifndef _HAL_H_
#define _HAL_H_
/******************************************************************************
* TIDA-01527 Discrete Resolver Front-end
* Module: Hardware Abstraction Layer
* Author: Jiri Panacek, j-panacek@ti.com
* Version: v1.0
*
* Copyright 2018 Texas Instruments Incorporated. All rights reserved.
******************************************************************************/

#include "F2806x_Device.h" /* registers and types  definition */

/******************************************************************************
 * MACROS
 *****************************************************************************/

/* Clock source details */
#define CPU_RATE       12.500L /* for 80 MHz CPU clock speed (SYSCLKOUT) */

/* short delay, reused from F2806x_Examples.h */
#define HAL_DELAY_US(A)  DSP28x_usDelay(((((long double) A * 1000.0L) /   \
                     (long double)CPU_RATE) - 9.0L) / 5.0L)

/* set duty cycle */
#define HAL_SET_DUTY(x)     EPwm1Regs.CMPA.half.CMPA = x

/* control analog multiplex */
/* INH pin */
#define HAL_INH_SET         GpioDataRegs.GPASET.bit.GPIO2 = 1
#define HAL_INH_CLR         GpioDataRegs.GPACLEAR.bit.GPIO2 = 1
#define HAL_INH_TGL         GpioDataRegs.GPATOGGLE.bit.GPIO2 = 1

/* CTRL pin */
#define HAL_CTRL_SET        GpioDataRegs.GPASET.bit.GPIO1 = 1
#define HAL_CTRL_CLR        GpioDataRegs.GPACLEAR.bit.GPIO1 = 1
#define HAL_CTRL_TGL        GpioDataRegs.GPATOGGLE.bit.GPIO1 = 1
#define HAL_CTRL_STAT       GpioDataRegs.GPADAT.bit.GPIO1

/* DEBUG pin */
#define HAL_DBG_SET         GpioDataRegs.GPASET.bit.GPIO3 = 1
#define HAL_DBG_CLR         GpioDataRegs.GPACLEAR.bit.GPIO3 = 1
#define HAL_DBG_TGL         GpioDataRegs.GPATOGGLE.bit.GPIO3 = 1

/* ADC registers */
#define HAL_ADC_RESULT_COS  AdcResult.ADCRESULT1
#define HAL_ADC_RESULT_SIN  AdcResult.ADCRESULT0
#define HAL_ADC_RESULT_EXC  AdcResult.ADCRESULT4

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
 * SYSTEM INTERFACE FUNCTIONS PROTOTYPES
 *****************************************************************************/

////////// CPU //////////

/* System clock & control init */
void hal_InitSysCtrl(void);

/* disable all interrupts */
void hal_GlobalIsrDisable(void);

/* enable global interrupts */
void hal_GlobalIsrEnable(void);

////////// ePWM //////////

/* ePWM interrupt routine in main.c */
extern void epwm1_isr(void);

/* clear ePWM interrupt flags - for main.c */
void hal_ClearePWMFlags(void);

////////// GPIO //////////

/* toggle LED on the launchpad */
void hal_toggle_LED(void);

////////// UTILITIES //////////

/* asm delay routine in F2806x_usDelay.asm */
#pragma CODE_SECTION(DSP28x_usDelay, "ramfuncs")
extern void DSP28x_usDelay(Uint32 Count);

#endif
