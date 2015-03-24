/*
 ============================================================================
 Name        : pacseries.c
 Author      : Katie Snow
 Version     :
 Copyright   : Copyright 2014 Robert Abram, Katie Snow
 Description : Ultimarc 2015 PAC board configuration library
 ============================================================================
 */

/* C */
#include <stdio.h>
#include <string.h>

/* Unix */
#include <json.h>
#include <libusb-1.0/libusb.h>

/* Local */
#include "common.h"
#include "dbg.h"
#include "ipacseries.h"

bool validateIPACSeriesData(json_object* jobj)
{
  bool valid = true;

  json_object* cfg = NULL;
  json_object* pins = NULL;
  json_object* pin = NULL;
  json_object* tmp = NULL;
  char* key = NULL;

  if (json_object_object_get_ex(jobj, "Config", &cfg))
  {
    if (json_object_object_get_ex(cfg, "pins", &pins))
    {
      json_object_object_foreach(pins, key, pin)
      {
        if (strcmp(key, "Height") == 0 || strcmp(key, "Width") == 0)
        {
          continue;
        }

        if (json_object_object_get_ex(pin, "Title", &tmp))
        {
          if (!json_object_is_type(tmp, json_type_string))
          {
            valid = false;
            log_err ("%s 'Title' is not defined as a string", key);
          }
        }
        else
        {
          valid = false;
          log_err ("%s 'Title' is not defined in the configuration file", key);
        }

        if (json_object_object_get_ex(pin, "Options", &tmp))
        {
          if (!json_object_is_type(tmp, json_type_int))
          {
            valid = false;
            log_err ("%s 'Options' is not defined as an integer", key);
          }
        }
        else
        {
          valid = false;
          log_err ("%s 'Options' is not defined in the configuration file", key);
        }

        if (json_object_object_get_ex(pin, "Sequence", &tmp))
        {
          if (!json_object_is_type(tmp, json_type_int))
          {
            valid = false;
            log_err ("%s 'Sequence' is not defined as a integer", key);
          }
        }
        else
        {
          valid = false;
          log_err ("%s 'Sequence' is not defined in the configuration file", key);
        }

        if (json_object_object_get_ex(pin, "QuadPartner", &tmp))
        {
          if (!json_object_is_type(tmp, json_type_int))
          {
            valid = false;
            log_err ("%s 'QuadPartner' is not defined as a integer", key);
          }
        }
        else
        {
          valid = false;
          log_err ("%s 'QuadPartner' is not defined in the configuration file", key);
        }
      }
    }
    else
    {
      valid = false;
      log_err ("'pins' is not defined in the configuration file");
    }
  }
  else
  {
    valid = false;
    log_err ("'Config' is not defined in the configuration file");
  }

  return valid;
}
