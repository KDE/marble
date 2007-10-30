#!/bin/sh
#set -x
# Copy Qt frameworks to marble bundle
# and make search paths for them relative to bundle

#
# Change base prefix of you built to another prefix
#

BASEPREFIX=/Applications
PREFIX=${BASEPREFIX}/Marble.app/Contents/MacOS
FRAMEWORKPREFIX=${BASEPREFIX}/Marble.app/Contents/Frameworks
LIBPREFIX=${BASEPREFIX}/Marble.app/Contents/lib
PLUGINPREFIX=${BASEPREFIX}/Marble.app/Contents/plugins

QTPREFIX=/Library/Frameworks
QTFRAMEWORKS="QtCore QtGui QtNetwork QtSvg QtXml"

# Copy supporting frameworks to application bundle
mkdir -p $FRAMEWORKPREFIX
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
	install_name_tool -change QtCore.framework/Versions/4/QtCore \
		@executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore \
		$FRAMEWORK.framework/Versions/4/$FRAMEWORK
done
for FRAMEWORK in QtSvg 
do
	install_name_tool -change QtGui.framework/Versions/4/QtGui \
		@executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui \
		$FRAMEWORK.framework/Versions/4/$FRAMEWORK
	install_name_tool -change QtXml.framework/Versions/4/QtXml \
		@executable_path/../Frameworks/QtXml.framework/Versions/4/QtXml \
		$FRAMEWORK.framework/Versions/4/$FRAMEWORK
done
cd ../
mkdir -p plugins/imageformats
cd plugins/imageformats
LIBJPEG=libjpeg.dylib
LIBQJPEG=/Developer/Applications/Qt/plugins/imageformats/libqjpeg.dylib
if test ! -f $LIBJPEG; then
	cp $LIBQJPEG $LIBJPEG
	# Update path to supporting libraries
	install_name_tool -change QtCore.framework/Versions/4/QtCore \
		@executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore \
		$LIBJPEG
	install_name_tool -change QtGui.framework/Versions/4/QtGui \
		@executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui \
		$LIBJPEG
	install_name_tool -change $LNKJPEG @executable_path/../plugins/imageformats/$LIBJPEG $LIBJPEG
fi

cd ../../MacOS
echo `pwd`
for FILE in \
	marble \
	lib/libmarblewidget.0.5.0.dylib
do
	for FRAMEWORK in QtCore QtGui QtNetwork QtSvg QtXml 
	do
		install_name_tool -change $FRAMEWORK.framework/Versions/4/$FRAMEWORK \
			@executable_path/../Frameworks/$FRAMEWORK.framework/Versions/4/$FRAMEWORK \
			$PREFIX/$FILE
	done
done
