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
  pIPAC.ipac2 = (strcmp(prodStr, IPAC_STR_2) == 0);
  pIPAC.minipac = (strcmp(prodStr, IPAC_STR_M) == 0);
  pIPAC.ipac4 = (strcmp(prodStr, IPAC_STR_4) == 0);
  pIPAC.jpac = (strcmp(prodStr, JPAC_STR) == 0);

/* TODO #7 split if statement for minipac and ipac2
 * size is 28 for IPAC2, size 32 for Minipac
 */
  if (pIPAC.ipac2 || pIPAC.minipac)
  {
    isBoardCfg = validateIPACData(jobj, 32);
  }
  else if (pIPAC.ipac4)
  {
    isBoardCfg = validateIPAC4Data(jobj);
  }
  else if (pIPAC.jpac)
  {
    isBoardCfg = validateIPACData(jobj, 30);
  }

  return isBoardCfg;
}

bool validateIPACData(json_object* jobj, int size)
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

      if (pinCount != size)
      {
        log_err("Incorrect number of pin objects.  Needs to be %i entries.", size);
        valid = false;
      }
    }
  }

  return valid;
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

bool updateBoardIPAC (json_object *jobj)
{
  bool result = false;
  int bprod = 0;
  unsigned char* barray = NULL;

  switch (pIPAC.version)
  {
  case 1:
    /* TODO #7 split if statement.  Duplicate internal if code */
    if (pIPAC.ipac2 || pIPAC.minipac)
    {
      log_info ("Updating IPAC2/MinIPAC board...");
      barray = calloc(IPAC_SIZE_PRE_2015, sizeof(unsigned char));

      if (barray != NULL)
      {
      /* TODO #7 Update function name and create new function for the split*/
        updatePre2015IPACBoard (jobj, barray);
        result = writeIPACSeriesUSB(barray, IPAC_SIZE_PRE_2015,
                                    IPAC_VENDOR_PRE_2015, IPAC_PRODUCT_PRE_2015,
                                    IPAC_INTERFACE, 1, true);
        free(barray);
      }
    }

    if (pIPAC.ipac4)
    {
      log_info ("Updating IPAC4 board...");
      barray = calloc((IPAC_SIZE_PRE_2015 * 2), sizeof(unsigned char));

      if (barray != NULL)
      {
        updatePre2015IPAC4Board(jobj, barray);
        log_info ("%i", barray[5]);
        result = writeIPACSeriesUSB(barray, (IPAC_SIZE_PRE_2015 * 2),
                                    IPAC_VENDOR_PRE_2015, IPAC_PRODUCT_PRE_2015,
                                    IPAC_INTERFACE, 1, true);

        free(barray);
      }
    }
    break;

  case 2:
    barray = calloc(IPACSERIES_SIZE, sizeof(unsigned char));

    if (barray != NULL)
    {
      if (pIPAC.ipac2)
      {
        log_info ("Updating IPAC2 board...");
        bprod = IPAC_2_PRODUCT;
        update2015IPAC2Board(jobj, barray);
      }

      else if (pIPAC.minipac)
      {
        log_info ("Updating MinIPAC board...");
        bprod = IPAC_M_PRODUCT;
        update2015MinIPACBoard(jobj, barray);
      }

      else if (pIPAC.ipac4)
      {
        log_info ("Updating IPAC4 board...");
        bprod = IPAC_4_PRODUCT;
        update2015IPAC4Board(jobj, barray);
      }
      else if (pIPAC.jpac)
      {
        log_info ("Updating JPAC board...");
        bprod = JPAC_PRODUCT;
        update2015JPACBoard(jobj, barray);
      }

      if (bprod != 0)
      {
        result = writeIPACSeriesUSB(barray, IPACSERIES_SIZE, IPAC_VENDOR_2015,
                                    bprod, IPACSERIES_INTERFACE, 1, true);
      }

      free(barray);
    }
    break;
  }

  return result;
}

/* TODO #7 Update function name */
void updatePre2015IPACBoard (json_object *jobj, unsigned char* barray)
{
  json_object *shiftKey = NULL;
  json_object *pins = NULL;

  /* Header data */
  unsigned char header[4] = {0x50, 0xdd, 0x00, 0x00};
  memcpy (barray, &header, sizeof(header));

  /* Macro data */
  barray[69] = 0x30;

  json_object_object_get_ex(jobj, "1/2 shift key", &shiftKey);
  barray[4] = convertIPAC(shiftKey);

  json_object_object_get_ex(jobj, "pins", &pins);
  populateBoardArray(PRE_IPAC2_BOARD, pins, &barray[4]);
}

