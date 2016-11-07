 /*
 ============================================================================
 Name        : servostik.h
 Author      : Katie Snow
 Version     :
 Copyright   : Copyright 2015 Robert Abram, Katie Snow
 Description : ServoStik configuration library
 ============================================================================
 */

#ifndef SERVOSTIK_H_
#define SERVOSTIK_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SERVOSTIK_VENDOR  0xD209
#define SERVOSTIK_PRODUCT 0x1700
#define SERVOSTIK_INTERFACE 0
#define SERVOSTIK_VALUE 0x0200
#define SERVOSTIK_MESG_LENGTH 4
#define SERVOSTIK_SIZE 4

typedef struct json_object json_object;
typedef struct ulboard ulboard;

bool isServoStikConfig(json_object *jobj, ulboard* board);
bool validateServoStikData(json_object* jobj, ulboard* board);

bool updateServoStik(json_object* bcfg, ulboard* board);

bool writeServoStik(unsigned char* barray, int autoconnect, bool transfer);

#ifdef __cplusplus
}
#endif

#endif /* SERVOSTIK_H_ */
