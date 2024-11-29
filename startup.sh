#!/bin/bash

# Variables
IMG_NAME="storage_vgc.img"
MOUNT_DIR="mount"
SYMLINK_NAME="<device-file>"

# Create mount directory if it doesn't exist
if [ ! -d "$MOUNT_DIR" ]; then
    echo "Creating mount directory..."
    mkdir "$MOUNT_DIR"
fi

# Attach the image to an available loop device
LOOP_DEVICE=$(sudo losetup -f --show $IMG_NAME)

# Create filesystem on the image if not already done
if ! sudo file -s $LOOP_DEVICE | grep -q "ext4"; then
    echo "Creating filesystem on the image..."
    sudo mkfs.ext4 $LOOP_DEVICE
fi

# Mount the image
sudo mount $LOOP_DEVICE $MOUNT_DIR

# Create a symbolic link
ln -sf $LOOP_DEVICE $SYMLINK_NAME

echo "Disk image $IMG_NAME is mounted as $LOOP_DEVICE and ready to use."
