#!/bin/bash

IMAGE="storage_vgc.img"
IMAGE_SIZE="100M"

if [ -f "$IMAGE" ]; then
    echo "Image file '$IMAGE' exists. Overriding it."
    rm -f "$IMAGE"
fi

dd if=/dev/zero of="$IMAGE" bs=1M count=100
mkfs.ext4 "$IMAGE"

echo "Disk image '$IMAGE' created and formatted."
exit 0
