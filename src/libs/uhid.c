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
#include <json.h>
#include <libusb-1.0/libusb.h>

/* Local */
#include "ulboard.h"
#include "common.h"
#include "uhid.h"
#include "dbg.h"

enum uhid_boards_t {NO_UHID_BOARD = -1, UHID, NANO};

bool isUHidConfig(json_object *jobj, ulboard* board)
{
  bool result = false;

  if (board->type == ulboard_type_uhid)
  {
    result = validateUHidData(jobj, board);
  }

  return result;
}

bool validateUHidData(json_object* jobj, ulboard* board)
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
            result = validateLED(tmp, key, result);
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

      if (pinCount != 50)
      {
        log_err("Incorrect number of pin objects(%i).  Needs to be 50 entries.", pinCount);
        result = false;
      }
    }
  }

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

  if (json_object_object_get_ex(jobj, "mouse", &entity))
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
bool validateLED(json_object* jobj, const char* key, bool curResult)
{
  bool result = curResult;

  json_object* entity = NULL;

  /* Optional */
  if (json_object_object_get_ex(jobj, "switch", &entity))
  {
    if (!json_object_is_type(entity, json_type_string))
    {
      log_err("Pin '%s' entry 'switch' needs to be of type string", key);
      result = false;
    }
  }

  /* Optional */
  if (json_object_object_get_ex(jobj, "pc", &entity))
  {
    if (!json_object_is_type(entity, json_type_string))
    {
      log_err("Pin '%s' entry 'pc' needs to be of type string", key);
      result = false;
    }
  }
  return result;
}

bool updateUHid (json_object* bcfg, ulboard* board)
{
  bool result = true;

  unsigned char* barray = NULL;

  result = writeUHid(barray, 1, true);

  return result;
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
    memcpy(&mesg[0], &barray[pos], 4);

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
