#!/bin/bash

# SPDX-FileCopyrightText: 2008-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#
# Copy this script on root folder where are source code

#export VERBOSE=1

# We will work on command line using MinGW compiler
export MAKEFILES_TYPE='Unix Makefiles'

if [ ! -d "build" ]; then
    mkdir build
fi

cd build

cmake -G "$MAKEFILES_TYPE" \
      -DCMAKE_INSTALL_PREFIX=/usr \
      -DWITH_DESIGNER_PLUGIN=ON \
      -DBUILD_MARBLE_TESTS=ON \
      -DBUILD_MARBLE_TOOLS=ON \
      -DBUILD_MARBLE_EXAMPLES=ON \
      -DBUILD_MARBLE_APPS=ON \
      -DBUILD_WITH_DBUS=ON \
      -DQTONLY=ON \
      -DBUILD_TESTING=OFF \
      -Wno-dev \
      ..

