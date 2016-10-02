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
#include "TileId.h"
#include "VectorTileModel.h"

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
    qreal north, west, south, east;
    latLonBox.boundaries(north, south, east, west);
    unsigned int N = pow(2, zoomLevel);
    m_bounds.setLeft(TileId::lon2tileX(west, N));
    m_bounds.setTop(TileId::lat2tileY(north, N));
    m_bounds.setRight(qMin(N-1, TileId::lon2tileX(east, N)));
    m_bounds.setBottom(TileId::lat2tileY(south, N));
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
