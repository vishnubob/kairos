#!/bin/sh
set -e

version=1.8.5
prefix=/usr/share
wget -q -O- https://downloads.arduino.cc/arduino-${version}-linux64.tar.xz | tar xJC ${prefix}
ln -s /usr/local/share/arduino-${version} ${prefix}/arduino
