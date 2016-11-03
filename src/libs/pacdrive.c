/*
 ============================================================================
 Name        : pacdrive.c
 Author      : Robert Abram, Katie Snow
 Version     :
 Copyright   : Copyright 2014 Robert Abram, Katie Snow
 Description : Ultimarc Pacdrive configuration library
 ============================================================================
 */

/* C */
#include "pacdrive.h"

#include <stdio.h>
#include <string.h>

/* Unix */
#include <json.h>
#include <libusb-1.0/libusb.h>

/* Local */
#include "common.h"
#include "dbg.h"
#include "ulboard.h"

bool isPACDriveConfig(json_object* jobj, ulboard* board)
{
  bool isBoardCfg = false;
  if (board->type == ulboard_type_pacDrive)
  {
    isBoardCfg = validatePacDriveData(jobj);
  }

  return isBoardCfg;
}

bool validatePacDriveData(json_object* jobj)
{
	bool valid = true;

	int idx = 0;

	json_object* leds = NULL;
	json_object* led = NULL;

	if (checkBoardID(jobj, "board id"))
	{
		if (json_object_object_get_ex(jobj, "led", &leds))
		{
			if (json_object_get_type(leds) == json_type_array)
			{
				for (idx = 0; idx < json_object_array_length(leds); ++ idx)
				{
					led = json_object_array_get_idx(leds, idx);
					if (json_object_get_type(led) != json_type_int)
					{
						log_err ("Type invalid.  Valid type is integer");
						valid = false;
					}
				}
			}
			else
			{
				log_err ("'led' is not defined as an array");
				valid = false;
			}
		}
		else
		{
			log_err ("Missing required entry.  Valid entry is 'led'");
			valid = false;
		}
	}
	return valid;
}

int convertLED (int led)
{
	int retVal = led;
	switch (led)
	{
	case  3: retVal = 4;  break; case  4: retVal = 8;  break; case  5: retVal = 16;  break;
	case  6: retVal = 32; break; case  7: retVal = 64; break; case  8: retVal = 128; break;
	case  9: retVal = 1;  break; case 10: retVal = 2;  break; case 11: retVal = 4;   break;
	case 12: retVal = 8;  break; case 13: retVal = 16; break; case 14: retVal = 32;  break;
	case 15: retVal = 64; break; case 16: retVal = 128; break;
	}

	return retVal;
}

bool updateBoardPacDrive (json_object *jobj)
{
  libusb_context *ctx = NULL;
  struct libusb_device_handle *handle = NULL;

  int idx = 0;
  int board = 0;
  int map2 = 0;
  int map3 = 0;
  int total = 0;
  uint16_t product = PACDRIVE_PRODUCT;

  char map[PACDRIVE_DATA_SIZE] = {0,0,0,0};

  bool result = true;

  json_object *leds = NULL;
  json_object *led  = NULL;
  json_object *tmp  = NULL;

  json_object_object_get_ex(jobj, "board id", &tmp);
  board = json_object_get_int(tmp);
  product += (board - 1);

  handle = openUSB(ctx, PACDRIVE_VENDOR, product, PACDRIVE_INTERFACE, 0);

  if (!handle)
  {
    result = false;
    goto error;
  }

  /* populate data */
  json_object_object_get_ex(jobj, "led", &leds);
  for (idx = 0; idx < json_object_array_length(leds); ++ idx)
  {
	  led = json_object_array_get_idx(leds, idx);

	  if (json_object_get_int(led) < 9)
	  {
		  map[3] |= convertLED(json_object_get_int(led)) & 0xFF;
	  }
	  else
	  {
		  map[2] |= convertLED(json_object_get_int(led)) & 0xFF;
	  }
  }

  /* ship this data off to the USB device */
  libusb_control_transfer(handle,
						UM_REQUEST_TYPE,
						UM_REQUEST,
						PACDRIVE_VALUE,
						PACDRIVE_INDEX,
						map,
						PACDRIVE_MESG_LENGTH,
						UM_TIMEOUT);

  if (!handle)
  {
    result = false;
    goto error;
  }

  exit:
    closeUSB(ctx, handle, PACDRIVE_INTERFACE);
    return result;

  error:
    return result;
}
