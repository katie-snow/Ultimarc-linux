/*
 ============================================================================
 Name        : uhid.c
 Author      : Katie Snow
 Version     :
 Copyright   : Copyright 2014 Robert Abram, Katie Snow
 Description : U-HID board configuration library
 ============================================================================
 */

/* C */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Unix */
#include <json-c/json.h>
#include <libusb-1.0/libusb.h>

/* Local */
#include "ulboard.h"
#include "common.h"
#include "uhid.h"
#include "ipacseries.h"
#include "dbg.h"

bool isUHidConfig(json_object *jobj, ulboard* board)
{
  bool result = false;

  if (board->type == ulboard_type_uhid)
  {
    result = validateUHidData(jobj, board, 50);
  }

  if (board->type == ulboard_type_uhidNano)
  {
    result = validateUHidData(jobj, board, 8);
  }

  return result;
}

bool validateUHidData(json_object* jobj, ulboard* board, int pCount)
{
  bool result = true;

  int pinCount = 0;
  int objCount = 0;

  char* key = NULL;

  json_object* pins = NULL;
  json_object* pin  = NULL;
  json_object* tmp  = NULL;

  /* Required */
  if (json_object_object_get_ex(jobj, "pins", &pins))
  {
    if (json_object_is_type(pins, json_type_object))
    {
      json_object_object_foreach(pins, key, pin)
      {
        pinCount++;

        if (json_object_is_type(pin, json_type_object))
        {
          if (json_object_object_get_ex(pin, "switch", &tmp))
          {
            result = validateSwitch(tmp, key, result);
          }
          else if (json_object_object_get_ex(pin, "quadrature", &tmp))
          {
            result = validateQuadrature(tmp, key, result);
          }
          else if (json_object_object_get_ex(pin, "led", &tmp))
          {
            result = validateLED(tmp, key, result, pins);
          }
          else if (json_object_object_get_ex(pin, "analog axis", &tmp))
          {
            if (!json_object_is_type(tmp, json_type_string))
            {
              log_err("Pin '%s' entry 'analog axis' needs to be of type string", key);
              result = false;
            }
          }
          else if (json_object_object_get_ex(pin, "+5v", &tmp))
          {
            if (!json_object_is_type(tmp, json_type_boolean))
            {
              log_err("Pin '%s' entry '+5v' needs to be of type boolean", key);
              result = false;
            }
          }
          else if (json_object_object_get_ex(pin, "ground", &tmp))
          {
            if (!json_object_is_type(tmp, json_type_boolean))
            {
              log_err("Pin '%s' entry 'ground' needs to be of type boolean", key);
              result = false;
            }
          }
          else
          {
            log_err("Entry for pin '%s' does not have a valid object defined", key);
            result = false;
          }
        }
        else if (!json_object_is_type(pin, json_type_string))
        {
          log_err("pin '%s' needs to be of type object or string", key);
          result = false;
        }
      }

      if (pinCount != pCount)
      {
        log_err("Incorrect number of pin objects(%i).  Needs to be %i entries.", pinCount, pCount);
        result = false;
      }
    }
  }

  result = validateUHidMacros(jobj, result);

  result = validateUHidCalibration(jobj, result);

  result = validateUHidQuadratureButtonTime(jobj, result);

  return result;
}

