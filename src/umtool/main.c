/*
 ============================================================================
 Name        : main.c
 Author      : Robert Abram, Katie Snow
 Version     :
 Copyright   : Copyright 2014 Robert Abram, Katie Snow
 Description : Ultimarc command line utility
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libs/ultimarc.h>
#include <libs/ulboard.h>

typedef struct args
{
  int help;
} args;

int
main (int argc, char **argv)
{
  int idx;
  int retVal;

  ulboard board;

  args args;
  args.help = 0;

  for (idx = 1; idx < argc; ++idx)
  {
    if (strcmp (argv[idx], "-h") == 0 || strcmp (argv[idx], "--help") == 0)
      args.help = 1;
  }

  if (args.help)
  {
    printf ("umtool [-h] [--help] [-m] [--m] [config files...]\n");
    printf ("-h | --help\t\t Prints this information\n");
    printf ("-m | --m\t\t File provided has multiple configuration\n");
    printf ("config files\t\t JSON Configuration files to be processed\n");
    retVal = EXIT_SUCCESS;
    goto exit;
  }

  for (idx = 1; idx < argc; ++idx)
  {
    printf ("Loading %s...\n", argv[idx]);
    retVal = ulValidateConfigFileStr (argv[idx], &board);

    if (retVal == 0)
    {
      retVal = ulWriteToBoardFileStr(argv[idx], &board);
    }
  }

  exit: return retVal;
}
