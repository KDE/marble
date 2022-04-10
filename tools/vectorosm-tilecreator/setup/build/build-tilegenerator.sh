#!/bin/bash
# SPDX-License-Identifier: BSD-3-clause
# SPDX-FileCopyrightText: 2020-2022 Volker Krause <vkrause@kde.org>

function build-static-qt-module() {
    local module=$1
    shift
    cd
    git clone https://invent.kde.org/qt/qt/$module
    cd $module
    git checkout kde/5.15
    mkdir build
    cd build
    if [ $module == "qtbase" ]; then
        ../configure -prefix ${HOME}/qt-inst $@
    else
        ${HOME}/qt-inst/bin/qmake .. $@
    fi
    make -j 4
    make install
}

build-static-qt-module qtbase --release -qpa offscreen -no-pch -no-icu -no-dbus -no-glib -openssl-linked -no-feature-vulkan -no-feature-sql-mysql -no-feature-sql-psql -no-feature-sql-odbc -no-feature-sha3-fast -static -confirm-license -opensource -make libs -make tools
build-static-qt-module qtdeclarative -- -no-feature-qml-debug -no-feature-qml-devtools -no-feature-qml-preview -no-feature-qml-profiler
build-static-qt-module qtsvg

cd
git clone https://invent.kde.org/education/marble
cd marble
mkdir build
cd build
cmake -DSTATIC_BUILD=1 \
    -DCMAKE_PREFIX_PATH=${HOME}/qt-inst \
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
cp tools/vectorosm-tilecreator/marble-vectorosm-tilecreator /output/usr/bin/
