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

  pIPAC.ipac56 = (strcmp(prodStr, IPAC_STR_4) == 0);
  pIPAC.ipac32 = ((strcmp(prodStr, IPAC_STR_2) == 0) || strcmp(prodStr, IPAC_STR_M) == 0);

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

  /* Optional */
  if (json_object_object_get_ex(jobj, "3/4 shift key", &tmp))
  {
    if (!json_object_is_type(tmp, json_type_string))
    {
      log_err ("3/4 shift key needs to be of type string");
      valid = false;
    }
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

      if (pinCount != 32 && pinCount != 56)
      {
        log_err("Incorrect number of pin objects.  Needs to be 32 or 56 entries.");
        valid = false;
      }
    }
  }

  return valid;
}

char
convertIPAC (json_object *jobj)
{
  char retval = 0x00;
  const char* str = json_object_get_string(jobj);

  if (strlen(str) > 0)
  {
    if (!strcasecmp(str, "A"))
      retval = 0x1C;
    if (!strcasecmp(str, "B"))
      retval = 0x32;
    if (!strcasecmp(str, "C"))
      retval = 0x21;
    if (!strcasecmp(str, "D"))
      retval = 0x23;
    if (!strcasecmp(str, "E"))
      retval = 0x24;
    if (!strcasecmp(str, "F"))
      retval = 0x2B;
    if (!strcasecmp(str, "G"))
      retval = 0x34;
    if (!strcasecmp(str, "H"))
      retval = 0x33;
    if (!strcasecmp(str, "I"))
      retval = 0x43;
    if (!strcasecmp(str, "J"))
      retval = 0x3B;
    if (!strcasecmp(str, "K"))
      retval = 0x42;
    if (!strcasecmp(str, "L"))
      retval = 0x4B;
    if (!strcasecmp(str, "M"))
      retval = 0x3A;
    if (!strcasecmp(str, "N"))
      retval = 0x31;
    if (!strcasecmp(str, "O"))
      retval = 0x44;
    if (!strcasecmp(str, "P"))
      retval = 0x4D;
    if (!strcasecmp(str, "Q"))
      retval = 0x15;
    if (!strcasecmp(str, "R"))
      retval = 0x2D;
    if (!strcasecmp(str, "S"))
      retval = 0x1B;
    if (!strcasecmp(str, "T"))
      retval = 0x2C;
    if (!strcasecmp(str, "U"))
      retval = 0x3C;
    if (!strcasecmp(str, "V"))
      retval = 0x2A;
    if (!strcasecmp(str, "W"))
      retval = 0x1D;
    if (!strcasecmp(str, "X"))
      retval = 0x22;
    if (!strcasecmp(str, "Y"))
      retval = 0x35;
    if (!strcasecmp(str, "Z"))
      retval = 0x1A;
    if (!strcasecmp(str, "1"))
      retval = 0x16;
    if (!strcasecmp(str, "2"))
      retval = 0x1E;
    if (!strcasecmp(str, "3"))
      retval = 0x26;
    if (!strcasecmp(str, "4"))
      retval = 0x25;
    if (!strcasecmp(str, "5"))
      retval = 0x2E;
    if (!strcasecmp(str, "6"))
      retval = 0x36;
    if (!strcasecmp(str, "7"))
      retval = 0x3D;
    if (!strcasecmp(str, "8"))
      retval = 0x3E;
    if (!strcasecmp(str, "9"))
      retval = 0x46;
    if (!strcasecmp(str, "0"))
      retval = 0x45;
    if (!strcasecmp(str, "ESC"))
      retval = 0x76;
    if (!strcasecmp(str, "-"))
      retval = 0x4E;
    if (!strcasecmp(str, "="))
      retval = 0x55;
    if (!strcasecmp(str, "BKSP"))
      retval = 0x66;
    if (!strcasecmp(str, "TAB"))
      retval = 0x0D;
    if (!strcasecmp(str, "["))
      retval = 0x54;
    if (!strcasecmp(str, "]"))
      retval = 0x5B;
    if (!strcasecmp(str, "ENTER"))
      retval = 0x5A;
    if (!strcasecmp(str, "KP ENTER"))
      retval = 0x5A;
    if (!strcasecmp(str, "CTRL L"))
      retval = 0x14;
    if (!strcasecmp(str, "CTRL R"))
      retval = 0x14;
    if (!strcasecmp(str, ";"))
      retval = 0x4C;
    if (!strcasecmp(str, "'"))
      retval = 0x52;
    if (!strcasecmp(str, "`"))
      retval = 0x0E;
    if (!strcasecmp(str, "SHIFT L"))
      retval = 0x12;
    if (!strcasecmp(str, "\\"))
      retval = 0x5D;
    if (!strcasecmp(str, ","))
      retval = 0x41;
    if (!strcasecmp(str, "."))
      retval = 0x49;
    if (!strcasecmp(str, "/"))
      retval = 0x4A;
    if (!strcasecmp(str, "KP /"))
      retval = 0x4A;
    if (!strcasecmp(str, "SHIFT R"))
      retval = 0x59;
    if (!strcasecmp(str, "PRNT SCRN"))
      retval = 0x7C;
    if (!strcasecmp(str, "ALT L"))
      retval = 0x11;
    if (!strcasecmp(str, "ALT R"))
      retval = 0x11;
    if (!strcasecmp(str, "SPACE"))
      retval = 0x29;
    if (!strcasecmp(str, "CAPS"))
      retval = 0x58;
    if (!strcasecmp(str, "F1"))
      retval = 0x05;
    if (!strcasecmp(str, "F2"))
      retval = 0x06;
    if (!strcasecmp(str, "F3"))
      retval = 0x04;
    if (!strcasecmp(str, "F4"))
      retval = 0x0C;
    if (!strcasecmp(str, "F5"))
      retval = 0x03;
    if (!strcasecmp(str, "F6"))
      retval = 0x0B;
    if (!strcasecmp(str, "F7"))
      retval = 0x83;
    if (!strcasecmp(str, "F8"))
      retval = 0x0A;
    if (!strcasecmp(str, "F9"))
      retval = 0x01;
    if (!strcasecmp(str, "F10"))
      retval = 0x09;
    if (!strcasecmp(str, "F11"))
      retval = 0x78;
    if (!strcasecmp(str, "F12"))
      retval = 0x07;
    if (!strcasecmp(str, "NUM"))
      retval = 0x77;
    if (!strcasecmp(str, "SCROLL"))
      retval = 0x7E;
    if (!strcasecmp(str, "KP 1"))
      retval = 0x69;
    if (!strcasecmp(str, "KP 2"))
      retval = 0x72;
    if (!strcasecmp(str, "KP 3"))
      retval = 0x7A;
    if (!strcasecmp(str, "KP 4"))
      retval = 0x6B;
    if (!strcasecmp(str, "KP 5"))
      retval = 0x73;
    if (!strcasecmp(str, "KP 6"))
      retval = 0x74;
    if (!strcasecmp(str, "KP 7"))
      retval = 0x6C;
    if (!strcasecmp(str, "KP 8"))
      retval = 0x75;
    if (!strcasecmp(str, "KP 9"))
      retval = 0x7D;
    if (!strcasecmp(str, "KP 0"))
      retval = 0x70;
    if (!strcasecmp(str, "KP /"))
      retval = 0x4A;
    if (!strcasecmp(str, "KP *"))
      retval = 0x7C;
    if (!strcasecmp(str, "KP -"))
      retval = 0x7B;
    if (!strcasecmp(str, "KP +"))
      retval = 0x79;
    if (!strcasecmp(str, "KP ."))
      retval = 0x71;
    if (!strcasecmp(str, "SYSRQ"))
      retval = 0x84;
    if (!strcasecmp(str, "WIN MENU"))
      retval = 0x2F;
    if (!strcasecmp(str, "up"))
      retval = 0xF5;
    if (!strcasecmp(str, "down"))
      retval = 0xF2;
    if (!strcasecmp(str, "right"))
      retval = 0xF4;
    if (!strcasecmp(str, "left"))
      retval = 0xEB;
    if (!strcasecmp(str, "1right") || !strcasecmp(str, "3right"))
      retval = 0x02;
    if (!strcasecmp(str, "1up") || !strcasecmp(str, "3up"))
      retval = 0x01;
    if (!strcasecmp(str, "1down") || !strcasecmp(str, "3down"))
      retval = 0x06;
    if (!strcasecmp(str, "1left") || !strcasecmp(str, "3left"))
      retval = 0x04;
    if (!strcasecmp(str, "1sw1") || !strcasecmp(str, "3sw1"))
      retval = 0x03;
    if (!strcasecmp(str, "1sw2") || !strcasecmp(str, "3sw2"))
      retval = 0x08;
    if (!strcasecmp(str, "1sw3") || !strcasecmp(str, "3sw3"))
      retval = 0x05;
    if (!strcasecmp(str, "1sw4") || !strcasecmp(str, "3sw4"))
      retval = 0x0A;
    if (!strcasecmp(str, "1sw5") || !strcasecmp(str, "3sw5"))
      retval = 0x07;
    if (!strcasecmp(str, "1sw6") || !strcasecmp(str, "3sw6"))
      retval = 0x0C;
    if (!strcasecmp(str, "1sw7") || !strcasecmp(str, "3sw7"))
      retval = 0x18;
    if (!strcasecmp(str, "1sw8") || !strcasecmp(str, "3sw8"))
      retval = 0x1A;
    if (!strcasecmp(str, "1start") || !strcasecmp(str, "3start"))
      retval = 0x10;
    if (!strcasecmp(str, "1coin") || !strcasecmp(str, "3coin"))
      retval = 0x14;
    if (!strcasecmp(str, "2right") || !strcasecmp(str, "4right"))
      retval = 0x09;
    if (!strcasecmp(str, "2up") || !strcasecmp(str, "4up"))
      retval = 0x0D;
    if (!strcasecmp(str, "2down") || !strcasecmp(str, "4down"))
      retval = 0x0E;
    if (!strcasecmp(str, "2left") || !strcasecmp(str, "4left"))
      retval = 0x0B;
    if (!strcasecmp(str, "2sw1") || !strcasecmp(str, "4sw1"))
      retval = 0x0F;
    if (!strcasecmp(str, "2sw2") || !strcasecmp(str, "4sw2"))
      retval = 0x11;
    if (!strcasecmp(str, "2sw3") || !strcasecmp(str, "4sw3"))
      retval = 0x13;
    if (!strcasecmp(str, "2sw4") || !strcasecmp(str, "4sw4"))
      retval = 0x15;
    if (!strcasecmp(str, "2sw5") || !strcasecmp(str, "4sw5"))
      retval = 0x17;
    if (!strcasecmp(str, "2sw6") || !strcasecmp(str, "4sw6"))
      retval = 0x19;
    if (!strcasecmp(str, "2sw7") || !strcasecmp(str, "4sw7"))
      retval = 0x1B;
    if (!strcasecmp(str, "2sw8") || !strcasecmp(str, "4sw8"))
      retval = 0x1C;
    if (!strcasecmp(str, "2start") || !strcasecmp(str, "4start"))
      retval = 0x12;
    if (!strcasecmp(str, "2coin") || !strcasecmp(str, "4coin"))
      retval = 0x16;
    if (!strcasecmp(str, "1a"))
      retval = 0x1D;
    if (!strcasecmp(str, "1b"))
      retval = 0x1E;
    if (!strcasecmp(str, "2a"))
      retval = 0x1F;
    if (!strcasecmp(str, "2b"))
      retval = 0x20;
  }

  return retval;
}

