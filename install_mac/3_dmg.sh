#!/bin/bash

#
# A bash script to create a dmg image file of the 
#            final application bundle
#              (c) Tim Sutton 2007

set -x

APPNAME=Marble
APPDIR=${HOME}/apps
DMGNAME=${APPNAME}0.4.5

echo "Removing old dmg if it exists"
  rm ${APPDIR}/${DMGNAME}.dmg

  pushd .
  cd ${APPDIR}
  tar cvfz ${APPNAME}.app.tar.gz ${APPNAME}.app
  popd
  
echo "Building new dmg of application bundle"

  echo `pwd`
  #SIZE=`du -sh ${APPDIR}/${APPNAME}.app |grep -o "[0-9]*M"`
  SIZE=100M
  echo "dmg size will be $SIZE"
  hdiutil create -size $SIZE -attach -volname ${APPNAME} -fs HFS ${APPDIR}/${DMGNAME}.dmg
 
  #find out the name of the volume that was mounted...
  VOLUME_NAME=`mount | tail -1 |sed 's/^[a-zA-Z0-9 \/]*Volumes\///g'|sed 's/ ([a-zA-Z0-9 \/,)]*$//g'`
  
  # we need to extract into rather than copy into the dmg
  # because sometimes copy operation failes
  pushd .
  cd "/Volumes/${VOLUME_NAME}/" 
  tar xvfz ${APPDIR}/${APPNAME}.app.tar.gz
  popd
##  umount /Volumes/${APPNAME}
hdiutil unmount "/Volumes/${VOLUME_NAME}"

echo "Compressing dmg"

hdiutil convert -format UDZO ${APPDIR}/${DMGNAME}.dmg -o ${APPDIR}/${DMGNAME}-compressed.dmg
#rm ${APPDIR}/${DMGNAME}.dmg
#mv ${APPDIR}/${DMGNAME}-compressed.dmg ${APPDIR}/${DMGNAME}.dmg

echo "Checksumming the final application bundle"
  
  md5 ${APPDIR}/${DMGNAME}.dmg > ${APPDIR}/${DMGNAME}.dmg.md5

