// testing mai digispark

#include <avr/io.h>

#define F_CPU 16500000UL  // 16.5 MHz
#include <util/delay.h>

int main(void) {
    // pin1 (debug LED) set to output
    DDRB = (1<<DDB1); 

    while(1) {
        PORTB = (1<<PB1); // LED on
        _delay_ms(500);
        PORTB = 0;        // all off
        _delay_ms(200);
    }
}
