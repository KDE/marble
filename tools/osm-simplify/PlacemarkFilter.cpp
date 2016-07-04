//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      David Kolozsvari <freedawson@gmail.com>
//

#include "PlacemarkFilter.h"

#include "GeoDataPlacemark.h"

PlacemarkFilter::PlacemarkFilter(GeoDataDocument *document) :
    BaseFilter(document, GeoDataTypes::GeoDataPlacemarkType)
{
    qDebug() << "Placemark count:" << m_objects.size();
}

