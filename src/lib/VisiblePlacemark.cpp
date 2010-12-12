//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//

#include "VisiblePlacemark.h"

#include "MarbleDebug.h"

#include "GeoDataStyle.h"
#include "MarblePlacemarkModel.h"

using namespace Marble;

const QModelIndex& VisiblePlacemark::modelIndex() const
{
    return m_modelIndex;
}

void VisiblePlacemark::setModelIndex( const QModelIndex &modelIndex )
{
    m_modelIndex = modelIndex;
}

const QString VisiblePlacemark::name() const
{
    if ( m_name.isEmpty() )
        m_name = m_modelIndex.data( Qt::DisplayRole ).toString();

    return m_name;
}

const QPixmap& VisiblePlacemark::symbolPixmap() const
{
    GeoDataPlacemark *placemark = dynamic_cast<GeoDataPlacemark*>(qvariant_cast<GeoDataObject*>(m_modelIndex.data( MarblePlacemarkModel::ObjectPointerRole ) ));
    GeoDataStyle* style = placemark->style();
    if ( style ) {
        m_symbolPixmap = style->iconStyle().icon(); 
    } else {
        mDebug() << "Style pointer null";
    }
    return  m_symbolPixmap;
}

const QPoint& VisiblePlacemark::symbolPosition() const
{
    return m_symbolPosition;
}

void VisiblePlacemark::setSymbolPosition( const QPoint& position )
{
    m_symbolPosition = position;
}

const QPixmap& VisiblePlacemark::labelPixmap() const
{
    return m_labelPixmap;
}

void VisiblePlacemark::setLabelPixmap( const QPixmap& labelPixmap )
{
    m_labelPixmap = labelPixmap;
}

const QRect& VisiblePlacemark::labelRect() const
{
    return m_labelRect;
}

void VisiblePlacemark::setLabelRect( const QRect& labelRect )
{
    m_labelRect = labelRect;
}
