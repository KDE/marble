#/bin/bash
for F in `find . -type f -name "*svg"`
do
  BASE=`basename $F .svg`
  inkscape --export-area-page --export-png="${BASE}.png" --export-width=48 --export-height=48 "${F}" 
done
