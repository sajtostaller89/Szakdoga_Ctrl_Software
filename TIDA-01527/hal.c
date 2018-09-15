/******************************************************************************
* TIDA-01527 Discrete Resolver Front-end
* Module: Hardware Abstraction Layer
* Author: Jiri Panacek, j-panacek@ti.com
* Version: v1.0
*
* Copyright 2017 Texas Instruments Incorporated. All rights reserved.
******************************************************************************/

#include "hal.h"
#include <string.h> /* strings and arrays (for mcmcpy) */

/******************************************************************************
 * MACROS
 *****************************************************************************/

/* calibration routine for internal oscillators and ADC */
#define Device_cal      (void(*)(void)) 0x3D7C80

/* PLL module timing (80 MHz) */
#define DSP28_DIVSEL    2
#define DSP28_PLLCR     16

/******************************************************************************
 * ENUMS
 *****************************************************************************/

/******************************************************************************
 * TYPEDEFS
 *****************************************************************************/

/******************************************************************************
 * VARIABLES, CONSTANTS
 *****************************************************************************/

/* ram functions stored in FLASH */
extern Uint16 RamfuncsLoadStart; /* for transferring functions from FLASH */
extern Uint16 RamfuncsRunStart;  /* to RAM after reset */
extern Uint16 RamfuncsLoadSize;

/******************************************************************************
 * INTERNAL FUNCTIONS PROTOTYPES
 *****************************************************************************/

/* Disable internal WDT */
void hal_DisableWDT(void);

/* Use internal oscillator (10 Mhz) and disable the others */
void hal_IntOsc1Sel(void);

/* Initialize PLL */
void hal_InitPll(Uint16 val, Uint16 divsel);

/* Initialize FLASH - must be executed from RAM */
void hal_InitFlash(void);

/* Initialize clock for peripherals */
void hal_InitPeripheralClocks(void);

/* Initialize GPIOs settings */
void hal_InitGPIOs(void);

/* Initialize PIE periphery */
void hal_InitPIE(void);

/* HAL Assert */
void hal_Assert(void);

/* Initialize ePWM periphery */
void hal_InitPWM(void);

/* Initialize AD converter */
void hal_InitADC(void);

/******************************************************************************
 * INTERNAL FUNCTIONS
 *****************************************************************************/

/* Disable internal WDT */
void hal_DisableWDT(void)
{
    EALLOW;
    SysCtrlRegs.WDCR = 0x0068;
    EDIS;
}

/* Use internal oscillator (10 Mhz) and disable the others */
void hal_IntOsc1Sel(void)
{
    EALLOW;
    SysCtrlRegs.CLKCTL.bit.INTOSC1OFF = 0;   /* enable INTOSC1 if is disabled */
    SysCtrlRegs.CLKCTL.bit.OSCCLKSRCSEL = 0; /* switch clk source to INTOSC1 */
    SysCtrlRegs.CLKCTL.bit.XCLKINOFF = 1;    /* Turn off XCLKIN */
    SysCtrlRegs.CLKCTL.bit.XTALOSCOFF = 1;   /* Turn off XTALOSC */
    SysCtrlRegs.CLKCTL.bit.INTOSC2OFF = 1;   /* Turn off INTOSC2 */
    EDIS;
}

/* Initialize PLL */
void hal_InitPll(Uint16 val, Uint16 divsel)
{
    EALLOW;

    /* Make sure that the PLL is not running in the limp mode */
    if (SysCtrlRegs.PLLSTS.bit.MCLKSTS != 0)
    {
        hal_Assert();
    }

    /* DIVSEL MUST be 0 before PLLCR can be changed from 0x0000. */
    if (SysCtrlRegs.PLLSTS.bit.DIVSEL != 0)
    {
        SysCtrlRegs.PLLSTS.bit.DIVSEL = 0;
    }

    /* Change the PLLCR */
    if (SysCtrlRegs.PLLCR.bit.DIV != val)
    {
        /* disable missing clock detect logic before setting PLLCR */
        SysCtrlRegs.PLLSTS.bit.MCLKOFF = 1;
        /* set clock ratio */
        SysCtrlRegs.PLLCR.bit.DIV = val;
        /* enable missing clock detect logic */
        SysCtrlRegs.PLLSTS.bit.MCLKOFF = 0;
    }

    /* CLKIN/1 - check with the datasheet first ! */
    if (divsel == 3)
    {
        /* two step operation */
        SysCtrlRegs.PLLSTS.bit.DIVSEL = 2;
        HAL_DELAY_US(50L);
        SysCtrlRegs.PLLSTS.bit.DIVSEL = 3;
    }
    /* CLKIN/2 or CLKIN/4 */
    else
    {
        SysCtrlRegs.PLLSTS.bit.DIVSEL = divsel;
    }

    EDIS;
}

