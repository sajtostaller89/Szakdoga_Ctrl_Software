/*
 * general.h
 *
 *  Created on: 2018. okt. 16.
 *      Author: Ákos
 */

#ifndef GENERAL_H_
#define GENERAL_H_

//Update compare the order of the declaration of the typedef is important !
void update_compare();
double duty_calc(void);

void ConfigEnableInterrupts(void);  //should be somewhere else because it is no a pheripheral
void ConfigPieVectTable(void);      //same

#define phase_voltage = 60.0             //Egyelõre legyen global TODO: make it not global
#define dclink_voltage = 60.0           // TODO:declare elsewhere (in general.h?)



#endif /* GENERAL_H_ */
