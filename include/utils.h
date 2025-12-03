#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

// ============================================================================
//   Pin Definitions
// ============================================================================
#define LOCK_BTN 15     // Lock button
#define DOWN_BTN 2      // Down button
#define UP_BTN 0        // Up button
#define BUZZER 13       // Buzzer pin
#define LED 3           // Status LED
#define RELAY 23        // Relay control pin
#define DIGIT1 25       // 7-segment display digit 1
#define DIGIT2 1        // 7-segment display digit 2

// 7-segment binary inputs
#define A 12
#define B 27
#define C 26
#define D 14

// ============================================================================
//   Timing Constants
// ============================================================================
#define MSG_DELAY 2000               // Message duration in milliseconds
#define DEBOUNCE 300                 // Button debounce delay (ms)
#define DISPLAY_PERIOD_7SEGMENT 1    // 7-segment refresh period in ms
#define BUZZER_FREQ 2000             // Buzzer frequency in Hz
#define LOCK_TIME 20                 // Display unlock duration (seconds)

// ============================================================================
//   Function Prototypes
// ============================================================================

/**
 * Read the state of buttons and update internal flags.
 */
void readBtns();

/**
 * Handle menu navigation and selection logic.
 */
void handleMenu();

/**
 * Check if a button is currently pressed.
 * @param btn Button pin number
 * @return true if pressed, false otherwise
 */
boolean isPressed(uint8_t btn);

/**
 * Update the system time from RTC or internal counter.
 */
void updateTime();

/**
 * Check alarms and trigger buzzer/relay if an alarm is due.
 */
void checkAndTriggerAlarm();

/**
 * ISR for toggling the 7-segment display digits (fast refresh).
 */
void IRAM_ATTR onSevenSegmentDisplayToggle();

/**
 * Initialize relay hardware (set pin mode, default state, etc.).
 */
void initRelay();

/**
 * Function to execute tasks every second (time-based updates).
 */
void everySecond();

#endif
