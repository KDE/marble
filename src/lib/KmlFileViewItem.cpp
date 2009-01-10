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
#include "PlaceMarkManager.h"
#include "MarblePlacemarkModel.h"
#include <QtCore/QDebug>

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
    qDebug() << "closing file" << m_document.name() << m_document.fileName();
    m_placeMarkManager.model()->removePlaceMarks( m_document.fileName(), true );
}

QVariant KmlFileViewItem::data( int role ) const
{
    if( role == Qt::DisplayRole ) {
        if(!m_document.name().isEmpty())
            return m_document.name();
        else if(!m_document.fileName().isEmpty())
            return m_document.fileName();
        else
            return QString("KML Document");
    }
    else if( role == AbstractFileViewItem::FilePointerRole ) {
        return &m_document;
    }
    else
        return QVariant();
}

bool KmlFileViewItem::isShown() const
{
    return m_document.isVisible();
}

void KmlFileViewItem::setShown( bool value )
{
    m_document.setVisible( value );
}
