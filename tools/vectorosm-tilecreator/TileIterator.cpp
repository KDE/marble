//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      Dennis Nienhüser <nienhueser@kde.org>
//

#include "TileIterator.h"

#include "GeoSceneMercatorTileProjection.h"

#include <QDebug>

namespace Marble {

TileIterator::const_iterator const TileIterator::s_end = TileIterator();

const TileIterator &TileIterator::operator*()
{
    return *this;
}

bool TileIterator::operator!=(const TileIterator::const_iterator &other) const
{
    return m_state != other.m_state;
}

TileIterator::const_iterator &TileIterator::operator++()
{
    if (m_state.x() >= m_bounds.right()) {
        m_state.setX(m_bounds.left());
        if (m_state.y() < m_bounds.bottom()) {
            ++m_state.ry();
        } else {
            *this = s_end;
        }
    } else {
        ++m_state.rx();
    }
    return *this;
}

TileIterator::TileIterator(const GeoDataLatLonBox &latLonBox, int zoomLevel)
{
    int westX, northY, eastX, southY;
    GeoSceneMercatorTileProjection tileProjection;
    tileProjection.tileIndexes(latLonBox, zoomLevel, westX, northY, eastX, southY);

    m_bounds.setLeft(westX);
    m_bounds.setTop(northY);
    m_bounds.setRight(eastX);
    m_bounds.setBottom(southY);
}

TileIterator::const_iterator TileIterator::begin() const
{
    TileIterator iter = *this;
    iter.m_state = iter.m_bounds.topLeft();
    return iter;
}

TileIterator::const_iterator TileIterator::end() const
{
    return s_end;
}

int TileIterator::x() const
{
    return m_state.x();
}

int TileIterator::y() const
{
    return m_state.y();
}

int TileIterator::total() const
{
    return m_bounds.width() * m_bounds.height();
}

TileIterator::TileIterator() :
    m_state(-1, -1)
{
    // nothing to do
}

}
