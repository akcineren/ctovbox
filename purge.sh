#!/bin/bash

IMAGE_FILE="storage_vgc.img"
MOUNT_DIR="mount"

echo "Purging..."
./terminate.sh

if [ -f "$IMAGE_FILE" ]; then
    rm -f $IMAGE_FILE
    echo "Removed $IMAGE_FILE"
fi

if [ -d "$MOUNT_DIR" ]; then
    rm -rf $MOUNT_DIR
    echo "Removed $MOUNT_DIR"
fi
