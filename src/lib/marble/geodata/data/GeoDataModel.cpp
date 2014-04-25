//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mayank Madan <maddiemadan@gmail.com>
// Copyright 2013      Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "GeoDataModel.h"
#include "GeoDataTypes.h"

#include "stdio.h"

namespace Marble {

class GeoDataModelPrivate
{
public:
    GeoDataModelPrivate();

    GeoDataCoordinates m_coordinates;

    GeoDataScale m_scale;
    GeoDataOrientation m_orientation;
    GeoDataLocation m_location;
    GeoDataLink m_link;
    GeoDataResourceMap m_map;
    QString m_targetHref;
    QString m_sourceHref;
};

GeoDataModelPrivate::GeoDataModelPrivate() :
    m_coordinates(),
    m_scale(),
    m_orientation(),
    m_location(),
    m_link(),
    m_map(),
    m_targetHref(),
    m_sourceHref()
{
}

GeoDataModel::GeoDataModel() :
    GeoDataGeometry(),
    d( new GeoDataModelPrivate )
{
    setAltitudeMode( ClampToGround );
}

GeoDataModel::GeoDataModel( const GeoDataModel &other ) :
    GeoDataGeometry( other ),
    d( new GeoDataModelPrivate( *other.d ) )
{
    // nothing to do
}

GeoDataModel &GeoDataModel::operator=( const GeoDataModel &other )
{
    GeoDataGeometry::operator=( other );
    *d = *other.d;
    return *this;
}


bool GeoDataModel::operator==( const GeoDataModel &other ) const
{
    return equals(other) &&
           d->m_coordinates == other.d->m_coordinates &&
           d->m_scale == other.d->m_scale &&
           d->m_orientation == other.d->m_orientation &&
           d->m_location == other.d->m_location &&
           d->m_link == other.d->m_link &&
           d->m_map == other.d->m_map &&
           d->m_targetHref == other.d->m_targetHref &&
           d->m_sourceHref == other.d->m_sourceHref;
}

bool GeoDataModel::operator!=( const GeoDataModel &other ) const
{
    return !this->operator==( other );
}

GeoDataModel::~GeoDataModel()
{
    delete d;
}

const char *GeoDataModel::nodeType() const
{
    return GeoDataTypes::GeoDataModelType;
}

const GeoDataCoordinates &GeoDataModel::coordinates() const
{
    return d->m_coordinates;
}

GeoDataCoordinates &GeoDataModel::coordinates()
{
    return d->m_coordinates;
}

const GeoDataLocation &GeoDataModel::location() const
{
    return d->m_location;
}

GeoDataLocation &GeoDataModel::location()
{
    return d->m_location;
}

void GeoDataModel::setCoordinates(const GeoDataCoordinates &coordinates)
{
    d->m_coordinates = coordinates;
}

void GeoDataModel::setLocation(const GeoDataLocation &location)
{
    d->m_location = location;
}

const GeoDataLink &GeoDataModel::link() const
{
    return d->m_link;
}

GeoDataLink &GeoDataModel::link()
{
    return d->m_link;
}

void GeoDataModel::setLink( const GeoDataLink &link )
{
    d->m_link = link;
}

const GeoDataScale &GeoDataModel::scale() const
{
    return d->m_scale;
}

GeoDataScale &GeoDataModel::scale()
{
    return d->m_scale;
}

void GeoDataModel::setScale(const GeoDataScale &scale)
{
    d->m_scale=scale;
}

const GeoDataOrientation &GeoDataModel::orientation() const
{
    return d->m_orientation;
}

GeoDataOrientation &GeoDataModel::orientation()
{
    return d->m_orientation;
}

void GeoDataModel::setOrientation(const GeoDataOrientation &orientation)
{
    d->m_orientation=orientation;
}

const GeoDataResourceMap &GeoDataModel::resourceMap() const
{
    return d->m_map;
}

GeoDataResourceMap &GeoDataModel::resourceMap()
{
    return d->m_map;
}

void GeoDataModel::setResourceMap(const GeoDataResourceMap &map)
{
    d->m_map=map;
}

QString GeoDataModel::targetHref() const
{
    return d->m_map.targetHref();
}

void GeoDataModel::setTargetHref(const QString &targetHref)
{
    d->m_map.setTargetHref( targetHref );
}

QString GeoDataModel::sourceHref() const
{
    return d->m_map.sourceHref();
}

void GeoDataModel::setSourceHref(const QString &sourceHref)
{
    d->m_map.setSourceHref( sourceHref );
}

}