/*
 * Data validated in this function
 *
 * "primary" : "a",                Required
   "secondary" : "b",              Optional
   "down" : "Normal Primary",      Required
   "up" : "clear",                 Required
   "shift" : false,                Optional
*/
bool validateSwitch(json_object* jobj, const char* key, bool curResult)
{
  bool result = curResult;

  json_object* entity = NULL;

  /* Required */
  if (json_object_object_get_ex(jobj, "primary", &entity))
  {
    if (!json_object_is_type(entity, json_type_string))
    {
      log_err("Pin '%s' entry 'primary' needs to be of type string", key);
      result = false;
    }
  }
  else
  {
    log_err ("Pin '%s' entry 'primary' is not defined in the configuration", key);
    result = false;
  }

  /* Required */
  if (json_object_object_get_ex(jobj, "down", &entity))
  {
    if (!json_object_is_type(entity, json_type_string))
    {
      log_err("Pin '%s' entry 'down' needs to be of type string", key);
      result = false;
    }
  }
  else
  {
    log_err ("Pin '%s' entry 'down' is not defined in the configuration", key);
    result = false;
  }

  /* Required */
  if (json_object_object_get_ex(jobj, "up", &entity))
  {
    if (!json_object_is_type(entity, json_type_string))
    {
      log_err("Pin '%s' entry 'up' needs to be of type string", key);
      result = false;
    }
  }
  else
  {
    log_err ("Pin '%s' entry 'up' is not defined in the configuration", key);
    result = false;
  }

  /* Optional */
  if (json_object_object_get_ex(jobj, "secondary", &entity))
  {
    if (!json_object_is_type(entity, json_type_string))
    {
      log_err("Pin '%s' entry 'secondary' needs to be of type string", key);
      result = false;
    }
  }

  /* Optional */
  if (json_object_object_get_ex(jobj, "shift", &entity))
  {
    if (!json_object_is_type(entity, json_type_boolean))
    {
      log_err("Pin '%s' entry 'shift' needs to be of type boolean", key);
      result = false;
    }
  }

  return result;
}

/*
 * Data validated in this function
 *
 * "mouse" : {
       "axis" : "y",      Required
       "reverse" : false  Optional
    }
    OR
    "Button" : "1-2"   Required
 */
bool validateQuadrature(json_object* jobj, const char* key, bool curResult)
{
  bool result = curResult;

  json_object* entity = NULL;
  json_object* tmp = NULL;

  /* check for valid pin */
  if (strncasecmp(key, "j4-", 3) == -1 &&
      strncasecmp(key, "j5-", 3) == -1)
  {
    log_err("Pin '%s' does not support quadrature functionality", key);
    result = false;
  }
  else if (json_object_object_get_ex(jobj, "mouse", &entity))
  {
    /* Required */
    if (json_object_object_get_ex(entity, "axis", &tmp))
    {
      if (!json_object_is_type(tmp, json_type_string))
      {
        log_err("Pin '%s' entry 'axis' needs to be of type string", key);
        result = false;
      }
    }
    else
    {
      log_err ("Pin '%s' entry 'axis' is not defined in the configuration", key);
      result = false;
    }

    /* Optional */
    if (json_object_object_get_ex(entity, "reverse", &tmp))
    {
      if (!json_object_is_type(tmp, json_type_boolean))
      {
        log_err("Pin '%s' entry 'reverse' needs to be of type boolean", key);
        result = false;
      }
    }
  }
  /* Required */
  else if (json_object_object_get_ex(jobj, "button", &entity))
  {
    if (!json_object_is_type(entity, json_type_string))
    {
      log_err("Pin '%s' entry 'button' needs to be of type string", key);
      result = false;
    }
  }
  else
  {
    log_err ("Pin '%s' entries 'button' or 'mouse' are not defined in the configuration", key);
    result = false;
  }

  return result;
}

/*
 * Data validated in this function
 *
 * switch" : "j1-p3",  Optional
   "PC" : "9"          Optional
 */
bool validateLED(json_object* jobj, const char* key, bool curResult, json_object* pins)
{
  bool result = curResult;

  const char* str = NULL;

  json_object* entity = NULL;
  json_object* pin = NULL;
  json_object* tmp = NULL;

  /* Optional */
  if (json_object_object_get_ex(jobj, "switch", &entity))
  {
    if (!json_object_is_type(entity, json_type_string))
    {
      log_err("LED pin '%s' entry 'switch' needs to be of type string", key);
      result = false;
    }

    /* validate selected pin has an action */
    str = json_object_get_string(entity);
    if (!json_object_object_get_ex(pins, str, &pin))
    {
      log_err("LED pin '%s' entry 'switch' pin '%s' is not a valid pin", key, str);
      result = false;
    }
    else if (!json_object_is_type(pin, json_type_object))
    {
      log_err("Pin '%s' entry 'switch' pin '%s' is disabled", key, str);
      result = false;
    }
    else
    {
      if (!json_object_object_get_ex(pin, "switch", &tmp) &&
          !json_object_object_get_ex(pin, "analog axis", &tmp) &&
          !json_object_object_get_ex(pin, "quadrature", &tmp))
      {
        log_err("LED pin '%s' entry 'switch' pin '%s' is not an action pin", key, str);
        result = false;
      }
    }
  }

  /* Optional */
  if (json_object_object_get_ex(jobj, "pc", &entity))
  {
    if (json_object_is_type(entity, json_type_int))
    {
      if (json_object_get_int(entity) < 1 ||
          json_object_get_int(entity) > 16)
      {
      log_err("LED pin '%s' entry 'pc' value '%i' is not between 1 and 16",
               key, json_object_get_int(entity));
      result = false;
      }
    }
    else
    {
      log_err("LED pin '%s' entry 'pc' needs to be of type int", key);
      result = false;
    }
  }
  return result;
}

