
//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#include "KMLFolder.h"
#include "placecontainer.h"

KMLFolder::KMLFolder()
{
    m_placecontainer = new PlaceContainer();
}

KMLFolder::~KMLFolder()
{
    m_placecontainer->deleteAll();
    delete m_placecontainer;
}

void KMLFolder::addPlaceMark( PlaceMark* placemark )
{
    m_placecontainer->append( placemark );
}

PlaceContainer& KMLFolder::getPlaceContainer() const
{
    return *m_placecontainer;
}
