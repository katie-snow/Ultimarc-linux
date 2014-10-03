/*
 ============================================================================
 Name        : ultimarc.h
 Author      : Robert Abram, Katie Snow
 Version     :
 Copyright   : Copyright 2014 Robert Abram, Katie Snow
 Description : Ultimarc main configuration library
 ============================================================================
*/

#ifndef __ULTIMARC_H_
#define __ULTIMARC_H_


/* Unix */
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct json_object json_object;

typedef enum ultimarc_type {
  ultimarc_none,
  ultimarc_ipac,
  ultimarc_ultistik,
  ultimarc_pacled64,
  ultimarc_pacdrive,
} ultimarc_type;

extern void loadUltimarcConfigurations(int argc, char **argv);

/*
 * determine_device
 * Parameter: json object
 * Returns: true successfully parsed the data, false otherwise
 */
enum ultimarc_type determine_device (json_object* jobj);

/*
 * updateUltimarcBoard
 * Updates a board.  The board is selected based on the json file contents
 * Paramter: json object
 * Returns: true if successfully updated a board, false otherwise
 */
bool updateUltimarcBoard(json_object* jobj);

enum ultimarc_type validateProduct(json_object* jobj);
bool validateVersion(json_object* jobj, enum ultimarc_type device);
bool validateData(json_object*jobj, enum ultimarc_type device);

 #ifdef __cplusplus
}
#endif

#endif  // Ultimarc h
