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
#include "ulboard.h"
#include "common.h"
#include "ipac.h"
#include "ipacseries.h"
#include "dbg.h"

bool isIPACConfig (json_object* jobj, ulboard* board)
{
  bool isBoardCfg = false;

  if (board->type == ulboard_type_ipac2)
  {
    isBoardCfg = validateIPACData(jobj, 32, board);
  }
  else if (board->type == ulboard_type_ipac4)
  {
    isBoardCfg = validateIPAC4Data(jobj, board);
  }
  else if (board->type == ulboard_type_minipac)
  {
    isBoardCfg = validateIPACData(jobj, 32, board);
  }
  else if (board->type == ulboard_type_jpac)
  {
    isBoardCfg = validateIPACData(jobj, 31, board);
  }

  return isBoardCfg;
}

bool validateIPACData(json_object* jobj, int size, ulboard* board)
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
            log_err("pin '%s' has the incorrect number of children entities.", key);
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

  valid = validateIPACMacros(jobj, valid, board);

  return valid;
}

bool validateIPAC4Data (json_object* jobj, ulboard* board)
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

  valid = validateIPACMacros(jobj, valid, board);

  return valid;
}

bool validateIPACMacros(json_object* jobj, bool validState, ulboard* board)
{
  bool valid = validState;

  /* Pre2015 board variables */
  int macroCount = 0;
  static const int maxMacroCount = 4;

  int keyCount = 0;
  static const int maxKeyCount = 4;

  /* 2015 board variables */
  int macroEntries = 0;
  static const int maxMacroEntries = 85;

  int macroCount2015 = 0;
  static const int maxMacroCount2015 = 30;

  /* General variables */
  json_object* macros = NULL;
  json_object* key    = NULL;

  if (json_object_object_get_ex(jobj, "macros", &macros))
  {
    if (!json_object_is_type(macros, json_type_object))
    {
      log_err ("'macros' is not of type object");
      valid = false;
    }
    else
    {
      json_object_object_foreach(macros, name, macro)
      {
        macroCount++;
        macroEntries++;
        macroCount2015++;
        keyCount = 0;

        if (!json_object_is_type(macro, json_type_array))
        {
          log_err ("'%s' is not of type array", name);
          valid = false;
        }
        else
        {
          if (board->version == ulboard_version_pre2015)
          {
            if (json_object_array_length(macro) != maxKeyCount)
            {
              log_err ("'%s' array size should be 4", name);
              valid = false;
            }
            else
            {
              for (; keyCount < maxKeyCount; ++keyCount)
              {
                key = json_object_array_get_idx(macro, keyCount);
                if (!json_object_is_type(key, json_type_string))
                {
                  log_err ("Key entry in '%s' needs to be of type string", name);
                  valid = false;
                }
              }
            }
          }
          else
          {
            macroEntries += json_object_array_length(macro);
            for (; keyCount < json_object_array_length(macro); ++keyCount)
            {
              key = json_object_array_get_idx(macro, keyCount);
              if (!json_object_is_type(key, json_type_string))
              {
                log_err ("Key entry in '%s' needs to be of type string", name);
                valid = false;
              }
            }
          }
        }
      }

      if ((board->version == ulboard_version_pre2015) && (macroCount > maxMacroCount))
      {
        log_err("The number of macros defined is '%i'.  4 macro entries are allowed.", macroCount);
        valid = false;
      }
      else if (board->version == ulboard_version_2015)
      {
        if (macroCount2015 > maxMacroCount2015)
        {
          log_err("The number of macros defined is '%i'. 30 macro entries are allowed.", macroCount2015);
          valid = false;
        }

        if (macroEntries > maxMacroEntries)
        {
          log_err("The total size of the macros plus control characters is '%i'.  Needs to be a total size of 85 or less.", macroEntries);
          valid = false;
        }
      }
    }
  }

  return valid;
}

