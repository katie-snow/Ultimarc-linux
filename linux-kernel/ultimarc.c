/*
 * ultimarc.c
 *
 *  Created on: Jan 5, 2014
 *      Author: rob
 */

#include <linux/module.h>    /* included for all kernel modules */
#include <linux/kernel.h>    /* included for KERN_INFO */
#include <linux/init.h>      /* included for __init and __exit macros */


#include <linux/usb.h>	 	 /* included for usb macros */
#include <linux/slab.h>		 /* included for kmalloc */

#include <stdbool.h>         /* boolean values */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Robert Abram, Katie Snow");
MODULE_DESCRIPTION("Ultimarc USB tools");

#define UM_USB_TIMEOUT 500

#define UM_REQUEST_PROGRAM_MODE 0xE9

#define UM_CTRL_BUFFER_SIZE  5
#define UM_CTRL_REQUEST_TYPE 0x21
#define UM_CTRL_REQUEST      9
#define UM_CTRL_VALUE        0x0203
#define UM_CTRL_INDEX        0x2

#define UM_CTRL_PROGRAM_MODE 0x09

struct um_usb {
	struct usb_device *device;
	struct usb_class_driver class;
	struct usb_interface *interface;

	/*  Control Message */
	char                   *ctrl_buffer;    /* buffer for control message */
	struct urb             *ctrl_urb;
	struct usb_ctrlrequest *ctrl_request;   /* setup packet information */
};

static struct usb_driver ultimarc_driver;

static void um_delete(struct um_usb *dev)
{
  /* Free data structures. */
  if (dev->ctrl_urb)
    usb_free_urb(dev->ctrl_urb);

  kfree(dev->ctrl_buffer);
  kfree(dev->ctrl_request);
  kfree(dev);
}

static void um_ctrl_callback (struct urb *urb)
{
	printk(KERN_INFO "In um_ctrl_callback");
}

static int um_open(struct inode *i, struct file *f)
{
  struct um_usb *dev = NULL;
  struct usb_interface *interface;

  int subMinor;
  int retVal = 0;

  subMinor = iminor(i);

  interface = usb_find_interface(&ultimarc_driver, subMinor);

  if(!interface)
  {
    printk(KERN_DEBUG "Can't find device for minor %d\n", subMinor);
    retVal = -ENODEV;
    goto exit;
  }

  dev = usb_get_intfdata(interface);
  if (!dev)
  {
    retVal = -ENODEV;
    goto exit;
  }

  f->private_data = dev;

exit:
  return retVal;
}

static int um_close(struct inode *i, struct file *f)
{
    return 0;
}

static ssize_t um_write(struct file *f, const char __user *buf, size_t cnt, loff_t *off)
{
  char buf2[300];

  struct um_usb *dev;
  int retVal = 0;
  dev = f->private_data;
  
  memset(&buf2, 0, 200);


  if (copy_from_user(&buf2, buf, cnt))
  {
    retVal = -EFAULT;
    goto exit;
  }

  retVal = usb_control_msg(dev->device,                         /* device */
                           usb_sndctrlpipe(dev->device, 0),     /* pipe */
                           UM_CTRL_REQUEST,                /* request */
                           UM_CTRL_REQUEST_TYPE,                     /* request type */
                           UM_CTRL_VALUE,                                   /* value */
                           UM_CTRL_INDEX,                                   /* index */
                           &buf2,                                /* buffer */
                           5,                                   /* buffer size */
                           HZ*5);                     /* timeout */

  printk(KERN_INFO "retVal = %d\n", retVal);



  //if (retVal < 0)
  //  printk(KERN_INFO "Error submitting data URB %d\n", retVal);

exit:
  return retVal;
}

static struct file_operations fops =
{
	.owner = THIS_MODULE,
    .open = um_open,
    .release = um_close,
    .write = um_write
};

