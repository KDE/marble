//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#include "KMLDocument.h"
#include "placecontainer.h"

KMLDocument::KMLDocument()
{
    m_placecontainer = new PlaceContainer();
}

void KMLDocument::addPlaceMark( PlaceMark* placemark )
{
    m_placecontainer->append( placemark );
}

PlaceContainer& KMLDocument::getPlaceContainer() const
{
    return *m_placecontainer;
}
