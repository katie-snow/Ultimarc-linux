/*
 ============================================================================
 Name        : ipac.c
 Author      : Robert Abram, Katie Snow
 Version     :
 Copyright   : Copyright 2014 Robert Abram, Katie Snow
 Description : Ultimarc IPAC2, IPAC4 and Mini-PAC configuration library
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
#include "common.h"
#include "ipac.h"
#include "ipacseries.h"
#include "dbg.h"

struct ipac pIPAC;

bool isIPACConfig (const char* prodStr, int version, json_object* jobj)
{
  bool isBoardCfg = false;

  pIPAC.version = version;
  pIPAC.ipac32 = (strcmp(prodStr, IPAC_STR_2) == 0);
  pIPAC.minipac = (strcmp(prodStr, IPAC_STR_M) == 0);

  if (pIPAC.ipac32 || pIPAC.minipac)
  {
    isBoardCfg = validateIPacData(jobj);
  }

  return isBoardCfg;
}

bool validateIPacData(json_object* jobj)
{
  bool valid = true;
  json_object* tmp = NULL;
  json_object* pin = NULL;
  json_object* pins = NULL;

  int pinCount = 0;
  int tmpCount = 0;
  char* key = NULL;
  char* tmpKey = NULL;

  /* Required */
  if (json_object_object_get_ex(jobj, "1/2 shift key", &tmp))
  {
    if (!json_object_is_type(tmp, json_type_string))
    {
      log_err ("1/2 shift key needs to be of type string");
      valid = false;
    }
  }
  else
  {
    log_err ("'1/2 shift key' is not defined in the configuration");
    valid = false;
  }

  /* Required */
  if (json_object_object_get_ex(jobj, "pins", &pins))
  {
    if (json_object_is_type(pins, json_type_object))
    {
      json_object_object_foreach(pins, key, pin)
      {
        pinCount++;

        tmpCount = 0;
        if (json_object_is_type(pin, json_type_object))
        {
          json_object_object_foreach(pin, tmpKey, tmp)
          {
            tmpCount++;
            if (!json_object_is_type(tmp, json_type_string))
            {
              valid = false;
            }
          }

          if (tmpCount != 2)
          {
            log_err("pin '%s' has to many children entities.", key);
            valid = false;
          }
        }
      }

      if (pinCount != 32)
      {
        log_err("Incorrect number of pin objects.  Needs to be 32 entries.");
        valid = false;
      }
    }
  }

  return valid;
}

bool updateBoardIPAC (json_object *jobj)
{
  bool result = false;
  unsigned char* barray = NULL;

  switch (pIPAC.version)
  {
  case 1:
    result = updatePre2015Board (jobj);
    break;

  case 2:
    barray = calloc(IPACSERIES_SIZE, sizeof(unsigned char));

    if (barray != NULL)
    {
      if (pIPAC.ipac32)
      {
        update2015IPACBoard(jobj, barray);
        result = writeIPACSeriesUSB(barray, IPACSERIES_SIZE, IPAC_VENDOR_2015, IPAC_2_PRODUCT, IPACSERIES_INTERFACE, 1, false);
      }

      if (pIPAC.minipac)
      {
        update2015MinIPACBoard(jobj, barray);
        result = writeIPACSeriesUSB(barray, IPACSERIES_SIZE, IPAC_VENDOR_2015, IPAC_M_PRODUCT, IPACSERIES_INTERFACE, 1, false);
      }

      free(barray);
    }
    break;
  }

  return result;
}

bool updatePre2015Board (json_object *jobj)
{
  libusb_context *ctx = NULL;
  struct libusb_device_handle *handle = NULL;

  json_object *shiftKey = NULL;
  json_object *pins = NULL;

  int ipac_idx = 4;
  int pos = 0;
  int ret = 0;

  bool result = true;

  unsigned char header[4] = {0x50, 0xdd, 0x00, 0x00};
  unsigned char data[IPAC_SIZE_PRE_2015];
  unsigned char mesg[IPACSERIES_MESG_LENGTH] = {0x03,0,0,0,0};

/*
  handle = openUSB(ctx, IPAC_VENDOR_PRE_2015, IPAC_PRODUCT_PRE_2015, IPAC_INTERFACE, 1);

  if (!handle)
  {
    result = false;
    goto error;
  }
*/

  /* Setup data to send to board */
  memset (&data, 0, sizeof(data));

  /* Header data */
  memcpy (&data, &header, sizeof(header));

  /* Macro data */
  data[69] = 0x30;

  json_object_object_get_ex(jobj, "1/2 shift key", &shiftKey);
  data[4] = convertIPAC(shiftKey);

  json_object_object_get_ex(jobj, "pins", &pins);
  populateBoardArray(PRE_IPAC2_BOARD, pins, &data[4]);

  while (pos < (IPAC_SIZE_PRE_2015))
  {
    memcpy(&mesg[1], &data[pos], 4);
    pos+=4;

    debug ("Writing out the following data (%i): %x, %x, %x, %x, %x", pos, mesg[0], mesg[1], mesg[2], mesg[3], mesg[4]);
/*
    ret = libusb_control_transfer(handle,
                                  UM_REQUEST_TYPE,
                                  UM_REQUEST,
                                  IPACSERIES_VALUE,
                                  IPAC_INTERFACE,
                                  mesg,
                                  IPACSERIES_MESG_LENGTH,
                                  UM_TIMEOUT);
    debug ("Write result: %i", ret); */
  }

exit:
  //closeUSB(ctx, handle, IPAC_INTERFACE);
  return result;

error:
  return result;
}

void update2015IPACBoard (json_object *jobj, unsigned char* barray)
{
  json_object *shiftKey = NULL;
  json_object *pins = NULL;

  /* Header data */
  char header[4] = {0x50, 0xdd, 0x0f, 0x00};
  memcpy (barray, &header, sizeof(header));

  /* Setup data to send to board */
  memset (&barray[4], 0xff, 51);
  memset (&barray[104], 1, 1);
  memset (&barray[120], 1, 14);
  memset (&barray[135], 1, 13);
  memset (&barray[150], 1, 1);
  memset (&barray[151], 0x41, 1);
  memset (&barray[159], 0x7f, 8);

  json_object_object_get_ex(jobj, "1/2 shift key", &shiftKey);
  populateShiftPosition(IPAC2_BOARD, shiftKey, &barray[3]);

  json_object_object_get_ex(jobj, "pins", &pins);
  populateBoardArray(IPAC2_BOARD, pins, &barray[3]);
}

void update2015MinIPACBoard (json_object *jobj, unsigned char* barray)
{
  json_object *shiftKey = NULL;
  json_object *pins = NULL;

  /* Header data */
  char header[4] = {0x50, 0xdd, 0x0f, 0x00};
  memcpy (barray, &header, sizeof(header));

  /* Setup data to send to board */
  memset (&barray[54], 0xff, 32);
  memset (&barray[104], 1, 32);
  memset (&barray[154], 0x7f, 8);
  memset (&barray[162], 0x10, 8);

  json_object_object_get_ex(jobj, "1/2 shift key", &shiftKey);
  populateShiftPosition(MINIPAC_BOARD, shiftKey, &barray[3]);

  json_object_object_get_ex(jobj, "pins", &pins);
  populateBoardArray(MINIPAC_BOARD, pins, &barray[3]);
}