static int um_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
  struct um_usb *dev = NULL;

  struct usb_device *udev;
  struct usb_host_interface *iface_desc;
  struct usb_endpoint_descriptor *endpoint;

  int i;
  int retval = -ENODEV;
  bool validInterface = false;

  iface_desc = interface->cur_altsetting;



  for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i)
  {
	  endpoint = &iface_desc->endpoint[i].desc;

	  if (((endpoint->bEndpointAddress & USB_ENDPOINT_DIR_MASK) == USB_DIR_IN) &&
	      ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_INT) &&
	      (endpoint->wMaxPacketSize == 32))
	  {
	    printk(KERN_INFO "Ultimarc card (%04X:%04X) plugged in\n", id->idVendor, id->idProduct);

	    printk(KERN_INFO "Ultimarc i/f %d now probed: (%04X:%04X)\n",
	            iface_desc->desc.bInterfaceNumber, id->idVendor, id->idProduct);
	    printk(KERN_INFO "ID->bNumEndpoints: %02X\n",
	            iface_desc->desc.bNumEndpoints);
	    printk(KERN_INFO "ID->bInterfaceClass: %02X\n",
	            iface_desc->desc.bInterfaceClass);

	    printk(KERN_INFO "ED[%d]->bEndpointAddress: 0x%02X\n",
                i, endpoint->bEndpointAddress);
        printk(KERN_INFO "ED[%d]->bmAttributes: 0x%02X\n",
                i, endpoint->bmAttributes);
        printk(KERN_INFO "ED[%d]->wMaxPacketSize: 0x%04X (%d)\n",
                i, endpoint->wMaxPacketSize, endpoint->wMaxPacketSize);
	    /*dev->int_in_endpoint = endpoint;*/
        validInterface = true;
	  }
  }

  if (!validInterface)
  {
    /* don't register for this interface */
    goto exit;
  }

  udev = interface_to_usbdev(interface);

  if (!udev)
  {
    printk(KERN_DEBUG "udev is NULL");
    goto exit;
  }

  dev = kzalloc(sizeof(struct um_usb), GFP_KERNEL);
  if (!dev)
  {
    printk(KERN_DEBUG "Unable to allocate memory for struct um_usb");
    retval = -ENOMEM;
    goto exit;
  }

  dev->device = udev;
  dev->interface = interface;

  dev->class.name = "usb/ultimarc%d";
  dev->class.fops = &fops;

  /* Set up the control URB */
  dev->ctrl_urb = usb_alloc_urb(0, GFP_KERNEL);
  if (!dev->ctrl_urb)
  {
    printk(KERN_DEBUG "Unable to allocate ctrl_urb");
    retval = -ENOMEM;
    goto error;
  }

  dev->ctrl_buffer = kzalloc(UM_CTRL_BUFFER_SIZE, GFP_KERNEL);
  if (!dev->ctrl_buffer)
  {
    printk(KERN_DEBUG "Unable to allocate ctrl_buffer");
    retval = -ENOMEM;
    goto error;
  }

  dev->ctrl_request = kzalloc(sizeof(struct usb_ctrlrequest), GFP_KERNEL);
  if (!dev->ctrl_request)
  {
    printk(KERN_DEBUG "Unable to allocate ctrl_request");
    retval = -ENOMEM;
    goto error;
  }

  dev->ctrl_request->bRequestType = UM_CTRL_REQUEST_TYPE;
  dev->ctrl_request->bRequest = UM_CTRL_REQUEST;
  dev->ctrl_request->wValue = cpu_to_le16(UM_CTRL_VALUE);
  dev->ctrl_request->wIndex = cpu_to_le16(UM_CTRL_INDEX);
  dev->ctrl_request->wLength = cpu_to_le16(UM_CTRL_BUFFER_SIZE);

  usb_fill_control_urb(dev->ctrl_urb, dev->device,
                       usb_sndctrlpipe(dev->device, 0),
                       (unsigned char *)dev->ctrl_request,
                       dev->ctrl_buffer,
                       UM_CTRL_BUFFER_SIZE,
                       um_ctrl_callback,
                       dev);

  usb_set_intfdata(interface, dev);

  if ((retval = usb_register_dev(interface, &dev->class)) < 0)
  {
	/* Something prevented us from registering this driver */
	printk(KERN_INFO "Not able to get a minor for this device.");
	usb_set_intfdata(interface, NULL);
	goto error;
  }
  else
  {
	printk(KERN_INFO "Minor obtained: %d\n", interface->minor);
  }

exit:
  return retval;

error:
  um_delete (dev);
  return retval;
}

static void um_disconnect(struct usb_interface *interface)
{
	struct um_usb *dev;

	dev = usb_get_intfdata(interface);
	usb_set_intfdata(interface, NULL);

	printk(KERN_INFO "Ultimarc i/f %d now disconnected\n",
	            interface->cur_altsetting->desc.bInterfaceNumber);
	usb_deregister_dev(interface, &dev->class);

	um_delete(dev);
}

static struct usb_device_id ultimarc_table[] =
{
		{ USB_DEVICE(0xd208, 0x0310) },    /* IPAC-2, IPAC-4, IPAC-MINI */
		{ USB_DEVICE(0xd209, 0x0501) },    /* Ultra-Stick 360 */
		{ USB_DEVICE(0xd209, 0x1401) },    /* PAC-LED 64 */
		{} /* Terminating entry */
};
MODULE_DEVICE_TABLE (usb, ultimarc_table);

static struct usb_driver ultimarc_driver =
{
    .name = "ultimarc_driver",
    .id_table = ultimarc_table,
    .probe = um_probe,
    .disconnect = um_disconnect
};

static int __init um_init(void)
{
	int result;

	/* Register this driver with the USB subsystem */
	if ((result = usb_register(&ultimarc_driver)))
	{
		printk(KERN_INFO "usb_register failed. Error number %d", result);
	}
	return result;
}

static void __exit um_exit(void)
{
	usb_deregister(&ultimarc_driver);
}

module_init(um_init);
module_exit(um_exit);





