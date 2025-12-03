#ifndef VARS_H
#define VARS_H

#include "BluetoothSerial.h"
#include "database.h"
#include "datatypes.h"
#include "display.h"
#include "utils.h"
#include <DS3231.h>
#include <LiquidCrystal.h>
#include <Preferences.h>

// ============================================================================
//   Global EEPROMData Variables
// ============================================================================

// Main eeprom structure holding alarms, password, description, author, etc.
extern EEPROMData eeprom;

// Temporary byte variable for general-purpose use
extern byte tempByte;

// LCD object for 16x2 display
extern LiquidCrystal lcd;

// Current index of the selected alarm
extern int alarmIndex;

// Current time and date (hour, minute, second, day, month, day of week)
extern byte hourNow, minuteNow, secondNow, dayNow, monthNow, dayOfWeekNow, temperatureNow;

// Previous time and date (for detecting changes)
extern byte prevHour, prevMinute, prevSecond, prevDay, prevMonth, prevDayOfWeek, prevTemperature;

// Current and previous year
extern unsigned int yearNow, prevYear;

// RTC object for DS3231 real-time clock
extern DS3231 myRTC;

// Century flag for DS3231 (used in date calculations)
extern bool century;

// 12-hour format flag
extern bool h12Flag;  // true if using 12-hour format, false if 24-hour

// AM/PM flag for 12-hour format
extern bool pmFlag;    // true = PM, false = AM

// Alarm duration (in seconds)
extern byte duration;

// Preferences object for persistent storage (EEPROM/Flash)
extern Preferences preferences;

// Bluetooth Serial object
extern BluetoothSerial SerialBT;

// Current menu displayed on the LCD
extern MenuLcd currentMenu;

// Lock timer (used for display)
extern byte lockTime;

#endif
