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

struct pacLED pLED;

const char* getPacLED64ProductStr ()
{
  return PACLED_PRODUCT_STR;
}

int getPacLED64Version()
{
  return PACLED_VERSION;
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

char decToHex (int decimal)
{
  int retVal = 0;

  switch(decimal)
  {
    case 0:   retVal = 0x00; break;   case 1:   retVal = 0x01; break;   case 2:   retVal = 0x02; break;
    case 3:   retVal = 0x03; break;   case 4:   retVal = 0x04; break;   case 5:   retVal = 0x05; break;
    case 6:   retVal = 0x06; break;   case 7:   retVal = 0x07; break;   case 8:   retVal = 0x08; break;
    case 9:   retVal = 0x09; break;   case 10:  retVal = 0x0A; break;   case 11:  retVal = 0x0B; break;
    case 12:  retVal = 0x0C; break;   case 13:  retVal = 0x0D; break;   case 14:  retVal = 0x0E; break;
    case 15:  retVal = 0x0F; break;   case 16:  retVal = 0x10; break;   case 17:  retVal = 0x11; break;
    case 18:  retVal = 0x12; break;   case 19:  retVal = 0x13; break;   case 20:  retVal = 0x14; break;
    case 21:  retVal = 0x15; break;   case 22:  retVal = 0x16; break;   case 23:  retVal = 0x17; break;
    case 24:  retVal = 0x18; break;   case 25:  retVal = 0x19; break;   case 26:  retVal = 0x1A; break;
    case 27:  retVal = 0x1B; break;   case 28:  retVal = 0x1C; break;   case 29:  retVal = 0x1D; break;
    case 30:  retVal = 0x1E; break;   case 31:  retVal = 0x1F; break;   case 32:  retVal = 0x20; break;
    case 33:  retVal = 0x21; break;   case 34:  retVal = 0x22; break;   case 35:  retVal = 0x23; break;
    case 36:  retVal = 0x24; break;   case 37:  retVal = 0x25; break;   case 38:  retVal = 0x26; break;
    case 39:  retVal = 0x27; break;   case 40:  retVal = 0x28; break;   case 41:  retVal = 0x29; break;
    case 42:  retVal = 0x2A; break;   case 43:  retVal = 0x2B; break;   case 44:  retVal = 0x2C; break;
    case 45:  retVal = 0x2D; break;   case 46:  retVal = 0x2E; break;   case 47:  retVal = 0x2F; break;
    case 48:  retVal = 0x30; break;   case 49:  retVal = 0x31; break;   case 50:  retVal = 0x32; break;
    case 51:  retVal = 0x33; break;   case 52:  retVal = 0x34; break;   case 53:  retVal = 0x35; break;
    case 54:  retVal = 0x36; break;   case 55:  retVal = 0x37; break;   case 56:  retVal = 0x38; break;
    case 57:  retVal = 0x39; break;   case 58:  retVal = 0x3A; break;   case 59:  retVal = 0x3B; break;
    case 60:  retVal = 0x3C; break;   case 61:  retVal = 0x3D; break;   case 62:  retVal = 0x3E; break;
    case 63:  retVal = 0x3F; break;   case 64:  retVal = 0x40; break;   case 65:  retVal = 0x41; break;
    case 66:  retVal = 0x42; break;   case 67:  retVal = 0x43; break;   case 68:  retVal = 0x44; break;
    case 69:  retVal = 0x45; break;   case 70:  retVal = 0x46; break;   case 71:  retVal = 0x47; break;
    case 72:  retVal = 0x48; break;   case 73:  retVal = 0x49; break;   case 74:  retVal = 0x4A; break;
    case 75:  retVal = 0x4B; break;   case 76:  retVal = 0x4C; break;   case 77:  retVal = 0x4D; break;
    case 78:  retVal = 0x4E; break;   case 79:  retVal = 0x4F; break;   case 80:  retVal = 0x50; break;
    case 81:  retVal = 0x51; break;   case 82:  retVal = 0x52; break;   case 83:  retVal = 0x53; break;
    case 84:  retVal = 0x54; break;   case 85:  retVal = 0x55; break;   case 86:  retVal = 0x56; break;
    case 87:  retVal = 0x57; break;   case 88:  retVal = 0x58; break;   case 89:  retVal = 0x59; break;
    case 90:  retVal = 0x5A; break;   case 91:  retVal = 0x5B; break;   case 92:  retVal = 0x5C; break;
    case 93:  retVal = 0x5D; break;   case 94:  retVal = 0x5E; break;   case 95:  retVal = 0x5F; break;
    case 96:  retVal = 0x60; break;   case 97:  retVal = 0x61; break;   case 98:  retVal = 0x62; break;
    case 99:  retVal = 0x63; break;   case 100: retVal = 0x64; break;   case 101: retVal = 0x65; break;
    case 102: retVal = 0x66; break;   case 103: retVal = 0x67; break;   case 104: retVal = 0x68; break;
    case 105: retVal = 0x69; break;   case 106: retVal = 0x6A; break;   case 107: retVal = 0x6B; break;
    case 108: retVal = 0x6C; break;   case 109: retVal = 0x6D; break;   case 110: retVal = 0x6E; break;
    case 111: retVal = 0x6F; break;   case 112: retVal = 0x70; break;   case 113: retVal = 0x71; break;
    case 114: retVal = 0x72; break;   case 115: retVal = 0x73; break;   case 116: retVal = 0x74; break;
    case 117: retVal = 0x75; break;   case 118: retVal = 0x76; break;   case 119: retVal = 0x77; break;
    case 120: retVal = 0x78; break;   case 121: retVal = 0x79; break;   case 122: retVal = 0x7A; break;
    case 123: retVal = 0x7B; break;   case 124: retVal = 0x7C; break;   case 125: retVal = 0x7D; break;
    case 126: retVal = 0x7E; break;   case 127: retVal = 0x7F; break;   case 128: retVal = 0x80; break;
    case 129: retVal = 0x81; break;   case 130: retVal = 0x82; break;   case 131: retVal = 0x83; break;
    case 132: retVal = 0x84; break;   case 133: retVal = 0x85; break;   case 134: retVal = 0x86; break;
    case 135: retVal = 0x87; break;   case 136: retVal = 0x88; break;   case 137: retVal = 0x89; break;
    case 138: retVal = 0x8A; break;   case 139: retVal = 0x8B; break;   case 140: retVal = 0x8C; break;
    case 141: retVal = 0x8D; break;   case 142: retVal = 0x8E; break;   case 143: retVal = 0x8F; break;
    case 144: retVal = 0x90; break;   case 145: retVal = 0x91; break;   case 146: retVal = 0x92; break;
    case 147: retVal = 0x93; break;   case 148: retVal = 0x94; break;   case 149: retVal = 0x95; break;
    case 150: retVal = 0x96; break;   case 151: retVal = 0x97; break;   case 152: retVal = 0x98; break;
    case 153: retVal = 0x99; break;   case 154: retVal = 0x9A; break;   case 155: retVal = 0x9B; break;
    case 156: retVal = 0x9C; break;   case 157: retVal = 0x9D; break;   case 158: retVal = 0x9E; break;
    case 159: retVal = 0x9F; break;   case 160: retVal = 0xA0; break;   case 161: retVal = 0xA1; break;
    case 162: retVal = 0xA2; break;   case 163: retVal = 0xA3; break;   case 164: retVal = 0xA4; break;
    case 165: retVal = 0xA5; break;   case 166: retVal = 0xA6; break;   case 167: retVal = 0xA7; break;
    case 168: retVal = 0xA8; break;   case 169: retVal = 0xA9; break;   case 170: retVal = 0xAA; break;
    case 171: retVal = 0xAB; break;   case 172: retVal = 0xAC; break;   case 173: retVal = 0xAD; break;
    case 174: retVal = 0xAE; break;   case 175: retVal = 0xAF; break;   case 176: retVal = 0xB0; break;
    case 177: retVal = 0xB1; break;   case 178: retVal = 0xB2; break;   case 179: retVal = 0xB3; break;
    case 180: retVal = 0xB4; break;   case 181: retVal = 0xB5; break;   case 182: retVal = 0xB6; break;
    case 183: retVal = 0xB7; break;   case 184: retVal = 0xB8; break;   case 185: retVal = 0xB9; break;
    case 186: retVal = 0xBA; break;   case 187: retVal = 0xBB; break;   case 188: retVal = 0xBC; break;
    case 189: retVal = 0xBD; break;   case 190: retVal = 0xBE; break;   case 191: retVal = 0xBF; break;
    case 192: retVal = 0xC0; break;   case 193: retVal = 0xC1; break;   case 194: retVal = 0xC2; break;
    case 195: retVal = 0xC3; break;   case 196: retVal = 0xC4; break;   case 197: retVal = 0xC5; break;
    case 198: retVal = 0xC6; break;   case 199: retVal = 0xC7; break;   case 200: retVal = 0xC8; break;
    case 201: retVal = 0xC9; break;   case 202: retVal = 0xCA; break;   case 203: retVal = 0xCB; break;
    case 204: retVal = 0xCC; break;   case 205: retVal = 0xCD; break;   case 206: retVal = 0xCE; break;
    case 207: retVal = 0xCF; break;   case 208: retVal = 0xD0; break;   case 209: retVal = 0xD1; break;
    case 210: retVal = 0xD2; break;   case 211: retVal = 0xD3; break;   case 212: retVal = 0xD4; break;
    case 213: retVal = 0xD5; break;   case 214: retVal = 0xD6; break;   case 215: retVal = 0xD7; break;
    case 216: retVal = 0xD8; break;   case 217: retVal = 0xD9; break;   case 218: retVal = 0xDA; break;
    case 219: retVal = 0xDB; break;   case 220: retVal = 0xDC; break;   case 221: retVal = 0xDD; break;
    case 222: retVal = 0xDE; break;   case 223: retVal = 0xDF; break;   case 224: retVal = 0xE0; break;
    case 225: retVal = 0xE1; break;   case 226: retVal = 0xE2; break;   case 227: retVal = 0xE3; break;
    case 228: retVal = 0xE4; break;   case 229: retVal = 0xE5; break;   case 230: retVal = 0xE6; break;
    case 231: retVal = 0xE7; break;   case 232: retVal = 0xE8; break;   case 233: retVal = 0xE9; break;
    case 234: retVal = 0xEA; break;   case 235: retVal = 0xEB; break;   case 236: retVal = 0xEC; break;
    case 237: retVal = 0xED; break;   case 238: retVal = 0xEE; break;   case 239: retVal = 0xEF; break;
    case 240: retVal = 0xF0; break;   case 241: retVal = 0xF1; break;   case 242: retVal = 0xF2; break;
    case 243: retVal = 0xF3; break;   case 244: retVal = 0xF4; break;   case 245: retVal = 0xF5; break;
    case 246: retVal = 0xF6; break;   case 247: retVal = 0xF7; break;   case 248: retVal = 0xF8; break;
    case 249: retVal = 0xF9; break;   case 250: retVal = 0xFA; break;   case 251: retVal = 0xFB; break;
    case 252: retVal = 0xFC; break;   case 253: retVal = 0xFD; break;   case 254: retVal = 0xFE; break;
    case 255: retVal = 0xFF; break;   default:  retVal = 0; break;
  }

  return retVal;
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
      map[0] = decToHex(json_object_get_int(tmp));

      json_object_object_get_ex(led, "intensity", &tmp);
      map[1] = decToHex (json_object_get_int(tmp));

      /* ship this data off to the USB device */
      libusb_control_transfer(handle,
                              PACLED_REQUEST_TYPE,
                              PACLED_REQUEST,
                              PACLED_VALUE,
                              PACLED_INDEX,
                              map,
                              PACLED_MESG_LENGTH,
                              PACLED_TIMEOUT);
    }
  }
  else if (pLED.allIntensities == true)
  {
    map[0] = 0x80; // 128 decimal
    json_object_object_get_ex(jobj, "LED intensity all", &tmp);
    map[1] = decToHex (json_object_get_int(tmp));

    /* ship this data off to the USB device */
    libusb_control_transfer(handle,
                            PACLED_REQUEST_TYPE,
                            PACLED_REQUEST,
                            PACLED_VALUE,
                            PACLED_INDEX,
                            map,
                            PACLED_MESG_LENGTH,
                            PACLED_TIMEOUT);
  }

  /* Random states */
  if (pLED.random == true)
  {
    map[0] = 0x89; // 137 decimal
    map[1] = 0;

    /* ship this data off to the USB device */
    libusb_control_transfer(handle,
                            PACLED_REQUEST_TYPE,
                            PACLED_REQUEST,
                            PACLED_VALUE,
                            PACLED_INDEX,
                            map,
                            PACLED_MESG_LENGTH,
                            PACLED_TIMEOUT);
  }

  /* Fade settings */
  if (pLED.ledMapFade == true)
  {
    json_object_object_get_ex(jobj, "fade", &leds);
    for (idx = 0; idx < json_object_array_length(leds); ++ idx)
    {
      led = json_object_array_get_idx(leds, idx);
      json_object_object_get_ex(led, "led", &tmp);
      map[0] = decToHex((json_object_get_int(tmp) + PACLED_FADE_BASE));

      json_object_object_get_ex(led, "fade", &tmp);
      map[1] = decToHex (json_object_get_int(tmp));

      /* ship this data off to the USB device */
      libusb_control_transfer(handle,
                              PACLED_REQUEST_TYPE,
                              PACLED_REQUEST,
                              PACLED_VALUE,
                              PACLED_INDEX,
                              map,
                              PACLED_MESG_LENGTH,
                              PACLED_TIMEOUT);
    }
  }
  else if (pLED.allFade == true)
  {
    map[0] = 0x40; // 64 decimal
    json_object_object_get_ex(jobj, "LED fade all", &tmp);
    map[1] = decToHex ((json_object_get_int(tmp) + PACLED_FADE_ALL_BASE));

    /* ship this data off to the USB device */
    libusb_control_transfer(handle,
                            PACLED_REQUEST_TYPE,
                            PACLED_REQUEST,
                            PACLED_VALUE,
                            PACLED_INDEX,
                            map,
                            PACLED_MESG_LENGTH,
                            PACLED_TIMEOUT);
  }

  if (pLED.boardIDUpdate == true)
  {
    map[0] = 0xFE;  // 254 decimal
    json_object_object_get_ex(jobj, "new board id", &tmp);
    map[1] = decToHex ((json_object_get_int(tmp) + 240));

    /* ship this data off to the USB device */
    libusb_control_transfer(handle,
                            PACLED_REQUEST_TYPE,
                            PACLED_REQUEST,
                            PACLED_VALUE,
                            PACLED_INDEX,
                            map,
                            PACLED_MESG_LENGTH,
                            PACLED_TIMEOUT);
  }

  exit:
    closeUSB(ctx, handle, PACLED_INTERFACE);
    return result;

  error:
    return result;
}
