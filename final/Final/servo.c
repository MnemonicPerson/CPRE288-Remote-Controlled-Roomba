/*
 * servo.c
 *
 *  Created on: Apr 3, 2024
 *      Author: bsmith26
 */




#include "servo.h"
#include "button.h"


unsigned int left_val = 283802;  // default is 288000
unsigned int right_val = 312206; // default is 304000

void servo_init(void) {
    SYSCTL_RCGCGPIO_R |= 0x2; // enable port B clock
    while ((SYSCTL_PRGPIO_R & 0x2) == 0) {}

    GPIO_PORTB_DEN_R |= 0x20;
    GPIO_PORTB_AFSEL_R |= 0x20;
    GPIO_PORTB_DIR_R |= 0x20;   // set bit 5 as output
    GPIO_PORTB_PCTL_R &= 0xFF0FFFFF; // clear port B PCTL
    GPIO_PORTB_PCTL_R |= 0x00700000;

    SYSCTL_RCGCTIMER_R |= 0x2;
    while ((SYSCTL_RCGCTIMER_R & 0x2) == 0) {}

    TIMER1_CTL_R &= ~0x100;     // disable timer 1
    TIMER1_CFG_R &= ~0xFFF;
    TIMER1_CFG_R |= 0x4;        // set as 16 bit timer
    TIMER1_TBMR_R &= ~0xFFF;    // clear register first
    TIMER1_TBMR_R &= ~0x10;     // set in count down mode
    TIMER1_TBMR_R |= 0xA;       // set in PWM mode
    TIMER1_TBPR_R |= 0xFF;      // get 8-bit prescale register

    // sets whole timer start value to 0x4E200
    TIMER1_TBPR_R = 0x4;
    TIMER1_TBILR_R = 0xE200;

    TIMER1_TBPMR_R = 0x4;
    TIMER1_TBMATCHR_R = ((0x4E200 - right_val) + (0x4E200 - left_val)) / 2;

    TIMER1_CTL_R |= 0x100;      // re-enable timer
}

void servo_move(uint16_t degrees) {
    if (degrees > 180) {
        return;
    }

    double left_ms = (0x4E200 - left_val) / 16e+3;
    double right_ms = (0x4E200 - right_val) / 16e+3;


    double time_ms = ((double)(left_ms - right_ms) / 180.0) * degrees + right_ms;
    int cycles = time_ms * 16e+3;

    uint16_t curr_degrees =  ((double)180.0 / (left_ms - right_ms)) * (((0xE200 - TIMER1_TBMATCHR_R) / 16e+3) - right_ms) ;
    uint16_t delta_deg = abs(curr_degrees - degrees);



    TIMER1_TBPMR_R = 0x4; // we will never have to change this because of the angle bounds
    TIMER1_TBMATCHR_R = 0xE200 - cycles;

//    short delta_deg = 5;
    if (delta_deg < 10) {timer_waitMillis(40);}
    else {
    timer_waitMillis(10 * delta_deg);
    }
}

void servo_cal() {
    unsigned int left_val;
    unsigned int right_val;

    TIMER1_TBPMR_R = 0x4;
    TIMER1_TBMATCHR_R = 0x6500;
    int button_end = button_getButton();
    lcd_printf("B1 turns left\nB2 turns right\nB4 to stop\nFind the LEFT val");

    // get left val
    servo_move(180);
    while (button_end != 4) {
        if (button_end == 1) {
            TIMER1_TBMATCHR_R -= 200;
        }
        else if (button_end == 2) {
            TIMER1_TBMATCHR_R += 200;
        }

        button_end  = button_getButton();
    }
    left_val = 0x40000 + TIMER1_TBMATCHR_R;
//    lcd_printf("%d\n", 0x40000 + TIMER1_TBMATCHR_R);

//    lcd_printf("Press button 1 to move to 180 Degrees\nPress button 2 to move to 0 Degrees\n");
    button_end = button_getButton();
    lcd_printf("B1 turns left\nB2 turns right\nB4 to stop\nFind the RIGHT val");
    servo_move(0);
        while (button_end != 4) {
            if (button_end == 1) {
                TIMER1_TBMATCHR_R -= 200;
            }
            else if (button_end == 2) {
                TIMER1_TBMATCHR_R += 200;
            }

            button_end  = button_getButton();
        }
    right_val = 0x40000 + TIMER1_TBMATCHR_R;

    lcd_printf("Calibration Complete\nLeft: %d\nRight: %d", left_val, right_val);

    return;
}
