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

int main(int argc, char **argv) {
	int idx;
	int retVal;

	for (idx = 1; idx < argc; ++idx)
	{
	  if (strcmp(argv[idx], "-h") == 0 ||
	      strcmp(argv[idx], "--help") == 0)
	  {
	    printf ("umtool [-h] [--help] [config files...]\n");
	    printf ("-h | --help\t\t Prints this information\n");
	    printf ("config files\t\t JSON Configuration files to be processed\n");
	    retVal = EXIT_SUCCESS;
	    goto exit;
	  }
	}

	loadUltimarcConfigurations(argc, argv);

	exit:
	return retVal;
}
