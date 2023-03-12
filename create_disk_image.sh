#!/bin/sh

DISK_SECTOR_BYTES=512
DISK_HEADS=2
DISK_9_SECTORS=9
DISK_15_SECTORS=15
DISK_18_SECTORS=18
DISK_DD_TRACKS=40
DISK_HD_TRACKS=80

DISK_360_BYTES=`expr $DISK_SECTOR_BYTES \* $DISK_9_SECTORS \* $DISK_DD_TRACKS \* $DISK_HEADS`   # 368640
DISK_720_BYTES=`expr $DISK_SECTOR_BYTES \* $DISK_9_SECTORS \* $DISK_HD_TRACKS \* $DISK_HEADS`   # 737280
DISK_1200_BYTES=`expr $DISK_SECTOR_BYTES \* $DISK_15_SECTORS \* $DISK_HD_TRACKS \* $DISK_HEADS` # 1228800
DISK_1440_BYTES=`expr $DISK_SECTOR_BYTES \* $DISK_18_SECTORS \* $DISK_HD_TRACKS \* $DISK_HEADS` # 1474560

fallocate -l $DISK_360_BYTES image_360k.img
fallocate -l $DISK_720_BYTES image_720k.img
fallocate -l $DISK_1200_BYTES image_1200.img
fallocate -l $DISK_1440_BYTES image_1440k.img

# define serial number of disk image. For fun we use the first 8 digits of pi :)
SERIAL=31415926

mformat -i image_360k.img -v game -N $SERIAL
mformat -i image_720k.img -v game -N $SERIAL
mformat -i image_1200.img -v game -N $SERIAL
mformat -i image_1440k.img -v game -N $SERIAL

mcopy -i image_360k.img installer/* ::
mcopy -i image_720k.img installer/* ::
mcopy -i image_1200.img installer/* ::
mcopy -i image_1440k.img installer/* ::

zip images.zip image*.img
