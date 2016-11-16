/*
 ============================================================================
 Name        : ultistik.c
 Author      : Robert Abram, Katie Snow
 Version     :
 Copyright   : Copyright 2014 Robert Abram, Katie Snow
 Description : Ultimarc Ultistik configuration library
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
#include "ultistik.h"
#include "dbg.h"
#include "ulboard.h"

bool isUltistikConfig (json_object* jobj, ulboard* board)
{
  bool isBoardCfg = false;

  if (board->type == ulboard_type_ultistik)
  {
    isBoardCfg = validateUltistikData(jobj, board);
  }

  return isBoardCfg;
}

bool validateUltistikData(json_object* jobj, ulboard* board)
{
  int idx = 0;
  bool valid = false;

  char data;
  const char invalidKey = 0xFF;

  json_object* tmp = NULL;
  json_object* key = NULL;

  if (checkBoardID(jobj, "controller id"))
  {
    /* Must have a valid controller id in the configuration file to even attempt to say
     * the configuration is valid.  Do Not place valid = true in any
     * of the other checks
     */
    valid = true;

    if (json_object_object_get_ex(jobj, "map", &tmp))
    {
      if (json_object_get_type(tmp) == json_type_array)
      {
        if (json_object_array_length(tmp) == 81)
        {
          for (idx = 0; idx < json_object_array_length(tmp); ++ idx)
          {
            key = json_object_array_get_idx(tmp, idx);
            data = convertULTISTIK (key);
            if (strcmp(&invalidKey, &data) == -1)
            {
              log_err ("Error at index %i in 'map' array, entry is '%s'", idx, json_object_get_string(key));
              valid = false;
            }
          }
        }
        else
        {
          log_err ("'map' array is not the correct size. Size is %i, should be 81", json_object_array_length(tmp));
          valid = false;
        }
      }
      else
      {
        log_err ("'map' is not defined as an array");
        valid = false;
      }
    }
    else
    {
      log_err ("'map' is not defined in the configuration file");
      valid = false;
    }

    if (json_object_object_get_ex(jobj, "borders", &tmp))
    {
      if (json_object_get_type(tmp) == json_type_array)
      {
        if (json_object_array_length(tmp) != 8)
        {
          valid = false;
          log_err ("'borders' array is not the correct size. Size is %i, should be 8", json_object_array_length(tmp));
        }
      }
      else
      {
        log_err ("'borders' is not defined as an array");
        valid = false;
      }
    }
    else
    {
      log_err ("'borders' is not defined in the configuration file");
      valid = false;
    }

    if (json_object_object_get_ex(jobj, "map size", &tmp))
    {
      if (json_object_get_type(tmp) == json_type_int)
      {
        if (json_object_get_int(tmp) != 9)
        {
          valid = false;
          log_err ("'map size' value is not the correct, should be 9");
        }
      }
      else
      {
        log_err ("'map size' is not defined as an integer");
        valid = false;
      }
    }
    else
    {
      log_err ("'map size' is not defined in the configuration file");
      valid = false;
    }

    if (json_object_object_get_ex(jobj, "restrictor", &tmp))
    {
      if (json_object_get_type(tmp) != json_type_boolean)
      {
        log_err ("'restictor' is not defined as a boolean");
        valid = false;
      }
    }
    else
    {
      log_err ("'restrictor' is not defined in the configuration file");
      valid = false;
    }

    if (json_object_object_get_ex(jobj, "flash", &tmp))
    {
      if (json_object_get_type(tmp) != json_type_boolean)
      {
        log_err ("'flash' is not defined as a boolean");
        valid = false;
      }
    }
    else
    {
      log_err ("'flash' is not defined in the configuration file");
      valid = false;
    }

    if (board->version == ulboard_version_2015)
    {
      if (json_object_object_get_ex(jobj, "keep analog", &tmp))
      {
        if (json_object_get_type(tmp) != json_type_boolean)
        {
          log_err ("'keep analog' is not defined as a boolean");
          valid = false;
        }
      }
      else
      {
        log_err ("'keep analog' is not defined in the configuration file");
        valid = false;
      }
    }
  }
  else if (checkBoardID(jobj, "current controller id"))
  {
    if (checkBoardID(jobj, "new controller id"))
    {
      valid = true;
    }
    else
    {
      valid = false;

      if (!json_object_object_get_ex(jobj, "new controller id", &tmp))
      {
        log_err ("'new controller id' is not defined in the configuration file");
      }
    }
  }
  else
  {
    log_err ("Ultistik configuration file is not configured correctly");
  }

  return valid;
}

