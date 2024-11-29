#!/bin/bash

IMAGE="storage_vgc.img"
DEVICE="/dev/loop0"
SYMLINK="<device-file>"
MOUNT_DIR="mount"

echo "Starting up..."

# Create mount directory if not exists
if [ ! -d "$MOUNT_DIR" ]; then
    echo "Mount directory does not exist. Creating..."
    mkdir $MOUNT_DIR
fi

# Attach loop device
losetup $DEVICE $IMAGE
if [ $? -ne 0 ]; then
    echo "Error attaching device file to image."
    exit 1
fi

# Mount the image
mount $DEVICE $MOUNT_DIR
if [ $? -ne 0 ]; then
    echo "Error mounting the disk image."
    exit 1
fi

# Create symbolic link
ln -sf $DEVICE $SYMLINK

echo "Disk image mounted and ready to use."
