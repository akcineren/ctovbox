#!/bin/bash

# terminate.sh
# This script unmounts the disk image and cleans up the loop device and symbolic link.

IMAGE="storage_vgc.img"
MOUNT_DIR="mount"
DEVICE_LINK="device-file"

# Find the loop device associated with the image
LOOP_DEVICE=$(losetup -j "$IMAGE" | cut -d':' -f1)

if [ -z "$LOOP_DEVICE" ]; then
    echo "No loop device found for '$IMAGE'."
    exit 1
fi

# Unmount the mount directory
sudo umount "$MOUNT_DIR"
echo "Unmounted '$MOUNT_DIR'."

# Detach the loop device
sudo losetup -d "$LOOP_DEVICE"
echo "Loop device '$LOOP_DEVICE' detached."

# Remove the symbolic link
rm -f "$DEVICE_LINK"
echo "Symbolic link '$DEVICE_LINK' removed."

exit 0
