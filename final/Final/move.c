/*
 * move.c
 *
 *  Created on: Apr 18, 2024
 *      Author: bsmith26
 */


#include "move.h"
#include "button.h"
#include "lcd.h"
#include "math.h"
#include "uart-interrupt.h"

int complete;
int manual;

int turn_offset;

int normal_cliff = 2850;

void cal_turning(oi_t *sensor_data) {
    // reset turn_offset
    turn_offset = 0;

    lcd_printf("Set CyBOT on floor\nPress B4 when ready");
    while (button_getButton() != 4) {}

    lcd_printf("Use B1 & B2 to turn\nPress B4 when done");
    turn(sensor_data, 360);

    int offset = 0;
    int pressed = 0;
    while (1) {
        int button = button_getButton();

        if (button == 0) {
            pressed = 0;
        }

        switch (button) {
            case 4:
                if (pressed) {
                    continue;
                }
                pressed = 1;
                lcd_printf("Set turn_offset to: %d", offset);
                return;

            /*
             * I don't think the cybot can accurately turn 1 degree, so this is getting thrown off
             */
            case 2:
                if (pressed) {
                    continue;
                }
                pressed = 1;
                turn(sensor_data, 1);
                offset -= 1;
                break;
            case 1:
                if (pressed) {
                    continue;
                }
                pressed = 1;
                turn(sensor_data, -1);
                offset += 1;
                break;
            default:
                continue;
        }
    }
}

void move(oi_t *sensor_data, double distance_mm) {
    char str[20];
    if (distance_mm >= 0) {

        double distance_traveled = move_forward(sensor_data, distance_mm);

//        move_backward(sensor_data, 100);
//        return;
        complete = 1; //global variable to determine if another scan is needed, didn't work - nathan//

        double distance_left = distance_mm - distance_traveled;


        sprintf(str, "Moved forward %.1lf cm\r\n", distance_traveled / 10);
        uart_sendStr(str);
        if (distance_left > 0) {

            //*Sensor Conditions*//
            if (sensor_data -> bumpLeft) {
                uart_sendStr("Left Bump Detected\r\n");
            }
           if (sensor_data -> bumpRight) {
               uart_sendStr("Right Bump Detected\r\n");
           }
//            if (sensor_data -> cliffLeft) {
//                uart_sendStr("Left Cliff Detected\r\n");
//            }
//            if (sensor_data -> cliffFrontLeft) {
//                uart_sendStr("Front Left Cliff Detected\r\n");
//            }
            if (sensor_data -> cliffFrontRightSignal > 2750) {
                uart_sendStr("Front Right Cliff Boundary Detected\r\n");
            }
            if (sensor_data -> cliffFrontRightSignal < 100) {
                uart_sendStr("Front Right Cliff Hole Detected\r\n");
            }
            if (sensor_data -> cliffRightSignal > normal_cliff) {
                uart_sendStr("Right Cliff Boundary Detected\r\n");
            }
            if (sensor_data -> cliffRightSignal < 100) {
                uart_sendStr("Right Cliff Hole Detected\r\n");
            }
            if (sensor_data -> cliffFrontLeftSignal > normal_cliff) {
                uart_sendStr("Front Left Cliff Boundary Detected\r\n");
            }
            if (sensor_data -> cliffFrontLeftSignal < 100) {
                uart_sendStr("Front Left Cliff Hole Detected\r\n");
            }
            if (sensor_data -> cliffLeftSignal > 2600) {
                uart_sendStr("Left Cliff Boundary Detected\r\n");
            }
            if (sensor_data -> cliffRightSignal < 100) {
                uart_sendStr("Right Cliff Hole Detected\r\n");
            }
        }

//        uart_sendStr("DONE\r\n");
    }
    else {
        sprintf(str, "Moved backwards %.1lf cm\r\n", -distance_mm / 10);
        uart_sendStr(str);
        move_backward(sensor_data, -distance_mm);
    }
}

void turn(oi_t *sensor_data, double degrees) {
    double angle_to_turn = (360.0 - 5) * degrees / 360.0; // Replace 5 with turn offset //

    char str[20];
    if (angle_to_turn > 0) {
        turn_right(sensor_data, angle_to_turn);
        sprintf(str, "Turned %.1lf deg right\r\n", degrees);
    }
    else {
        turn_left(sensor_data, -angle_to_turn);
        sprintf(str, "Turned %.1lf deg left\r\n", -degrees);
    }
    uart_sendStr(str);
}

double move_forward(oi_t *sensor_data, double distance_mm) {
    oi_setWheels(100, 100);
    servo_move(90);
    double sum = 0;

   while (!(sensor_data -> bumpLeft)
       && !(sensor_data -> bumpRight)
       && !(sensor_data -> cliffLeft)
       && !(sensor_data -> cliffFrontLeft)
       && !(sensor_data -> cliffRight)
       && !(sensor_data -> cliffFrontRight)
       && (sensor_data -> cliffLeftSignal  > 100 && sensor_data -> cliffLeftSignal < normal_cliff)
       && (sensor_data -> cliffRightSignal  > 100 && sensor_data -> cliffRightSignal < normal_cliff)
       && (sensor_data -> cliffFrontLeftSignal  > 100 && sensor_data -> cliffFrontLeftSignal < 2600)
       && (sensor_data -> cliffFrontRightSignal  > 100 && sensor_data -> cliffFrontRightSignal < 2600)
       && sum < distance_mm) {
       oi_update(sensor_data);

      float ping_dist;
      ping_dist = ping();
      if (ping_dist < 15.0) {
          uart_sendStr("Close Object Detected. Scan Again\r\n");
          break;
      }
       sum += sensor_data -> distance;
   }


   oi_setWheels(0,0);
   return sum;

}

void move_backward(oi_t *sensor_data, double distance_mm) {
    double sum = 0;
    oi_setWheels(-100, -100);

    while (sum > -distance_mm) {
        oi_update(sensor_data);
        sum += sensor_data -> distance;
    }

    oi_setWheels(0, 0);
}

void turn_left(oi_t *sensor_data, double degrees) {
    double sum = 0;
    oi_setWheels(100, -100);

    while (sum < degrees) {
        oi_update(sensor_data);
        sum += sensor_data -> angle;
    }

    oi_setWheels(0, 0);
}

void turn_right(oi_t *sensor_data, double degrees){
    double sum = 0;
    oi_setWheels(-100, 100);

    while (sum > -degrees) {
        oi_update(sensor_data);
        sum += sensor_data -> angle;
    }

    oi_setWheels(0, 0);
}

