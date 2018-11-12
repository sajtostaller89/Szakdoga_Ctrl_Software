/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: Field_Oriented_Motor_Control.h
 *
 * Code generated for Simulink model 'Field_Oriented_Motor_Control'.
 *
 * Model version                  : 1.49
 * Simulink Coder version         : 8.14 (R2018a) 06-Feb-2018
 * C/C++ source code generated on : Mon Nov 12 12:24:38 2018
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Texas Instruments->C2000
 * Code generation objectives:
 *    1. Safety precaution
 *    2. MISRA C:2012 guidelines
 *    3. Execution efficiency
 * Validation result: Not run
 */

#ifndef RTW_HEADER_Field_Oriented_Motor_Control_h_
#define RTW_HEADER_Field_Oriented_Motor_Control_h_
#include <math.h>
#include <stddef.h>
#include <string.h>
#ifndef Field_Oriented_Motor_Control_COMMON_INCLUDES_
# define Field_Oriented_Motor_Control_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* Field_Oriented_Motor_Control_COMMON_INCLUDES_ */

#include "Field_Oriented_Motor_Control_types.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
# define rtmGetErrorStatus(rtm)        ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
# define rtmSetErrorStatus(rtm, val)   ((rtm)->errorStatus = (val))
#endif

/* Block states (default storage) for system '<Root>' */
typedef struct {
  real_T UnitDelay1_DSTATE;            /* '<S6>/Unit Delay1' */
  real_T UnitDelay2_DSTATE;            /* '<S6>/Unit Delay2' */
  real_T UnitDelay1_DSTATE_f;          /* '<S7>/Unit Delay1' */
  real_T UnitDelay2_DSTATE_b;          /* '<S7>/Unit Delay2' */
} DW_Field_Oriented_Motor_Control_T;

/* Invariant block signals (default storage) */
typedef struct {
  const real_T Product2;               /* '<S6>/Product2' */
  const real_T Gain1;                  /* '<S8>/Gain1' */
  const real_T Square;                 /* '<S8>/Square' */
  const real_T Gain;                   /* '<S4>/Gain' */
  const real_T Divide;                 /* '<S4>/Divide' */
  const real_T Divide1;                /* '<S4>/Divide1' */
  const real_T Product2_j;             /* '<S7>/Product2' */
} ConstB_Field_Oriented_Motor_Control_T;

/* Real-time Model Data Structure */
struct tag_RTM_Field_Oriented_Motor_Control_T {
  const char_T * volatile errorStatus;
};

/* Block states (default storage) */
extern DW_Field_Oriented_Motor_Control_T Field_Oriented_Motor_Control_DW;

/* External data declarations for dependent source files */
extern const Input Field_Oriented_Motor_Control_rtZInput;/* Input ground */
extern const Output Field_Oriented_Motor_Control_rtZOutput;/* Output ground */
extern const ConstB_Field_Oriented_Motor_Control_T
  Field_Oriented_Motor_Control_ConstB; /* constant block i/o */

/*
 * Exported Global Signals
 *
 * Note: Exported global signals are block signals with an exported global
 * storage class designation.  Code generation will declare the memory for
 * these signals and export their symbols.
 *
 */
extern Input Inputs;                   /* '<Root>/Input' */
extern Output Outputs;                 /* '<Root>/Bus Creator2' */

/* Model entry point functions */
extern void Field_Oriented_Motor_Control_initialize(void);
extern void Field_Oriented_Motor_Control_step(void);

/* Real-time Model object */
extern RT_MODEL_Field_Oriented_Motor_Control_T *const
  Field_Oriented_Motor_Control_M;

/*-
 * These blocks were eliminated from the model due to optimizations:
 *
 * Block '<Root>/Constant3' : Unused code path elimination
 * Block '<S9>/Data Type Duplicate' : Unused code path elimination
 * Block '<S9>/Data Type Propagation' : Unused code path elimination
 * Block '<S10>/Data Type Duplicate' : Unused code path elimination
 * Block '<S10>/Data Type Propagation' : Unused code path elimination
 * Block '<S11>/Data Type Duplicate' : Unused code path elimination
 * Block '<S11>/Data Type Propagation' : Unused code path elimination
 * Block '<S12>/Data Type Duplicate' : Unused code path elimination
 * Block '<S12>/Data Type Propagation' : Unused code path elimination
 */

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'Field_Oriented_Motor_Control'
 * '<S1>'   : 'Field_Oriented_Motor_Control/Current Control '
 * '<S2>'   : 'Field_Oriented_Motor_Control/Forward Clarke-,Park-transforms'
 * '<S3>'   : 'Field_Oriented_Motor_Control/Reverse Clarke-,Park-transforms'
 * '<S4>'   : 'Field_Oriented_Motor_Control/iqref,idref calculation'
 * '<S5>'   : 'Field_Oriented_Motor_Control/Current Control /Feedforward pre-control'
 * '<S6>'   : 'Field_Oriented_Motor_Control/Current Control /PI controller id'
 * '<S7>'   : 'Field_Oriented_Motor_Control/Current Control /PI controller iq'
 * '<S8>'   : 'Field_Oriented_Motor_Control/Current Control /Subsystem'
 * '<S9>'   : 'Field_Oriented_Motor_Control/Current Control /PI controller id/Saturation Dynamic'
 * '<S10>'  : 'Field_Oriented_Motor_Control/Current Control /PI controller iq/Saturation Dynamic'
 * '<S11>'  : 'Field_Oriented_Motor_Control/Current Control /Subsystem/Saturation vd'
 * '<S12>'  : 'Field_Oriented_Motor_Control/Current Control /Subsystem/Saturation vq'
 * '<S13>'  : 'Field_Oriented_Motor_Control/Forward Clarke-,Park-transforms/Clarke tranformation'
 * '<S14>'  : 'Field_Oriented_Motor_Control/Forward Clarke-,Park-transforms/Park transformation'
 * '<S15>'  : 'Field_Oriented_Motor_Control/Reverse Clarke-,Park-transforms/Inverse Clarke transformation'
 * '<S16>'  : 'Field_Oriented_Motor_Control/Reverse Clarke-,Park-transforms/Inverse Park transformation'
 */
#endif                                 /* RTW_HEADER_Field_Oriented_Motor_Control_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
