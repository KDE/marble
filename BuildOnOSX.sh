#! /bin/sh
set -xe
# Prefix can be set with: env PREFIX=/opt/local ./BuildOnOSX.sh
prefix=${PREFIX-/usr/local}

case "$prefix" in
  [\\/]* | ?:[\\/]* ) # Absolute
     ;;
  *)
     echo "'$prefix' is not an absolute path" >&2
     exit 1
     ;;
esac

# Find or create the build directory
if test -f Makefile && test -f ../CMakeLists.txt; then
  : # Nothing to do, we've been called from the build directory
else
  test -d build || mkdir build
  cd build
fi

# Build
cmake -DCMAKE_INSTALL_PREFIX="$prefix" -DQTONLY=ON ..
make

# Install
sudo make install

# Fix the resource location
resdir="$prefix/bin/marble.app/Contents/MacOS/Resources"
sudo mkdir -m 0755 -p "$resdir"
sudo ln -s -f ../../../../../share/apps/marble/data "$resdir/data"
# Fix the rights
sudo chmod -R a+rX "$prefix/bin/marble.app" "$prefix/share/apps/marble"