/*
 * macros are optional.
 * Validation will be the following:
 *  each macro entry is an array (all boards)
 *  each key entry is type string (all boards)
 *  limit of 85 bytes for the complete macro group (2015 boards)
 */
bool validateUHidMacros (json_object *jobj, bool curResult)
{
  bool result = curResult;

  /* 2015 board variables */
  int macroEntries = 0;
  static const int maxMacroEntries = 85;

  int macroCount2015 = 0;
  static const int maxMacroCount2015 = 30;

  /* General variables */
  json_object* macros = NULL;
  json_object* key    = NULL;

  int keyCount = 0;

  if (json_object_object_get_ex(jobj, "macros", &macros))
  {
    if (!json_object_is_type(macros, json_type_object))
    {
      log_err ("'macros' is not of type object");
      result = false;
    }
    else
    {
      json_object_object_foreach(macros, name, macro)
      {
        macroEntries++;
        macroCount2015++;

        if (!json_object_is_type(macro, json_type_array))
        {
          log_err ("'%s' is not of type array", name);
          result = false;
        }
        else
        {
          macroEntries += json_object_array_length(macro);
          for (; keyCount < json_object_array_length(macro); ++keyCount)
          {
            key = json_object_array_get_idx(macro, keyCount);
            if (!json_object_is_type(key, json_type_string))
            {
              log_err ("Key entry in '%s' needs to be of type string", name);
              result = false;
            }
          }
        }
      }

      if (macroCount2015 > maxMacroCount2015)
      {
        log_err("The number of macros defined is '%i'. 30 macro entries are allowed.", macroCount2015);
        result = false;
      }

      if (macroEntries > maxMacroEntries)
      {
        log_err("The total size of the macros plus control characters is '%i'.  Needs to be a total size of 85 or less.", macroEntries);
        result = false;
      }
    }
  }

  return result;
}

bool validateUHidCalibration (json_object* jobj, bool curResult)
{
  bool result = curResult;

  int axisCount = 0;

  json_object *calibration = NULL;
  json_object *tmp = NULL;

  if (json_object_object_get_ex(jobj, "axis calibration", &calibration))
  {
    if (json_object_is_type(calibration, json_type_object))
    {
      json_object_object_foreach(calibration, key, axis)
      {
        axisCount++;

        if (json_object_is_type(axis, json_type_object))
        {
          /* Required */
          if (json_object_object_get_ex(axis, "offset", &tmp))
          {
            if (json_object_is_type(tmp, json_type_int))
            {
              if (json_object_get_int(tmp) < 0 ||
                  json_object_get_int(tmp) > 256)
              {
                log_err("Axis '%s' entry 'offset' value '%i' is not between 0 and 255", key, json_object_get_int(tmp));
                result = false;
              }
            }
            else
            {
              log_err("Axis '%s' entry 'offset' needs to be of type integer", key);
              result = false;
            }
          }
          else
          {
            log_err ("Axis '%s' entry 'offset' is not defined in the configuration", key);
            result = false;
          }

          /* Required */
          if (json_object_object_get_ex(axis, "scale", &tmp))
          {
            if (json_object_is_type(tmp, json_type_int))
            {
              if (json_object_get_int(tmp) < 1 ||
                  json_object_get_int(tmp) > 33)
              {
                log_err("Axis '%s' entry 'scale' value '%i' is not between 1 and 32", key, json_object_get_int(tmp));
                result = false;
              }
            }
            else
            {
              log_err("Axis '%s' entry 'scale' needs to be of type integer", key);
              result = false;
            }
          }
          else
          {
            log_err ("Axis '%s' entry 'scale' is not defined in the configuration", key);
            result = false;
          }
        }
        else
        {
          log_err ("Axis '%s' is not of type object", key);
          result = false;
        }
      }
    }
    else
    {
      log_err("'axis calibration' is not of type object");
      result = false;
    }
  }
  else
  {
    log_err("'axis calibration' is not defined in the configuration");
    result = false;

  }

  return result;
}

