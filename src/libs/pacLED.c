/*
 * pacLED.c
 *
 *  Created on: Mar 23, 2014
 *      Author: katie
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

struct pacLED pLED;

bool isPacLED (json_object* jobj)
{
  int idx = 0;

  json_object* tmp = NULL;

  json_object* leds = NULL;
  json_object* led = NULL;

  pLED.allIntensities = false;
  pLED.allStates = false;
  pLED.ledMapIntensity = false;
  pLED.ledMapState = false;
  pLED.random = false;

  if (json_object_object_get_ex(jobj, "version", &tmp))
  {
    if (json_object_get_int(tmp) == PACLED_VERSION)
    {
      if (json_object_object_get_ex(jobj, "board id", &tmp)
          && json_object_object_get_ex(jobj, "product", &tmp))
      {
        /* product needs to be last so we can do this check */
        if (strncmp(json_object_get_string(tmp), PACLED_PRODUCT_STR, 4) == 0)
        {
          /* Figure out what items we have in this file */
          if (json_object_object_get_ex(jobj, "intensity", &leds))
          {
            for (idx = 0; idx < json_object_array_length(leds); ++ idx)
            {
              led = json_object_array_get_idx(leds, idx);
              /* required entries */
              if (!json_object_object_get_ex(led, "led", &tmp) &&
                  !json_object_object_get_ex(led, "intensity", &tmp))
              {
                printf ("failed in intensity map\n");
                return false;
              }
                pLED.ledMapIntensity = true;
            }
          }
          if (json_object_object_get_ex(jobj, "state", &leds))
          {
            for (idx = 0; idx < json_object_array_length(leds); ++idx)
            {
              led = json_object_array_get_idx(leds, idx);
              /* required entries */
              if (!json_object_object_get_ex(led, "led", &tmp) &&
                  !json_object_object_get_ex(led, "state", &tmp))
              {
                printf ("failed in state map\n");
                return false;
              }
              pLED.ledMapState = true;
            }
          }
          if (json_object_object_get_ex(jobj, "LED states all", &tmp))
          {
            /* can't have the 'LED map' entry with 'LED states all' */
            if (pLED.ledMapState == true)
            {
              return false;
            }

            pLED.allStates = true;
          }
          if (json_object_object_get_ex(jobj, "LED states random", &tmp))
          {
            /* can't have the 'LED map' or 'LED states all' entries with 'LED states random' */
            if (pLED.ledMapState == true || pLED.allStates == true)
            {
              return false;
            }

            pLED.random = true;
          }

          if (json_object_object_get_ex(jobj, "LED intensity all", &tmp))
          {
            /* can't have the 'LED map' entry with 'LED intensity all' */
            if (pLED.ledMapIntensity == true)
            {
              return false;
            }

            pLED.allIntensities = true;
          }

          return true;
        }
      }
    }
  }

  return false;
}