/* Initialize FLASH - must be executed from RAM */
#pragma CODE_SECTION(hal_InitFlash, "ramfuncs")
void hal_InitFlash(void)
{
    EALLOW;
    FlashRegs.FOPT.bit.ENPIPE = 1;        /* FLASH pipeline mode enable */
    FlashRegs.FBANKWAIT.bit.PAGEWAIT = 1; /* page WS - 1 @ 50 MHz */
    FlashRegs.FBANKWAIT.bit.RANDWAIT = 1; /* random WS - 1 @ 50 MHz */
    FlashRegs.FOTPWAIT.bit.OTPWAIT = 2;   /* otp WS - 2 @ 50 MHz */
    EDIS;
    asm(" RPT #7 || NOP"); /* force pipeline flush to finish reg. write before return */
}

/* Initialize clock for peripherals */
void hal_InitPeripheralClocks(void)
{
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 1;       /* ADC */
    SysCtrlRegs.PCLKCR3.bit.COMP1ENCLK = 0;     /* COMP1 */
    SysCtrlRegs.PCLKCR3.bit.COMP2ENCLK = 0;     /* COMP2 */
    SysCtrlRegs.PCLKCR3.bit.CPUTIMER0ENCLK = 1; /* CPU Timer-0 */
    SysCtrlRegs.PCLKCR3.bit.CPUTIMER1ENCLK = 0; /* CPU Timer-1 */
    SysCtrlRegs.PCLKCR3.bit.CPUTIMER2ENCLK = 0; /* CPU Timer-2 */
    SysCtrlRegs.PCLKCR1.bit.ECAP1ENCLK = 0;     /* eCAP1 */
    SysCtrlRegs.PCLKCR1.bit.EPWM1ENCLK = 1;     /* EPWM1 */
    SysCtrlRegs.PCLKCR1.bit.EPWM2ENCLK = 0;     /* EPWM2 */
    SysCtrlRegs.PCLKCR1.bit.EPWM3ENCLK = 0;     /* EPWM3 */
    SysCtrlRegs.PCLKCR1.bit.EPWM4ENCLK = 0;     /* EPWM4 */
    SysCtrlRegs.PCLKCR0.bit.HRPWMENCLK = 0;     /* HRPWM */
    SysCtrlRegs.PCLKCR0.bit.I2CAENCLK = 0;      /* I2C */
    SysCtrlRegs.PCLKCR0.bit.SCIAENCLK = 0;      /* SCI-A */
    SysCtrlRegs.PCLKCR0.bit.SPIAENCLK = 0;      /* SPI-A */
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;      /* Enable TBCLK within the EPWM */
    EDIS;
}

/* GPIOs settings */
void hal_InitGPIOs(void)
{
    EALLOW;
    /* Group A pins */
    GpioCtrlRegs.GPACTRL.all  = 0x00000000;     /* QUALPRD = SYSCLKOUT for all group A GPIO */
    GpioCtrlRegs.GPAQSEL1.all = 0x00000000;     /* No qualification for all group A GPIO 0-15 */
    GpioCtrlRegs.GPAQSEL2.all = 0x00000000;     /* No qualification for all group A GPIO 16-31 */
    GpioCtrlRegs.GPAMUX1.all  = 0x00000001;     /* Alternate functions for all group A GPIO 0-15 */
    GpioCtrlRegs.GPAMUX2.all  = 0x00000000;     /* Alternate functions for all group A GPIO 16-31 */
    GpioCtrlRegs.GPADIR.all   = 0x0000000E;     /* Group A inputs (0) / outputs (1) */
    GpioCtrlRegs.GPAPUD.all   = 0x00000000;     /* Group A pull-ups */
    /* Group B pins */
    GpioCtrlRegs.GPBCTRL.all  = 0x00000000;     /* QUALPRD = SYSCLKOUT for all group B GPIO */
    GpioCtrlRegs.GPBQSEL1.all = 0x00000000;     /* No qualification for all group B GPIO 32-38 */
    GpioCtrlRegs.GPBMUX1.all  = 0x00000000;     /* Alternate functions for all group B GPIO 32-38 */
    GpioCtrlRegs.GPBDIR.all   = 0x00000004;     /* Group B inputs (0) / outputs (1) */
    GpioCtrlRegs.GPBPUD.all   = 0x00000000;     /* Group B pull-ups */
    /* Analog Pins */
    EDIS;
}

