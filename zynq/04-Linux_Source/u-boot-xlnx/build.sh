echo "make ARCH=arm CROSS_COMPILE=arm-xilinx-linux-gnueabi- $*"
make ARCH=arm CROSS_COMPILE=arm-xilinx-linux-gnueabi- distclean
make ARCH=arm CROSS_COMPILE=arm-xilinx-linux-gnueabi- zynq_zturn_config
make ARCH=arm CROSS_COMPILE=arm-xilinx-linux-gnueabi-

