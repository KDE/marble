#! /usr/bin/env bash
$EXTRACTRC `find . -name '*.rc' \
                -o -name '*.kcfg'` >> rc.cpp

$XGETTEXT `find -name \*.cpp -o -name \*.qml` -o $podir/marble-maps.pot
