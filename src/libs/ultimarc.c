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


void loadUltimarcConfigurations (int argc, char **argv)
{
  int idx;
  int inner_idx;

  const char* fileStr = NULL;

  json_object *jobj = NULL;
  json_object *innerobj = NULL;
  json_object *item = NULL;

  for (idx = 1; idx < argc; ++idx)
  {
    jobj = json_object_from_file (argv[idx]);
    if (jobj)
    {
      log_info ("Loading %s...", argv[idx]);

      if (json_object_object_get_ex(jobj, "configurations", &innerobj))
      {
        printf ("\n");
        for (inner_idx = 0; inner_idx < json_object_array_length(innerobj); ++ inner_idx)
        {
          item = json_object_array_get_idx(innerobj, inner_idx);
          fileStr = json_object_get_string(item);
          item = json_object_from_file (fileStr);

          if (item)
          {
            log_info ("Loading %s...", fileStr);
            updateUltimarcBoard(item);
          }
          else
          {
            log_err ("%s. Format invalid\n", fileStr);
          }
        }
      }
      else
      {
        updateUltimarcBoard(jobj);
      }
    }
    else
    {
      log_err ("%s. Format invalid\n", argv[idx]);
    }
  }
}

bool updateUltimarcBoard (json_object* jobj)
{
  bool ret = false;

  const char* prodstr = NULL;
  int version = 0;

  json_object* prodobj = NULL;
  json_object* verobj = NULL;

  if (json_object_object_get_ex(jobj, "product", &prodobj) &&
      json_object_get_type(prodobj) == json_type_string)
  {
    if (json_object_object_get_ex(jobj, "version", &verobj) &&
        json_object_get_type(verobj) == json_type_int)
    {
      prodstr = json_object_get_string(prodobj);
      version = json_object_get_int(verobj);

      if (isIPACConfig(prodstr, version, jobj))
      {
        ret = updateBoardIPAC(jobj);
      }
      else if (isIPACUltimateConfig(prodstr, version, jobj))
      {
        log_info ("Updating IPAC Ultimate board...");
        ret = updateBoardIPacUltimate(jobj);
      }
      else if (isPACDriveConfig(prodstr, version, jobj))
      {
        log_info ("Updating PAC Drive board...");
        ret = updateBoardPacDrive(jobj);
      }
      else if (isPACLED64Config(prodstr, version, jobj))
      {
        log_info ("Updating PAC LED 64 board...");
        ret = updateBoardPacLED(jobj);
      }
      else if (isUltistikConfig(prodstr, version, jobj))
      {
        log_info ("Updating Ultistik board...");
        ret = updateBoardULTISTIK(jobj);
      }
    }
    else
    {
      if (json_object_object_get_ex(jobj, "version", &verobj))
      {
        log_err ("'version' is not defined as a integer");
      }
      else
      {
        log_err ("'version' is not defined in the configuration file");
      }
    }
  }
  else
  {
    if (json_object_object_get_ex(jobj, "product", &prodobj))
    {
      log_err ("'product' is not defined as a string");
    }
    else
    {
      log_err ("'product' is not defined in the configuration file");
    }
  }

  if (ret)
  {
    log_info ("Update done.\n");
  }
  else
  {
    log_info ("Update failed!\n");
  }

  json_object_put(jobj);
  return ret;
}
