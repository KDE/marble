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

PlaceMarkContainer& KMLContainer::activePlaceMarkContainer()
{
    /*
     * TODO: include only placemarks which is included
     * in a screen area
     */

    m_activePlaceMarkContainer.clear ();

    QVector < KMLPlaceMark* >::const_iterator  it;
    for ( it = m_placemarkVector.constBegin();
          it != m_placemarkVector.constEnd();
          it++ )
    {
        KMLPlaceMark* kmlPlaceMark = *it;
        m_activePlaceMarkContainer.append( kmlPlaceMark );
    }

    qDebug("KMLDocument::activePlaceMarkContainer (). PlaceMarks count: %d", m_activePlaceMarkContainer.count());
    return m_activePlaceMarkContainer;
}
