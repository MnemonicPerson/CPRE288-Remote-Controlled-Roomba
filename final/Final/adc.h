/*
 * adc.h
 *
 *  Created on: Mar 20, 2024
 *      Author: bsmith26
 */

#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>
#include <inc/tm4c123gh6pm.h>



void adc_init(void);

uint16_t adc_read(void);

float adc_get_distance();

void adc_calibration();

typedef struct{
    float distance_cm;
    float ir_val;
} ir_dist_lookup;


#endif /* ADC_H_ */
