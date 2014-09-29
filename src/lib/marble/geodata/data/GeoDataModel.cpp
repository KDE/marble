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
#include "GeoDataGeometry_p.h"

#include "GeoDataTypes.h"

#include "stdio.h"

namespace Marble {

class GeoDataModelPrivate : public GeoDataGeometryPrivate
{
public:
    GeoDataModelPrivate();

    const char *nodeType() const { return GeoDataTypes::GeoDataModelType; }

    GeoDataGeometryPrivate *copy() { return new GeoDataModelPrivate( *this ); }

    EnumGeometryId geometryId() const { return GeoDataModelId; }

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
    GeoDataGeometry( new GeoDataModelPrivate )
{
    setAltitudeMode( ClampToGround );
}

GeoDataModel::GeoDataModel( const GeoDataModel &other ) :
    GeoDataGeometry( other )
{
    // nothing to do
}

GeoDataModel &GeoDataModel::operator=( const GeoDataModel &other )
{
    GeoDataGeometry::operator=( other );
    return *this;
}


bool GeoDataModel::operator==( const GeoDataModel &other ) const
{
    return equals(other) &&
           p()->m_coordinates == other.p()->m_coordinates &&
           p()->m_scale == other.p()->m_scale &&
           p()->m_orientation == other.p()->m_orientation &&
           p()->m_location == other.p()->m_location &&
           p()->m_link == other.p()->m_link &&
           p()->m_map == other.p()->m_map &&
           p()->m_targetHref == other.p()->m_targetHref &&
           p()->m_sourceHref == other.p()->m_sourceHref;
}

bool GeoDataModel::operator!=( const GeoDataModel &other ) const
{
    return !this->operator==( other );
}

GeoDataModel::~GeoDataModel()
{
}

const GeoDataCoordinates &GeoDataModel::coordinates() const
{
    return p()->m_coordinates;
}

GeoDataCoordinates &GeoDataModel::coordinates()
{
    return p()->m_coordinates;
}

const GeoDataLocation &GeoDataModel::location() const
{
    return p()->m_location;
}

GeoDataLocation &GeoDataModel::location()
{
    return p()->m_location;
}

void GeoDataModel::setCoordinates(const GeoDataCoordinates &coordinates)
{
    detach();

    p()->m_coordinates = coordinates;
}

void GeoDataModel::setLocation(const GeoDataLocation &location)
{
    detach();

    p()->m_location = location;
}

const GeoDataLink &GeoDataModel::link() const
{
    return p()->m_link;
}

GeoDataLink &GeoDataModel::link()
{
    return p()->m_link;
}

void GeoDataModel::setLink( const GeoDataLink &link )
{
    detach();

    p()->m_link = link;
}

const GeoDataScale &GeoDataModel::scale() const
{
    return p()->m_scale;
}

GeoDataScale &GeoDataModel::scale()
{
    return p()->m_scale;
}

void GeoDataModel::setScale(const GeoDataScale &scale)
{
    detach();

    p()->m_scale=scale;
}

const GeoDataOrientation &GeoDataModel::orientation() const
{
    return p()->m_orientation;
}

GeoDataOrientation &GeoDataModel::orientation()
{
    return p()->m_orientation;
}

void GeoDataModel::setOrientation(const GeoDataOrientation &orientation)
{
    detach();

    p()->m_orientation=orientation;
}

const GeoDataResourceMap &GeoDataModel::resourceMap() const
{
    return p()->m_map;
}

GeoDataResourceMap &GeoDataModel::resourceMap()
{
    return p()->m_map;
}

void GeoDataModel::setResourceMap(const GeoDataResourceMap &map)
{
    detach();

    p()->m_map=map;
}

QString GeoDataModel::targetHref() const
{
    return p()->m_map.targetHref();
}

void GeoDataModel::setTargetHref(const QString &targetHref)
{
    detach();

    p()->m_map.setTargetHref( targetHref );
}

QString GeoDataModel::sourceHref() const
{
    return p()->m_map.sourceHref();
}

void GeoDataModel::setSourceHref(const QString &sourceHref)
{
    detach();

    p()->m_map.setSourceHref( sourceHref );
}

GeoDataModelPrivate *GeoDataModel::p() const
{
    return static_cast<GeoDataModelPrivate *>( d );
}

}
