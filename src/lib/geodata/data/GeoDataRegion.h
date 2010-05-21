//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009 Torsten Rahn <tackat@kde.org>
//


#ifndef MARBLE_GEODATAREGION_H
#define MARBLE_GEODATAREGION_H

#include "GeoDataObject.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataLod.h"


namespace Marble
{

class GeoDataFeature;
class GeoDataRegionPrivate;

/*!
    \class GeoDataRegion
    \brief GeoDataRegion describes the visibility and extent of a feature.

    GeoDataRegion is a tool class that implements the Region tag/class
    of the Open Geospatial Consortium standard KML 2.2.

    GeoDataRegion provides all aspects that can be taken into account to find
    out whether a GeoDataFeature (or a GeoDataLink) is visible on the map:

    \li On one hand GeoDataRegion owns a GeoDataLatLon(Alt)Box object.
        GeoDataLatLon(Alt)Box is a bounding box in geodesic coordinates.
        As such GeoDataLatLon(Alt)Box allows to determine whether the feature is
        located inside the area that the observer/camera is looking at
        ("viewLatLonAltBox").
    \li Additionally GeoDataRegion has got a GeoDataLod ("Level of Detail")
        object. GeoDataLod describes the least and maximum size on the
        screen that is required to consider a feature to be active.

     A feature is visible if it's active and if its latLonAltBox covers the area
     that is being looked at by the observer.
*/

class GEODATA_EXPORT GeoDataRegion : public GeoDataObject
{

  public:
/*!
    \brief Creates a new GeoDataRegion object that is not assigned to a \a feature.
    Naturally it's recommended to assign a feature or a link to the region (and
    therefore it's recommended to use one of the other constructors instead).
    This constructor allows to create a stand alone region which can get
    assigned to the feature or link later on.
*/
    GeoDataRegion();

/*!
    \brief Creates a new GeoDataRegion as a copy of \p other.
*/
    GeoDataRegion( const GeoDataRegion& other );
    
/*!
    \brief Creates a new GeoDataRegion object that is associated to a \a feature.
    In the model the feature represents the parent object of the region.
*/
    explicit GeoDataRegion( GeoDataFeature * feature );

    
/*!
    \brief Destroys a Region object.
*/
    virtual ~GeoDataRegion();

    
/*!
    \brief Provides type information for downcasting a GeoNode
*/
    virtual QString nodeType() const;


/*!
    \brief Assigns a feature associated as a parent
*/
    void setParent( GeoDataFeature * feature );

    
/*!
    \brief Returns a geodesic bounding box ("latLonAltBox") of the region.
    Returns a geodesic bounding box that describes the extent of a feature or
    a link.
    If no latLonAltBox has been set then a GeoDataLatLonAltBox object
    will be calculated automatically: If the associated parent object is
    a feature then the geometry that might be associated to the feature
    will be used to calculate the GeoDataLatLonAltBox. Otherwise the
    coordinate will be used to create a GeoDataLatLonAltBox (in case that
    there is no geometry assigned to the feature or if the parent object of the
    region is a GeoDataLink).
*/
    GeoDataLatLonAltBox& latLonAltBox() const;

    
/*!
    \brief Sets the \a latLonAltBox of the region.
    Sets the geodesic bounding box that describes the extent of a feature or
    a link.
*/
    void setLatLonAltBox( const GeoDataLatLonAltBox& latLonAltBox );


/*!
    \brief Returns the region's level of detail.
    The level of detail is returned as a \a lod object. 
    If no \a lod has been set then a GeoDataLod object with default values
    is being returned.
*/
    GeoDataLod& lod() const;

    
/*!
    \brief Sets a region's level of detail.
    The level of detail is set as a \a lod object.
*/
    void setLod( const GeoDataLod& lod );


    // Serialization
/*!
    \brief Serialize the Region to a stream.
    \param stream the stream.
*/
    virtual void pack( QDataStream& stream ) const;


/*!
    \brief Unserialize the Region from a stream.
    \param stream the stream.
*/
    virtual void unpack( QDataStream& stream );

    GeoDataRegion &operator=( const GeoDataRegion& other );

 private:
    void swap( GeoDataRegion & other );
    GeoDataRegionPrivate  *d;
};

}

#endif