bool validateUHidQuadratureButtonTime (json_object* jobj, bool curResult)
{
  bool result = curResult;

  json_object* tmp = NULL;

  /* Required */
  if (json_object_object_get_ex(jobj, "quadrature time", &tmp))
  {
    if (json_object_is_type(tmp, json_type_int))
    {
      if (json_object_get_int(tmp) < 0 ||
          json_object_get_int(tmp) > 256)
      {
        log_err("'quadrature time' value '%i' is not between 0 and 255",
                json_object_get_int(tmp));
        result = false;
      }
    }
    else
    {
      log_err("'quadrature time' needs to be of type integer");
      result = false;
    }
  }
  else
  {
    log_err ("'quadrature time' is not defined in the configuration");
    result = false;
  }

  return result;
}

/* j1-p3, j1-p4, j1-p5, j1-p6, j1-p7, j1-p8, j1-p9, j1-p10,
 * j2-p3, j2-p4, j2-p5, j2-p6, j2-p7, j2-p8, j2-p9, j2-p10,
 * j3-p3, j3-p4, j3-p5, j3-p6, j3-p7, j3-p8, j3-p9, j3-p10,
 * j4-p3, j4-p4, j4-p5, j4-p6, j4-p7, j4-p8, j4-p9, j4-p10,
 * j5-p3, j5-p4, j5-p5, j5-p6, j5-p7, j5-p8, j5-p9, j5-p10,
 * j6-p3, j6-p4, j6-p5, j6-p6, j6-p7, j6-p8, j6-p9, j6-p10,
 * j7-p9, j7-p10
 */
int uhidKeyLookupTable[3][50] = {
/* U-HID */
{1, 3, 5, 7, 8, 6, 4, 2,
 24, 22, 20, 18, 40, 38, 36, 34,
 32, 30, 28, 26, 48, 46, 44, 42,
 16, 14, 12, 10, 9, 11, 13, 15,
 33, 35, 37, 38, 17, 19, 21, 23,
 41, 43, 45, 47, 25, 27, 29, 31,
 49, 50},

/* NANO */
{-1, -1 ,-1, -1, -1, -1, -1, -1,
 -1, -1 ,-1, -1, -1, -1, -1, -1,
 -1, -1 ,-1, -1, -1, -1, -1, -1,
 -1, -1 ,-1, -1, -1, -1, -1, -1,
 33, 35, 37, 38, 17, 19, 21, 23,
 -1, -1 ,-1, -1, -1, -1, -1, -1,
 -1, -1},

 /* BLUE */
{1, 3, 5, 7, 8, 6, 4, 2,
 24, 22, 20, 18, -1, -1, 36, 34,
 32, 30, 28, 26, -1, -1, -1, -1,
 16, 14, 12, 10, 9, 11, 13, 15,
 33, 35, -1, -1, 17, 19, 21, 23,
 -1, -1, -1, -1, 25, 27, 29, 31,
 -1, -1}

};

/* Board order
 * j1-p3, j1-p10, j1-p4, j1-p9, j1-p5, j1-p8, j1-p6, j1-p7
 * j4-p7, j4-p6, j4-p8, j4-p5, j4-p9, j4-p4, j4-p10, j4-p3
 * j5-p7, j2-p6, j5-p8, j2-p5, j5-p9, j2-p4, j5-p10, j2-p3
 * j6-p7, j3-p6, j6-p8, j3-p5, j6-p9, j3-p4, j6-p10, j3-p3
 * j5-p3, j2-p10, j5-p4, j2-p9, j5-p5, j2-p8, j5-p6, j2-p7,
 * j6-p3, j3-p10, j6-p4, j3-p9, j6-p5, j3-p8, j6-p6, j3-p7,
 * j7-p9, j7-p10
 */
