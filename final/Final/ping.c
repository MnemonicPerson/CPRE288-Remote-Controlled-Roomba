/**
 * Driver for ping sensor
 * @file ping.c
 * @author
 */

#include "ping.h"
#include "Timer.h"
#include "math.h"

volatile unsigned long START_TIME = 0;
volatile unsigned long END_TIME = 0;
volatile enum{LOW, HIGH, DONE} STATE = LOW; // State of ping echo pulse

extern short overflow = 0;

float ping (void) {
    ping_trigger();
    timer_waitMillis(250);
    float distance = ping_getDistance();
    return distance;
}

void ping_init (void){

  // YOUR CODE HERE
    SYSCTL_RCGCGPIO_R |=  0b00000010;
    while ((SYSCTL_PRGPIO_R & 0x02) == 0) { }

    GPIO_PORTB_AFSEL_R |= 0x8;
    GPIO_PORTB_DEN_R |= 0x8;

    SYSCTL_RCGCTIMER_R |= 0x08;
    while ((SYSCTL_RCGCTIMER_R & 0x08) == 0) {}

    TIMER3_CTL_R &= ~0x100;
    TIMER3_CFG_R |= 0x4;
    TIMER3_TBMR_R |= 0x07;           // enable capture mode
    TIMER3_TBPR_R |= 0xFF;
    TIMER3_TBILR_R = 0xFFFF;         // start value, might need to change

    TIMER3_CTL_R |= 0xC00;

    GPIO_PORTB_DIR_R |= 0x8;
    GPIO_PORTB_PCTL_R |= 0x00007000;

    TIMER3_ICR_R &= ~0x400;
    TIMER3_IMR_R |= 0x400;

    NVIC_EN1_R |= 0x10; // need to change
    TIMER3_CTL_R |= 0x100;

    IntRegister(INT_TIMER3B, TIMER3B_Handler);
    IntMasterEnable();


    // Configure and enable the timer
//    TIMER3_CTL_R &= ~0x627F;
//    TIMER3_CTL_R |= 0xD00;
}

void ping_trigger (void){
    // Disable timer and disable timer interrupt
    TIMER3_CTL_R &= ~0x100;
    TIMER3_IMR_R &= ~0x400;
    // Disable alternate function (disconnect timer from port pin)
    GPIO_PORTB_AFSEL_R &= ~0x8;
    GPIO_PORTB_DIR_R |= 0x8; // set to output

    // YOUR CODE HERE FOR PING TRIGGER/START PULSE
        GPIO_PORTB_DATA_R &= ~0x8;
        timer_waitMicros(10);
        GPIO_PORTB_DATA_R |= 0x8;
        timer_waitMicros(10);
        GPIO_PORTB_DATA_R &= ~0x8;

        GPIO_PORTB_DIR_R &= ~0x8; //Sets Pin3 back to input
    // Clear an interrupt that may have been erroneously triggered
    TIMER3_ICR_R |= 0x400;
    // Re-enable alternate function, timer interrupt, and timer
    GPIO_PORTB_AFSEL_R |= 0x8;
    TIMER3_CTL_R |= 0x100;
    TIMER3_IMR_R |= 0x400;

    STATE = LOW;
}

void TIMER3B_Handler(void){

  // YOUR CODE HERE


    if (TIMER3_MIS_R & 0x400) {


        if (STATE == LOW) {
            START_TIME = TIMER3_TBR_R;
            STATE = HIGH;
        }
        else if (STATE == HIGH) {
            END_TIME = TIMER3_TBR_R;
            STATE = DONE;
        }

        TIMER3_ICR_R |= 0x400; // Clears Trigger flag
    }

  // As needed, go back to review your interrupt handler code for the UART lab.
  // What are the first lines of code in the ISR? Regardless of the device, interrupt handling
  // includes checking the source of the interrupt and clearing the interrupt status bit.
  // Checking the source: test the MIS bit in the MIS register (is the ISR executing
  // because the input capture event happened and interrupts were enabled for that event?
  // Clearing the interrupt: set the ICR bit (so that same event doesn't trigger another interrupt)
  // The rest of the code in the ISR depends on actions needed when the event happens.

}
//
float ping_getDistance (void){

    long delta = START_TIME - END_TIME; // number of clock cycles

    if (delta < 0) {
        overflow = 1;
        delta = (overflow << 24) + (START_TIME - END_TIME);
    } else {overflow = 0;}

    float distance = delta  * 6.25*pow(10, -8) * (1.0 / 2.0) * 343 * 100; // speed of sound (in cm/ms) * time (ms)

    // reset timer vars
    START_TIME = 0;
    END_TIME = 0;

    return distance;
}
