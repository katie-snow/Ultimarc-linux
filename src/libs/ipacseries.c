/*
 ============================================================================
 Name        : pacseries.c
 Author      : Katie Snow
 Version     :
 Copyright   : Copyright 2014 Robert Abram, Katie Snow
 Description : Ultimarc 2015 PAC board configuration library
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
#include "ipacseries.h"

union ipacconfig piconfig;

unsigned char
convertIPACKey (enum ipac_boards_t bid, json_object* jobj)
{
  switch (bid)
  {
    case PRE_IPAC2_BOARD:
    case PRE_IPAC4_BOARD:
    case PRE_MINIPAC_BOARD:
      return convertIPAC(jobj);
      break;

    case IPAC2_BOARD:
    case MINIPAC_BOARD:
    case IPAC4_BOARD:
    case JPAC_BOARD:
      return convertIPACSeries(jobj);
      break;

    default:
      return 0x00;
  }
}

unsigned char
convertIPACSeries (json_object* jobj)
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
    if (!strcasecmp(str, "ENTER"))
      retval = 0x28;
    if (!strcasecmp(str, "ESC"))
      retval = 0x29;
    if (!strcasecmp(str, "BKSP"))
      retval = 0x2A;
    if (!strcasecmp(str, "TAB"))
      retval = 0x2B;
    if (!strcasecmp(str, "SPACE"))
      retval = 0x2C;
    if (!strcasecmp(str, "-"))
      retval = 0x2D;
    if (!strcasecmp(str, "="))
      retval = 0x2E;
    if (!strcasecmp(str, "["))
      retval = 0x2F;
    if (!strcasecmp(str, "]"))
      retval = 0x30;
    if (!strcasecmp(str, "non US #"))
      retval = 0x32;
    if (!strcasecmp(str, "'"))
      retval = 0x34;
    if (!strcasecmp(str, "`"))
      retval = 0x35;
    if (!strcasecmp(str, ","))
      retval = 0x36;
    if (!strcasecmp(str, "."))
      retval = 0x37;
    if (!strcasecmp(str, "/"))
      retval = 0x38;
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
    if (!strcasecmp(str, "PRNT SCRN"))
      retval = 0x46;
    if (!strcasecmp(str, "SCROLL"))
      retval = 0x47;
    if (!strcasecmp(str, "PAUSE"))
      retval = 0x48;
    if (!strcasecmp(str, "INSERT"))
      retval = 0x49;
    if (!strcasecmp(str, "HOME"))
      retval = 0x4A;
    if (!strcasecmp(str, "PGUP"))
      retval = 0x4B;
    if (!strcasecmp(str, "DEL"))
      retval = 0x4C;
    if (!strcasecmp(str, "END"))
      retval = 0x4D;
    if (!strcasecmp(str, "PGDWN"))
      retval = 0x4E;
    if (!strcasecmp(str, "RIGHT"))
      retval = 0x4F;
    if (!strcasecmp(str, "LEFT"))
      retval = 0x50;
    if (!strcasecmp(str, "DOWN"))
      retval = 0x51;
    if (!strcasecmp(str, "UP"))
      retval = 0x52;
    if (!strcasecmp(str, "NUM"))
      retval = 0x53;
    if (!strcasecmp(str, "KP /"))
      retval = 0x54;
    if (!strcasecmp(str, "KP *"))
      retval = 0x55;
    if (!strcasecmp(str, "KP -"))
      retval = 0x56;
    if (!strcasecmp(str, "KP +"))
      retval = 0x57;
    if (!strcasecmp(str, "KP ENTER"))
      retval = 0x58;
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
    if (!strcasecmp(str, "KP ."))
      retval = 0x63;
    if (!strcasecmp(str, "\\") || !strcasecmp(str, "NON US \\"))
      retval = 0x64;
    if (!strcasecmp(str, "APP"))
      retval = 0x65;
    if (!strcasecmp(str, "KEYBOARD POWER"))
      retval = 0x66;
    if (!strcasecmp(str, "KP ="))
      retval = 0x67;
    if (!strcasecmp(str, "CTRL L"))
      retval = 0x70;
    if (!strcasecmp(str, "SHIFT L"))
      retval = 0x71;
    if (!strcasecmp(str, "ALT L"))
      retval = 0x72;
    if (!strcasecmp(str, "WIN L"))
      retval = 0x73;
    if (!strcasecmp(str, "CTRL R"))
      retval = 0x74;
    if (!strcasecmp(str, "SHIFT R"))
      retval = 0x75;
    if (!strcasecmp(str, "ALT R"))
      retval = 0x76;
    if (!strcasecmp(str, "WIN MENU"))
      retval = 0x77;
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
      retval = 0x8A;
    if (!strcasecmp(str, "VOL UP"))
      retval = 0x8B;
    if (!strcasecmp(str, "VOL DOWN"))
      retval = 0x8C;
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
      retval = 0x9A;
    if (!strcasecmp(str, "GAMEPAD 12"))
      retval = 0x9B;
    if (!strcasecmp(str, "GAMEPAD 13"))
      retval = 0x9C;
    if (!strcasecmp(str, "GAMEPAD 14"))
      retval = 0x9D;
    if (!strcasecmp(str, "GAMEPAD 15"))
      retval = 0x9E;
    if (!strcasecmp(str, "GAMEPAD 16"))
      retval = 0x9F;
    if (!strcasecmp(str, "GAMEPAD 17"))
      retval = 0xA0;
    if (!strcasecmp(str, "GAMEPAD 18"))
      retval = 0xA1;
    if (!strcasecmp(str, "GAMEPAD 19"))
      retval = 0xA2;
    if (!strcasecmp(str, "GAMEPAD 20"))
      retval = 0xA3;
    if (!strcasecmp(str, "GAMEPAD 21"))
      retval = 0xA4;
    if (!strcasecmp(str, "GAMEPAD 22"))
      retval = 0xA5;
    if (!strcasecmp(str, "GAMEPAD 23"))
      retval = 0xA6;
    if (!strcasecmp(str, "GAMEPAD 24"))
      retval = 0xA7;
    if (!strcasecmp(str, "GAMEPAD 25"))
      retval = 0xA8;
    if (!strcasecmp(str, "GAMEPAD 26"))
      retval = 0xA9;
    if (!strcasecmp(str, "GAMEPAD 27"))
      retval = 0xAA;
    if (!strcasecmp(str, "GAMEPAD 28"))
      retval = 0xAB;
    if (!strcasecmp(str, "GAMEPAD 29"))
      retval = 0xAC;
    if (!strcasecmp(str, "GAMEPAD 30"))
      retval = 0xAD;
    if (!strcasecmp(str, "GAMEPAD 31"))
      retval = 0xAE;
    if (!strcasecmp(str, "GAMEPAD 32"))
      retval = 0xAF;
    if (!strcasecmp(str, "ANALOG 0"))
      retval = 0xB0;
    if (!strcasecmp(str, "ANALOG 1"))
      retval = 0xB1;
    if (!strcasecmp(str, "ANALOG 2"))
      retval = 0xB2;
    if (!strcasecmp(str, "ANALOG 3"))
      retval = 0xB3;
    if (!strcasecmp(str, "ANALOG 4"))
      retval = 0xB4;
    if (!strcasecmp(str, "ANALOG 5"))
      retval = 0xB5;
    if (!strcasecmp(str, "ANALOG 6"))
      retval = 0xB6;
    if (!strcasecmp(str, "ANALOG 7"))
      retval = 0xB7;
    if (!strcasecmp(str, "HAT 0"))
      retval = 0xBA;
    if (!strcasecmp(str, "HAT 1"))
      retval = 0xBB;
    if (!strcasecmp(str, "HAT 2"))
      retval = 0xDC;
    if (!strcasecmp(str, "HAT 3"))
      retval = 0xBD;
    if (!strcasecmp(str, "TRACKBALL X"))
      retval = 0xC0;
    if (!strcasecmp(str, "TRACKBALL Y"))
      retval = 0xC2;
    if (!strcasecmp(str, "TRACKBALL Z"))
      retval = 0xC4;
    if (!strcasecmp(str, "m1"))
      retval = 0xe0;
    if (!strcasecmp(str, "m2"))
      retval = 0xe1;
    if (!strcasecmp(str, "m3") || !strcasecmp(str, "MUTE"))
      retval = 0xe2;
    if (!strcasecmp(str, "m4") || !strcasecmp(str, "PLAY/PAUSE"))
      retval = 0xe3;
    if (!strcasecmp(str, "m5") || !strcasecmp(str, "NEXT"))
      retval = 0xe4;
    if (!strcasecmp(str, "m6") || !strcasecmp(str, "PREV"))
      retval = 0xe5;
    if (!strcasecmp(str, "m7") || !strcasecmp(str, "STOP"))
      retval = 0xe6;
    if (!strcasecmp(str, "m8"))
      retval = 0xe7;
    if (!strcasecmp(str, "m9"))
      retval = 0xe8;
    if (!strcasecmp(str, "m10"))
      retval = 0xe9;
    if (!strcasecmp(str, "m11"))
      retval = 0xea;
    if (!strcasecmp(str, "m12"))
      retval = 0xeb;
    if (!strcasecmp(str, "m13"))
      retval = 0xec;
    if (!strcasecmp(str, "m14"))
      retval = 0xed;
    if (!strcasecmp(str, "m15"))
      retval = 0xee;
    if (!strcasecmp(str, "m16"))
      retval = 0xef;
    if (!strcasecmp(str, "m17") || !strcasecmp(str, "EMAIL"))
      retval = 0xf0;
    if (!strcasecmp(str, "m18") || !strcasecmp(str, "SEARCH"))
      retval = 0xf1;
    if (!strcasecmp(str, "m19") || !strcasecmp(str, "BOOKMARKS"))
      retval = 0xf2;
    if (!strcasecmp(str, "m20") || !strcasecmp(str, "OPEN BROWSER"))
      retval = 0xf3;
    if (!strcasecmp(str, "m21") || !strcasecmp(str, "WEB BACK"))
      retval = 0xf4;
    if (!strcasecmp(str, "m22") || !strcasecmp(str, "WEB FORWARD"))
      retval = 0xf5;
    if (!strcasecmp(str, "m23") || !strcasecmp(str, "WEB STOP"))
      retval = 0xf6;
    if (!strcasecmp(str, "m24") || !strcasecmp(str, "WEB REFRESH"))
      retval = 0xf7;
    if (!strcasecmp(str, "m25") || !strcasecmp(str, "MEDIA PLAYER"))
      retval = 0xf8;
    if (!strcasecmp(str, "m26"))
      retval = 0xf9;
    if (!strcasecmp(str, "m27") || !strcasecmp(str, "CALCULATOR"))
      retval = 0xfa;
    if (!strcasecmp(str, "m28"))
      retval = 0xfb;
    if (!strcasecmp(str, "m29") || !strcasecmp(str, "EXPLORER"))
      retval = 0xfc;
    if (!strcasecmp(str, "m30"))
      retval = 0xfd;
    if (!strcasecmp(str, "m31") || !strcasecmp(str, "WAIT 3 SEC"))
      retval = 0xfe;
  }

  return retval;
}

unsigned char
convertIPAC (json_object *jobj)
{
  unsigned char retval = 0x00;
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
    if (!strcasecmp(str, "m1"))
      retval = 0xe0;
    if (!strcasecmp(str, "m2"))
      retval = 0xe1;
    if (!strcasecmp(str, "m3"))
      retval = 0xe2;
    if (!strcasecmp(str, "m4"))
      retval = 0xe3;
  }

  return retval;
}

/* Array row order
1up, 1dn, 1rt, 1lt, 2up, 2dn, 2rt, 2lt, 3up, 3dn, 3rt, 3lt, 4up, 4dn, 4rt, 4lt,
1s1, 1s2, 1s3, 1s4, 1s5, 1s6, 1s7, 1s8, 2s1, 2s2, 2s3, 2s4, 2s5, 2s6, 2s7, 2s8,
3s1, 3s2, 3s3, 3s4, 3s5, 3s6, 3s7, 3s8, 4s1, 4s2, 4s3, 4s4, 4s5, 4s6, 4s7, 4s8,
1start, 1coin, 1a, 1b, 2start, 2coin, 2a, 2b, 3start, 3coin, 4start, 4coin,
1test, 2test, service */

