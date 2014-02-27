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
#define IPAC_PRODUCT 0x0310
#define IPAC_DEVICE "0310"
#define IPAC_DATA_SIZE 204
#define IPAC_REPORT 0x03

typedef struct json_object json_object;

/*
 * Determine if the json file is an IPAC configuration
 */
bool isIPAC (json_object* jobj);

/*
 * Convert the JSON keys data into IPAC data
 * This is done one array element at a time
 */
char convert (json_object* jobj);

extern bool updateBoard (json_object *jobj);

#ifdef __cplusplus
}
#endif

#endif /* IPAC_H_ */
