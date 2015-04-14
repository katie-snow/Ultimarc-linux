/*
 ============================================================================
 Name        : ipac4.c
 Author      : Katie Snow
 Version     :
 Copyright   : Copyright 2014 Robert Abram, Katie Snow
 Description : Ultimarc IPAC4 board configuration library
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
#include "dbg.h"
#include "ipac4.h"
#include "ipacseries.h"

struct ipac4 pIPAC4;

bool isIPAC4Config (const char* prodStr, int version, json_object* jobj)
{
  bool isBoardCfg = false;

  pIPAC4.version = version;

  if (strcmp(prodStr, IPAC4_STR) == 0)
  {
    switch(version)
    {
    case 1:
      isBoardCfg = validateIPAC4Data(jobj);
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

bool validateIPAC4Data (json_object* jobj)
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

  if (json_object_object_get_ex(jobj, "3/4 shift key", &tmp))
  {
    if (!json_object_is_type(tmp, json_type_string))
    {
      log_err ("3/4 shift key needs to be of type string");
      valid = false;
    }
  }
  else
  {
    log_err ("'3/4 shift key' is not defined in the configuration");
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

      if (pinCount != 56)
      {
        log_err("Incorrect number of pin objects.  Needs to be 56 entries.");
        valid = false;
      }
    }
  }

  return valid;
}

bool updateBoardIPAC4 (json_object *jobj)
{
  bool result = false;

  switch (pIPAC4.version)
  {
  case 1:
    result = updatePre2015BoardIPAC4(jobj);
    break;

  case 2:
    result = update2015BoardIPAC4(jobj);
    break;
  }
  return result;
}

bool updatePre2015BoardIPAC4 (json_object *jobj)
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
  char data[IPAC4_SIZE_PRE_2015];
  unsigned char mesg[IPACSERIES_MESG_LENGTH] = {0x03,0,0,0,0};

  handle = openUSB(ctx, IPAC4_VENDOR_PRE_2015, IPAC4_PRODUCT_PRE_2015, IPAC4_INTERFACE, 1);

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

  json_object_object_get_ex(jobj, "3/4 shift key", &shiftKey);
  data[104] = convertIPAC(shiftKey);

  json_object_object_get_ex(jobj, "pins", &pins);
  populateIPAC4Data(pins, data);

  while (pos < (IPAC4_SIZE_PRE_2015))
  {
    memcpy(&mesg[1], &data[pos], 4);
    pos+=4;

    debug ("Writing out the following data (%i): %x, %x, %x, %x, %x", pos, mesg[0], mesg[1], mesg[2], mesg[3], mesg[4]);
    ret = libusb_control_transfer(handle,
                                 UM_REQUEST_TYPE,
                                  UM_REQUEST,
                                  IPACSERIES_VALUE,
                                  IPAC4_INTERFACE,
                                  mesg,
                                  IPACSERIES_MESG_LENGTH,
                                  UM_TIMEOUT);
    debug ("Write result: %i", ret);
  }

exit:
  closeUSB(ctx, handle, IPAC4_INTERFACE);
  return result;

error:
  return result;
}

bool update2015BoardIPAC4 (json_object *jobj)
{
  bool result = false;

  return result;
}

void populateIPAC4Data (json_object* jobj, unsigned char* data)
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
      data[5] = keyval; data[33] = shiftval;
    }
    if (!strcasecmp(key, "1right"))
    {
      data[6] = keyval; data[34] = shiftval;
    }
    if (!strcasecmp(key, "1sw1"))
    {
      data[7] = keyval; data[35] = shiftval;
    }
    if (!strcasecmp(key, "1left"))
    {
      data[8] = keyval; data[36] = shiftval;
    }
    if (!strcasecmp(key, "1sw3"))
    {
      data[9] = keyval; data[37] = shiftval;
    }
    if (!strcasecmp(key, "1down"))
    {
      data[10] = keyval; data[38] = shiftval;
    }
    if (!strcasecmp(key, "1sw5"))
    {
      data[11] = keyval; data[39] = shiftval;
    }
    if (!strcasecmp(key, "1sw2"))
    {
      data[12] = keyval; data[40] = shiftval;
    }
    if (!strcasecmp(key, "2right"))
    {
      data[13] = keyval; data[41] = shiftval;
    }
    if (!strcasecmp(key, "1sw4"))
    {
      data[14] = keyval; data[42] = shiftval;
    }
    if (!strcasecmp(key, "2left"))
    {
      data[15] = keyval; data[43] = shiftval;
    }
    if (!strcasecmp(key, "1sw6"))
    {
      data[16] = keyval; data[44] = shiftval;
    }
    if (!strcasecmp(key, "2up"))
    {
      data[17] = keyval; data[45] = shiftval;
    }
    if (!strcasecmp(key, "2down"))
    {
      data[18] = keyval; data[46] = shiftval;
    }
    if (!strcasecmp(key, "2sw1"))
    {
      data[19] = keyval; data[47] = shiftval;
    }
    if (!strcasecmp(key, "1start"))
    {
      data[20] = keyval; data[48] = shiftval;
    }
    if (!strcasecmp(key, "2sw2"))
    {
      data[21] = keyval; data[49] = shiftval;
    }
    if (!strcasecmp(key, "2start"))
    {
      data[22] = keyval; data[50] = shiftval;
    }
    if (!strcasecmp(key, "2sw3"))
    {
      data[23] = keyval; data[51] = shiftval;
    }
    if (!strcasecmp(key, "1coin"))
    {
      data[24] = keyval; data[52] = shiftval;
    }
    if (!strcasecmp(key, "2sw4"))
    {
      data[25] = keyval; data[53] = shiftval;
    }
    if (!strcasecmp(key, "2coin"))
    {
      data[26] = keyval; data[54] = shiftval;
    }
    if (!strcasecmp(key, "2sw5"))
    {
      data[27] = keyval; data[55] = shiftval;
    }
    if (!strcasecmp(key, "1sw7"))
    {
      data[28] = keyval; data[56] = shiftval;
    }
    if (!strcasecmp(key, "2sw6"))
    {
      data[29] = keyval; data[57] = shiftval;
    }
    if (!strcasecmp(key, "1sw8"))
    {
      data[30] = keyval; data[58] = shiftval;
    }
    if (!strcasecmp(key, "2sw7"))
    {
      data[31] = keyval; data[59] = shiftval;
    }
    if (!strcasecmp(key, "2sw8"))
    {
      data[32] = keyval; data[60] = shiftval;
    }
    if (!strcasecmp(key, "3up"))
    {
      data[105] = keyval; data[133] = shiftval;
    }
    if (!strcasecmp(key, "3right"))
    {
      data[106] = keyval; data[134] = shiftval;
    }
    if (!strcasecmp(key, "3sw1"))
    {
      data[107] = keyval; data[135] = shiftval;
    }
    if (!strcasecmp(key, "3left"))
    {
      data[108] = keyval; data[136] = shiftval;
    }
    if (!strcasecmp(key, "3sw3"))
    {
      data[109] = keyval; data[137] = shiftval;
    }
    if (!strcasecmp(key, "3down"))
    {
      data[110] = keyval; data[138] = shiftval;
    }
    if (!strcasecmp(key, "3sw5"))
    {
      data[111] = keyval; data[139] = shiftval;
    }
    if (!strcasecmp(key, "3sw2"))
    {
      data[112] = keyval; data[140] = shiftval;
    }
    if (!strcasecmp(key, "4right"))
    {
      data[113] = keyval; data[141] = shiftval;
    }
    if (!strcasecmp(key, "3sw4"))
    {
      data[114] = keyval; data[142] = shiftval;
    }
    if (!strcasecmp(key, "4left"))
    {
      data[115] = keyval; data[143] = shiftval;
    }
    if (!strcasecmp(key, "3sw6"))
    {
      data[116] = keyval; data[144] = shiftval;
    }
    if (!strcasecmp(key, "4up"))
    {
      data[117] = keyval; data[145] = shiftval;
    }
    if (!strcasecmp(key, "4down"))
    {
      data[118] = keyval; data[146] = shiftval;
    }
    if (!strcasecmp(key, "4sw1"))
    {
      data[119] = keyval; data[147] = shiftval;
    }
    if (!strcasecmp(key, "3start"))
    {
      data[120] = keyval; data[148] = shiftval;
    }
    if (!strcasecmp(key, "4sw2"))
    {
      data[121] = keyval; data[149] = shiftval;
    }
    if (!strcasecmp(key, "4start"))
    {
      data[122] = keyval; data[150] = shiftval;
    }
    if (!strcasecmp(key, "4sw3"))
    {
      data[123] = keyval; data[151] = shiftval;
    }
    if (!strcasecmp(key, "3coin"))
    {
      data[124] = keyval; data[152] = shiftval;
    }
    if (!strcasecmp(key, "4sw4"))
    {
      data[125] = keyval; data[153] = shiftval;
    }
    if (!strcasecmp(key, "4coin"))
    {
      data[126] = keyval; data[154] = shiftval;
    }
    if (!strcasecmp(key, "4sw5"))
    {
      data[127] = keyval; data[155] = shiftval;
    }
    if (!strcasecmp(key, "3sw7"))
    {
      data[128] = keyval; data[156] = shiftval;
    }
    if (!strcasecmp(key, "4sw6"))
    {
      data[129] = keyval; data[157] = shiftval;
    }
    if (!strcasecmp(key, "3sw8"))
    {
      data[130] = keyval; data[158] = shiftval;
    }
    if (!strcasecmp(key, "4sw7"))
    {
      data[131] = keyval; data[159] = shiftval;
    }
    if (!strcasecmp(key, "4sw8"))
    {
      data[132] = keyval; data[160] = shiftval;
    }
  }
}
