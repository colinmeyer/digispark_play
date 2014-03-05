// testing mai digispark

#include <avr/io.h>

#define F_CPU 16500000UL  // 16.5 MHz
#include <util/delay.h>

// hi & lo 0-4 (but not the same)
void charlieOn(uint8_t hi, uint8_t lo) {
    PORTB = 0; // all off
    DDRB = 0;  // all inputs
    DDRB = (1<<hi)|(1<<lo);
    PORTB = (1<<hi);
}


int main(void) {
    // pin1 (debug LED) set to output
    DDRB = (1<<DDB1); 

    charlieOn(1,2);
//     while (1) {
//         for ( uint8_t x = 0; x < 5; x++ ) {
//             for ( uint8_t y = 0; y < 5; y++ ) {
//                 if ( x != y ) {
//                     charlieOn(x,y);
//                     _delay_ms(300);
//                 }
//             }
//         }
//     }
}
