#!/bin/bash

IMAGE_FILE="storage_vgc.img"

echo "Initializing disk image..."
if [ -f "$IMAGE_FILE" ]; then
    echo "Disk image exists. Overriding..."
    rm -f "$IMAGE_FILE"
fi

dd if=/dev/zero of=$IMAGE_FILE bs=1M count=100
mkfs.ext4 $IMAGE_FILE
echo "Disk image initialized: $IMAGE_FILE"
