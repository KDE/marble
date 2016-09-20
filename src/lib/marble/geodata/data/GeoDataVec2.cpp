//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "GeoDataVec2.h"

#include "MarbleDebug.h"

namespace Marble {

class GeoDataVec2Private
{
public:
    GeoDataVec2Private();

    GeoDataVec2::Unit  m_xunit;
    GeoDataVec2::Unit  m_yunit;

    static GeoDataVec2::Unit  parseUnits( const QString &value );
};

GeoDataVec2Private::GeoDataVec2Private() :
    m_xunit(GeoDataVec2::Fraction), m_yunit(GeoDataVec2::Fraction)
{
}

GeoDataVec2::GeoDataVec2() :
    d( new GeoDataVec2Private )
{
}

GeoDataVec2::GeoDataVec2(qreal x, qreal y, const QString &xunits, const QString &yunits) :
    d( new GeoDataVec2Private )
{
    setX( x );
    setY( y );
    d->m_xunit = GeoDataVec2Private::parseUnits( xunits );
    d->m_yunit = GeoDataVec2Private::parseUnits( yunits );
}

GeoDataVec2::Unit GeoDataVec2Private::parseUnits( const QString &value )
{
    if (value == QLatin1String("fraction")) {
        return GeoDataVec2::Fraction;
    }
    if (value == QLatin1String("pixels")) {
        return GeoDataVec2::Pixels;
    }
    if (value == QLatin1String("insetPixels")) {
        return GeoDataVec2::InsetPixels;
    }

    mDebug() << "Warning: Unknown units value " << value << " - falling back to default 'fraction'";
    return GeoDataVec2::Fraction;
}

GeoDataVec2::GeoDataVec2( const Marble::GeoDataVec2 &other ) :
  QPointF(other), d( new GeoDataVec2Private( *other.d ) )
{
}

GeoDataVec2 &GeoDataVec2::operator=( const GeoDataVec2 &other )
{
    QPointF::operator=(other);
    *d = *other.d;
    return *this;
}

bool GeoDataVec2::operator==(const GeoDataVec2& other) const
{
    return x() == other.x() && y() == other.y() &&
           d->m_xunit == other.d->m_xunit && d->m_yunit == other.d->m_yunit;
}

bool GeoDataVec2::operator!=(const GeoDataVec2& other) const
{
    return !this->operator==(other);
}

GeoDataVec2::~GeoDataVec2()
{
    delete d;
}

GeoDataVec2::Unit GeoDataVec2::xunit() const
{
    return d->m_xunit;
}

void GeoDataVec2::setXunits(Unit xunit)
{
    d->m_xunit = xunit;
}

GeoDataVec2::Unit GeoDataVec2::yunit() const
{
    return d->m_yunit;
}

void GeoDataVec2::setYunits(Unit yunit)
{
    d->m_yunit = yunit;
}

}
