/*
 * adc.c
 *
 *  Created on: Mar 20, 2024
 *      Author: bsmith26
 */


#include "adc.h"
#include "math.h"

#include "ping.h"
#include "Timer.h"
#include "math.h"
#include "adc.h"
#include "lcd.h"
#include "button.h"
#include "scan.h"
#include "move.h"
#include "open_interface.h"
#include "servo.h"


ir_dist_lookup lookup_table[] = {
(ir_dist_lookup) {.distance_cm = 5.8, .ir_val=3849},
(ir_dist_lookup) {.distance_cm = 6.1, .ir_val=3848},
(ir_dist_lookup) {.distance_cm = 6.4, .ir_val=3822},
(ir_dist_lookup) {.distance_cm = 7.1, .ir_val=3642},
(ir_dist_lookup) {.distance_cm = 8.3, .ir_val=3378},
(ir_dist_lookup) {.distance_cm = 8.6, .ir_val=3119},
(ir_dist_lookup) {.distance_cm = 9.9, .ir_val=2938},
(ir_dist_lookup) {.distance_cm = 10.5, .ir_val=3013},
(ir_dist_lookup) {.distance_cm = 11.2, .ir_val=2600},
(ir_dist_lookup) {.distance_cm = 11.9, .ir_val=2453},
(ir_dist_lookup) {.distance_cm = 12.6, .ir_val=2321},
(ir_dist_lookup) {.distance_cm = 13.3, .ir_val=2226},
(ir_dist_lookup) {.distance_cm = 14.0, .ir_val=2134},
(ir_dist_lookup) {.distance_cm = 14.7, .ir_val=2026},
(ir_dist_lookup) {.distance_cm = 15.5, .ir_val=1948},
(ir_dist_lookup) {.distance_cm = 16.2, .ir_val=1996},
(ir_dist_lookup) {.distance_cm = 16.9, .ir_val=1790},
(ir_dist_lookup) {.distance_cm = 18.0, .ir_val=1726},
(ir_dist_lookup) {.distance_cm = 18.8, .ir_val=1961},
(ir_dist_lookup) {.distance_cm = 19.5, .ir_val=1626},
(ir_dist_lookup) {.distance_cm = 20.2, .ir_val=1581},
(ir_dist_lookup) {.distance_cm = 21.0, .ir_val=1582},
(ir_dist_lookup) {.distance_cm = 21.7, .ir_val=1483},
(ir_dist_lookup) {.distance_cm = 22.8, .ir_val=1440},
(ir_dist_lookup) {.distance_cm = 23.1, .ir_val=1424},
(ir_dist_lookup) {.distance_cm = 24.3, .ir_val=1368},
(ir_dist_lookup) {.distance_cm = 25.0, .ir_val=1614},
(ir_dist_lookup) {.distance_cm = 25.3, .ir_val=1298},
(ir_dist_lookup) {.distance_cm = 26.5, .ir_val=1247},
(ir_dist_lookup) {.distance_cm = 27.2, .ir_val=1220},
(ir_dist_lookup) {.distance_cm = 27.9, .ir_val=1202},
(ir_dist_lookup) {.distance_cm = 28.2, .ir_val=1168},
(ir_dist_lookup) {.distance_cm = 28.5, .ir_val=1154},
(ir_dist_lookup) {.distance_cm = 29.2, .ir_val=1247},
(ir_dist_lookup) {.distance_cm = 29.9, .ir_val=1104},
(ir_dist_lookup) {.distance_cm = 30.8, .ir_val=1084},
(ir_dist_lookup) {.distance_cm = 31.4, .ir_val=1060},
(ir_dist_lookup) {.distance_cm = 32.1, .ir_val=1032},
(ir_dist_lookup) {.distance_cm = 32.8, .ir_val=1008},
(ir_dist_lookup) {.distance_cm = 33.5, .ir_val=998},
(ir_dist_lookup) {.distance_cm = 34.2, .ir_val=965},
(ir_dist_lookup) {.distance_cm = 35.2, .ir_val=961},
(ir_dist_lookup) {.distance_cm = 35.8, .ir_val=933},
(ir_dist_lookup) {.distance_cm = 36.5, .ir_val=926},
(ir_dist_lookup) {.distance_cm = 37.2, .ir_val=917},
(ir_dist_lookup) {.distance_cm = 37.5, .ir_val=952},
(ir_dist_lookup) {.distance_cm = 38.6, .ir_val=891},
(ir_dist_lookup) {.distance_cm = 39.3, .ir_val=937},
(ir_dist_lookup) {.distance_cm = 40.5, .ir_val=853},
(ir_dist_lookup) {.distance_cm = 41.2, .ir_val=854},
(ir_dist_lookup) {.distance_cm = 41.9, .ir_val=998},
(ir_dist_lookup) {.distance_cm = 42.6, .ir_val=828},
(ir_dist_lookup) {.distance_cm = 42.9, .ir_val=807},
(ir_dist_lookup) {.distance_cm = 44.1, .ir_val=803},
(ir_dist_lookup) {.distance_cm = 44.4, .ir_val=809},
(ir_dist_lookup) {.distance_cm = 45.6, .ir_val=779},
(ir_dist_lookup) {.distance_cm = 45.9, .ir_val=780},
(ir_dist_lookup) {.distance_cm = 47.0, .ir_val=756},
(ir_dist_lookup) {.distance_cm = 47.3, .ir_val=753},
(ir_dist_lookup) {.distance_cm = 48.0, .ir_val=762},
(ir_dist_lookup) {.distance_cm = 49.2, .ir_val=733},
(ir_dist_lookup) {.distance_cm = 49.9, .ir_val=732},
(ir_dist_lookup) {.distance_cm = 50.2, .ir_val=790},
};
short lookup_len = 63;


