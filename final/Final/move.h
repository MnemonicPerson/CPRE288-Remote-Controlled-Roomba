/*
 * move.h
 *
 *  Created on: Apr 18, 2024
 *      Author: bsmith26
 */

#ifndef MOVE_H_
#define MOVE_H_

#include "open_interface.h"

int complete;
int turn_offset;
int manual;

void cal_turning(oi_t *sensor_data);

void move(oi_t *sensor_data, double distance_mm);
void turn(oi_t *sensor_data, double degrees);
double move_forward(oi_t *sensor_data, double distance_mm);
void move_backward(oi_t *sensor_data, double distance_mm);
void turn_left(oi_t *sensor_data, double degrees);
void turn_right(oi_t *sensor_data, double degrees);



#endif /* MOVE_H_ */
