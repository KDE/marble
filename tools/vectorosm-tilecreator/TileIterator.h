// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2016 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_TILEITERATOR_H
#define MARBLE_TILEITERATOR_H

#include "GeoDataLatLonBox.h"

#include <QRect>

namespace Marble {

class TileIterator {
public:
    using const_iterator = TileIterator;

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