int decipherUHidLookupKey (const char* key)
{
  int lkey = -1;

  if (!strcasecmp(key, "j1-p3"))
    lkey = 0;
  if (!strcasecmp(key, "j1-p4"))
    lkey = 1;
  if (!strcasecmp(key, "j1-p5"))
    lkey = 2;
  if (!strcasecmp(key, "j1-p6"))
    lkey = 3;
  if (!strcasecmp(key, "j1-p7"))
    lkey = 4;
  if (!strcasecmp(key, "j1-p8"))
    lkey = 5;
  if (!strcasecmp(key, "j1-p9"))
    lkey = 6;
  if (!strcasecmp(key, "j1-p10"))
    lkey = 7;
  if (!strcasecmp(key, "j2-p3"))
    lkey = 8;
  if (!strcasecmp(key, "j2-p4"))
    lkey = 9;
  if (!strcasecmp(key, "j2-p5"))
    lkey = 10;
  if (!strcasecmp(key, "j2-p6"))
    lkey = 11;
  if (!strcasecmp(key, "j2-p7"))
    lkey = 12;
  if (!strcasecmp(key, "j2-p8"))
    lkey = 13;
  if (!strcasecmp(key, "j2-p9"))
    lkey = 14;
  if (!strcasecmp(key, "j2-p10"))
    lkey = 15;
  if (!strcasecmp(key, "j3-p3"))
    lkey = 16;
  if (!strcasecmp(key, "j3-p4"))
    lkey = 17;
  if (!strcasecmp(key, "j3-p5"))
    lkey = 18;
  if (!strcasecmp(key, "j3-p6"))
    lkey = 19;
  if (!strcasecmp(key, "j3-p7"))
    lkey = 20;
  if (!strcasecmp(key, "j3-p8"))
    lkey = 21;
  if (!strcasecmp(key, "j3-p9"))
    lkey = 22;
  if (!strcasecmp(key, "j3-p10"))
    lkey = 23;
  if (!strcasecmp(key, "j4-p3"))
    lkey = 24;
  if (!strcasecmp(key, "j4-p4"))
    lkey = 25;
  if (!strcasecmp(key, "j4-p5"))
    lkey = 26;
  if (!strcasecmp(key, "j4-p6"))
    lkey = 27;
  if (!strcasecmp(key, "j4-p7"))
    lkey = 28;
  if (!strcasecmp(key, "j4-p8"))
    lkey = 29;
  if (!strcasecmp(key, "j4-p9"))
    lkey = 30;
  if (!strcasecmp(key, "j4-p10"))
    lkey = 31;
  if (!strcasecmp(key, "j5-p3"))
    lkey = 32;
  if (!strcasecmp(key, "j5-p4"))
    lkey = 33;
  if (!strcasecmp(key, "j5-p5"))
    lkey = 34;
  if (!strcasecmp(key, "j5-p6"))
    lkey = 35;
  if (!strcasecmp(key, "j5-p7"))
    lkey = 36;
  if (!strcasecmp(key, "j5-p8"))
    lkey = 37;
  if (!strcasecmp(key, "j5-p9"))
    lkey = 38;
  if (!strcasecmp(key, "j5-p10"))
    lkey = 39;
  if (!strcasecmp(key, "j6-p3"))
    lkey = 40;
  if (!strcasecmp(key, "j6-p4"))
    lkey = 41;
  if (!strcasecmp(key, "j6-p5"))
    lkey = 42;
  if (!strcasecmp(key, "j6-p6"))
    lkey = 43;
  if (!strcasecmp(key, "j6-p7"))
    lkey = 44;
  if (!strcasecmp(key, "j6-p8"))
    lkey = 45;
  if (!strcasecmp(key, "j6-p9"))
    lkey = 46;
  if (!strcasecmp(key, "j6-p10"))
    lkey = 47;
  if (!strcasecmp(key, "j7-p9"))
    lkey = 48;
  if (!strcasecmp(key, "j7-p10"))
    lkey = 49;

  if (lkey == -1)
  {
    log_info("Unable to decipher pin '%s'", key);
  }

  return lkey;
}