/* Normal key press value */
int keyLookupTable[7][63] = {
/* Pre2015 IPAC2 */
{1, 6, 2, 4, 13, 14, 9, 11, -1, -1, -1, -1, -1, -1, -1, -1, 3, 8, 5, 10, 7, 12, 24, 26,
 15, 17, 19, 21, 23, 25, 27, 28, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
 -1, -1, -1, 16, 20, 29, 30, 18, 22, 31, 32, -1, -1, -1, -1, -1, -1, -1},

/* Pre2015 IPAC4 */
{1, 6, 2, 4, 13, 14, 9, 11, 101, 106, 102, 104, 113, 114, 109, 111, 3, 8, 5, 10, 7, 12,
 24, 26, 15, 17, 19, 21, 23, 25, 27, 28, 103, 108, 105, 110, 107, 112, 124, 126, 115,
 117, 119, 121, 123, 125, 127, 128, 16, 20, -1, -1, 18, 22, -1, -1, 116, 120, 118, 122,
 -1, -1, -1},

/* 2015 IPAC2 */
{20, 18, 24, 22, 21, 19, 1, 23, -1, -1, -1, -1, -1, -1, -1, -1, 40, 38, 36, 34, 32, 30,
 28, 26, 17, 39, 37, 35, 33, 29, 27, 25, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
 -1, -1, -1, -1, 48, 46, 44, 42, 47, 45, 43, 41, -1, -1, -1, -1, -1, -1, -1},

/* 2015 MinIPAC */
{11, 9, 15, 13, 38, 40, 34, 36, -1, -1, -1, -1, -1, -1, -1, -1, 10, 12, 14, 16, 42, 44,
 46, 48, 18, 20, 22, 24, 2, 4, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
 -1, -1, -1, 26, 30, 7, 5, 28, 32, 3, 1, -1, -1, -1, -1, -1, -1, -1},

/* 2015 IPAC4 */
{16, 14, 20, 18, 13, 11, 17, 15, 48, 38, 47, 40, 37, 35, 39, 45, 12, 10, 32, 30, 28, 53,
 64, 56, 9, 31,29, 27, 60, 61, 49, 57, 36, 34, 8, 6, 4, 2, 50, 58, 33, 7, 5, 3, 1, 23,
 59, 51, 62, 54, -1, -1, 55, 63, -1, -1, 24, 22, 21, 19, -1, -1, -1},

/* 2015 JPAC */
{25, 29, 37, 33, 27, 31, 39, 35, -1, -1, -1, -1, -1, -1, -1, -1, 17, 21, 1, 5, 20, 18,
 40, 38, 19, 23, 3, 7, 32, 30, 34, 36, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
 -1, -1, -1, -1, 45, 41, -1, -1, 47, 43, -1, -1, -1, -1, -1, -1, 13, 15, 11},

 /* Pre2015 MINIPAC */
{1, 6, 2, 4, 13, 14, 9, 11, -1, -1, -1, -1, -1, -1, -1, -1, 3, 8, 5, 10, 7, 12, 24, 26,
 15, 17, 19, 21, 23, 25, 27, 28, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
 -1, -1, -1, 16, 20, 29, 30, 18, 22, 31, 32, -1, -1, -1, -1, -1, -1, -1}

};

