Ultimarc-linux
==============

Library and command line utility

#### Introduction:
This utility will configure the following Ultimarc boards; PACDrive, IPAC Ultimate, I-Pac 2, I-Pac 4, Mini-Pac, UltraStik 360 and PacLED64.  It uses json configuration files to configure the different boards.  It also supports the ability to change the device ID of the UltraStik 360 boards.  Allowing for the configuring of four different boards at once.

#### Required Libraries
To build this tool the following libraries need to be install on your system.  
* json-c (0.11),
* libusb-dev (1.0.18)
* libtool

#### UDEV Rule:
This utility requires folder permission changes to the usb device directories before it can do the configuration changes to the boards.  The udev rule in the base directory named 21-ultimarc.rules needs to be placed in /etc/udev/rules.d directory.  Placing the file in that directory usually requires root permissions.

#### Building Utility:
To build this project, at the base directory run the following commands
* ./configure
* make

If you need extra debug statements for the IPac boards then run the following
* ./configure CFLAGS='-DDEBUG'
* make

The executable will be in src/umtool directory and named umtool.out.

#### Project TODO's:
* MACRO support for boards that use MACRO's.
* Flash rate for PACLED64 board.

#### Donations:
<a href='https://pledgie.com/campaigns/26846'><img alt='Click here to lend your support to: Ultimarc-Linux and make a donation at pledgie.com !' src='https://pledgie.com/campaigns/26846.png?skin_name=chrome' border='0' ></a>
