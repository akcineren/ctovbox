#!/bin/bash

IMAGE="storage_vgc.img"
MOUNT_DIR="mount"
DEVICE_LINK="device-file"

LOOP_DEVICE=$(losetup -j "$IMAGE" | cut -d':' -f1)

if [ -z "$LOOP_DEVICE" ]; then
    echo "No loop device found for '$IMAGE'."
    exit 1
fi
sudo umount "$MOUNT_DIR"
sudo losetup -d "$LOOP_DEVICE"
rm -f "$DEVICE_LINK"
exit 0
