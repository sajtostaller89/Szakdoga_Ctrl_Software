/*
 * general.h
 *
 *  Created on: 2018. okt. 16.
 *      Author: Ákos
 */

#ifndef GENERAL_H_
#define GENERAL_H_

#include <MotorCtrl_FOC/Field_Oriented_Motor_Control.h>
#include <MotorCtrl_FOC/Field_Oriented_Motor_Control_private.h>
#include <MotorCtrl_FOC/Field_Oriented_Motor_Control_types.h>

//Update compare the order of the declaration of the typedef is important !
extern void update_compare(Output FOC_voltages);

void ConfigEnableInterrupts(void);
void ConfigPieVectTable(void);

extern float dclink_voltage;

extern Uint16 deadtime;      // In 1/90 ns = 0.0111111111 ns, 450 => 5ns


#endif /* GENERAL_H_ */
