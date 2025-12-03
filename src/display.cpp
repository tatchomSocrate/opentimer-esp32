#include "display.h"
#include "global_vars.h"

// ============================================================================
//   Initialize the Home Screen
//   Displays current date, time, program state, and temperature on the LCD.
// ============================================================================
void initHome() {
    lcd.clear();
    lcd.home();

    // -------------------------
    // Display Date: Day, DD/MM/YYYY
    // -------------------------
    lcd.setCursor(1, 0);
    lcd.print(DaysOfWeek[dayOfWeekNow - 1]); // Day of week
    lcd.print(',');
    lcd.print(dayNow / 10);
    lcd.print(dayNow % 10);
    lcd.print('/');
    lcd.print(monthNow / 10);
    lcd.print(monthNow % 10);
    lcd.print('/');
    lcd.print((yearNow / 1000) % 10);
    lcd.print((yearNow / 100) % 10);
    lcd.print((yearNow / 10) % 10);
    lcd.print(yearNow % 10);

    // -------------------------
    // Display Time: HH:MM:SS
    // -------------------------
    lcd.setCursor(0, 1);
    lcd.print(hourNow / 10);
    lcd.print(hourNow % 10);
    lcd.print(':');
    lcd.print(minuteNow / 10);
    lcd.print(minuteNow % 10);
    lcd.print(':');
    lcd.print(secondNow / 10);
    lcd.print(secondNow % 10);

    // -------------------------
    // Display Program AlarmState and Temperature
    // -------------------------
    lcd.print(' ');
    lcd.write(eeprom.state ? ON_CHAR : OFF_CHAR); // ON_CHAR/OFF_CHAR symbol
    lcd.print(' ');
    lcd.write(THERMOMETER_CHAR);           // Thermometer symbol
    lcd.print((byte)temperatureNow / 10);
    lcd.print((byte)temperatureNow % 10);
    lcd.write(DEGRE_CHAR);                 // Degree symbol
    lcd.print('C');
}

// ============================================================================
//   Refresh Home Screen
//   Updates only the parts of the display that have changed since last refresh.
// ============================================================================
void refreshHome() {
    if (dayOfWeekNow != prevDayOfWeek) {
        lcd.setCursor(1, 0);
        lcd.print(DaysOfWeek[dayOfWeekNow - 1]);
    }
    if (dayNow != prevDay) {
        lcd.setCursor(5, 0);
        lcd.print(dayNow / 10);
        lcd.print(dayNow % 10);
    }
    if (monthNow != prevMonth) {
        lcd.setCursor(8, 0);
        lcd.print(monthNow / 10);
        lcd.print(monthNow % 10);
    }
    if (yearNow != prevYear) {
        lcd.setCursor(11, 0);
        lcd.print((yearNow / 1000) % 10);
        lcd.print((yearNow / 100) % 10);
        lcd.print((yearNow / 10) % 10);
        lcd.print(yearNow % 10);
    }
    if (hourNow != prevHour) {
        lcd.setCursor(0, 1);
        lcd.print(hourNow / 10);
        lcd.print(hourNow % 10);
    }
    if (minuteNow != prevMinute) {
        lcd.setCursor(3, 1);
        lcd.print(minuteNow / 10);
        lcd.print(minuteNow % 10);
    }
    if (secondNow != prevSecond) {
        lcd.setCursor(6, 1);
        lcd.print(secondNow / 10);
        lcd.print(secondNow % 10);
    }
    if (temperatureNow != prevTemperature) {
        lcd.setCursor(12, 1);
        lcd.print(temperatureNow / 10);
        lcd.print(temperatureNow % 10);
    }
}

// ============================================================================
//   Display Alarm
//   Shows alarm details including time, duration/state, active days, and navigation.
// ============================================================================
void displayAlarm() {
    lcd.clear();
    lcd.home();

    // -------------------------
    // Display Alarm Number
    // -------------------------
    lcd.print('n');
    lcd.write(DEGRE_CHAR); // Degree symbol used as a separator
    lcd.print((alarmIndex + 1) / 10);
    lcd.print((alarmIndex + 1) % 10);
    lcd.print("  ");

    // -------------------------
    // Display Alarm Time HH:MM
    // -------------------------
    lcd.print(eeprom.alarms[alarmIndex].hour / 10);
    lcd.print(eeprom.alarms[alarmIndex].hour % 10);
    lcd.print(':');
    lcd.print(eeprom.alarms[alarmIndex].minute / 10);
    lcd.print(eeprom.alarms[alarmIndex].minute % 10);
    lcd.print("  ");

    // -------------------------
    // Display Duration or AlarmState
    // -------------------------
    if (eeprom.programType == 0) { // Numeric duration mode
        lcd.print(eeprom.alarms[alarmIndex].duration / 10);
        lcd.print(eeprom.alarms[alarmIndex].duration % 10);
        lcd.print('s');
    } else { // ON_CHAR/OFF_CHAR mode
        lcd.print(AlarmState[bitRead(eeprom.alarms[alarmIndex].duration, 0)]);
    }

    // -------------------------
    // Display Active Days
    // -------------------------
    lcd.setCursor(0, 1);
    for (int i = 6; i >= 1; i--) {
        lcd.print(bitRead(eeprom.alarms[alarmIndex].days, i) ? 
        DaysOfWeek[7 - i][0] : '_');
    }
    lcd.print(bitRead(eeprom.alarms[alarmIndex].days, 7) ? 
        DaysOfWeek[0][0] : '_');

    // Display alarm ON_CHAR/OFF_CHAR indicator
    lcd.print("  ");
    lcd.write(bitRead(eeprom.alarms[alarmIndex].days, 0) ? ON_CHAR : OFF_CHAR);

    // -------------------------
    // Display Navigation Arrows
    // -------------------------
    if (alarmIndex == 0) {
        lcd.setCursor(15, 1);
        lcd.write(NEXT_CHAR);
    } else if (alarmIndex == eeprom.alarmCount - 1) {
        lcd.setCursor(13, 1);
        lcd.write(PREV_CHAR);
    } else {
        lcd.setCursor(13, 1);
        lcd.write(PREV_CHAR);
        lcd.setCursor(15, 1);
        lcd.write(NEXT_CHAR);
    }
}
