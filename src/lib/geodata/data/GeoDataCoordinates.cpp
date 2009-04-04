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
#if QT_VERSION < 0x040400
# include <qatomic.h>
#else
# include <QtCore/QAtomicInt>
#endif

#include "global.h"

#include "Quaternion.h"

namespace Marble
{
#if QT_VERSION < 0x040400
    typedef QAtomic QAtomicInt;
#endif

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

/* simply copy the d pointer
* it will be replaced in the detach function instead
*/
GeoDataCoordinates::GeoDataCoordinates( const GeoDataCoordinates& other )
  : d( other.d )
{
    d->ref.ref();
}

/*
 * standard ctor;
 * create a new private pointer which initializes the atomic reference counter
 */
GeoDataCoordinates::GeoDataCoordinates()
  : d( new GeoDataCoordinatesPrivate() )
{
}

/*
 * only delete the private d pointer if the number of references is 0
 * remember that all copies share the same d pointer!
 */
GeoDataCoordinates::~GeoDataCoordinates()
{
    if (!d->ref.deref())
        delete d;
#ifdef DEBUG_GEODATA
//    qDebug() << "delete coordinates";
#endif
}

/*
 * if only one copy exists, return
 * else make a new private d pointer object and assign the values of the current
 * one to it
 * at the end, if the number of references thus reaches 0 delete it
 * this state shouldn't happen, but if it does, we have to clean up behind us.
 */
void GeoDataCoordinates::detach()
{
    if(d->ref == 1)
        return;

    GeoDataCoordinatesPrivate *new_d = new GeoDataCoordinatesPrivate( *d );

    if (!d->ref.deref())
        delete d;

    d = new_d;
}

/*
 * call detach() at the start of all non-static, non-const functions
 */
void GeoDataCoordinates::set( qreal _lon, qreal _lat, qreal _alt, GeoDataCoordinates::Unit unit )
{
    detach();
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

/*
 * call detach() at the start of all non-static, non-const functions
 */
void GeoDataCoordinates::setLongitude( qreal _lon, GeoDataCoordinates::Unit unit )
{
    detach();
    switch( unit ){
    case Radian:
        d->m_q = Quaternion( _lon, d->m_lat );
        d->m_lon = _lon;
        break;
    case Degree:
        d->m_q = Quaternion( _lon * DEG2RAD , d->m_lat  );
        d->m_lon = _lon * DEG2RAD;
        break;
    }
}

/*
 * call detach() at the start of all non-static, non-const functions
 */
void GeoDataCoordinates::setLatitude( qreal _lat, GeoDataCoordinates::Unit unit )
{
    detach();
    switch( unit ){
    case Radian:
        d->m_q = Quaternion( d->m_lon, _lat );
        d->m_lat = _lat;
        break;
    case Degree:
        d->m_q = Quaternion( d->m_lon, _lat * DEG2RAD   );
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

qreal GeoDataCoordinates::longitude( GeoDataCoordinates::Unit unit ) const
{
    switch ( unit ) 
    {
    case Radian:
            return d->m_lon;
        break;
    case Degree:
            return d->m_lon * RAD2DEG;
        break;
    }
}

qreal GeoDataCoordinates::latitude( GeoDataCoordinates::Unit unit ) const
{
    switch ( unit ) 
    {
    case Radian:
            return d->m_lat;
        break;
    case Degree:
            return d->m_lat * RAD2DEG;
        break;
    }
}

//static
GeoDataCoordinates::Notation GeoDataCoordinates::defaultNotation()
{
    return s_notation;
}

//static
void GeoDataCoordinates::setDefaultNotation( GeoDataCoordinates::Notation notation )
{
    s_notation = notation;
}

//static
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

//static
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

//static
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

//static
GeoDataCoordinates GeoDataCoordinates::fromString( const QString& string, bool& successful )
{
    QString input = string.toLower().trimmed();
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
    for(int i = 0; i < 4; ++i) { firstletters.append( c[i].at(0) ); }
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
    QString dir = '(' + c[0] + '|' + c[1] + '|' + c[2] + '|' + c[3] + "|["
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

        //convert the string to a number
        lat = latdeg.toDouble();
        lon = londeg.toDouble();

        //if south make the value negative
        if( latdir == c[2].at(0) || latdir == c[2] ) 
            lat *= -1.0;
        //if west make the value negative
        if( londir == c[3].at(0) || londir == c[3] ) 
            lon *= -1.0;
        
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

        //Add the fractional parts to make the total
        lat = latdeg.toDouble() + (latmin.toDouble()/60.0) + (latsec.toDouble()/3600.0);
        lon = londeg.toDouble() + (lonmin.toDouble()/60.0) + (lonsec.toDouble()/3600.0);

        //if the direction is south, make the value negative
        if( latdir == c[2].at(0) || latdir == c[2] )
            lat *= -1.0;
        //if the direction is west, make the value negative
        if( londir == c[3].at(0) || londir == c[3] )
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

        if( latdir == c[2].at(0) || latdir == c[2] ) //south, so we invert
            lat *= -1.0;

        if( londir == c[3].at(0) || londir == c[3] ) //west, so we invert
            lon *= -1.0;

        qDebug() << "Created lat / lon " << lat << lon;
        
        GeoDataCoordinates coords( lon, lat, 0, GeoDataCoordinates::Degree );
        successful = true;
        return coords;
    }
    //END REGEX4

    //we didn't match so leave "successful" as FALSE and return empty coords
    successful = false;
    return GeoDataCoordinates();
}


QString GeoDataCoordinates::toString() const
{
    return GeoDataCoordinates::toString( s_notation );
}

QString GeoDataCoordinates::toString( GeoDataCoordinates::Notation notation ) const
{
    QString nsstring = ( d->m_lat > 0 ) ? QCoreApplication::tr("N") : QCoreApplication::tr("S");  
    QString westring = ( d->m_lon < 0 ) ? QCoreApplication::tr("W") : QCoreApplication::tr("E");  

    qreal lat, lon;
    lon = fabs( (qreal)d->m_lon * RAD2DEG );
    lat = fabs( (qreal)d->m_lat * RAD2DEG );

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
    detach();
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
    detach();
    d->m_detail = det;
}

const Quaternion& GeoDataCoordinates::quaternion() const
{
    return d->m_q;
}

bool GeoDataCoordinates::isAtPole( Marble::Pole pole ) const
{
    // Evaluate the most likely case first:
    // The case where we haven't hit the pole and where our latitude is normalized
    // to the range of 90 deg S ... 90 deg N
    if ( fabs( (qreal) 2.0 * d->m_lat ) < M_PI ) {
        return false;
    }
    else {
        if ( fabs( (qreal) 2.0 * d->m_lat ) == M_PI ) {
            // Ok, we have hit a pole. Now let's check whether it's the one we've asked for:
            if ( pole == Marble::AnyPole ){
                return true;
            }
            else {
                if ( pole == Marble::NorthPole && 2.0 * d->m_lat == +M_PI ) {
                    return true;
                }
                if ( pole == Marble::SouthPole && 2.0 * d->m_lat == -M_PI ) {
                    return true;
                }
                return false;
            }
        }
        // 
        else {
            // FIXME: Should we just normalize latitude and longitude and be done?
            //        While this might work well for persistent data it would create some 
            //        possible overhead for temporary data, so this needs careful thinking.
            qDebug() << "GeoDataCoordinates not normalized!";

            // Only as a last resort we cover the unlikely case where
            // the latitude is not normalized to the range of 
            // 90 deg S ... 90 deg N
            if ( fabs( (qreal) 2.0 * normalizeLat( d->m_lat ) ) < M_PI  ) {
                return false;
            }
            else {
                // Ok, we have hit a pole. Now let's check whether it's the one we've asked for:
                if ( pole == Marble::AnyPole ){
                    return true;
                }
                else {
                    if ( pole == Marble::NorthPole && 2.0 * d->m_lat == +M_PI ) {
                        return true;
                    }
                    if ( pole == Marble::SouthPole && 2.0 * d->m_lat == -M_PI ) {
                        return true;
                    }
                    return false;
                }
            }            
        }
    }
}

GeoDataCoordinates& GeoDataCoordinates::operator=( const GeoDataCoordinates &other )
{
    qAtomicAssign(d, other.d);
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
    // call detach even though it shouldn't be needed - one never knows
    detach();
    stream >> d->m_lon;
    stream >> d->m_lat;
    stream >> d->m_altitude;

    d->m_q.set( d->m_lon, d->m_lat );
}

}
