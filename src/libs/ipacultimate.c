/*
 Name        : ipacultimate.c
 Author      : Robert Abram, Katie Snow
 Version     :
 Copyright   : Copyright 2014 Robert Abram, Katie Snow
 Description : Ultimarc IPac Ultimate configuration library
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
#include "ipacultimate.h"
#include "dbg.h"

struct ipacultimate pLED;

bool isIPACUltimateConfig(const char* prodStr, int version, json_object* jobj)
{
  bool isBoardCfg = false;

  if (strcmp(prodStr, IPACULTIMATE_PRODUCT_STR) == 0 ||
      strcmp(prodStr, IPACULTIMATE_STR) == 0)
  {
    if (version >= IPACULTIMATE_VERSION_MIN &&
        version <= IPACULTIMATE_VERSION_MAX)
    {
      if (version == 1)
      {
        isBoardCfg = validateIPacUltimateData(jobj);
      }
    }
  }

  return isBoardCfg;
}

const char* getIPacUltimateProductStr ()
{
  return IPACULTIMATE_PRODUCT_STR;
}

int getIPacUltimateVersion()
{
  return IPACULTIMATE_VERSION_MIN;
}

bool validateIPacUltimateData(json_object* jobj)
{
  const char invalidKey = 0x00;
  char data;

  bool valid = false;
  int idx = 0;
  int boardID = 0;
  json_object* tmp = NULL;
  json_object* leds = NULL;
  json_object* led = NULL;
  json_object* pins = NULL;

  pLED.allIntensities = false;
  pLED.ledMapIntensity = false;
  pLED.random = false;
  pLED.boardIDUpdate = false;
  pLED.pins = false;

  if (checkBoardID(jobj, "board id"))
  {
	valid = true;

	/* Figure out what items we have in this file */
	if (json_object_object_get_ex(jobj, "intensity", &leds))
	{
	  if (json_object_get_type(leds) == json_type_array)
	  {
		for (idx = 0; idx < json_object_array_length(leds); ++ idx)
		{
		  led = json_object_array_get_idx(leds, idx);
		  /* required entries */
		  if (!json_object_object_get_ex(led, "led", &tmp) &&
			  !json_object_object_get_ex(led, "intensity", &tmp))
		  {
			log_err ("Missing required entries.  Valid entries 'led' and 'intensity'");
			valid = false;
		  }

		  pLED.ledMapIntensity = true;
		}
	  }
	  else
	  {
		log_err ("'intensity' is not defined as an array");
	  }
	}
	if (json_object_object_get_ex(jobj, "LED intensity all", &tmp))
	{
	  /* can't have the intensity entry with 'LED intensity all' */
	  if (pLED.ledMapIntensity == true)
	  {
		log_err ("'intensity' and 'LED intensity all' are not permitted same configuration file");
		valid = false;
	  }
	  else if (json_object_get_type(tmp) == json_type_int)
	  {
		pLED.allIntensities = true;
	  }
	  else
	  {
		log_err ("'LED intensity all' is not defined as an integer");
		valid = false;
	  }
	}

	if (json_object_object_get_ex(jobj, "LED states random", &tmp))
	{
	  if (json_object_get_type(tmp) == json_type_boolean)
	  {
		pLED.random = json_object_get_boolean(tmp);
	  }
	  else
	  {
		log_err ("'LED states random' is not defined as a boolean");
		valid = false;
	  }
	}

	if (json_object_object_get_ex(jobj, "fade rate", &tmp))
	{
	  if (json_object_get_type(tmp) == json_type_int)
	  {
	    if (json_object_get_int(tmp) >= 0 &&
	        json_object_get_int(tmp) <= 255)
	    {
	      pLED.fadeRate = true;
	    }
	    else
	    {
	      log_err ("'Fade rate' value is not between 0 and 255");
	      valid = false;
	    }
      }
      else
      {
        log_err ("'Fade rate' is not defined as an integer");
        valid = false;
      }
	}

	if (json_object_object_get_ex(jobj, "pins", &tmp))
    {
      valid = true;

	  if (json_object_get_type(tmp) == json_type_array)
	  {
	    if (json_object_array_length(tmp) == 48)
	    {
	      for (idx = 0; idx < json_object_array_length(tmp); ++ idx)
          {
            pins = json_object_array_get_idx(tmp, idx);

	        json_object_object_foreach(pins, key, pin)
	        {
              if (json_object_get_type(pin) == json_type_array)
              {
                if (json_object_get_type(json_object_array_get_idx(pin, 0)) != json_type_string ||
                    json_object_get_type(json_object_array_get_idx(pin, 1)) != json_type_string ||
                    json_object_get_type(json_object_array_get_idx(pin, 2)) != json_type_boolean)
                {
                  log_err ("The pin at index %i is not defined as a string, string, boolean", idx);
                  valid = false;
                }
              }
              else
              {
                log_err ("'pin' object is not defined as an array");
                valid = false;
              }
	        }
	      }
	    }
	    else
	    {
		  log_err ("'pins' array is not the correct size. Size is %i, should be 48", json_object_array_length(tmp));
		  valid = false;
	    }
	  }
	  else
	  {
	    log_err ("'pins' is not defined as an array");
	    valid = false;
	  }

	  if (json_object_object_get_ex(jobj, "x threshold", &tmp))
      {
        if (json_object_get_type(tmp) == json_type_int)
        {
          if (json_object_get_int(tmp) <= 0 ||
              json_object_get_int(tmp) > 128)
          {
            log_err ("'x threshold' value is not between 0 and 127");
            valid = false;
          }
        }
        else
        {
          log_err ("'x threshold' is not defined as an integer");
          valid = false;
        }
      }
      else
      {
        log_err ("'x threshold' is required when pins array is defined");
        valid = false;
      }

	  if (json_object_object_get_ex(jobj, "y threshold", &tmp))
      {
        if (json_object_get_type(tmp) == json_type_int)
        {
          if (json_object_get_int(tmp) <= 0 ||
              json_object_get_int(tmp) >= 128)
          {
            log_err ("'y threshold' value is not between 0 and 127");
            valid = false;
          }
        }
        else
        {
          log_err ("'y threshold' is not defined as an integer");
          valid = false;
        }
      }
      else
      {
        log_err ("'y threshold' is required when pins array is defined");
        valid = false;
      }

	  pLED.pins = true;
    }
  }
  else if (checkBoardID(jobj, "current board id"))
  {
	if (checkBoardID(jobj, "new board id"))
	{
	  pLED.boardIDUpdate = true;
	  valid = true;
	}
	else
	{
	  valid = false;

	  if (!json_object_object_get_ex(jobj, "new board id", &tmp))
	  {
		log_err ("'new board id' is not defined in the configuration file");
	  }
	}
  }
  else
  {
	log_err ("IPAC Ultimate configuration file is not configured correctly");
  }

  return valid;
}

