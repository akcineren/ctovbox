#!/bin/bash

# Variables
IMG_NAME="storage_vgc.img"
MOUNT_DIR="mount"
DEVICE_FILE="/dev/loop0"
SYMLINK_NAME="<device-file>"

# Detach loop device if it's already in use
if losetup -l | grep -q "$DEVICE_FILE"; then
    echo "Detaching existing loop device..."
    sudo losetup -d $DEVICE_FILE
fi

# Create mount directory if it doesn't exist
if [ ! -d "$MOUNT_DIR" ]; then
    echo "Creating mount directory..."
    mkdir "$MOUNT_DIR"
fi

# Set up a loop device
sudo losetup $DEVICE_FILE $IMG_NAME

# Create filesystem on the image if not already done
if ! sudo file -s $DEVICE_FILE | grep -q "ext4"; then
    echo "Creating filesystem on the image..."
    sudo mkfs.ext4 $DEVICE_FILE
fi

# Mount the image
sudo mount $DEVICE_FILE $MOUNT_DIR

# Create a symbolic link
ln -sf $DEVICE_FILE $SYMLINK_NAME

echo "Disk image $IMG_NAME is mounted and ready to use."
