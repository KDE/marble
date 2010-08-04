//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//

#include "KmlFileViewItem.h"

#include "FileManager.h"


using namespace Marble;

KmlFileViewItem::KmlFileViewItem( GeoDataDocument *document ) :
    m_document( document )
{
}

KmlFileViewItem::~KmlFileViewItem()
{
    //TODO
}

void KmlFileViewItem::saveFile()
{
    //TODO
}

GeoDataDocument* KmlFileViewItem::document()
{
    return m_document;
}

bool KmlFileViewItem::isShown() const
{
    return m_document->isVisible();
}

void KmlFileViewItem::setShown( bool value )
{
    m_document->setVisible( value );
}

QString KmlFileViewItem::name() const
{
    if(!m_document->name().isEmpty())
        return m_document->name();
    else if(!m_document->fileName().isEmpty())
        return m_document->fileName();
    else
        return QString("KML Document");

}