bool updateBoardUHid (json_object* bcfg, ulboard* board)
{
  unsigned char* barray = NULL;

  bool result = false;

  json_object* pins = NULL;
  json_object* macros = NULL;
  json_object* calibration = NULL;

  int bid = -1;

  /* Header data */
  char header[4] = {0x50, 0xdd, 0x00, 0x00};

  if (board->type == ulboard_type_uhid ||
      board->type == ulboard_type_uhidNano)
  {
    log_info ("Updating %s board...", ulBoardTypeToString(board->type));

    if (board->type == ulboard_type_uhid)
    {
      bid = UHID;
    }

    if (board->type == ulboard_type_uhidNano)
    {
      bid = NANO;
    }

    /* Pin assignment */
    barray = calloc(UHID_SIZE, sizeof(unsigned char));

    memcpy (barray, &header, sizeof(header));

    if (barray != NULL)
    {
      json_object_object_get_ex(bcfg, "pins", &pins);
      populateUHidBoardArray(bid, pins, &barray[3]);

      json_object_object_get_ex(bcfg, "macros", &macros);
      if (macros != NULL)
      {
        populateUHidMacro(macros, &barray[3]);
      }

      /* quadrature time */
      json_object_object_get_ex(bcfg, "quadrature time", &calibration);
      barray[3] = convertDecimalToHex(json_object_get_int(calibration));

      /* offset and scale */
      json_object_object_get_ex(bcfg, "axis calibration", &calibration);
      populateUHidCalibration(calibration, &barray[3]);

      result = writeUHid(barray, 1, true);
      free(barray);
    }
  }


  return result;
}