int shiftAdjTable[] = {32, 28, 50, 50, 64, 50, 32};
int shiftPosAdjTable[] = {-1, -1, 100, 100, 128, 100, -1};


/* Only the pre2015 boards will use this lookup array */
int macroLookupTable[7][4] = {
/* Pre2015 IPAC2 */
{63, 67, 71, 75},

/* Pre2015 IPAC4 */
{59, 63, 67, 71},

/* 2015 IPAC2 */
{-1, -1, -1, -1},

/* 2015 MinIPAC */
{-1, -1, -1, -1},

/* 2015 IPAC4 */
{-1, -1, -1, -1},

/* 2015 JPAC */
{-1, -1, -1, -1},

 /* Pre2015 MINIPAC */
{63, 67, 71, 75}
};

void populateBoardArray (int bid, json_object* jobj, unsigned char* barray)
{
  int idx = -1;
  int lkey = -1;

  unsigned char keyval;
  char shiftval;

  json_object *tmp = NULL;

  json_object_object_foreach(jobj, key, pin)
  {
    lkey = decipherLookupKey(key);

    if (lkey == -1)
      continue;

    // access table with lkey and bid to get the location to place data in barray
    idx = keyLookupTable[bid][lkey];

    if (idx == -1)
    {
      log_warn("The key given does not have a position for the given board");
      continue;
    }

    /* Populate board array with normal key */
    json_object_object_get_ex(pin, "key", &tmp);
    barray[idx] = convertIPACKey(bid, tmp);

    /* Case for IPAC2 and MINIPAC pre 2015 boards.  The a1, a2, b1, b2 keys do not have a shift key value */
    if (bid == PRE_IPAC2_BOARD ||
        bid == PRE_MINIPAC_BOARD)
    {
      if (strcasecmp(key, "1a") != 0 &&
          strcasecmp(key, "1b") != 0 &&
          strcasecmp(key, "2a") != 0 &&
          strcasecmp(key, "2b") != 0)
      {
        /* Populate board array with shift key */
        json_object_object_get_ex(pin, "shift", &tmp);
        barray[idx + shiftAdjTable[bid]] = convertIPACKey(bid, tmp);
        debug("key:%s,\tvalue idx: %i, value sft: %i", key, idx, idx + shiftAdjTable[bid]);
      }
      else
      {
        debug("key:%s,\tvalue idx: %i", key, idx);
      }
    }
    else
    {
      /* Populate board array with shift key */
      json_object_object_get_ex(pin, "shift", &tmp);
      barray[idx + shiftAdjTable[bid]] = convertIPACKey(bid, tmp);
      debug("key:%s,\tvalue idx: %i, value sft: %i", key, idx, idx + shiftAdjTable[bid]);
    }
  }
}

