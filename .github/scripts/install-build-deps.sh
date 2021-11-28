#!/bin/sh
set -e

sudo apt-get purge -y libgcc-*-dev || true
sudo apt-get purge -y llvm-* clang-* || true
sudo apt-get install -y \
    autoconf \
    automake \
    build-essential \
    libtool \
    pkg-config \
    libboost-coroutine-dev \
    libboost-stacktrace-dev \
    libclang-dev \
    libgflags-dev \
    libgmp3-dev \
    libgoogle-glog-dev \
    libisl-dev \
    libntl-dev \
    libyaml-dev \
    llvm-dev \
    clang \
    python3-pip

sudo apt-get autoremove -y
sudo -H python3 -m pip install --upgrade pip==20.3.4
sudo -H python3 -m pip install cmake
wget -O - https://raw.githubusercontent.com/UCLA-VAST/tapa/master/install.sh | bash
sudo mkdir -p /opt/xilinx/xrt/include