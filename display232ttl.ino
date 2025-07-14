#include <Arduino.h>
#include <avr/pgmspace.h>
#include "HardwareSerial.h"
#include "Config.hpp"
#include "SymbolTable.hpp"

/**
 * EN: An array that contains the current state of each symbol on the display.
 * CZ: Pole obsahující stav každého symbolu na displeji.
 */
uint8_t display_buffer[DISPLAY_SYMBOL_COUNT];

/**
 * EN: Last automatic refresh time.
 * CZ: Čas posledního automatického obnovení.
 */
uint16_t last_autorefresh;

/**
 * EN: A flag that determines whether the latest data received from the user
 * was uploaded to the display.
 * CZ: Vlajka, která rozhoduje jestli nejnovější data od uživatele byla
 * nahrána na displej.
 */
uint8_t buffer_was_uploaded;

/**
 * EN: Sends a pulse on a given pin.
 * CZ: Vyšle pulz na daném pinu.
 */
void pulse_pin(uint8_t pin) {
    digitalWrite(pin, HIGH);
    digitalWrite(pin, LOW);
}

/**
 * EN: Uploads display_buffer to the display.
 * CZ: Nahraje display_buffer na displej.
 */
void upload_buffer() {
    for (size_t i = DISPLAY_SYMBOL_COUNT; i--;)
        for (uint8_t mask = 0x80; mask; mask >>= 1) {
            digitalWrite(PIN_SER, (display_buffer[i] & mask) ? HIGH : LOW);
            pulse_pin(PIN_SRCLK);
        }
    pulse_pin(PIN_RCLK);
}

/**
 * EN: Clears the display.
 * CZ: Vynuluje displej.
 */
void clear_display() {
    if (SRCLR_AVAILABLE)
        pulse_pin(PIN_SRCLR);
    else {
        digitalWrite(PIN_SER, LOW);
        for (size_t i = 8 * DISPLAY_SYMBOL_COUNT; i--;)
            pulse_pin(PIN_SRCLK);
    }
    pulse_pin(PIN_RCLK);
}

/**
 * EN: Clears display_buffer.
 * CZ: Vynuluje display_buffer.
 */
void clear_display_buffer() {
    memset(display_buffer, 0, DISPLAY_SYMBOL_COUNT);
}

/**
 * EN: Looks up the symbol table for a given character and returns its seven
 * segment symbol. The caller guarantees that the character is between 0x20
 * and 0x7F (inclusively).
 * CZ: Vyhledá v symbolové tabulce daný znak a vrátí jeho sedmi segmentový
 * symbol. Volající garantuje, že znak je mezi 0x20 a 0x7F (inkluzivně).
 */
uint8_t lookup_symbol(uint8_t character) {
    return pgm_read_byte(symbol_table + character - 0x20);
}

/**
 * EN: Shifts all symbols to the left by one position and sets the last
 * symbol to the given value.
 * CZ: Posune všechny symboly doleva o jednu pozici a nastaví poslední
 * symbol na danou hodnotu
 */
void push_back_symbol(uint8_t symbol) {
    if (buffer_was_uploaded)
        clear_display_buffer();
    buffer_was_uploaded = 0;
    for (size_t i = 0; i < DISPLAY_SYMBOL_COUNT - 1; i++)
        display_buffer[i] = display_buffer[i + 1];
    display_buffer[DISPLAY_SYMBOL_COUNT - 1] = symbol;
}

/**
 * EN: Sets the brightness level. The caller guarantees that the parameter is
 * between 0 and 7 (inclusive).
 * CZ: Nastaví úroveň jasu. Volajicí garantuje, že parametr je mezi 0 a 7
 * (inkluzivně).
 */
void set_brightness(uint8_t level) {
    analogWrite(PIN_OE, (7 - level) * 32);
}

/**
 * EN: Executes a special function based on a numeric identifier. The caller
 * guarantees that the parameter is between 0x00 and 0x19 (inclusive).
 * CZ: Spustí speciální funkci podle číselného identifikátoru. Volající
 * garantuje že paramter je mezi 0x00 a 0x19 (inkluzivně).
 */
void exec_special_function(uint8_t code) {
    if (code >= 0x10 && code <= 0x17) {
        set_brightness(code - 0x10);
        return;
    }
    switch (code) {
        case 0x00: // null
        case 0x0A: // line feed
        case 0x0D: // carriage return
            if (buffer_was_uploaded)
                return;
            upload_buffer();
            buffer_was_uploaded = 1;
            return;
        case 0x0C: // form feed
            clear_display();
            clear_display_buffer();
            buffer_was_uploaded = 1;
            return;
        case 0x01:
            display_buffer[DISPLAY_SYMBOL_COUNT - 1] |= 0x80;
            return;
    }
}

/**
 * EN: Initial setup.
 * CZ: Prvotní nastavení.
 */
void setup() {
    pinMode(PIN_OE, OUTPUT);
    pinMode(PIN_RCLK, OUTPUT);
    pinMode(PIN_SRCLR, OUTPUT);
    pinMode(PIN_SRCLK, OUTPUT);
    pinMode(PIN_SER, OUTPUT);
    clear_display();
    clear_display_buffer();
    Serial.begin(SERIAL_BAUDRATE);
    last_autorefresh = millis();
    buffer_was_uploaded = 0;
}

/**
 * EN: Main loop.
 * CZ: Hlavní smyčka.
 */
void loop() {
    uint8_t received_byte;
    while (!Serial.available()) {
        uint16_t now = millis();
        if (last_autorefresh > now ||
            now - last_autorefresh >= AUTOREFRESH_DELAY) {
            upload_buffer();
            last_autorefresh = now;
        }
    }
    received_byte = Serial.read();
    if (received_byte <= 0x19) {
        exec_special_function(received_byte);
        return;
    }
    uint8_t decimal_point_bit = 0;
    if (received_byte >= 0x80) {
        decimal_point_bit = 1;
        received_byte -= 0x80;
    }
    if (received_byte <= 0x19)
        return;
    uint8_t symbol = lookup_symbol(received_byte);
    push_back_symbol(symbol);
    if (decimal_point_bit)
        display_buffer[DISPLAY_SYMBOL_COUNT - 1] |= 0x80;
}
