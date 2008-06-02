//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


#ifndef XMLHANDLER_H
#define XMLHANDLER_H


#include <QtCore/QDebug>
#include <QtXml/QXmlDefaultHandler>


class GeoDataPlacemark;
class PlaceMarkContainer;

// FIXME: memory leak in this class (m_placeMarkContainer), but beware
// that the ownership is unclear if a non-null PlaceMarkContainer pointer
// is passed via constructor. Eventually use Q_DISABLE_COPY.

class XmlHandler : public QXmlDefaultHandler
{
 public:
    XmlHandler();
    explicit XmlHandler( PlaceMarkContainer* );

    bool startDocument();
    bool stopDocument();

    bool startElement( const QString&, const QString&, const QString &name, 
                       const QXmlAttributes& attrs );
    bool endElement( const QString&, const QString&, const QString &name );

    bool characters( const QString& str );

 private:
    PlaceMarkContainer  *m_placeMarkContainer;
    GeoDataPlacemark    *m_placemark;

    QString              m_currentText;

    // State machine?
    bool                 m_inKml;
    bool                 m_inPlacemark;
    bool                 m_inPoint;

    bool                 m_coordsset;
};

#endif // XMLHANDLER_H
