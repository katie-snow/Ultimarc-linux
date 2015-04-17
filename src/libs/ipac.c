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
  pIPAC.ipac32 = (strcmp(prodStr, IPAC_STR_2) == 0);
  pIPAC.minipac = (strcmp(prodStr, IPAC_STR_M) == 0);

  if (pIPAC.ipac32 || pIPAC.minipac)
  {
    isBoardCfg = validateIPacData(jobj);
  /*
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
    */
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

  while (pos < IPAC_SIZE_PRE_2015)
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

  uint16_t product = 0;

  bool result = true;

  char header[4] = {0x50, 0xdd, 0x0f, 0x00};
  char data[IPAC_SIZE_2015];
  unsigned char mesg[IPACSERIES_MESG_LENGTH] = {0x03,0,0,0,0};

  if (pIPAC.ipac32)
  {
    product = IPAC_2_PRODUCT;
  }
  if (pIPAC.ipac32)
  {
    product = IPAC_M_PRODUCT;
  }

/*
  handle = openUSB(ctx, IPAC_VENDOR_2015, product, IPAC_INTERFACE, 1);

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

  /* Shift key data */
  memset (&data[107], 1, 47);

  json_object_object_get_ex(jobj, "pins", &pins);
  if (pIPAC.ipac32)
  {
    populateIPAC2Data(pins, &data[3]);
  }
  if (pIPAC.minipac)
  {
    populateMinIPACData(pins, data);
  }

  while (pos < IPAC_SIZE_2015)
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
    debug ("Write result: %i", ret);
    */
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

void populateIPAC2Data(json_object* jobj, unsigned char* data)
{
  int keypos = 0;
  int shiftpos = 0;

  unsigned char keyval;
  char shiftval;

  json_object *tmp = NULL;

/*
  json_object_object_get_ex(jobj, "1/2 shift key", &tmp);
  shiftval = json_object_get_string(tmp);
  {
*/
    /* Shift key position */
/*
    if (!strcasecmp(shiftval, "1right"))
      data[129] = 0x02;
    if (!strcasecmp(shiftval, "1up"))
      data[125] = 0x01;
    if (!strcasecmp(shiftval, "1down"))
      data[123] = 0x06;
    if (!strcasecmp(shiftval, "1left"))
      data[127] = 0x04;
    if (!strcasecmp(shiftval, "1sw1"))
      data[145] = 0x03;
    if (!strcasecmp(shiftval, "1sw2"))
      data[143] = 0x08;
    if (!strcasecmp(shiftval, "1sw3"))
      data[141] = 0x05;
    if (!strcasecmp(shiftval, "1sw4"))
      data[139] = 0x0A;
    if (!strcasecmp(shiftval, "1sw5"))
      data[137] = 0x07;
    if (!strcasecmp(shiftval, "1sw6"))
      data[135] = 0x0C;
    if (!strcasecmp(shiftval, "1sw7"))
      data[133] = 0x18;
    if (!strcasecmp(shiftval, "1sw8"))
      data[131] = 0x1A;
    if (!strcasecmp(shiftval, "1start"))
      data[153] = 0x10;
    if (!strcasecmp(shiftval, "1coin"))
      data[151] = 0x14;
    if (!strcasecmp(shiftval, "2right"))
      data[107] = 0x09;
    if (!strcasecmp(shiftval, "2up"))
      data[126] = 0x0D;
    if (!strcasecmp(shiftval, "2down"))
      data[124] = 0x0E;
    if (!strcasecmp(shiftval, "2left"))
      data[128] = 0x0B;
    if (!strcasecmp(shiftval, "2sw1"))
      data[122] = 0x0F;
    if (!strcasecmp(shiftval, "2sw2"))
      data[144] = 0x11;
    if (!strcasecmp(shiftval, "2sw3"))
      data[142] = 0x13;
    if (!strcasecmp(shiftval, "2sw4"))
      data[140] = 0x15;
    if (!strcasecmp(shiftval, "2sw5"))
      data[138] = 0x17;
    if (!strcasecmp(shiftval, "2sw6"))
      data[134] = 0x19;
    if (!strcasecmp(shiftval, "2sw7"))
      data[132] = 0x1B;
    if (!strcasecmp(shiftval, "2sw8"))
      data[130] = 0x1C;
    if (!strcasecmp(shiftval, "2start"))
      data[152] = 0x12;
    if (!strcasecmp(shiftval, "2coin"))
      data[150] = 0x16;
    if (!strcasecmp(shiftval, "1a"))
      data[149] = 0x1D;
    if (!strcasecmp(shiftval, "1b"))
      data[147] = 0x1E;
    if (!strcasecmp(shiftval, "2a"))
      data[148] = 0x1F;
    if (!strcasecmp(shiftval, "2b"))
      data[146] = 0x20;
    if (!strcasecmp(shiftval, "3right"))
      data[] = 0x02;
    if (!strcasecmp(shiftval, "3up"))
      data[] = 0x01;
    if (!strcasecmp(shiftval, "3down"))
      data[] = 0x06;
    if (!strcasecmp(shiftval, "3left"))
      data[] = 0x04;
    if (!strcasecmp(shiftval, "3sw1"))
      data[] = 0x03;
    if (!strcasecmp(shiftval, "3sw2"))
      data[] = 0x08;
    if (!strcasecmp(shiftval, "3sw3"))
      data[] = 0x05;
    if (!strcasecmp(shiftval, "3sw4"))
      data[] = 0x0A;
    if (!strcasecmp(shiftval, "3sw5"))
      data[] = 0x07;
    if (!strcasecmp(shiftval, "3sw6"))
      data[] = 0x0C;
    if (!strcasecmp(shiftval, "3sw7"))
      data[] = 0x18;
    if (!strcasecmp(shiftval, "3sw8"))
      data[] = 0x1A;
    if (!strcasecmp(shiftval, "3start"))
      data[] = 0x10;
    if (!strcasecmp(shiftval, "3coin"))
      data[] = 0x14;
    if (!strcasecmp(shiftval, "4right"))
      data[] = 0x09;
    if (!strcasecmp(shiftval, "4up"))
      data[] = 0x0D;
    if (!strcasecmp(shiftval, "4down"))
      data[] = 0x0E;
    if (!strcasecmp(shiftval, "4left"))
      data[] = 0x0B;
    if (!strcasecmp(shiftval, "4sw1"))
      data[] = 0x0F;
    if (!strcasecmp(shiftval, "4sw2"))
      data[] = 0x11;
    if (!strcasecmp(shiftval, "4sw3"))
      data[] = 0x13;
    if (!strcasecmp(shiftval, "4sw4"))
      data[] = 0x15;
    if (!strcasecmp(shiftval, "4sw5"))
      data[] = 0x17;
    if (!strcasecmp(shiftval, "4sw6"))
      data[] = 0x19;
    if (!strcasecmp(shiftval, "4sw7"))
      data[] = 0x1B;
    if (!strcasecmp(shiftval, "4sw8"))
      data[] = 0x1C;
    if (!strcasecmp(shiftval, "4start"))
      data[] = 0x12;
    if (!strcasecmp(shiftval, "4coin"))
      data[] = 0x16;
  } */

  json_object_object_foreach(jobj, key, pin)
  {
    /* Key value */
    json_object_object_get_ex(pin, "key", &tmp);
    keyval = convertIPACSeries(tmp);

    /* Shift key value */
    json_object_object_get_ex(pin, "shift", &tmp);
    shiftval = convertIPACSeries(tmp);

    if (!strcasecmp(key, "1up"))
    {
      data[19] = keyval; data[68] = shiftval;
    }
    if (!strcasecmp(key, "1right"))
    {
      data[23] = keyval; data[72] = shiftval;
    }
    if (!strcasecmp(key, "1sw1"))
    {
      data[39] = keyval; data[88] = shiftval;
    }
    if (!strcasecmp(key, "1left"))
    {
      data[21] = keyval; data[70] = shiftval;
    }
    if (!strcasecmp(key, "1sw3"))
    {
      data[35] = keyval; data[84] = shiftval;
    }
    if (!strcasecmp(key, "1down"))
    {
      data[17] = keyval; data[66] = shiftval;
    }
    if (!strcasecmp(key, "1sw5"))
    {
      data[31] = keyval; data[80] = shiftval;
    }
    if (!strcasecmp(key, "1sw2"))
    {
      data[37] = keyval; data[86] = shiftval;
    }
    if (!strcasecmp(key, "2right"))
    {
      data[1] = keyval; data[50] = shiftval;
    }
    if (!strcasecmp(key, "1sw4"))
    {
      data[33] = keyval; data[82] = shiftval;
    }
    if (!strcasecmp(key, "2left"))
    {
      data[22] = keyval; data[71] = shiftval;
    }
    if (!strcasecmp(key, "1sw6"))
    {
      data[29] = keyval; data[78] = shiftval;
    }
    if (!strcasecmp(key, "2up"))
    {
      data[20] = keyval; data[69] = shiftval;
    }
    if (!strcasecmp(key, "2down"))
    {
      data[18] = keyval; data[67] = shiftval;
    }
    if (!strcasecmp(key, "2sw1"))
    {
      data[16] = keyval; data[65] = shiftval;
    }
    if (!strcasecmp(key, "1start"))
    {
      data[47] = keyval; data[96] = shiftval;
    }
    if (!strcasecmp(key, "2sw2"))
    {
      data[38] = keyval; data[87] = shiftval;
    }
    if (!strcasecmp(key, "2start"))
    {
      data[46] = keyval; data[95] = shiftval;
    }
    if (!strcasecmp(key, "2sw3"))
    {
      data[36] = keyval; data[85] = shiftval;
    }
    if (!strcasecmp(key, "1coin"))
    {
      data[45] = keyval; data[94] = shiftval;
    }
    if (!strcasecmp(key, "2sw4"))
    {
      data[34] = keyval; data[83] = shiftval;
    }
    if (!strcasecmp(key, "2coin"))
    {
      data[44] = keyval; data[93] = shiftval;
    }
    if (!strcasecmp(key, "2sw5"))
    {
      data[32] = keyval; data[81] = shiftval;
    }
    if (!strcasecmp(key, "1sw7"))
    {
      data[27] = keyval; data[76] = shiftval;
    }
    if (!strcasecmp(key, "2sw6"))
    {
      data[28] = keyval; data[77] = shiftval;
    }
    if (!strcasecmp(key, "1sw8"))
    {
      data[25] = keyval; data[74] = shiftval;
    }
    if (!strcasecmp(key, "2sw7"))
    {
      data[26] = keyval; data[75] = shiftval;
    }
    if (!strcasecmp(key, "2sw8"))
    {
      data[24] = keyval; data[73] = shiftval;
    }
    if (!strcasecmp(key, "1a"))
    {
      data[43] = keyval; data[92] = shiftval;
    }
    if (!strcasecmp(key, "1b"))
    {
      data[41] = keyval; data[90] = shiftval;
    }
    if (!strcasecmp(key, "2a"))
    {
      data[42] = keyval; data[91] = shiftval;
    }
    if (!strcasecmp(key, "2b"))
    {
      data[40] = keyval; data[89] = shiftval;
    }
  }
}

void populateMinIPACData(json_object* jobj, unsigned char* data)
{

}
