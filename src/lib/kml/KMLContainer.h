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

#include "KMLFeature.h"

#include <QtCore/QVector>

class KMLPlaceMark;

class KMLContainer : public KMLFeature
{
 public:
    virtual ~KMLContainer();

    void addPlaceMark( KMLPlaceMark* placemark );

 protected:
    KMLContainer();

 protected:
     QVector <KMLPlaceMark*> m_placemarkVector;
};

#endif // KMLCONTAINER_H
