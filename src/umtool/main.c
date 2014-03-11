/*
 ============================================================================
 Name        : main.c
 Author      : Robert Abram, Katie Snow
 Version     :
 Copyright   : Copyright 2014 Robert Abram, Katie Snow
 Description : Ultimarc command line utility
 ============================================================================
 */

#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

 #define DEFAULT_DEVICE "/dev/ultimarc0"
// #define DEFAULT_DEVICE "/dev/ultimarc5"


#include <libs/ultimarc.h>

int main(void) {
	char *dev;
	int   fd;
	int   retVal;

/*	int i, j, k = 0;

	char buf3[32];

	MapRow1=N,N,N,N,N,N,N,N,N
	MapRow2=N,N,N,N,N,N,N,N,N
	MapRow3=N,N,N,N,N,N,N,N,N
	MapRow4=C,C,C,C,C,C,C,C,C
	MapRow5=C,C,C,C,C,C,C,C,C
	MapRow6=C,C,C,C,C,C,C,C,C
	MapRow7=S,S,S,S,S,S,S,S,S
	MapRow8=S,S,S,S,S,S,S,S,S
	MapRow9=S,S,S,S,S,S,S,S,S

	// PROBLEM: Calibration data used is not quite right.  Joystick won't return to center
	char buf[] = { 0x50, 0x09, 0x10, 0x30, 0x58,
				   0x86, 0x114, 0x142, 0x170, 0x198, 0x226,  Header information
				   0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
				   0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
				   0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
				   0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
				   0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
				   0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
				   0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
				   0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
				   0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
				   0x00, 0x00, 0x00, 0x00   filler to get to 96
				   };  50

	fprintf (stdout, "sizeof(buf) = %d\n", sizeof(buf));

	int result = write_to_device(DEFAULT_DEVICE, &buf, sizeof(buf));
	printf("write_to_device returned '%d'.", result);

	return 0;


	dev = DEFAULT_DEVICE;
    fd = open(dev, O_RDWR);

    puts("Opening...");
    if (fd == -1)
    {
        perror("open");
        exit(1);
    }

    retVal = write (fd, NULL, 0);
    for (i=0; i < 3; ++i)
      {
        retVal = write (fd, buf+(32*i), sizeof(buf)/3);
      }
    retVal = write (fd, NULL, 0);


	for (i = 0; i < 200; i = i + 5)
    {
	  k = i;
	  fprintf (stdout, "i=%d\n", i);
      for (j = 0; j < 5; ++j)
        {
          buf3[j] = buf[k];
          ++k;
        }

      retVal = write (fd, &buf3, sizeof(buf3));
      fprintf (stdout, "retVal=%d\n", retVal);
    }

	if (retVal < 0)
	{
		fprintf (stderr, "Error occurred\n");
	}

	close (fd);*/

	//foo();

	//char *test = "{ \"admin\" : true, \"reviewer\" : false, \"author\" : true }";
	//validate_json(test);


	updateBoard ("ipac.json");
	updateBoard ("ultistik.json");

	return EXIT_SUCCESS;
}
