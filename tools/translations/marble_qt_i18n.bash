#!/bin/bash

# A script to download Marble Qt translations from KDE's SVN,
# and transform them to binary .qm format.
#
# This program is free software licensed under the GNU LGPL. You can
# find a copy of this license in LICENSE.txt in the top directory of
# the source code.
#
# Copyright 2011     Dennis Nienhüser <earthwings@gentoo.org>
#

set -e

MERGE_TOOL="$(dirname ${0})/merge_ts_po"
test -x "${MERGE_TOOL}" || {
  MERGE_TOOL="$(mktemp)"
  g++ -o ${MERGE_TOOL} "$(dirname ${0})/merge_ts_po.cpp" -I /usr/include/qt4 -lQtCore
}

test -x "${MERGE_TOOL}" || { 
  echo "./merge_ts_po is not there. Please compile it: g++ -o merge_ts_po merge_ts_po.cpp -I /usr/include/qt4 -lQtCore."; exit 1; 
}

workdir="$(mktemp -d)"

#prefix="svn://anonsvn.kde.org/home/kde/branches/stable/l10n-kde4/"
#TAG="4.7.0"
#prefix="svn://anonsvn.kde.org/home/kde/tags/KDE/${TAG}/kde-l10n/"
# Translations can also be loaded from SVN trunk, uncomment below.
prefix="svn://anonsvn.kde.org/home/kde/trunk/l10n-kde4"

echo "Generating translation template"
lupdate $(find "$(dirname ${0})"/../../ -name "*.cpp" -o -name "*.h" -o -name "*.ui" | xargs echo) -ts "${workdir}/template.ts"

echo "Processing translations, please wait. This can take some time..."

svn -q export "${prefix}/subdirs" "${workdir}/subdirs"
for i in $(cat "${workdir}/subdirs")
do
	if svn -q export --force "${prefix}/${i}/messages/kdeedu/marble_qt.po" "${workdir}/marble_qt.po" 2>/dev/null
        then
          if svn -q export --force "${prefix}/${i}/messages/kdeedu/marble.po" "${workdir}/marble.po" 2>/dev/null
          then
            echo >> "${workdir}/marble_qt.po"
            cat "${workdir}/marble.po" >> "${workdir}/marble_qt.po"
            # Convert to binary .qm file format
            "${MERGE_TOOL}" "${workdir}/template.ts" "${workdir}/marble_qt.po" > "${workdir}/marble_qt.ts"
            sed -i 's@DownloadRegionDialog::Private@DownloadRegionDialog@' "${workdir}/marble_qt.ts"
            lconvert "${workdir}/marble_qt.ts" -o "marble_qt_${i}.qm"

            # If you need the .ts file, uncomment below
            #cp "${workdir}/marble_qt.ts" "marble_qt_${i}.ts"

            # If you need the .po files, uncomment below
            #cp "${workdir}/marble_qt.po" "marble_qt_${i}.po"
            #cp "${workdir}/marble.po" "marble_${i}.po"

            mv "marble_qt_${i}.qm" "marble-${i}"
            echo "marble-${i}"
          fi
        fi
done

rm "${workdir}/template.ts"
rm "${workdir}/marble.po"
rm "${workdir}/marble_qt.po"
rm "${workdir}/marble_qt.ts"
rm "${workdir}/subdirs"
rmdir "${workdir}"

echo "Done. The files marble-* contain the translations in .qm format"
