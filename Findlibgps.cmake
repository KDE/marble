# - Find Gpsd libs - gps daemon library
#
# This module finds if Gpsd is installed.
#
# Copyright (c) 2006, Andrew Manson, <g.real.ate@gmail.com>
#

include(FindPkgConfig)

pkg_check_modules(libgps libgps>=2.35)
