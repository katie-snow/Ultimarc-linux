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
#ifdef __linux__
  #include <linux/usbdevice_fs.h>
#endif
#include <json.h>

/* C */
#include <stdio.h>
#include <string.h>

#include "ultimarc.h"
#include "ulboard.h"
#include "ipac.h"
#include "pacLED.h"
#include "ultrastik.h"
#include "pacdrive.h"
#include "ipacultimate.h"
#include "usbbutton.h"
#include "servostik.h"
#include "uhid.h"
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
          || isIPACUltimateConfig (bcfg, ulcfg)
          || isPACDriveConfig (bcfg, ulcfg)
          || isPACLED64Config (bcfg, ulcfg)
          || isUltraStikConfig (bcfg, ulcfg)
          || isUSBButtonConfig(bcfg, ulcfg)
          || isServoStikConfig(bcfg, ulcfg)
          || isUHidConfig (bcfg, ulcfg))
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
  bool ret = false;

  if (bcfg && board)
  {
    if (board->type == ulboard_type_ipac2 ||
        board->type == ulboard_type_ipac4 ||
        board->type == ulboard_type_jpac ||
        board->type == ulboard_type_minipac)
    {
      ret = updateBoardIPAC (bcfg, board);
    }
    else if (board->type == ulboard_type_ultimate)
    {
      log_info("Updating IPAC Ultimate board...");
      ret = updateBoardIPacUltimate (bcfg);
    }
    else if (board->type == ulboard_type_pacDrive)
    {
      log_info("Updating PAC Drive board...");
      ret = updateBoardPacDrive (bcfg);
    }
    else if (board->type == ulboard_type_pacLED)
    {
      log_info("Updating PAC LED 64 board...");
      ret = updateBoardPacLED (bcfg);
    }
    else if (board->type == ulboard_type_ultrastik)
    {
      log_info("Updating UltraStik board...");
      ret = updateBoardUltraStik (bcfg, board);
    }
    else if (board->type == ulboard_type_usbbutton)
    {
      log_info("Updating USBButton board...");
      ret = updateBoardUSBButton (bcfg, board);
    }
    else if (board->type == ulboard_type_servostik)
    {
      log_info("Updating ServoStik board...");
      ret = updateBoardServoStik (bcfg, board);
    }
    else if (board->type == ulboard_type_uhid ||
             board->type == ulboard_type_uhidNano)
    {
      log_info("Updating UHid board...");
      ret = updateBoardUHid (bcfg, board);
    }

    if (ret)
    {
      log_info("Board update successful.");
    }
    else
    {
      log_info("Board update failed.");
      retCode = 1;
    }
  }
  else
  {
    retCode = 1;
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

void
ulMultiConfigurationsFileStr (const char* file)
{
  ulboard board;

  json_object *bcfg = NULL;
  json_object *jobj = NULL;
  json_object *mcfg = json_object_from_file (file);

  int valid = true;
  int idx = 0;

  const char* fileStr = NULL;

  if (mcfg)
  {
    if (json_object_object_get_ex(mcfg, "list", &bcfg))
    {
      if (!json_object_is_type(bcfg, json_type_array))
      {
        log_err ("'list' needs to be of type array");
        valid = false;
      }
      else
      {
        for (idx = 0; idx < json_object_array_length(bcfg); ++ idx)
        {
          log_info ("-------");
          jobj = json_object_array_get_idx(bcfg, idx);
          fileStr = json_object_get_string (jobj);
          jobj = json_object_from_file (fileStr);

          if (jobj)
          {
            log_info ("Loading %s...", fileStr);
            if (ulValidateConfig(jobj, &board) == 0)
            {
              ulWriteToBoard(jobj, &board);
            }
          }
        }
      }
    }
    else
    {
      log_err ("'list' is not defined in the configuration");
      valid = false;
    }

    if (!valid)
    {
      log_err ("Configuration. [Not validated]");
    }
  }
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
