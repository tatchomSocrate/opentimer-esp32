#include "database.h"
#include "global_vars.h"
#include <Arduino.h>

// ============================================================================
//   Store Alarms in Preferences (EEPROM/Flash)
// ============================================================================
bool storeAlarms() {
    // Store number of alarms
    if (preferences.putInt(NBR_ALARMS_KEY, eeprom.alarmCount) == 0)
        return false;

    // Calculate total bytes for alarms (each alarm = 4 bytes)
    size_t nbBytes = eeprom.alarmCount * 4;

    // Store alarm array
    if (preferences.putBytes(ALARMS_KEY, eeprom.alarms, nbBytes) != nbBytes)
        return false;

    return true;
}

// ============================================================================
//   Retrieve Alarms from Preferences
// ============================================================================
bool getAlarms() {
    int value = preferences.getInt(NBR_ALARMS_KEY, -1);

    if (value == -1 || value > MAX_ALARMS)
        return false;

    eeprom.alarmCount = value;
    size_t nbBytes = eeprom.alarmCount * 4;

    if (preferences.getBytes(ALARMS_KEY, eeprom.alarms, nbBytes) != nbBytes)
        return false;

    return true;
}

// ============================================================================
//   Store Password
// ============================================================================
bool storePassword() {
    return preferences.putBytes(PASSWORD_KEY, eeprom.password, PASSWORD_LEN) == PASSWORD_LEN;
}

// ============================================================================
//   Retrieve Password
// ============================================================================
bool getPassword() {
    return preferences.getBytes(PASSWORD_KEY, eeprom.password, PASSWORD_LEN) == PASSWORD_LEN;
}

// ============================================================================
//   Store Program Description
// ============================================================================
bool storeDescription() {
    if (preferences.putInt(DESCRIPTION_LEN_KEY, eeprom.descriptionLength) == 0)
        return false;

    if (preferences.putBytes(DESCRIPTION_KEY, eeprom.description, eeprom.descriptionLength) != eeprom.descriptionLength)
        return false;

    return true;
}

// ============================================================================
//   Retrieve Program Description
// ============================================================================
bool getDescription() {
    int value = preferences.getInt(DESCRIPTION_LEN_KEY, -1);

    if (value == -1 || value > MAX_DESCRIPTION_LEN)
        return false;

    eeprom.descriptionLength = value;

    if (preferences.getBytes(DESCRIPTION_KEY, eeprom.description, value) != value)
        return false;

    return true;
}

// ============================================================================
//   Store Author Name
// ============================================================================
bool storeAuthor() {
    if (preferences.putInt(AUTHOR_LEN_KEY, eeprom.authorLength) == 0)
        return false;

    if (preferences.putBytes(AUTHOR_KEY, eeprom.author, eeprom.authorLength) != eeprom.authorLength)
        return false;

    return true;
}

// ============================================================================
//   Retrieve Author Name
// ============================================================================
bool getAuthor() {
    int value = preferences.getInt(AUTHOR_LEN_KEY, -1);

    if (value == -1 || value > MAX_AUTHOR_LEN)
        return false;

    eeprom.authorLength = value;

    if (preferences.getBytes(AUTHOR_KEY, eeprom.author, value) != value)
        return false;

    return true;
}

// ============================================================================
//   Store Program AlarmState
// ============================================================================
bool storeState() {
    return preferences.putInt(STATE_KEY, eeprom.state) != 0;
}

// ============================================================================
//   Retrieve Program AlarmState
// ============================================================================
bool getState() {
    int value = preferences.getInt(STATE_KEY, -1);

    if (value == -1 || value > 255)
        return false;

    eeprom.state = value;
    return true;
}

// ============================================================================
//   Store Program Type
// ============================================================================
bool storeProgramType() {
    return preferences.putInt(PROGRAM_TYPE_KEY, eeprom.programType) != 0;
}

// ============================================================================
//   Retrieve Program Type
// ============================================================================
bool getProgramType() {
    int value = preferences.getInt(PROGRAM_TYPE_KEY, -1);

    if (value == -1 || value > 255)
        return false;

    eeprom.programType = value;
    return true;
}
