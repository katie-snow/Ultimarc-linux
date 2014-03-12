/*
 * ipac.h
 *
 *  Created on: Feb 18, 2014
 *      Author: Katie Snow
 */

#ifndef IPAC_H_
#define IPAC_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Required items in the json file for IPAC cards (IPAC32, IPAC64, IPAC-MINI)*/
#define IPAC_VERSION 1
#define IPAC_PRODUCT_STR "0310"

/* Required items for writing out through the USB port */
#define IPAC_VENDOR       0xD208
#define IPAC_PRODUCT      0x0310
#define IPAC_DATA_SIZE    204
#define IPAC_REPORT       0x03
#define IPAC_REQUEST_TYPE 0x21
#define IPAC_REQUEST      9
#define IPAC_VALUE        0x0203
#define IPAC_INDEX        2
#define IPAC_MESG_LENGTH  5
#define IPAC_TIMEOUT      2000
#define IPAC_INTERFACE    2

typedef struct json_object json_object;

/*
 * Determine if the json file is an IPAC configuration
 */
bool isIPAC (json_object* jobj);

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

#ifdef __cplusplus
}
#endif

#endif /* IPAC_H_ */
