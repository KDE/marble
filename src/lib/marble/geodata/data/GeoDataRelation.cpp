// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2017 Dennis Nienhüser <nienhueser@kde.org>

#include "GeoDataRelation.h"

#include "GeoDataTypes.h"
#include "OsmPlacemarkData.h"

#include <QSet>

namespace Marble
{

class GeoDataRelationPrivate
{
public:
    QSet<const GeoDataFeature *> m_features;
    OsmPlacemarkData m_osmData;
    QSet<qint64> m_memberIds;

    mutable GeoDataRelation::RelationType m_relationType = GeoDataRelation::UnknownType;
    mutable bool m_relationTypeDirty = true;
    static QHash<QString, GeoDataRelation::RelationType> s_relationTypes;
};

QHash<QString, GeoDataRelation::RelationType> GeoDataRelationPrivate::s_relationTypes;

GeoDataRelation::GeoDataRelation()
    : GeoDataFeature()
    , d_ptr(new GeoDataRelationPrivate)
{
    // nothing to do
}

GeoDataRelation::~GeoDataRelation()
{
    delete d_ptr;
}

GeoDataRelation::GeoDataRelation(const GeoDataRelation &other)
    : GeoDataFeature(other)
    , d_ptr(new GeoDataRelationPrivate)
{
    Q_D(GeoDataRelation);
    d->m_features = other.d_func()->m_features;
    d->m_osmData = other.d_func()->m_osmData;
    d->m_memberIds = other.d_func()->m_memberIds;
    d->m_relationType = other.d_func()->m_relationType;
    d->m_relationTypeDirty = other.d_func()->m_relationTypeDirty;
}

GeoDataRelation &GeoDataRelation::operator=(GeoDataRelation other) // passed by value
{
    GeoDataFeature::operator=(other);
    std::swap(*this->d_ptr, *other.d_ptr);
    return *this;
}

bool GeoDataRelation::operator<(const GeoDataRelation &other) const
{
    if (relationType() == other.relationType()) {
        Q_D(const GeoDataRelation);
        auto const refA = d->m_osmData.tagValue(QStringLiteral("ref"));
        auto const refB = other.osmData().tagValue(QStringLiteral("ref"));
        if (refA == refB) {
            return name() < other.name();
        }
        return refA < refB;
    }
    return relationType() < other.relationType();
}

const char *GeoDataRelation::nodeType() const
{
    return GeoDataTypes::GeoDataRelationType;
}

GeoDataFeature *GeoDataRelation::clone() const
{
    return new GeoDataRelation(*this);
}

void GeoDataRelation::addMember(const GeoDataFeature *feature, qint64 id, OsmType type, const QString &role)
{
    Q_D(GeoDataRelation);
    d->m_features << feature;
    d->m_osmData.addRelation(id, type, role);
    d->m_memberIds << id;
}

QSet<const GeoDataFeature *> GeoDataRelation::members() const
{
    Q_D(const GeoDataRelation);
    return d->m_features;
}

OsmPlacemarkData &GeoDataRelation::osmData()
{
    Q_D(GeoDataRelation);
    d->m_relationTypeDirty = true;
    return d->m_osmData;
}

const OsmPlacemarkData &GeoDataRelation::osmData() const
{
    Q_D(const GeoDataRelation);
    return d->m_osmData;
}

GeoDataRelation::RelationType GeoDataRelation::relationType() const
{
    Q_D(const GeoDataRelation);
    if (!d->m_relationTypeDirty) {
        return d->m_relationType;
    }

    if (GeoDataRelationPrivate::s_relationTypes.isEmpty()) {
        auto &map = GeoDataRelationPrivate::s_relationTypes;
        map[QStringLiteral("road")] = RouteRoad;
        map[QStringLiteral("detour")] = RouteDetour;
        map[QStringLiteral("ferry")] = RouteFerry;
        map[QStringLiteral("train")] = RouteTrain;
        map[QStringLiteral("subway")] = RouteSubway;
        map[QStringLiteral("tram")] = RouteTram;
        map[QStringLiteral("bus")] = RouteBus;
        map[QStringLiteral("trolleybus")] = RouteTrolleyBus;
        map[QStringLiteral("bicycle")] = RouteBicycle;
        map[QStringLiteral("mtb")] = RouteMountainbike;
        map[QStringLiteral("foot")] = RouteFoot;
        map[QStringLiteral("hiking")] = GeoDataRelation::RouteHiking;
        map[QStringLiteral("horse")] = RouteHorse;
        map[QStringLiteral("inline_skates")] = RouteInlineSkates;
    }

    d->m_relationType = GeoDataRelation::UnknownType;
    d->m_relationTypeDirty = false;
    if (d->m_osmData.containsTag(QStringLiteral("type"), QStringLiteral("route"))) {
        auto const route = d->m_osmData.tagValue(QStringLiteral("route"));
        if (route == QStringLiteral("piste")) {
            auto const piste = d->m_osmData.tagValue(QStringLiteral("piste:type"));
            if (piste == QStringLiteral("downhill")) {
                d->m_relationType = RouteSkiDownhill;
            } else if (piste == QStringLiteral("nordic")) {
                d->m_relationType = RouteSkiNordic;
            } else if (piste == QStringLiteral("skitour")) {
                d->m_relationType = RouteSkitour;
            } else if (piste == QStringLiteral("sled")) {
                d->m_relationType = RouteSled;
            }
        } else {
            d->m_relationType = GeoDataRelationPrivate::s_relationTypes.value(route, UnknownType);
        }
    }

    return d->m_relationType;
}

QSet<qint64> GeoDataRelation::memberIds() const
{
    Q_D(const GeoDataRelation);
    return d->m_memberIds;
}

bool GeoDataRelation::containsAnyOf(const QSet<qint64> &memberIds) const
{
    Q_D(const GeoDataRelation);
    return d->m_memberIds.intersects(memberIds);
}

Q_DECLARE_OPERATORS_FOR_FLAGS(GeoDataRelation::RelationTypes)

}
