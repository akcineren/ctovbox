#!/bin/bash

# Initialize the storage image
# Remove existing image file if it exists
if [ -f storage_vgc.img ]; then
    rm -f storage_vgc.img
fi

# Create a 100MB image file
dd if=/dev/zero of=storage_vgc.img bs=1M count=100

# Format it with ext4 filesystem
mkfs.ext4 storage_vgc.img

# Temporarily mount the image to create directories
# Create a temporary mount point
mkdir -p temp_mount

# Attach the image to a loop device
LOOP_DEVICE=$(losetup -f)
losetup $LOOP_DEVICE storage_vgc.img

# Mount the loop device
mount $LOOP_DEVICE temp_mount

# Create directories bin and src inside the image
mkdir temp_mount/bin
mkdir temp_mount/src

# Unmount and detach loop device
umount temp_mount
losetup -d $LOOP_DEVICE

# Remove temporary mount point
rmdir temp_mount
