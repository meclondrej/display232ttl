#pragma once

/**
 * EN: The number of symbols on the display.
 * CZ: Počet symbolů na displeji.
 */
#define DISPLAY_SYMBOL_COUNT 8

/**
 * EN: The baudrate of the serial interface.
 * CZ: Přenosová rychlost sériového rozhraní.
 */
#define SERIAL_BAUDRATE 9600

/**
 * EN: The delay between automatic refreshes of the display.
 * CZ: Prodleva mezi automatickými obnoveními displeje.
 */
#define AUTOREFRESH_DELAY 1000

/**
 * EN: A flag that determines whether the SRCLR pin is available.
 * CZ: Vlajka která rozhoduje, jestli je pin SRCLR dostupný.
 */
#define SRCLR_AVAILABLE 0

/**
 * EN: Numbers of pins connected to the shift register.
 * CZ: Čísla pinů připojených k posuvnému registru.
 */
#define PIN_OE 3
#define PIN_RCLK 4
#define PIN_SRCLR 5
#define PIN_SRCLK 6
#define PIN_SER 7
