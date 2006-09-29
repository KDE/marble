#!/bin/bash
rm ../data/mwdbii/PGLACIER.PNT
../bin/svg2pnt -o ../data/mwdbii/PGLACIER.PNT ../data/vectorsource/glaciers.svg
cp ../data/mwdbii/PLAKE.PNT.orig ../data/mwdbii/PLAKE.PNT
../bin/svg2pnt -o ../data/mwdbii/PLAKE.PNT ../data/vectorsource/lakes.svg
cp ../data/mwdbii/PISLAND.PNT.orig ../data/mwdbii/PISLAND.PNT
../bin/svg2pnt -o ../data/mwdbii/PISLAND.PNT ../data/vectorsource/islands.svg

