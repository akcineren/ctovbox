#!/bin/bash

# Variables
MOUNT_DIR="mount"
DEVICE_FILE="/dev/loop0"
SYMLINK_NAME="<device-file>"

# Unmount the image
if mount | grep -q "$MOUNT_DIR"; then
    echo "Unmounting the image..."
    sudo umount $MOUNT_DIR
else
    echo "No image is mounted."
fi

# Detach the loop device
if losetup -l | grep -q "$DEVICE_FILE"; then
    echo "Detaching the loop device..."
    sudo losetup -d $DEVICE_FILE
fi

# Remove the symbolic link
if [ -L "$SYMLINK_NAME" ]; then
    echo "Removing symbolic link..."
    rm -f $SYMLINK_NAME
fi

echo "Disk image is safely unplugged."
