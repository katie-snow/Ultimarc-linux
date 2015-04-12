/*
 ============================================================================
 Name        : ipac4.h
 Author      : Katie Snow
 Version     :
 Copyright   : Copyright 2014 Robert Abram, Katie Snow
 Description : Ultimarc IPAC4 board configuration library
 ============================================================================
*/

#ifndef IPAC4_H_
#define IPAC4_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define IPAC4_STR "ipac4"

#define IPAC4_VENDOR_PRE_2015  0xD208
#define IPAC4_VENDOR_2015      0xD209
#define IPAC4_PRODUCT_PRE_2015 0x0310
#define IPAC4_PRODUCT          0x0430
#define IPAC4_SIZE_PRE_2015    200
#define IPAC4_SIZE_2015        260
#define IPAC4_INTERFACE        2

typedef struct json_object json_object;

struct ipac4
{
  int version;
};

/* Verify the configuration is a IPAC4 */
bool isIPAC4Config (const char* prodStr, int version, json_object* jobj);
bool validateIPAC4Data(json_object* jobj);

/*
 * Convert the JSON keys data into IPAC data
 * This is done one array element at a time
 * NOTE: The IPAC uses scan codes set 2.
 * http://www.quadibloc.com/comp/scan.htm Has the three sets listed out for reference
 */
char convertIPAC4 (json_object* jobj);

/*
 * Writes the data out to the board.
 */
bool updateBoardIPAC4 (json_object *jobj);

bool updatePre2015BoardIPAC4 (json_object *jobj);

bool update2015BoardIPAC4 (json_object *jobj);

/**
 * Update data array that will be written out to the board.
 */
void populateIPAC4Data(json_object* jobj, unsigned char* data);

#ifdef __cplusplus
}
#endif

#endif /* IPAC4_H_ */
