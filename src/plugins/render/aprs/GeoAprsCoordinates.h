// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Wes Hardaker <hardaker@users.sourceforge.net>
//

#ifndef GEOAPRSCOORDINATES_H
#define GEOAPRSCOORDINATES_H

#include "GeoDataCoordinates.h"
#include <QElapsedTimer>

namespace Marble
{
class GeoAprsCoordinates : public GeoDataCoordinates
{
public:
    // must match AprsObject's SeenFrom
    enum SeenFrom {
        FromNowhere = 0x00,
        FromTTY = 0x01,
        FromTCPIP = 0x02,
        FromFile = 0x04,
        Directly = 0x08
    };

    GeoAprsCoordinates(qreal lon, qreal lat, int where);
    ~GeoAprsCoordinates() override;

    void addSeenFrom(int where);
    int seenFrom() const;
    void resetTimestamp();
    const QElapsedTimer &timestamp() const;

private:
    int m_seenFrom;
    QElapsedTimer m_timestamp;
};
}

#endif /* GEOAPRSCOORDINATES_H */
