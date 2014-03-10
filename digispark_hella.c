// testing mai digispark

#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 16500000UL  // 16.5 MHz
#include <util/delay.h>


// the graphical corridinate grid: 20 pixels, 8bit gray scale
volatile uint8_t display[4][5] = {
    { 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0 },
//     { 64, 64, 64, 64, 64 },

//     { 36, 87, 138, 189, 240 },
//     { 24, 75, 126, 177, 228 },
//     { 12, 63, 114, 165, 216 },
//     { 0, 51, 102, 153, 204 },
};


// https://github.com/dmcinnes/charlieplex-marquee/blob/master/charlieplex.ino
// LEDon() lightPixel()

void charlieOff() {
    PORTB = 0; // all off
    DDRB = 0;  // all inputs
}

// hi & lo 0-4 (but not the same)
void charlieOn(uint8_t hi, uint8_t lo) {
    DDRB = (1<<hi)|(1<<lo);
    PORTB = (1<<hi);
}

#define DISPLAY_WIDTH 4
void lightPixel(uint8_t x, uint8_t y) {
    if (x >= 0 && x < DISPLAY_WIDTH) {
        if (y <= x) {
            x++;
        }
        charlieOn(y, x);
    }
}

// timer interrupt; refresh display
ISR(TIM0_COMPA_vect) {                                                                                                                                                  
    static uint8_t soft_pwm_count = 0;         
    soft_pwm_count = (soft_pwm_count + 1) & 0x0f;

    for (uint8_t x = 0; x < 4; x++) {
        for (uint8_t y = 0; y < 5; y++) {
            if (display[x][y] > soft_pwm_count) {
                lightPixel(x,y);
                charlieOff();
            }
        }
    }
}

uint8_t lsfr_next2() {
    static uint8_t lfsr = (uint8_t)0xbc;
    lfsr = (lfsr >> 1) ^ (-(uint8_t)(lfsr & 1) & 0b10111000);
    return lfsr;
}

uint8_t lsfr_next() {
    static uint8_t lfsr = (uint8_t)0xcb;
    lfsr = (lfsr >> 1) ^ (-(uint8_t)(lfsr & 1) & 0b10111000);
    return lfsr;
}

int main(void) {
    //====================================================
    // chip setup


    // set CTC mode - 11.7.2
    //    registers - 11.9.1
    TCCR0A = 1<<WGM01;
    // set the clock source and prescaling - 11.9.2
    // ((16.5Mhz) / 64) ~ 16Khz  // or 16.0Mhz, w/o usb synchro
    TCCR0B |= (1<<CS01);
    // set the timer comparison register
    // 256 timer cycles ~ 16ms
    OCR0A = 0xff;

    // enable timer comparison interrupt - 11.9.6
    TIMSK = 1<<OCIE0A;

    sei(); // Enable global interrupts 

    // 
    //====================================================
    
    while (1) {
        uint8_t pos = lsfr_next2();
        uint8_t x   = (pos & 0xf) % 4;
        uint8_t y   = ((pos & 0xf0)>>4) % 5;
        display[x][y] = lsfr_next() & 0x0c;
        _delay_ms(20);
    }
}
