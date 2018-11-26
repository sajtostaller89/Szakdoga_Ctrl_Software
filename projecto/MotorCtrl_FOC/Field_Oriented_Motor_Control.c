/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: Field_Oriented_Motor_Control.c
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

#include "Field_Oriented_Motor_Control.h"
#include "Field_Oriented_Motor_Control_private.h"

const Output Field_Oriented_Motor_Control_rtZOutput = {
  0.0F,                                /* va */
  0.0F,                                /* vb */
  0.0F                                 /* vc */
} ;                                    /* Output ground */

/* Exported block signals */
Input Inputs;                          /* '<Root>/Input' */
Output Outputs;                        /* '<Root>/Bus Creator2' */

/* Block states (default storage) */
DW_Field_Oriented_Motor_Control_T Field_Oriented_Motor_Control_DW;

/* Real-time model */
RT_MODEL_Field_Oriented_Motor_Control_T Field_Oriented_Motor_Control_M_;
RT_MODEL_Field_Oriented_Motor_Control_T *const Field_Oriented_Motor_Control_M =
  &Field_Oriented_Motor_Control_M_;
const Input Field_Oriented_Motor_Control_rtZInput = { 0.0F,/* ia */
  0.0F,                                /* ib */
  0.0F,                                /* ic */
  0.0F,                                /* angle */
  0.0F                                 /* speed */
};

