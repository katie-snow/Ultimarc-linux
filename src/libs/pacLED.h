/*
 * pacLED.h
 *
 *  Created on: Mar 23, 2014
 *      Author: katie
 */

#ifndef PACLED_H_
#define PACLED_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PACLED_VERSION 1
#define PACLED_PRODUCT_STR "1401"

/* Required items for writing out through the USB port */
#define PACLED_VENDOR        0xD209
#define PACLED_PRODUCT       0x1401
#define PACLED_DATA_SIZE     2
#define PACLED_REPORT        0x03
#define PACLED_REQUEST_TYPE  0x21
#define PACLED_REQUEST       9
#define PACLED_VALUE         0x0200
#define PACLED_INDEX         0
#define PACLED_MESG_LENGTH   2
#define PACLED_TIMEOUT       2000
#define PACLED_INTERFACE     0
#define PACLED_FADE_BASE     64
#define PACLED_FADE_ALL_BASE 4

typedef struct json_object json_object;
struct libusb_device_handle;

struct pacLED
{
  bool ledMapFade;
  bool ledMapIntensity;
  bool allIntensities;
  bool allFade;
  bool random;
};

/*
 * Determine if the json file is an pacLED configuration
 */
bool isPacLED (json_object* jobj);

char decToHex (int decimal);

bool updateBoardPacLED (json_object* jobj);

#ifdef __cplusplus
}
#endif

#endif /* PACLED_H_ */
