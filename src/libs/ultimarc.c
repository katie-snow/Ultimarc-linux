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

void parseCmdLineArgs (int argc, char **argv)
{
  int idx;
  int inner_idx;

  json_object *jobj = NULL;
  json_object *innerobj = NULL;
  json_object *item = NULL;

  for (idx = 1; idx < argc; ++idx)
  {
    jobj = json_object_from_file (argv[idx]);

    if (determine_device(jobj) == ultimarc_multiple)
    {
      json_object_object_get_ex(jobj, "configurations", &innerobj);
      for (inner_idx = 0; inner_idx < json_object_array_length(innerobj); ++ inner_idx)
      {
        item = json_object_array_get_idx(innerobj, inner_idx);
        updateBoard(json_object_get_string(item));
      }
    }
    else
    {
      updateBoard(argv[idx]);
    }
  }
}

enum ultimarc_type determine_device (json_object* jobj)
{
  enum ultimarc_type type = ultimarc_none;
  if (isMultiple(jobj))
  {
    type = ultimarc_multiple;
  }
  else if (isIPAC(jobj))
  {
    type = ultimarc_ipac;
  }
  else if (isULTISTIK(jobj))
  {
    type = ultimarc_ultistik;
  }
  else if (isULTISTIKConfig(jobj))
  {
    type = ultimarc_ultistik_config;
  }
  else if (isPacLED(jobj))
  {
    type = ultimarc_pacled64;
  }

  return type;
}

bool updateBoard (const char* file)
{
  bool ret = false;
  json_object *jobj = NULL;

  jobj = json_object_from_file (file);

  switch (determine_device(jobj))
  {
    case ultimarc_ipac:
      printf ("Updating IPAC board...");
      ret = updateBoardIPAC(jobj);
      break;

    case ultimarc_ultistik:
      printf ("Updating Ultistick board...");
      ret = updateBoardULTISTIK(jobj);
      break;

    case ultimarc_ultistik_config:
      printf("Updating Ultistick controller ID...");
      ret = updateControllerULTISTIK(jobj);
      break;

    case ultimarc_pacled64:
      printf ("Updating PAC LED 64 board...");
      ret = updateBoardPacLED(jobj);
      break;

    default:
      printf ("ERROR: Unknown json file.  ");
      break;
  }

  if (ret)
  {
    printf ("Update done.\n");
  }
  else
  {
    printf ("Update failed!\n");
  }

  json_object_put(jobj);
  return ret;
}

bool isMultiple(json_object* jobj)
{
  json_object* tmp = NULL;
  if (json_object_object_get_ex(jobj, "configurations", &tmp))
  {
    return true;
  }

  return false;
}
