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

using namespace Marble;

VisiblePlacemark::VisiblePlacemark( const GeoDataPlacemark *placemark )
    : m_placemark( placemark )
{
}

const GeoDataPlacemark* VisiblePlacemark::placemark() const
{
    return m_placemark;
}

const QString VisiblePlacemark::name() const
{
    if ( m_name.isEmpty() )
        m_name = m_placemark->name();

    return m_name;
}

const QPixmap& VisiblePlacemark::symbolPixmap() const
{    
    GeoDataStyle* style = m_placemark->style();
    if ( style ) {
        m_symbolPixmap = style->iconStyle().icon(); 
    } else {
        mDebug() << "Style pointer null";
    }
    return  m_symbolPixmap;
}

bool VisiblePlacemark::selected() const
{
    return m_selected;
}

void VisiblePlacemark::setSelected( bool selected )
{
    m_selected = selected;
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
