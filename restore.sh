#!/bin/sh
sudo dfu-util -d 0483:df11 -a 0 -s 0x08000000:leave -D k1_max_stock_backup.bin