#!/bin/bash

IMAGE_FILE="storage_vgc.img"
MOUNT_DIR="mount"
DEVICE_FILE="device_file"
SYMLINK_FILE="<device-file>"

echo "Starting up..."
if [ ! -d "$MOUNT_DIR" ]; then
    echo "Mount directory does not exist. Creating..."
    mkdir $MOUNT_DIR
fi

if [ ! -e "$IMAGE_FILE" ]; then
    echo "Disk image not found. Run initialize.sh first."
    exit 1
fi

LOOP_DEVICE=$(losetup -f)
losetup $LOOP_DEVICE $IMAGE_FILE
ln -s $LOOP_DEVICE $SYMLINK_FILE
mount $LOOP_DEVICE $MOUNT_DIR

echo "Image mounted and ready at $MOUNT_DIR"
