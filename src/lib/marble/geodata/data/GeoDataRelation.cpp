//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2017    Dennis Nienhüser <nienhueser@kde.org>

#include "GeoDataRelation.h"

#include "GeoDataTypes.h"
#include "OsmPlacemarkData.h"

#include <QSet>

namespace Marble
{

class GeoDataRelationPrivate
{
public:
    QSet<const GeoDataFeature*> m_features;
    OsmPlacemarkData m_osmData;

    static QHash<QString, GeoDataRelation::RelationType> s_relationTypes;
};

QHash<QString, GeoDataRelation::RelationType> GeoDataRelationPrivate::s_relationTypes;

GeoDataRelation::GeoDataRelation() :
    GeoDataFeature(),
    d_ptr(new GeoDataRelationPrivate)
{
    // nothing to do
}

GeoDataRelation::~GeoDataRelation()
{
    delete d_ptr;
}

GeoDataRelation::GeoDataRelation(const GeoDataRelation &other) :
    GeoDataFeature(other),
    d_ptr(new GeoDataRelationPrivate)
{
    Q_D(GeoDataRelation);
    d->m_features = other.d_func()->m_features;
    d->m_osmData = other.d_func()->m_osmData;
}

GeoDataRelation &GeoDataRelation::operator=(GeoDataRelation other) // passed by value
{
    GeoDataFeature::operator=(other);
    std::swap(*this->d_ptr, *other.d_ptr);
    return *this;
}

const char *GeoDataRelation::nodeType() const
{
    return GeoDataTypes::GeoDataRelationType;
}

GeoDataFeature *GeoDataRelation::clone() const
{
    return new GeoDataRelation(*this);
}

void GeoDataRelation::addMember(const GeoDataFeature *feature, qint64 id, const QString &role)
{
    Q_D(GeoDataRelation);
    d->m_features << feature;
    d->m_osmData.addRelation(id, role);
}

QSet<const GeoDataFeature *> GeoDataRelation::members() const
{
    Q_D(const GeoDataRelation);
    return d->m_features;
}

OsmPlacemarkData &GeoDataRelation::osmData()
{
    Q_D(GeoDataRelation);
    return d->m_osmData;
}

const OsmPlacemarkData &GeoDataRelation::osmData() const
{
    Q_D(const GeoDataRelation);
    return d->m_osmData;
}

GeoDataRelation::RelationType GeoDataRelation::relationType() const
{
    if (GeoDataRelationPrivate::s_relationTypes.isEmpty()) {
        auto &map = GeoDataRelationPrivate::s_relationTypes;
        map["road"] = RouteRoad;
        map["detour"] = RouteDetour;
        map["ferry"] = RouteFerry;
        map["train"] = RouteTrain;
        map["tram"] = RouteTram;
        map["bus"] = RouteBus;
        map["trolleybus"] = RouteTrolleyBus;
        map["bicycle"] = RouteBicycle;
        map["mtb"] = RouteMountainbike;
        map["foot"] = RouteFoot;
        map["hiking"] = GeoDataRelation::RouteHiking;
        map["horse"] = RouteHorse;
        map["inline_skates"] = RouteInlineSkates;
        map["ski"] = RouteSki;
    }

    Q_D(const GeoDataRelation);
    if (d->m_osmData.containsTag(QStringLiteral("type"), QStringLiteral("route"))) {
        auto const route = d->m_osmData.tagValue(QStringLiteral("route"));
        return GeoDataRelationPrivate::s_relationTypes.value(route, UnknownType);
    }

    return UnknownType;
}

QSet<qint64> GeoDataRelation::memberIds() const
{
    QSet<qint64> result;
    Q_D(const GeoDataRelation);
    for (auto const &member : d->m_features) {
        if (member->nodeType() == GeoDataTypes::GeoDataPlacemarkType) {
            result << static_cast<const GeoDataPlacemark*>(member)->osmData().oid();
        } else if (member->nodeType() == GeoDataTypes::GeoDataRelationType) {
            result << static_cast<const GeoDataRelation*>(member)->osmData().oid();
        }
    }
    return result;
}

}
