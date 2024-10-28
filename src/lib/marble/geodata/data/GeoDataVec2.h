// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Mohammed Nafees <nafees.technocool@gmail.com>
//

#ifndef GEODATAVEC2_H
#define GEODATAVEC2_H

#include "geodata_export.h"
#include <QPointF>

class QString;

namespace Marble
{

class GeoDataVec2Private;

class GEODATA_EXPORT GeoDataVec2 : public QPointF
{
public:
    enum Unit {
        Fraction,
        Pixels,
        InsetPixels
    };

    GeoDataVec2();

    GeoDataVec2(qreal x, qreal y, const QString &xunit, const QString &yunit);

    GeoDataVec2(const GeoDataVec2 &other);

    GeoDataVec2 &operator=(const GeoDataVec2 &other);
    bool operator==(const GeoDataVec2 &other) const;
    bool operator!=(const GeoDataVec2 &other) const;
    ~GeoDataVec2();

    Unit xunit() const;
    void setXunits(Unit xunit);

    Unit yunit() const;
    void setYunits(Unit yunit);

private:
    GeoDataVec2Private *const d;
};

}

#endif
