/*
 ============================================================================
 Name        : common.h
 Author      : Robert Abram, Katie Snow
 Version     :
 Copyright   : Copyright 2014 Robert Abram, Katie Snow
 Description : Common functions between device libraries
 ============================================================================
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <libusb-1.0/libusb.h>


/**
 * Will attempt to open the USB product and claim the interface specified.
 * If successful ctx variable will be populated and the return handle will be populated,
 * otherwise they will be NULL;
 * autoconnect = 1 the kernel driver will be attached after we are done with the device
 * Only needed when changing the ID of the ultistik board
 */
struct libusb_device_handle* openUSB(libusb_context *ctx,
                                      uint16_t vendor,
                                      uint16_t product,
                                      int interface,
                                      int autoconnect);

/**
 * Releases the interface, closes the context pointer and the handle pointer
 */
void closeUSB(libusb_context *ctx,
              struct libusb_device_handle *handle,
              int interface);

#endif /* COMMON_H_ */
