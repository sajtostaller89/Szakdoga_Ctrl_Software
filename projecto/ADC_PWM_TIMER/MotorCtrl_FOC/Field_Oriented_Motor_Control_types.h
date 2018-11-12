/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: Field_Oriented_Motor_Control_types.h
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

#ifndef RTW_HEADER_Field_Oriented_Motor_Control_types_h_
#define RTW_HEADER_Field_Oriented_Motor_Control_types_h_
#include "rtwtypes.h"
#ifndef DEFINED_TYPEDEF_FOR_Input_
#define DEFINED_TYPEDEF_FOR_Input_

typedef struct {
  real32_T ia;
  real32_T ib;
  real32_T ic;
  real32_T angle;
  real32_T speed;
} Input;

#endif

#ifndef DEFINED_TYPEDEF_FOR_Output_
#define DEFINED_TYPEDEF_FOR_Output_

typedef struct {
  real32_T va;
  real32_T vb;
  real32_T vc;
} Output;

#endif

/* Forward declaration for rtModel */
typedef struct tag_RTM_Field_Oriented_Motor_Control_T
  RT_MODEL_Field_Oriented_Motor_Control_T;

#endif                                 /* RTW_HEADER_Field_Oriented_Motor_Control_types_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
