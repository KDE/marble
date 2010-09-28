# - Find Qt Mobility libs
#
# This module determines whether Qt Mobility is installed.
#
# Copyright (c) 2010, Dennis Nienhüser, <earthwings@gentoo.org>
#

include(FindPkgConfig)

pkg_check_modules(QtSystemInfo QtSystemInfo>=1.0.1)
