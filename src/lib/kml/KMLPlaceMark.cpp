//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#include "KMLPlaceMark.h"

#include <QtCore/QDebug>

KMLPlaceMark::KMLPlaceMark()
{
}

KMLPlaceMark::~KMLPlaceMark()
{
}

void KMLPlaceMark::setCoordinate( double lon, double lat )
{
    m_coordinate = GeoPoint( lon, lat );
}

QString KMLPlaceMark::toString() const
{
    return QString();
}
