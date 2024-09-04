/*
 * scan.c
 *
 *  Created on: Mar 8, 2024
 *      Author: nkallal1
 */


#include "Timer.h"
#include "lcd.h"
#include "uart-interrupt.h"
#include "scan.h"
#include "open_interface.h"
#include "movement.h"
#include "adc.h"
#include "ping.h"
#include "servo.h"
#include "button.h"





void scan() {
    lcd_printf("Scanning...");

    short scan_inc = 1;
    short start_angle = 0;
    short end_angle = 180;


    char buffer[42];
    sprintf(buffer, "%20s%20s\r\n", "angle (degrees)", "IR Distance");
    uart_sendStr(buffer);



    float distance;
    short angle;

    short i;
    short j = 0;
    for (i = start_angle; i <= end_angle; i+=scan_inc) {


           servo_move(i);

           angle = i;

           distance = adc_get_distance();


           sprintf(buffer, "%20d%20.1f\r\n", angle, distance);
           uart_sendStr(buffer);
           j++;
       }


//    uart_sendStr("DONE\r\n");

    lcd_printf("Done with Scan");
}
