#ifndef DATA_H
#define DATA_H

#include "database.h"
#include <Arduino.h>

// ============================================================================
//   Alarm structure
//   Represents a single alarm entry with time, duration, and active days.
// ============================================================================
struct Alarm {
  byte hour;      // Hour of the alarm (0–23)
  byte minute;    // Minute of the alarm (0–59)
  byte duration;  // Duration of the alarm (in seconds)
  byte days;      // Bitmask representing active days and state (e.g., 0b0111110 for Sun-Mon–Tue-Wed-Thu-Fri-Sat-State)
};

// ============================================================================
//   Main EEPROMData Structure
//   This structure stores all user-defined settings and persistent eeprom.
//   It is designed to fit in EEPROM / Flash managed by "database.h".
// ============================================================================
struct EEPROMData {

  // List of alarms stored in memory
  Alarm alarms[MAX_ALARMS];

  // Password used for securing access
  byte password[PASSWORD_LEN];

  // Program description (UTF-8 or ASCII)
  byte description[MAX_DESCRIPTION_LEN];

  // Author name (UTF-8 or ASCII)
  byte author[MAX_AUTHOR_LEN];

  // Program type or operating mode
  byte programType;

  // Number of alarms currently stored
  byte alarmCount;

  // Effective length of the program description
  byte descriptionLength;

  // Effective length of the author name
  byte authorLength;

  // Program or device state (application-specific)
  byte state;
};

#endif
