#!/bin/sh
# Tim Sutton 2007
# Copy supportibng libraries (except Qt) to Marble bundle
# and make search paths for them relative to bundle
#
# Change prefix if you built to a different install dest
#
PREFIX=/Applications/Marble.app/
#set -x
install_name_tool -change $HOME/dev/cpp/marble/build/src/lib/libmarblewidget.5.dylib \
  @executable_path/lib/libmarblewidget.5.dylib \
  ${PREFIX}/Contents/MacOS/marble

# Remove any debug etc symbols - comment this line out if you are debugging!
strip -x ${PREFIX}/Contents/MacOS/marble
