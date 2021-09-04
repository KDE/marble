// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2017 Dennis Nienhüser <nienhueser@kde.org>

#ifndef MARBLE_GEODATARELATION_H
#define MARBLE_GEODATARELATION_H

#include "GeoDataCoordinates.h"
#include "GeoDataPlacemark.h"

#include "geodata_export.h"

namespace Marble
{
class GeoDataRelationPrivate;
enum class OsmType;

class GEODATA_EXPORT GeoDataRelation: public GeoDataFeature
{
public:
    enum RelationType {
        UnknownType = 0,
        RouteRoad = 1 << 1,
        RouteDetour = 1 << 2,
        RouteFerry = 1 << 3,
        RouteTrain = 1 << 4,
        RouteSubway = 1 << 5,
        RouteTram = 1 << 6,
        RouteBus = 1 << 7,
        RouteTrolleyBus = 1 << 8,
        RouteBicycle = 1 << 9,
        RouteMountainbike = 1 << 10,
        RouteFoot = 1 << 11,
        RouteHiking = 1 << 12,
        RouteHorse = 1 << 13,
        RouteInlineSkates = 1 << 14,
        RouteSkiDownhill = 1 << 15,
        RouteSkiNordic = 1 << 16,
        RouteSkitour = 1 << 17,
        RouteSled = 1 << 18
    };

    Q_DECLARE_FLAGS(RelationTypes, RelationType)

    GeoDataRelation();
    ~GeoDataRelation() override;
    GeoDataRelation(const GeoDataRelation &other);
    GeoDataRelation & operator=(GeoDataRelation other);
    bool operator<(const GeoDataRelation &other) const;

    const char* nodeType() const override;
    GeoDataFeature * clone() const override;

    void addMember(const GeoDataFeature* feature, qint64 id, OsmType type, const QString &role);
    QSet<const GeoDataFeature*> members() const;

    OsmPlacemarkData &osmData();
    const OsmPlacemarkData &osmData() const;

    RelationType relationType() const;
    QSet<qint64> memberIds() const;
    bool containsAnyOf(const QSet<qint64> &memberIds) const;

private:
    GeoDataRelationPrivate* d_ptr;
    Q_DECLARE_PRIVATE(GeoDataRelation)

};

}

#endif
