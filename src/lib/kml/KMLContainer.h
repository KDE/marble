//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#ifndef KMLCONTAINER_H
#define KMLCONTAINER_H

#include <QtCore/QVector>

#include "KMLFeature.h"
#include "PlaceMarkContainer.h"

class KMLPlaceMark;
class ViewParams;

class KMLContainer : public KMLFeature
{
 public:
    virtual ~KMLContainer();

    void addPlaceMark( KMLPlaceMark* placemark );

    virtual void pack( QDataStream& stream ) const;
    virtual void unpack( QDataStream& stream );

    /*
     * Will move this method to KMLDocumentModel in a next step
     */
    PlaceMarkContainer& activePlaceMarkContainer( const ViewParams& viewParams );

 protected:
    KMLContainer();
    QVector < KMLPlaceMark* > m_placemarkVector;

 private:
    PlaceMarkContainer& sphericalActivePlaceMarkContainer( const ViewParams& viewParams );
    PlaceMarkContainer& rectangularActivePlaceMarkContainer( const ViewParams& viewParams );

    PlaceMarkContainer m_activePlaceMarkContainer;
};

#endif // KMLCONTAINER_H
