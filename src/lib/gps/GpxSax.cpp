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
#include "Track.h"
#include "TrackContainer.h"
#include "TrackSegment.h"
#include "TrackPoint.h"
#include "GpxFile.h"

#include <QtXml/QXmlAttributes>
#include <QMessageBox>
/*
GpxSax::GpxSax(WaypointContainer *wptContainer, 
               TrackContainer *trkContainer)
{
    m_wptContainer = wptContainer;
    m_trackContainer = trkContainer;
    
    m_tempLat = 0.0;
    m_tempLon = 0.0;
}*/

GpxSax::GpxSax( GpxFile *gpxFile )
{
    m_gpxFile = gpxFile;
    
    m_tempLat = 0.0;
    m_tempLon = 0.0;

    m_track = 0;
    m_trackSeg = 0;
}

GpxSax::~GpxSax()
{
    delete m_track;
    delete m_trackSeg;
}

bool GpxSax::startElement( const QString &namespaceURI,
                           const QString &localName,
                           const QString &qName,
                           const QXmlAttributes &attributes)
{
    Q_UNUSED( namespaceURI );
    Q_UNUSED( localName );

    if(qName == "wpt"){
        m_tempLat = (attributes.value("lat")).toDouble();
        m_tempLon = (attributes.value("lon")).toDouble();
        /*
        m_wptContainer->append( new Waypoint( m_tempLat, m_tempLon ));
        */
        m_gpxFile->addWaypoint( new Waypoint( m_tempLat, m_tempLon ));
    }
    else if ( qName == "trk") {
        m_track = new Track();
    }
    else if (qName == "trkseg") {
        m_trackSeg = new TrackSegment();
    }
    else if (qName == "trkpt") {
        m_tempLat = (attributes.value("lat")).toDouble();
        m_tempLon = (attributes.value("lon")).toDouble();
        m_trackSeg->append( new TrackPoint( m_tempLat, m_tempLon ));
    }
    return true;
}

bool GpxSax::endElement( const QString &namespaceURI,
                         const QString &localName,
                         const QString &qName )
{
    
    Q_UNUSED( namespaceURI );
    Q_UNUSED( localName );
    
    if ( qName == "trkseg") {
        m_trackSeg->createBoundingBox();
        m_track -> append( m_trackSeg );
        m_trackSeg = 0;
    }
    else if ( qName == "trk") {
        m_track->createBoundingBox();
//         m_trackContainer->append(m_track);
        m_gpxFile->addTrack( m_track );
        m_track =0;
    }

    return true;
}

bool GpxSax::fatalError(const QXmlParseException &exception)
{
    QMessageBox::warning(0, QObject::tr("GpxSax"),
                         QObject::tr("Parse error at line %1, column "
                                 "%2:\n%3.")
                                 .arg(exception.lineNumber())
                                 .arg(exception.columnNumber())
                                 .arg(exception.message()));
    return false;
}
