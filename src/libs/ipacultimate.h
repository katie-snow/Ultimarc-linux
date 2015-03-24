/*
 ============================================================================
 Name        : ipacultimate.h
 Author      : Robert Abram, Katie Snow
 Version     :
 Copyright   : Copyright 2014 Robert Abram, Katie Snow
 Description : Ultimarc IPac Ultimate configuration library
 ============================================================================
 */

#ifndef IPACULTIMATE_H_
#define IPACULTIMATE_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define IPACULTIMATE_VERSION_MIN 1
#define IPACULTIMATE_VERSION_MAX 2
#define IPACULTIMATE_PRODUCT_STR "0410"
#define IPACULTIMATE_STR "ULTIMATE"

/* Required items for writing out through the USB port */
#define IPACULTIMATE_VENDOR         0xD209
#define IPACULTIMATE_PRODUCT        0x0410
#define IPACULTIMATE_MESG_LENGTH    5
#define IPACULTIMATE_REPORT         0x03
#define IPACULTIMATE_REQUEST_TYPE   0x21
#define IPACULTIMATE_REQUEST        9
#define IPACULTIMATE_VALUE          0x0203
#define IPACULTIMATE_TIMEOUT        2000
#define IPACULTIMATE_INTERFACE      3
#define IPACULTIMATE_DATA_SIZE      260

typedef struct json_object json_object;

struct ipacultimate
{
  bool ledMapIntensity;
  bool allIntensities;
  bool random;
  bool boardIDUpdate;
  bool fadeRate;
  bool pins;
};

/*
 * Determine if the json file is an IPac Ultimate configuration
 */
bool isIPACUltimateConfig(const char* prodStr, int version, json_object* jobj);
const char* getIPacUltimateProductStr ();
int getIPacUltimateVersion();
bool validateIPacUltimateData(json_object* jobj);

/** populates the data array from the json configuration file */
bool populateIPACUltimateData(json_object* jobj, unsigned char* data);

/*
 * Convert the JSON keys data into IPAC Ultimate data
 * This is done one array element at a time
 */
unsigned char convertIPACUltimate (json_object* jobj);

/*
 * Assigns the quadrature pins to the correct corresponding value
 * Only certain pins need this function
 */
bool quadAssignmentIPACUltimate (unsigned char* data, unsigned char keyval,
                                 int idx1, int idx2, int idx3, int idx4);

bool updateBoardIPacUltimate (json_object* jobj);

#ifdef __cplusplus
}
#endif

#endif /* IPACULTIMATE_H_ */