bool
quadAssignmentIPACUltimate (unsigned char* data, unsigned char keyval,
                            int idx1, int idx2, int idx3, int idx4)
{
  bool quadAssigned = true;

  if (data[idx1] == 0x00)
  {
    if (keyval == 0xc0)
    {
      data[idx1] = keyval;
      data[idx2] = 0xc1;
      data[idx3] = 0x01;
      data[idx4] = 0x01;
    }
    else if (keyval == 0xc2)
    {
      data[idx1] = keyval;
      data[idx2] = 0xc3;
      data[idx3] = 0x01;
      data[idx4] = 0x01;
    }
    else if (keyval == 0xc4)
    {
      data[idx1] = keyval;
      data[idx2] = 0xc5;
      data[idx3] = 0x01;
      data[idx4] = 0x01;
    }
    else
    {
      quadAssigned = false;
    }
  }
  else if (data[idx1] != 0xc0 &&
           data[idx1] != 0xc2 &&
           data[idx1] != 0xc4)
  {
    quadAssigned = false;
  }

  return quadAssigned;
}


unsigned char
convertIPACUltimate (json_object *jobj)
{
  unsigned char retval = 0x00;
  const char* str = json_object_get_string(jobj);

  if (strlen(str) > 0)
  {
    if (!strcasecmp(str, "A"))
      retval = 0x04;
    if (!strcasecmp(str, "B"))
      retval = 0x05;
    if (!strcasecmp(str, "C"))
      retval = 0x06;
    if (!strcasecmp(str, "D"))
      retval = 0x07;
    if (!strcasecmp(str, "E"))
      retval = 0x08;
    if (!strcasecmp(str, "F"))
      retval = 0x09;
    if (!strcasecmp(str, "G"))
      retval = 0x0A;
    if (!strcasecmp(str, "H"))
      retval = 0x0B;
    if (!strcasecmp(str, "I"))
      retval = 0x0C;
    if (!strcasecmp(str, "J"))
      retval = 0x0D;
    if (!strcasecmp(str, "K"))
      retval = 0x0E;
    if (!strcasecmp(str, "L"))
      retval = 0x0F;
    if (!strcasecmp(str, "M"))
      retval = 0x10;
    if (!strcasecmp(str, "N"))
      retval = 0x11;
    if (!strcasecmp(str, "O"))
      retval = 0x12;
    if (!strcasecmp(str, "P"))
      retval = 0x13;
    if (!strcasecmp(str, "Q"))
      retval = 0x14;
    if (!strcasecmp(str, "R"))
      retval = 0x15;
    if (!strcasecmp(str, "S"))
      retval = 0x16;
    if (!strcasecmp(str, "T"))
      retval = 0x17;
    if (!strcasecmp(str, "U"))
      retval = 0x18;
    if (!strcasecmp(str, "V"))
      retval = 0x19;
    if (!strcasecmp(str, "W"))
      retval = 0x1A;
    if (!strcasecmp(str, "X"))
      retval = 0x1B;
    if (!strcasecmp(str, "Y"))
      retval = 0x1C;
    if (!strcasecmp(str, "Z"))
      retval = 0x1D;
    if (!strcasecmp(str, "1"))
      retval = 0x1E;
    if (!strcasecmp(str, "2"))
      retval = 0x1F;
    if (!strcasecmp(str, "3"))
      retval = 0x20;
    if (!strcasecmp(str, "4"))
      retval = 0x21;
    if (!strcasecmp(str, "5"))
      retval = 0x22;
    if (!strcasecmp(str, "6"))
      retval = 0x23;
    if (!strcasecmp(str, "7"))
      retval = 0x24;
    if (!strcasecmp(str, "8"))
      retval = 0x25;
    if (!strcasecmp(str, "9"))
      retval = 0x26;
    if (!strcasecmp(str, "0"))
      retval = 0x27;
    if (!strcasecmp(str, "ESC"))
      retval = 0x29;
    if (!strcasecmp(str, "-"))
      retval = 0x2D;
    if (!strcasecmp(str, "="))
      retval = 0x2E;
    if (!strcasecmp(str, "BKSP"))
      retval = 0x2A;
    if (!strcasecmp(str, "TAB"))
      retval = 0x2B;
    if (!strcasecmp(str, "["))
      retval = 0x2F;
    if (!strcasecmp(str, "]"))
      retval = 0x30;
    if (!strcasecmp(str, "ENTER"))
      retval = 0x28;
    if (!strcasecmp(str, "KP ENTER"))
      retval = 0x58;
    if (!strcasecmp(str, "CTRL L"))
      retval = 0x70;
    if (!strcasecmp(str, "CTRL R"))
      retval = 0x74;
    if (!strcasecmp(str, "'"))
      retval = 0x34;
    if (!strcasecmp(str, "`"))
      retval = 0x35;
    if (!strcasecmp(str, "SHIFT L"))
      retval = 0x71;
    if (!strcasecmp(str, "\\"))
      retval = 0x64;
    if (!strcasecmp(str, ","))
      retval = 0x36;
    if (!strcasecmp(str, "."))
      retval = 0x37;
    if (!strcasecmp(str, "/"))
      retval = 0x38;
    if (!strcasecmp(str, "KP /"))
      retval = 0x54;
    if (!strcasecmp(str, "SHIFT R"))
      retval = 0x75;
    if (!strcasecmp(str, "PRNT SCRN"))
      retval = 0x46;
    if (!strcasecmp(str, "ALT L"))
      retval = 0x72;
    if (!strcasecmp(str, "ALT R"))
      retval = 0x76;
    if (!strcasecmp(str, "SPACE"))
      retval = 0x2c;
    if (!strcasecmp(str, "CAPS"))
      retval = 0x39;
    if (!strcasecmp(str, "F1"))
      retval = 0x3A;
    if (!strcasecmp(str, "F2"))
      retval = 0x3B;
    if (!strcasecmp(str, "F3"))
      retval = 0x3C;
    if (!strcasecmp(str, "F4"))
      retval = 0x3D;
    if (!strcasecmp(str, "F5"))
      retval = 0x3E;
    if (!strcasecmp(str, "F6"))
      retval = 0x3F;
    if (!strcasecmp(str, "F7"))
      retval = 0x40;
    if (!strcasecmp(str, "F8"))
      retval = 0x41;
    if (!strcasecmp(str, "F9"))
      retval = 0x42;
    if (!strcasecmp(str, "F10"))
      retval = 0x43;
    if (!strcasecmp(str, "F11"))
      retval = 0x44;
    if (!strcasecmp(str, "F12"))
      retval = 0x45;
    if (!strcasecmp(str, "NUM"))
      retval = 0x53;
    if (!strcasecmp(str, "SCROLL"))
      retval = 0x47;
    if (!strcasecmp(str, "KP 1"))
      retval = 0x59;
    if (!strcasecmp(str, "KP 2"))
      retval = 0x5A;
    if (!strcasecmp(str, "KP 3"))
      retval = 0x5B;
    if (!strcasecmp(str, "KP 4"))
      retval = 0x5C;
    if (!strcasecmp(str, "KP 5"))
      retval = 0x5D;
    if (!strcasecmp(str, "KP 6"))
      retval = 0x5E;
    if (!strcasecmp(str, "KP 7"))
      retval = 0x5F;
    if (!strcasecmp(str, "KP 8"))
      retval = 0x60;
    if (!strcasecmp(str, "KP 9"))
      retval = 0x61;
    if (!strcasecmp(str, "KP 0"))
      retval = 0x62;
    if (!strcasecmp(str, "KP /"))
      retval = 0x54;
    if (!strcasecmp(str, "KP *"))
      retval = 0x55;
    if (!strcasecmp(str, "KP -"))
      retval = 0x56;
    if (!strcasecmp(str, "KP +"))
      retval = 0x57;
    if (!strcasecmp(str, "KP ."))
      retval = 0x63;
    if (!strcasecmp(str, "APP"))
      retval = 0x65;
    if (!strcasecmp(str, "KEYBOARD POWER"))
      retval = 0x66;
    if (!strcasecmp(str, "KP ="))
      retval = 0x67;
    if (!strcasecmp(str, "WIN MENU"))
      retval = 0x77;
    if (!strcasecmp(str, "TRACKBALL X"))
      retval = 0xc0;
    if (!strcasecmp(str, "TRACKBALL Y"))
      retval = 0xc2;
    if (!strcasecmp(str, "TRACKBALL Z"))
      retval = 0xc4;
    if (!strcasecmp(str, "MOUSE LEFT DBL CLICK"))
      retval = 0x80;
    if (!strcasecmp(str, "MOUSE LEFT"))
      retval = 0x81;
    if (!strcasecmp(str, "MOUSE MIDDLE"))
      retval = 0x82;
    if (!strcasecmp(str, "MOUSE RIGHT"))
      retval = 0x83;
    if (!strcasecmp(str, "POWER"))
      retval = 0x88;
    if (!strcasecmp(str, "SLEEP"))
      retval = 0x89;
    if (!strcasecmp(str, "WAKE"))
      retval = 0x8a;
    if (!strcasecmp(str, "VOL UP"))
      retval = 0x8b;
    if (!strcasecmp(str, "VOL DOWN"))
      retval = 0x8c;
    if (!strcasecmp(str, "GAMEPAD 1"))
      retval = 0x90;
    if (!strcasecmp(str, "GAMEPAD 2"))
      retval = 0x91;
    if (!strcasecmp(str, "GAMEPAD 3"))
      retval = 0x92;
    if (!strcasecmp(str, "GAMEPAD 4"))
      retval = 0x93;
    if (!strcasecmp(str, "GAMEPAD 5"))
      retval = 0x94;
    if (!strcasecmp(str, "GAMEPAD 6"))
      retval = 0x95;
    if (!strcasecmp(str, "GAMEPAD 7"))
      retval = 0x96;
    if (!strcasecmp(str, "GAMEPAD 8"))
      retval = 0x97;
    if (!strcasecmp(str, "GAMEPAD 9"))
      retval = 0x98;
    if (!strcasecmp(str, "GAMEPAD 10"))
      retval = 0x99;
    if (!strcasecmp(str, "GAMEPAD 11"))
      retval = 0x9a;
    if (!strcasecmp(str, "GAMEPAD 12"))
      retval = 0x9b;
    if (!strcasecmp(str, "GAMEPAD 13"))
      retval = 0x9c;
    if (!strcasecmp(str, "GAMEPAD 14"))
      retval = 0x9d;
    if (!strcasecmp(str, "GAMEPAD 15"))
      retval = 0x9e;
    if (!strcasecmp(str, "GAMEPAD 16"))
      retval = 0x9f;
    if (!strcasecmp(str, "GAMEPAD 17"))
      retval = 0xa0;
    if (!strcasecmp(str, "GAMEPAD 18"))
      retval = 0xa1;
    if (!strcasecmp(str, "GAMEPAD 19"))
      retval = 0xa2;
    if (!strcasecmp(str, "GAMEPAD 20"))
      retval = 0xa3;
    if (!strcasecmp(str, "GAMEPAD 21"))
      retval = 0xa4;
    if (!strcasecmp(str, "GAMEPAD 22"))
      retval = 0xa5;
    if (!strcasecmp(str, "GAMEPAD 23"))
      retval = 0xa6;
    if (!strcasecmp(str, "GAMEPAD 24"))
      retval = 0xa7;
    if (!strcasecmp(str, "GAMEPAD 25"))
      retval = 0xa8;
    if (!strcasecmp(str, "GAMEPAD 26"))
      retval = 0xa9;
    if (!strcasecmp(str, "GAMEPAD 27"))
      retval = 0xaa;
    if (!strcasecmp(str, "GAMEPAD 28"))
      retval = 0xab;
    if (!strcasecmp(str, "GAMEPAD 29"))
      retval = 0xac;
    if (!strcasecmp(str, "GAMEPAD 30"))
      retval = 0xad;
    if (!strcasecmp(str, "GAMEPAD 31"))
      retval = 0xae;
    if (!strcasecmp(str, "GAMEPAD 32"))
      retval = 0xaf;
    if (!strcasecmp(str, "ANALOG 0"))
      retval = 0xb0;
    if (!strcasecmp(str, "ANALOG 1"))
      retval = 0xb1;
    if (!strcasecmp(str, "ANALOG 2"))
      retval = 0xb2;
    if (!strcasecmp(str, "ANALOG 3"))
      retval = 0xb3;
    if (!strcasecmp(str, "ANALOG 4"))
      retval = 0xb4;
    if (!strcasecmp(str, "ANALOG 5"))
      retval = 0xb5;
    if (!strcasecmp(str, "ANALOG 6"))
      retval = 0xb6;
    if (!strcasecmp(str, "ANALOG 7"))
      retval = 0xb7;
    if (!strcasecmp(str, "HAT 0"))
      retval = 0xba;
    if (!strcasecmp(str, "HAT 1"))
      retval = 0xbb;
    if (!strcasecmp(str, "HAT 2"))
      retval = 0xbc;
    if (!strcasecmp(str, "HAT 3"))
      retval = 0xbd;
  }

  return retval;
}

