#!/bin/bash
#https://wiki.linaro.org/LMG/Kernel/AndroidConfigFSGadgets#ADB_gadget_configuration

cd /sys/kernel/config/usb_gadget/
mkdir -p g1
cd g1

#Set default Vendor and Product IDs for now
echo 0x18d1 > idVendor
echo 0x4E26 > idProduct

#Create English strings and add random deviceID
mkdir strings/0x409
echo 0123459876 > strings/0x409/serialnumber

#Update following if you want to
echo "Lineo" > strings/0x409/manufacturer
echo "FAM" > strings/0x409/product

#Create gadget configuration
mkdir configs/c.1
mkdir configs/c.1/strings/0x409
echo "Conf 1" > configs/c.1/strings/0x409/configuration
echo 120 > configs/c.1/Maxpower

#Create Adb FunctionFS function
#And link it to the gadget configuration
#stop adbd
mkdir -p functions/ffs.adb
mkdir /dev/usb-ffs
mkdir /dev/usb-ffs/adb

killall adbd

mount -o uid=2000,gid=2000 -t functionfs adb /dev/usb-ffs/adb
ln -s functions/ffs.adb configs/c.1/ffs.adb

adbd &

echo "38100000.dwc3" > UDC