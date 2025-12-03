#ifndef MEMORY_H
#define MEMORY_H

// ============================================================================
//   GENERAL CONSTANTS
// ============================================================================

// Maximum number of alarms that can be stored
#define MAX_ALARMS      40

// Maximum lengths for stored eeprom
#define MAX_DESCRIPTION_LEN 100
#define MAX_AUTHOR_LEN      45
#define PASSWORD_LEN        32 // sha256 hash

// Database / namespace name used for storage
#define DB_NAME "OpentimerDB"

// ============================================================================
//   STORAGE KEYS (EEPROM / Preferences / Flash)
//   → Each key identifies a stored element.
// ============================================================================

// Program type (mode or configuration)
#define PROGRAM_TYPE_KEY     "type"

// Number of alarms stored
#define NBR_ALARMS_KEY       "nb"

// Alarms array
#define ALARMS_KEY           "alarms"

// Password
#define PASSWORD_KEY         "passwd"

// Length of program description
#define DESCRIPTION_LEN_KEY  "Dlen"
// Program description
#define DESCRIPTION_KEY      "desc"

// Length of the author's name
#define AUTHOR_LEN_KEY       "Alen"
// Author's name
#define AUTHOR_KEY           "author"

// Program state / system state
#define STATE_KEY            "state"

// ============================================================================
//   STORAGE FUNCTION PROTOTYPES
// ============================================================================

bool storeAlarms();
bool getAlarms();

bool storePassword();
bool getPassword();

bool storeDescription();
bool getDescription();

bool storeAuthor();
bool getAuthor();

bool storeState();
bool getState();

bool storeProgramType();
bool getProgramType();

#endif
