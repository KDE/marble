//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//

#ifndef KMLFOLDER_H
#define KMLFOLDER_H

#include "KMLFeature.h"

class PlaceMark;
class PlaceContainer;

class KMLFolder : public KMLFeature
{
 public:
    KMLFolder();
    ~KMLFolder();

    void addPlaceMark( PlaceMark* placemark );
    PlaceContainer& getPlaceContainer() const;

 private:
    // Use PlaceContainer instead of QVector <KMLPlaceMark*>
    PlaceContainer* m_placecontainer;
};

#endif // KMLFOLDER_H