void hal_InitPIE(void)
{
    DINT;
    EALLOW;
    /* Disable the PIE */
    PieCtrlRegs.PIECTRL.bit.ENPIE = 0;
    /* Disable all peripheral interrupts  */
    PieCtrlRegs.PIEIER1.all = 0;
    PieCtrlRegs.PIEIER2.all = 0;
    PieCtrlRegs.PIEIER3.all = 0;
    PieCtrlRegs.PIEIER4.all = 0;
    PieCtrlRegs.PIEIER5.all = 0;
    PieCtrlRegs.PIEIER6.all = 0;
    PieCtrlRegs.PIEIER7.all = 0;
    PieCtrlRegs.PIEIER8.all = 0;
    PieCtrlRegs.PIEIER9.all = 0;
    PieCtrlRegs.PIEIER10.all = 0;
    PieCtrlRegs.PIEIER11.all = 0;
    PieCtrlRegs.PIEIER12.all = 0;
    /* Clear all flags */
    PieCtrlRegs.PIEIFR1.all = 0;
    PieCtrlRegs.PIEIFR2.all = 0;
    PieCtrlRegs.PIEIFR3.all = 0;
    PieCtrlRegs.PIEIFR4.all = 0;
    PieCtrlRegs.PIEIFR5.all = 0;
    PieCtrlRegs.PIEIFR6.all = 0;
    PieCtrlRegs.PIEIFR7.all = 0;
    PieCtrlRegs.PIEIFR8.all = 0;
    PieCtrlRegs.PIEIFR9.all = 0;
    PieCtrlRegs.PIEIFR10.all = 0;
    PieCtrlRegs.PIEIFR11.all = 0;
    PieCtrlRegs.PIEIFR12.all = 0;
    /* Application Specific */
    PieVectTable.EPWM1_INT = &epwm1_isr;    /* map ePWM ISR */
    PieCtrlRegs.PIEIER3.bit.INTx1 = 1;      /* Enable group 3, interrupt 1 (ePWM1) */
    IER |= M_INT3;                          /* Enable CPU interrupt from group 3 */
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;      /* Enable group 1, interrupt 7 (TINT0) */
    IER |= M_INT1;                          /* Enable CPU interrupt from group 1 */
    /* Enable the PIE controller */
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;
    PieCtrlRegs.PIEACK.all = 0xFFFF;
    /* enable CPU interrupts */
    EINT;
    EDIS;
}

/* HAL Assert */
void hal_Assert(void)
{
    while (1)
    {
        /* Temporary or forever ? */
        asm(" NOP");
    }
}

/* PWM Init */
void hal_InitPWM(void)
{
    /* Setup TBCLK */
    EPwm1Regs.TBCTL.bit.CTRMODE = 0x0;      /* Count up */
    EPwm1Regs.TBPRD = 0x00FF;               /* Set timer period */
    EPwm1Regs.TBCTL.bit.PHSEN = 0x0;        /* Disable phase loading */
    EPwm1Regs.TBCTR = 0x0000;               /* Clear counter */
    EPwm1Regs.TBCTL.bit.HSPCLKDIV = 0x0;    /* Clock ratio to SYSCLKOUT */
    EPwm1Regs.TBCTL.bit.CLKDIV = 0x0;
    EPwm1Regs.CMPCTL.bit.SHDWAMODE = 0;
    EPwm1Regs.CMPCTL.bit.LOADAMODE = 0x00;
    EPwm1Regs.CMPA.half.CMPA = 0x00;        /* default duty */
    EPwm1Regs.AQCTLA.bit.ZRO = 0x2;         /* Set PWM1A on Zero */
    EPwm1Regs.AQCTLA.bit.CAU = 0x1;         /* Clear PWM1A on event A, up count */
    EPwm1Regs.ETSEL.bit.INTSEL = 0x1;       /* Select INT on Zero event */
    EPwm1Regs.ETSEL.bit.INTEN = 1;          /* Enable INT */
    EPwm1Regs.ETPS.bit.INTPRD = 0x1;        /* Generate INT on 1st event */
}

