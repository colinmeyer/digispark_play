// testing mai digispark

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#define F_CPU 16500000UL  // 16.5 MHz
#include <util/delay.h>

#define DISPLAY_WIDTH 4
#define DISPLAY_HEIGHT 5


volatile uint16_t _clicks; // user level "clock ticks"
volatile uint8_t _timerflags = 0;

#define MAX_D 6
// the graphical corridinate grid: 20 pixels, 8bit gray scale
volatile uint8_t display[MAX_D][4][5] = {
    { // stripes
        { 7, 1, 7, 1, 7 },
        { 7, 1, 7, 1, 7 },
        { 7, 1, 7, 1, 7 },
        { 7, 1, 7, 1, 7 },
    },

    {
    // Eye
        { 7, 7, 1, 7, 7 },
        { 7, 1, 0, 1, 7 },
        { 7, 1, 0, 1, 7 },
        { 7, 7, 1, 7, 7 },
    },

    {
    // Off
        { 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0 },
    },

    {
    // Calix
        { 7, 0, 0, 0, 7 },
        { 0, 0, 0, 0, 0 },
        { 0, 7, 0, 7, 0 },
        { 7, 0, 0, 0, 7 },
    },

    {
    // stripes
        { 7, 7, 7, 7, 7 },
        { 0, 0, 0, 0, 0 },
        { 7, 7, 7, 7, 7 },
        { 0, 0, 0, 0, 0 },
    },

    {
    // O
        { 0, 7, 7, 7, 0 },
        { 7, 0, 0, 0, 7 },
        { 7, 0, 0, 0, 7 },
        { 0, 7, 7, 7, 0 },
    },
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
    soft_pwm_count = (soft_pwm_count + 1) & 0b00000111;

    static uint8_t c = 0;
    for (uint8_t x = 0; x < 4; x++) {
        for (uint8_t y = 0; y < 5; y++) {
            if (display[c][x][y] && display[c][x][y] >= soft_pwm_count) {
                lightPixel(x,y);
            }
        }
    }
    charlieOff();

    _clicks++;
    
    // make the integer rollover happen on a boundary that is divisible by lots
    // of shit
    if (_clicks == 60000)
        _clicks = 0;

    if ( _clicks % 3000 == 0 ) {
        c++;
        if ( c == MAX_D )
            c = 0;
    }
}

uint16_t clicks () {
    uint16_t clicks;
    ATOMIC_BLOCK(ATOMIC_FORCEON) {                                                                                                                                      
        clicks = _clicks;
    }
    return clicks;
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

    charlieOff();
    
//     for (uint8_t x = 0; x < DISPLAY_WIDTH; x++) {
//         for (uint8_t y = 0; y < DISPLAY_HEIGHT; y++) {
// 
//         }
//     }


    uint8_t c = 0;
    while (1) {
        ++c;

        _delay_ms(50);
    }
}
