// Comment to initialize EEPROM memory with default values
#define RELEASE

#include "BluetoothSerial.h"
#include "database.h"
#include "display.h"
#include "global_vars.h"
#include "server.h"
#include "utils.h"
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Preferences.h>

// ============================================================================
//   Custom Characters for LCD
// ============================================================================
byte thermometre[] = {B00100, B01010, B01010, B01010, B01110, B11111, B11111, B01110};
byte next[] = {B01000, B01100, B01110, B01111, B01110, B01100, B01000, B00000};
byte prev[] = {B00010, B00110, B01110, B11110, B01110, B00110, B00010, B00000};
byte line[] = {B10000, B10000, B10000, B10000, B10000, B10000, B10000, B10000};
byte on[] = {B00000, B00000, B00000, B00001, B00010, B10100, B01000, B00000};
byte off[] = {B00000, B00000, B10001, B01010, B00100, B01010, B10001, B00000};

// ============================================================================
//   Global Variables
// ============================================================================
hw_timer_t *sevenSegmentTimer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
volatile bool flag = true;

MenuLcd currentMenu = HOME;
byte lockTime = 0;
boolean firstDigit;

EEPROMData eeprom;
byte tempByte;
int alarmIndex;
byte hourNow, minuteNow, secondNow, dayNow, monthNow, dayOfWeekNow;
byte prevHour, prevMinute, prevSecond, prevDay, prevMonth, prevDayOfWeek;
unsigned int yearNow, prevYear;
byte temperatureNow, prevTemperature;
DS3231 myRTC;
bool century;
bool h12Flag; // 12-hour format flag
bool pmFlag;  // AM/PM flag
byte duration;
int count;

Preferences preferences;
BluetoothSerial SerialBT;
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

// ============================================================================
//   Setup for EEPROM Initialization (default password / config)
// ============================================================================
#ifndef RELEASE
void setup() {
    preferences.begin(DB_NAME, false);

    // Initialize EEPROM with default values
    preferences.putInt(PROGRAM_TYPE_KEY, 0);
    preferences.putInt(NBR_ALARMS_KEY, 0);
    preferences.putInt(DESCRIPTION_LEN_KEY, 0);
    preferences.putInt(AUTHOR_LEN_KEY, 0);
    preferences.putInt(STATE_KEY, 0);

    const byte defaultPassword[32] = { // sha256 of "0000"
        0x9a, 0xf1, 0x5b, 0x33, 0x6e, 0x6a, 0x96, 0x19,
        0x92, 0x85, 0x37, 0xdf, 0x30, 0xb2, 0xe6, 0xa2,
        0x37, 0x65, 0x69, 0xfc, 0xf9, 0xd7, 0xe7, 0x73,
        0xec, 0xce, 0xde, 0x65, 0x60, 0x65, 0x29, 0xa0
    };
    preferences.putBytes(PASSWORD_KEY, defaultPassword, PASSWORD_LEN);
    preferences.end();
}

void loop() {}
#else

// ============================================================================
//   Main Setup for RELEASE / Production
// ============================================================================
void setup() {
    // Configure buttons
    pinMode(LOCK_BTN, INPUT_PULLUP);
    pinMode(DOWN_BTN, INPUT_PULLUP);
    pinMode(UP_BTN, INPUT_PULLUP);

    // Configure outputs
    pinMode(BUZZER, OUTPUT);
    pinMode(LED, OUTPUT);
    pinMode(RELAY, OUTPUT);
    pinMode(DIGIT1, OUTPUT);
    pinMode(DIGIT2, OUTPUT);

    // Configure 7-segment binary inputs as outputs
    pinMode(A, OUTPUT);
    pinMode(B, OUTPUT);
    pinMode(C, OUTPUT);
    pinMode(D, OUTPUT);

    // Initialize LCD
    lcd.begin(16, 2);
    lcd.createChar(THERMOMETER_CHAR, thermometre);
    lcd.createChar(NEXT_CHAR, next);
    lcd.createChar(PREV_CHAR, prev);
    lcd.createChar(LINE_CHAR, line);
    lcd.createChar(ON_CHAR, on);
    lcd.createChar(OFF_CHAR, off);

    lcd.home();
    lcd.print("    OpenTimer   ");

    // Initialize I2C
    Wire.begin();

    // Load stored eeprom from Preferences (EEPROM)
    preferences.begin(DB_NAME, true);
    getProgramType();
    getAlarms();
    getDescription();
    getAuthor();
    getPassword();
    getState();
    preferences.end();

    serialFlush();
    delay(MSG_DELAY);

    // Initialize time and home screen
    updateTime();
    initHome();

    // Initialize 7-segment display timer
    sevenSegmentTimer = timerBegin(0, 80, true); // 80 MHz / 80 = 1 MHz
    timerAttachInterrupt(sevenSegmentTimer, &onSevenSegmentDisplayToggle, true);
    timerAlarmWrite(sevenSegmentTimer, DISPLAY_PERIOD_7SEGMENT * 1000, true);
    timerAlarmEnable(sevenSegmentTimer);

    if (eeprom.programType == 1) {
        initRelay();
    }

    // Start Bluetooth
    SerialBT.begin("Inch Bluetooth");
}

// ============================================================================
//   7-Segment Display Timer ISR
// ============================================================================
void IRAM_ATTR onSevenSegmentDisplayToggle() {
    firstDigit = !firstDigit;

    // Enable both digits
    REG_WRITE(GPIO_OUT_REG, REG_READ(GPIO_OUT_REG) | 1 << DIGIT1 | 1 << DIGIT2);

    // Disable segments ABCD temporarily
    REG_WRITE(GPIO_OUT_REG, REG_READ(GPIO_OUT_REG) & ~(ABCD));

    if (firstDigit) {
        REG_WRITE(GPIO_OUT_REG, REG_READ(GPIO_OUT_REG) | bcd[duration / 10]);
        REG_WRITE(GPIO_OUT_REG, REG_READ(GPIO_OUT_REG) & ~(1 << DIGIT1));
    } else {
        REG_WRITE(GPIO_OUT_REG, REG_READ(GPIO_OUT_REG) | bcd[duration % 10]);
        REG_WRITE(GPIO_OUT_REG, REG_READ(GPIO_OUT_REG) & ~(1 << DIGIT2));
    }

    count = (count + 1) % 1000;
    if (count == 0) {
        portENTER_CRITICAL_ISR(&timerMux);
        flag = true;
        portEXIT_CRITICAL_ISR(&timerMux);
    }
}

// ============================================================================
//   Main Loop
// ============================================================================
void loop() {
    readBtns();

    if (flag) {
        portENTER_CRITICAL(&timerMux);
        flag = false;
        portEXIT_CRITICAL(&timerMux);
        everySecond(); // Called every 1 second
    }

    handleMenu();   // Handle LCD menu navigation
    execRequest();  // Handle Bluetooth requests
}

#endif