int decipherLookupKey (const char* key)
{
  int lkey = -1;

  if (!strcasecmp(key, "1up"))
    lkey = 0;
  if (!strcasecmp(key, "1down"))
    lkey = 1;
  if (!strcasecmp(key, "1right"))
    lkey = 2;
  if (!strcasecmp(key, "1left"))
    lkey = 3;
  if (!strcasecmp(key, "2up"))
    lkey = 4;
  if (!strcasecmp(key, "2down"))
    lkey = 5;
  if (!strcasecmp(key, "2right"))
    lkey = 6;
  if (!strcasecmp(key, "2left"))
    lkey = 7;
  if (!strcasecmp(key, "3up"))
    lkey = 8;
  if (!strcasecmp(key, "3down"))
    lkey = 9;
  if (!strcasecmp(key, "3right"))
    lkey = 10;
  if (!strcasecmp(key, "3left"))
    lkey = 11;
  if (!strcasecmp(key, "4up"))
    lkey = 12;
  if (!strcasecmp(key, "4down"))
    lkey = 13;
  if (!strcasecmp(key, "4right"))
    lkey = 14;
  if (!strcasecmp(key, "4left"))
    lkey = 15;
  if (!strcasecmp(key, "1sw1"))
    lkey = 16;
  if (!strcasecmp(key, "1sw2"))
    lkey = 17;
  if (!strcasecmp(key, "1sw3"))
    lkey = 18;
  if (!strcasecmp(key, "1sw4"))
    lkey = 19;
  if (!strcasecmp(key, "1sw5"))
    lkey = 20;
  if (!strcasecmp(key, "1sw6"))
    lkey = 21;
  if (!strcasecmp(key, "1sw7"))
    lkey = 22;
  if (!strcasecmp(key, "1sw8"))
    lkey = 23;
  if (!strcasecmp(key, "2sw1"))
    lkey = 24;
  if (!strcasecmp(key, "2sw2"))
    lkey = 25;
  if (!strcasecmp(key, "2sw3"))
    lkey = 26;
  if (!strcasecmp(key, "2sw4"))
    lkey = 27;
  if (!strcasecmp(key, "2sw5"))
    lkey = 28;
  if (!strcasecmp(key, "2sw6"))
    lkey = 29;
  if (!strcasecmp(key, "2sw7"))
    lkey = 30;
  if (!strcasecmp(key, "2sw8"))
    lkey = 31;
  if (!strcasecmp(key, "3sw1"))
    lkey = 32;
  if (!strcasecmp(key, "3sw2"))
    lkey = 33;
  if (!strcasecmp(key, "3sw3"))
    lkey = 34;
  if (!strcasecmp(key, "3sw4"))
    lkey = 35;
  if (!strcasecmp(key, "3sw5"))
    lkey = 36;
  if (!strcasecmp(key, "3sw6"))
    lkey = 37;
  if (!strcasecmp(key, "3sw7"))
    lkey = 38;
  if (!strcasecmp(key, "3sw8"))
    lkey = 39;
  if (!strcasecmp(key, "4sw1"))
    lkey = 40;
  if (!strcasecmp(key, "4sw2"))
    lkey = 41;
  if (!strcasecmp(key, "4sw3"))
    lkey = 42;
  if (!strcasecmp(key, "4sw4"))
    lkey = 43;
  if (!strcasecmp(key, "4sw5"))
    lkey = 44;
  if (!strcasecmp(key, "4sw6"))
    lkey = 45;
  if (!strcasecmp(key, "4sw7"))
    lkey = 46;
  if (!strcasecmp(key, "4sw8"))
    lkey = 47;
  if (!strcasecmp(key, "1start"))
    lkey = 48;
  if (!strcasecmp(key, "1coin"))
    lkey = 49;
  if (!strcasecmp(key, "1a"))
    lkey = 50;
  if (!strcasecmp(key, "1b"))
    lkey = 51;
  if (!strcasecmp(key, "2start"))
    lkey = 52;
  if (!strcasecmp(key, "2coin"))
    lkey = 53;
  if (!strcasecmp(key, "2a"))
    lkey = 54;
  if (!strcasecmp(key, "2b"))
    lkey = 55;
  if (!strcasecmp(key, "3start"))
    lkey = 56;
  if (!strcasecmp(key, "3coin"))
    lkey = 57;
  if (!strcasecmp(key, "4start"))
    lkey = 58;
  if (!strcasecmp(key, "4coin"))
    lkey = 59;
  if (!strcasecmp(key, "1test"))
    lkey = 60;
  if (!strcasecmp(key, "2test"))
    lkey = 61;
  if (!strcasecmp(key, "service"))
    lkey = 62;

  if (lkey == -1)
  {
    log_info("Unable to decipher pin '%s'.", key);
  }

  return lkey;
}

