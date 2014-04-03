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
  ultimarc_multiple,
  ultimarc_ipac,
  ultimarc_ultistik,
  ultimarc_ultistik_config,
  ultimarc_pacled64,
  ultimarc_pacled64_config
} ultimarc_type;

extern void parseCmdLineArgs(int argc, char **argv);

/*
 * determine_device
 * Parameter: path to a json file
 * Returns: true successfully parsed the data, false otherwise
 */
enum ultimarc_type determine_device (json_object* jobj);

/*
 * updateBoard
 * Updates a board.  The board is selected based on the json file contents
 * Paramter: path to json file
 * Returns: true if successfully updated a board, false otherwise
 */
bool updateBoard(const char* file);


bool isMultiple(json_object* jobj);

 #ifdef __cplusplus
}
#endif

#endif  // Ultimarc h
