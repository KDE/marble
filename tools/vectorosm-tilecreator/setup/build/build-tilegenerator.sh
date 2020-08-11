#!/bin/bash

cd
git clone https://invent.kde.org/education/marble
cd marble
mkdir build
cd build
cmake -DSTATIC_BUILD=1 \
    -DMARBLE_NO_WEBKITWIDGETS=TRUE \
    -DMARBLE_NO_DBUS=TRUE \
    -DBUILD_MARBLE_TOOLS=ON \
    -DBUILD_MARBLE_APPS=OFF \
    -DCMAKE_EXE_LINKER_FLAGS="-Wl,--as-needed" \
    -DCMAKE_BUILD_TYPE=Release ..

make -j 4

# extract the relevant output
mkdir -p /output/usr/bin
cp tools/vectorosm-tilecreator/marble-vectorosm-tirex-backend /output/usr/bin/
cp tools/vectorosm-tilecreator/marble-vectorosm-process-land-polygons /output/usr/bin/