/* ADC Init */
void hal_InitADC(void)
{
    EALLOW;
    /**/
    AdcRegs.ADCCTL1.bit.ADCREFSEL = 0;      /* Select internal VREF */
    AdcRegs.ADCCTL1.bit.ADCPWDN   = 1;      /* Power ADC */
    AdcRegs.ADCCTL1.bit.ADCBGPWD  = 1;      /* Power ADC BG */
    AdcRegs.ADCCTL1.bit.ADCREFPWD = 1;      /* Power reference */
    AdcRegs.ADCCTL1.bit.ADCENABLE = 1;      /* Enable ADC */
    /* power-up delay 5 ms */
    HAL_DELAY_US(5000);
    /**/
    AdcRegs.ADCCTL2.bit.CLKDIV2EN = 0;      /* clk = 80 MHz; F29069 should not have linearity and INL problem as F28027 */
    AdcRegs.ADCCTL2.bit.ADCNONOVERLAP = 1;  /* one conversion after another */
    AdcRegs.ADCSAMPLEMODE.bit.SIMULEN0 = 1; /* SOC0 & SOC1 at the same time */
    AdcRegs.ADCSAMPLEMODE.bit.SIMULEN2 = 1; /* SOC2 & SOC3 at the same time */
    /* OSIN sensing */
    AdcRegs.ADCSOC0CTL.bit.CHSEL  = 0x0;    /* ADCINA0 is the input */
    AdcRegs.ADCSOC0CTL.bit.ACQPS  = 30;     /* 80 MHz clk => 12.5 ns/cycle; 12+13 ADC cycles -> 312.5 ns / SOC */
    AdcRegs.ADCSOC2CTL.bit.CHSEL  = 0x0;
    AdcRegs.ADCSOC2CTL.bit.ACQPS  = 30;
    /* OCOS sensing */
    AdcRegs.ADCSOC1CTL.bit.CHSEL  = 0x8;    /* ADCINB0 is the input */
    AdcRegs.ADCSOC1CTL.bit.ACQPS  = 30;
    AdcRegs.ADCSOC3CTL.bit.CHSEL  = 0x8;
    AdcRegs.ADCSOC3CTL.bit.ACQPS  = 30;
    /* EXC sensing */
    AdcRegs.ADCSOC4CTL.bit.CHSEL  = 0x1;    /* ADCINA1 is the input */
    AdcRegs.ADCSOC4CTL.bit.ACQPS  = 30;
    /**/
    AdcRegs.ADCCTL1.bit.INTPULSEPOS = 1;    /* ADCINT1 trips after AdcResults latch */
    AdcRegs.INTSEL1N2.bit.INT1E     = 1;    /* Enabled ADCINT1 (PIE ignored) */
    AdcRegs.INTSEL1N2.bit.INT1CONT  = 1;    /* Enable ADCINT1 Continuous mode */
    /**/
    AdcRegs.ADCINTSOCSEL1.bit.SOC0 = 1;     /* SOC0 triggers SOC1 */
    AdcRegs.ADCINTSOCSEL1.bit.SOC2 = 1;     /* SOC2 triggers SOC3 */
    AdcRegs.ADCINTSOCSEL1.bit.SOC4 = 1;     /* SOC4 */
    /**/
    AdcRegs.INTSEL1N2.bit.INT1SEL = 0x04;   /* EOC4 triggers the interrupt */
    /* Initial Trigger of ADC (then triggered automatically by interrupt (EoC of SOC3) */
    AdcRegs.ADCSOCFRC1.bit.SOC4 = 1;
    EDIS;
}

/******************************************************************************
 * SYSTEM INTERFACE FUNCTIONS
 *****************************************************************************/

/* System clock & control init */
void hal_InitSysCtrl(void)
{
    /* disable internal watchdog */
    hal_DisableWDT();
    /* copy FLASH code which runs from the RAM */
    memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, (Uint32)&RamfuncsLoadSize);
    /* use int. oscillator (10 MHz), disable the others */
    hal_IntOsc1Sel();
    /* Enable PLL (80 MHz, 12.5ns instruction cycle) */
    hal_InitPll(DSP28_PLLCR, DSP28_DIVSEL);
    /* Clock for peripherals */
    hal_InitPeripheralClocks();
    /* Init FLASH */
    hal_InitFlash();
    /* Init GPIOs */
    hal_InitGPIOs();
    /* Init PIE */
    hal_InitPIE();
    /* Init ePWM */
    hal_InitPWM();
    /* Init ADC */
    hal_InitADC();
}

/* disable all interrupts */
void hal_GlobalIsrDisable(void)
{
    DINT;
}

/* enable global interrupts */
void hal_GlobalIsrEnable(void)
{
    EINT;
}

/* toggle LED on the launchpad */
void hal_toggle_LED(void)
{
    GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;
}

/* clear ePWM interrupt flags - for main.c */
#pragma CODE_SECTION(hal_ClearePWMFlags, "ramfuncs")
void hal_ClearePWMFlags(void)
{
    EPwm1Regs.ETCLR.bit.INT = 1; /* clear ePWM peripheral flag */
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3; /* clear PIE flag */
}
