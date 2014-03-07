/*
 * ipac.c
 *
 *  Created on: Feb 20, 2014
 *      Author: Katie Snow
 */

/* C */
#include <stdio.h>
#include <string.h>

/* Unix */
#include <json.h>
#include <libusb-1.0/libusb.h>

/* Local */
#include "ipac.h"

bool
isIPAC (json_object *jobj)
{
  /* TODO: Check for 'version' and 'keys' objects exist
   * and are types number and array
   * and the version is correct and the array is size 200
   */
  json_object* tmp = NULL;
  if (json_object_object_get_ex(jobj, "version", &tmp))
  {
    if (json_object_get_int(tmp) == IPAC_VERSION)
    {
      if (json_object_object_get_ex(jobj, "keys", &tmp) &&
          json_object_object_get_ex(jobj, "device", &tmp))
      {
        if (strncmp(json_object_get_string(tmp), IPAC_DEVICE, 4) == 0)
        {
          return true;
        }
      }
    }
  }

  return false;
}

char
convert (json_object *jobj)
{
  char retval = 0x00;
  const char* str = json_object_get_string(jobj);

  if (strlen(str) > 0)
  {
    switch(str[0])
    {
      case 'A':
        retval = 0x1C;
        if (strlen(str) == 4)
        {
          /* APPS */
          retval = 0x66;
        }
        else if (strlen(str) == 3)
        {
          /* APP */
          retval = 0xAF;
        }
        break;
      case 'B':
        retval = 0x32;
        if (strlen(str) > 1)
        {
          /* BKSP */
          retval = 0x66;
        }
        break;
      case 'C':
        retval = 0x21;
        if (strlen(str) > 1)
        {
          /* CAPS */
          retval = 0x58;
        }
        break;
      case 'D':
        retval = 0x23;
        retval = 0x1C;
        if (strlen(str) == 6)
        {
          /* DELETE */
          retval = 0xE0;
        }
        else if (strlen(str) == 7)
        {
          /* D ARROW */
          retval = 0xF2;
        }
        break;
      case 'E':
        retval = 0x24;
        if (strlen(str) > 1)
        {
          switch (str[1])
          {
            case 'r':
              /* Err:520 */
              retval = 0x55;
              break;
            case 'N':
              if (str[2] == 'T')
              {
                /* ENTER */
                retval = 0x5A;
              }
              else if (str[2] == 'D')
              {
                /* END */
                retval = 0xE0;
              }
              else
              {
                /* Set back to E */
                retval = 0x24;
              }
              break;
            case 'S':
              /* ESC */
              retval = 0x76;
              break;
            default:
              /* Set back to E */
              retval = 0x24;
              break;
          }
        }
        break;
      case 'F':
        retval = 0x28;
        if (strlen(str) > 1)
        {
          switch (str[1])
          {
            case '1':
              if (strcmp(str, "F1") == 0)
              {
                retval = 0x05;
              }
              else if (strcmp(str, "F10") == 0)
              {
                retval = 0x09;
              }
              else if (strcmp(str, "F11") == 0)
              {
                retval = 0x78;
              }
              else if (strcmp(str, "F12") == 0)
              {
                retval = 0x07;
              }
              break;
            case '2':
              /* F2 */
              retval = 0x06;
              break;
            case '3':
              /* F3 */
              retval = 0x04;
              break;
            case '4':
              /* F4 */
              retval = 0x0C;
              break;
            case '5':
              /* F5 */
              retval = 0x03;
              break;
            case '6':
              /* F6 */
              retval = 0x0B;
              break;
            case '7':
              /* F7 */
              retval = 0x83;
              break;
            case '8':
              /* F8 */
              retval = 0x0A;
              break;
            case '9':
              /* F9 */
              retval = 0x01;
              break;
          }
        }
        break;
      case 'G':
        retval = 0x34;
        break;
      case 'H':
        retval = 0x33;
        if (strlen(str) > 0)
        {
          /* HOME */
          retval = 0xE0;
        }
        break;
      case 'I':
        retval = 0x43;
        if (strlen(str) > 0)
        {
          /* INSERT */
          retval = 0xE0;
        }
        break;
      case 'J':
        retval = 0x3B;
        break;
      case 'K':
        retval = 0x42;
        if (strlen(str) > 0)
        {
          switch (str[3])
          {
            case '/':
              retval = 0xE0;
              break;
            case '*':
              retval = 0x7C;
              break;
            case '-':
              retval = 0x7B;
              break;
            case '+':
              retval = 0x79;
              break;
            case 'E':
              retval = 0xE0;
              break;
            case '.':
              retval = 0x71;
              break;
            case '0':
              retval = 0x70;
              break;
            case '1':
              retval = 0x69;
              break;
            case '2':
              retval = 0x72;
              break;
            case '3':
              retval = 0x7A;
              break;
            case '4':
              retval = 0x6B;
              break;
            case '5':
              retval = 0x73;
              break;
            case '6':
              retval = 0x74;
              break;
            case '7':
              retval = 0x6C;
              break;
            case '8':
              retval = 0x75;
              break;
            case '9':
              retval = 0x7D;
              break;
          }
        }
        break;
      case 'L':
        retval = 0x4B;
        if (strlen(str) > 0)
        {
          switch (str[3])
          {
            case 'H':
              /* L SHFT */
              retval = 0x12;
              break;
            case 'T':
              /* L CTRL */
              retval = 0x14;
              break;
            case 'U':
              /* L GUI */
              retval = 0xE0;
              break;
            case 'L':
              /* L ALT */
              retval = 0x11;
              break;
            case 'R':
              /* L ARROW */
              retval = 0xEB;
              break;
          }
        }
        break;
      case 'M':
        retval = 0x3A;
        break;
      case 'N':
        retval = 0x31;
        if (strlen(str) > 1)
        {
          /* NUM */
          retval = 0x77;
        }
        break;
      case 'O':
        retval = 0x44;
        break;
      case 'P':
        retval = 0x4D;
        if (strcmp(str, "Power") == 0)
        {
          retval = 0xB7;
        }
        else if (strcmp(str, "PG DN") == 0)
        {
          retval = 0xE0;
        }
        else if (strcmp(str, "PG UP") == 0)
        {
          retval = 0xE0;
        }
        else if (strcmp(str, "PAUSE") == 0)
        {
          retval = 0xE1;
        }
        else if (strcmp(str, "PRNT SCRN") == 0)
        {
          retval = 0xE0;
        }
        break;
      case 'Q':
        retval = 0x15;
        break;
      case 'R':
        retval = 0x2D;
        if (strlen(str) > 0)
        {
          switch (str[3])
          {
            case 'H':
              /* R SHFT */
              retval = 0x59;
              break;
            case 'T':
              /* R CTRL */
              retval = 0xE0;
              break;
            case 'U':
              /* R GUI */
              retval = 0xE0;
              break;
            case 'L':
              /* R ALT */
              retval = 0xE0;
              break;
            case 'R':
              /* R ARROW */
              retval = 0xF4;
              break;
          }
        }
        break;
      case 'S':
        retval = 0x1B;
        if (strlen(str) > 1)
        {
          switch (str[1])
          {
            case 'P':
              /* SPACE */
              retval = 0x29;
              break;
            case 'C':
              /* SCROLL */
              retval = 0x7E;
              break;
            case 'l':
              /* Sleep */
              retval = 0xBF;
              break;
          }
        }
        break;
      case 'T':
        retval = 0x2C;
        if (strlen(str) > 1)
        {
          /* TAB */
          retval = 0x0D;
        }
        break;
      case 'U':
        retval = 0x3C;
        if (strlen(str) > 1)
        {
          /* U ARROW */
          retval = 0xF5;
        }
        break;
      case 'V':
        retval = 0x2A;
        if (strlen(str) > 1)
        {
          if (strcmp(str, "VolumeUp") == 0)
          {
            retval = 0xB8;
          }
          else if (strcmp(str, "VolumeDown") == 0)
          {
            retval = 0xB9;
          }
        }
        break;
      case 'W':
        retval = 0x1D;
        if (strlen(str) > 1)
        {
          if(strcmp(str, "WinLeft") == 0)
          {
            retval = 0x9F;
          }
          else if (strcmp(str, "WinRight") == 0)
          {
            retval = 0xA7;
          }
          else if (strcmp(str, "Wake") == 0)
          {
            retval = 0xDE;
          }
        }
        break;
      case 'X':
        retval = 0x22;
        break;
      case 'Y':
        retval = 0x35;
        break;
      case 'Z':
        retval = 0x1A;
        break;
      case '0':
        retval = 0x45;
        break;
      case '1':
        retval = 0x16;
        break;
      case '2':
        retval = 0x1E;
        break;
      case '3':
        retval = 0x26;
        break;
      case '4':
        retval = 0x25;
        break;
      case '5':
        retval = 0x2E;
        break;
      case '6':
        retval = 0x36;
        break;
      case '7':
        retval = 0x3D;
        break;
      case '8':
        retval = 0x3E;
        break;
      case '9':
        retval = 0x46;
        break;
      case '`':
        retval = 0x0E;
        break;
      case '-':
        retval = 0x4E;
        break;
      case '\\':
        retval = 0x5D;
        break;
      case '[':
        retval = 0x54;
        break;
      case ']':
        retval = 0x5B;
        break;
      case ';':
        retval = 0x4C;
        break;
      case ' ':
        retval = 0x52;
        break;
      case '.':
        retval = 0x49;
        break;
      case '/':
        retval = 0x4A;
        break;
      default:
        retval = 0x00;
        break;
    }
  }

  return retval;
}

