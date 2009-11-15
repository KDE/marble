//own
#include "GeoDataLookAt.h"
#include "GeoDataLookAt_p.h"
#include "MarbleDebug.h"

//Qt
#include <QtCore/QDataStream>

namespace Marble
{
	
GeoDataLookAt::GeoDataLookAt()
{
	d = new GeoDataLookAtPrivate();
	d->m_coord = new GeoDataCoordinates();
}

GeoDataLookAt::~GeoDataLookAt()
{
	if( !d )
		delete d;
}

void GeoDataLookAt::setAltitude( qreal altitude)
{
	d->m_coord->setAltitude(altitude);
}

qreal GeoDataLookAt::altitude() const
{
	return d->m_coord->altitude();
}

void GeoDataLookAt::setLatitude( qreal latitude, GeoDataCoordinates::Unit unit)
{
	d->m_coord->setLatitude(latitude,unit);

}

qreal GeoDataLookAt::latitude(GeoDataCoordinates::Unit unit) const
{
	return d->m_coord->latitude(unit);

}

void GeoDataLookAt::setLongitude( qreal longitude, GeoDataCoordinates::Unit unit)
{
	d->m_coord->setLongitude(longitude,unit);
}

qreal GeoDataLookAt::longitude(GeoDataCoordinates::Unit unit) const
{
	return d->m_coord->longitude(unit);

}

GeoDataCoordinates* GeoDataLookAt::coordinate() const
{
	return d->m_coord;
}
}