char
convertULTISTIK (json_object *jobj)
{
  char retval = 0xFF;
  const char* str = json_object_get_string(jobj);

  if (strlen(str) > 0)
  {
    if (!strcasecmp(str, "-"))
      retval = 0x00;
    if (!strcasecmp(str, "C"))
      retval = 0x01;
    if (!strcasecmp(str, "N"))
      retval = 0x02;
    if (!strcasecmp(str, "NE"))
      retval = 0x03;
    if (!strcasecmp(str, "E"))
      retval = 0x04;
    if (!strcasecmp(str, "SE"))
      retval = 0x05;
    if (!strcasecmp(str, "S"))
      retval = 0x06;
    if (!strcasecmp(str, "SW"))
      retval = 0x07;
    if (!strcasecmp(str, "W"))
      retval = 0x08;
    if (!strcasecmp(str, "NW"))
      retval = 0x09;
    if (!strcasecmp(str, "*"))
      retval = 0x0A;
  }

    return retval;
}

bool updateBoardULTISTIK (json_object* jobj, ulboard* board)
{
  int idx     = 0;
  int itemidx = 0;
  int ret     = false;
  int controlCur = 0;
  int controlNew = 0;

  uint16_t product = -1;

  bool result = true;

  unsigned char data[USTIK_DATA_SIZE];

  libusb_context *ctx = NULL;
  struct libusb_device_handle *handle = NULL;

  json_object *innerobj  = NULL;
  json_object *item = NULL;

  memset(data, 0, sizeof(data));

  if (json_object_object_get_ex(jobj, "controller id", &innerobj))
  {
    data[0] = 0x50;

    json_object_object_get_ex(jobj, "map size", &innerobj);
    data[1] = json_object_get_int (innerobj);

    /* Restrictor: false off(0x10), true on(0x09) */
    json_object_object_get_ex(jobj, "restrictor", &innerobj);
    data[2] = (json_object_get_boolean(innerobj)? 0x09 : 0x10);

    /* Flash: false RAM(0xFF), true FLASH(0x00) */
    json_object_object_get_ex(jobj, "flash", &innerobj);
    switch (board->version)
    {
      case ulboard_version_pre2015:
        data[95] = (json_object_get_boolean(innerobj)? 0x00 : 0xFF);
      break;

      case ulboard_version_2015:
        /* 2015 and newer boards this value is zero */
        data[95] = 0;
      break;
    }

    /* Keep Analog: false off(0x50), true on(0x11) */
    if (board->version == ulboard_version_2015)
    {
      json_object_object_get_ex(jobj, "keep analog", &innerobj);
      data[0] = (json_object_get_boolean(innerobj)? 0x11 : 0x50);
    }

    /* Borders 3 - 10 */
    itemidx = 3;
    json_object_object_get_ex(jobj, "borders", &innerobj);
    for (idx = 0; idx < json_object_array_length(innerobj); ++ idx)
    {
      item = json_object_array_get_idx(innerobj, idx);
      data[itemidx] = json_object_get_int(item);
      ++itemidx;
    }

    /* Map */
    itemidx = 11;
    json_object_object_get_ex(jobj, "map", &innerobj);
    for (idx = 0; idx < json_object_array_length(innerobj); ++ idx)
    {
      item = json_object_array_get_idx(innerobj, idx);
      data[itemidx] = convertULTISTIK(item);
      ++itemidx;
    }

    json_object_object_get_ex(jobj, "controller id", &innerobj);
    controlCur = json_object_get_int(innerobj);

    switch (board->version)
    {
      case ulboard_version_pre2015:
        product = USTIK_PRODUCT_PRE_2015;
        product += (controlCur - 1);

        handle = openUSB(ctx, USTIK_VENDOR, product, USTIK_INTERFACE_PRE_2015, 1);

        if (!handle)
        {
          result = false;
          goto error;
        }

        ret = libusb_control_transfer(handle,
                                      USTIK_REQUEST_TYPE_1,
                                      USTIK_REQUEST_1,
                                      1,
                                      0,
                                      NULL,
                                      0,
                                      UM_TIMEOUT);

        for (idx = 0; idx < 3; ++ idx)
        {
          ret = libusb_control_transfer(handle,
                                        USTIK_REQUEST_TYPE_1,
                                        USTIK_REQUEST_2,
                                        0,
                                        0,
                                        data + (USTIK_MESG_PRE_2015*idx),
                                        USTIK_MESG_PRE_2015,
                                        UM_TIMEOUT);

          ret = libusb_control_transfer(handle,
                                        USTIK_REQUEST_TYPE_2,
                                        USTIK_REQUEST_3,
                                        0,
                                        0,
                                        NULL,
                                        0,
                                        UM_TIMEOUT);
        }

        ret = libusb_control_transfer(handle,
                                      USTIK_REQUEST_TYPE_1,
                                      USTIK_REQUEST_1,
                                      0,
                                      0,
                                      NULL,
                                      0,
                                      UM_TIMEOUT);


        closeUSB(ctx, handle, USTIK_INTERFACE_PRE_2015);
      break;

      case ulboard_version_2015:
        product = USTIK_PRODUCT;
        product += (controlCur - 1);

        handle = openUSB(ctx, USTIK_VENDOR, product, USTIK_INTERFACE, 1);

        if (!handle)
        {
          result = false;
          goto error;
        }

        idx = 0;
        while (idx < USTIK_DATA_SIZE)
        {
          debug ("Writing (%i): %x, %x, %x, %x", idx, data[idx],
              data[idx+1], data[idx+2], data[idx+3]);
          ret = libusb_control_transfer(handle,
                                        UM_REQUEST_TYPE,
                                        UM_REQUEST,
                                        USTIK_VALUE,
                                        USTIK_INTERFACE,
                                        data + idx,
                                        USTIK_MESG_LENGTH,
                                        UM_TIMEOUT);
          debug ("Write result: %i", ret);

          idx+=4;
        }

        closeUSB(ctx, handle, USTIK_INTERFACE);
      break;
    }
  }
  else  // Changing controller ID
  {
    json_object_object_get_ex(jobj, "new controller id", &innerobj);
    controlNew = json_object_get_int(innerobj);
    data[0] = USTIK_CONFIG_BASE + (controlNew - 1);

    json_object_object_get_ex(jobj, "current controller id", &innerobj);
    controlCur = json_object_get_int(innerobj);

    switch (board->version)
    {
      case ulboard_version_pre2015:
        product = USTIK_PRODUCT_PRE_2015;
        product += (controlCur - 1);

        handle = openUSB(ctx, USTIK_VENDOR, product, USTIK_INTERFACE_PRE_2015, 0);

        if (!handle)
        {
          result = false;
          goto error;
        }

        ret = libusb_control_transfer(handle,
                                      USTIK_REQUEST_TYPE_1,
                                      USTIK_REQUEST_1,
                                      1,
                                      0,
                                      NULL,
                                      0,
                                      UM_TIMEOUT);

        ret = libusb_control_transfer(handle,
                                      USTIK_REQUEST_TYPE_1,
                                      USTIK_REQUEST_2,
                                      0,
                                      0,
                                      data,
                                      USTIK_MESG_PRE_2015,
                                      UM_TIMEOUT);

        ret = libusb_control_transfer(handle,
                                      USTIK_REQUEST_TYPE_2,
                                      USTIK_REQUEST_3,
                                      0,
                                      0,
                                      NULL,
                                      0,
                                      UM_TIMEOUT);

        ret = libusb_control_transfer(handle,
                                      USTIK_REQUEST_TYPE_1,
                                      USTIK_REQUEST_1,
                                      0,
                                      0,
                                      NULL,
                                      0,
                                      UM_TIMEOUT);

        closeUSB(ctx, handle, USTIK_INTERFACE_PRE_2015);
      break;

    case ulboard_version_2015:
      product = USTIK_PRODUCT;
      product += (controlCur - 1);

      handle = openUSB(ctx, USTIK_VENDOR, product, USTIK_INTERFACE, 1);

      if (!handle)
      {
        result = false;
        goto error;
      }

      debug ("Writing (%i): %x, %x, %x, %x", 0, data[0],
          data[1], data[2], data[3]);
      ret = libusb_control_transfer(handle,
                                    UM_REQUEST_TYPE,
                                    UM_REQUEST,
                                    USTIK_VALUE,
                                    USTIK_INTERFACE,
                                    data,
                                    USTIK_MESG_LENGTH,
                                    UM_TIMEOUT);
      debug ("Write result: %i", ret);

      closeUSB(ctx, handle, USTIK_INTERFACE);
    break;
    }

    log_info ("Ultistik #%i needs to be physically disconnected and reconnected before use.", controlNew);
  }

/* I know these have the same statement currently, but leaving
 * exit in for the just-in-case moment down the line.
 */
  exit:
    return result;

  error:
    return result;
}