void populateUHidBoardArray(int bid, json_object* jobj, unsigned char* barray)
{
  int idx = -1;
  int idx2 = -1;
  int lkey = -1;
  int lkeyAdj = 0;

  const char *actionStr = NULL;
  unsigned char keyval;

  json_object *entity = NULL;
  json_object *axis = NULL;
  json_object *tmp = NULL;
  json_object *reverse = NULL;

  bool isReverse = false;

  json_object_object_foreach(jobj, key, pin)
  {
    lkey = decipherUHidLookupKey(key);

    if (lkey == -1)
      continue;

    // access uhid key lookup table with bid and lkey
    idx = uhidKeyLookupTable[bid][lkey];

    if (idx == -1)
    {
      log_warn("The key given does not have a position for the given board");
      continue;
    }

    if (json_object_is_type(pin, json_type_object))
    {
      if (json_object_object_get_ex(pin, "switch", &entity))
      {
        /* Populate board array with primary key */
        json_object_object_get_ex(entity, "primary", &tmp);
        barray[idx] = convertIPACSeries(tmp);

        /* Populate board array with secondary key */
        if (json_object_object_get_ex(entity, "secondary", &tmp))
        {
          json_object_object_get_ex(entity, "secondary", &tmp);
          keyval = convertIPACSeries(tmp);
        }
        else
        {
          keyval = 0;
        }
        barray[idx + 50] = keyval;

        /* Populate board array with action data */
        keyval = barray[idx + 100];

        /* Down */
        /* Set bits 0, 1, 2 */
        keyval += switchActionUHid(entity, false);

        /* Up */
        /* Set bits 3, 4, 5 */
        keyval |= switchActionUHid(entity, true);

        /* Shift */
        json_object_object_get_ex(entity, "shift", &tmp);
        if (tmp != NULL)
        {
          if (json_object_get_boolean(tmp) == true)
          {
            /* Set bit 6 */
            keyval |= 0x40;
          }
        }

        barray[idx + 100] = keyval;
      }
      else if (json_object_object_get_ex(pin, "quadrature", &entity))
      {
        /* lkey between 24 and 39 inclusive
         * if lkey is even then pin is odd adjust if reverse is true
         * if lkey is odd then pin is even adjust if reverse is false
         */
        if (lkey % 2 == 0)
        {
          /* pin is odd */
          lkeyAdj = 1;
        }
        else
        {
          /* pin is even */
          lkeyAdj = -1;
        }

        // access uhid key lookup table with bid and lkey + lkeyAdj
        idx2 = uhidKeyLookupTable[bid][lkey + lkeyAdj];

        if (json_object_object_get_ex(entity, "mouse", &tmp))
        {
          if (json_object_object_get_ex(tmp, "reverse", &reverse))
          {

            isReverse = json_object_get_boolean(reverse);
          }

          json_object_object_get_ex(tmp, "axis", &axis);

          actionStr = json_object_get_string(axis);

          if (actionStr[0] == 'x')
          {
            quadratureUHid(barray, idx, idx2, 0xC0, 0xC1, isReverse);
          }

          if (actionStr[0] == 'y')
          {
            quadratureUHid(barray, idx, idx2, 0xC2, 0xC3, isReverse);
          }

          if (actionStr[0] == 'z')
          {
            quadratureUHid(barray, idx, idx2, 0xC4, 0xC5, isReverse);
          }
        }
        else if (json_object_object_get_ex(entity, "button", &tmp))
        {
          actionStr = json_object_get_string(tmp);

          if (!strcasecmp(actionStr, "1-2"))
          {
            barray[idx] = 0xC6;
            barray[idx2] = 0xC6;
          }
          else if (!strcasecmp(actionStr, "3-4"))
          {
            barray[idx] = 0xC7;
            barray[idx2] = 0xC7;
          }
          else if (!strcasecmp(actionStr, "5-6"))
          {
            barray[idx] = 0xC8;
            barray[idx2] = 0xC8;
          }
          else if (!strcasecmp(actionStr, "7-8"))
          {
            barray[idx] = 0xC9;
            barray[idx2] = 0xC9;
          }
          else if (!strcasecmp(actionStr, "9-10"))
          {
            barray[idx] = 0xCA;
            barray[idx2] = 0xCA;
          }
          else if (!strcasecmp(actionStr, "11-12"))
          {
            barray[idx] = 0xCB;
            barray[idx2] = 0xCB;
          }
          else if (!strcasecmp(actionStr, "13-14"))
          {
            barray[idx] = 0xCC;
            barray[idx2] = 0xCC;
          }
          else if (!strcasecmp(actionStr, "15-16"))
          {
            barray[idx] = 0xCD;
            barray[idx2] = 0xCD;
          }
          else
          {
            log_info ("Pin '%s' does not have a valid entry for quadrature button", key);
          }
        }

        barray[idx + 100] = 0x01;
        barray[idx2 + 100] = 0x01;
      }
      else if (json_object_object_get_ex(pin, "led", &entity))
      {
        barray[idx] = 0x01;

        if (json_object_object_get_ex(entity, "switch", &tmp))
        {
          /* Get the pin details for the named pin in switch */
          actionStr = json_object_get_string(tmp);
          json_object_object_get_ex(jobj, actionStr, &tmp);

          /* get index and update action section of this pin */
          lkey = decipherUHidLookupKey(actionStr);

          /* access uhid key lookup table with bid and lkey */
          idx2 = uhidKeyLookupTable[bid][lkey];

          barray[idx2 + 100] |= 0x80;

          /* connect the two switches */
          barray[idx + 50] = convertDecimalToHex(idx2 + 4);
        }

        if (json_object_object_get_ex(entity, "pc", &tmp))
        {
          barray[idx + 100] = convertDecimalToHex(json_object_get_int(tmp));
        }
      }
      else if (json_object_object_get_ex(pin, "analog axis", &entity))
      {
        barray[idx] = analogAxisUHid (entity);
        barray[idx + 100] = 0x01;
      }
      else if (json_object_object_get_ex(pin, "+5v", &entity))
      {
        if (json_object_get_boolean(entity))
        {
          barray[idx] = 0xD1;
        }
      }
      else if (json_object_object_get_ex(pin, "ground", &entity))
      {
        if (json_object_get_boolean(entity))
        {
          barray[idx] = 0xD0;
        }
      }
    }
    /* else the pin is disabled */
  }
}

void populateUHidMacro(json_object* jobj, unsigned char* barray)
{
  int pos = 0;
  int idx = 168;

  json_object *tmp = NULL;

  json_object_object_foreach(jobj, key, macro)
  {
    barray[idx] = convertIPACSeries(json_object_new_string(key));

    idx++;
    for (pos = 0; pos < json_object_array_length(macro); pos++)
    {
      tmp = json_object_array_get_idx(macro, pos);
      barray[idx+pos] = convertIPACSeries(tmp);
    }

    /* Move the idx to the next available location */
    idx += pos;
  }
}