bool updateBoardIPAC (json_object *jobj, ulboard *board)
{
  bool result = false;
  int bprod = 0;
  unsigned char* barray = NULL;

  switch (board->version)
  {
  case ulboard_version_pre2015:
    if (board->type == ulboard_type_ipac2)
    {
      log_info ("Updating IPAC2 board...");
      barray = calloc(IPAC_SIZE_PRE_2015, sizeof(unsigned char));

      if (barray != NULL)
      {
        updatePre2015IPAC2Board (jobj, barray);
        result = writeIPACSeriesUSB(barray, IPAC_SIZE_PRE_2015,
                                    IPAC_VENDOR_PRE_2015, IPAC_PRODUCT_PRE_2015,
                                    IPAC_INTERFACE, 0, true);
        free(barray);
      }
    }

    if (board->type == ulboard_type_minipac)
    {
      log_info ("Updating MinIPAC board...");
      barray = calloc(IPAC_SIZE_PRE_2015, sizeof(unsigned char));

      if (barray != NULL)
      {
        updatePre2015MINIPACBoard (jobj, barray);
        result = writeIPACSeriesUSB(barray, IPAC_SIZE_PRE_2015,
                                    IPAC_VENDOR_PRE_2015, IPAC_PRODUCT_PRE_2015,
                                    IPAC_INTERFACE, 0, true);
        free(barray);
      }
    }

    if (board->type == ulboard_type_ipac4)
    {
      log_info ("Updating IPAC4 board...");
      barray = calloc((IPAC_SIZE_PRE_2015 * 2), sizeof(unsigned char));

      if (barray != NULL)
      {
        updatePre2015IPAC4Board(jobj, barray);
        result = writeIPACSeriesUSB(barray, (IPAC_SIZE_PRE_2015 * 2),
                                    IPAC_VENDOR_PRE_2015, IPAC_PRODUCT_PRE_2015,
                                    IPAC_INTERFACE, 0, true);

        free(barray);
      }
    }
    break;

  case ulboard_version_2015:
    barray = calloc(IPACSERIES_SIZE, sizeof(unsigned char));

    if (barray != NULL)
    {
      if (board->type == ulboard_type_ipac2)
      {
        log_info ("Updating IPAC2 board...");
        bprod = IPAC_2_PRODUCT;
        update2015IPAC2Board(jobj, barray);
      }

      else if (board->type == ulboard_type_minipac)
      {
        log_info ("Updating MinIPAC board...");
        bprod = IPAC_M_PRODUCT;
        update2015MinIPACBoard(jobj, barray);
      }

      else if (board->type == ulboard_type_ipac4)
      {
        log_info ("Updating IPAC4 board...");
        bprod = IPAC_4_PRODUCT;
        update2015IPAC4Board(jobj, barray);
      }
      else if (board->type == ulboard_type_jpac)
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

void updatePre2015IPAC2Board (json_object *jobj, unsigned char* barray)
{
  json_object *shiftKey = NULL;
  json_object *pins = NULL;
  json_object *macros = NULL;

  /* Header data */
  unsigned char header[4] = {0x50, 0xdd, 0x00, 0x00};
  memcpy (barray, &header, sizeof(header));

  /* Checksum data - Redundant? */
  barray[65] = 0x00;
  /* Control data */
  barray[66] = 0x00;

  json_object_object_get_ex(jobj, "1/2 shift key", &shiftKey);
  barray[4] = convertIPAC(shiftKey);

  json_object_object_get_ex(jobj, "pins", &pins);
  populateBoardArray(PRE_IPAC2_BOARD, pins, &barray[4]);

  /* Macro data */
  json_object_object_get_ex(jobj, "macros", &macros);
  populateMacrosPosition(PRE_IPAC2_BOARD, macros, &barray[4]);
}

void updatePre2015IPAC4Board (json_object *jobj, unsigned char* barray)
{
  json_object *shiftKey = NULL;
  json_object *pins = NULL;
  json_object *macros = NULL;

  /* Header data */
  unsigned char header[4] = {0x50, 0xdd, 0x00, 0x00};
  memcpy (barray, &header, sizeof(header));
  memcpy (&barray[100], &header, sizeof(header));

  /* Control data */
  barray[61] = 0xa6;
  barray[161] = 0xFB;
  barray[162] = 0x01;

  json_object_object_get_ex(jobj, "1/2 shift key", &shiftKey);
  barray[4] = convertIPAC(shiftKey);

  json_object_object_get_ex(jobj, "3/4 shift key", &shiftKey);
  barray[104] = convertIPAC(shiftKey);

  json_object_object_get_ex(jobj, "pins", &pins);
  populateBoardArray(PRE_IPAC4_BOARD, pins, &barray[4]);

  /* Macro data */
  json_object_object_get_ex(jobj, "macros", &macros);
  populateMacrosPosition(PRE_IPAC4_BOARD, macros, &barray[4]);
  populateMacrosPosition(PRE_IPAC4_BOARD, macros, &barray[104]);
}

void updatePre2015MINIPACBoard (json_object *jobj, unsigned char* barray)
{
  json_object *shiftKey = NULL;
  json_object *pins = NULL;
  json_object *macros = NULL;

  /* Header data */
  unsigned char header[4] = {0x50, 0xdd, 0x00, 0x00};
  memcpy (barray, &header, sizeof(header));

  /* Shift key data */
  json_object_object_get_ex(jobj, "1/2 shift key", &shiftKey);
  barray[4] = convertIPAC(shiftKey);

  /* Key data */
  json_object_object_get_ex(jobj, "pins", &pins);
  populateBoardArray(PRE_MINIPAC_BOARD, pins, &barray[4]);

  /* Macro data */
  json_object_object_get_ex(jobj, "macros", &macros);
  populateMacrosPosition(PRE_MINIPAC_BOARD, macros, &barray[4]);
}

void update2015IPAC2Board (json_object *jobj, unsigned char* barray)
{
  json_object *shiftKey = NULL;
  json_object *pins = NULL;
  json_object *macros = NULL;

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

  /* Macro data */
  json_object_object_get_ex(jobj, "macros", &macros);
  populateMacrosPosition(IPAC2_BOARD, macros, &barray[3]);
}

void update2015MinIPACBoard (json_object *jobj, unsigned char* barray)
{
  json_object *shiftKey = NULL;
  json_object *pins = NULL;
  json_object *macros = NULL;

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

  /* Macro data */
  json_object_object_get_ex(jobj, "macros", &macros);
  populateMacrosPosition(MINIPAC_BOARD, macros, &barray[3]);
}

void update2015IPAC4Board (json_object *jobj, unsigned char* barray)
{
  json_object *shiftKey = NULL;
  json_object *pins = NULL;
  json_object *macros = NULL;

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

  /* Macro data */
  json_object_object_get_ex(jobj, "macros", &macros);
  populateMacrosPosition(IPAC4_BOARD, macros, &barray[3]);
}

void update2015JPACBoard (json_object *jobj, unsigned char* barray)
{
  json_object *shiftKey = NULL;
  json_object *pins = NULL;
  json_object *macros = NULL;

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
  memset (&barray[256], 0x59, 1);
  memset (&barray[257], 0xdd, 1);
  memset (&barray[258], 0x0f, 1);

  json_object_object_get_ex(jobj, "1/2 shift key", &shiftKey);
  populateShiftPosition(JPAC_BOARD, shiftKey, &barray[3]);

  json_object_object_get_ex(jobj, "pins", &pins);
  populateBoardArray(JPAC_BOARD, pins, &barray[3]);

  /* Macro data */
  json_object_object_get_ex(jobj, "macros", &macros);
  populateMacrosPosition(JPAC_BOARD, macros, &barray[3]);
}
