# - Find Qt Mobility libs
#
# This module determines whether QtMobility is installed.
#
# Copyright (c) 2011, Daniel Marth, <danielmarth@gmx.at>
#

FIND_PATH( QTMOBILITY_INCLUDE_DIR NAMES qmobilityglobal.h PATH_SUFFIXES QtMobility )

FIND_PACKAGE_HANDLE_STANDARD_ARGS( QtMobility DEFAULT_MSG QTMOBILITY_INCLUDE_DIR )