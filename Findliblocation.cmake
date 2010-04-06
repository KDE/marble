# - Find liblocation libs - Maemo positioning
#
# This module determines whether Maemo's liblocation is installed.
#
# Copyright (c) 2010, Dennis Nienhüser, <earthwings@gentoo.org>
#

include(FindPkgConfig)

pkg_check_modules(liblocation liblocation>=0.102)
