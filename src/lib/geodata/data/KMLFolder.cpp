
//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#include "KMLFolder.h"

#include "PlaceMarkContainer.h"


KMLFolder::KMLFolder()
{
}

KMLFolder::~KMLFolder()
{
    qDebug("KMLFolder::~KMLFolder(). Subfolders count: %d", m_folderVector.count());

    foreach ( KMLFolder* folder, m_folderVector ) {
        delete folder;
    }
}

void KMLFolder::addFolder( KMLFolder* folder )
{
    m_folderVector.append( folder );
}
