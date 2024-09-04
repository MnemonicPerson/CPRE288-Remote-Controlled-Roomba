/*
 * servo.h
 *
 *  Created on: Apr 3, 2024
 *      Author: bsmith26
 */

#ifndef SERVO_H_
#define SERVO_H_

#include <stdint.h>
#include <inc/tm4c123gh6pm.h>
#include "timer.h"

void servo_init(void);

void servo_move(uint16_t degrees);

void servo_cal();

#endif /* SERVO_H_ */
