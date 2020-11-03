#!/bin/bash

cd
git clone https://github.com/openstreetmap/tirex.git
cd tirex
make deb

# extract the relevant output
cd
cp *.deb /output/
