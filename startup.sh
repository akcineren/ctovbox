#!/bin/bash
IMAGE="storage_vgc.img"
MOUNT_DIR="mount"
DEVICE_LINK="device-file"

if [ ! -d "$MOUNT_DIR" ]; then
    mkdir "$MOUNT_DIR"
    echo "Mount directory '$MOUNT_DIR' created."
fi

LOOP_DEVICE=$(sudo losetup -f --show "$IMAGE")
if [ -z "$LOOP_DEVICE" ]; then
    echo "Failed to set up loop device."
    exit 1
fi
ln -sf "$LOOP_DEVICE" "$DEVICE_LINK"
sudo mount "$LOOP_DEVICE" "$MOUNT_DIR"
if [ ! -d "$MOUNT_DIR/bin" ]; then
    sudo cp -r bin "$MOUNT_DIR/"
    echo "'bin' directory copied into the disk image."
else
    echo "'bin' directory already exists in the disk image."
fi

exit 0
