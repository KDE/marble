//
// This file is part of the Marble Project.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007     Inge Wallin  <ingwa@kde.org>"
//


#include "XmlHandler.h"

#include <cmath>
#include <QtGlobal>

#include "global.h"
#include "GeoDataPlacemark.h"
#include "PlaceMarkContainer.h"


XmlHandler::XmlHandler()
{
    m_placeMarkContainer = new PlaceMarkContainer("placecontainer" );
    m_placemark = 0;
    m_coordsset = false;
}


XmlHandler::XmlHandler( PlaceMarkContainer* placeMarkContainer )
{
    m_placeMarkContainer = placeMarkContainer;
    m_placemark = 0;
    m_coordsset = false;
}


bool XmlHandler::startDocument()
{
    m_inKml         = false;
    m_inPlacemark   = false;
    m_inPoint       = false;

    qDebug("Starting KML-Import" );

    return true;
}



bool XmlHandler::startElement( const QString&, const QString&,
                               const QString &name,
                               const QXmlAttributes& attrs )
{
    Q_UNUSED( attrs );

    QString  nameLower = name.toLower();

    if ( nameLower == "kml" ) {
        m_inKml = true;
    }

    if ( m_inKml && nameLower == "placemark" || m_inKml && nameLower == "marbleplacemark" ) {
        m_inPlacemark = true;
        m_coordsset = false;
        m_placemark = new GeoDataPlacemark();
    }

    if ( m_inPlacemark && nameLower == "name" ) {
        m_currentText="";
    }

    if ( m_inPlacemark && nameLower == "description" ) {
        m_currentText="";
    }

    if ( m_inPlacemark && nameLower == "countrycode" ) {
        m_currentText="";
    }

    if ( m_inPlacemark && nameLower == "pop" ) {
        m_currentText="";
    }

    if ( m_inPlacemark && nameLower == "area" ) {
        m_currentText="";
    }

    if ( m_inPlacemark && nameLower == "point" ) {
        m_inPoint = true;
    }

    if ( m_inPlacemark && nameLower == "role" ) {
        m_currentText="";
    }

    if ( m_inPoint && nameLower == "coordinates" ) {
        m_currentText="";
    }

    return true;
}


bool XmlHandler::characters( const QString& str )
{
    m_currentText += str;

    return true;
}


bool XmlHandler::endElement( const QString&, const QString&,
                             const QString &name )
{
    QString nameLower = name.toLower();

    if ( nameLower == "kml" ) {
        m_inKml = false;
    }

    if ( m_inKml && nameLower == "placemark" || m_inKml && nameLower == "marbleplacemark" ) {

//  First we derive the popularity:

        if ( m_coordsset == true )
            m_placeMarkContainer->append( m_placemark );

        m_inPlacemark = false;
    }

    if ( m_inPlacemark && nameLower == "name" ) {
        // qDebug() << m_currentText;
        m_placemark->setName( m_currentText );
    }

    if ( m_inPlacemark && nameLower == "description" ) {
        // qDebug() << m_currentText;
        m_placemark->setDescription( m_currentText );
    }

    if ( m_inPlacemark && nameLower == "countrycode" ) {
//        qDebug() << m_currentText;
        m_placemark->setCountryCode( m_currentText );
    }

    if ( m_inPlacemark && nameLower == "pop" ) {
        qint64 population;
        if ( m_currentText.isEmpty() )
            population = 0;
        else
            population = m_currentText.toLongLong();
        m_placemark->setPopulation( population );

        // else if(population < 7500000) popidx=15;

        // m_placemark->setPopularityIndex( popIdx( population ) );
    }

    if ( m_inPlacemark && nameLower == "area" ) {
        double area;
        if ( m_currentText.isEmpty() )
            area = 0.0;
        else
            area = m_currentText.toDouble();
        m_placemark->setArea( area );
    }

    if ( m_inKml && nameLower == "point" ) {
        m_inPoint = false;
    }

    if ( m_inKml && nameLower == "role" ) {
        m_placemark->setRole( m_currentText.at(0) );
    }

    if ( m_inPoint && nameLower == "coordinates" ) {
        QStringList splitline = m_currentText.split( "," );

        if ( splitline.size() == 2 ) {
            m_placemark->setCoordinate( DEG2RAD * splitline[0].toDouble(),
                                        DEG2RAD * splitline[1].toDouble() );
        }
        if ( splitline.size() == 3 ) {
            m_placemark->setCoordinate( DEG2RAD * splitline[0].toDouble(),
                                        DEG2RAD * splitline[1].toDouble(),
                                        splitline[2].toDouble() );
        }

        m_coordsset = true;
    }

    return true;
}


bool XmlHandler::stopDocument()
{
    qDebug() << "Placemarks: " << m_placeMarkContainer->size();

    return true;
}
