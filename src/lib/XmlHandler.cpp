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

#include "global.h"
#include "PlaceMark.h"
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
    m_inKml       = false;
    m_inPlacemark = false;
    m_inPoint     = false;

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

    if ( m_inKml && nameLower == "placemark" ) {
        m_inPlacemark = true;
        m_coordsset = false;
        m_placemark = new PlaceMark();
        m_placemark->setSymbol( 0 );
    }

    if ( m_inPlacemark && nameLower == "name" ) {
        m_currentText="";
    }

    if ( m_inPlacemark && nameLower == "description" ) {
        m_currentText="";
    }

    if ( m_inPlacemark && nameLower == "countrynamecode" ) {
        m_currentText="";
    }

    if ( m_inPlacemark && nameLower == "pop" ) {
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

    if ( m_inKml && nameLower == "placemark" ) {

        if ( m_placemark->role() == 'P' )      m_placemark->setSymbol(16);
        else if ( m_placemark->role() == 'M' ) m_placemark->setSymbol(17);
        else if ( m_placemark->role() == 'H' ) m_placemark->setSymbol(18);
        else if ( m_placemark->role() == 'V' ) m_placemark->setSymbol(19);
        else if ( m_placemark->role() == 'F' ) m_placemark->setSymbol(20);
        else if ( m_placemark->role() == 'N' ) m_placemark->setSymbol( ( m_placemark->popidx() -1 ) / 4 * 4 );
        else if ( m_placemark->role() == 'R' ) m_placemark->setSymbol( ( m_placemark->popidx() -1 ) / 4 * 4 + 2);
        else if ( m_placemark->role() == 'C' || m_placemark->role() == 'B' ) m_placemark->setSymbol( ( m_placemark->popidx() -1 ) / 4 * 4 + 3 );

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

    if ( m_inPlacemark && nameLower == "countrynamecode" ) {
        // qDebug() << m_currentText;
        m_placemark->setCountryCode( m_currentText );
    }

    if ( m_inPlacemark && nameLower == "pop" ) {
        int  population;
        if ( m_currentText.isEmpty() )
            population = 0;
        else
            population = m_currentText.toInt();

        m_placemark->setPopulation( population );

        // else if(population < 7500000) popidx=15;

        m_placemark->setPopidx( popIdx( population ) );
    }

    if ( m_inKml && nameLower == "point" ) {
        m_inPoint = false;
    }

    if ( m_inKml && nameLower == "role" ) {
        m_placemark->setRole( m_currentText.at(0) );
    }

    if ( m_inPoint && nameLower == "coordinates" ) {
        QStringList splitline = m_currentText.split( "," );

        m_placemark->setCoordinate( DEG2RAD * splitline[0].toFloat(),
                                    -DEG2RAD * splitline[1].toFloat() );

        if ( splitline.size() == 3 ) {
            int elevation = splitline[2].toInt();
            m_placemark->setPopulation( elevation*1000 );
            m_placemark->setPopidx( popIdx( abs(elevation*1000) ) );
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


int XmlHandler::popIdx( int population )
{
    int popidx = 15;

    if ( population < 2500 )        popidx=1;
    else if ( population < 5000)    popidx=2;
    else if ( population < 7500)    popidx=3;
    else if ( population < 10000)   popidx=4;
    else if ( population < 25000)   popidx=5;
    else if ( population < 50000)   popidx=6;
    else if ( population < 75000)   popidx=7;
    else if ( population < 100000)  popidx=8;
    else if ( population < 250000)  popidx=9;
    else if ( population < 500000)  popidx=10;
    else if ( population < 750000)  popidx=11;
    else if ( population < 1000000) popidx=12;
    else if ( population < 2500000) popidx=13;
    else if ( population < 5000000) popidx=14;

    return popidx;
}
