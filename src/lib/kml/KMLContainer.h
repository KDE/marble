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

class KMLContainer : public KMLFeature
{
 public:
    virtual ~KMLContainer();

    void addPlaceMark( KMLPlaceMark* placemark );

    /*
     * Will move this method to KMLDocumentModel in a next step
     */
    PlaceMarkContainer& activePlaceMarkContainer();

 protected:
    KMLContainer();

 protected:
     QVector <KMLPlaceMark*> m_placemarkVector;

 private:
     PlaceMarkContainer m_activePlaceMarkContainer;
};

#endif // KMLCONTAINER_H
