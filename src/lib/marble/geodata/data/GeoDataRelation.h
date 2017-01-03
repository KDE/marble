//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2017    Dennis Nienhüser <nienhueser@kde.org>

#ifndef MARBLE_GEODATARELATION_H
#define MARBLE_GEODATARELATION_H

#include "GeoDataCoordinates.h"
#include "GeoDataPlacemark.h"

#include "geodata_export.h"

namespace Marble
{
class GeoDataRelationPrivate;

class GEODATA_EXPORT GeoDataRelation: public GeoDataFeature
{
public:
    enum RelationType {
        UnknownType,
        RouteHiking
    };

    GeoDataRelation();
    ~GeoDataRelation();
    GeoDataRelation(const GeoDataRelation &other);
    GeoDataRelation & operator=(GeoDataRelation other);
    const char* nodeType() const override;
    GeoDataFeature * clone() const override;

    void add(const GeoDataFeature* placemark);
    QSet<const GeoDataFeature*> members() const;

    OsmPlacemarkData &osmData();
    const OsmPlacemarkData &osmData() const;

    RelationType relationType() const;
    QSet<qint64> memberIds() const;

private:
    GeoDataRelationPrivate* d_ptr;
    Q_DECLARE_PRIVATE(GeoDataRelation)

};

}

#endif
