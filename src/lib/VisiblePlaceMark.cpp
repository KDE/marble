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
    if ( m_symbolPixmap.isNull() )
        m_symbolPixmap = m_modelIndex.data( Qt::DecorationRole ).value<QPixmap>();
    return  m_symbolPixmap;
}

const QSize& VisiblePlaceMark::symbolSize() const
{
    if ( !m_symbolSize.isValid() )
      m_symbolSize = m_modelIndex.data( PlaceMarkModel::SymbolSizeRole ).toSize();
    return m_symbolSize;
}

const QPoint& VisiblePlaceMark::symbolPos() const
{
    return m_symbolPos;
}

void VisiblePlaceMark::setSymbolPos( const QPoint& sympos )
{
    m_symbolPos = sympos;
}

const QPixmap& VisiblePlaceMark::labelPixmap() const
{
    return m_labelPixmap;
}

void VisiblePlaceMark::setLabelPixmap( const QPixmap& labelPixmap )
{
    m_labelPixmap = labelPixmap;
}

const QFont& VisiblePlaceMark::labelFont() const
{
    return m_labelFont;
}

void VisiblePlaceMark::setLabelFont( const QFont& labelFont )
{
    m_labelFont = labelFont;
}

const QRect& VisiblePlaceMark::labelRect() const
{
    return m_labelRect;
}

void VisiblePlaceMark::setLabelRect( const QRect& labelRect )
{
    m_labelRect = labelRect;
}
