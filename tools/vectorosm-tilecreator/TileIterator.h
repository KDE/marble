//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_TILEITERATOR_H
#define MARBLE_TILEITERATOR_H

#include "GeoDataLatLonBox.h"

#include <QRect>

namespace Marble {

class TileIterator {
public:
    typedef TileIterator const_iterator;

    const const_iterator& operator*();
    bool operator!=( const const_iterator& ) const;
    const_iterator& operator++();
    const_iterator begin() const;
    const_iterator end() const;

    TileIterator(const GeoDataLatLonBox &latLonBox, int zoomLevel);
    int x() const;
    int y() const;
    int total() const;

private:
    TileIterator();

    QRect m_bounds;
    QPoint m_state;
    static const_iterator const s_end;
};

}

#endif
