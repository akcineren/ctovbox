#!/bin/bash

# Variables
IMG_NAME="storage_vgc.img"

# Run terminate.sh first
echo "Terminating resources..."
./terminate.sh

# Remove the disk image
if [ -f "$IMG_NAME" ]; then
    echo "Removing disk image $IMG_NAME..."
    rm -f $IMG_NAME
else
    echo "No disk image found."
fi

echo "All resources have been purged."
