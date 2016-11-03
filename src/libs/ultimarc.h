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
extern "C"
{
#endif

typedef struct json_object json_object;

typedef struct ulboard ulboard;

/**
 * ulValidateConfig
 * Validates the configuration.
 * Populates ulboard structure
 */
extern int
ulValidateConfig (json_object* bcfg, ulboard* ulobj);
extern int
ulValidateConfigFileStr (const char* file, ulboard* ulobj);

/**
 * ulWriteToBoard
 * Writes the configuration provided out to the Ultimarc hardware
 */
extern int
ulWriteToBoard (json_object* bcfg, ulboard* ulobj);
extern int
ulWriteToBoardFileStr (const char* file, ulboard* ulobj);

/**
 * ulMultiConfigurationsFileStr
 * Loads multiple configuration.
 * Performs both validate and write functions.
 */
void
ulMultiConfigurationsFileStr (const char* file);

/**
 * Populates the ulobj with the product and version in the json_object
 */
int
ulGetProdAndVersion (json_object* jobj, ulboard* ulobj);

#ifdef __cplusplus
}
#endif

#endif  // Ultimarc h