int decipherLookupMacroKey (const char* key)
{
  int lkey = -1;

  if (!strcasecmp(key, "m1"))
    lkey = 0;
  if (!strcasecmp(key, "m2"))
    lkey = 1;
  if (!strcasecmp(key, "m3"))
    lkey = 2;
  if (!strcasecmp(key, "m4"))
    lkey = 3;

  if (lkey == -1)
  {
    log_info("Unable to decipher macro '%s'.", key);
  }
  return lkey;
}

/* Configuration options
 * Enable high-current output on UIO board, NOT IMPLEMENTED
 * Enable accelerometer on UIO board, NOT IMPLEMENTED
 * Enable PacLink interface on I-Pac 2, NOT IMPLEMENTED
 * Debounce value
 */
bool validateConfigData (json_object* jobj, bool oldValid)
{
  bool valid = oldValid;

  json_object* config = NULL;

  piconfig.config = 0;

  if (json_object_object_get_ex(jobj, "config", &config))
  {
    if (json_object_is_type(config, json_type_object))
    {
      debug ("Configuration Settings:");

      /* Debounce */
      valid = validateConfigDebounce (config, valid);
    }
    else
    {
      log_err("config entry needs to be of type object");
      valid = false;
    }
  }

  return valid;
}

void populateConfigurationValue (unsigned char* barray)
{
  /* populate this here so that ipacconfig variable can be accessed. */
  barray[3] = piconfig.config;
}

