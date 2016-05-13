/*
 ============================================================================
 Name        : ipac.h
 Author      : Robert Abram, Katie Snow
 Version     :
 Copyright   : Copyright 2015 Robert Abram, Katie Snow
 Description : Ultimarc IPAC2, IPAC4 and Mini-PAC configuration library
 ============================================================================
 */

#ifndef IPAC_H_
#define IPAC_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Required items for writing out through the USB port */
#define IPAC_VENDOR_PRE_2015  0xD208
#define IPAC_VENDOR_2015      0xD209
#define IPAC_PRODUCT_PRE_2015 0x0310
#define IPAC_2_PRODUCT        0x0420
#define IPAC_4_PRODUCT        0x0430
#define IPAC_M_PRODUCT        0x0440
#define JPAC_PRODUCT          0x0450
#define IPAC_SIZE_PRE_2015    100
#define IPAC_INTERFACE        2

typedef struct json_object json_object;
typedef struct ulboard ulboard;

bool isIPACConfig (json_object* jobj, ulboard* board);
bool validateIPACData(json_object* jobj, int size, ulboard* board);
bool validateIPAC4Data(json_object* jobj, ulboard* board);

/*
 * macros are optional.
 * Validation will be the following:
 *  4 macro entries (pre2015 board)
 *  each macro entry is an array (all boards)
 *  4 key entries per macro array (pre2015 board)
 *  each key entry is type string (all boards)
 *  limit of 85 bytes for the complete macro group (2015 boards)
 *
 */
bool validateIPACMacros(json_object* jobj, bool valid, ulboard* board);

/*
 * Writes the data out to the board.
 */
bool updateBoardIPAC (json_object *jobj, ulboard* board);

void updatePre2015IPAC2Board (json_object *jobj, unsigned char* barray);

void updatePre2015IPAC4Board (json_object *jobj, unsigned char* barray);

void updatePre2015MINIPACBoard (json_object *jobj, unsigned char* barray);

void update2015IPAC2Board (json_object *jobj, unsigned char* barray);

void update2015MinIPACBoard (json_object *jobj, unsigned char* barray);

void update2015IPAC4Board (json_object *jobj, unsigned char* barray);

void update2015JPACBoard (json_object *jobj, unsigned char* barray);

#ifdef __cplusplus
}
#endif

#endif /* IPAC_H_ */
