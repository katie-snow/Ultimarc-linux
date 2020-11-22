# Script to install udev rule and the X11 configuration file.
# Update the specified mame.ini file for the aimtrak guns from Ultimarc
# Author: Katie Snow

import argparse
import os.path
import subprocess
import sys 

from os import path

def udevRule():
  fileStr = '/etc/udev/rules.d/65-aimtrak.rules'

  if (not (path.exists(fileStr))):
    sudoPriv()
    print('Writing ' + fileStr)
    with open (fileStr, 'w') as f:
      f.write('# Set mode & disable libinput handling to avoid X11 picking up the wrong interfaces/devices.\n')
      f.write('SUBSYSTEMS=="usb", ATTRS{idVendor}=="d209", ATTRS{idProduct}=="160*", MODE="0666", ENV{ID_INPUT}="", ENV{LIBINPUT_IGNORE_DEVICE}="1"\n')

      f.write('\n# For ID_USB_INTERFACE_NUM==2, enable libinput handling.\n')
      f.write('SUBSYSTEMS=="usb", ATTRS{idVendor}=="d209", ATTRS{idProduct}=="160*", ENV{ID_USB_INTERFACE_NUM}=="02", ENV{ID_INPUT}="1", ENV{LIBINPUT_IGNORE_DEVICE}="0"\n')
  else:
    print(fileStr + ' aready exists!  Not overwriting!')

def x11Config():
  dirStr = '/etc/X11/xorg.conf.d/'
  dirStrAlt = '/etc/X11/Xsession.d/'
  fileStr = '60-aimtrak.conf'

  if (not (path.exists(dirStr))):
    dirStr = dirStrAlt

  if (not (path.exists(dirStr + fileStr))):
    sudoPriv()
    print('Writing ' + dirStr + fileStr)
    with open (dirStr + fileStr, 'w') as f:
      f.write('Section "InputClass"\n')
      f.write('    Identifier      "AimTrak Guns"\n')
      f.write('    MatchDevicePath "/dev/input/event*"\n')
      f.write('    MatchUSBID      "d209:160*"\n')
      f.write('    Driver          "libinput"\n')
      f.write('    Option          "Floating" "yes"\n')
      f.write('    Option          "AutoServerLayout" "no"\n')
      f.write('EndSection\n')
  else:
    print(dirStr + fileStr + ' aready exists!  Not overwriting!')

def sudoPriv():
  # Verify sudo privleges
  if (os.geteuid() != 0):
    os.execvp('sudo', ['sudo', 'python3'] + sys.argv)

def mameConfig(file):
  nums = None

  # Edit mame file
  if (path.exists(file)):
    print ('Found file: ' + file)

    with open(file, 'r') as mame:
      mameData = mame.readlines()

    res = [i for i, s in enumerate(mameData) if 'lightgun ' in s]
    if (res): mameData[res[0]] = 'lightgun                  1\n'

    res = [i for i, s in enumerate(mameData) if 'offscreen_reload' in s]
    if (res): mameData[res[0]] = 'offscreen_reload          1\n'

    res = [i for i, s in enumerate(mameData) if 'lightgun_device' in s]
    if (res): mameData[res[0]] = 'lightgun_device           lightgun\n'

    res = [i for i, s in enumerate(mameData) if 'lightgunprovider' in s]
    if (res): mameData[res[0]] = 'lightgundevice            x11\n'

    mameData.append('\n#\n')
    mameData.append('# SDL lightgun indexes\n')
    mameData.append('#\n')

    multiple = input('Is there more than 1 AimTrak installed (y/n)?')
    if (multiple == 'y'):
      # Run command xinput
      subprocess.run('xinput')
   
      # Have user enter Ultimarc id's
      nums = input('Enter Ultimarc id numbers: ').split()

      # Add the id numbers to the file
      if (len(nums) > 0): mameData.append('lightgun_index1      "' + nums[0] + '"\n')
      if (len(nums) > 1): mameData.append('lightgun_index2      "' + nums[1] + '"\n')
      if (len(nums) > 2): mameData.append('lightgun_index3      "' + nums[2] + '"\n')
      if (len(nums) > 3): mameData.append('lightgun_index4      "' + nums[3] + '"\n')
    else:
      # Add Ultimarc Ultimarc to the first index
      mameData.append('lightgun_index1      "Ultimarc Ultimarc"\n')

    with open(file, 'w') as mame:
      mameData = "".join(mameData)
      mame.write (mameData)
  else:
    print ('Unable to find file: ' + file)

def main(argv):
  parser = argparse.ArgumentParser(description='Setup rules and config files for Aimtrak guns.', epilog='First execute the sys argument, then reboot the machine, last execute the mame argument.')

  group = parser.add_mutually_exclusive_group(required=True)
  group.add_argument('-s', '--sys', help='Add udev and x11 files', action='store_true')
  group.add_argument('-m', '--mame', help='Change mame config file, PATH full path to the mame.ini file', metavar='PATH')
  args = parser.parse_args()

  if (args.mame):
    mameConfig(args.mame)
  elif (args.sys):
    udevRule()
    x11Config()

if __name__ == "__main__":
  main(sys.argv[1:])