unsigned char switchActionUHid (json_object* jobj, bool isUp)
{
  json_object *tmp = NULL;
  unsigned char action = 0;
  const char *str = NULL;

  if (isUp)
  {
    json_object_object_get_ex(jobj, "up", &tmp);
  }
  else
  {
    json_object_object_get_ex(jobj, "down", &tmp);
  }

  str = json_object_get_string(tmp);


  if (!strcasecmp(str, "clear"))
  {
    action |= 0;
  }
  else if (!strcasecmp(str, "normal primary"))
  {
    action |= 1;
  }
  else if (!strcasecmp(str, "normal secondary"))
  {
    action |= 3;
  }
  else if (!strcasecmp(str, "pulse primary"))
  {
    action |= 5;
  }
  else if (!strcasecmp(str, "pulse secondary"))
  {
    action |= 7;
  }

  if (isUp)
  {
    action <<= 3;
  }

  return action;
}

void quadratureUHid (unsigned char* barray, int idx, int idx2, char val1, char val2, bool isReverse)
{
  if (isReverse)
  {
    barray[idx] = val2;
    barray[idx2] = val1;
  }
  else
  {
    barray[idx] = val1;
    barray[idx2] = val2;
  }

  barray[idx + 100] = 0x01;
  barray[idx2 + 100] = 0x01;
}

unsigned char analogAxisUHid(json_object* jobj)
{
  const char *str = NULL;
  unsigned char axis;

  str = json_object_get_string(jobj);

  if (!strcasecmp(str, "1(x)"))
    axis = 0xB0;
  if (!strcasecmp(str, "2(y)"))
    axis = 0xB1;
  if (!strcasecmp(str, "3 slider"))
    axis = 0xB2;
  if (!strcasecmp(str, "4 slider"))
    axis = 0xB3;
  if (!strcasecmp(str, "5 slider"))
    axis = 0xB4;
  if (!strcasecmp(str, "6 slider"))
    axis = 0xB5;
  if (!strcasecmp(str, "7 slider"))
    axis = 0xB6;
  if (!strcasecmp(str, "8 slider"))
    axis = 0xB7;
  if (!strcasecmp(str, "1(x) dz"))
    axis = 0xB8;
  if (!strcasecmp(str, "2(y) dz"))
    axis = 0xB9;

  return axis;
}

void populateUHidCalibration(json_object* jobj, unsigned char* barray)
{
  int offsetIdx = 151;
  int scaleIdx = 159;
  int pos = 0;

  json_object* offset = NULL;
  json_object* scale = NULL;

  json_object_object_foreach(jobj, key, axis)
  {
    json_object_object_get_ex(axis, "offset", &offset);
    json_object_object_get_ex(axis, "scale", &scale);

    pos = strtol(key, NULL, 10);

    barray[offsetIdx + pos] = convertDecimalToHex(json_object_get_int(offset));
    barray[scaleIdx + pos] = convertDecimalToHex(json_object_get_int(scale));
  }
}

bool writeUHid(unsigned char* barray, int autoconnect, bool transfer)
{
  libusb_context *ctx = NULL;
  struct libusb_device_handle *handle = NULL;
  unsigned char mesg[UHID_MESG_LENGTH] = {0,0,0,0};

  bool result = true;

  int pos = 0;
  int ret = 0;

  if (transfer)
  {
    handle = openUSB(ctx, UHID_VENDOR, UHID_PRODUCT, UHID_INTERFACE, autoconnect);

    if (!handle)
    {
      result = false;
      goto error;
    }
  }

  while (pos < UHID_SIZE)
  {
    memcpy(&mesg[0], &barray[pos], UHID_MESG_LENGTH);

    debug ("Writing (%i): %x, %x, %x, %x", pos, mesg[0], mesg[1], mesg[2], mesg[3]);
    if (transfer)
    {
      ret = libusb_control_transfer(handle,
                                    UM_REQUEST_TYPE,
                                    UM_REQUEST,
                                    UHID_VALUE,
                                    UHID_INTERFACE,
                                    mesg,
                                    UHID_MESG_LENGTH,
                                    UM_TIMEOUT);
      debug ("Write result: %i", ret);
    }
    pos+=UHID_MESG_LENGTH;
  }

exit:
  if (transfer)
  {
    closeUSB(ctx, handle, UHID_INTERFACE);
  }
  else
  {
    log_info ("board array was not written out!!!");
  }
  return result;

error:
  return result;
}
