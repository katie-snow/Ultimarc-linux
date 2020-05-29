/*
 ============================================================================
 Name        : uhid.h
 Author      : Katie Snow
 Version     :
 Copyright   : Copyright 2015 Robert Abram, Katie Snow
 Description : U-Hid Board configuration library, 
               includes U-Hid Nano
 ============================================================================
 */

#ifndef U_HID_H_
#define U_HID_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UHID_VENDOR  0xD209
#define UHID_PRODUCT 0x1501
#define UHID_INTERFACE 0
#define UHID_VALUE 0x0200
#define UHID_MESG_LENGTH 4
#define UHID_SIZE 260

typedef struct json_object json_object;
typedef struct ulboard ulboard;

enum uhid_boards_t {NO_UHID_BOARD = -1, UHID, NANO};

bool isUHidConfig(json_object *jobj, ulboard* board);
bool validateUHidData(json_object* jobj, ulboard* board, int pCount);

bool validateSwitch(json_object* jobj, const char* key, bool curResult);
bool validateQuadrature(json_object* jobj, const char* key, bool curResult);
bool validateLED(json_object* jobj, const char* key, bool curResult, json_object* pins);
bool validateUHidMacros(json_object* jobj, bool curResult);
bool validateUHidCalibration(json_object* jobj, bool curResult);
bool validateUHidQuadratureButtonTime(json_object* jobj, bool curResult);

bool updateBoardUHid(json_object* bcfg, ulboard* board);

void populateUHidBoardArray(enum uhid_boards_t bid, json_object* jobj, unsigned char* barray);
void populateUHidMacro(json_object* jobj, unsigned char* barray);
void populateUHidCalibration(json_object* jobj, unsigned char* barray);

int decipherUHidLookupKey (const char* key);
unsigned char switchActionUHid (json_object* action, bool isUp);
void quadratureUHid (unsigned char* barray, int idx, int idx2, char val1, char val2, bool isReverse);
unsigned char analogAxisUHid(json_object* jobj);

bool writeUHid(unsigned char* barray, int autoconnect, bool transfer);
#ifdef __cplusplus
}
#endif

#endif /* U_HID_H_ */
