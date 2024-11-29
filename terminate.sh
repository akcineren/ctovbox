#!/bin/bash

# Unmount the image
umount ./mount

# Get the loop device
if [ -f loop_device.info ]; then
    LOOP_DEVICE=$(cat loop_device.info)

    # Detach the loop device
    losetup -d $LOOP_DEVICE

    # Remove the loop_device.info file
    rm -f loop_device.info
else
    echo "Error: loop_device.info not found."
    exit 1
fi

# Remove the symbolic link
rm -f ./<device-file>
