#ifndef PERIPHERALS_H_
#define PERIPHERALS_H_

#define GLOBAL_Q                19 /* range from -4096 to 4095.999 998 093 */
#include "IQmathLib.h" /* Fixed point Q-numbers based math library */

//ADC
__interrupt void adc1_isr(void);

void Adc_Config(void);
void Adc1_Config(void);
void Adc2_Config(void);
void Adc4_Config(void);


//ePWM
__interrupt void epwm1_isr(void);
__interrupt void epwm2_isr(void);
__interrupt void epwm3_isr(void);

void EPwm1_Config(void);

// Defines that configure the period for each timer
#define EPWM1_TIMER_TBPRD  3000  // Period register
#define EPWM2_TIMER_TBPRD  2000  // Period register
#define EPWM3_TIMER_TBPRD  2000  // Period register

//Defines to keep track of which way the timer value is moving (by Mate 2018.09.16.)
#define EPWM_TIMER_UP   1
#define EPWM_TIMER_DOWN 0
#define PWM1_TIMER_MIN     10
#define PWM1_TIMER_MAX     1900


//TIMER
__interrupt void cpu_timer0_isr(void);
__interrupt void cpu_timer1_isr(void);
__interrupt void cpu_timer2_isr(void);


//eCAP
__interrupt void ecap1_isr(void);
void InitECapture(void);
void InitEPwmTimer(void);
void Fail(void);
#endif
