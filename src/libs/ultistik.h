/*
 ============================================================================
 Name        : ultistik.h
 Author      : Robert Abram, Katie Snow
 Version     :
 Copyright   : Copyright 2014 Robert Abram, Katie Snow
 Description : Ultimarc Ultistik configuration library
 ============================================================================
 */

#ifndef ULTISTIK_H_
#define ULTISTIK_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Required items in the json file for Ultistik card*/
#define USTIK_VERSION 1
#define USTIK_CONFIG_VERSION 2
#define USTIK_PRODUCT_STR "0501"
#define USTIK_STR "ULTISTIK"

/* Required items for writing out through the USB port */
#define USTIK_VENDOR       0xD209
#define USTIK_PRODUCT      0x0501
#define USTIK_DATA_SIZE    96
#define USTIK_CONFIG_DATA_SIZE 32
#define USTIK_REQUEST_TYPE_1 0x43
#define USTIK_REQUEST_TYPE_2 0xC3
#define USTIK_REQUEST_1    0xE9
#define USTIK_REQUEST_2    0xEB
#define USTIK_REQUEST_3    0xEA
#define USTIK_MESG_LENGTH  32
#define USTIK_INTERFACE    0
#define USTIK_CONFIG_BASE  0x51

typedef struct json_object json_object;

struct Ultistik
{
  bool controllerIDUpdate;
};

bool isUltistikConfig (const char* prodStr, int version, json_object* jobj);

/*
 * Determine if the json file is an ULTISTIK configuration
 */
const char* getUltistikProductStr ();
int getUltistikVersion();
bool validateUltistikData(json_object* jobj);

/*
 * Convert the JSON keys data into IPAC data
 * This is done one array element at a time
 */
char convertULTISTIK (json_object* jobj);

bool updateBoardULTISTIK (json_object* jobj);

#ifdef __cplusplus
}
#endif

#endif /* ULTISTIK_H_ */
