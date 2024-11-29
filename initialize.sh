#!/bin/bash
# initialize.sh - Creates the storage_vgc.img file

IMG_FILE="storage_vgc.img"

# Create or overwrite the disk image
echo "Initializing disk image: $IMG_FILE"
dd if=/dev/zero of=$IMG_FILE bs=1M count=64 status=progress
echo "Disk image $IMG_FILE created."