/* TODO #7 Create new function for MinIPAC enumeration */

void updatePre2015IPAC4Board (json_object *jobj, unsigned char* barray)
{
  json_object *shiftKey = NULL;
  json_object *pins = NULL;

  /* Header data */
  unsigned char header[4] = {0x50, 0xdd, 0x00, 0x00};
  memcpy (barray, &header, sizeof(header));
  memcpy (&barray[100], &header, sizeof(header));

  /* Macro data */
  barray[61] = 0x30;
  barray[161] = 0xFB;
  barray[162] = 0x01;

  json_object_object_get_ex(jobj, "1/2 shift key", &shiftKey);
  barray[4] = convertIPAC(shiftKey);

  json_object_object_get_ex(jobj, "3/4 shift key", &shiftKey);
  barray[104] = convertIPAC(shiftKey);

  json_object_object_get_ex(jobj, "pins", &pins);
  populateBoardArray(PRE_IPAC4_BOARD, pins, &barray[4]);
}

void update2015IPAC2Board (json_object *jobj, unsigned char* barray)
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
  memset (&barray[4], 0xff, 32);
  memset (&barray[40], 0xff, 28);
  memset (&barray[104], 1, 52);
  memset (&barray[158], 0x7f, 8);
  memset (&barray[166], 0x10, 8);

  json_object_object_get_ex(jobj, "1/2 shift key", &shiftKey);
  populateShiftPosition(MINIPAC_BOARD, shiftKey, &barray[3]);

  json_object_object_get_ex(jobj, "pins", &pins);
  populateBoardArray(MINIPAC_BOARD, pins, &barray[3]);
}

void update2015IPAC4Board (json_object *jobj, unsigned char* barray)
{
  json_object *shiftKey = NULL;
  json_object *pins = NULL;

  /* Header data */
  char header[4] = {0x50, 0xdd, 0x0f, 0x00};
  memcpy (barray, &header, sizeof(header));

  /* Setup data to send to board */
  memset (&barray[132], 1, 24);
  memset (&barray[158], 1, 14);
  memset (&barray[176], 1, 1);
  memset (&barray[178], 1, 18);

  json_object_object_get_ex(jobj, "1/2 shift key", &shiftKey);
  populateShiftPosition(IPAC4_BOARD, shiftKey, &barray[3]);

  json_object_object_get_ex(jobj, "3/4 shift key", &shiftKey);
  populateShiftPosition(IPAC4_BOARD, shiftKey, &barray[3]);

  json_object_object_get_ex(jobj, "pins", &pins);
  populateBoardArray(IPAC4_BOARD, pins, &barray[3]);
}

void update2015JPACBoard (json_object *jobj, unsigned char* barray)
{
  json_object *shiftKey = NULL;
  json_object *pins = NULL;

  /* Header data */
  char header[4] = {0x50, 0xdd, 0x0f, 0x00};
  memcpy (barray, &header, sizeof(header));

  /* Setup data to send to board */
  memset (&barray[4], 0xff, 50);
  memset (&barray[14], 0, 1);
  memset (&barray[104], 1, 1);
  memset (&barray[106], 1, 1);
  memset (&barray[108], 1, 1);
  memset (&barray[110], 1, 1);
  memset (&barray[116], 1, 1);
  memset (&barray[118], 1, 1);
  memset (&barray[120], 1, 5);
  memset (&barray[128], 1, 1);
  memset (&barray[130], 1, 1);
  memset (&barray[132], 1, 13);
  memset (&barray[146], 1, 1);
  memset (&barray[148], 1, 1);
  memset (&barray[150], 1, 1);
  memset (&barray[154], 0x0a, 1);
  memset (&barray[155], 0x80, 7);
  memset (&barray[162], 0x0a, 1);
  memset (&barray[163], 0x10, 7);

  json_object_object_get_ex(jobj, "1/2 shift key", &shiftKey);
  populateShiftPosition(JPAC_BOARD, shiftKey, &barray[3]);

  json_object_object_get_ex(jobj, "pins", &pins);
  populateBoardArray(JPAC_BOARD, pins, &barray[3]);
}

void populateIPAC2Data(json_object* jobj, unsigned char* data)
{
  int keypos = 0;
  int shiftpos = 0;

  unsigned char keyval;
  char shiftval;

  json_object *tmp = NULL;


/* TODO #7 remove commented code. */
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

/* TODO #7 Remove unused function  */
void populateMinIPACData(json_object* jobj, unsigned char* data)
{

}
