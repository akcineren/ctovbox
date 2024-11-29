#!/bin/bash

# Unmount and detach
./terminate.sh

# Remove storage_vgc.img
if [ -f storage_vgc.img ]; then
    rm -f storage_vgc.img
    echo "storage_vgc.img has been deleted."
else
    echo "storage_vgc.img not found."
fi
