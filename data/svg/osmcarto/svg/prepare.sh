#/bin/bash
for F in `find . -type f -name "*svg"`
do
BASE=`basename $F .svg`
inkscape --verb=EditSelectAll --verb=EditCut --verb=EditPasteInPlace --verb=FileSave --verb=FileQuit $F 
perl -pi -e 's/pagecolor=\"#ffffff\"/pagecolor=\"#dadada\"/g' $F
done