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
