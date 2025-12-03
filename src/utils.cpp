#include "utils.h"
#include "global_vars.h"

// ============================================================================
//   Global Variables
// ============================================================================
byte buttonsPressed; // Stores the state of LOCK, UP, and DOWN buttons

// ============================================================================
//   Read Buttons
//   Checks the state of each button and sets the corresponding bits.
// ============================================================================
void readBtns() {
    buttonsPressed = 0b000;

    if (digitalRead(LOCK_BTN) == LOW) {
        delay(DEBOUNCE);
        bitSet(buttonsPressed, 2); // LOCK button
    }
    if (digitalRead(DOWN_BTN) == LOW) {
        delay(DEBOUNCE);
        bitSet(buttonsPressed, 1); // DOWN button
    }
    if (digitalRead(UP_BTN) == LOW) {
        delay(DEBOUNCE);
        bitSet(buttonsPressed, 0); // UP button
    }
}

// ============================================================================
//   Handle Menu Navigation
// ============================================================================
void handleMenu() {
    switch (currentMenu) {
        case HOME:
            if (isPressed(LOCK_BTN)) {
                tone(BUZZER, 1500, DEBOUNCE);
                if (eeprom.alarmCount == 0) {
                    lcd.home();
                    lcd.clear();
                    lcd.print("    No alarm    ");
                    lcd.setCursor(0, 1);
                    lcd.print("   configured   ");
                    delay(MSG_DELAY);
                    initHome();
                } else {
                    currentMenu = ALARMS;
                    alarmIndex = 0;
                    displayAlarm();
                }
            }
            break;

        case ALARMS:
            if (isPressed(LOCK_BTN)) {
                tone(BUZZER, 1500, DEBOUNCE);
                lockTime = 0;
                currentMenu = HOME;
                initHome();
                delay(DEBOUNCE);
            }
            if (isPressed(UP_BTN)) {
                tone(BUZZER, 1500, DEBOUNCE);
                lockTime = 0;
                alarmIndex = (alarmIndex + 1) % eeprom.alarmCount;
                displayAlarm();
                delay(DEBOUNCE);
            }
            if (isPressed(DOWN_BTN)) {
                tone(BUZZER, 1500, DEBOUNCE);
                lockTime = 0;
                alarmIndex--;
                if (alarmIndex < 0) alarmIndex = eeprom.alarmCount - 1;
                displayAlarm();
                delay(DEBOUNCE);
            }
            break;
    }
}

// ============================================================================
//   Check if a Button is Pressed
// ============================================================================
boolean isPressed(uint8_t btn) {
    if (btn == LOCK_BTN) return bitRead(buttonsPressed, 2);
    if (btn == DOWN_BTN) return bitRead(buttonsPressed, 1);
    if (btn == UP_BTN) return bitRead(buttonsPressed, 0);
    return false;
}

// ============================================================================
//   Update Current Time from RTC
// ============================================================================
void updateTime() {
    // Save previous time values
    prevHour = hourNow; prevMinute = minuteNow; prevSecond = secondNow;
    prevDay = dayNow; prevMonth = monthNow; prevYear = yearNow;
    prevTemperature = temperatureNow; prevDayOfWeek = dayOfWeekNow;

    // Read current time from DS3231 RTC
    hourNow = myRTC.getHour(h12Flag, pmFlag);
    minuteNow = myRTC.getMinute();
    secondNow = myRTC.getSecond();
    dayNow = myRTC.getDate();
    monthNow = myRTC.getMonth(century);
    yearNow = myRTC.getYear() + 1970;
    temperatureNow = (byte)roundf(myRTC.getTemperature());
    dayOfWeekNow = myRTC.getDoW();
}

// ============================================================================
//   Check and Trigger Alarms
// ============================================================================
void checkAndTriggerAlarm() {
    for (byte i = 0; i < eeprom.alarmCount; i++) {
        if (bitRead(eeprom.alarms[i].days, 0) &&       // Alarm active
            eeprom.alarms[i].hour == hourNow &&            // Hour matches
            eeprom.alarms[i].minute == minuteNow &&          // Minute matches
            bitRead(eeprom.alarms[i].days, 8 - dayOfWeekNow)) // Day matches
        {
            if (eeprom.programType == 0) {
                duration = eeprom.alarms[i].duration; // Retrieve duration
            } else {
                digitalWrite(RELAY, eeprom.alarms[i].duration == 0 ? HIGH : LOW);
                digitalWrite(LED, eeprom.alarms[i].duration == 0 ? LOW : HIGH);
            }
            break;
        }
    }
}

// ============================================================================
//   Initialize Relay based on previous Alarm
// ============================================================================
void initRelay() {
    if (eeprom.alarmCount == 0) return;

    int minutes = hourNow * 60 + minuteNow;
    int idx = 0;

    while (minutes < eeprom.alarms[idx].hour * 60 + eeprom.alarms[idx].minute &&
           idx < eeprom.alarmCount) {
        idx++;
    }
    idx--;
    if (idx < 0) idx = eeprom.alarmCount - 1;

    if (bitRead(eeprom.alarms[idx].days, 0)) {
        digitalWrite(RELAY, eeprom.alarms[idx].duration == 0 ? HIGH : LOW);
        digitalWrite(LED, eeprom.alarms[idx].duration == 0 ? LOW : HIGH);
    }
}

// ============================================================================
//   Tasks to Execute Every Second
// ============================================================================
void everySecond() {
    updateTime();

    // Trigger alarm if minute has changed and program is active
    if (minuteNow != prevMinute && eeprom.state) {
        checkAndTriggerAlarm();
    }

    // Handle numeric program type alarms
    if (eeprom.programType == 0) {
        if (duration == 0) {
            digitalWrite(RELAY, HIGH);
        } else {
            digitalWrite(RELAY, LOW);
            duration--;
            tone(BUZZER, BUZZER_FREQ, 300);
        }
    }

    // Refresh display
    if (currentMenu == HOME) {
        refreshHome();
    }

    // Return to HOME menu if ALARMS menu has been idle for too long
    if (currentMenu == ALARMS) {
        lockTime++;
        if (lockTime > LOCK_TIME) {
            currentMenu = HOME;
            lockTime = 0;
            initHome();
        }
    }
}
