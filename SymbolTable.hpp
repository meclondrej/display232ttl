#pragma once

#include <Arduino.h>

/**
 * EN: A table that maps ASCII characters from 0x20 to 0x7F to seven segment
 * symbols (see byte format below).
 * CZ: Tabulka mapující ASCII znaky od 0x20 do 0x7F na sedmi segmentové
 * symboly (viz formát bajtu níže).
 *
 * |-A-|
 * F   B
 * |-G-|   => 0bABCDEFGP
 * E   C
 * |-D-| P
 */
extern const uint8_t symbol_table[96];
