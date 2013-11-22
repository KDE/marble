#!/bin/bash

# A script to download Marble Qt translations from KDE's SVN,
# transform them to binary .qm format while limiting the scope
# of translations to driving instructions. The output is a set
# of routing-instructions_$lang.qm files that can be used to
# have the routing-instructions binary translate driving 
# instructions according to the system's locale.
#
# This program is free software licensed under the GNU LGPL. You can
# find a copy of this license in LICENSE.txt in the top directory of
# the source code.
#
# Copyright 2010     Dennis Nienhüser <earthwings@gentoo.org>
#

set -e

workdir="$(mktemp -d)"

# Translations are loaded from SVN trunk. Can be switched to
# TAG="4.6.0"
# prefix="svn://anonsvn.kde.org/home/kde/tags/KDE/${TAG}/kde-l10n/"
# where ${TAG} is something like 4.6.0
prefix="svn://anonsvn.kde.org/home/kde/trunk/l10n-kde4"

echo "Processing translations, please wait. This can take some time..."
svn -q export "${prefix}/subdirs" "${workdir}/subdirs"
untranslated=""
for i in $(cat "${workdir}/subdirs")
do
	if svn -q export "${prefix}/${i}/messages/kdeedu/marble_qt.po" "${workdir}/marble_qt.po" 2>/dev/null
        then
          # Limit translations to RoutingInstruction.cpp
          # Assumption: All translations needed for routing-instructions are contained in this file
          grep -A 4 "#: src/lib/routing/instructions/RoutingInstruction.cpp:" "${workdir}/marble_qt.po" | sed 's/^--$//' > "${workdir}/marble_fi.po"

          # Convert the gettext format to Qt's ts format. Set the translation context to QObject
          # Assumption: All translated strings are invoked by QObject::tr (from a private class)
          # and not from a QObject derived class. Otherwise this needs to be adjusted
	  lconvert "${workdir}/marble_fi.po" -o routing-instructions_${i}.ts
          sed -i -e 's@<context>@<context>\n    <name>QObject</name>@' -e '/<name><\/name>/d' routing-instructions_${i}.ts

          # Convert to binary .qm file format
          lconvert routing-instructions_${i}.ts -o routing-instructions_${i}.qm

          # Examine number of translations, debug output
          total="$(grep '</message>' routing-instructions_${i}.ts | wc -l)"
          missing="$(grep '<translation type="unfinished">' routing-instructions_${i}.ts | wc -l)"
          if [[ "${total}" == "${missing}" ]]
          then
            untranslated="${untranslated} ${i}"
            rm routing-instructions_${i}.qm
          else
            echo "$i: $((total-missing)) of ${total} driving instructions translated"
          fi
          rm routing-instructions_${i}.ts
        else
          untranslated="${untranslated} ${i}"
        fi
done

rm "${workdir}/marble_qt.po"
rm "${workdir}/marble_fi.po"
rm "${workdir}/subdirs"
rmdir "${workdir}"
echo -e "Languages not yet translated:\n${untranslated}" | fold -s
