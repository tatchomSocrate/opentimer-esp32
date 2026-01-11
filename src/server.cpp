#include "server.h"
#include "database.h"
#include "datatypes.h"
#include "display.h"
#include "global_vars.h"
#include "utils.h"
#include <Arduino.h>

// ============================================================================
//   Constants
// ============================================================================
#define TX_BUFFER_SIZE 200

// Macros for reading bytes from SerialBT
#define SERIAL_READ_BYTE(x) \
  x = SerialBT.read(); \
  size--;

#define SERIAL_READ_BYTE_S(x) \
  if (size == 0) goto end; \
  if (SerialBT.available() == 0) goto end; \
  SERIAL_READ_BYTE(x)

#define SERIAL_READ_SIZE(x) \
  SERIAL_READ_BYTE_S(x) \
  if (size < x) \
    goto bad;

#define SERIAL_READ() \
  SerialBT.read(); \
  size--;

// Macros for writing bytes to SerialBT with overflow check
#define SERIAL_WRITE_BYTE(x) \
  if (idx >= TX_BUFFER_SIZE) { \
    SerialBT.write(0x01); \
    SerialBT.write(BUFFER_OVERFLOW); \
    SerialBT.flush(); \
    goto end; \
  } \
  txBuffer[idx] = x; \
  idx++;

// Flush remaining SerialBT eeprom
#define FLUSH_REQUEST() \
  while (size != 0) { \
    SerialBT.read(); \
    size--; \
  }

// ============================================================================
//   Global Variables
// ============================================================================
unsigned char txBuffer[TX_BUFFER_SIZE]; // Buffer for outgoing eeprom
byte size = 0;
TaskHandle_t taskHandle = NULL; // Handle to cancel timeout task

bool ledOn = true;

// ============================================================================
//   Timeout Task for Bluetooth Request
// ============================================================================
void enableTimeout() {
    if (taskHandle != NULL) return; // Already running
    xTaskCreatePinnedToCore(
        taskFunction,   // Function to run
        "TimeoutTask",  // Task name
        2048,           // Stack size (2 KB)
        NULL,           // Parameters
        1,              // Priority
        &taskHandle,    // Task handle
        1               // Run on core 1
    );
}

void disableTimeout() {
    if (taskHandle != NULL) {
        vTaskDelete(taskHandle);
        taskHandle = NULL;
    }
}

void toggleLed() {
    digitalWrite(LED, ledOn ? HIGH : LOW);
    ledOn = !ledOn;
}

// Task executed 2 seconds after request start
void taskFunction(void *parameter) {
    vTaskDelay(2000 / portTICK_PERIOD_MS); // Wait 2 seconds

    SerialBT.write(0x01);
    SerialBT.write(TIMEOUT);
    SerialBT.flush();
    serialFlush();
    toggleLed();
    taskHandle = NULL;
    vTaskDelete(NULL); // Delete itself
}

// ============================================================================
//   Helper: Flush SerialBT input
// ============================================================================
void serialFlush() {
    while (SerialBT.available()) {
        SerialBT.read();
    }
    size = 0;
}

