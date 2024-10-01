// SPDX-License-Identifier: LGPL-2.1-or-later

#include "TileCoordsPyramid.h"

#include <MarbleDebug.h>
#include <QList>
#include <QRect>

#include <algorithm>

namespace Marble
{

class Q_DECL_HIDDEN TileCoordsPyramid::Private
{
public:
    Private(int const topLevel, int const bottomLevel);

    int m_topLevel;
    int m_bottomLevel;
    QRect m_bottomLevelCoords;
    QList<int> m_validLevels;
};

TileCoordsPyramid::Private::Private(int const topLevel, int const bottomLevel)
    : m_topLevel(topLevel)
    , m_bottomLevel(bottomLevel)
{
    Q_ASSERT(m_topLevel <= m_bottomLevel);
}

TileCoordsPyramid::TileCoordsPyramid(int const topLevel, int const bottomLevel)
    : d(new Private(topLevel, bottomLevel))
{
}

TileCoordsPyramid::TileCoordsPyramid(TileCoordsPyramid const &other)
    : d(new Private(*other.d))
{
}

TileCoordsPyramid::TileCoordsPyramid()
    : d(new Private(0, 0))
{
}

TileCoordsPyramid &TileCoordsPyramid::operator=(TileCoordsPyramid const &rhs)
{
    TileCoordsPyramid temp(rhs);
    swap(temp);
    return *this;
}

TileCoordsPyramid::~TileCoordsPyramid()
{
    delete d;
}

int TileCoordsPyramid::topLevel() const
{
    return d->m_topLevel;
}

int TileCoordsPyramid::bottomLevel() const
{
    return d->m_bottomLevel;
}

void TileCoordsPyramid::setBottomLevelCoords(QRect const &coords)
{
    d->m_bottomLevelCoords = coords;
}

QRect TileCoordsPyramid::coords(int const level) const
{
    Q_ASSERT(d->m_topLevel <= level && level <= d->m_bottomLevel);
    int bottomX1, bottomY1, bottomX2, bottomY2;
    d->m_bottomLevelCoords.getCoords(&bottomX1, &bottomY1, &bottomX2, &bottomY2);
    int const deltaLevel = d->m_bottomLevel - level;
    int const x1 = bottomX1 >> deltaLevel;
    int const y1 = bottomY1 >> deltaLevel;
    int const x2 = bottomX2 >> deltaLevel;
    int const y2 = bottomY2 >> deltaLevel;
    QRect result;
    result.setCoords(x1, y1, x2, y2);
    return result;
}

void TileCoordsPyramid::setValidTileLevels(const QList<int> validLevels)
{
    d->m_validLevels = validLevels;
}

QList<int> TileCoordsPyramid::validTileLevels()
{
    return d->m_validLevels;
}

qint64 TileCoordsPyramid::tilesCount() const
{
    qint64 result = 0;
    for (int level = d->m_topLevel; level <= d->m_bottomLevel; ++level) {
        if (!d->m_validLevels.isEmpty() && !d->m_validLevels.contains(level))
            continue;

        QRect const levelCoords = coords(level);
        // w*h can exceed 32 bit range, so force 64 bit calculation; see bug 342397
        result += qint64(levelCoords.width()) * levelCoords.height();
    }
    return result;
}

void TileCoordsPyramid::swap(TileCoordsPyramid &other)
{
    std::swap(d, other.d);
}

}
