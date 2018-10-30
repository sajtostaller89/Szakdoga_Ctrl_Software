#ifndef PERIPHERALS_H_
#define PERIPHERALS_H_

#define GLOBAL_Q                19 /* range from -4096 to 4095.999 998 093 */
#include "IQmathLib.h" /* Fixed point Q-numbers based math library */

//ADC
__interrupt void adc1_isr(void);
__interrupt void adc2_isr(void);
__interrupt void adc4_isr(void);

void Adc_Config(void);
void Adc1_Config(void);
void Adc2_Config(void);
void Adc4_Config(void);


//ePWM
__interrupt void epwm4_isr(void);
__interrupt void epwm5_isr(void);
__interrupt void epwm6_isr(void);

void EPwm1_Config(void);    // For the current meas
void EPwm2_Config(void);    // For the resolver meas
void EPwm3_Config(void);    // For the voltage meas
void EPwm4_Config(void);
void EPwm5_Config(void);
void EPwm6_Config(void);

// Defines that configure the period for each timer
#define EPWM1_TIMER_TBPRD  15000  // Period register because fpwm = 1 /2*15000*90 MHz = 3000 Hz
#define EPWM2_TIMER_TBPRD  900    // Period register for the 100 kHz sample time
#define EPWM3_TIMER_TBPRD  15000  // Period register

#define EPWM4_TIMER_TBPRD  15000  // Period register because fpwm = 1 /2*15000*90 MHz = 3000 Hz
#define EPWM5_TIMER_TBPRD  15000  // Period register
#define EPWM6_TIMER_TBPRD  15000  // Period register

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
