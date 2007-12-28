//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#include "KMLFileViewItem.h"
#include "GeoDataDocument.h"    // In geodata/data/

KMLFileViewItem::KMLFileViewItem( PlaceMarkManager& manager, GeoDataDocument& document ) :
    m_placeMarkManager( manager ),
    m_document( document )
{
}

KMLFileViewItem::~KMLFileViewItem()
{
    //TODO
}

void KMLFileViewItem::saveFile()
{
    //TODO
}

void KMLFileViewItem::closeFile()
{
    //TODO
}

QVariant KMLFileViewItem::data() const
{
    return m_document.name();
}

bool KMLFileViewItem::isShown() const
{
    return m_document.isVisible();
}

void KMLFileViewItem::setShown( bool value )
{
    m_document.setVisible( value );
}
