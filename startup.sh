#!/bin/bash

# startup.sh
# This script mounts the disk image and prepares the virtual disk.
# It creates a loop device, attaches it to the image, and creates a symbolic link to the device file.

IMAGE="storage_vgc.img"
MOUNT_DIR="mount"
DEVICE_LINK="device-file"

# Create mount directory if it doesn't exist
if [ ! -d "$MOUNT_DIR" ]; then
    mkdir "$MOUNT_DIR"
    echo "Mount directory '$MOUNT_DIR' created."
fi

# Set up a loop device and attach it to the image
LOOP_DEVICE=$(sudo losetup -f --show "$IMAGE")

# Check if the loop device was created successfully
if [ -z "$LOOP_DEVICE" ]; then
    echo "Failed to set up loop device."
    exit 1
fi

echo "Loop device '$LOOP_DEVICE' attached to '$IMAGE'."

# Create a symbolic link to the device file in the current directory
ln -sf "$LOOP_DEVICE" "$DEVICE_LINK"

echo "Symbolic link '$DEVICE_LINK' created, pointing to '$LOOP_DEVICE'."

# Mount the loop device to the mount directory
sudo mount "$LOOP_DEVICE" "$MOUNT_DIR"

echo "Disk image mounted to '$MOUNT_DIR'."

exit 0
