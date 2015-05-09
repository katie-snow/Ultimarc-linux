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

/* Required items in the json file for IPAC2 and MINIPAC cards */
#define IPAC_STR_2 "ipac2"
#define IPAC_STR_M "minipac"

/* Required items for writing out through the USB port */
#define IPAC_VENDOR_PRE_2015  0xD208
#define IPAC_VENDOR_2015      0xD209
#define IPAC_PRODUCT_PRE_2015 0x0310
#define IPAC_2_PRODUCT        0x0420
#define IPAC_M_PRODUCT        0x0440
#define IPAC_SIZE_PRE_2015    100
#define IPAC_INTERFACE        2

typedef struct json_object json_object;

struct ipac
{
  int version;
  bool minipac;
  bool ipac32;
};

bool isIPACConfig (const char* prodStr, int version, json_object* jobj);
bool validateIPacData(json_object* jobj);

/*
 * Writes the data out to the board.
 */
bool updateBoardIPAC (json_object *jobj);

bool updatePre2015Board (json_object *jobj);

void update2015IPACBoard (json_object *jobj, unsigned char* barray);

void update2015MinIPACBoard (json_object *jobj, unsigned char* barray);

#ifdef __cplusplus
}
#endif

#endif /* IPAC_H_ */
