/*
 ============================================================================
 Name        : main.c
 Author      : Robert Abram, Katie Snow
 Version     :
 Copyright   : Copyright 2014 Robert Abram, Katie Snow
 Description : Ultimarc command line utility
 ============================================================================
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define DEFAULT_DEVICE "/dev/ultimarc0"

int main(void) {
	char *dev;
	int   fd;
	int   retVal;

	puts("Nothing to do yet.");


	dev = DEFAULT_DEVICE;
	fd = open(dev, O_RDWR);

	if (fd == -1)
	{
		perror("open");
		exit(1);
	}

	retVal = write (fd, NULL, 0);

	if (retVal < 0)
	{
		fprintf (stderr, "Error occurred\n");
	}

	close (fd);
	return EXIT_SUCCESS;
}