bool validateConfigDebounce (json_object* config, bool oldValid)
{
  bool valid = oldValid;
  int  lvalue = -1;

  json_object* debounce = NULL;
  const char* debounceStr;

  if (json_object_object_get_ex(config, "debounce", &debounce))
  {
    /*
     * Debounce values; None, Standard, Long, Short
     */
    if (json_object_is_type(debounce, json_type_string))
    {
      debounceStr = json_object_get_string(debounce);
      if (!strcasecmp(debounceStr, "none"))
      {
        piconfig.parts.debounce_enabled = 0;
        piconfig.parts.debounce_pos_4 = 1;
        piconfig.parts.debounce_pos_5 = 1;
        lvalue = 0;
      }
      if (!strcasecmp(debounceStr, "standard"))
      {
        piconfig.parts.debounce_enabled = 1;
        piconfig.parts.debounce_pos_4 = 0;
        piconfig.parts.debounce_pos_5 = 1;
        lvalue = 0;
      }
      if (!strcasecmp(debounceStr, "long"))
      {
        piconfig.parts.debounce_enabled = 1;
        piconfig.parts.debounce_pos_4 = 1;
        piconfig.parts.debounce_pos_5 = 0;
        lvalue = 0;
      }
      if (!strcasecmp(debounceStr, "short"))
      {
        piconfig.parts.debounce_enabled = 1;
        piconfig.parts.debounce_pos_4 = 0;
        piconfig.parts.debounce_pos_5 = 0;
        lvalue = 0;
      }

      debug ("    Debounce: %02x", piconfig.config);

      if (lvalue == -1)
      {
        log_info("Unable to decipher debounce '%s'.", debounceStr);
        valid = false;
      }
    }
    else
    {
      log_err("Debounce config needs to be of type string");
      valid = false;
    }
  }

  return valid;
}

