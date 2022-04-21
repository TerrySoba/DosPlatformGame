#!/bin/sh


DISK_SECTOR_BYTES=512
DISK_HEADS=2
DISK_9_SECTORS=9
DISK_15_SECTORS=15
DISK_18_SECTORS=18
DISK_DD_TRACKS=40
DISK_HD_TRACKS=80

DISK_360_BYTES=`expr $DISK_SECTOR_BYTES \* $DISK_9_SECTORS \* $DISK_DD_TRACKS \* $DISK_HEADS`
DISK_720_BYTES=`expr $DISK_SECTOR_BYTES \* $DISK_9_SECTORS \* $DISK_HD_TRACKS \* $DISK_HEADS`
DISK_1200_BYTES=`expr $DISK_SECTOR_BYTES \* $DISK_15_SECTORS \* $DISK_HD_TRACKS \* $DISK_HEADS`
DISK_1440_BYTES=`expr $DISK_SECTOR_BYTES \* $DISK_18_SECTORS \* $DISK_HD_TRACKS \* $DISK_HEADS`

echo $DISK_1200_BYTES

# fallocate -l 368640 image_360k.img
# fallocate -l 737280 image_720k.img
# fallocate -l 1228800 image_1200.img
# fallocate -l 1474560 image_1440k.img

# mformat -i image_360k.img -v game
# mformat -i image_720k.img -v game
# mformat -i image_1200.img -v game
# mformat -i image_1440k.img -v game

# mcopy -i image_360k.img release ::game
# mcopy -i image_720k.img release ::game
# mcopy -i image_1200.img release ::game
# mcopy -i image_1440k.img release ::game