bool populateIPACUltimateData(json_object* jobj, unsigned char* data)
{
  bool retval = false;

  int idx = 0;
  int keypos = 0;
  int shiftpos = 0;
  int swappos = 0;

  unsigned char keyval;
  char shiftval;
  char swapval;

  json_object *tmp = NULL;

  json_object_object_foreach(jobj, key, pin)
  {
    if (strlen(key) == 0)
    {
      log_err ("\nKey value is empty!  Invalid configuration.\n");
      retval = false;
      continue;
    }

    /* Key value */
    tmp = json_object_array_get_idx(pin, 0);
    keyval = convertIPACUltimate(tmp);

    /* Shift key value */
    tmp = json_object_array_get_idx(pin, 1);
    shiftval = convertIPACUltimate(tmp);

    /* Is a swap key */
    tmp = json_object_array_get_idx(pin, 2);

    swapval = ((!strcasecmp(json_object_get_string(tmp), "true")) > 0 ? 0x41 : 0x01);

    if (!strcasecmp(key, "1right"))
    {
      data[0] = keyval; data[50] = shiftval; data[100] = swapval;
    }
    if (!strcasecmp(key, "1up"))
    {
      data[4] = keyval; data[54] = shiftval; data[104] = swapval;
    }
    if (!strcasecmp(key, "1sw1"))
    {
      data[7] = keyval; data[57] = shiftval; data[107] = swapval;
    }
    if (!strcasecmp(key, "1sw3"))
    {
      data[3] = keyval; data[53] = shiftval; data[103] = swapval;
    }
    if (!strcasecmp(key, "1left"))
    {
      data[2] = keyval; data[52] = shiftval; data[102] = swapval;
    }
    if (!strcasecmp(key, "1down"))
    {
      data[6] = keyval; data[56] = shiftval; data[106] = swapval;
    }
    if (!strcasecmp(key, "1sw2"))
    {
      data[5] = keyval; data[55] = shiftval; data[105] = swapval;
    }
    if (!strcasecmp(key, "1sw4"))
    {
      data[1] = keyval; data[51] = shiftval; data[101] = swapval;
    }
    if (!strcasecmp(key, "1sw5"))
    {
      data[23] = keyval; data[73] = shiftval; data[121] = swapval;
    }
    if (!strcasecmp(key, "1sw7"))
    {
      data[19] = keyval; data[69] = shiftval; data[117] = swapval;
    }
    if (!strcasecmp(key, "1start"))
    {
      data[39] = keyval; data[89] = shiftval; data[137] = swapval;
    }
    if (!strcasecmp(key, "1coin"))
    {
      data[35] = keyval; data[85] = shiftval; data[133] = swapval;
    }
    if (!strcasecmp(key, "1sw6"))
    {
      data[21] = keyval; data[71] = shiftval; data[119] = swapval;
    }
    if (!strcasecmp(key, "1sw8"))
    {
      data[17] = keyval; data[67] = shiftval; data[115] = swapval;
    }
    if (!strcasecmp(key, "2start"))
    {
      data[37] = keyval; data[87] = shiftval; data[135] = swapval;
    }
    if (!strcasecmp(key, "2coin"))
    {
      data[33] = keyval; data[88] = shiftval; data[131] = swapval;
    }
    if (!strcasecmp(key, "2right"))
    {
      data[31] = keyval; data[81] = shiftval; data[128] = swapval;
    }
    if (!strcasecmp(key, "2up"))
    {
      data[27] = keyval; data[77] = shiftval; data[125] = swapval;
    }
    if (!strcasecmp(key, "2sw1"))
    {
      data[47] = keyval; data[97] = shiftval; data[145] = swapval;
    }
    if (!strcasecmp(key, "2sw3"))
    {
      data[43] = keyval; data[93] = shiftval; data[141] = swapval;
    }
    if (!strcasecmp(key, "2left"))
    {
      data[29] = keyval; data[79] = shiftval; data[127] = swapval;
    }
    if (!strcasecmp(key, "2down"))
    {
      data[25] = keyval; data[75] = shiftval; data[123] = swapval;
    }
    if (!strcasecmp(key, "2sw2"))
    {
      data[45] = keyval; data[95] = shiftval; data[143] = swapval;
    }
    if (!strcasecmp(key, "2sw4"))
    {
      data[41] = keyval; data[91] = shiftval; data[138] = swapval;
    }
    if (!strcasecmp(key, "2sw5"))
    {
      data[11] = keyval; data[61] = shiftval; data[111] = swapval;
    }
    if (!strcasecmp(key, "2sw7"))
    {
      data[48] = keyval; data[98] = shiftval; data[146] = swapval;
    }
    if (!strcasecmp(key, "1a"))
    {
      data[12] = keyval; data[62] = shiftval; data[112] = swapval;
    }
    if (!strcasecmp(key, "2a"))
    {
      data[40] = keyval; data[90] = shiftval; data[138] = swapval;
    }
    if (!strcasecmp(key, "2sw6"))
    {
      data[49] = keyval; data[99] = shiftval; data[147] = swapval;
    }
    if (!strcasecmp(key, "2sw8"))
    {
      data[10] = keyval; data[60] = shiftval; data[110] = swapval;
    }
    if (!strcasecmp(key, "1b"))
    {
      data[14] = keyval; data[64] = shiftval; data[112] = swapval;
    }
    if (!strcasecmp(key, "2b"))
    {
      data[42] = keyval; data[92] = shiftval; data[140] = swapval;
    }
    if (!strcasecmp(key, "3rt"))
    {
      if (!quadAssignmentIPACUltimate(data, keyval, 22, 20, 118, 120))
      {
        data[22] = keyval; data[72] = shiftval; data[120] = swapval;
      }
    }
    if (!strcasecmp(key, "3up"))
    {
      if (!quadAssignmentIPACUltimate(data, keyval, 16, 18, 114, 116))
      {
        data[16] = keyval; data[66] = shiftval; data[114] = swapval;
      }
    }
    if (!strcasecmp(key, "3s1"))
    {
      if (!quadAssignmentIPACUltimate(data, keyval, 36, 38, 134, 136))
      {
        data[36] = keyval; data[86] = shiftval; data[134] = swapval;
      }
    }
    if (!strcasecmp(key, "3s3"))
    {
      if (!quadAssignmentIPACUltimate(data, keyval, 32, 34, 130, 132))
      {
        data[32] = keyval; data[82] = shiftval; data[130] = swapval;
      }
    }
    if (!strcasecmp(key, "3lf"))
    {
      if (!quadAssignmentIPACUltimate(data, keyval, 22, 20, 118, 120))
      {
        data[22] = keyval; data[72] = shiftval; data[120] = swapval;
      }
    }
    if (!strcasecmp(key, "3dn"))
    {
      if (!quadAssignmentIPACUltimate(data, keyval, 16, 18, 114, 116))
      {
        data[18] = keyval; data[68] = shiftval; data[116] = swapval;
      }
    }
    if (!strcasecmp(key, "3s2"))
    {
      if (!quadAssignmentIPACUltimate(data, keyval, 36, 38, 134, 136))
      {
        data[38] = keyval; data[88] = shiftval; data[136] = swapval;
      }
    }
    if (!strcasecmp(key, "3s4"))
    {
      if (!quadAssignmentIPACUltimate(data, keyval, 32, 34, 130, 132))
      {
        data[34] = keyval; data[84] = shiftval; data[132] = swapval;
      }
    }
    if (!strcasecmp(key, "4rt"))
    {
      if (!quadAssignmentIPACUltimate(data, keyval, 9, 8, 109, 108))
      {
        data[9] = keyval; data[59] = shiftval; data[109] = swapval;
      }
    }
    if (!strcasecmp(key, "4up"))
    {
      data[30] = keyval; data[80] = shiftval; data[128] = swapval;
    }
    if (!strcasecmp(key, "4s1"))
    {
      data[26] = keyval; data[76] = shiftval; data[124] = swapval;
    }
    if (!strcasecmp(key, "4s3"))
    {
      data[46] = keyval; data[96] = shiftval; data[144] = swapval;
    }
    if (!strcasecmp(key, "4lf"))
    {
      if (!quadAssignmentIPACUltimate(data, keyval, 9, 8, 109, 108))
      {
        data[8] = keyval; data[58] = shiftval; data[108] = swapval;
      }
    }
    if (!strcasecmp(key, "4dn"))
    {
      data[28] = keyval; data[78] = shiftval; data[126] = swapval;
    }
    if (!strcasecmp(key, "4s2"))
    {
      data[24] = keyval; data[74] = shiftval; data[122] = swapval;
    }
    if (!strcasecmp(key, "4s4"))
    {
      data[44] = keyval; data[94] = shiftval; data[142] = swapval;
    }
  }

  return retval;
}

