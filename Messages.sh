#! /bin/sh

for fname in  `find -iname \*.dgml`; do
  for field in name description heading text ; do
    grep -inH "<$field>.*<\/$field>" $fname \
    | sed 's/\"/\\\"/g' \
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
        -e 's/<!--[- "&'\''./0-9:;<=>A-Z_a-z]*-->[\t ]*/\n/g' \
        -e 's/<[A-Za-z0-9]*\( [a-z:]*=\"[-A-Za-z0-9:/_.% ]*\"\)*>[\t ]*/\n/g' \
        -e 's/<\/[A-Za-z0-9]*>[\t ]*/\n/g' \
  | sed -e 's/^ *//' -e 's/ *$//' -e 's/^&nbsp;$//' -e '/^$/d' \
  | sed -e 's/^\(.*\)$/\/\/i18n: file .\/data\/legend.html\ni18n(\"\1\");/' \
  >> rc.cpp

$EXTRACTRC `find . -name '*.ui' -o -name '*.rc'` >> rc.cpp
$XGETTEXT -ktr:1,1t -ktr:1,2c,2t -kQT_TRANSLATE_NOOP:1c,2,2t -kQT_TR_NOOP:1,1t -ktranslate:1c,2,2t -ktranslate:2,3c,3t `find . -name '*.cpp' -o -name '*.h'` -o $podir/marble.pot
rm -f rc.cpp
