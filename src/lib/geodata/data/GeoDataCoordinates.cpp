//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007-2008 Inge Wallin  <ingwa@kde.org>
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>
//


#include "GeoDataCoordinates.h"
#include "GeoDataCoordinates_p.h"

#include <cmath>

#include <QtCore/QRegExp>
#include <QtCore/QDebug>
#include <QtCore/QLocale>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QCoreApplication>
#include "global.h"

namespace Marble
{

GeoDataCoordinates::Notation GeoDataCoordinates::s_notation = GeoDataCoordinates::DMS;

GeoDataCoordinates::GeoDataCoordinates( qreal _lon, qreal _lat, qreal _alt, GeoDataCoordinates::Unit unit, int _detail )
  : d( new GeoDataCoordinatesPrivate() )
{
    d->m_altitude = _alt;
    d->m_detail = _detail;
    switch( unit ){
    case Radian:
        d->m_q = Quaternion( _lon, _lat );
        d->m_lon = _lon;
        d->m_lat = _lat;
        break;
    case Degree:
        d->m_q = Quaternion( _lon * DEG2RAD , _lat * DEG2RAD  );
        d->m_lon = _lon * DEG2RAD;
        d->m_lat = _lat * DEG2RAD;
        break;
    }
}

GeoDataCoordinates::GeoDataCoordinates( const GeoDataCoordinates& other )
  : d( new GeoDataCoordinatesPrivate( *other.d ) )
{
}

GeoDataCoordinates::GeoDataCoordinates()
  : d( new GeoDataCoordinatesPrivate() )
{
}

GeoDataCoordinates::~GeoDataCoordinates()
{
    delete d;
#ifdef DEBUG_GEODATA
//    qDebug() << "delete coordinates";
#endif
}

void GeoDataCoordinates::set( qreal _lon, qreal _lat, qreal _alt, GeoDataCoordinates::Unit unit )
{
    d->m_altitude = _alt;
    switch( unit ){
    case Radian:
        d->m_q = Quaternion( _lon, _lat );
        d->m_lon = _lon;
        d->m_lat = _lat;
        break;
    case Degree:
        d->m_q = Quaternion( _lon * DEG2RAD , _lat * DEG2RAD  );
        d->m_lon = _lon * DEG2RAD;
        d->m_lat = _lat * DEG2RAD;
        break;
    }
}

void GeoDataCoordinates::geoCoordinates( qreal& lon, qreal& lat, 
                               GeoDataCoordinates::Unit unit ) const
{
    switch ( unit ) 
    {
    case Radian:
            lon = d->m_lon;
            lat = d->m_lat;
        break;
    case Degree:
            lon = d->m_lon * RAD2DEG;
            lat = d->m_lat * RAD2DEG;
        break;
    }
}

GeoDataCoordinates::Notation GeoDataCoordinates::defaultNotation()
{
    return s_notation;
}

void GeoDataCoordinates::setDefaultNotation( GeoDataCoordinates::Notation notation )
{
    s_notation = notation;
}

qreal GeoDataCoordinates::normalizeLon( qreal lon )
{
    if ( lon > +M_PI ) {
        int cycles = (int)( ( lon + M_PI ) / ( 2 * M_PI ) );
        return lon - ( cycles * 2 * M_PI );
    } 
    if ( lon < -M_PI ) {
        int cycles = (int)( ( lon - M_PI ) / ( 2 * M_PI ) );
        return lon - ( cycles * 2 * M_PI );
    }

    return lon;
}

qreal GeoDataCoordinates::normalizeLat( qreal lat )
{
    if ( lat > ( +M_PI / 2.0 ) ) {
        int cycles = (int)( ( lat + M_PI ) / ( 2 * M_PI ) );
        qreal temp;
        if( cycles == 0 ) { // pi/2 < lat < pi
            temp = M_PI - lat;
        } else {
            temp = lat - ( cycles * 2 * M_PI );
        }
        if ( temp > ( +M_PI / 2.0 ) ) {
            return ( +M_PI - temp );
        }
        if ( temp < ( -M_PI / 2.0 ) ) {
            return ( -M_PI - temp );
        }
        return temp;
    } 
    if ( lat < ( -M_PI / 2.0 ) ) {
        int cycles = (int)( ( lat - M_PI ) / ( 2 * M_PI ) );
        qreal temp;
        if( cycles == 0 ) { 
            temp = -M_PI - lat;
        } else {
            temp = lat - ( cycles * 2 * M_PI );
        }
        if ( temp > ( +M_PI / 2.0 ) ) {
            return ( +M_PI - temp );
        }
        if ( temp < ( -M_PI / 2.0 ) ) {
            return ( -M_PI - temp );
        }
        return temp;
    }
    return lat;
}

void GeoDataCoordinates::normalizeLonLat( qreal &lon, qreal &lat )
{
    if ( lon > +M_PI ) {
        int cycles = (int)( ( lon + M_PI ) / ( 2 * M_PI ) );
        lon = lon - ( cycles * 2 * M_PI );
    } 
    if ( lon < -M_PI ) {
        int cycles = (int)( ( lon - M_PI ) / ( 2 * M_PI ) );
        lon = lon - ( cycles * 2 * M_PI );
    }

    if ( lat > ( +M_PI / 2.0 ) ) {
        int cycles = (int)( ( lat + M_PI ) / ( 2 * M_PI ) );
        qreal temp;
        if( cycles == 0 ) { // pi/2 < lat < pi
            temp = M_PI - lat;
        } else {
            temp = lat - ( cycles * 2 * M_PI );
        }
        if ( temp > ( +M_PI / 2.0 ) ) {
            lat =  +M_PI - temp;
        }
        if ( temp < ( -M_PI / 2.0 ) ) {
            lat =  -M_PI - temp;
        }
        lat = temp;
        if( lon > 0 ) { 
            lon = -M_PI + lon;
        } else {
            lon = M_PI + lon;
        }
    } 
    if ( lat < ( -M_PI / 2.0 ) ) {
        int cycles = (int)( ( lat - M_PI ) / ( 2 * M_PI ) );
        qreal temp;
        if( cycles == 0 ) { 
            temp = -M_PI - lat;
        } else {
            temp = lat - ( cycles * 2 * M_PI );
        }
        if ( temp > ( +M_PI / 2.0 ) ) {
            lat =  +M_PI - temp;
        }
        if ( temp < ( -M_PI / 2.0 ) ) {
            lat =  -M_PI - temp;
        }
        lat = temp;
        if( lon > 0 ) { 
            lon = -M_PI + lon;
        } else {
            lon = M_PI + lon;
        }
    } 
    return;
}

GeoDataCoordinates GeoDataCoordinates::fromString( const QString& string, bool& successful )
{
    successful = false; //assume failure
    
    QString input = string.toLower();
    input = input.trimmed(); //remove front spaces
    qDebug() << "Creating GeoDataCoordinates from string " << input;
    
    qreal lat, lon;
    
    // c is for cardinal directions and is short which saves space in regexps
    QStringList c; 
    c << QCoreApplication::translate( "GeoDataCoordinates", //can't use QObject::tr()
                        "North", "The compass direction" ).toLower();
    c << QCoreApplication::translate( "GeoDataCoordinates", 
                        "East",  "The compass direction" ).toLower();
    c << QCoreApplication::translate( "GeoDataCoordinates", 
                        "South", "The compass direction" ).toLower();
    c << QCoreApplication::translate( "GeoDataCoordinates", 
                        "West",  "The compass direction" ).toLower();
    QRegExp regex; //the main regex we use
    QString regexstr; // temp. string for constructing the regex
    QString dec( QLocale::system().decimalPoint() ); //for regex construction
    if( dec.contains(".") )
        dec.prepend("\\");
    
    //BEGIN REGEX1
    // #1: Just two numbers, no directions, eg 74.2245 -32.2434 etc
    //firstletters is used for *negative* matching to ensure that this is not case #2
    QString firstletters;
    for(int i = 0; i < 4; i++) { firstletters.append( c[i].at(0) ); }
    // <frac> = fractional part; <sp> = whitespace
    //            <first coord ><decimal  ><frac><sp/symbol but NOT a direction>
    regexstr = "^(-?\\+?\\d{1,3}" + dec + "?\\d*)(?:\\s|[^\\d" + firstletters + dec + "]|"
    //           ^----------cap(1)--------------^
    //         <sp ><2nd coord     ><decimal ><frac>
             + "\\s)+(-?\\+?\\d{1,3}" + dec + "?\\d*)\\s*$";
    //               ^----------cap(2)--------------^
    regex = QRegExp( regexstr );
    if( input.contains( regex ) ) {
        qDebug() << "REGEX: " << regexstr << "matches" << regex.cap(0);
        lat = regex.cap(1).toDouble();
        lon = regex.cap(2).toDouble();
        
        qDebug() << "Created lat / lon " << lat << lon;
        
        GeoDataCoordinates coords( lon, lat, 0, GeoDataCoordinates::Degree );
        successful = true;
        return coords;
    }
    //END REGEX1


    //BEGIN STUFF
    //we put this after regex #1 since it is not used in that case so it will only make things slower.
                      
    //this is used to make regexps which determine which coord element comes first.
    //if it doesn't match it is assumed that the longitude comes first.
    //Note that it is "QString( c[0].at(0) )" because QChar doesn't work
    //for adding things in a big long chain like this. TODO: investigate.
    //becomes (?:(?:north|south)|[ns],?\\s).*(?:(?:east|west)|[ew]) in en
    const QString latfirst = "(?:" + c[0] + '|' + c[2] + "|[" 
                           + QString( c[0].at(0) ) + QString( c[2].at(0) ) + "],?\\s).*"
                           + "(?:" + c[1] + '|' + c[3] + "|[" 
                           + QString( c[1].at(0) ) + QString( c[3].at(0) ) + "])";
                           
    //this is a snippet which matches (AND CAPTURES!!) a direction.
    //becomes  (north|east|south|west|[nsew]) in en.
    //               <north   >   <east    >   <south   >   <west    >
    QString dir = "(" + c[0] + '|' + c[1] + '|' + c[2] + '|' + c[3] + "|["
                      + QString( c[0].at(0) ) + QString( c[2].at(0) ) // + <n> + <s>
                      + QString( c[1].at(0) ) + QString( c[3].at(0) ) + "])"; // + <e> + <w>
                      
                      
    //END STUFF
    
    //BEGIN REGEX2
    // #2: Two numbers with directions, eg 74.2245 N 32.2434 W etc
    regexstr = "^([\\-\\+]?\\d{1,3}" + dec + "?\\d*)(?:\\s|[^\\d" +dec+ "])+"
             + dir + ",?\\s*(-?\\+?\\d{1,3}" +  dec + "?\\d*)"
             + "(?:\\s|[^\\d" +dec+ "])+" + dir + "\\s*$";
    regex = QRegExp( regexstr );
    if( input.contains( regex ) ) {
        qDebug() << "REGEX: " << regexstr << "matches" << regex.cap(0);
        QString latdeg, londeg, latdir, londir;
        
        if( input.contains( QRegExp( latfirst ) ) ) {
            qDebug() << "latitude comes first";
            latdeg = regex.cap( 1 );
            latdir = regex.cap( 2 );
            londeg = regex.cap( 3 );
            londir = regex.cap( 4 );
        } else { 
            qDebug() << "longitude comes first";
            londeg = regex.cap( 1 );
            londir = regex.cap( 2 );
            latdeg = regex.cap( 3 );
            latdir = regex.cap( 4 );
        }
        
        if( latdir == c[2] ) //south
            lat = latdeg.toDouble() * -1.0;
        else //north
            lat = latdeg.toDouble();
            
        if( londir == c[3] ) //west
            lon = londeg.toDouble() * -1.0;
        else // east
            lon = londeg.toDouble();
        
        qDebug() << "Created lat / lon " << lat << lon;
        
        GeoDataCoordinates coords( lon, lat, 0, GeoDataCoordinates::Degree );
        successful = true;
        return coords;
    }
    //END REGEX2
    
    //BEGIN REGEX3
    // #3: Sexagesimal
    regexstr = "^(\\d{1,3})(?:\\s|[^\\d" +dec+ "])+(\\d{1,2})(?:\\s|[^\\d" +dec+ "])+";
    regexstr += "(\\d{1,2})(?:\\s|[^\\d" +dec+ "])+" +   dir  + ",?\\s*";
    regexstr += "(\\d{1,3})(?:\\s|[^\\d" +dec+ "])+(\\d{1,2})(?:\\s|[^\\d" +dec+ "])+";
    regexstr += "(\\d{1,2})(?:\\s|[^\\d" +dec+ "])+"  +  dir + "\\s*$";
    regex = QRegExp( regexstr );
    if( input.contains( regex ) ) {
        qDebug() << "REGEX: " << regexstr << "matches" << regex.cap(0);
        QString latdeg, londeg, latmin, lonmin, latsec, lonsec, latdir, londir;
        
        if( input.contains( QRegExp( latfirst ) ) ) {
            qDebug() << "latitude comes first";
            latdeg = regex.cap( 1 );
            latmin = regex.cap( 2 );
            latsec = regex.cap( 3 );
            latdir = regex.cap( 4 );
            londeg = regex.cap( 5 );
            lonmin = regex.cap( 6 );
            lonsec = regex.cap( 7 );
            londir = regex.cap( 8 );
        } else { 
            qDebug() << "longitude comes first";
            londeg = regex.cap( 1 );
            lonmin = regex.cap( 2 );
            lonsec = regex.cap( 3 );
            londir = regex.cap( 4 );
            latdeg = regex.cap( 5 );
            latmin = regex.cap( 6 );
            latsec = regex.cap( 7 );
            latdir = regex.cap( 8 );
        }
        lat = latdeg.toDouble() + (latmin.toDouble()/60.0) + (latsec.toDouble()/3600.0);
        lon = londeg.toDouble() + (lonmin.toDouble()/60.0) + (lonsec.toDouble()/3600.0);
        
        if( latdir == c[2] ) //south, so we invert
            lat *= -1.0;
            
        if( londir == c[3] ) //west, so we invert
            lon *= -1.0;

        qDebug() << "Created lat / lon " << lat << lon;
        
        GeoDataCoordinates coords( lon, lat, 0, GeoDataCoordinates::Degree );
        successful = true;
        return coords;
    }
    //END REGEX3
    
    //BEGIN REGEX4
    // #4: Sexagesimal with minute precision
    regexstr = "^(\\d{1,3})(?:\\s|[^\\d" +dec+ "])+(\\d{1,2})(?:\\s|[^\\d" +dec+ "])+"
             +   dir  + ",?\\s*"
             +  "(\\d{1,3})(?:\\s|[^\\d" +dec+ "])+(\\d{1,2})(?:\\s|[^\\d" +dec+ "])+"
             +   dir + "\\s*$";
    regex = QRegExp( regexstr );
    if( input.contains( regex ) ) {
        qDebug() << "REGEX: " << regexstr << "matches" << regex.cap(0);
        QString latdeg, londeg, latmin, lonmin, latdir, londir;
        
        if( input.contains( QRegExp( latfirst ) ) ) {
            qDebug() << "latitude comes first";
            latdeg = regex.cap( 1 );
            latmin = regex.cap( 2 );
            latdir = regex.cap( 3 );
            londeg = regex.cap( 4 );
            lonmin = regex.cap( 5 );
            londir = regex.cap( 6 );
        } else { 
            qDebug() << "longitude comes first";
            londeg = regex.cap( 1 );
            lonmin = regex.cap( 2 );
            londir = regex.cap( 3 );
            latdeg = regex.cap( 4 );
            latmin = regex.cap( 5 );
            latdir = regex.cap( 6 );
        }
        lat = latdeg.toDouble() + (latmin.toDouble()/60.0);
        lon = londeg.toDouble() + (lonmin.toDouble()/60.0);

        if( latdir == c[2] ) //south, so we invert
            lat *= -1.0;

        if( londir == c[3] ) //west, so we invert
            lon *= -1.0;

        qDebug() << "Created lat / lon " << lat << lon;
        
        GeoDataCoordinates coords( lon, lat, 0, GeoDataCoordinates::Degree );
        successful = true;
        return coords;
    }
    //END REGEX4
    
    return GeoDataCoordinates();
}


QString GeoDataCoordinates::toString()
{
    return GeoDataCoordinates::toString( s_notation );
}

QString GeoDataCoordinates::toString( GeoDataCoordinates::Notation notation )
{
    QString nsstring = ( d->m_lat > 0 ) ? QCoreApplication::tr("N") : QCoreApplication::tr("S");  
    QString westring = ( d->m_lon < 0 ) ? QCoreApplication::tr("W") : QCoreApplication::tr("E");  

    qreal lat, lon;
    lon = fabs( d->m_lon * RAD2DEG );
    lat = fabs( d->m_lat * RAD2DEG );

    if ( notation == GeoDataCoordinates::DMS )
    {
        int londeg = (int) lon;
        int lonmin = (int) ( 60 * (lon - londeg) );
        int lonsec = (int) ( 3600 * (lon - londeg - ((qreal)(lonmin) / 60) ) );


        int latdeg = (int) lat;
        int latmin = (int) ( 60 * (lat - latdeg) );
        int latsec = (int) ( 3600 * (lat - latdeg - ((qreal)(latmin) / 60) ) );

        return QString("%1\xb0 %2\' %3\"%4, %5\xb0 %6\' %7\"%8")
        .arg(londeg, 3, 10, QChar(' ') ).arg(lonmin, 2, 10, QChar('0') )
        .arg(lonsec, 2, 10, QChar('0') ).arg(westring)
        .arg(latdeg, 3, 10, QChar(' ') ).arg(latmin, 2, 10, QChar('0') )
        .arg(latsec, 2, 10, QChar('0') ).arg(nsstring);
    }
    else // notation = GeoDataCoordinates::Decimal
    {
        return QString("%L1\xb0%2, %L3\xb0%4")
        .arg(lon, 6, 'f', 3, QChar(' ') ).arg(westring)
        .arg(lat, 6, 'f', 3, QChar(' ') ).arg(nsstring);
    }
}

bool GeoDataCoordinates::operator==( const GeoDataCoordinates &test ) const
{
    // Comparing 2 ints is faster than comparing 4 ints
    // Therefore we compare the Lon-Lat coordinates instead 
    // of the Position-Quaternion.

    qreal lonTest;
    qreal latTest;
    qreal lonThis;
    qreal latThis;
    
    geoCoordinates( lonThis, latThis );
    test.geoCoordinates( lonTest, latTest );
    
    return ( lonThis == lonTest 
             && latTest == latThis );
}

void GeoDataCoordinates::setAltitude( const qreal altitude )
{
    d->m_altitude = altitude;
}

qreal GeoDataCoordinates::altitude() const
{
    return d->m_altitude;
}

int GeoDataCoordinates::detail() const
{
    return d->m_detail;
}

void GeoDataCoordinates::setDetail( const int det )
{
    d->m_detail = det;
}

const Quaternion& GeoDataCoordinates::quaternion() const
{
    return d->m_q;
}

GeoDataCoordinates& GeoDataCoordinates::operator=( const GeoDataCoordinates &other )
{
    *d = *other.d;
    return *this;
}

void GeoDataCoordinates::pack( QDataStream& stream ) const
{
    stream << d->m_lon;
    stream << d->m_lat;
    stream << d->m_altitude;
}

void GeoDataCoordinates::unpack( QDataStream& stream )
{
    stream >> d->m_lon;
    stream >> d->m_lat;
    stream >> d->m_altitude;

    d->m_q.set( d->m_lon, d->m_lat );
}

}
