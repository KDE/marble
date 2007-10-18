//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#ifndef GEODATACONTAINER_H
#define GEODATACONTAINER_H

#include <QtCore/QVector>

#include "GeoDataFeature.h"
#include "PlaceMarkContainer.h"

class GeoDataPlaceMark;
class ViewParams;

class GeoDataContainer : public GeoDataFeature
{
 public:
    virtual ~GeoDataContainer();

    void addPlaceMark( GeoDataPlaceMark* placemark );

    virtual void pack( QDataStream& stream ) const;
    virtual void unpack( QDataStream& stream );

    /*
     * Will move this method to GeoDataDocumentModel in a next step
     */
    PlaceMarkContainer& activePlaceMarkContainer( const ViewParams& viewParams );

 protected:
    GeoDataContainer();
    QVector < GeoDataPlaceMark* > m_placemarkVector;

 private:
    PlaceMarkContainer& sphericalActivePlaceMarkContainer( const ViewParams& viewParams );
    PlaceMarkContainer& rectangularActivePlaceMarkContainer( const ViewParams& viewParams );

    PlaceMarkContainer m_activePlaceMarkContainer;
};

#endif // GEODATACONTAINER_H
