//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson    <g.real.ate@gmail.com>
//

#include "GpxSax.h"
#include "WaypointContainer.h"
#include "Waypoint.h"

#include <QtXml/QXmlAttributes>
#include <QString>

GpxSax::GpxSax(WaypointContainer *wptContainer)
{
    m_wptContainer = wptContainer;
    
    m_tempLat = 0.0;
    m_tempLon = 0.0;
    qDebug("the parser has been created");
}



bool GpxSax::startElement( const QString &namespaceURI,
                           const QString &localName,
                           const QString &qName,
                           const QXmlAttributes &attributes)
{
    Q_UNUSED( namespaceURI );
    Q_UNUSED( localName );

    qDebug("startElement");
    if(qName == "wpt"){
        m_tempLat = (attributes.value("lat")).toDouble();
        m_tempLon = (attributes.value("lon")).toDouble();
        m_wptContainer->append( new Waypoint( m_tempLat, m_tempLon ));
    }else if(qName == "trkpt"){
        m_tempLat = (attributes.value("lat")).toDouble();
        m_tempLon = (attributes.value("lon")).toDouble();
        m_wptContainer->append( new Waypoint( m_tempLat, m_tempLon ));
    }else{
        qDebug("not what i want");
    }
    return true;
}

bool GpxSax::endElement( const QString &namespaceURI,
                         const QString &localName,
                         const QString &qName )
{
    Q_UNUSED( namespaceURI );
    Q_UNUSED( localName );
    Q_UNUSED( qName );

    return true;
}
