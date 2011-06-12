# - Find Qt Location libs
#
# This module determines whether QtLocation is installed.
#
# Copyright (c) 2011, Daniel Marth, <danielmarth@gmx.at>
#

include(FindPkgConfig)

pkg_check_modules(QtLocation QtLocation>=1.0.1)