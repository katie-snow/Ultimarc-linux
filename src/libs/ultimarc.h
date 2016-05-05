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

typedef struct ulobject
{
  const char* pStr;
  int version;
  bool ipac;
  bool ultimate;
  bool pacDrive;
  bool pacLED;
  bool ultistik;
} ulobject;

/**
 * ulValidateConfig
 * Validates the configuration
 */
extern int
ulValidateConfig (json_object* bcfg, ulobject* ulobj);
extern int
ulValidateConfigFileStr (const char* file, ulobject* ulobj);

/**
 * ulWriteToBoard
 * Writes the configuration provided out to the Ultimarc hardware
 */
extern int
ulWriteToBoard (json_object* bcfg, ulobject* ulobj);
extern int
ulWriteToBoardFileStr (const char* file, ulobject* ulobj);

/**
 * Populates the ulobj with the product and version in the json_object
 */
int
ulGetProdAndVersion (json_object* jobj, ulobject* ulobj);

#ifdef __cplusplus
}
#endif

#endif  // Ultimarc h