bool updateBoardIPacUltimate(json_object* jobj)
{
  libusb_context *ctx = NULL;
  struct libusb_device_handle *handle = NULL;

  int ipac_idx = 4;
  int pos = 0;
  int ret = 0;

  int idx = 0;
  int intensity = 0;
  int board = 0;
  uint16_t product = IPACULTIMATE_PRODUCT;

  unsigned char data[IPACULTIMATE_DATA_SIZE];
  char header[4] = {0x50, 0xdd, 0x18, 0x00};
  unsigned char map[IPACULTIMATE_MESG_LENGTH] = {0x03,0,0,0,0};

  bool result = true;

  json_object *leds = NULL;
  json_object *led  = NULL;
  json_object *tmp  = NULL;
  json_object *pins = NULL;
  json_object *pin  = NULL;

  if (pLED.boardIDUpdate == true)
  {
	json_object_object_get_ex(jobj, "current board id", &tmp);
	board = json_object_get_int(tmp);
	product += (board - 1);
  }
  else
  {
	json_object_object_get_ex(jobj, "board id", &tmp);
	board = json_object_get_int(tmp);
	product += (board - 1);
  }

  handle = openUSB(ctx, IPACULTIMATE_VENDOR, product, IPACULTIMATE_INTERFACE, 1);

  if (!handle)
  {
	result = false;
	goto error;
  }

  /* Intensity settings */
  if (pLED.ledMapIntensity == true)
  {
	json_object_object_get_ex(jobj, "intensity", &leds);
	for (idx = 0; idx < json_object_array_length(leds); ++ idx)
	{
	  led = json_object_array_get_idx(leds, idx);
	  json_object_object_get_ex(led, "led", &tmp);
	  map[1] = convertDecimalToHex(json_object_get_int(tmp));

	  json_object_object_get_ex(led, "intensity", &tmp);
	  map[2] = convertDecimalToHex (json_object_get_int(tmp));

	  /* ship this data off to the USB device */
	  libusb_control_transfer(handle,
							  IPACULTIMATE_REQUEST_TYPE,
							  IPACULTIMATE_REQUEST,
							  IPACULTIMATE_VALUE,
							  IPACULTIMATE_INTERFACE,
							  map,
							  IPACULTIMATE_MESG_LENGTH,
							  IPACULTIMATE_TIMEOUT);
	}
  }
  else if (pLED.allIntensities == true)
  {
	map[1] = 0x80; // 128 decimal
	json_object_object_get_ex(jobj, "LED intensity all", &tmp);
	map[2] = convertDecimalToHex (json_object_get_int(tmp));

	/* ship this data off to the USB device */
	libusb_control_transfer(handle,
							IPACULTIMATE_REQUEST_TYPE,
							IPACULTIMATE_REQUEST,
							IPACULTIMATE_VALUE,
							IPACULTIMATE_INTERFACE,
							map,
							IPACULTIMATE_MESG_LENGTH,
							IPACULTIMATE_TIMEOUT);
  }

  /* Random states */
  if (pLED.random == true)
  {
	map[1] = 0x89; // 137 decimal
	map[2] = 0;

	/* ship this data off to the USB device */
	libusb_control_transfer(handle,
							IPACULTIMATE_REQUEST_TYPE,
							IPACULTIMATE_REQUEST,
							IPACULTIMATE_VALUE,
							IPACULTIMATE_INTERFACE,
							map,
							IPACULTIMATE_MESG_LENGTH,
							IPACULTIMATE_TIMEOUT);
  }

  /* Fade rate */
  if (pLED.fadeRate == true)
  {
	map[1] = 0xc0; // 192 decimal
	json_object_object_get_ex(jobj, "fade rate", &tmp);
	map[2] = convertDecimalToHex (json_object_get_int(tmp));

	/* ship this data off to the USB device */
	libusb_control_transfer(handle,
							IPACULTIMATE_REQUEST_TYPE,
							IPACULTIMATE_REQUEST,
							IPACULTIMATE_VALUE,
							IPACULTIMATE_INTERFACE,
							map,
							IPACULTIMATE_MESG_LENGTH,
							IPACULTIMATE_TIMEOUT);
  }

  if (pLED.pins == true)
  {
    /* Setup data to send to board */
     memset (&data, 0, sizeof(data));
     memcpy (&data, &header, sizeof(header));
     data[13] = 0xff;
     data[15] = 0xff;
     data[63] = 0xff;
     data[65] = 0xff;

     json_object_object_get_ex(jobj, "pins", &pins);

     for (idx = 0; idx < json_object_array_length(pins); ++ idx)
     {
       pin = json_object_array_get_idx(pins, idx);
       populateIPACUltimateData(pin, data);
       ++ipac_idx;
     }

     json_object_object_get_ex(jobj, "x threshold", &tmp);
     data[155] = convertDecimalToHex(json_object_get_int(tmp));

     json_object_object_get_ex(jobj, "y threshold", &tmp);
     data[163] = convertDecimalToHex(json_object_get_int(tmp));

     while (pos < IPACULTIMATE_DATA_SIZE)
     {
       memcpy(&map[1], &data[pos], 4);
       pos+=4;

       debug ("Writing out the following data (%i): %x, %x, %x, %x, %x", pos, map[0], map[1], map[2], map[3], map[4]);
       ret = libusb_control_transfer(handle,
                                     IPACULTIMATE_REQUEST_TYPE,
                                     IPACULTIMATE_REQUEST,
                                     IPACULTIMATE_VALUE,
                                     IPACULTIMATE_INTERFACE,
                                     map,
                                     IPACULTIMATE_MESG_LENGTH,
                                     IPACULTIMATE_TIMEOUT);
       debug ("Write result: %i", ret);
     }
  }

  if (pLED.boardIDUpdate == true)
  {
    /* THIS IS NOT WORKING CORRECTLY */
    memcpy (&map[1], &header, sizeof(header));
	json_object_object_get_ex(jobj, "new board id", &tmp);
	map[1] = convertDecimalToHex((json_object_get_int(tmp) + 80));

	/* ship this data off to the USB device */
	ret = libusb_control_transfer(handle,
							IPACULTIMATE_REQUEST_TYPE,
							IPACULTIMATE_REQUEST,
							IPACULTIMATE_VALUE,
							IPACULTIMATE_INTERFACE,
							map,
							IPACULTIMATE_MESG_LENGTH,
							IPACULTIMATE_TIMEOUT);
  }

  exit:
	closeUSB(ctx, handle, IPACULTIMATE_INTERFACE);
	return result;

  error:
	return result;
}
