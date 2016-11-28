/*
 ============================================================================
 Name        : uhid.h
 Author      : Katie Snow
 Version     :
 Copyright   : Copyright 2015 Robert Abram, Katie Snow
 Description : U-Hid Board configuration library, 
               includes U-Hid Nano and BlueHid
 ============================================================================
 */

#ifndef U_HID_H_
#define U_HID_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UHID_VENDOR  0xD209
#define UHID_PRODUCT 0x1200
#define UHID_INTERFACE 0
#define UHID_VALUE 0x0200
#define UHID_MESG_LENGTH 4
#define UHID_SIZE 64

typedef struct json_object json_object;
typedef struct ulboard ulboard;

bool isUHidConfig(json_object *jobj, ulboard* board);
bool validateUHidData(json_object* jobj, ulboard* board);

bool validateSwitch(json_object* jobj, const char* key, bool curResult);
bool validateQuadrature(json_object* jobj, const char* key, bool curResult);
bool validateLED(json_object* jobj, const char* key, bool curResult);

bool updateUHid(json_object* bcfg, ulboard* board);

bool writeUHid(unsigned char* barray, int autoconnect, bool transfer);
#ifdef __cplusplus
}
#endif

#endif /* U_HID_H_ */
