/*
 ============================================================================
 Name        : pacLED.h
 Author      : Robert Abram, Katie Snow
 Version     :
 Copyright   : Copyright 2014 Robert Abram, Katie Snow
 Description : Ultimarc PacLED64 configuration library
 ============================================================================
 */

#ifndef PACLED_H_
#define PACLED_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Required items for writing out through the USB port */
#define PACLED_VENDOR        0xD209
#define PACLED_PRODUCT       0x1401
#define PACLED_DATA_SIZE     2
#define PACLED_REPORT        0x03
#define PACLED_VALUE         0x0200
#define PACLED_INDEX         0
#define PACLED_MESG_LENGTH   2
#define PACLED_INTERFACE     0
#define PACLED_FADE_BASE     64
#define PACLED_FADE_ALL_BASE 4

typedef struct json_object json_object;
typedef struct ulboard ulboard;

struct pacLED
{
  bool ledMapFade;
  bool ledMapIntensity;
  bool allIntensities;
  bool allFade;
  bool random;
  bool boardIDUpdate;
};

/*
 * Determine if the json file is an pacLED configuration
 */
bool isPACLED64Config(json_object* jobj, ulboard* board);
bool validatePacLED64Data(json_object* jobj);
bool updateBoardPacLED (json_object* jobj);

#ifdef __cplusplus
}
#endif

#endif /* PACLED_H_ */
