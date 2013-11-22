#!/bin/bash

# A script to download Marble Qt translations from KDE's SVN.
#
# This program is free software licensed under the GNU LGPL. You can
# find a copy of this license in LICENSE.txt in the top directory of
# the source code.
#
# Copyright 2011     Dennis Nienhüser <earthwings@gentoo.org>
# Copyright 2013     Bernhard Beschow <bbeschow@cs.tu-berlin.de>
#

set -e

workdir="$(mktemp -d)"

#prefix="svn://anonsvn.kde.org/home/kde/branches/stable/l10n-kde4/"
#TAG="4.7.0"
#prefix="svn://anonsvn.kde.org/home/kde/tags/KDE/${TAG}/l10n-kde4/"
# Translations can also be loaded from SVN trunk, uncomment below.
prefix="svn://anonsvn.kde.org/home/kde/trunk/l10n-kde4"

echo "Downloading translations, please wait. This may take some time..."

svn -q export "${prefix}/subdirs" "${workdir}/subdirs"
for i in $(cat "${workdir}/subdirs")
do
  if svn -q export --force "${prefix}/${i}/messages/kdeedu/marble_qt.po" "${workdir}/marble_qt.po" 2>/dev/null
  then
    if svn -q export --force "${prefix}/${i}/messages/kdeedu/marble.po" "${workdir}/marble.po" 2>/dev/null
    then
      echo >> "${workdir}/marble_qt.po"
      cat "${workdir}/marble.po" >> "${workdir}/marble_qt.po"
      mv "${workdir}/marble_qt.po" marble-${i}.po
    fi
  fi
done

rm "${workdir}/marble.po"
rm "${workdir}/subdirs"
rmdir "${workdir}"

test -e CMakeLists.txt && touch CMakeLists.txt

echo "Done."
