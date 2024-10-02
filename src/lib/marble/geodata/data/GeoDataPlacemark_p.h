// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef MARBLE_GEODATAPLACEMARKPRIVATE_H
#define MARBLE_GEODATAPLACEMARKPRIVATE_H

#include "GeoDataFeature_p.h"

#include "GeoDataLinearRing.h"
#include "GeoDataMultiGeometry.h"
#include "GeoDataMultiTrack.h"
#include "GeoDataPoint.h"
#include "GeoDataPolygon.h"
#include "GeoDataTrack.h"
#include "GeoDataTypes.h"
#include "osm/OsmPlacemarkData.h"

namespace Marble
{

class GeoDataPlacemarkExtendedData
{
public:
    GeoDataPlacemarkExtendedData()
        : m_area(-1.0)
        , m_isBalloonVisible(false)
    {
        // nothing to do
    }
    GeoDataPlacemarkExtendedData &operator=(const GeoDataPlacemarkExtendedData &other) = default;

    bool operator==(const GeoDataPlacemarkExtendedData &other) const
    {
        return m_countrycode == other.m_countrycode && m_area == other.m_area && m_state == other.m_state;
    }

    bool operator!=(const GeoDataPlacemarkExtendedData &other) const
    {
        return !(*this == other);
    }

    QString m_countrycode; // Country code.
    qreal m_area; // Area in square kilometer
    QString m_state; // State
    bool m_isBalloonVisible; // Visibility of balloon
};

class GeoDataPlacemarkPrivate : public GeoDataFeaturePrivate
{
    Q_DECLARE_TR_FUNCTIONS(GeoDataPlacemark)

public:
    GeoDataPlacemarkPrivate()
        : m_geometry(new GeoDataPoint)
        , m_population(-1)
        , m_placemarkExtendedData(nullptr)
        , m_visualCategory(GeoDataPlacemark::Default)
        , m_osmPlacemarkData(nullptr)
    {
    }

    GeoDataPlacemarkPrivate(const GeoDataPlacemarkPrivate &other)
        : GeoDataFeaturePrivate(other)
        , m_geometry(other.m_geometry->copy())
        , m_population(other.m_population)
        , m_placemarkExtendedData(nullptr)
        , m_visualCategory(other.m_visualCategory)
        , m_osmPlacemarkData(nullptr)
    {
        if (other.m_placemarkExtendedData) {
            m_placemarkExtendedData = new GeoDataPlacemarkExtendedData(*other.m_placemarkExtendedData);
        }
        if (other.m_osmPlacemarkData) {
            m_osmPlacemarkData = new OsmPlacemarkData(*other.m_osmPlacemarkData);
        }
    }

    ~GeoDataPlacemarkPrivate() override
    {
        delete m_geometry;
        delete m_placemarkExtendedData;
        delete m_osmPlacemarkData;
    }

    GeoDataPlacemarkPrivate &operator=(const GeoDataPlacemarkPrivate &other)
    {
        if (this == &other) {
            return *this;
        }

        GeoDataFeaturePrivate::operator=(other);

        m_population = other.m_population;
        m_visualCategory = other.m_visualCategory;

        delete m_geometry;
        m_geometry = other.m_geometry->copy();
        // TODO: why not set parent here to geometry?

        delete m_placemarkExtendedData;
        if (other.m_placemarkExtendedData) {
            m_placemarkExtendedData = new GeoDataPlacemarkExtendedData(*other.m_placemarkExtendedData);
        } else {
            m_placemarkExtendedData = nullptr;
        }

        delete m_osmPlacemarkData;
        if (other.m_osmPlacemarkData) {
            m_osmPlacemarkData = new OsmPlacemarkData(*other.m_osmPlacemarkData);
        } else {
            m_osmPlacemarkData = nullptr;
        }

        return *this;
    }

    EnumFeatureId featureId() const override
    {
        return GeoDataPlacemarkId;
    }

    GeoDataPlacemarkExtendedData &placemarkExtendedData()
    {
        if (!m_placemarkExtendedData) {
            m_placemarkExtendedData = new GeoDataPlacemarkExtendedData;
        }

        return *m_placemarkExtendedData;
    }

    const GeoDataPlacemarkExtendedData &placemarkExtendedData() const
    {
        return m_placemarkExtendedData ? *m_placemarkExtendedData : s_nullPlacemarkExtendedData;
    }

    OsmPlacemarkData &osmPlacemarkData()
    {
        if (!m_osmPlacemarkData) {
            m_osmPlacemarkData = new OsmPlacemarkData;
        }
        return *m_osmPlacemarkData;
    }

    const OsmPlacemarkData &osmPlacemarkData() const
    {
        return m_osmPlacemarkData ? *m_osmPlacemarkData : s_nullOsmPlacemarkData;
    }

    // Data for a Placemark in addition to those in GeoDataFeature.
    GeoDataGeometry *m_geometry; // any GeoDataGeometry entry like locations
    qint64 m_population; // population in number of inhabitants
    GeoDataPlacemarkExtendedData *m_placemarkExtendedData;
    GeoDataPlacemark::GeoDataVisualCategory m_visualCategory; // the visual category

    OsmPlacemarkData *m_osmPlacemarkData;
    static const OsmPlacemarkData s_nullOsmPlacemarkData;
    static const GeoDataPlacemarkExtendedData s_nullPlacemarkExtendedData;
};

} // namespace Marble

#endif