void populateShiftPosition (enum ipac_boards_t bid, json_object* key, unsigned char* barray)
{
  int idx = -1;
  int lkey = -1;

  lkey = decipherLookupKey(json_object_get_string(key));

  if (lkey == -1)
  {
   log_info("No shifter pin defined");
   return;
  }

  // access table with lkey and bid to get the location to place data in barray
  idx = keyLookupTable[bid][lkey];
  barray[idx + shiftPosAdjTable[bid]] = 0x41;
}

void populateMacrosPosition (enum ipac_boards_t bid, json_object* macros, unsigned char* barray)
{
  int lkey = -1;
  int idx = -1;
  int pos = -1;
  int idx2015 = 168;

  json_object *tmp = NULL;

  json_object_object_foreach(macros, key, macro)
  {
    switch (bid)
    {
      case PRE_IPAC2_BOARD:
      case PRE_IPAC4_BOARD:
      case PRE_MINIPAC_BOARD:
        lkey = decipherLookupMacroKey(key);

        if (lkey == -1)
          continue;

        // access table with lkey and bid to get the location to place data in barray
        idx = macroLookupTable[bid][lkey];

        if (idx == -1)
        {
          log_warn("The macro given does not have a position for the given board");
          continue;
        }

        debug("macro:%s,\tvalue idx: %i", key, idx);
        /* Populate board array with macros */
        for (pos = 0; pos < json_object_array_length(macro); pos++)
        {
          tmp = json_object_array_get_idx(macro, pos);
          barray[idx + pos] = convertIPACKey(bid, tmp);
        }
       break;

       case IPAC2_BOARD:
       case IPAC4_BOARD:
       case JPAC_BOARD:
       case MINIPAC_BOARD:
         /* Insert E0 through FE into the barray */
         barray[idx2015] = convertIPACKey(bid, json_object_new_string(key));
         debug("macro:%s,\tvalue idx: %i", key, idx2015);

         /* Populate board array with macros */
         idx2015++;
         for (pos = 0; pos < json_object_array_length(macro); pos++)
         {
           tmp = json_object_array_get_idx(macro, pos);
           barray[idx2015 + pos] = convertIPACKey(bid, tmp);
         }

         /* Move the idx2015 to the next available location */
         idx2015 += pos;
       break;
    }
  }
}

