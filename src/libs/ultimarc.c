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
#include "ipac.h"
#include "pacLED.h"
#include "ultistik.h"
#include "pacdrive.h"
#include "ipacultimate.h"
#include "dbg.h"

int
ulValidateConfig (json_object* bcfg, ulobject* ulobj)
{
  int retCode = 0;

  if (bcfg)
  {
    retCode = ulGetProdAndVersion (bcfg, ulobj);

    if (retCode == 0)
    {
      if ((ulobj->ipac = isIPACConfig (ulobj->pStr, ulobj->version, bcfg))
          || (ulobj->ultimate = isIPACUltimateConfig (ulobj->pStr,
                                                      ulobj->version, bcfg))
          || (ulobj->pacDrive = isPACDriveConfig (ulobj->pStr, ulobj->version,
                                                  bcfg)) || (ulobj->pacLED =
              isPACLED64Config (ulobj->pStr, ulobj->version, bcfg))
          || (ulobj->ultistik = isUltistikConfig (ulobj->pStr, ulobj->version,
                                                  bcfg)))
      {
        log_info("Configuration is %s. [Validated]", ulobj->pStr);
      }
      else
      {
        log_err("Configuration is '%s'. [Not validated].", ulobj->pStr);
      }
    }
    else
    {
      log_err("Configuration is '%s'. [Not validated].", ulobj->pStr);
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
ulValidateConfigFileStr (const char* file, ulobject* ulobj)
{
  json_object *bcfg = json_object_from_file (file);

  return ulValidateConfig (bcfg, ulobj);
}

int
ulWriteToBoard (json_object* bcfg, ulobject* ulobj)
{
  int retCode = 0;

  if (bcfg && ulobj)
  {
    if (ulobj->ipac)
    {
      retCode = updateBoardIPAC (bcfg);
    }
    else if (ulobj->ultimate)
    {
      log_info("Updating IPAC Ultimate board...");
      retCode = updateBoardIPacUltimate (bcfg);
    }
    else if (ulobj->pacDrive)
    {
      log_info("Updating PAC Drive board...");
      retCode = updateBoardPacDrive (bcfg);
    }
    else if (ulobj->pacLED)
    {
      log_info("Updating PAC LED 64 board...");
      retCode = updateBoardPacLED (bcfg);
    }
    else if (ulobj->ultistik)
    {
      log_info("Updating Ultistik board...");
      retCode = updateBoardULTISTIK (bcfg);
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
ulWriteToBoardFileStr (const char* file, ulobject* ulobj)
{
  json_object *bcfg = json_object_from_file (file);

  return ulWriteToBoard (bcfg, ulobj);
}

int
ulGetProdAndVersion (json_object* jobj, ulobject* ulobj)
{
  int retCode = 0;

  json_object* prodobj = NULL;
  json_object* verobj = NULL;

  if (jobj && ulobj)
  {
    if (json_object_object_get_ex (jobj, "product", &prodobj))
    {
      if (json_object_get_type (prodobj) == json_type_string)
      {
        ulobj->pStr = json_object_get_string (prodobj);
      }
      else
      {
        log_err("'product' is not defined as a string");
        ulobj->pStr = "";
        retCode = 1;
      }
    }
    else
    {
      log_err("'product' is not defined in the configuration file");
      ulobj->pStr = "";
      retCode = 1;
    }

    if (json_object_object_get_ex (jobj, "version", &verobj))
    {
      if (json_object_get_type (verobj) == json_type_int)
      {
        ulobj->version = json_object_get_int (verobj);
      }
      else
      {
        log_err("'version' is not defined as a integer");
        ulobj->version = -1;
        retCode = 1;
      }
    }
    else
    {
      log_err("'version' is not defined in the configuration file");
      ulobj->version = -1;
      retCode = 1;
    }
  }
  else
  {
    log_err("JSON_Object is null");
  }

  return retCode;
}
