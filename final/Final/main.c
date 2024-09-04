

/**
 * main.c
 */

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


int main(void) {

    // Ice Cream Truck Music
    // G = 79 // D = 74         B,  A,  G,  G,  G,  B,  C,  D,  E,  D,  B,  D,  G,  A,  B,  B,  B,  B,  B,  G,  A,  B,  A,  A,  A,  A   B,  A,  G   G,  G,  G,  G,  D,  B,  C,  D,  E,  D,  B,  D,  D,  G,  A,  B,  A,  G
    unsigned char notes[50] = {83, 81, 79, 79, 79, 71, 72, 74, 76, 74, 71, 74, 79, 81, 83, 83, 83, 83, 83, 79, 81, 83, 81, 81, 81, 81, 83, 81, 79, 79, 79, 79, 79, 74, 71, 72, 74, 76, 74, 71, 74, 74, 79, 81, 83, 81, 79};
    unsigned char duration[50] = {12, 12, 28, 28, 28, 12, 12, 12, 12, 12, 12, 16, 14, 14, 12, 12, 12, 12, 15, 13, 13, 13, 12, 12, 12, 15, 13, 13, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 12, 13, 13, 16, 64, 64, 64};

   timer_init();
   lcd_init();
   uart_interrupt_init();
   ping_init();
   adc_init();
   servo_init();
   button_init();


    oi_t *sensor = oi_alloc();
    oi_init(sensor);


    lcd_printf("READY");

    char input;
    double dist = 150;
    short angle = 10;

    double left, right;

//    while(1) {
//        lcd_printf("%d, %d, %d, %d", sensor->cliffLeftSignal, sensor->cliffFrontLeftSignal, sensor->cliffRightSignal, sensor->cliffFrontRightSignal);
//        oi_update(sensor);
//    }

    while(1) {
        input = get_command_byte();

        if (input == 0) {
            continue;
        }

        switch (input) {
            case 'w' :
                lcd_printf("Move forward %.1f cm", dist/10);
                servo_move(90);
                move(sensor, 150);
                break;
            case 'f':
                lcd_printf("move forward 0.5 m");
                servo_move(90);
                move(sensor, 500);
                break;
            case 'a' :
                lcd_printf("Turn left %d Degrees", angle);
                turn(sensor, -10);
                break;
            case '<':
                lcd_printf("Turn left 90 Degrees");
                turn(sensor, -90);
                break;
            case 's' :
                lcd_printf("Move backwards 5 cm");
                move(sensor, -50);
                break;
            case 'd' :
               lcd_printf("Turn right %d Degrees", angle);
               turn(sensor, 10);
               break;
            case '>':
                lcd_printf("Turn Right 90 Degrees");
                turn(sensor, 90);
                break;
            case 'm' :
                scan();
                break;
            case 'c' :
                servo_cal();
                break;
            case 't' :
                lcd_printf("Calibrating CyBot Motors");
                left = oi_getMotorCalibrationLeft();
                right = oi_getMotorCalibrationRight();
                oi_setMotorCalibration(left, right);
                lcd_printf("Done with Calibration");
                break;
            case 'i' :
                adc_calibration();
                break;
            case 'p' :
                lcd_printf("SELL ICE CREAM");
                oi_loadSong(0, 49, notes, duration);
                oi_play_song(0);
                timer_waitMillis(10000);
                break;
            case '-':
                lcd_printf("%d percent" , 100*(sensor->batteryCharge) / (sensor->batteryCapacity));
                break;
            default:
                continue;
//                lcd_printf("Unrecognized Command");
//                uart_sendStr("Unrecognized Command\r\n");
        }
        uart_sendStr("DONE\r\n");
    }


//





	return 0;
}


