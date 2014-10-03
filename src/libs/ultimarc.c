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

  enum ultimarc_type product;

  product = validateProduct(jobj);
  if (product != ultimarc_none &&
      validateVersion(jobj, product) &&
      validateData(jobj, product))
  {
    switch (product)
    {
      case ultimarc_ipac:
        log_info ("Updating IPAC board...");
        ret = updateBoardIPAC(jobj);
        break;

      case ultimarc_ultistik:
        log_info ("Updating Ultistik board...");
        ret = updateBoardULTISTIK(jobj);
        break;

      case ultimarc_pacled64:
        log_info ("Updating PAC LED 64 board...");
        ret = updateBoardPacLED(jobj);
        break;

      case ultimarc_pacdrive:
    	log_info ("Updating PAC Drive board...");
    	ret = updateBoardPacDrive(jobj);
    	break;

      default:
        break;
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

enum ultimarc_type validateProduct(json_object* jobj)
{
  enum ultimarc_type type = ultimarc_none;
  const char* s1 = NULL;
  char* s2 = NULL;

  json_object* prodobj = NULL;

  if (json_object_object_get_ex(jobj, "product", &prodobj) &&
      json_object_get_type(prodobj) == json_type_string)
  {
    if (strcmp(json_object_get_string(prodobj), getIPacProductStr()) == 0)
    {
      type = ultimarc_ipac;
    }
    else if (strcmp(json_object_get_string(prodobj), getUltistikProductStr()) == 0)
    {
      type = ultimarc_ultistik;
    }
    else if (strcmp(json_object_get_string(prodobj), getPacLED64ProductStr()) == 0)
    {
      type = ultimarc_pacled64;
    }
    else if (strcmp(json_object_get_string(prodobj), getPacDriveProductStr()) == 0)
    {
      type = ultimarc_pacdrive;
    }
    else
    {
      log_err ("'product' does not have a valid entry");
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

  return type;
}

bool validateVersion(json_object* jobj, enum ultimarc_type device)
{
  json_object* verobj = NULL;
  int version = 0;
  bool valid = false;

  if (json_object_object_get_ex(jobj, "version", &verobj) &&
      json_object_get_type(verobj) == json_type_int)
  {
    switch(device)
    {
      case ultimarc_ipac:
        version = getIPacVersion();
        break;

      case ultimarc_ultistik:
        version = getUltistikVersion();
        break;

      case ultimarc_pacled64:
        version = getPacLED64Version();
        break;

      case ultimarc_pacdrive:
    	version = getPacDriveVersion();
    	break;

      default:
        break;
    }

    if (version == json_object_get_int(verobj))
    {
      valid = true;
    }
    else
    {
      log_err ("Expected version %i, but configuration file contained %i", version, json_object_get_int(verobj));
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

  return valid;
}

bool validateData(json_object*jobj, enum ultimarc_type device)
{
  bool dataValid = false;
  switch(device)
  {
    case ultimarc_ipac:
      dataValid = validateIPacData(jobj);
      break;

    case ultimarc_ultistik:
      dataValid = validateUltistikData(jobj);
      break;

    case ultimarc_pacled64:
      dataValid = validatePacLED64Data(jobj);
      break;

    case ultimarc_pacdrive:
      dataValid = validatePacDriveData(jobj);
      break;

    default:
      break;
  }

  return dataValid;
}
