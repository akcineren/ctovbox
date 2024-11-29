#!/bin/bash

# purge.sh
# This script unmounts the disk image, detaches the loop device, removes the symbolic link, and deletes the disk image file.

IMAGE="storage_vgc.img"
MOUNT_DIR="mount"
DEVICE_LINK="device-file"

# Call terminate.sh to unmount and detach
./terminate.sh

# Remove the disk image file
rm -f "$IMAGE"
echo "Disk image '$IMAGE' has been removed."

exit 0
