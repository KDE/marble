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
#include "GeoDataDocument.h"    // In geodata/data/

using namespace Marble;

KmlFileViewItem::KmlFileViewItem( PlaceMarkManager& manager, GeoDataDocument& document ) :
    m_placeMarkManager( manager ),
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

void KmlFileViewItem::closeFile()
{
    //TODO
}

QVariant KmlFileViewItem::data() const
{
    if(!m_document.name().isEmpty())
        return m_document.name();
    else
        return QString("KML Document"); // this should be replaced by the file name later
}

bool KmlFileViewItem::isShown() const
{
    return m_document.isVisible();
}

void KmlFileViewItem::setShown( bool value )
{
    m_document.setVisible( value );
}
