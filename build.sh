#!/bin/sh

build_dir=/tmp/__arduino_build
if [ -d $build ]; then
	rm -rf $build_dir
fi
mkdir -p ${build_dir}

/usr/share/arduino/arduino-builder \
	-hardware /usr/share/arduino/hardware \
	-tools /usr/share/arduino/hardware/tools \
	-tools /usr/share/arduino/tools-builder \
	-libraries $(pwd)/libraries \
	-fqbn arduino:avr:diecimila:cpu=atmega328 \
	-verbose \
	-build-path ${build_dir} \
	$(pwd)

cp ${build_dir}/*.hex .
