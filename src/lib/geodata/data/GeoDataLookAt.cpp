//own
#include "GeoDataLookAt.h"
#include "GeoDataLookAt_p.h"
#include "MarbleDebug.h"

//Qt
#include <QtCore/QDataStream>

namespace Marble
{

GeoDataLookAt::GeoDataLookAt() : d (new GeoDataLookAtPrivate)
{
    // nothing to do
}

GeoDataLookAt::GeoDataLookAt(const GeoDataLookAt& other) : 
        d (new GeoDataLookAtPrivate(*other.d))
{
    // nothing to do
}

GeoDataLookAt& GeoDataLookAt::operator=(const GeoDataLookAt &other)                                   
{
    *d = *other.d;
    return *this;    
}

GeoDataLookAt::~GeoDataLookAt()
{
    delete d;
}

void GeoDataLookAt::setAltitude( qreal altitude)
{
    d->m_coord.setAltitude(altitude);
}

qreal GeoDataLookAt::altitude() const
{
    return d->m_coord.altitude();
}

void GeoDataLookAt::setLatitude( qreal latitude, GeoDataCoordinates::Unit unit)
{
    d->m_coord.setLatitude(latitude,unit);
}

qreal GeoDataLookAt::latitude(GeoDataCoordinates::Unit unit) const
{
    return d->m_coord.latitude(unit);
}

void GeoDataLookAt::setLongitude( qreal longitude, GeoDataCoordinates::Unit unit)
{
    d->m_coord.setLongitude(longitude,unit);
}

qreal GeoDataLookAt::longitude(GeoDataCoordinates::Unit unit) const
{
    return d->m_coord.longitude(unit);
}

GeoDataCoordinates GeoDataLookAt::coordinates() const
{
    return d->m_coord;
}

void GeoDataLookAt::setRange( qreal range )
{
    d->m_range = range;
}

qreal GeoDataLookAt::range() const
{
    return d->m_range;
}

}
