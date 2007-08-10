#!/bin/sh
# Tim Sutton 2007
# Copy supportibng libraries (except Qt) to Marble bundle
# and make search paths for them relative to bundle

PREFIX=${HOME}/apps/Marble.app/
#set -x
install_name_tool -change $HOME/dev/cpp/marble/build/src/lib/libmarblewidget.5.dylib \
  @executable_path/lib/libmarblewidget.5.dylib \
  $HOME/apps/Marble.app/Contents/MacOS/marble

