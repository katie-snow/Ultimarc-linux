/*
 ============================================================================
 Name        : servostik.c
 Author      : Katie Snow
 Version     :
 Copyright   : Copyright 2014 Robert Abram, Katie Snow
 Description : servostik configuration library
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
#include "servostik.h"
#include "dbg.h"

bool isServoStikConfig(json_object *jobj, ulboard* board)
{
  bool result = false;

  if (board->type == ulboard_type_servostik)
  {
    result = validateServoStikData(jobj, board);
  }

  return result;
}

bool validateServoStikData(json_object* jobj, ulboard* board)
{
  bool result = true;
  int  val = 0;
  json_object* tmp = NULL;

  /* Required */
  if (json_object_object_get_ex(jobj, "switch", &tmp))
  {
    if (!json_object_is_type(tmp, json_type_int))
    {
      log_err("'switch' needs to be of type int");
      result = false;
    }
    else
    {
      val = json_object_get_int(tmp);
      if (val != 4 && val != 8)
      {
        log_err("'switch' value needs to be 4 or 8");
        result = false;
      }
    }
  }
  else
  {
    log_err("'switch' is not defined in the configuration");
    result = false;
  }

  /* Optional */
  /* NOTE: hardware option is a permanent action on this board.
   * Not documented in json configuration file
   * USE WITH CAUTION
   */
  if (json_object_object_get_ex(jobj, "hardware", &tmp))
  {
    if (!json_object_is_type(tmp, json_type_boolean))
      {
        log_err("'hardware' needs to be of type bool");
        result = false;
      }
  }

  return result;
}

bool updateServoStik (json_object* bcfg, ulboard* board)
{
  bool result = false;

  json_object* tmp = NULL;

  int val = 0;
  bool hardware = false;

  unsigned char barray[SERVOSTIK_SIZE];
  memset (&barray, 0, sizeof(barray));

  /* Header */
  barray[0] = 0x00;
  barray[1] = 0xdd;

  /* NOTE: hardware option is a permanent action on this board.
   * Not documented in json configuration file
   * USE WITH CAUTION
   */
  json_object_object_get_ex(bcfg, "hardware", &tmp);
  hardware = json_object_get_boolean(tmp);

  if (hardware)
  {
    barray[0] = 0xff;
    barray[1] = 0xdd;
    barray[2] = 0x55;
    barray[3] = 0x00;
  }
  else
  {
    /* Action */
    json_object_object_get_ex(bcfg, "switch", &tmp);
    val = json_object_get_int(tmp);

    if (val == 4)
    {
      barray[3] = 0;
    }
    else if (val == 8)
    {
      barray[3] = 0x01;
    }
  }

  result = writeServoStik(barray, 1, true);

  return result;
}

bool writeServoStik(unsigned char* barray, int autoconnect, bool transfer)
{
  libusb_context *ctx = NULL;
  struct libusb_device_handle *handle = NULL;
  unsigned char mesg[SERVOSTIK_MESG_LENGTH] = {0,0,0,0};

  bool result = true;

  int pos = 0;
  int ret = 0;

  if (transfer)
  {
    handle = openUSB(ctx, SERVOSTIK_VENDOR, SERVOSTIK_PRODUCT, SERVOSTIK_INTERFACE, autoconnect);

    if (!handle)
    {
      result = false;
      goto error;
    }
  }

  memcpy(&mesg[0], &barray[pos], 4);

  debug ("Writing : %x, %x, %x, %x", mesg[0], mesg[1], mesg[2], mesg[3]);
  if (transfer)
  {
    ret = libusb_control_transfer(handle,
                                  UM_REQUEST_TYPE,
                                  UM_REQUEST,
                                  SERVOSTIK_VALUE,
                                  SERVOSTIK_INTERFACE,
                                  mesg,
                                  SERVOSTIK_MESG_LENGTH,
                                  UM_TIMEOUT);
    debug ("Write result: %i", ret);

    ret = libusb_control_transfer(handle,
                                  UM_REQUEST_TYPE,
                                  UM_REQUEST,
                                  SERVOSTIK_VALUE,
                                  SERVOSTIK_INTERFACE,
                                  mesg,
                                  SERVOSTIK_MESG_LENGTH,
                                  UM_TIMEOUT);
    debug ("Write result: %i", ret);
  }

exit:
  if (transfer)
  {
    closeUSB(ctx, handle, SERVOSTIK_INTERFACE);
  }
  else
  {
    log_info ("board array was not written out!!!");
  }
  return result;

error:
  return result;
}
