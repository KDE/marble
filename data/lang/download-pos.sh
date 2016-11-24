#!/bin/bash

# A script to download Marble Qt translations from KDE's SVN.
#
# To be used before cmake is called on the sources.
#
# This program is free software licensed under the GNU LGPL. You can
# find a copy of this license in LICENSE.txt in the top directory of
# the source code.
#
# Copyright 2016     Friedrich W. H. Kossebau <kossebau@kde.org>
#

# TODO: support updates properly (e.g. removing no longer existing po files)

has_subdirs=true
case "$1" in
    trunk)
        echo "Downloading from trunk"
        svn_path_prefix="svn://anonsvn.kde.org/home/kde/trunk/l10n-kf5"
        ;;
    stable)
        echo "Downloading from stable"
        svn_path_prefix="svn://anonsvn.kde.org/home/kde/branches/stable/l10n-kf5"
        ;;
    "")
	echo "Syntax: $0 trunk|stable|\"KDE Applications tag\" [lang]"
        exit 1
        ;;
    *)
        TAG=$1
        echo "Downloading from KDE Applications tag $1"
        svn_path_prefix="svn://anonsvn.kde.org/home/kde/tags/Applications/${TAG}/kde-l10n/5"
        has_subdirs=false
        ;;
esac

if [ ! -e data/lang ]; then
    echo "$0 needs to be invoked from the toplevel source dir."
    exit 1
fi

shift

workdir="$(mktemp -d)"
pofile="${workdir}//marble_qt.po"

if [ $# -gt 0 ] ; then
    languages="${*}"
else
    subdirs="${workdir}/subdirs"
    if [ "$has_subdirs" = true ] ; then
        svn -q export "${svn_path_prefix}/subdirs" ${subdirs}
    else
        # check if tag exists
        if svn info ${svn_path_prefix} > /dev/null 2>&1 ; then
            # tags don't have a subdirs file, so create one from the dirs present
            svn list ${svn_path_prefix} | egrep "/$"  | sed "s,/$,," > ${subdirs}
        else
            echo "There seems to be no tag $TAG."
            exit 1
        fi
    fi
    languages=$(cat ${subdirs})
fi

for i in ${languages}
do
    svn -q export "${svn_path_prefix}/${i}/messages/kdeedu/marble_qt.po" ${pofile} > /dev/null 2>&1
    # some languages might not have a catalog
    if [ -e ${pofile} ]; then
        chmod a-w  ${pofile} # mark as not-to-be-edited
        target_dir="data/lang/po/${i}"
        mkdir -p ${target_dir}
        mv -f ${pofile} ${target_dir}
        echo "Downloaded for language $i"
    fi
done

if [ -e data/lang/po ]; then
    # TODO: think about some way to check if there are local modifications to prevent their loss
    touch data/lang/po/WARNING_CHANGES_TO_PO_FILES_WILL_BE_LOST
    # language files are only picked up at configuration time by a glob expression
    # TODO: think about some way to trigger this automatically, by detecting the need and changing some file
    echo "For picking up new languages, the CMake config needs to be redone, e.g. by calling: make rebuild_cache"
fi