void adc_init(void) {
    SYSCTL_RCGCADC_R |= 0x0001;
    SYSCTL_RCGCGPIO_R |= 0x02;
    while ((SYSCTL_PRGPIO_R & 0x02) != 0x02) {};
    GPIO_PORTB_DIR_R &= ~0x10;
    GPIO_PORTB_AFSEL_R |= 0x10;
    GPIO_PORTB_DEN_R &= ~0x10;
    GPIO_PORTB_AMSEL_R |= 0x10;

    ADC0_SAC_R = 0x4; // enable automatic averaging

    ADC0_PC_R &= ~0xF; // clear register
    ADC0_PC_R |= 0x1; // 125 ksps
    ADC0_SSPRI_R = 0x0123;
    ADC0_ACTSS_R &= ~0x0008;
    ADC0_EMUX_R &= ~0xF000;
    ADC0_SSMUX3_R &= ~0x000F;
    ADC0_SSMUX3_R |= 0x0A; // bit 3 enables temperature readin
    ADC0_SSCTL3_R = 0x0006;
    ADC0_IM_R &= ~0x0008;
    ADC0_ACTSS_R |= 0x0008;
}

uint16_t adc_read(void) {
    uint16_t result;
    ADC0_PSSI_R |= 0x0008;
    while ((ADC0_RIS_R & 0x08) == 0) {};
    result = ADC0_SSFIFO3_R & 0xFFF;
    ADC0_ISC_R |= 0x0008;
    return result;
}

float adc_get_distance() {
    uint16_t value = adc_read();
//    float distance = -2*pow(10, -8)*pow(value, 3) + 0.0001*pow(value, 2) - 0.2544*value + 215.82;
//    return distance;

    // find first value in lookup table that is less than given value
    int index = 0;
    int lookup_val = lookup_table[index].ir_val;
    if (value < 730) {
        return 50;
    }
    while (value < lookup_val) {
        lookup_val = lookup_table[++index].ir_val;


    }


    int dist_from_higher_val = lookup_table[index-1].ir_val - value;
    int dist_from_lower_val = value - lookup_table[index].ir_val;

    if (dist_from_higher_val > dist_from_lower_val) {
        return lookup_table[index].distance_cm;
    }
    else {
        return lookup_table[index-1].distance_cm;
    }
}

void adc_calibration() {
    lcd_printf("Place Robot Facing Wall \nPress Button when ready\n");
    short input = 0;
    servo_move(90);
    while (input == 0) {input = button_getButton();}
    lcd_printf("Calibrating ADC \nLookup Table");
    float distance = 0;
    short ir_val;
    char str[60];
//    sprintf(str, "%15s %10s\r\n", "Distance", "IR val");
    uart_sendStr(str);

    sprintf(str, "ir_dist_lookup lookup_table[] = {\r\n");
        uart_sendStr(str);

    oi_setWheels(-30, -30);
    short index = 0;

    while (distance <= 50) {
        distance = ping();
        ir_val = adc_read();
        index++;

        sprintf(str, "(ir_dist_lookup) {.distance_cm = %.1f, .ir_val=%d},\r\n", distance, ir_val);
        uart_sendStr(str);
    }
    sprintf(str, "};\r\n");
    uart_sendStr(str);

    sprintf(str, "short lookup_len = %d;\r\n", index);
    uart_sendStr(str);
    oi_setWheels(0, 0);
    lcd_printf("Copy into Lookup Table");

}
