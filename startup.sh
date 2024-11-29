#!/bin/bash

# Check if storage_vgc.img exists
if [ ! -f storage_vgc.img ]; then
    echo "Error: storage_vgc.img not found. Please run initialize.sh first."
    exit 1
fi

# Create mount directory if it doesn't exist
if [ ! -d mount ]; then
    mkdir mount
fi

# Attach storage_vgc.img to a loop device
LOOP_DEVICE=$(losetup -f)
losetup $LOOP_DEVICE storage_vgc.img

# Save loop device info to a file
echo $LOOP_DEVICE > loop_device.info

# Create a symbolic link to the device file in our directory
ln -s $LOOP_DEVICE ./<device-file>

# Mount the loop device to mount directory
mount $LOOP_DEVICE ./mount
