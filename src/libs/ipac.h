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

/* Required items in the json file for IPAC cards (IPAC2, IPAC4, MINI-PAC)*/
#define IPAC_STR_2 "ipac2"
#define IPAC_STR_4 "ipac4"
#define IPAC_STR_M "mini-pac"

/* Required items for writing out through the USB port */
#define IPAC_VENDOR_PRE_2015  0xD208
#define IPAC_VENDOR_2015      0xD209
#define IPAC_PRODUCT_PRE_2015 0x0310
#define IPAC_2_PRODUCT        0x0420
#define IPAC_4_PRODUCT        0x0430
#define IPAC_M_PRODUCT        0x0440
#define IPAC_SIZE_PRE_2015    200
#define IPAC_SIZE_2015        260
#define IPAC_INTERFACE        2

typedef struct json_object json_object;

struct ipac
{
  int version;
  bool minipac;
  bool ipac32;
  bool ipac56;
};

bool isIPACConfig (const char* prodStr, int version, json_object* jobj);
bool validateIPacData(json_object* jobj);

/*
 * Convert the JSON keys data into IPAC data
 * This is done one array element at a time
 * NOTE: The IPAC uses scan codes set 2.
 * http://www.quadibloc.com/comp/scan.htm Has the three sets listed out for reference
 */
char convertIPAC (json_object* jobj);

/*
 * Writes the data out to the board.
 */
bool updateBoardIPAC (json_object *jobj);

bool updatePre2015Board (json_object *jobj);

bool update2015Board (json_object *jobj);

/**
 * Update data array that will be written out to the board.
 */
void populateIPACData(json_object* jobj, unsigned char* data);

#ifdef __cplusplus
}
#endif

#endif /* IPAC_H_ */
