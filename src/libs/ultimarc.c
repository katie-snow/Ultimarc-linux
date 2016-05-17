/*
 ============================================================================
 Name        : ultimarc.c
 Author      : Robert Abram, Katie Snow
 Version     :
 Copyright   : Copyright 2014 Robert Abram, Katie Snow
 Description : Ultimarc main configuration library
 ============================================================================
 */

/* Unix */
#include <linux/usbdevice_fs.h>
#include <json.h>

/* C */
#include <stdio.h>
#include <string.h>

#include "ultimarc.h"
#include "ulboard.h"
#include "ipac.h"
#include "pacLED.h"
#include "ultistik.h"
#include "pacdrive.h"
#include "ipacultimate.h"
#include "dbg.h"

int
ulValidateConfig (json_object* bcfg, ulboard* ulcfg)
{
  int retCode = 0;

  if (bcfg && ulcfg)
  {
    retCode = ulGetProdAndVersion (bcfg, ulcfg);

    if (retCode == 0)
    {
      if (isIPACConfig (bcfg, ulcfg)
          || isIPACUltimateConfig (ulBoardTypeToString(ulcfg->type), ulcfg->version, bcfg)
          || isPACDriveConfig (ulBoardTypeToString(ulcfg->type), ulcfg->version, bcfg)
          || isPACLED64Config (bcfg, ulcfg)
          || isUltistikConfig (bcfg, ulcfg))
      {
        log_info("Configuration is %s. [Validated]", ulBoardTypeToString(ulcfg->type));
      }
      else
      {
        retCode = -1;
        log_err("Configuration is '%s'. [Not validated].", ulBoardTypeToString(ulcfg->type));
      }
    }
    else
    {
      retCode = -1;
      log_err("Configuration is '%s'. [Not validated].", ulBoardTypeToString(ulcfg->type));
    }
  }
  else
  {
    retCode = -1;
    log_err("JSON format invalid");
  }

  return retCode;
}

int
ulValidateConfigFileStr (const char* file, ulboard* board)
{
  json_object *bcfg = json_object_from_file (file);

  return ulValidateConfig (bcfg, board);
}

int
ulWriteToBoard (json_object* bcfg, ulboard* board)
{
  int retCode = 0;

  if (bcfg && board)
  {
    if (board->type == ulboard_type_ipac2)
    {
      retCode = updateBoardIPAC (bcfg, board);
    }
    else if (board->type == ulboard_type_ultimate)
    {
      log_info("Updating IPAC Ultimate board...");
      retCode = updateBoardIPacUltimate (bcfg);
    }
    else if (board->type == ulboard_type_pacDrive)
    {
      log_info("Updating PAC Drive board...");
      retCode = updateBoardPacDrive (bcfg);
    }
    else if (board->type == ulboard_type_pacLED)
    {
      log_info("Updating PAC LED 64 board...");
      retCode = updateBoardPacLED (bcfg);
    }
    else if (board->type == ulboard_type_ultistik)
    {
      log_info("Updating Ultistik board...");
      retCode = updateBoardULTISTIK (bcfg, board);
    }

    if (retCode)
    {
      log_info("Board update successful.");
    }
    else
    {
      log_info("Board update failed.");
    }
  }
  else
  {
    retCode = -1;
    log_info("Board update failed.");
  }

  return retCode;
}

int
ulWriteToBoardFileStr (const char* file, ulboard* board)
{
  json_object *bcfg = json_object_from_file (file);

  return ulWriteToBoard (bcfg, board);
}

int
ulGetProdAndVersion (json_object* jobj, ulboard* ulcfg)
{
  int retCode = 0;
  int version = 0;

  json_object* prodobj = NULL;
  json_object* verobj = NULL;

  if (jobj && ulcfg)
  {
    if (json_object_object_get_ex (jobj, "product", &prodobj))
    {
      if (json_object_get_type (prodobj) == json_type_string)
      {
        ulcfg->type = ulStringToBoardType(json_object_get_string (prodobj));
      }
      else
      {
        log_err("'product' is not defined as a string");
        ulcfg->type = ulboard_type_null;
        retCode = 1;
      }
    }
    else
    {
      log_err("'product' is not defined in the configuration file");
      ulcfg->type = ulboard_type_null;
      retCode = 1;
    }

    if (json_object_object_get_ex (jobj, "version", &verobj))
    {
      if (json_object_get_type (verobj) == json_type_int)
      {
        ulcfg->version = ulIntToBoardVersion(json_object_get_int (verobj));
      }
      else
      {
        log_err("'version' is not defined as a integer");
        ulcfg->version = ulboard_version_null;
        retCode = 1;
      }
    }
    else
    {
      log_err("'version' is not defined in the configuration file");
      ulcfg->version = ulboard_version_null;
      retCode = 1;
    }
  }
  else
  {
    log_err("JSON_Object is null");
  }

  return retCode;
}
