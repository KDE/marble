#!/bin/sh

if [ "$1" = "" ]; then
	echo "Please specify the target directory as a parameter."
    echo "marble-qt-l10n -o outputdir"
	exit
fi

SVNEXEC="https://svn.kde.org/home/kde"
SVNPATH="/messages/kdeedu/marble.po"

SVNROOT="/branches/stable/l10n-kde4/"

LANGCODE="de ab aa af sq am ar hy as ay az ba eu bn dz bh bi br bg my be km ca zh co \
	hr cs da nl en eo et fo fa fj fi fr fy gl gd gv ka de el kl gn gu ha he \
	hi hu is id ia ie iu ik ga it ja jv kn ks kk rw ky rn ko ku lo la lv li \
	ln lt mk mg ms ml mt mi mr mo mn na ne no oc or om ps pl pt pa qu rm ro \
	ru sm sg sa sr sh st tn sn sd si ss sk sl so es su sw sv tl tg ta tt te \
	th bo ti to ts tr tk tw ug uk ur uz vi vo cy wo xh yi yo zu \
	pt-br en-gb en-us de-de de-ch"

for currentcode in $LANGCODE
do
	echo "Exporting $currentcode po-file ..."
    TARGET="/tmp/marble-$currentcode.po"
	svn export $SVNEXEC$SVNROOT$currentcode$SVNPATH $TARGET
    if [ -s $TARGET ]; then
#    	msgfmt -c --statistics --qt -o $1/marble-$currentcode.qm /tmp/marble-$currentcode.po
        po2ts /tmp/marble-$currentcode.po /tmp/marble-$currentcode.ts
        lrelease /tmp/marble-$currentcode.ts -qm $1/marble-$currentcode.qm
    	rm /tmp/marble-$currentcode.po 
    else
        echo "Couldn't find: $TARGET"
    fi

done