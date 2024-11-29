#!/bin/bash

# Set the image name
IMG_NAME="storage_vgc.img"

# Check if image already exists and inform the user
if [ -f "$IMG_NAME" ]; then
    echo "Disk image $IMG_NAME already exists. Overriding it."
    rm -f "$IMG_NAME"
fi

# Create a new image of 100MB
dd if=/dev/zero of=$IMG_NAME bs=1M count=100
echo "Disk image $IMG_NAME created."
