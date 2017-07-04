#!/bin/bash

sudo rm -rf /tmp/ext_file

build -a AARCH64 -t ${2} -b ${1} -p OpenPlatformPkg/Platforms/Marvell/Armada/Armada80x0McBin.dsc
cd ../atf
export ARCH=arm64
export CROSS_COMPILE=/opt/gcc-linaro-5.3.1-2016.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-
export BL33=/home/mw/git/edk2/Build/Armada80x0McBin-AARCH64/${1}_${2}/FV/ARMADA_EFI.fd
make USE_COHERENT_MEM=0 LOG_LEVEL=20 MV_DDR_PATH=/home/mw/git/mv-ddr PLAT=a80x0_mcbin all fip
scp /home/mw/git/edk2/Build/Armada80x0McBin-AARCH64/${1}_${2}/FV/ARMADA_EFI.fd 10.3.210.166:/tftpboot/users/mw/a8k/ARMADA_EFI.fd_mcbin${3}
scp build/a80x0_mcbin/release/flash-image.bin 10.3.210.166:/tftpboot/users/mw/a8k/flash-image-a8k-mcbin.bin${3}
export ARCH=
cd -
