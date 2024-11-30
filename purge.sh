#!/bin/bash
IMAGE="storage_vgc.img"
MOUNT_DIR="mount"
DEVICE_LINK="device-file"
./terminate.sh
rm -f "$IMAGE"
exit 0
