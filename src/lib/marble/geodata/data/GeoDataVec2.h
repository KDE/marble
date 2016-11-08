//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Mohammed Nafees <nafees.technocool@gmail.com>
//

#ifndef GEODATAVEC2_H
#define GEODATAVEC2_H

#include <QPointF>
#include "geodata_export.h"

class QString;

namespace Marble {

class GeoDataVec2Private;

class GEODATA_EXPORT GeoDataVec2 : public QPointF
{
public:
    enum Unit {Fraction, Pixels, InsetPixels};

    GeoDataVec2();

    GeoDataVec2(qreal x, qreal y, const QString &xunit, const QString &yunit);

    GeoDataVec2( const GeoDataVec2 &other );

    GeoDataVec2& operator=( const GeoDataVec2 &other );
    bool operator==( const GeoDataVec2 &other ) const;
    bool operator!=( const GeoDataVec2 &other ) const;
    ~GeoDataVec2();

    Unit xunit() const;
    void setXunits( Unit xunit );

    Unit yunit() const;
    void setYunits( Unit yunit );

private:
    GeoDataVec2Private* const d;
};

}

#endif
