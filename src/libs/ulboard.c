/*
 ============================================================================
 Name        : ulboard.c
 Author      : Katie Snow
 Version     :
 Copyright   : Copyright 2014 Robert Abram, Katie Snow
 Description : Ultimarc board type and version
 ============================================================================
 */
#include <stdlib.h>

#include "ulboard.h"
#include "dbg.h"

#define ULSIZE(a) (sizeof(a)/sizeof(a[0]))
const char* ulBoardTypeName[] =
  { "null", "IPAC2", "IPAC4", "JPAC", "MINIPAC", "ULTIMATE",
    "PACDRIVE", "PACLED64", "ULTRASTIK", "USBBUTTON", "SERVOSTIK",
    "UHID", "UHID NANO"};

const char* ulBoardVersionName[] =
  { "null", "PRE2015", "2015" };

const char*
ulBoardTypeToString (ulboard_type bType)
{
  int _bType = (int) bType;

  if (_bType < 0 || _bType >= (int)ULSIZE(ulBoardTypeName))
  {
    log_err ("Board type %d is out of range [0,%d]", bType,
             (int)ULSIZE(ulBoardTypeName));
    return NULL;
  }

  return ulBoardTypeName[_bType];
}

ulboard_type
ulStringToBoardType (const char* bStr)
{
  int pos = 0;
  for (pos = 0; pos < (int)ULSIZE(ulBoardTypeName); pos++)
  {
    if (!strcasecmp (bStr, ulBoardTypeName[pos]))
    {
      debug ("'%s' = '%s'", bStr, ulBoardTypeName[pos]);
      return (ulboard_type) pos;
    }
  }

  return ulboard_type_null;
}

ulboard_version
ulIntToBoardVersion (int bVersion)
{
  if (bVersion < 0 || bVersion >= (int)ULSIZE(ulBoardVersionName))
  {
    log_err ("Board version %d is out of range [0,%d]", bVersion,
             (int)ULSIZE(ulBoardVersionName));
    return ulboard_version_null;
  }

  return (ulboard_version) bVersion;
}
