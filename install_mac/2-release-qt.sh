#!/bin/sh
#set -x
# Copy Qt frameworks to marble bundle
# and make search paths for them relative to bundle

#
# Change the next two lines to match your system
#

UNIVERSAL_LIBS_PREFIX=/usr/local/qgis_universal_deps
APP_PREFIX=/Applications/Marble.app

# Note: that if you are debugging you may want to disable the 
# strip section at the end of this file.

#------ You should not need to edit anything after this -----

MACOS_PREFIX=${APP_PREFIX}/Contents/MacOS
FRAMEWORKPREFIX=${APP_PREFIX}/Contents/Frameworks
QTPREFIX=${UNIVERSAL_LIBS_PREFIX}/lib
mkdir -p $MACOS_PREFIX
mkdir -p $FRAMEWORKPREFIX
pushd $PWD
cd $FRAMEWORKPREFIX

QTFRAMEWORKS="QtCore QtGui QtNetwork QtSvg QtXml"

#
# Copy supporting frameworks to application bundle
#
cd $FRAMEWORKPREFIX
for FRAMEWORK in $QTFRAMEWORKS
do
	LIBFRAMEWORK=$FRAMEWORK.framework/Versions/4/$FRAMEWORK
	if test ! -f $LIBFRAMEWORK; then
		mkdir -p $FRAMEWORK.framework/Versions/4
		cp $QTPREFIX/$LIBFRAMEWORK $LIBFRAMEWORK
		install_name_tool -id @executable_path/../Frameworks/$LIBFRAMEWORK $LIBFRAMEWORK
	fi
done

# Update path to supporting frameworks
for FRAMEWORK in QtGui QtNetwork QtSvg QtXml
do
	install_name_tool -change ${QTPREFIX}/QtCore.framework/Versions/4/QtCore \
		@executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore \
		$FRAMEWORK.framework/Versions/4/$FRAMEWORK
done

for FRAMEWORK in QtSvg
do
	install_name_tool -change ${QTPREFIX}/QtGui.framework/Versions/4/QtGui \
		@executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui \
		$FRAMEWORK.framework/Versions/4/$FRAMEWORK
	install_name_tool -change ${QTPREFIX}/QtXml.framework/Versions/4/QtXml \
		@executable_path/../Frameworks/QtXml.framework/Versions/4/QtXml \
		$FRAMEWORK.framework/Versions/4/$FRAMEWORK
done

#
# Update Marble related libs and binaries
#

cd $MACOS_PREFIX
for FILE in \
  marble \
  lib/libmarblewidget.0.5.0.dylib
do
 for FRAMEWORK in QtCore QtGui QtNetwork QtSvg QtXml 
	do
		install_name_tool -change ${QTPREFIX}/${FRAMEWORK}.framework/Versions/4/$FRAMEWORK \
			@executable_path/../Frameworks/$FRAMEWORK.framework/Versions/4/$FRAMEWORK \
			$MACOS_PREFIX/$FILE
	done
done



#
# Update qt imageformat plugin paths
#


cd ${MACOS_PREFIX}/../
mkdir -p plugins/imageformats
cd plugins/imageformats

LIBJPEG=libjpeg.dylib
LIBQJPEG=${UNIVERSAL_LIBS_PREFIX}/plugins/imageformats/libqjpeg.dylib
if test ! -f $LIBJPEG; then
	cp $LIBQJPEG $LIBJPEG
	# Update path to supporting libraries
	install_name_tool -change ${QTPREFIX}/QtCore.framework/Versions/4/QtCore \
		@executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore \
		$LIBJPEG
	install_name_tool -change ${QTPREFIX}/QtGui.framework/Versions/4/QtGui \
		@executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui \
		$LIBJPEG
	install_name_tool -change $LIBQJPEG @executable_path/../plugins/imageformats/$LIBJPEG 
fi
LIBGIF=libgif.dylib
LIBQGIF=${UNIVERSAL_LIBS_PREFIX}/plugins/imageformats/libqgif.dylib
if test ! -f $LIBGIF; then
	cp $LIBQGIF $LIBGIF
	# Update path to supporting libraries
	install_name_tool -change ${QTPREFIX}/QtCore.framework/Versions/4/QtCore \
		@executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore \
		$LIBGIF
	install_name_tool -change ${QTPREFIX}/QtGui.framework/Versions/4/QtGui \
		@executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui \
		$LIBGIF
	install_name_tool -change $LIBQGIF @executable_path/../plugins/imageformats/$LIBGIF 
fi

popd


# You may want to comment out this section if you are trying to debug...
# but enable it for final packaging because your bundle will be much
# smaller
for FRAMEWORK in $QTFRAMEWORKS
do
  strip -x ${APP_PREFIX}/Contents/Frameworks/${FRAMEWORK}.framework/Versions/4/${FRAMEWORK}
done
