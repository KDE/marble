#!/usr/bin/env bash
cd src
qmake -config release -spec macx-xcode marbleosx.pro
xcodebuild
mkdir marbleosx.app/Contents/MacOS/Resources/
cp -R ../data/ marbleosx.app/Contents/MacOS/Resources/data/
