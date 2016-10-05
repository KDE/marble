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
#include "GeoDataLink.h"
#include "GeoDataLocation.h"
#include "GeoDataOrientation.h"
#include "GeoDataResourceMap.h"
#include "GeoDataScale.h"

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
    Q_D(const GeoDataModel);
    const GeoDataModelPrivate *other_d = other.d_func();

    return equals(other) &&
           d->m_coordinates == other_d->m_coordinates &&
           d->m_scale == other_d->m_scale &&
           d->m_orientation == other_d->m_orientation &&
           d->m_location == other_d->m_location &&
           d->m_link == other_d->m_link &&
           d->m_map == other_d->m_map &&
           d->m_targetHref == other_d->m_targetHref &&
           d->m_sourceHref == other_d->m_sourceHref;
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
    Q_D(const GeoDataModel);
    return d->m_coordinates;
}

GeoDataCoordinates &GeoDataModel::coordinates()
{
    detach();

    Q_D(GeoDataModel);
    return d->m_coordinates;
}

const GeoDataLocation &GeoDataModel::location() const
{
    Q_D(const GeoDataModel);
    return d->m_location;
}

GeoDataLocation &GeoDataModel::location()
{
    detach();

    Q_D(GeoDataModel);
    return d->m_location;
}

void GeoDataModel::setCoordinates(const GeoDataCoordinates &coordinates)
{
    detach();

    Q_D(GeoDataModel);
    d->m_coordinates = coordinates;
}

void GeoDataModel::setLocation(const GeoDataLocation &location)
{
    detach();

    Q_D(GeoDataModel);
    d->m_location = location;
}

const GeoDataLink &GeoDataModel::link() const
{
    Q_D(const GeoDataModel);
    return d->m_link;
}

GeoDataLink &GeoDataModel::link()
{
    detach();

    Q_D(GeoDataModel);
    return d->m_link;
}

void GeoDataModel::setLink( const GeoDataLink &link )
{
    detach();

    Q_D(GeoDataModel);
    d->m_link = link;
}

const GeoDataScale &GeoDataModel::scale() const
{
    Q_D(const GeoDataModel);
    return d->m_scale;
}

GeoDataScale &GeoDataModel::scale()
{
    detach();

    Q_D(GeoDataModel);
    return d->m_scale;
}

void GeoDataModel::setScale(const GeoDataScale &scale)
{
    detach();

    Q_D(GeoDataModel);
    d->m_scale = scale;
}

const GeoDataOrientation &GeoDataModel::orientation() const
{
    Q_D(const GeoDataModel);
    return d->m_orientation;
}

GeoDataOrientation &GeoDataModel::orientation()
{
    detach();

    Q_D(GeoDataModel);
    return d->m_orientation;
}

void GeoDataModel::setOrientation(const GeoDataOrientation &orientation)
{
    detach();

    Q_D(GeoDataModel);
    d->m_orientation = orientation;
}

const GeoDataResourceMap &GeoDataModel::resourceMap() const
{
    Q_D(const GeoDataModel);
    return d->m_map;
}

GeoDataResourceMap &GeoDataModel::resourceMap()
{
    detach();

    Q_D(GeoDataModel);
    return d->m_map;
}

void GeoDataModel::setResourceMap(const GeoDataResourceMap &map)
{
    detach();

    Q_D(GeoDataModel);
    d->m_map = map;
}

QString GeoDataModel::targetHref() const
{
    Q_D(const GeoDataModel);
    return d->m_map.targetHref();
}

void GeoDataModel::setTargetHref(const QString &targetHref)
{
    detach();

    Q_D(GeoDataModel);
    d->m_map.setTargetHref( targetHref );
}

QString GeoDataModel::sourceHref() const
{
    Q_D(const GeoDataModel);
    return d->m_map.sourceHref();
}

void GeoDataModel::setSourceHref(const QString &sourceHref)
{
    detach();

    Q_D(GeoDataModel);
    d->m_map.setSourceHref( sourceHref );
}

}
