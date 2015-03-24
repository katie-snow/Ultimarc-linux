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

extern void loadUltimarcConfigurations(int argc, char **argv);

/*
 * updateUltimarcBoard
 * Updates a board.  The board is selected based on the json file contents
 * Paramter: json object
 * Returns: true if successfully updated a board, false otherwise
 */
bool updateUltimarcBoard(json_object* jobj);

#ifdef __cplusplus
}
#endif

#endif  // Ultimarc h
