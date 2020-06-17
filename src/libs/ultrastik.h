/*
 ============================================================================
 Name        : ultrastik.h
 Author      : Robert Abram, Katie Snow
 Version     :
 Copyright   : Copyright 2014 Robert Abram, Katie Snow
 Description : Ultimarc UltraStik configuration library
 ============================================================================
 */

#ifndef ULTRASTIK_H_
#define ULTRASTIK_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Required items for writing out through the USB port */
#define USTIK_VENDOR             0xD209
#define USTIK_PRODUCT_PRE_2015   0x0501
#define USTIK_PRODUCT            0x0511
#define USTIK_DATA_SIZE          96
#define USTIK_CONFIG_DATA_SIZE   32
#define USTIK_REQUEST_TYPE_1     0x43
#define USTIK_REQUEST_TYPE_2     0xC3
#define USTIK_REQUEST_1          0xE9
#define USTIK_REQUEST_2          0xEB
#define USTIK_REQUEST_3          0xEA
#define USTIK_VALUE              0x0200
#define USTIK_MESG_PRE_2015      32
#define USTIK_MESG_LENGTH        4
#define USTIK_INTERFACE_PRE_2015 0
#define USTIK_INTERFACE          2
#define USTIK_CONFIG_BASE        0x51

typedef struct json_object json_object;
typedef struct ulboard ulboard;

bool isUltraStikConfig (json_object* jobj, ulboard* board);

bool validateUltraStikData(json_object* jobj, ulboard* board);

/*
 * Convert the JSON keys data into UltraStik data
 * This is done one array element at a time
 */
char convertUltraStik (json_object* jobj);

bool updateBoardUltraStik (json_object* jobj, ulboard* board);

#ifdef __cplusplus
}
#endif

#endif /* ULTRASTIK_H_ */
