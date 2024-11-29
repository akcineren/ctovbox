#!/bin/bash

IMAGE="storage_vgc.img"

echo "Initializing disk image..."
if [ -f "$IMAGE" ]; then
    echo "Disk image exists. Overwriting..."
fi

# Create a new image file (1GB size as an example)
dd if=/dev/zero of=$IMAGE bs=1M count=1024

echo "Disk image $IMAGE created successfully."