void convertIntensityMap (json_object* jobj)
{
  int intensity = 0;
  char map[2] = {0,0};

  json_object* tmp = NULL;

  json_object_object_get_ex(jobj, "led", &tmp);
  map[0] = json_object_get_int(tmp);

  json_object_object_get_ex(jobj, "intensity", &tmp);
  intensity = json_object_get_int(tmp);

  switch(intensity)
  {
    case 0:   map[1] = 0x00; break;   case 1:   map[1] = 0x01; break;   case 2:   map[1] = 0x02; break;
    case 3:   map[1] = 0x03; break;   case 4:   map[1] = 0x04; break;   case 5:   map[1] = 0x05; break;
    case 6:   map[1] = 0x06; break;   case 7:   map[1] = 0x07; break;   case 8:   map[1] = 0x08; break;
    case 9:   map[1] = 0x09; break;   case 10:  map[1] = 0x0A; break;   case 11:  map[1] = 0x0B; break;
    case 12:  map[1] = 0x0C; break;   case 13:  map[1] = 0x0D; break;   case 14:  map[1] = 0x0E; break;
    case 15:  map[1] = 0x0F; break;   case 16:  map[1] = 0x10; break;   case 17:  map[1] = 0x11; break;
    case 18:  map[1] = 0x12; break;   case 19:  map[1] = 0x13; break;   case 20:  map[1] = 0x14; break;
    case 21:  map[1] = 0x15; break;   case 22:  map[1] = 0x16; break;   case 23:  map[1] = 0x17; break;
    case 24:  map[1] = 0x18; break;   case 25:  map[1] = 0x19; break;   case 26:  map[1] = 0x1A; break;
    case 27:  map[1] = 0x1B; break;   case 28:  map[1] = 0x1C; break;   case 29:  map[1] = 0x1D; break;
    case 30:  map[1] = 0x1E; break;   case 31:  map[1] = 0x1F; break;   case 32:  map[1] = 0x20; break;
    case 33:  map[1] = 0x21; break;   case 34:  map[1] = 0x22; break;   case 35:  map[1] = 0x23; break;
    case 36:  map[1] = 0x24; break;   case 37:  map[1] = 0x25; break;   case 38:  map[1] = 0x26; break;
    case 39:  map[1] = 0x27; break;   case 40:  map[1] = 0x28; break;   case 41:  map[1] = 0x29; break;
    case 42:  map[1] = 0x2A; break;   case 43:  map[1] = 0x2B; break;   case 44:  map[1] = 0x2C; break;
    case 45:  map[1] = 0x2D; break;   case 46:  map[1] = 0x2E; break;   case 47:  map[1] = 0x2F; break;
    case 48:  map[1] = 0x30; break;   case 49:  map[1] = 0x31; break;   case 50:  map[1] = 0x32; break;
    case 51:  map[1] = 0x33; break;   case 52:  map[1] = 0x34; break;   case 53:  map[1] = 0x35; break;
    case 54:  map[1] = 0x36; break;   case 55:  map[1] = 0x37; break;   case 56:  map[1] = 0x38; break;
    case 57:  map[1] = 0x39; break;   case 58:  map[1] = 0x3A; break;   case 59:  map[1] = 0x3B; break;
    case 60:  map[1] = 0x3C; break;   case 61:  map[1] = 0x3D; break;   case 62:  map[1] = 0x3E; break;
    case 63:  map[1] = 0x3F; break;   case 64:  map[1] = 0x40; break;   case 65:  map[1] = 0x41; break;
    case 66:  map[1] = 0x42; break;   case 67:  map[1] = 0x43; break;   case 68:  map[1] = 0x44; break;
    case 69:  map[1] = 0x45; break;   case 70:  map[1] = 0x46; break;   case 71:  map[1] = 0x47; break;
    case 72:  map[1] = 0x48; break;   case 73:  map[1] = 0x49; break;   case 74:  map[1] = 0x4A; break;
    case 75:  map[1] = 0x4B; break;   case 76:  map[1] = 0x4C; break;   case 77:  map[1] = 0x4D; break;
    case 78:  map[1] = 0x4E; break;   case 79:  map[1] = 0x4F; break;   case 80:  map[1] = 0x50; break;
    case 81:  map[1] = 0x51; break;   case 82:  map[1] = 0x52; break;   case 83:  map[1] = 0x53; break;
    case 84:  map[1] = 0x54; break;   case 85:  map[1] = 0x55; break;   case 86:  map[1] = 0x56; break;
    case 87:  map[1] = 0x57; break;   case 88:  map[1] = 0x58; break;   case 89:  map[1] = 0x59; break;
    case 90:  map[1] = 0x5A; break;   case 91:  map[1] = 0x5B; break;   case 92:  map[1] = 0x5C; break;
    case 93:  map[1] = 0x5D; break;   case 94:  map[1] = 0x5E; break;   case 95:  map[1] = 0x5F; break;
    case 96:  map[1] = 0x60; break;   case 97:  map[1] = 0x61; break;   case 98:  map[1] = 0x62; break;
    case 99:  map[1] = 0x63; break;   case 100: map[1] = 0x64; break;   case 101: map[1] = 0x65; break;
    case 102: map[1] = 0x66; break;   case 103: map[1] = 0x67; break;   case 104: map[1] = 0x68; break;
    case 105: map[1] = 0x69; break;   case 106: map[1] = 0x6A; break;   case 107: map[1] = 0x6B; break;
    case 108: map[1] = 0x6C; break;   case 109: map[1] = 0x6D; break;   case 110: map[1] = 0x6E; break;
    case 111: map[1] = 0x6F; break;   case 112: map[1] = 0x70; break;   case 113: map[1] = 0x71; break;
    case 114: map[1] = 0x72; break;   case 115: map[1] = 0x73; break;   case 116: map[1] = 0x74; break;
    case 117: map[1] = 0x75; break;   case 118: map[1] = 0x76; break;   case 119: map[1] = 0x77; break;
    case 120: map[1] = 0x78; break;   case 121: map[1] = 0x79; break;   case 122: map[1] = 0x7A; break;
    case 123: map[1] = 0x7B; break;   case 124: map[1] = 0x7C; break;   case 125: map[1] = 0x7D; break;
    case 126: map[1] = 0x7E; break;   case 127: map[1] = 0x7F; break;   case 128: map[1] = 0x80; break;
    case 129: map[1] = 0x81; break;   case 130: map[1] = 0x82; break;   case 131: map[1] = 0x83; break;
    case 132: map[1] = 0x84; break;   case 133: map[1] = 0x85; break;   case 134: map[1] = 0x86; break;
    case 135: map[1] = 0x87; break;   case 136: map[1] = 0x88; break;   case 137: map[1] = 0x89; break;
    case 138: map[1] = 0x8A; break;   case 139: map[1] = 0x8B; break;   case 140: map[1] = 0x8C; break;
    case 141: map[1] = 0x8D; break;   case 142: map[1] = 0x8E; break;   case 143: map[1] = 0x8F; break;
    case 144: map[1] = 0x90; break;   case 145: map[1] = 0x91; break;   case 146: map[1] = 0x92; break;
    case 147: map[1] = 0x93; break;   case 148: map[1] = 0x94; break;   case 149: map[1] = 0x95; break;
    case 150: map[1] = 0x96; break;   case 151: map[1] = 0x97; break;   case 152: map[1] = 0x98; break;
    case 153: map[1] = 0x99; break;   case 154: map[1] = 0x9A; break;   case 155: map[1] = 0x9B; break;
    case 156: map[1] = 0x9C; break;   case 157: map[1] = 0x9D; break;   case 158: map[1] = 0x9E; break;
    case 159: map[1] = 0x9F; break;   case 160: map[1] = 0xA0; break;   case 161: map[1] = 0xA1; break;
    case 162: map[1] = 0xA2; break;   case 163: map[1] = 0xA3; break;   case 164: map[1] = 0xA4; break;
    case 165: map[1] = 0xA5; break;   case 166: map[1] = 0xA6; break;   case 167: map[1] = 0xA7; break;
    case 168: map[1] = 0xA8; break;   case 169: map[1] = 0xA9; break;   case 170: map[1] = 0xAA; break;
    case 171: map[1] = 0xAB; break;   case 172: map[1] = 0xAC; break;   case 173: map[1] = 0xAD; break;
    case 174: map[1] = 0xAE; break;   case 175: map[1] = 0xAF; break;   case 176: map[1] = 0xB0; break;
    case 177: map[1] = 0xB1; break;   case 178: map[1] = 0xB2; break;   case 179: map[1] = 0xB3; break;
    case 180: map[1] = 0xB4; break;   case 181: map[1] = 0xB5; break;   case 182: map[1] = 0xB6; break;
    case 183: map[1] = 0xB7; break;   case 184: map[1] = 0xB8; break;   case 185: map[1] = 0xB9; break;
    case 186: map[1] = 0xBA; break;   case 187: map[1] = 0xBB; break;   case 188: map[1] = 0xBC; break;
    case 189: map[1] = 0xBD; break;   case 190: map[1] = 0xBE; break;   case 191: map[1] = 0xBF; break;
    case 192: map[1] = 0xC0; break;   case 193: map[1] = 0xC1; break;   case 194: map[1] = 0xC2; break;
    case 195: map[1] = 0xC3; break;   case 196: map[1] = 0xC4; break;   case 197: map[1] = 0xC5; break;
    case 198: map[1] = 0xC6; break;   case 199: map[1] = 0xC7; break;   case 200: map[1] = 0xC8; break;
    case 201: map[1] = 0xC9; break;   case 202: map[1] = 0xCA; break;   case 203: map[1] = 0xCB; break;
    case 204: map[1] = 0xCC; break;   case 205: map[1] = 0xCD; break;   case 206: map[1] = 0xCE; break;
    case 207: map[1] = 0xCF; break;   case 208: map[1] = 0xD0; break;   case 209: map[1] = 0xD1; break;
    case 210: map[1] = 0xD2; break;   case 211: map[1] = 0xD3; break;   case 212: map[1] = 0xD4; break;
    case 213: map[1] = 0xD5; break;   case 214: map[1] = 0xD6; break;   case 215: map[1] = 0xD7; break;
    case 216: map[1] = 0xD8; break;   case 217: map[1] = 0xD9; break;   case 218: map[1] = 0xDA; break;
    case 219: map[1] = 0xDB; break;   case 220: map[1] = 0xDC; break;   case 221: map[1] = 0xDD; break;
    case 222: map[1] = 0xDE; break;   case 223: map[1] = 0xDF; break;   case 224: map[1] = 0xE0; break;
    case 225: map[1] = 0xE1; break;   case 226: map[1] = 0xE2; break;   case 227: map[1] = 0xE3; break;
    case 228: map[1] = 0xE4; break;   case 229: map[1] = 0xE5; break;   case 230: map[1] = 0xE6; break;
    case 231: map[1] = 0xE7; break;   case 232: map[1] = 0xE8; break;   case 233: map[1] = 0xE9; break;
    case 234: map[1] = 0xEA; break;   case 235: map[1] = 0xEB; break;   case 236: map[1] = 0xEC; break;
    case 237: map[1] = 0xED; break;   case 238: map[1] = 0xEE; break;   case 239: map[1] = 0xEF; break;
    case 240: map[1] = 0xF0; break;   case 241: map[1] = 0xF1; break;   case 242: map[1] = 0xF2; break;
    case 243: map[1] = 0xF3; break;   case 244: map[1] = 0xF4; break;   case 245: map[1] = 0xF5; break;
    case 246: map[1] = 0xF6; break;   case 247: map[1] = 0xF7; break;   case 248: map[1] = 0xF8; break;
    case 249: map[1] = 0xF9; break;   case 250: map[1] = 0xFA; break;   case 251: map[1] = 0xFB; break;
    case 252: map[1] = 0xFC; break;   case 253: map[1] = 0xFD; break;   case 254: map[1] = 0xFE; break;
    case 255: map[1] = 0xFF; break;   default:  map[1] = 0; break;
  }

  printf ("LED: %d, 0x%02x(%d)\n", map[0], map[1], intensity);
  /* ship this data off to the USB device */
}

bool updateBoardPacLED (json_object *jobj)
{
  int idx = 0;

  json_object *leds = NULL;
  json_object *led = NULL;

  if (pLED.ledMapIntensity == true)
  {
    json_object_object_get_ex(jobj, "intensity", &leds);
    for (idx = 0; idx < json_object_array_length(leds); ++ idx)
    {
      /* TODO: send the USB handle to the convert function */
      led = json_object_array_get_idx(leds, idx);
      convertIntensityMap (led);
    }
  }
}
