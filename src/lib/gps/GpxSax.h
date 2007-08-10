//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson    <g.real.ate@gmail.com>
//

#ifndef GPXSAX_H
#define GPXSAX_H


#include <QtXml/QXmlDefaultHandler>

class QXmlAttributes;
class QString;
class WaypointContainer;
class Track;
class TrackContainer;
class TrackSegment;
class GpxFile;

class GpxSax : public QXmlDefaultHandler
{
 public:
  //GpxSax( WaypointContainer *wptContainer, TrackContainer *track );
    GpxSax( GpxFile *gpxFile );
    
    bool startElement( const QString &namespaceURI,
                       const QString &localName,
                       const QString &qName,
                       const QXmlAttributes &attributes);
    bool endElement( const QString &namespaceURI,
                     const QString &localName,
                     const QString &qName );
    bool fatalError(const QXmlParseException &exception);
 private:
//     WaypointContainer   *m_wptContainer;
//     TrackContainer      *m_trackContainer;
    GpxFile             *m_gpxFile;
    
    
    Track               *m_track;
    TrackSegment        *m_trackSeg;
    double              m_tempLat;
    double              m_tempLon;
};
#endif

