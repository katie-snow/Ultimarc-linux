/*
 * Hidraw Userspace Example
 *
 * Copyright (c) 2010 Alan Ott <alan@signal11.us>
 * Copyright (c) 2010 Signal 11 Software
 *
 * The code may be used by anyone for any purpose,
 * and can serve as a starting point for developing
 * applications using hidraw.
 */

/* Linux */
#include <linux/types.h>
#include <linux/input.h>
#include <linux/hidraw.h>

/*
 * Ugly hack to work around failing compilation on systems that don't
 * yet populate new version of hidraw.h to userspace.
 */
#ifndef HIDIOCSFEATURE
#warning Please have your distro update the userspace kernel headers
#define HIDIOCSFEATURE(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x06, len)
#define HIDIOCGFEATURE(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x07, len)
#endif

/* Unix */
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/* C */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

const char *bus_str(int bus);

int main(int argc, char **argv)
{
	int fd;
	int i, res, desc_size = 0;
	char buf[256];
	struct hidraw_report_descriptor rpt_desc;
	struct hidraw_devinfo info;

	/* Open the Device with non-blocking reads. In real life,
	   don't use a hard coded path; use libudev instead. */
	fd = open("/dev/hidraw2", O_RDWR|O_NONBLOCK);

	if (fd < 0) {
		perror("Unable to open device");
		return 1;
	}

	memset(&rpt_desc, 0x0, sizeof(rpt_desc));
	memset(&info, 0x0, sizeof(info));
	memset(buf, 0x0, sizeof(buf));

	/* Get Report Descriptor Size */
//	res = ioctl(fd, HIDIOCGRDESCSIZE, &desc_size);
	if (res < 0)
		perror("HIDIOCGRDESCSIZE");
	else
		printf("Report Descriptor Size: %d\n", desc_size);

	/* Get Report Descriptor */
	rpt_desc.size = desc_size;
//	res = ioctl(fd, HIDIOCGRDESC, &rpt_desc);
	if (res < 0) {
		perror("HIDIOCGRDESC");
	} else {
		printf("Report Descriptor:\n");
		for (i = 0; i < rpt_desc.size; i++)
			printf("%hhx ", rpt_desc.value[i]);
		puts("\n");
	}

	/* Get Raw Name */
//	res = ioctl(fd, HIDIOCGRAWNAME(256), buf);
	if (res < 0)
		perror("HIDIOCGRAWNAME");
	else
		printf("Raw Name: %s\n", buf);

	/* Get Physical Location */
	res = ioctl(fd, HIDIOCGRAWPHYS(256), buf);
	if (res < 0)
		perror("HIDIOCGRAWPHYS");
	else
		printf("Raw Phys: %s\n", buf);

	/* Get Raw Info */
//	res = ioctl(fd, HIDIOCGRAWINFO, &info);
	if (res < 0) {
		perror("HIDIOCGRAWINFO");
	} else {
		printf("Raw Info:\n");
		printf("\tbustype: %d (%s)\n",
			info.bustype, bus_str(info.bustype));
		printf("\tvendor: 0x%04hx\n", info.vendor);
		printf("\tproduct: 0x%04hx\n", info.product);
	}

	/* Set Feature */
	buf[0] = 0x00; /* Report Number */
	buf[1] = 0xff;
	buf[2] = 0xff;
	buf[3] = 0xff;
//	res = ioctl(fd, HIDIOCSFEATURE(4), buf);
	if (res < 0)
		perror("HIDIOCSFEATURE");
	else {
		printf("ioctl HIDIOCGFEATURE returned: %d\n", res);
                for (i = 0; i < res; i++)
                        printf("%hhx ", buf[i]);
                puts("\n");
        } 

	/* Get Feature */
	buf[0] = 0x0; /* Report Number */
//	res = ioctl(fd, HIDIOCGFEATURE(256), buf);
	if (res < 0) {
		perror("HIDIOCGFEATURE");
	} else {
		printf("ioctl HIDIOCGFEATURE returned: %d\n", res);
		printf("Report data (not containing the report number):\n\t");
		for (i = 0; i < res; i++)
			printf("%hhx ", buf[i]);
		puts("\n");
	}



	/* Send a Report to the Device */
	buf[0] = 0x03; /* Report Number */
        static char ipac_data[200] = { 0x50, 0xdd, 0x00, 0x00, 
			   0x10, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,   /* 2  */
                           0x11, 0x34, 0x12, 0x23, 0x22, 0x2D, 0x2B, 0x1C,   /* 3 */
                           0x16, 0x1B, 0x1E, 0x15, 0x2E, 0x1D, 0x36, 0x43,   /* 4 */
                           0x21, 0x42, 0x2A, 0x3B, 0x4B, 0x0E, 0x0D, 0x2E,   /* 5 */
                           0x5A, 0x00, 0x4D, 0x00, 0x00, 0x00, 0x00, 0x00,   /* 6 */
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00,   /* 7 */
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* 8 */
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* 9 */
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* 10 */
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* 11 */
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* 12 */
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* 13 */
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* 14 */
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* 15 */
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* 16 */
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* 17 */
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* 18 */
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* 19 */
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* 20 */
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* 21 */
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* 22 */
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* 23 */
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* 24 */
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* 25 */
                           0x00, 0x00, 0x00, 0x00 }; /* 26 */
        int pos = 0;
       
        while (pos < 200)
        {
	   memcpy(&buf[1], &ipac_data[pos], 4);
	   pos+=4;


	 


	   res = write(fd, buf, 5);
	   if (res < 0) {
		printf("Error: %d\n", errno);
		perror("write");
	   } else {
		printf("write() wrote %d bytes\n", res);
	   }
         }

	/* Get a report from the device */
//	res = read(fd, buf, 16);
	if (res < 0) {
		perror("read");
	} else {
		printf("read() read %d bytes:\n\t", res);
		for (i = 0; i < res; i++)
			printf("%hhx ", buf[i]);
		puts("\n");
	}
	close(fd);
	return 0;
}

const char *
bus_str(int bus)
{
	switch (bus) {
	case BUS_USB:
		return "USB";
		break;
	case BUS_HIL:
		return "HIL";
		break;
	case BUS_BLUETOOTH:
		return "Bluetooth";
		break;
	case BUS_VIRTUAL:
		return "Virtual";
		break;
	default:
		return "Other";
		break;
	}
}

