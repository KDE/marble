// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Torsten Rahn <tackat@kde.org>
//


#ifndef MARBLE_GEODATALOD_H
#define MARBLE_GEODATALOD_H

#include "GeoDataObject.h"


namespace Marble
{

class GeoDataLodPrivate;

/*!
    \class GeoDataLod
    \brief The level of detail which indicates visibility and importance.

    GeoDataLod is a tool class that implements the Lod tag/class
    of the Open Geospatial Consortium standard KML 2.2.

    "Lod" is an abbreviation for "Level of Detail" and refers to 
    the extent of the region of a feature.
    In geodesic coordinates the size of the region can be described 
    in terms of a LatLon(Alt)Box. 
    Projected to screen coordinates the size of a region would naturally
    be measured in pixels.
    The size of such a region varies depending on the distance of the 
    observer towards the feature. 
    
    The "Level of Detail" describes how many pixels a region needs to
    cover in order to be considered "active" and visible. 
    It also describes how "quickly" the feature fades in and out. 
*/

class GEODATA_EXPORT GeoDataLod : public GeoDataObject
{

  public:
/*!
    \brief Creates a new Level of Detail object.
*/
    GeoDataLod();

/*!
    \brief Creates a new Level of Detail object as a copy of @p other.
*/
    GeoDataLod( const GeoDataLod& other );
    
/*!
    \brief Destroys a Level of Detail object.
*/
    ~GeoDataLod() override;


/*!
    \brief Provides type information for downcasting a GeoNode
*/

    bool operator==( const GeoDataLod &other ) const;
    bool operator!=( const GeoDataLod &other ) const;

    const char* nodeType() const override;


/*!
    \brief Returns the minimum size that is needed for the region to be active
    Returns the minimum number of pixels the region has to be projected on for
    the feature to be considered active. 
    A value of 0 would mean no minimum number of pixels which is also the
    standard value.
*/
    qreal minLodPixels() const;


/*!
    \brief Sets the minimum size that is needed for the region to be active
    Sets the minimum number of \a pixels the region has to be projected on for
    the feature to be considered active.
*/
    void setMinLodPixels( qreal pixels );


/*!
    \brief Returns the maximum size that is needed for the region to be active
    Returns the maximum number of pixels the region has to be projected on for
    the feature to be considered active. 
    A value of -1 would mean no minimum number of pixels which is also the
    standard value.
*/
    qreal maxLodPixels() const;


/*!
    \brief Sets the maximum size that is needed for the region to be active
    Sets the maximum number of \a pixels the region has to be projected on for
    the feature to be considered active.
*/
    void setMaxLodPixels( qreal pixels );


/*!
    \brief Returns how "quickly" the region fades when the region is far away.
    Returns the distance (counted from minLodPixels) over which the feature
    fades in or out. 
*/
    qreal minFadeExtent() const;


/*!
    \brief Sets how "quickly" the region fades when the region is far away.
    Sets the distance (counted from minLodPixels) over which the feature fades
    in or out. 
*/
    void setMinFadeExtent( qreal pixels );


/*!
    \brief Returns how "quickly" the region fades when the region is near.
    Returns the distance (counted from maxLodPixels) over which the feature
    fades in or out. 
*/
    qreal maxFadeExtent() const;


/*!
    \brief Sets how "quickly" the region fades when the region is near.
    Sets the distance (counted from maxLodPixels) over which the feature fades
    in or out. 
*/
    void setMaxFadeExtent( qreal pixels );


    // Serialization
/*!
    \brief Serialize the Lod to a stream.
    \param stream the stream.
*/
    void pack( QDataStream& stream ) const override;


/*!
    \brief Unserialize the Lod from a stream.
    \param stream the stream.
*/
    void unpack( QDataStream& stream ) override;

    GeoDataLod &operator=( const GeoDataLod& other );

 protected:
    GeoDataLodPrivate  * const d;
};

}

#endif
