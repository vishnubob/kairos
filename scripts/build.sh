#!/bin/sh

build_dir=/tmp/__arduino_build
if [ -d $build ]; then
	rm -rf $build_dir
fi
mkdir -p ${build_dir}/build
mkdir -p ${build_dir}/flashphoto
cp *.h *.ino ${build_dir}/flashphoto

pwd=$(pwd)
cd $build_dir/flashphoto

/usr/share/arduino/arduino-builder \
	-hardware /usr/share/arduino/hardware \
	-tools /usr/share/arduino/hardware/tools \
	-tools /usr/share/arduino/tools-builder \
	-fqbn arduino:avr:diecimila:cpu=atmega328 \
	-verbose \
	-build-path ${build_dir}/build \
	$(pwd)

cd ${pwd}
cp ${build_dir}/build/*.ino.hex .