/* Model step function */
void Field_Oriented_Motor_Control_step(void)
{
  real32_T rtb_Gain2;
  real32_T rtb_Add_p;
  real32_T rtb_Add_b;
  real_T rtb_Gain;
  real_T rtb_Sum2;
  real_T rtb_Sqrt;
  real_T rtb_Gain_j;
  real32_T rtb_Cos_tmp;
  real32_T rtb_Sin_tmp;

  /* Trigonometry: '<S16>/Cos' incorporates:
   *  Inport: '<Root>/Input'
   *  Trigonometry: '<S14>/Cos'
   */
  rtb_Cos_tmp = cosf(Inputs.angle);

  /* Gain: '<S13>/Gain2' incorporates:
   *  Gain: '<S13>/Gain'
   *  Gain: '<S13>/Gain1'
   *  Inport: '<Root>/Input'
   *  Sum: '<S13>/Add'
   */
  rtb_Gain2 = ((Inputs.ia - 0.5F * Inputs.ib) - 0.5F * Inputs.ic) * 0.666666687F;

  /* Trigonometry: '<S14>/Sin' incorporates:
   *  Inport: '<Root>/Input'
   *  Trigonometry: '<S16>/Sin'
   */
  rtb_Sin_tmp = sinf(Inputs.angle);

  /* Gain: '<S13>/Gain5' incorporates:
   *  Gain: '<S13>/Gain3'
   *  Gain: '<S13>/Gain4'
   *  Inport: '<Root>/Input'
   *  Sum: '<S13>/Add1'
   */
  rtb_Add_b = (0.866025388F * Inputs.ib - 0.866025388F * Inputs.ic) *
    0.666666687F;

  /* Sum: '<S14>/Add' incorporates:
   *  Product: '<S14>/Product'
   *  Product: '<S14>/Product1'
   *  Trigonometry: '<S14>/Sin'
   */
  rtb_Add_p = rtb_Cos_tmp * rtb_Gain2 + rtb_Sin_tmp * rtb_Add_b;

  /* Sum: '<S6>/Add2' incorporates:
   *  Constant: '<Root>/Constant4'
   *  Constant: '<S4>/Constant'
   *  Product: '<S6>/Product3'
   *  Product: '<S6>/Product4'
   *  Sum: '<S6>/Subtract'
   *  Sum: '<S6>/Subtract1'
   *  UnitDelay: '<S6>/Unit Delay1'
   *  UnitDelay: '<S6>/Unit Delay2'
   */
  Field_Oriented_Motor_Control_DW.UnitDelay2_DSTATE += ((0.0 - rtb_Add_p) -
    Field_Oriented_Motor_Control_DW.UnitDelay1_DSTATE) * 25.0 + (0.0 - rtb_Add_p)
    * Field_Oriented_Motor_Control_ConstB.Product2;

  /* Gain: '<S6>/Gain' incorporates:
   *  UnitDelay: '<S6>/Unit Delay2'
   */
  rtb_Gain = 2.3 * Field_Oriented_Motor_Control_DW.UnitDelay2_DSTATE;

  /* Sum: '<S14>/Add1' incorporates:
   *  Product: '<S14>/Product2'
   *  Product: '<S14>/Product3'
   *  Trigonometry: '<S14>/Sin'
   */
  rtb_Add_b = rtb_Add_b * rtb_Cos_tmp - rtb_Gain2 * rtb_Sin_tmp;

  /* Switch: '<S9>/Switch2' incorporates:
   *  Constant: '<S6>/Constant'
   *  Constant: '<S6>/Constant1'
   *  RelationalOperator: '<S9>/LowerRelop1'
   *  RelationalOperator: '<S9>/UpperRelop'
   *  Switch: '<S9>/Switch'
   */
  if (rtb_Gain > 60.0) {
    rtb_Gain = 60.0;
  } else {
    if (rtb_Gain < -60.0) {
      /* Switch: '<S9>/Switch' incorporates:
       *  Constant: '<S6>/Constant1'
       */
      rtb_Gain = -60.0;
    }
  }

  /* End of Switch: '<S9>/Switch2' */

  /* Sum: '<S1>/Sum2' incorporates:
   *  Constant: '<Root>/Constant2'
   *  Inport: '<Root>/Input'
   *  Product: '<S5>/Product3'
   *  Product: '<S5>/Product4'
   */
  rtb_Sum2 = rtb_Gain - rtb_Add_b * 0.0069 * Inputs.speed;

  /* Switch: '<S11>/Switch2' incorporates:
   *  Constant: '<S8>/Constant1'
   *  RelationalOperator: '<S11>/LowerRelop1'
   *  RelationalOperator: '<S11>/UpperRelop'
   *  Switch: '<S11>/Switch'
   */
  if (rtb_Sum2 > 34.641016151377549) {
    rtb_Sum2 = 34.641016151377549;
  } else {
    if (rtb_Sum2 < Field_Oriented_Motor_Control_ConstB.Gain1) {
      /* Switch: '<S11>/Switch' */
      rtb_Sum2 = Field_Oriented_Motor_Control_ConstB.Gain1;
    }
  }

  /* End of Switch: '<S11>/Switch2' */

  /* Sqrt: '<S8>/Sqrt' incorporates:
   *  Math: '<S8>/Square1'
   *  Sum: '<S8>/Subtract'
   */
  rtb_Sqrt = sqrt(Field_Oriented_Motor_Control_ConstB.Square - rtb_Sum2 *
                  rtb_Sum2);

  /* Sum: '<S7>/Subtract' */
  rtb_Gain = Field_Oriented_Motor_Control_ConstB.Divide1 - rtb_Add_b;

  /* Sum: '<S7>/Add2' incorporates:
   *  Constant: '<Root>/Constant4'
   *  Product: '<S7>/Product3'
   *  Product: '<S7>/Product4'
   *  Sum: '<S7>/Subtract1'
   *  UnitDelay: '<S7>/Unit Delay1'
   *  UnitDelay: '<S7>/Unit Delay2'
   */
  Field_Oriented_Motor_Control_DW.UnitDelay2_DSTATE_b += (rtb_Gain -
    Field_Oriented_Motor_Control_DW.UnitDelay1_DSTATE_f) * 25.0 + rtb_Gain *
    Field_Oriented_Motor_Control_ConstB.Product2_j;

  /* Gain: '<S7>/Gain' incorporates:
   *  UnitDelay: '<S7>/Unit Delay2'
   */
  rtb_Gain_j = 2.3 * Field_Oriented_Motor_Control_DW.UnitDelay2_DSTATE_b;

  /* Switch: '<S10>/Switch2' incorporates:
   *  Constant: '<S7>/Constant'
   *  Constant: '<S7>/Constant1'
   *  RelationalOperator: '<S10>/LowerRelop1'
   *  RelationalOperator: '<S10>/UpperRelop'
   *  Switch: '<S10>/Switch'
   */
  if (rtb_Gain_j > 60.0) {
    rtb_Gain_j = 60.0;
  } else {
    if (rtb_Gain_j < -60.0) {
      /* Switch: '<S10>/Switch' incorporates:
       *  Constant: '<S7>/Constant1'
       */
      rtb_Gain_j = -60.0;
    }
  }

  /* End of Switch: '<S10>/Switch2' */

  /* Sum: '<S1>/Sum3' incorporates:
   *  Constant: '<Root>/Constant2'
   *  Constant: '<Root>/Constant9'
   *  Inport: '<Root>/Input'
   *  Product: '<S5>/Product1'
   *  Product: '<S5>/Product2'
   *  Sum: '<S5>/Sum1'
   */
  rtb_Gain_j += (rtb_Add_p * 0.0069 + 0.129) * Inputs.speed;

  /* Switch: '<S12>/Switch2' incorporates:
   *  Gain: '<S8>/Gain'
   *  RelationalOperator: '<S12>/LowerRelop1'
   *  RelationalOperator: '<S12>/UpperRelop'
   *  Switch: '<S12>/Switch'
   */
  if (rtb_Gain_j > rtb_Sqrt) {
    rtb_Gain_j = rtb_Sqrt;
  } else {
    if (rtb_Gain_j < -rtb_Sqrt) {
      /* Switch: '<S12>/Switch' incorporates:
       *  Gain: '<S8>/Gain'
       */
      rtb_Gain_j = -rtb_Sqrt;
    }
  }

  /* End of Switch: '<S12>/Switch2' */

  /* Sum: '<S16>/Add' incorporates:
   *  Product: '<S16>/Product'
   *  Product: '<S16>/Product1'
   *  Trigonometry: '<S16>/Cos'
   */
  rtb_Add_b = (real32_T)(rtb_Cos_tmp * rtb_Sum2 - rtb_Sin_tmp * rtb_Gain_j);

  /* Sum: '<S16>/Add1' incorporates:
   *  Product: '<S16>/Product2'
   *  Product: '<S16>/Product3'
   *  Trigonometry: '<S16>/Cos'
   */
  rtb_Sum2 = rtb_Gain_j * rtb_Cos_tmp + rtb_Sum2 * rtb_Sin_tmp;

  /* Sum: '<S15>/Add' incorporates:
   *  Gain: '<S15>/Gain'
   *  Gain: '<S15>/Gain3'
   */
  Outputs.vb = (real32_T)(-0.5F * rtb_Add_b + 0.8660254037844386 * rtb_Sum2);

  /* Sum: '<S15>/Add1' incorporates:
   *  Gain: '<S15>/Gain1'
   *  Gain: '<S15>/Gain4'
   */
  Outputs.vc = (real32_T)(-0.8660254037844386 * rtb_Sum2 + -0.5F * rtb_Add_b);

  /* BusCreator: '<Root>/Bus Creator2' */
  Outputs.va = rtb_Add_b;

  /* Update for UnitDelay: '<S6>/Unit Delay1' incorporates:
   *  Constant: '<S4>/Constant'
   *  Sum: '<S6>/Subtract'
   */
  Field_Oriented_Motor_Control_DW.UnitDelay1_DSTATE = 0.0 - rtb_Add_p;

  /* Update for UnitDelay: '<S7>/Unit Delay1' */
  Field_Oriented_Motor_Control_DW.UnitDelay1_DSTATE_f = rtb_Gain;
}

/* Model initialize function */
void Field_Oriented_Motor_Control_initialize(void)
{
  /* Registration code */

  /* initialize error status */
  rtmSetErrorStatus(Field_Oriented_Motor_Control_M, (NULL));

  /* block I/O */

  /* exported global signals */
  Outputs = Field_Oriented_Motor_Control_rtZOutput;

  /* states (dwork) */
  (void) memset((void *)&Field_Oriented_Motor_Control_DW, 0,
                sizeof(DW_Field_Oriented_Motor_Control_T));

  /* external inputs */
  Inputs = Field_Oriented_Motor_Control_rtZInput;
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
