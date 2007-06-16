//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#include "KMLContainer.h"
#include "KMLPlaceMark.h"

KMLContainer::KMLContainer()
{
}

KMLContainer::~KMLContainer()
{
    qDebug("KMLContainer::~KMLContainer(). Object count: %d", m_placemarkVector.count());

    foreach ( KMLPlaceMark* placemark, m_placemarkVector ) {
        delete placemark;
    }
}

void KMLContainer::addPlaceMark( KMLPlaceMark* placemark )
{
    m_placemarkVector.append( placemark );
}
