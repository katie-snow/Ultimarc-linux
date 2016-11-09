/*
 ============================================================================
 Name        : ulboard.h
 Author      : Katie Snow
 Version     :
 Copyright   : Copyright 2014 Robert Abram, Katie Snow
 Description : Ultimarc board types and structures
 ============================================================================
 */

#ifndef ULTIMARCBOARD_H_
#define ULTIMARCBOARD_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* All the supported board types */
typedef enum ulboard_type
{
  ulboard_type_null,
  ulboard_type_ipac2,
  ulboard_type_ipac4,
  ulboard_type_jpac,
  ulboard_type_minipac,
  ulboard_type_ultimate,
  ulboard_type_pacDrive,
  ulboard_type_pacLED,
  ulboard_type_ultistik,
  ulboard_type_usbbutton,
  ulboard_type_servostik
} ulboard_type;

/* Supported versions */
typedef enum ulboard_version
{
  ulboard_version_null,
  ulboard_version_pre2015,
  ulboard_version_2015
} ulboard_version;

/* Data structure */
typedef struct ulboard
{
  ulboard_type type;
  ulboard_version version;
} ulboard;

extern const char* ulBoardTypeToString (ulboard_type bType);

extern ulboard_type ulStringToBoardType (const char* bStr);

extern ulboard_version ulIntToBoardVersion (int bVersion);

#ifdef __cplusplus
}
#endif


#endif /* ULTIMARCBOARD_H_ */
