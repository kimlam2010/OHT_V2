#!/bin/bash
cd /home/orangepi/Desktop/OHT_V2/firmware_new/build
echo "=== Rebuilding firmware ==="
make -j4
echo ""
echo "=== Build status: $? ==="
ls -lh oht50_main 2>/dev/null

