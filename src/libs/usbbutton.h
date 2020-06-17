/*
 ============================================================================
 Name        : usbbutton.h
 Author      : Katie Snow
 Version     :
 Copyright   : Copyright 2015 Robert Abram, Katie Snow
 Description : USB Button configuration library
 ============================================================================
 */

#ifndef USBBUTTON_H_
#define USBBUTTON_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define USBBTN_VENDOR  0xD209
#define USBBTN_PRODUCT 0x1200
#define USBBTN_INTERFACE 0
#define USBBTN_VALUE 0x0200
#define USBBTN_MESG_LENGTH 4
#define USBBTN_SIZE 64

typedef struct json_object json_object;
typedef struct ulboard ulboard;

bool isUSBButtonConfig(json_object *jobj, ulboard* board);
bool validateUSBButtonColor(json_object* jobj, bool curResult);
bool validateUSBButtonData(json_object* jobj, ulboard* board);
bool validateUSBButtonRowData(json_object* entries, const char* rowStr, bool curResult);

bool updateBoardUSBButton(json_object* bcfg, ulboard* board);
bool updateBoardUSBButtonColor(json_object* bcfg, ulboard* board);
bool updateBoardUSBButtonData(json_object* bcfg, ulboard* board);
void populateUSBKeys(json_object* keys, int row, unsigned char* barray);

bool writeUSBButton(unsigned char* barray, int autoconnect, bool transfer, unsigned int size);
#ifdef __cplusplus
}
#endif

#endif /* USBBUTTON_H_ */