// ============================================================================
//   Main Bluetooth Request Handler
// ============================================================================
void execRequest() {
    if (SerialBT.available() < 1) return;

    if (size == 0) {
        size = SerialBT.read(); // First byte = total size
        enableTimeout();
    }

    if (SerialBT.available() < size) return; // Wait until all eeprom received
    disableTimeout();

    bool passwordSent = false;
    bool isPasswordCorrect = false;
    bool showSuccessMsg = false;

    byte idx = 1; // First byte reserved for length in response

    while (true) {
        SERIAL_READ_BYTE_S(tempByte);

        switch (tempByte) {

            // ------------------ Connection Events ------------------
            case CONNECTED:
                lcd.home(); lcd.clear();
                tone(BUZZER, BUZZER_FREQ, 200);
                lcd.print("   CONNECTED    ");
                delay(300);
                tone(BUZZER, BUZZER_FREQ, 200);
                delay(MSG_DELAY);
                if (currentMenu == HOME) initHome(); else displayAlarm();
                break;

            case DISCONNECTED:
                lcd.home(); lcd.clear();
                tone(BUZZER, BUZZER_FREQ, 200);
                lcd.print("  DISCONNECTED  ");
                delay(300);
                tone(BUZZER, BUZZER_FREQ, 200);
                delay(MSG_DELAY);
                if (currentMenu == HOME) initHome(); else displayAlarm();
                break;

            // ------------------ GET Commands ------------------
            case GET_PROGRAM_TYPE:
                SERIAL_WRITE_BYTE(SET_PROGRAM_TYPE);
                SERIAL_WRITE_BYTE(eeprom.programType);
                break;

            case GET_ALARMS:
                SERIAL_WRITE_BYTE(SET_ALARMS);
                SERIAL_WRITE_BYTE(4 * eeprom.alarmCount);
                for (byte i = 0; i < eeprom.alarmCount; i++) {
                    SERIAL_WRITE_BYTE(eeprom.alarms[i].hour);
                    SERIAL_WRITE_BYTE(eeprom.alarms[i].minute);
                    SERIAL_WRITE_BYTE(eeprom.alarms[i].duration);
                    SERIAL_WRITE_BYTE(eeprom.alarms[i].days);
                }
                break;

            case GET_DESCRIPTION:
                SERIAL_WRITE_BYTE(SET_DESCRIPTION);
                SERIAL_WRITE_BYTE(eeprom.descriptionLength);
                for (byte i = 0; i < eeprom.descriptionLength; i++) {
                    SERIAL_WRITE_BYTE(eeprom.description[i]);
                }
                break;

            case GET_AUTHOR:
                SERIAL_WRITE_BYTE(SET_AUTHOR);
                SERIAL_WRITE_BYTE(eeprom.authorLength);
                for (byte i = 0; i < eeprom.authorLength; i++) {
                    SERIAL_WRITE_BYTE(eeprom.author[i]);
                }
                break;

            case GET_HOUR: SERIAL_WRITE_BYTE(SET_HOUR); SERIAL_WRITE_BYTE(myRTC.getHour(h12Flag, pmFlag)); break;
            case GET_MINUTE: SERIAL_WRITE_BYTE(SET_MINUTE); SERIAL_WRITE_BYTE(myRTC.getMinute()); break;
            case GET_SECOND: SERIAL_WRITE_BYTE(SET_SECOND); SERIAL_WRITE_BYTE(myRTC.getSecond()); break;
            case GET_DAY_OF_WEEK: SERIAL_WRITE_BYTE(SET_DAY_OF_WEEK); SERIAL_WRITE_BYTE(myRTC.getDoW()); break;
            case GET_DAY: SERIAL_WRITE_BYTE(SET_DAY); SERIAL_WRITE_BYTE(myRTC.getDate()); break;
            case GET_MONTH: SERIAL_WRITE_BYTE(SET_MONTH); SERIAL_WRITE_BYTE(myRTC.getMonth(century)); break;
            case GET_YEAR: SERIAL_WRITE_BYTE(SET_YEAR); SERIAL_WRITE_BYTE(myRTC.getYear()); break;
            case GET_TEMPERATURE: SERIAL_WRITE_BYTE(SET_TEMPERATURE); SERIAL_WRITE_BYTE((byte)roundf(myRTC.getTemperature())); break;
            case GET_STATE: SERIAL_WRITE_BYTE(SET_STATE); SERIAL_WRITE_BYTE(eeprom.state); break;

            // ------------------ SET Commands ------------------
            case SET_PROGRAM_TYPE:
                SERIAL_READ_BYTE_S(tempByte);
                if (isPasswordCorrect) {
                    eeprom.programType = tempByte;
                    if (!storeProgramType) { handleError(); return; }
                }
                break;

            case SET_PASSWORD:
                if (size < PASSWORD_LEN) goto bad;
                if (isPasswordCorrect) {
                    for (byte i = 0; i < PASSWORD_LEN; i++) SERIAL_READ_BYTE(eeprom.password[i]);
                    if (!storePassword()) { handleError(); return; }
                } else {
                    for (byte i = 0; i < PASSWORD_LEN; i++) SERIAL_READ();
                }
                break;

            case SET_ALARMS:
                SERIAL_READ_SIZE(tempByte);
                tempByte /= 4;
                if (tempByte > MAX_ALARMS) goto bad;
                if (isPasswordCorrect) {
                    eeprom.alarmCount = tempByte;
                    for (byte i = 0; i < eeprom.alarmCount; i++) {
                        SERIAL_READ_BYTE(tempByte); if (tempByte < 24) eeprom.alarms[i].hour = tempByte; else { getAlarms(); goto bad; }
                        SERIAL_READ_BYTE(tempByte); if (tempByte < 60) eeprom.alarms[i].minute = tempByte; else { getAlarms(); goto bad; }
                        SERIAL_READ_BYTE(tempByte); if (tempByte < 100) eeprom.alarms[i].duration = tempByte; else { getAlarms(); goto bad; }
                        SERIAL_READ_BYTE(eeprom.alarms[i].days);
                    }
                    if (!storeAlarms()) { handleError(); return; }
                } else {
                    for (byte i = 0; i < tempByte; i++) { SERIAL_READ(); SERIAL_READ(); SERIAL_READ(); SERIAL_READ(); }
                }
                break;

            case SET_DESCRIPTION:
                SERIAL_READ_SIZE(tempByte);
                if (tempByte > MAX_DESCRIPTION_LEN) goto bad;
                if (isPasswordCorrect) {
                    eeprom.descriptionLength = tempByte;
                    for (byte i = 0; i < tempByte; i++) SERIAL_READ_BYTE(eeprom.description[i]);
                    if (!storeDescription()) { handleError(); return; }
                } else { for (byte i = 0; i < tempByte; i++) SERIAL_READ(); }
                break;

            case SET_AUTHOR:
                SERIAL_READ_SIZE(tempByte);
                if (tempByte > MAX_AUTHOR_LEN) goto bad;
                if (isPasswordCorrect) {
                    eeprom.authorLength = tempByte;
                    for (byte i = 0; i < tempByte; i++) SERIAL_READ_BYTE(eeprom.author[i]);
                    if (!storeAuthor()) { handleError(); return; }
                } else { for (byte i = 0; i < tempByte; i++) SERIAL_READ(); }
                break;

            case SET_HOUR:
                SERIAL_READ_BYTE_S(tempByte);
                if (tempByte > 23) goto bad;
                if (isPasswordCorrect) myRTC.setHour(tempByte);
                break;
            case SET_MINUTE:
                SERIAL_READ_BYTE_S(tempByte);
                if (tempByte > 59) goto bad;
                if (isPasswordCorrect) myRTC.setMinute(tempByte);
                break;
            case SET_SECOND:
                SERIAL_READ_BYTE_S(tempByte);
                if (tempByte > 59) goto bad;
                if (isPasswordCorrect) myRTC.setSecond(tempByte);
                break;
            case SET_DAY_OF_WEEK:
                SERIAL_READ_BYTE_S(tempByte);
                if (tempByte < 1 || tempByte > 7) goto bad;
                if (isPasswordCorrect) myRTC.setDoW(tempByte);
                break;
            case SET_DAY:
                SERIAL_READ_BYTE_S(tempByte);
                if (tempByte < 1 || tempByte > 31) goto bad;
                if (isPasswordCorrect) myRTC.setDate(tempByte);
                break;
            case SET_MONTH:
                SERIAL_READ_BYTE_S(tempByte);
                if (tempByte < 1 || tempByte > 12) goto bad;
                if (isPasswordCorrect) myRTC.setMonth(tempByte);
                break;
            case SET_YEAR:
                SERIAL_READ_BYTE_S(tempByte);
                if (tempByte > 99) goto bad;
                if (isPasswordCorrect) myRTC.setYear(tempByte);
                break;
            case SET_STATE:
                SERIAL_READ_BYTE_S(tempByte);
                if (isPasswordCorrect) {
                    eeprom.state = tempByte;
                    if (!storeState()) { handleError(); return; }
                }
                break;

            // ------------------ Password Handling ------------------
            case POST_PASSWORD:
                showSuccessMsg=true;
            case POST_PASSWORD_UPLOAD:
            case POST_PASSWORD_CHANGE:
                passwordSent = true;
                if (size < PASSWORD_LEN) goto bad;
                isPasswordCorrect = true;
                for (byte i = 0; i < PASSWORD_LEN; i++) {
                    byte b;
                    SERIAL_READ_BYTE(b);
                    if (eeprom.password[i] != b) isPasswordCorrect = false;
                }
                SERIAL_WRITE_BYTE(POST_PASSWORD_RESPONSE);
                if(tempByte == POST_PASSWORD){
                    SERIAL_WRITE_BYTE(isPasswordCorrect ? PASSWORD_RESPONSE_CORRECT : PASSWORD_RESPONSE_INCORRECT);
                }else if(tempByte == POST_PASSWORD_UPLOAD){
                    SERIAL_WRITE_BYTE(isPasswordCorrect ? PASSWORD_RESPONSE_UPLOAD : PASSWORD_RESPONSE_INCORRECT);
                }else if(tempByte == POST_PASSWORD_CHANGE){
                    SERIAL_WRITE_BYTE(isPasswordCorrect ? PASSWORD_RESPONSE_CHANGE : PASSWORD_RESPONSE_INCORRECT);
                }
                
                if (isPasswordCorrect) preferences.begin(DB_NAME, false);
                break;
            default:
                lcd.home(); lcd.clear();
                tone(BUZZER, BUZZER_FREQ, MSG_DELAY);
                lcd.print(tempByte);
                delay(MSG_DELAY);
                if (currentMenu == HOME) initHome(); else displayAlarm();
        } // switch
    } // while

bad:
    SerialBT.write(0x01);
    SerialBT.write(BAD_REQUEST);
    SerialBT.flush();

end:
    if (idx > 1) {
        txBuffer[0] = idx - 1;
        SerialBT.write(txBuffer, idx);
        SerialBT.flush();
    }

    if (isPasswordCorrect) preferences.end();
    FLUSH_REQUEST();

    // ------------------ Feedback on LCD ------------------
    if (showSuccessMsg && isPasswordCorrect) {
        lcd.home(); lcd.clear();
        tone(BUZZER, BUZZER_FREQ, 200);
        lcd.print("      DONE      ");
        lcd.setCursor(0,1); lcd.print("    SUCCESS !   ");
        delay(300); tone(BUZZER, BUZZER_FREQ, 200); delay(MSG_DELAY);
        if (currentMenu == HOME) initHome(); else displayAlarm();
    }

    if (passwordSent && !isPasswordCorrect) {
        lcd.home(); lcd.clear();
        tone(BUZZER, BUZZER_FREQ, 200);
        lcd.print(" WRONG PASSWORD ");
        delay(300); tone(BUZZER, BUZZER_FREQ, 200); delay(MSG_DELAY);
        if (currentMenu == HOME) initHome(); else displayAlarm();
    }
}

// ============================================================================
//   Error Handler
// ============================================================================
void handleError() {
    SerialBT.write(0x01);
    SerialBT.write(ERROR);
    SerialBT.flush();
    FLUSH_REQUEST();
    lcd.home(); lcd.clear();
    tone(BUZZER, BUZZER_FREQ, MSG_DELAY);
    lcd.print("     ERROR!     ");
    delay(MSG_DELAY);
    if (currentMenu == HOME) initHome(); else displayAlarm();
}
