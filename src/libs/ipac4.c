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
  bool valid = false;

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

char convertIPAC4 (json_object *jobj)
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

bool updateBoardIPAC4 (json_object *jobj)
{
  bool result = false;

  return result;
}

bool updatePre2015BoardIPAC4 (json_object *jobj)
{
  bool result = false;

  return result;
}

bool update2015BoardIPAC4 (json_object *jobj)
{
  bool result = false;

  return result;
}

void populateIPAC4Data (json_object* jobj, unsigned char* data)
{

}
