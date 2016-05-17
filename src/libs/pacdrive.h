/*
 ============================================================================
 Name        : pacdrive.h
 Author      : Robert Abram, Katie Snow
 Version     :
 Copyright   : Copyright 2014 Robert Abram, Katie Snow
 Description : Ultimarc PacDrive configuration library
 ============================================================================
 */

#ifndef PACDRIVE_H_
#define PACDRIVE_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Required items for writing out through the USB port */
#define PACDRIVE_VENDOR        0xD209
#define PACDRIVE_PRODUCT       0x1500
#define PACDRIVE_DATA_SIZE     4
#define PACDRIVE_REPORT        0x03
#define PACDRIVE_VALUE         0x0200
#define PACDRIVE_INDEX         0
#define PACDRIVE_MESG_LENGTH   4
#define PACDRIVE_INTERFACE     0

typedef struct json_object json_object;
typedef struct ulboard ulboard;

/*
 * Determine if the json file is an PAC drive configuration
 */
bool isPACDriveConfig(json_object* jobj, ulboard* board);
bool validatePacDriveData(json_object* jobj);
bool updateBoardPacDrive (json_object* jobj);

/*
 * Have to convert from 1-16 to
 * bit representation (1,2,4,8.16.32.64,128)
 * This allows the bitwise OR to work in the
 * updateBoardPacDrive function.
 */
int convertLED (int led);


#ifdef __cplusplus
}
#endif

#endif /* PACDRIVE_H_ */
