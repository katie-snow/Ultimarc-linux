/*
 ============================================================================
 Name        : usbbutton.c
 Author      : Katie Snow
 Version     :
 Copyright   : Copyright 2014 Robert Abram, Katie Snow
 Description : USBButton configuration library
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
#include "usbbutton.h"
#include "dbg.h"

bool isUSBButtonConfig(json_object *jobj, ulboard* board)
{
  bool result = false;

  result = validateUSBButtonData(jobj, board);

  return result;
}

bool validateUSBButtonData(json_object* jobj, ulboard* board)
{
  bool result = true;
  json_object* tmp = NULL;
  json_object* color = NULL;
  json_object* entries = NULL;
  json_object* row = NULL;

  /* Required */
  if (json_object_object_get_ex(jobj, "action", &tmp))
  {
    if (!json_object_is_type(tmp, json_type_string))
    {
      log_err("'action' needs to be of type string");
      result = false;
    }
  }
  else
  {
    log_err("'action' is not defined in the configuration");
    result = false;
  }

  if (json_object_object_get_ex(jobj, "pressed", &tmp))
  {
    if (!json_object_is_type(tmp, json_type_object))
    {
      log_err("'pressed' needs to be of type object");
      result = false;
    }
    else
    {
      if (json_object_object_get_ex(tmp, "red", &color))
      {
        if(!json_object_is_type(color, json_type_int))
        {
          log_err("'red' needs to be of type integer");
          result = false;
        }
      }
      else
      {
        log_err("'red' is not defined in the configuration");
        result = false;
      }

      if (json_object_object_get_ex(tmp, "green", &color))
      {
        if(!json_object_is_type(color, json_type_int))
        {
          log_err("'green' needs to be of type integer");
          result = false;
        }
      }
      else
      {
        log_err("'green' is not defined in the configuration");
        result = false;
      }

      if (json_object_object_get_ex(tmp, "blue", &color))
      {
        if(!json_object_is_type(color, json_type_int))
        {
          log_err("'blue' needs to be of type integer");
          result = false;
        }
      }
      else
      {
        log_err("'blue' is not defined in the configuration");
        result = false;
      }
    }
  }
  else
  {
    log_err("'pressed' is not defined in the configuration");
    result = false;
  }

  if (json_object_object_get_ex(jobj, "released", &tmp))
  {
    if (!json_object_is_type(tmp, json_type_object))
    {
      log_err("'released' needs to be of type object");
      result = false;
    }
    else
    {
      if (json_object_object_get_ex(tmp, "red", &color))
      {
        if(!json_object_is_type(color, json_type_int))
        {
          log_err("'red' needs to be of type integer");
          result = false;
        }
      }
      else
      {
        log_err("'red' is not defined in the configuration");
        result = false;
      }

      if (json_object_object_get_ex(tmp, "green", &color))
      {
        if(!json_object_is_type(color, json_type_int))
        {
          log_err("'green' needs to be of type integer");
          result = false;
        }
      }
      else
      {
        log_err("'green' is not defined in the configuration");
        result = false;
      }

      if (json_object_object_get_ex(tmp, "blue", &color))
      {
        if(!json_object_is_type(color, json_type_int))
        {
          log_err("'blue' needs to be of type integer");
          result = false;
        }
      }
      else
      {
        log_err("'blue' is not defined in the configuration");
        result = false;
      }
    }
  }
  else
  {
    log_err("'released' is not defined in the configuration");
    result = false;
  }

  if (json_object_object_get_ex(jobj, "keys", &tmp))
  {
    if (!json_object_is_type(tmp, json_type_object))
    {
      log_err("'keys' needs to be of type object");
      result = false;
    }
    else
    {
      if (json_object_object_get_ex(tmp, "primary", &entries))
      {
        if (!json_object_is_type(entries, json_type_object))
        {
          log_err("'primary' needs to be of type object");
          result = false;
        }
        else
        {
          result = validateUSBButtonRowData (entries, "row 1", result);
          result = validateUSBButtonRowData (entries, "row 2", result);
          result = validateUSBButtonRowData (entries, "row 3", result);
          result = validateUSBButtonRowData (entries, "row 4", result);
        }
      }
      else
      {
        log_err("'primary' is not defined in the configuration");
        result = false;
      }

      if (json_object_object_get_ex(tmp, "secondary", &entries))
      {
        if (!json_object_is_type(entries, json_type_object))
        {
          log_err("'secondary' needs to be of type object");
          result = false;
        }
        else
        {
          result = validateUSBButtonRowData (entries, "row 1", result);
          result = validateUSBButtonRowData (entries, "row 2", result);
          result = validateUSBButtonRowData (entries, "row 3", result);
          result = validateUSBButtonRowData (entries, "row 4", result);
        }
      }
      else
      {
        log_err("'secondary' is not defined in the configuration");
        result = false;
      }
    }
  }
  else
  {
    log_err("'keys' is not defined in the configuration");
    result = false;
  }

  return result;
}

