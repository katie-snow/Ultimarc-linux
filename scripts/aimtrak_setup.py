#!/usr/bin/env python3

# Script to install udev rule and the X11 configuration file.
# Update the specified mame.ini file for the aimtrak guns from Ultimarc
# Author: Katie Snow

import argparse
import os.path
import subprocess
import sys 

from os import path

UDEV_RULES = """
# Set mode & disable libinput handling to avoid X11 picking up the wrong interfaces/devices.
SUBSYSTEMS=="usb", ATTRS{idVendor}=="d209", ATTRS{idProduct}=="160*", MODE="0666", ENV{ID_INPUT}="", ENV{LIBINPUT_IGNORE_DEVICE}="1"

# For ID_USB_INTERFACE_NUM==2, enable libinput handling.
SUBSYSTEMS=="usb", ATTRS{idVendor}=="d209", ATTRS{idProduct}=="160*", ENV{ID_USB_INTERFACE_NUM}=="02", ENV{ID_INPUT}="1", ENV{LIBINPUT_IGNORE_DEVICE}="0", SYMLINK+="input/mouse%n"
"""

X11_RULES = """
Section "InputClass"
    Identifier      "AimTrak Guns"
    MatchDevicePath "/dev/input/event*"
    MatchUSBID      "d209:160*"
    Driver          "libinput"
    Option          "Floating" "yes"
    Option          "AutoServerLayout" "no"
EndSection
"""

DESCRIPTION = """
Setup rules and config files for Ultimarc Aimtrak guns.
"""

EPILOG = """
First execute the "sys" argument to install the X11 and UDev rules. Afterwards, either
reboot the machine or run "sudo udevadm control --reload-rules && sudo udevadm trigger"
to reload the UDev rules. last execute the mame argument.'
"""

def udevRule():
  fileStr = '/etc/udev/rules.d/65-aimtrak.rules'

  if not path.exists(fileStr):
    sudoPriv()
    print('Writing ' + fileStr)
    open(fileStr, 'w').write(UDEV_RULES)
  else:
    print(f'{fileStr} already exists!  Not overwriting!')

def x11Config():
  dirStr = '/etc/X11/xorg.conf.d/'
  dirStrAlt = '/etc/X11/Xsession.d/'
  fileStr = '60-aimtrak.conf'

  configStr = os.path.join(dirStr, fileStr) if os.path.exists(dirStr) else os.path.join(dirStrAlt, fileStr)

  if not path.exists(configStr):
    sudoPriv()
    print(f'Writing {configStr}')
    open(configStr, 'w').write(X11_RULES)
  else:
    print(f'{configStr} already exists!  Not overwriting!')

def sudoPriv():
  # Verify sudo privleges
  if (os.geteuid() != 0):
    os.execvp('sudo', ['sudo', 'python3'] + sys.argv)

def mameConfig(file):

  mame_config = {
    'mouse': '1',
    'lightgun': '1',
    'offscreen_reload': '1',
    'lightgun_device': 'lightgun'
  }

  # Edit mame file
  if (path.exists(file)):
    print('Found file: ' + file)
    mameData = open(file, 'r').readlines()

    config = list()
    for line in mameData:
      line = line.strip()
      try:
        key, val = [i for i in line.split(' ') if i]
        if key in mame_config:
          config.append(f'{key.ljust(26)}{mame_config[key]}')
        elif key.startswith('lightgun_index'):  # Strip any lightgun_index lines.
          pass
        else:
          config.append(line)
      except ValueError:
        config.append(line)

    # Run command xinput and capture lightgun index ids.
    lines = subprocess.run('xinput', capture_output=True).stdout.decode('utf-8').split('\n')
    ids = [i.split('\t')[1].split('=')[1] for i in lines if i and "Ultimarc Ultimarc" in i]

    for pos in range(len(ids)):
      config.append(f'lightgun_index{pos+1:<12}{ids[pos]}')

    open(file, 'w').writelines([f'{i}\n' for i in config])
    print(f'Updated Mame config in: {file}.')
  else:
    print(f'Error: Unable to find file: {file}')

def advMameConfig():
  file = '/opt/retropie/configs/mame-advmame/advmame.rc'

  if not os.path.exists(file):
    print(f'Error: {file} not found.\n')
    print(f'Please enter path to the AdvMame "advmame.rc" file or enter to abort: ')
    file = input()

    if not file:
      print('\nAborting.\n')
      exit(0)

    if not os.path.exists(file):
      print(f'\nError: {file} not found. Aborting.\n')

  lines = open(file).readlines()

  config = list()
  for line in lines:
    line = line.strip()
    if line.startswith('device_mouse '):
      config.append('device_mouse raw')
    else:
      config.append(line)

  open(file, 'w').writelines([f'{i}\n' for i in config])
  print('Updated AdvMame config.')

def main(argv):

  parser = argparse.ArgumentParser(description=DESCRIPTION, epilog=EPILOG)

  group = parser.add_mutually_exclusive_group(required=True)
  group.add_argument('-s', '--sys', help='Add udev and x11 rules', action='store_true')
  group.add_argument('-m', '--mame', help='Change mame config file, PATH full path to the mame.ini file', metavar='PATH')
  group.add_argument('--advmame', help='Update AdvMame config', action='store_true')
  args = parser.parse_args()

  if (args.mame):
    mameConfig(args.mame)
  elif (args.advmame):
    advMameConfig()
  elif (args.sys):
    udevRule()
    x11Config()
    print('\nrun "sudo udevadm control --reload-rules && sudo udevadm trigger" or restart machine to apply changes.\n')

if __name__ == "__main__":
  main(sys.argv[1:])
