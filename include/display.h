#ifndef DISPLAY_H
#define DISPLAY_H

#include "utils.h"
#include <Arduino.h>
#include <LiquidCrystal.h>

// ============================================================================
//   LCD Pin Definitions
// ============================================================================
#define RS 4
#define EN 16
#define D4 17
#define D5 5
#define D6 18
#define D7 19

// ============================================================================
//   Days of the week abbreviations
// ============================================================================
const char DaysOfWeek[][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

// ============================================================================
//   AlarmState strings for display
// ============================================================================
const char AlarmState[][4] = {"OFF", " ON"};

// ============================================================================
//   Custom LCD characters
// ============================================================================
enum LcdCustomChars {
    NEXT_CHAR,          // Next arrow
    PREV_CHAR,          // Previous arrow
    THERMOMETER_CHAR,   // Thermometer symbol
    LINE_CHAR,          // Vertical line
    ON_CHAR,            // ON symbol
    OFF_CHAR,           // OFF symbol
    DEGRE_CHAR = 223    // Degree symbol (ASCII code 223)
};

// ============================================================================
//   LCD menu identifiers
// ============================================================================
enum MenuLcd {
    HOME,   // Main/home screen
    ALARMS  // Alarm settings screen
};

// ============================================================================
//   BCD mapping for 7-segment display segments (A, B, C, D assumed defined in utils.h)
// ============================================================================
const uint32_t bcd[10] = {
    0,                  // 0
    1 << A,             // 1
    1 << B,             // 2
    (1 << A) | (1 << B),// 3
    1 << C,             // 4
    (1 << A) | (1 << C),// 5
    (1 << B) | (1 << C),// 6
    (1 << A) | (1 << B) | (1 << C), // 7
    1 << D,             // 8
    (1 << A) | (1 << D) // 9
};

// Mask for 7-segments binary input (A + B + C + D)
const uint32_t ABCD = (1 << A) | (1 << B) | (1 << C) | (1 << D);

// ============================================================================
//   Function prototypes for LCD display
// ============================================================================
void displayAlarm();   // Display alarm info on the LCD
void refreshHome();    // Refresh the main/home screen
void initHome();       // Initialize home screen layout

#endif
