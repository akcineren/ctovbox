#!/bin/bash

# initialize.sh
# This script creates a new disk image file 'storage_vgc.img'.
# If the image already exists, it will override the previous one.

IMAGE="storage_vgc.img"
IMAGE_SIZE="100M"  # Adjust the size as needed

# Remove existing image if it exists
if [ -f "$IMAGE" ]; then
    echo "Image file '$IMAGE' exists. Overriding it."
    rm -f "$IMAGE"
fi

# Create a new empty disk image
dd if=/dev/zero of="$IMAGE" bs=1M count=100

# Create a filesystem on the image (e.g., ext4)
mkfs.ext4 "$IMAGE"

echo "Disk image '$IMAGE' created and formatted."

exit 0
