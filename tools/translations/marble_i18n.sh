#!/bin/sh
TAG=$1
if [ "${TAG}" = "" ]; then
	echo "Syntax: marble_i18n KDE-tag"
	echo "E.g.:   marble_i18n 4.5.0"
	echo "-------------------------"
	TAG=4.5.0
	echo "Assuming ${TAG} as a KDE-tag version for now."
fi

svn export svn://anonsvn.kde.org/home/kde/tags/KDE/${TAG}/kde-l10n/subdirs
for i in $(cat subdirs)
do
	echo "Processing Language $i"
	svn export svn://anonsvn.kde.org/home/kde/tags/KDE/${TAG}/kde-l10n/${i}/messages/kdeedu/marble.po
	perl -pi -e 's/^[[:space:]]*#.*$//g' marble.po
	lconvert marble.po -o marble_${i}.qm
done
