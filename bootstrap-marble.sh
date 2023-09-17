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

export MarbleOptions='-DWITH_DESIGNER_PLUGIN=OFF \
                      -DBUILD_MARBLE_TESTS=OFF \
                      -DBUILD_MARBLE_TOOLS=OFF \
                      -DBUILD_MARBLE_EXAMPLES=OFF \
                      -DBUILD_MARBLE_APPS=OFF \
                      -DBUILD_WITH_DBUS=OFF \
                      -DQTONLY=ON \
                      -DBUILD_TESTING=OFF'

cmake -G "$MAKEFILES_TYPE" . \
      -DCMAKE_INSTALL_PREFIX=/usr \
      -Wno-dev \
      $MarbleOptions \
      ..

