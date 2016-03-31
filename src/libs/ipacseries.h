/*
 ============================================================================
 Name        : ipacseries.h
 Author      : Katie Snow
 Version     :
 Copyright   : Copyright 2014 Robert Abram, Katie Snow
 Description : Ultimarc 2015 PAC board configuration library
 ============================================================================
*/

#ifndef PACSERIES_H_
#define PACSERIES_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define IPACSERIES_VERSION     2
#define IPACSERIES_VALUE       0x0203
#define IPACSERIES_MESG_LENGTH 5
#define IPACSERIES_INTERFACE   3
#define IPACSERIES_SIZE        260

typedef struct json_object json_object;

enum ipac_boards_t {NO_IPAC_BOARD = -1, PRE_IPAC2_BOARD = 0, PRE_IPAC4_BOARD,
                    ULTIMATE_IO_BOARD, IPAC2_BOARD, MINIPAC_BOARD, IPAC4_BOARD,
                    JPAC_BOARD, PRE_MINIPAC_BOARD, HIDIO_BOARD};

/**
 * Encapsulating function for the two convert functions.
 */
unsigned char convertIPACKey (enum ipac_boards_t bid, json_object* jobj);

/**
 * 2015 board and the pre-2015 Ultimate I/O board.  Key value to hex value.
 */
unsigned char convertIPACSeries (json_object* jobj);

/*
 * Convert the JSON keys data into IPAC data
 * This is done one array element at a time
 * NOTE: The IPAC uses scan codes set 2.
 * http://www.quadibloc.com/comp/scan.htm Has the three sets listed out for reference
 */
unsigned char convertIPAC (json_object* jobj);

/**
 * Update one of the PAC Series boards.  These boards are the 2015 boards from Ultimarc.
 */
bool updatePACSeriesBoard(json_object* jobj);

void populateBoardArray (enum ipac_boards_t bid, json_object* jobj, unsigned char* barray);

/**
 * Lookup the Lookup Table Key based on the configuration name
 */
int decipherLookupKey (const char* key);

/**
 * Lookup the Macro Table key
 */
int decipherLookupMacroKey (const char* key);

void populateShiftPosition (enum ipac_boards_t bid, json_object* key, unsigned char* barray);

/**
 * Update the macros data in the array
 */
void populateMacrosPosition (enum ipac_boards_t bid, json_object* macros, unsigned char* barray);

/**
 * IPAC Series generic USB write function
 */
bool writeIPACSeriesUSB (unsigned char* barray, int size, uint16_t vendor, uint16_t product, int interface, int autoconnect, bool transfer);

#ifdef __cplusplus
}
#endif

#endif /* PACSERIES_H_ */