bool
updateBoardIPAC (json_object *jobj)
{
  libusb_context *ctx = NULL;
  struct libusb_device_handle *handle = NULL;

  json_object *shiftKey = NULL;
  json_object *pins = NULL;

  int ipac_idx = 4;
  int pos = 0;
  int ret = 0;

  /* divide by 2 the data size (200) for the IPAC2 and Mini-PAC. */
  int div = 2;

  bool result = true;

  char header[4] = {0x50, 0xdd, 0x00, 0x00};
  char data[IPAC_DATA_SIZE];
  unsigned char *mesg = (unsigned char*) malloc (5);
  mesg[0] = IPAC_REPORT;

  handle = openUSB(ctx, IPAC_VENDOR, IPAC_PRODUCT_PRE_2015, IPAC_INTERFACE, 1);

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

  if (pIPAC.ipac56)
  {
    div = 1;
  }

  json_object_object_get_ex(jobj, "pins", &pins);
  populateIPACData(pins, data);

  while (pos < (IPAC_DATA_SIZE/div))
  {
    memcpy(&mesg[1], &data[pos], 4);
    pos+=4;

    ret = libusb_control_transfer(handle,
                                  IPAC_REQUEST_TYPE,
                                  IPAC_REQUEST,
                                  IPAC_VALUE,
                                  IPAC_INDEX,
                                  mesg,
                                  IPAC_MESG_LENGTH,
                                  IPAC_TIMEOUT);
    debug ("Writing out the following data (%i): %x, %x, %x, %x, %x", pos, mesg[0], mesg[1], mesg[2], mesg[3], mesg[4]);
    debug ("Write result: %i", ret);
  }

exit:
  free (mesg);
  closeUSB(ctx, handle, IPAC_INTERFACE);
  return result;

error:
  free (mesg);
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

    /*IPAC2 and IPAC-MINI data */
    if (pIPAC.ipac32)
    {
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

    /* IPAC4 data */
    if (pIPAC.ipac56)
    {
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
}
