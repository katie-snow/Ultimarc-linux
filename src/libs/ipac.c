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
  pIPAC.ipac56 = false;
  pIPAC.ipac32 = ((strcmp(prodStr, IPAC_STR_2) == 0) || strcmp(prodStr, IPAC_STR_M) == 0);
  pIPAC.minipac = (strcmp(prodStr, IPAC_STR_M) == 0);

  if (pIPAC.ipac32 || pIPAC.ipac56)
  {
    switch (version)
    {
    case 1:
      isBoardCfg = validateIPacData(jobj);
      break;

    case 2:
      isBoardCfg = validateIPACSeriesData(jobj);
      break;

    default:
      log_info ("Configuration file version '%i' incorrect", version);
    }
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

  switch (pIPAC.version)
  {
  case 1:
    result = updatePre2015Board (jobj);
    break;

  case 2:
    result = update2015Board(jobj);
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

  char header[4] = {0x50, 0xdd, 0x00, 0x00};
  char data[IPAC_SIZE_PRE_2015];
  unsigned char mesg[IPACSERIES_MESG_LENGTH] = {0x03,0,0,0,0};

  handle = openUSB(ctx, IPAC_VENDOR_PRE_2015, IPAC_PRODUCT_PRE_2015, IPAC_INTERFACE, 1);

  if (!handle)
  {
    result = false;
    goto error;
  }

  /* Setup data to send to board */
  memset (&data, 0, sizeof(data));

  /* Header data */
  memcpy (&data, &header, sizeof(header));

  /* Macro data */
  data[61] = 0x30;

  json_object_object_get_ex(jobj, "1/2 shift key", &shiftKey);
  data[4] = convertIPAC(shiftKey);

  json_object_object_get_ex(jobj, "pins", &pins);
  populateIPACData(pins, data);

  while (pos < (IPAC_SIZE_PRE_2015))
  {
    memcpy(&mesg[1], &data[pos], 4);
    pos+=4;

    debug ("Writing out the following data (%i): %x, %x, %x, %x, %x", pos, mesg[0], mesg[1], mesg[2], mesg[3], mesg[4]);
    ret = libusb_control_transfer(handle,
                                  UM_REQUEST_TYPE,
                                  UM_REQUEST,
                                  IPACSERIES_VALUE,
                                  IPAC_INTERFACE,
                                  mesg,
                                  IPACSERIES_MESG_LENGTH,
                                  UM_TIMEOUT);
    debug ("Write result: %i", ret);
  }

exit:
  closeUSB(ctx, handle, IPAC_INTERFACE);
  return result;

error:
  return result;
}

bool update2015Board (json_object *jobj)
{
  libusb_context *ctx = NULL;
  struct libusb_device_handle *handle = NULL;

  json_object *shiftKey = NULL;
  json_object *pins = NULL;

  int ipac_idx = 4;
  int pos = 0;
  int ret = 0;

  uint16_t product;

  bool result = true;

  char header[4] = {0x50, 0xdd, 0x00, 0x00};
  char data[IPAC_SIZE_2015];
  unsigned char mesg[IPACSERIES_MESG_LENGTH] = {0x03,0,0,0,0};



  handle = openUSB(ctx, IPAC_VENDOR_2015, IPAC_PRODUCT_PRE_2015, IPAC_INTERFACE, 1);

  if (!handle)
  {
    result = false;
    goto error;
  }

  /* Setup data to send to board */
  memset (&data, 0, sizeof(data));

  /* Header data */
  memcpy (&data, &header, sizeof(header));
  memcpy (&data[100], &header, sizeof(header));

  /* Macro data */
  data[61] = 0x30;
  data[161] = 0xFB;
  data[162] = 0x01;

  json_object_object_get_ex(jobj, "1/2 shift key", &shiftKey);
  data[4] = convertIPAC(shiftKey);

  if (json_object_object_get_ex(jobj, "3/4 shift key", &shiftKey))
  {
    data[104] = convertIPAC(shiftKey);
  }

  json_object_object_get_ex(jobj, "pins", &pins);
  populateIPACData(pins, data);

  while (pos < (IPACSERIES_MESG_LENGTH))
  {
    memcpy(&mesg[1], &data[pos], 4);
    pos+=4;

    debug ("Writing out the following data (%i): %x, %x, %x, %x, %x", pos, mesg[0], mesg[1], mesg[2], mesg[3], mesg[4]);
    ret = libusb_control_transfer(handle,
                                  UM_REQUEST_TYPE,
                                  UM_REQUEST,
                                  IPACSERIES_VALUE,
                                  IPAC_INTERFACE,
                                  mesg,
                                  IPACSERIES_MESG_LENGTH,
                                  UM_TIMEOUT);
    debug ("Write result: %i", ret);
  }

exit:
  closeUSB(ctx, handle, IPAC_INTERFACE);
  return result;

error:
  return result;
}

void populateIPACData(json_object* jobj, unsigned char* data)
{
  int keypos = 0;
  int shiftpos = 0;

  unsigned char keyval;
  char shiftval;

  json_object *tmp = NULL;

  json_object_object_foreach(jobj, key, pin)
  {
    /* Key value */
    json_object_object_get_ex(pin, "key", &tmp);
    keyval = convertIPAC(tmp);

    /* Shift key value */
    json_object_object_get_ex(pin, "shift", &tmp);
    shiftval = convertIPAC(tmp);

    if (!strcasecmp(key, "1up"))
    {
      data[5] = keyval; data[37] = shiftval;
    }
    if (!strcasecmp(key, "1right"))
    {
      data[6] = keyval; data[38] = shiftval;
    }
    if (!strcasecmp(key, "1sw1"))
    {
      data[7] = keyval; data[39] = shiftval;
    }
    if (!strcasecmp(key, "1left"))
    {
      data[8] = keyval; data[40] = shiftval;
    }
    if (!strcasecmp(key, "1sw3"))
    {
      data[9] = keyval; data[41] = shiftval;
    }
    if (!strcasecmp(key, "1down"))
    {
      data[10] = keyval; data[42] = shiftval;
    }
    if (!strcasecmp(key, "1sw5"))
    {
      data[11] = keyval; data[43] = shiftval;
    }
    if (!strcasecmp(key, "1sw2"))
    {
      data[12] = keyval; data[44] = shiftval;
    }
    if (!strcasecmp(key, "2right"))
    {
      data[13] = keyval; data[45] = shiftval;
    }
    if (!strcasecmp(key, "1sw4"))
    {
      data[14] = keyval; data[46] = shiftval;
    }
    if (!strcasecmp(key, "2left"))
    {
      data[15] = keyval; data[47] = shiftval;
    }
    if (!strcasecmp(key, "1sw6"))
    {
      data[16] = keyval; data[48] = shiftval;
    }
    if (!strcasecmp(key, "2up"))
    {
      data[17] = keyval; data[49] = shiftval;
    }
    if (!strcasecmp(key, "2down"))
    {
      data[18] = keyval; data[50] = shiftval;
    }
    if (!strcasecmp(key, "2sw1"))
    {
      data[19] = keyval; data[51] = shiftval;
    }
    if (!strcasecmp(key, "1start"))
    {
      data[20] = keyval; data[52] = shiftval;
    }
    if (!strcasecmp(key, "2sw2"))
    {
      data[21] = keyval; data[53] = shiftval;
    }
    if (!strcasecmp(key, "2start"))
    {
      data[22] = keyval; data[54] = shiftval;
    }
    if (!strcasecmp(key, "2sw3"))
    {
      data[23] = keyval; data[55] = shiftval;
    }
    if (!strcasecmp(key, "1coin"))
    {
      data[24] = keyval; data[56] = shiftval;
    }
    if (!strcasecmp(key, "2sw4"))
    {
      data[25] = keyval; data[57] = shiftval;
    }
    if (!strcasecmp(key, "2coin"))
    {
      data[26] = keyval; data[58] = shiftval;
    }
    if (!strcasecmp(key, "2sw5"))
    {
      data[27] = keyval; data[59] = shiftval;
    }
    if (!strcasecmp(key, "1sw7"))
    {
      data[28] = keyval; data[60] = shiftval;
    }
    if (!strcasecmp(key, "2sw6"))
    {
      data[29] = keyval; data[61] = shiftval;
    }
    if (!strcasecmp(key, "1sw8"))
    {
      data[30] = keyval; data[62] = shiftval;
    }
    if (!strcasecmp(key, "2sw7"))
    {
      data[31] = keyval; data[63] = shiftval;
    }
    if (!strcasecmp(key, "2sw8"))
    {
      data[32] = keyval; data[63] = shiftval;
    }
    if (!strcasecmp(key, "1a"))
    {
      data[33] = keyval; data[64] = shiftval;
    }
    if (!strcasecmp(key, "1b"))
    {
      data[34] = keyval; data[65] = shiftval;
    }
    if (!strcasecmp(key, "2a"))
    {
      data[35] = keyval; data[66] = shiftval;
    }
    if (!strcasecmp(key, "2b"))
    {
      data[36] = keyval; data[67] = shiftval;
    }
  }
}
