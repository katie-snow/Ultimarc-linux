/*
 ============================================================================
 Name        : pacLED.c
 Author      : Robert Abram, Katie Snow
 Version     :
 Copyright   : Copyright 2014 Robert Abram, Katie Snow
 Description : Ultimarc PacLED64 configuration library
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
#include "pacLED.h"
#include "dbg.h"
#include "ulboard.h"

struct pacLED pLED;

bool isPACLED64Config(json_object* jobj, ulboard* board)
{
  bool isBoardCfg = false;

  if (board->type == ulboard_type_pacLED)
  {
    isBoardCfg = validatePacLED64Data(jobj);
  }

  return isBoardCfg;
}

bool validatePacLED64Data(json_object* jobj)
{
  bool valid = false;
  int idx = 0;
  int boardID = 0;
  json_object* tmp = NULL;
  json_object* leds = NULL;
  json_object* led = NULL;

  pLED.allIntensities = false;
  pLED.ledMapIntensity = false;
  pLED.allFade = false;
  pLED.ledMapFade = false;
  pLED.random = false;
  pLED.boardIDUpdate = false;

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

    if (json_object_object_get_ex(jobj, "fade", &leds))
    {
      if (json_object_get_type(leds) == json_type_array)
      {
        for (idx = 0; idx < json_object_array_length(leds); ++ idx)
        {
          led = json_object_array_get_idx(leds, idx);
          /* required entries */
          if (!json_object_object_get_ex(led, "led", &tmp) &&
              !json_object_object_get_ex(led, "fade", &tmp))
          {
            log_err ("Missing required entries.  Valid entries 'led' and 'fade' both are type integers");
            valid = false;
          }

          pLED.ledMapFade = true;
        }
      }
      else
      {
        log_err ("'fade' is not defined as an array");
      }
    }
    if (json_object_object_get_ex(jobj, "LED fade all", &tmp))
    {
      /* can't have the fade entry with 'LED fade all' */
      if (pLED.ledMapFade == true)
      {
        log_err ("'fade' and 'LED fade all' are not permitted same configuration file");
        valid = false;
      }
      else if (json_object_get_type(tmp) == json_type_int)
      {
        pLED.allFade = true;
      }
      else
      {
        log_err ("'LED fade all' is not defined as an integer");
        valid = false;
      }
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
    log_err ("PAC LED configuration file is not configured correctly");
  }

  return valid;
}

bool updateBoardPacLED (json_object *jobj)
{
  libusb_context *ctx = NULL;
  struct libusb_device_handle *handle = NULL;

  int idx = 0;
  int intensity = 0;
  int board = 0;
  uint16_t product = PACLED_PRODUCT;

  char map[PACLED_DATA_SIZE] = {0,0};

  bool result = true;

  json_object *leds = NULL;
  json_object *led  = NULL;
  json_object *tmp  = NULL;

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

  handle = openUSB(ctx, PACLED_VENDOR, product, PACLED_INTERFACE, 1);

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
      map[0] = convertDecimalToHex(json_object_get_int(tmp));

      json_object_object_get_ex(led, "intensity", &tmp);
      map[1] = convertDecimalToHex (json_object_get_int(tmp));

      /* ship this data off to the USB device */
      libusb_control_transfer(handle,
                              UM_REQUEST_TYPE,
                              UM_REQUEST,
                              PACLED_VALUE,
                              PACLED_INDEX,
                              map,
                              PACLED_MESG_LENGTH,
                              UM_TIMEOUT);
    }
  }
  else if (pLED.allIntensities == true)
  {
    map[0] = 0x80; // 128 decimal
    json_object_object_get_ex(jobj, "LED intensity all", &tmp);
    map[1] = convertDecimalToHex (json_object_get_int(tmp));

    /* ship this data off to the USB device */
    libusb_control_transfer(handle,
                            UM_REQUEST_TYPE,
                            UM_REQUEST,
                            PACLED_VALUE,
                            PACLED_INDEX,
                            map,
                            PACLED_MESG_LENGTH,
                            UM_TIMEOUT);
  }

  /* Random states */
  if (pLED.random == true)
  {
    map[0] = 0x89; // 137 decimal
    map[1] = 0;

    /* ship this data off to the USB device */
    libusb_control_transfer(handle,
                            UM_REQUEST_TYPE,
                            UM_REQUEST,
                            PACLED_VALUE,
                            PACLED_INDEX,
                            map,
                            PACLED_MESG_LENGTH,
                            UM_TIMEOUT);
  }

  /* Fade settings */
  if (pLED.ledMapFade == true)
  {
    json_object_object_get_ex(jobj, "fade", &leds);
    for (idx = 0; idx < json_object_array_length(leds); ++ idx)
    {
      led = json_object_array_get_idx(leds, idx);
      json_object_object_get_ex(led, "led", &tmp);
      map[0] = convertDecimalToHex((json_object_get_int(tmp) + PACLED_FADE_BASE));

      json_object_object_get_ex(led, "fade", &tmp);
      map[1] = convertDecimalToHex (json_object_get_int(tmp));

      /* ship this data off to the USB device */
      libusb_control_transfer(handle,
                              UM_REQUEST_TYPE,
                              UM_REQUEST,
                              PACLED_VALUE,
                              PACLED_INDEX,
                              map,
                              PACLED_MESG_LENGTH,
                              UM_TIMEOUT);
    }
  }
  else if (pLED.allFade == true)
  {
    map[0] = 0x40; // 64 decimal
    json_object_object_get_ex(jobj, "LED fade all", &tmp);
    map[1] = convertDecimalToHex ((json_object_get_int(tmp) + PACLED_FADE_ALL_BASE));

    /* ship this data off to the USB device */
    libusb_control_transfer(handle,
                            UM_REQUEST_TYPE,
                            UM_REQUEST,
                            PACLED_VALUE,
                            PACLED_INDEX,
                            map,
                            PACLED_MESG_LENGTH,
                            UM_TIMEOUT);
  }

  if (pLED.boardIDUpdate == true)
  {
    map[0] = 0xFE;  // 254 decimal
    json_object_object_get_ex(jobj, "new board id", &tmp);
    map[1] = convertDecimalToHex ((json_object_get_int(tmp) + 240));

    /* ship this data off to the USB device */
    libusb_control_transfer(handle,
                            UM_REQUEST_TYPE,
                            UM_REQUEST,
                            PACLED_VALUE,
                            PACLED_INDEX,
                            map,
                            PACLED_MESG_LENGTH,
                            UM_TIMEOUT);
  }

  exit:
    closeUSB(ctx, handle, PACLED_INTERFACE);
    return result;

  error:
    return result;
}
