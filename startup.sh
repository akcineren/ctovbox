#!/bin/bash

# startup.sh
# This script mounts the disk image and prepares the virtual disk.
# It creates a loop device, attaches it to the image, and creates a symbolic link to the device file.
# It also copies 'bin' and 'src' directories into the disk image after mounting.

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

# Copy 'bin' and 'src' directories into the mounted disk image
# Check if they exist in the mounted image to prevent duplication
if [ ! -d "$MOUNT_DIR/bin" ]; then
    sudo cp -r bin "$MOUNT_DIR/"
    echo "'bin' directory copied into the disk image."
else
    echo "'bin' directory already exists in the disk image."
fi

if [ ! -d "$MOUNT_DIR/src" ]; then
    sudo cp -r src "$MOUNT_DIR/"
    echo "'src' directory copied into the disk image."
else
    echo "'src' directory already exists in the disk image."
fi

echo "Startup process completed."

exit 0
