//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#ifndef KMLPLACEMARK_H
#define KMLPLACEMARK_H

#include "KMLFeature.h"

#include "GeoPoint.h"

class KMLPlaceMark : public KMLFeature
{
 public:
    KMLPlaceMark();
    virtual ~KMLPlaceMark();

    void setCoordinate( double lon, double lat );

    QString toString() const;

 private:
    GeoPoint    m_coordinate;
};

#endif // KMLPLACEMARK_H
