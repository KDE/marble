#!/bin/bash
rm ../data/mwdbii/PGLACIER.PNT
./svg2pnt -o ../../data/mwdbii/PGLACIER.PNT ../../data/vectorsource/glaciers.svg
cp ../data/mwdbii/PLAKE.PNT.orig ../../data/mwdbii/PLAKE.PNT
./svg2pnt -o ../../data/mwdbii/PLAKE.PNT ../../data/vectorsource/lakes.svg
cp ../data/mwdbii/PISLAND.PNT.orig ../../data/mwdbii/PISLAND.PNT
./svg2pnt -o ../../data/mwdbii/PISLAND.PNT ../../data/vectorsource/islands.svg