#define USBBTN_ROW_SIZE 6

bool validateUSBButtonRowData(json_object* entries, const char* rowStr, bool curResult)
{
  bool result = curResult;

  int pos = 0;

  json_object* row = NULL;
  json_object* item = NULL;

  if (json_object_object_get_ex(entries, rowStr, &row))
  {
    if (!json_object_is_type(row, json_type_array))
    {
      log_err("'%s' needs to be of type array", rowStr);
      result = false;
    }
    else
    {
      if (json_object_array_length(row) != USBBTN_ROW_SIZE)
      {
        log_err("'%s' array size was %i, but needs to be %i",
                rowStr, json_object_array_length(row),
                USBBTN_ROW_SIZE);
        result = false;
      }
      else
      {
        for (pos = 0; pos < USBBTN_ROW_SIZE; ++pos)
        {
          item = json_object_array_get_idx(row, pos);

          if (!json_object_is_type(item, json_type_string))
          {
            log_err("'%s' 'index %i' needs to be of type string",
                    rowStr, pos);
            result = false;
          }
        }
      }
    }
  }
  else
  {
    log_err("'%s' is not defined in the configuration",
            rowStr);
    result = false;
  }

  return result;
}

int usbKeyLookupTable[8][6] = {
/* Primary */
/* Row 1 */
{10, 11, 12, 13, 14, 15},
/* Row 2 */
{16, 17, 18, 19, 20, 21},
/* Row 3 */
{22, 23, 24, 25, 26, 27},
/* Row 4 */
{28, 29, 30, 31, 32, 33},

/* Secondary */
/* Row 1 */
{34, 35, 36, 37, 38, 39},
/* Row 2 */
{40, 41, 42, 43, 44, 45},
/* Row 3 */
{46, 47, 48, 49, 50, 51},
/* Row 4 */
{52, 53, 54, 55, 56, 57}

};

