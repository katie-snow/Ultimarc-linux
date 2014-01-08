/*
 * ultimarc.c
 *
 *  Created on: Jan 5, 2014
 *      Author: rob
 */

#include <linux/module.h>    // included for all kernel modules
#include <linux/kernel.h>    // included for KERN_INFO
#include <linux/init.h>      // included for __init and __exit macros
#include <linux/usb.h>	 	 // included for usb macros

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Robert Abram, Katie Snow");
MODULE_DESCRIPTION("Ultimarc USB tools");

static struct usb_device *device;

static int um_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
  printk(KERN_INFO "Ultimarc drive (%04X:%04X) plugged in\n", id->idVendor, id->idProduct);
  struct usb_host_interface *iface_desc;
  struct usb_endpoint_descriptor *endpoint;
  int i;

  iface_desc = interface->cur_altsetting;
  printk(KERN_INFO "Ultimarc i/f %d now probed: (%04X:%04X)\n",
		  iface_desc->desc.bInterfaceNumber, id->idVendor, id->idProduct);
  printk(KERN_INFO "ID->bNumEndpoints: %02X\n",
		  iface_desc->desc.bNumEndpoints);
  printk(KERN_INFO "ID->bInterfaceClass: %02X\n",
		  iface_desc->desc.bInterfaceClass);

  for (i = 0; i < iface_desc->desc.bNumEndpoints; i++)
  {
	  endpoint = &iface_desc->endpoint[i].desc;

	  printk(KERN_INFO "ED[%d]->bEndpointAddress: 0x%02X\n",
			  i, endpoint->bEndpointAddress);
	  printk(KERN_INFO "ED[%d]->bmAttributes: 0x%02X\n",
			  i, endpoint->bmAttributes);
	  printk(KERN_INFO "ED[%d]->wMaxPacketSize: 0x%04X (%d)\n",
			  i, endpoint->wMaxPacketSize, endpoint->wMaxPacketSize);
  }

  device = interface_to_usbdev(interface);
  return 0;
}

static void um_disconnect(struct usb_interface *interface)
{
	printk(KERN_INFO "Ultimarc i/f %d now disconnected\n",
	            interface->cur_altsetting->desc.bInterfaceNumber);
}

static struct usb_device_id ultimarc_table[] =
{
		{ USB_DEVICE(0xd208, 0x0310) },
		{} /* Terminating entry */
};
MODULE_DEVICE_TABLE (usb, ultimarc_table);

static struct usb_driver ultimarc_driver =
{
    .name = "ultimarc_driver",
    .id_table = ultimarc_table,
    .probe = um_probe,
    .disconnect = um_disconnect,
};

static int __init um_init(void)
{
	printk ("My module worked!\n");

	return usb_register(&ultimarc_driver);
}

static void __exit um_exit(void)
{
	printk ("Unloading my module.\n");
	usb_deregister(&ultimarc_driver);
}

module_init(um_init);
module_exit(um_exit);





