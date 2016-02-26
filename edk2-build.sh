#!/bin/bash

usage()
{
	printf "\nUsage: edk2-build.sh -b <target> -p <platform>\n";
	printf "Example: ./edk2-build.sh -b DEBUG -p Armada7040 -t GCC48\n";
	printf "\n";
	printf "Mandatory parameters:\n";
	printf "\t-b\tTarget type. Accepts:\tDEBUG,RELEASE\n";
	printf "\t-p\tPlatform name. Accepts:\tArmada7040_rz, Apn806\n";
	printf "\t-t\tToolchain version. Accepts:\tGCC45, GCC46 ... GCC49\n";

	printf "Optional parameters:\n";
	printf "\t-c\tClean build. remove all build artifacts)\n";
	printf "\n";
	printf "Environment Variables:\n";
	printf "\t<GCCver>_AARCH64_PREFIX     Cross compiler to build EDK2\n";
	printf "\n";
	exit 1;
}

target=0
platform=0
clean=NO
toolchain=0

# read all input arguments
while [[ $# > 0 ]]
do
key="$1"

case $key in
	-p|--platform)
	platform="$2"
	shift # past argument
	;;
	-t|--toolchain)
	toolchain="$2"
	shift # past argument
	;;
	-b|--target)
	target="$2"
	shift # past argument
	;;
	-l|--lib)
	LIBPATH="$2"
	shift # past argument
	;;
	-c|--clean)
	clean=YES
	;;
	*)
	# unknown option
	;;
esac
shift # past argument or value
done

if [ "$platform" -eq 0 ]; then
	printf "\n *** Error: Please set platform name\n"
	usage
fi

if [ "$target" -eq 0 ]; then
	printf "\n *** Error: Please set platform name\n"
	usage
fi

if [ "$toolchain" -eq 0 ]; then
	printf "\n *** Error: Please set toolchain version (GCC45 - GCC49)\n"
	usage
fi

case $toolchain in
	GCC45);&
	GCC46);&
	GCC47);&
	GCC48);&
	GCC49);;
	*)
	printf "\n *** Error: Unsupported toolchain \"$toolchain\"\n"
	usage
	;;
esac
toolchain_env="${toolchain}_AARCH64_PREFIX"

if [ -z "${!toolchain_env}" ]; then
	printf " *** Error: Please set environment variable ${toolchain_env}\n";
	usage
fi

dsc_file=0
output_dir=0
bin_file=0
bin_ext_file=0
# Locate the DSC file
case $platform in
	Apn806);&
	Armada7040_rz)
	plat_dir="OpenPlatformPkg/Platforms/Marvell/Armada/"
	dsc_file="$plat_dir/${platform}.dsc"
	bin_ext_file="$plat_dir/Binary/${platform}-spl.bin"
	output_dir="Build/${platform}/${target}_$toolchain/FV"
	bin_file="${output_dir}/${platform}_EFI.bin"
	;;
	*)
	printf "\n *** Error: Unknown platform \"$platform\"\n"
	usage
	;;
esac

# Configure Make
printf "\n##### [Setting up Environment]\t#####\n\n";
printf "Platform	= ${platform}\n"
printf "Target		= ${target}\n"
printf "Clean build	= ${clean}\n"
printf "Compiler	= ${toolchain_env}\n"

source edksetup.sh
if [ $? -ne 0 ]; then
	printf "**** Error: Failed setting up EDK2 environment\n"
	exit 1
fi

if [ $clean = "YES" ]; then
	printf "Cleaning build env\n"
	rm -rf ./Build/*
fi

printf "\n##### [Building Base Tools]\t#####\n\n";
make -C BaseTools
if [ $? -ne 0 ]; then
	printf "**** Error: Failed building base tools\n"
	exit 1
fi

printf "\n##### [Building EDK2 ]\t#####\n\n";
build -a AARCH64 -t $toolchain -b $target -p $dsc_file
if [ $? -ne 0 ]; then
	printf "**** Error: Failed building EDK2\n"
	exit 1
fi

printf "\n##### [Building Output files ]\t#####\n\n";
rm -rf $bin_file
mv $output_dir/ARMADA7040_EFI.fd $bin_file
if [ $? -ne 0 ]; then
	printf "**** Error: Failed creating binary file\n"
	exit 1
fi
printf "Binary image:\t$bin_file\n";