bool updateUSBButton (json_object* bcfg, ulboard* board)
{
  bool result = false;

  json_object* val = NULL;
  json_object* tmp = NULL;
  json_object* keys = NULL;
  json_object* color = NULL;

  const char* str = NULL;

  unsigned char barray[USBBTN_SIZE];
  memset (&barray, 0, sizeof(barray));

  /* Header */
  barray[0] = 0x50;
  barray[1] = 0xdd;

  /* Action */
  json_object_object_get_ex(bcfg, "action", &val);
  str = json_object_get_string(val);
  if (!strcasecmp(str, "alternate"))
  {
    barray[2] = 0;
  }
  else if (!strcasecmp(str, "extended"))
  {
    barray[2] = 0x01;
  }
  else if (!strcasecmp(str, "both"))
  {
    barray[2] = 0x02;
  }

  /* Release Colors */
  json_object_object_get_ex(bcfg, "released", &val);
  json_object_object_get_ex(val, "red", &color);
  barray[4] = convertDecimalToHex(json_object_get_int(color));

  json_object_object_get_ex(val, "green", &color);
  barray[5] = convertDecimalToHex(json_object_get_int(color));

  json_object_object_get_ex(val, "blue", &color);
  barray[6] = convertDecimalToHex(json_object_get_int(color));

  /* Press Colors */
  json_object_object_get_ex(bcfg, "pressed", &val);
  json_object_object_get_ex(val, "red", &color);
  barray[7] = convertDecimalToHex(json_object_get_int(color));

  json_object_object_get_ex(val, "green", &color);
  barray[8] = convertDecimalToHex(json_object_get_int(color));

  json_object_object_get_ex(val, "blue", &color);
  barray[9] = convertDecimalToHex(json_object_get_int(color));

  json_object_object_get_ex(bcfg, "keys", &tmp);

  /* Primary Keys*/
  json_object_object_get_ex(tmp, "primary", &val);
  json_object_object_get_ex(val, "row 1", &keys);
  populateUSBKeys(keys, 0, barray);

  json_object_object_get_ex(val, "row 2", &keys);
  populateUSBKeys(keys, 1, barray);

  json_object_object_get_ex(val, "row 3", &keys);
  populateUSBKeys(keys, 2, barray);

  json_object_object_get_ex(val, "row 4", &keys);
  populateUSBKeys(keys, 3, barray);

  /* Secondary Keys */
  json_object_object_get_ex(tmp, "secondary", &val);
  json_object_object_get_ex(val, "row 1", &keys);
  populateUSBKeys(keys, 4, barray);

  json_object_object_get_ex(val, "row 2", &keys);
  populateUSBKeys(keys, 5, barray);

  json_object_object_get_ex(val, "row 3", &keys);
  populateUSBKeys(keys, 6, barray);

  json_object_object_get_ex(val, "row 4", &keys);
  populateUSBKeys(keys, 7, barray);

  result = writeUSBButton(barray, 1, true);

  return result;
}

void populateUSBKeys(json_object* keys, int row, unsigned char* barray)
{
  int pos = 0;
  json_object* key = NULL;

  for(; pos < json_object_array_length(keys); ++pos)
  {
    key = json_object_array_get_idx(keys, pos);
    barray[usbKeyLookupTable[row][pos]] = convertIPACSeries(key);
  }
}

bool writeUSBButton(unsigned char* barray, int autoconnect, bool transfer)
{
  libusb_context *ctx = NULL;
  struct libusb_device_handle *handle = NULL;
  unsigned char mesg[USBBTN_MESG_LENGTH] = {0,0,0,0};

  bool result = true;

  int pos = 0;
  int ret = 0;

  if (transfer)
  {
    handle = openUSB(ctx, USBBTN_VENDOR, USBBTN_PRODUCT, USBBTN_INTERFACE, autoconnect);

    if (!handle)
    {
      result = false;
      goto error;
    }
  }

  while (pos < USBBTN_SIZE)
  {
    memcpy(&mesg[0], &barray[pos], 4);

    debug ("Writing (%i): %x, %x, %x, %x", pos, mesg[0], mesg[1], mesg[2], mesg[3]);
    if (transfer)
    {
      ret = libusb_control_transfer(handle,
                                    UM_REQUEST_TYPE,
                                    UM_REQUEST,
                                    USBBTN_VALUE,
                                    USBBTN_INTERFACE,
                                    mesg,
                                    USBBTN_MESG_LENGTH,
                                    UM_TIMEOUT);
      debug ("Write result: %i", ret);
    }
    pos+=USBBTN_MESG_LENGTH;
  }

exit:
  if (transfer)
  {
    closeUSB(ctx, handle, USBBTN_INTERFACE);
  }
  else
  {
    log_info ("board array was not written out!!!");
  }
  return result;

error:
  return result;
}