bool
updateBoard (json_object *jobj)
{
  libusb_context *ctx = NULL;
  struct libusb_device_handle *handle = NULL;

  json_object *keys = NULL;
  json_object *key  = NULL;

  int ipac_idx = 4;
  int pos = 0;
  int idx = 0;
  int ret = 0;

  bool result = false;

  char header[4] = {0x50, 0xdd, 0x00, 0x00};
  char data[IPAC_DATA_SIZE];
  unsigned char *mesg = (unsigned char*) malloc (5);
  mesg[0] = IPAC_REPORT;

  /* Open USB communication */
  ret = libusb_init(&ctx);
  if (ret < 0)
  {
    printf ("libusb_init failed: %i\n", ret);
    result = false;
    goto error;
  }
  libusb_set_debug(ctx, 3);

  handle = libusb_open_device_with_vid_pid(ctx, IPAC_VENDOR, IPAC_PRODUCT);
  if (!handle)
  {
    printf ("Unable to open IPAC device\n");
    result = false;
    goto error;
  }

  /* detach the kernel driver */
  if(libusb_kernel_driver_active(handle, IPAC_INTERFACE) == 1)
  {
    printf ("Kernel Driver Active\n");
    if(libusb_detach_kernel_driver(handle, IPAC_INTERFACE) == 0) //detach it
      printf ("Kernel Driver Detached!\n");
  }

  ret = libusb_claim_interface(handle, IPAC_INTERFACE);
  if (ret < 0)
  {
    printf ("Unable to claim interface\n");
  }
  /* Setup data to send to board */
  memset (&data, 0, sizeof(data));
  memcpy (&data, &header, sizeof(header));

  json_object_object_get_ex(jobj, "keys", &keys);
  for (idx = 0; idx < json_object_array_length(keys); ++ idx)
  {
    key = json_object_array_get_idx(keys, idx);
    data[ipac_idx] = convert (key);
    ++ipac_idx;
  }

  while (pos < IPAC_DATA_SIZE)
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
    //printf ("Writing out the following data (%i): %x, %x, %x, %x, %x\n", pos, mesg[0], mesg[1], mesg[2], mesg[3], mesg[4]);
    //printf ("Write result: %i\n", ret);
  }

  libusb_release_interface(handle, IPAC_INTERFACE);

exit:
  free (mesg);
  libusb_close(handle);
  libusb_exit(ctx);
  return result;

error:
  free (mesg);
  libusb_exit(ctx);
  return result;
}
