//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef MARBLE_GEODATAPLACEMARKPRIVATE_H
#define MARBLE_GEODATAPLACEMARKPRIVATE_H

#include "GeoDataFeature_p.h"

#include "GeoDataPoint.h"
#include "GeoDataLinearRing.h"
#include "GeoDataPolygon.h"
#include "GeoDataMultiTrack.h"
#include "GeoDataTrack.h"
#include "GeoDataTypes.h"
#include "GeoDataMultiGeometry.h"
#include "osm/OsmPlacemarkData.h"

namespace Marble
{

class GeoDataPlacemarkExtendedData
{
public:
    GeoDataPlacemarkExtendedData() :
        m_area( -1.0 ),
        m_isBalloonVisible( false )
    {
        // nothing to do
    }
    GeoDataPlacemarkExtendedData & operator=(const GeoDataPlacemarkExtendedData &other)
    {
        m_countrycode = other.m_countrycode;
        m_area = other.m_area;
        m_state = other.m_state;
        m_isBalloonVisible = other.m_isBalloonVisible;
        return *this;
    }

    bool operator==(const GeoDataPlacemarkExtendedData &other) const
    {
        return m_countrycode == other.m_countrycode &&
                m_area == other.m_area &&
                m_state == other.m_state;
    }

    QString             m_countrycode;  // Country code.
    qreal               m_area;         // Area in square kilometer
    QString             m_state;        // State
    bool                m_isBalloonVisible;  //Visibility of balloon
};

class GeoDataPlacemarkPrivate : public GeoDataFeaturePrivate
{
    Q_DECLARE_TR_FUNCTIONS(GeoDataPlacemark)

  public:
    GeoDataPlacemarkPrivate() :
        m_geometry(new GeoDataPoint),
        m_population( -1 ),
        m_placemarkExtendedData(nullptr),
        m_visualCategory(GeoDataPlacemark::Default)
    {
    }

    GeoDataPlacemarkPrivate(const GeoDataPlacemarkPrivate& other)
      : GeoDataFeaturePrivate(other),
        m_geometry(cloneGeometry(other.m_geometry)),
        m_population(other.m_population),
        m_placemarkExtendedData(nullptr),
        m_visualCategory(other.m_visualCategory)
    {
        if (other.m_placemarkExtendedData) {
            m_placemarkExtendedData = new GeoDataPlacemarkExtendedData(*other.m_placemarkExtendedData);
        }
    }

    virtual ~GeoDataPlacemarkPrivate()
    {
        delete m_geometry;
        delete m_placemarkExtendedData;
    }

    GeoDataPlacemarkPrivate& operator=( const GeoDataPlacemarkPrivate& other )
    {
        if ( this == &other ) {
            return *this;
        }

        GeoDataFeaturePrivate::operator=( other );

        m_population = other.m_population;
        m_visualCategory = other.m_visualCategory;

        delete m_geometry;
        m_geometry = cloneGeometry(other.m_geometry);
        // TODO: why not set parent here to geometry?

        delete m_placemarkExtendedData;
        if (other.m_placemarkExtendedData) {
            m_placemarkExtendedData = new GeoDataPlacemarkExtendedData(*other.m_placemarkExtendedData);
        } else {
            m_placemarkExtendedData = nullptr;
        }

        return *this;
    }

    virtual EnumFeatureId featureId() const
    {
        return GeoDataPlacemarkId;
    }

    static GeoDataGeometry * cloneGeometry(const GeoDataGeometry * geometry)
    {
        GeoDataGeometry * result = nullptr;
        if (geometry) {
            switch (geometry->geometryId()) {
            case InvalidGeometryId:
                break;
            case GeoDataPointId:
                result = new GeoDataPoint(*static_cast<const GeoDataPoint*>(geometry));
                break;
            case GeoDataLineStringId:
                result = new GeoDataLineString(*static_cast<const GeoDataLineString*>(geometry));
                break;
            case GeoDataLinearRingId:
                result = new GeoDataLinearRing(*static_cast<const GeoDataLinearRing*>(geometry));
                break;
            case GeoDataPolygonId:
                result = new GeoDataPolygon(*static_cast<const GeoDataPolygon*>(geometry));
                break;
            case GeoDataMultiGeometryId:
                result = new GeoDataMultiGeometry(*static_cast<const GeoDataMultiGeometry*>(geometry));
                break;
            case GeoDataTrackId:
                result = new GeoDataTrack(*static_cast<const GeoDataTrack*>(geometry));
                break;
            case GeoDataMultiTrackId:
                result = new GeoDataMultiTrack(*static_cast<const GeoDataMultiTrack*>(geometry));
                break;
            case GeoDataModelId:
                break;
            default:
                break;
            }
        }
        return result;
    }

    GeoDataPlacemarkExtendedData & placemarkExtendedData()
    {
        if (!m_placemarkExtendedData) {
            m_placemarkExtendedData = new GeoDataPlacemarkExtendedData;
        }

        return *m_placemarkExtendedData;
    }

    const GeoDataPlacemarkExtendedData & placemarkExtendedData() const
    {
        return m_placemarkExtendedData ? *m_placemarkExtendedData : s_nullPlacemarkExtendedData;
    }

    // Data for a Placemark in addition to those in GeoDataFeature.
    GeoDataGeometry    *m_geometry;     // any GeoDataGeometry entry like locations
    qint64              m_population;   // population in number of inhabitants
    GeoDataPlacemarkExtendedData *m_placemarkExtendedData;
    GeoDataPlacemark::GeoDataVisualCategory m_visualCategory; // the visual category

    static const OsmPlacemarkData s_nullOsmPlacemarkData;
    static const GeoDataPlacemarkExtendedData s_nullPlacemarkExtendedData;
};

} // namespace Marble

#endif
