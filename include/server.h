#ifndef REQUEST_H
#define REQUEST_H

// ============================================================================
//   Request Codes for Bluetooth Communication
// ============================================================================
// These codes represent the type of request sent/received over Bluetooth.
enum RequestCodes {
    CONNECTED,           // Client connected
    GET_ALARMS,          // Request alarms
    GET_DESCRIPTION,     // Request program description
    GET_AUTHOR,          // Request author name
    GET_HOUR,            // Request current hour
    GET_MINUTE,          // Request current minute
    GET_SECOND,          // Request current second
    GET_DAY_OF_WEEK,     // Request day of the week
    GET_DAY,             // Request day of the month
    GET_MONTH,           // Request month
    GET_YEAR,            // Request year
    GET_TEMPERATURE,     // Request temperature from sensor
    GET_STATE,           // Request program or system state
    SET_PASSWORD,        // Set password
    SET_ALARMS,          // Set alarms
    SET_DESCRIPTION,     // Set program description
    SET_AUTHOR,          // Set author
    SET_HOUR,            // Set hour
    SET_MINUTE,          // Set minute
    SET_SECOND,          // Set second
    SET_DAY_OF_WEEK,     // Set day of the week
    SET_DAY,             // Set day of the month
    SET_MONTH,           // Set month
    SET_YEAR,            // Set year
    SET_TEMPERATURE,     // Set temperature (if applicable)
    SET_STATE,           // Set program or system state
    POST_PASSWORD,       // Post password for verification
    POST_PASSWORD_CHANGE,// Post request to change password
    POST_PASSWORD_UPLOAD,// Post request to upload program
    POST_PASSWORD_RESPONSE, // Post password verification response
    BUFFER_OVERFLOW,     // EEPROMData buffer overflow
    BAD_REQUEST,         // Invalid request
    TIMEOUT,             // Communication timeout
    SET_PROGRAM_TYPE,    // Set program type
    GET_PROGRAM_TYPE,    // Get program type
    DISCONNECTED,        // Client disconnected
    ERROR                // General error
};

// ============================================================================
//   Password Response Codes
// ============================================================================
// Defines responses returned after password verification.
enum PasswordResponse {
    PASSWORD_RESPONSE_INCORRECT, // Password incorrect
    PASSWORD_RESPONSE_CORRECT, // Password correct
    PASSWORD_RESPONSE_CHANGE, // Password correct, proceed to change password
    PASSWORD_RESPONSE_UPLOAD  // Password correct, proceed to upload a program
};

// ============================================================================
//   Function Prototypes
// ============================================================================

/**
 * Execute a Bluetooth request, update EEPROM and MEM_ADDR as needed.
 */
void execRequest();

/**
 * Clear the serial receive buffer.
 */
void serialFlush();

/**
 * Task function for FreeRTOS or loop execution.
 * @param parameter Pointer to task parameter (unused)
 */
void taskFunction(void *parameter);

/**
 * Disable request timeout handling.
 */
void disableTimeout();

/**
 * Enable request timeout handling.
 */
void enableTimout();

/**
 * Handle communication or processing errors.
 */
void handleError();

#endif
