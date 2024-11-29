#!/bin/bash

MOUNT_DIR="mount"
SYMLINK_FILE="<device-file>"

echo "Terminating..."
if mountpoint -q $MOUNT_DIR; then
    umount $MOUNT_DIR
    echo "Unmounted $MOUNT_DIR"
fi

if [ -L "$SYMLINK_FILE" ]; then
    LOOP_DEVICE=$(readlink -f $SYMLINK_FILE)
    losetup -d $LOOP_DEVICE
    rm -f $SYMLINK_FILE
    echo "Device file detached and symbolic link removed"
fi
