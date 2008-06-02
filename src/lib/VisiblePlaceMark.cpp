//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

#include "VisiblePlaceMark.h"

#include <QtCore/QDebug>

#include "GeoDataStyle.h"
#include "MarblePlacemarkModel.h"

VisiblePlaceMark::VisiblePlaceMark()
{
}

VisiblePlaceMark::~VisiblePlaceMark()
{
}

const QPersistentModelIndex& VisiblePlaceMark::modelIndex() const
{
    return m_modelIndex;
}

void VisiblePlaceMark::setModelIndex( const QPersistentModelIndex &modelIndex )
{
    m_modelIndex = modelIndex;
}

const QString VisiblePlaceMark::name() const
{
    if ( m_name.isEmpty() )
        m_name = m_modelIndex.data( Qt::DisplayRole ).toString();

    return m_name;
}

const QPixmap& VisiblePlaceMark::symbolPixmap() const
{
    GeoDataStyle* style = qobject_cast<const MarblePlacemarkModel*>( m_modelIndex.model() )
        ->styleData( m_modelIndex );
//    GeoDataStyle* style = m_modelIndex.data( MarblePlacemarkModel::StyleRole ).value<GeoDataStyle*>();
    if ( style == 0 )
    {
        qDebug() << "Style pointer null";
    }
    m_symbolPixmap = style->iconStyle()->icon(); 
    return  m_symbolPixmap;
}

const QPoint& VisiblePlaceMark::symbolPosition() const
{
    return m_symbolPosition;
}

void VisiblePlaceMark::setSymbolPosition( const QPoint& position )
{
    m_symbolPosition = position;
}

const QPixmap& VisiblePlaceMark::labelPixmap() const
{
    return m_labelPixmap;
}

void VisiblePlaceMark::setLabelPixmap( const QPixmap& labelPixmap )
{
    m_labelPixmap = labelPixmap;
}

const QRect& VisiblePlaceMark::labelRect() const
{
    return m_labelRect;
}

void VisiblePlaceMark::setLabelRect( const QRect& labelRect )
{
    m_labelRect = labelRect;
}
