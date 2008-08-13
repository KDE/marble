/*
    Copyright 2008 Henry de Valence <hdevalence@gmail.com>
    
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public 
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "LatLonRunner.h"

#include "MarbleAbstractRunner.h"
#include "MarbleRunnerResult.h"
#include "GeoDataPlacemark.h"
#include "PlaceMarkContainer.h"

#include <QtCore/QString>
#include <QtCore/QStringList>

#include <QtDebug>

namespace Marble
{

LatLonRunner::LatLonRunner(QObject *parent) : MarbleAbstractRunner(parent)
{
    m_cardinals << tr("N", "one-letter uppercase abbreviation for North");
    m_cardinals << tr("E", "one-letter uppercase abbreviation for East");
    m_cardinals << tr("S", "one-letter uppercase abbreviation for South");
    m_cardinals << tr("W", "one-letter uppercase abbreviation for West");
}

LatLonRunner::~LatLonRunner()
{
}

void LatLonRunner::parse(const QString &input)
{
    emit runnerStarted();
    double coordinates[2];
    //lon
    coordinates[0] = 9999;
    //lat
    coordinates[1] = 9999;
    for( int coord = 0; coord < 2; coord++) {
        QString workstr = input.section( ',', coord, coord );
        workstr.remove( QRegExp( "^\\s+" ) ); //remove front spaces
//         qDebug() << "Working on string" << workstr;
        QRegExp regex;
        QString regexstr;
        
        
        //FIRST REGEX: dms
        regexstr = "^-?\\+?[01]?[0-9]{1,2}[^0-9]\\s*[0-5]?[0-9][^0-9]\\s*[0-5]?[0-9][^0-9]*\\s*[";
        QString cardinalstr;
        if(coord == 0 ) {
            cardinalstr.append( m_cardinals[0] );
            cardinalstr.append( m_cardinals[0].toLower() );
            cardinalstr.append( m_cardinals[2] );
            cardinalstr.append( m_cardinals[2].toLower() );
        } else {
            cardinalstr.append( m_cardinals[1] );
            cardinalstr.append( m_cardinals[1].toLower() );
            cardinalstr.append( m_cardinals[3] );
            cardinalstr.append( m_cardinals[3].toLower() );
        }
        regexstr.append( cardinalstr );
        regexstr.append( "]*.*$" );
        qDebug() << "Lat/ Lon: trying regex" << regexstr;
        //eg   ^-?\\+?[01]?[0-9]{1,2}[^0-9]\\s*[0-5]?[0-9][^0-9]\\s*[0-5]?[0-9][^0-9]*\\s*[EeWw]*.*$
        //     125 23 87 E
        //     23 2 14 w
        //     126*12'24"W
        //     126* 12' 24" W
        //     -14 23 18
        //     +23 12 49
        //     12 27 34
        //     etc
        regex = QRegExp( regexstr );
        if( workstr.contains(regex) ) {
            QRegExp spacer( "[^0-9]\\s*" );
            QStringList slist = workstr.split( spacer );
//             qDebug() << "Match found, broken into list: " << slist;
            double degrees = slist.at(0).toDouble();
            double minutes = slist.at(1).toDouble();
            double seconds = slist.at(2).toDouble();
            double value = 0;
            
            qDebug() << "Found DMS values " << degrees << minutes << seconds;
            
            if( workstr.contains( QRegExp( "[" + cardinalstr + "]" ) ) ) {
                if( slist.at(3).contains( QRegExp( "^[" + cardinalstr.right(2) + "]" ) ) ) {
                    // south or west so the value is negative
                    value = degrees + ( minutes / 60 ) + ( seconds / 3600 );
                    value *= -1;
                } else {
                    // north or east so the value is positive
                    value = degrees + ( minutes / 60 ) + ( seconds / 3600 );
                }
            } else {
                if( degrees < 0 ) {
                    value = degrees - ( minutes / 60 ) - ( seconds / 3600 );
                } else {
                    value = degrees + ( minutes / 60 ) + ( seconds / 3600 );
                }
            }
//             qDebug() << "Found value" << value;
            coordinates[coord] = value;
            continue;
        }
        
        //Add other regexes here
    }
    
    ////////////////////////////
    // ACT ON COORDS / / / / / /
    ////////////////////////////
    int score;
    GeoDataPlacemark *placemark = new GeoDataPlacemark();
    placemark->setName( input );
    
    if( coordinates[0] != 9999 && coordinates[1] != 9999 ) {
        qDebug() << "Matched, making placemark @ lat " << coordinates[0] << "lon" << coordinates[1];
        placemark->setCoordinate( coordinates[1] * DEG2RAD, coordinates[0] * DEG2RAD );
        placemark->setVisible( true );
        score = 100; // matches regex
    } else {
        qDebug() << "Failed to match";
        placemark->setCoordinate( 0, 0 );
        score = 0; //does not match
    }
    
    PlaceMarkContainer container;
    container.setName( "Coordinate search result" );
    container.append( placemark );
    
    MarbleRunnerResult result( container, static_cast<MarbleRunnerResult::Score>(score) );
    
    emit runnerFinished( result );    
}

}