bool writeIPACSeriesUSB (unsigned char* barray, int size, uint16_t vendor, uint16_t product, int interface, int autoconnect, bool transfer)
{
  libusb_context *ctx = NULL;
  libusb_device  *device = NULL;

  struct libusb_device_descriptor descriptor;
  struct libusb_device_handle *handle = NULL;

  unsigned char mesg[IPACSERIES_MESG_LENGTH] = {0x03,0,0,0,0};

  bool result = true;

  int pos = 0;
  int ret = 0;

  if (transfer)
  {
    handle = openUSB(ctx, vendor, product, interface, autoconnect);

    if (!handle)
    {
      result = false;
      goto error;
    }
 
    debug ("Determine which interface to use...");
    device = libusb_get_device(handle);

    if (!device)
    {
      result = false;
      goto error;
    }

    libusb_get_device_descriptor (device, &descriptor);
    if ((descriptor.idVendor == 0xd208 && descriptor.idProduct == 0x310) || (descriptor.bcdDevice >= 0x40 && descriptor.bcdDevice < 0x56))
    {
      debug ("No Game Controller interface");
      interface = IPACSERIES_NGC_INTERFACE;
    }
    else
    {
      debug ("Game Controller interface");
      interface = IPACSERIES_INTERFACE;
    }
  }
  
  debug ("Claiming interface...");
  if (!claimInterface(handle, interface, true))
  {
    result = false;
    goto error;
  }

  while (pos < size)
  {
    memcpy(&mesg[1], &barray[pos], 4);

    debug ("Writing (%i): %x, %x, %x, %x", pos, mesg[1], mesg[2], mesg[3], mesg[4]);
    if (transfer)
    {
      ret = libusb_control_transfer(handle,
                                    UM_REQUEST_TYPE,
                                    UM_REQUEST,
                                    IPACSERIES_VALUE,
                                    interface,
                                    mesg,
                                    IPACSERIES_MESG_LENGTH,
                                    UM_TIMEOUT);
      debug ("Write result: %i", ret);
    }
    pos+=4;
  }

exit:
  if (transfer)
  {
    closeUSB(ctx, handle, interface);
  }
  else
  {
    log_info ("board array was not written out!!!");
  }
  return result;

error:
  return result;
}

