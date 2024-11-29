#!/bin/bash
# startup.sh - Mount and prepare the virtual disk

IMG_FILE="storage_vgc.img"
DEVICE="/dev/loop0"
SYMLINK="./<device-file>"
MOUNT_DIR="./mount"

# Check and create mount directory if not exists
if [ ! -d "$MOUNT_DIR" ]; then
    mkdir -p "$MOUNT_DIR"
    echo "Mount directory $MOUNT_DIR created."
fi

# Attach image to loop device
losetup $DEVICE $IMG_FILE || { echo "Failed to attach image to loop device."; exit 1; }

# Create filesystem on the image (only if it doesn't already exist)
if [ "$(blkid -o value -s TYPE $DEVICE 2>/dev/null)" == "" ]; then
    mkfs.ext4 $DEVICE || { echo "Failed to format loop device."; exit 1; }
    echo "Filesystem created on $DEVICE."
fi

# Mount the image
mount $DEVICE $MOUNT_DIR || { echo "Failed to mount image."; exit 1; }
echo "Image mounted on $MOUNT_DIR."

# Create a symbolic link to the device file
ln -sf $DEVICE $SYMLINK
echo "Symbolic link $SYMLINK created."
