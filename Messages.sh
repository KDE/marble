#! /bin/sh

for fname in  `find -iname \*.dgml`; do
  for field in name description heading text ; do
    grep -inH "<$field>.*<\/$field>" $fname \
    | sed 's/\"/\\\"/g' \
    | sed 's/<!\[CDATA\[\(.*\)\]\]>/\1/' \
    | sed "s/\\([^:]*\\):\\([^:]*\\):.*<$field>\(.*\)<\/$field>.*/\
           \/\/i18n: file \1 line \2\n\
           i18n(\"\3\");\
           /i" \
    >> rc.cpp
  done
done

cat data/legend.html \
  | tr -d '\n' \
  | sed -e 's/.*<body>\(.*\)<\/body>/\1/' \
        -e 's/^<!DOCTYPE html>//' \
        -e 's@<link href="legend.css" rel="stylesheet" type="text/css" />@@' \
        -e 's/%!{bootstrap}%//' \
        -e 's/body *{ *padding: 10px; *}//' \
        -e 's/<!--[- "&'\''./0-9:;<=>A-Z_a-z]*-->[\t ]*/\n/g' \
        -e 's/<[A-Za-z0-9]*\( [a-z:]*=\"[-A-Za-z0-9:/_.% ]*\"\)*>[\t ]*/\n/g' \
        -e 's/<\/[A-Za-z0-9]*>[\t ]*/\n/g' \
  | sed -e 's/^ *//' -e 's/ *$//' -e 's/^&nbsp;$//' -e '/^$/d' \
  | sed -e 's/^\(.*\)$/\/\/i18n: file .\/data\/legend.html\ni18n(\"\1\");/' \
  >> rc.cpp

# Eliminate empty i18n calls.
egrep -B1 'i18nc?\(".*[^ ].*"\)' rc.cpp > rc.cpp.1
mv rc.cpp.1 rc.cpp

$EXTRACTRC `find . -name '*.ui' \
            -o -name '*.rc' -a ! -name marble.rc \
            -o -name '*.kcfg'` >> rc.cpp
$XGETTEXT src/apps/marble-kde/kdemain.cpp src/apps/marble-kde/marble_part.cpp rc.cpp -o $podir/marble.pot
$XGETTEXT_QT src/apps/marble-qt/QtMainWindow.cpp src/apps/marble-qt/qtmain.cpp src/apps/marble-ui/ControlView.cpp `find src/plugins -name '*.cpp' | grep -v '/test/'` `find src/lib -name '*.cpp'` src/lib/marble/global.h src/lib/marble/MarbleGlobal.h -o $podir/marble_qt.pot
