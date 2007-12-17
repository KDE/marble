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
    m_hasPopularity = false;

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

    if ( m_inKml && nameLower == "placemark" ) {

//  First we derive the popularity:

        if ( m_placemark->role() == 'H' || m_placemark->role() == 'V' )
        {
            qint64 altitude = (qint64)( m_placemark->coordinate().altitude() );
            if ( altitude != 0 )
            {
                m_hasPopularity = true;
                m_placemark->setPopularity( altitude * 1000 );
                m_placemark->setPopularityIndex( cityPopIdx( altitude * 1000 ) );
            }
        }
        else if ( m_placemark->role() == 'K' || m_placemark->role() == 'O' || m_placemark->role() == 'S' )
        {
            double area = m_placemark->area();
            if ( area >= 0.0 )
            {
                m_hasPopularity = true;
                m_placemark->setPopularity( (qint64)(area) );
                m_placemark->setPopularityIndex( areaPopIdx( area ) );
            }
        }
        else if ( m_placemark->role() == 'P' )
        {
            m_placemark->setPopularity( 100000000 );
            m_placemark->setPopularityIndex( cityPopIdx( 100000000 ) );
        }
        else if ( m_placemark->role() == 'M' )
        {
            m_placemark->setPopularity( 1000000 );
            m_placemark->setPopularityIndex( cityPopIdx( 1000000 ) );
        }
        else
        {
            qint64 population = m_placemark->population();
            if ( population >= 0 )
            {
                m_hasPopularity = true;
                m_placemark->setPopularity( population );
                m_placemark->setPopularityIndex( cityPopIdx( population ) );
            }
        }

//  Then we set the visual category:

        if ( m_placemark->role() == 'H' )      m_placemark->setVisualCategory( GeoDataPlacemark::Mountain );
        else if ( m_placemark->role() == 'V' ) m_placemark->setVisualCategory( GeoDataPlacemark::Volcano );
        else if ( m_placemark->role() == 'P' ) m_placemark->setVisualCategory( GeoDataPlacemark::GeographicPole );
        else if ( m_placemark->role() == 'M' ) m_placemark->setVisualCategory( GeoDataPlacemark::MagneticPole );
        else if ( m_placemark->role() == 'W' ) m_placemark->setVisualCategory( GeoDataPlacemark::ShipWreck );
        else if ( m_placemark->role() == 'F' ) m_placemark->setVisualCategory( GeoDataPlacemark::AirPort );
        else if ( m_placemark->role() == 'K' ) m_placemark->setVisualCategory( GeoDataPlacemark::Continent );
        else if ( m_placemark->role() == 'O' ) m_placemark->setVisualCategory( GeoDataPlacemark::Ocean );
        else if ( m_placemark->role() == 'S' ) m_placemark->setVisualCategory( GeoDataPlacemark::Nation );
        else if ( m_placemark->role() == 'N' ) m_placemark->setVisualCategory( 
            ( ( GeoDataPlacemark::GeoDataVisualCategory )( (int)( GeoDataPlacemark::SmallCity )
                + ( m_placemark->popularityIndex() -1 ) / 4 * 4 ) ) );
        else if ( m_placemark->role() == 'R' ) m_placemark->setVisualCategory( 
            ( ( GeoDataPlacemark::GeoDataVisualCategory )( (int)( GeoDataPlacemark::SmallStateCapital )
                + ( m_placemark->popularityIndex() -1 ) / 4 * 4 ) ) );
        else if ( m_placemark->role() == 'C' || m_placemark->role() == 'B' ) m_placemark->setVisualCategory( 
            ( ( GeoDataPlacemark::GeoDataVisualCategory )( (int)( GeoDataPlacemark::SmallNationCapital )
                + ( m_placemark->popularityIndex() -1 ) / 4 * 4 ) ) );

        else if ( m_placemark->role() == ' ' && !m_hasPopularity )
            m_placemark->setVisualCategory( GeoDataPlacemark::Default ); // default location

        if ( m_placemark->role() == 'W' && m_placemark->popularityIndex() > 12 )
            m_placemark->setPopularityIndex( 12 );
        if ( m_placemark->role() == 'O' )
            m_placemark->setPopularityIndex( 14 );
        if ( m_placemark->role() == 'K' )
            m_placemark->setPopularityIndex( 15 );
        if ( m_placemark->role() == 'S' && m_placemark->popularityIndex() < 12 )
            m_placemark->setPopularityIndex( 12 );

        if ( m_coordsset == true )
            m_placeMarkContainer->append( m_placemark );

        m_inPlacemark = false;

        m_hasPopularity = false;
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
        m_hasPopularity = true;
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
                                       -DEG2RAD * splitline[1].toDouble() );
        }
        if ( splitline.size() == 3 ) {
            m_placemark->setCoordinate( DEG2RAD * splitline[0].toDouble(),
                                       -DEG2RAD * splitline[1].toDouble(),
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


int XmlHandler::cityPopIdx( qint64 population )
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

int XmlHandler::areaPopIdx( double area )
{
    Q_UNUSED( area );
    int popidx = 15;
/*
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
*/
    return popidx;
}
