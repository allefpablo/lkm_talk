#!/bin/sh

export ARCH=arm64
export CROSS_COMPILE=aarch64-linux-gnu-
# If Raspberry is using 32 bit OS, commment 2 lines above and uncomment below
# export ARCH=arm
# export CROSS_COMPILE=arm-linux-gnueabihf-
export KDIR='/home/allef/code/kernels/linux' # Change to the path where your kernel was downloaded