#!/bin/bash

LOOPDEV=$(losetup -f)
MNT=mnt

[ ! -d $MNT ] && mkdir -p $MNT

sudo losetup $LOOPDEV floppy.img
sudo mount $LOOPDEV $MNT
sudo cp kernel $MNT/kernel
sudo umount $LOOPDEV
sudo losetup -d $LOOPDEV
