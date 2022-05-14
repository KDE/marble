#!/bin/bash
set -x

git clone https://github.com/openstreetmap/tirex.git
pushd tirex
make deb
popd
