#ifndef PERIPHERALS_H_
#define PERIPHERALS_H_

//ADC
void Adc1_Config(void);
__interrupt void adc1_isr(void);


//ePWM
void InitEPwm1Example(void);
void InitEPwm2Example(void);
void InitEPwm3Example(void);

__interrupt void epwm1_isr(void);
__interrupt void epwm2_isr(void);
__interrupt void epwm3_isr(void);

void EPwm1_Config(void);

typedef struct
{
    volatile struct EPWM_REGS *EPwmRegHandle;
    Uint16 EPwm_CMPA_Direction;
    Uint16 EPwm_CMPB_Direction;
    Uint16 EPwmTimerIntCount;
    Uint16 EPwmMaxCMPA;
    Uint16 EPwmMinCMPA;
    Uint16 EPwmMaxCMPB;
    Uint16 EPwmMinCMPB;
} EPWM_INFO;

//Update compare the order of the declaration of the typedef is important !
void update_compare(EPWM_INFO*);

// Defines that configure the period for each timer
//
#define EPWM1_TIMER_TBPRD  3000  // Period register
#define EPWM1_MAX_CMPA     2999 //holtido benne legyen 2 orajelciklusnyi
#define EPWM1_MIN_CMPA      1
#define EPWM1_MAX_CMPB     2999
#define EPWM1_MIN_CMPB      1

#define EPWM2_TIMER_TBPRD  2000  // Period register
#define EPWM2_MAX_CMPA     1950
#define EPWM2_MIN_CMPA       50
#define EPWM2_MAX_CMPB     1950
#define EPWM2_MIN_CMPB       50

#define EPWM3_TIMER_TBPRD  2000  // Period register
#define EPWM3_MAX_CMPA      950
#define EPWM3_MIN_CMPA       50
#define EPWM3_MAX_CMPB     1950
#define EPWM3_MIN_CMPB     1050

//Defines to keep track of which way the timer value is moving (by Mate 2018.09.16.)
#define EPWM_TIMER_UP   1
#define EPWM_TIMER_DOWN 0
#define PWM1_TIMER_MIN     10
#define PWM1_TIMER_MAX     2900


// Defines to keep track of which way the compare value is moving

#define EPWM_CMP_UP   1
#define EPWM_CMP_DOWN 0


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
