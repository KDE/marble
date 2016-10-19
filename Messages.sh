#! /bin/sh

### creates marble_qt.pot: strings from marble-maps, marble-qt, libs & plugins

# collect strings from DGML files
# TODO: is there a way to make lupdate pick up the filename and line of the original string?
for fname in  `find data/maps -iname \*.dgml`; do
  for field in name description heading text ; do
    grep -inH "<$field>.*<\/$field>" $fname \
    | sed 's/\"/\\\"/g' \
    | sed 's/<!\[CDATA\[\(.*\)\]\]>/\1/' \
    | sed "s/\\([^:]*\\):\\([^:]*\\):.*<$field>\(.*\)<\/$field>.*/\
           \/\/: file \1:\2\n\
           QCoreApplication::translate(\"DGML\", \"\3\");\
           /i" \
    >> rc.cpp
  done
done

# collect strings from legend.html file
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
  | sed -e 's/^\(.*\)$/\/\/: file data\/legend.html\nQCoreApplication::translate(\"Legends\", \"\1\");/' \
  >> rc.cpp

# collect strings from stars file
cut -d ';' -f 1 data/stars/names.csv | sed -e 's/^/\/\/: file data\/stars\/names.csv\nQCoreApplication::translate(\"StarNames\", \"/' -e 's/$/\");/' >> rc.cpp

# Eliminate empty translate calls (expects one comment line above each translate call)
egrep -B1 'QCoreApplication::translate\(".*", ".*[^ ].*"\)' rc.cpp > rc.cpp.1
mv rc.cpp.1 rc.cpp

$EXTRACT_TR_STRINGS \
    `find src/apps/marble-maps -name '*.cpp' -o -name '*.qml'` \
    src/apps/marble-qt/QtMainWindow.cpp src/apps/marble-qt/qtmain.cpp \
    src/apps/marble-ui/ControlView.cpp \
    `find src/plugins -name '*.cpp' -o -name '*.ui' | grep -v '/test/'` \
    `find src/lib     -name '*.cpp' -o -name '*.ui'` \
    src/lib/marble/PluginInterface.h \
    src/lib/marble/MarbleGlobal.h rc.cpp \
    -o $podir/marble_qt.pot
