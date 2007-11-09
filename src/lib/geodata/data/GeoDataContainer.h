//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
// Copyright 2007      Inge Wallin   <inge@lysator.liu.se>
//


#ifndef GEODATACONTAINER_H
#define GEODATACONTAINER_H

#include <QtCore/QVector>

#include "GeoDataFeature.h"
#include "PlaceMarkContainer.h"

//class PlaceMark;
class ViewParams;

/**
 * @short  A base class that can hold GeoDataPlaceMarks
 * FIXME: Should hold GeoDataFeatures
 *
 * GeoDataContainer is the base class for the GeoData container
 * classes GeoDataFolder and GeoDataDocument.  It is never
 * instantiated by itself, but is always used as part of a derived
 * class.
 *
 * It is based on GeoDataFeature, and it only adds a
 * PlaceMarkContainer to it, making it a Feature that can hold other
 * Features.
 *
 * @see GeoDataFolder
 * @see GeoDataDocument
 */
class GeoDataContainer : public GeoDataFeature
{
 public:
    /// Destruct the GeoDataContainer
    virtual ~GeoDataContainer();

    void addPlaceMark( GeoDataPlacemark* placemark );

    virtual void pack( QDataStream& stream ) const;
    virtual void unpack( QDataStream& stream );

    /*
     * Will move this method to GeoDataDocumentModel in a next step
     */
    PlaceMarkContainer& activePlaceMarkContainer( const ViewParams& viewParams );

 protected:
    GeoDataContainer();
    QVector < GeoDataPlacemark* > m_placemarkVector;

 private:
    PlaceMarkContainer& sphericalActivePlaceMarkContainer( const ViewParams& viewParams );
    PlaceMarkContainer& rectangularActivePlaceMarkContainer( const ViewParams& viewParams );

    PlaceMarkContainer m_activePlaceMarkContainer;
};

#endif // GEODATACONTAINER_H
