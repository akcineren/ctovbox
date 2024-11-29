#!/bin/bash
# startup.sh - Mount and prepare the virtual disk

IMG_FILE="storage_vgc.img"
SYMLINK="./<device-file>"
MOUNT_DIR="./mount"

# Check and create mount directory if not exists
if [ ! -d "$MOUNT_DIR" ]; then
    mkdir -p "$MOUNT_DIR"
    echo "Mount directory $MOUNT_DIR created."
fi

# Attach image to a free loop device
LOOP_DEVICE=$(losetup --find --show $IMG_FILE)
if [ $? -ne 0 ]; then
    echo "Failed to attach image to loop device."
    exit 1
fi
echo "Image attached to $LOOP_DEVICE."

# Create filesystem on the image (only if it doesn't already exist)
if [ "$(blkid -o value -s TYPE $LOOP_DEVICE 2>/dev/null)" == "" ]; then
    mkfs.ext4 $LOOP_DEVICE || { echo "Failed to format loop device."; exit 1; }
    echo "Filesystem created on $LOOP_DEVICE."
fi

# Mount the image
mount $LOOP_DEVICE $MOUNT_DIR || { echo "Failed to mount image."; exit 1; }
echo "Image mounted on $MOUNT_DIR."

# Change ownership to current user
chown -R $(id -u):$(id -g) $MOUNT_DIR
chmod -R u+w $MOUNT_DIR
echo "Permissions adjusted for $MOUNT_DIR."

# Create a symbolic link to the device file
ln -sf $LOOP_DEVICE $SYMLINK
echo "Symbolic link $SYMLINK created."
