//
// This file is part of the Marble Virtual Globe.
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
#include <QtCore/QLocale>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QCoreApplication>
#include <QtCore/QAtomicInt>

#include "global.h"
#include "MarbleDebug.h"

#include "Quaternion.h"

namespace Marble
{

GeoDataCoordinates::Notation GeoDataCoordinates::s_notation = GeoDataCoordinates::DMS;

GeoDataCoordinates::GeoDataCoordinates( qreal _lon, qreal _lat, qreal _alt, GeoDataCoordinates::Unit unit, int _detail )
  : d( new GeoDataCoordinatesPrivate( _lon, _lat, _alt, unit, _detail ) )
{
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
//    mDebug() << "delete coordinates";
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
    default:
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
    default:
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
    default:
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
    default:
    case Radian:
        return d->m_lon;
    case Degree:
        return d->m_lon * RAD2DEG;
    }
}

qreal GeoDataCoordinates::latitude( GeoDataCoordinates::Unit unit ) const
{
    switch ( unit )
    {
    default:
    case Radian:
        return d->m_lat;
    case Degree:
        return d->m_lat * RAD2DEG;
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
qreal GeoDataCoordinates::normalizeLon( qreal lon, GeoDataCoordinates::Unit unit )
{
    qreal halfCircle;
    if ( unit == GeoDataCoordinates::Radian ) {
        halfCircle = M_PI;
    }
    else {
        halfCircle = 180;
    }

    if ( lon > halfCircle ) {
        int cycles = (int)( ( lon + halfCircle ) / ( 2 * halfCircle ) );
        return lon - ( cycles * 2 * halfCircle );
    } 
    if ( lon < -halfCircle ) {
        int cycles = (int)( ( lon - halfCircle ) / ( 2 * halfCircle ) );
        return lon - ( cycles * 2 * halfCircle );
    }

    return lon;
}

//static
qreal GeoDataCoordinates::normalizeLat( qreal lat, GeoDataCoordinates::Unit unit )
{
    qreal halfCircle;
    if ( unit == GeoDataCoordinates::Radian ) {
        halfCircle = M_PI;
    }
    else {
        halfCircle = 180;
    }

    if ( lat > ( halfCircle / 2.0 ) ) {
        int cycles = (int)( ( lat + halfCircle ) / ( 2 * halfCircle ) );
        qreal temp;
        if( cycles == 0 ) { // pi/2 < lat < pi
            temp = halfCircle - lat;
        } else {
            temp = lat - ( cycles * 2 * halfCircle );
        }
        if ( temp > ( halfCircle / 2.0 ) ) {
            return ( halfCircle - temp );
        }
        if ( temp < ( -halfCircle / 2.0 ) ) {
            return ( -halfCircle - temp );
        }
        return temp;
    } 
    if ( lat < ( -halfCircle / 2.0 ) ) {
        int cycles = (int)( ( lat - halfCircle ) / ( 2 * halfCircle ) );
        qreal temp;
        if( cycles == 0 ) { 
            temp = -halfCircle - lat;
        } else {
            temp = lat - ( cycles * 2 * halfCircle );
        }
        if ( temp > ( +halfCircle / 2.0 ) ) {
            return ( +halfCircle - temp );
        }
        if ( temp < ( -halfCircle / 2.0 ) ) {
            return ( -halfCircle - temp );
        }
        return temp;
    }
    return lat;
}

//static
void GeoDataCoordinates::normalizeLonLat( qreal &lon, qreal &lat, GeoDataCoordinates::Unit unit )
{
    qreal halfCircle;
    if ( unit == GeoDataCoordinates::Radian ) {
        halfCircle = M_PI;
    }
    else {
        halfCircle = 180;
    }

    if ( lon > +halfCircle ) {
        int cycles = (int)( ( lon + halfCircle ) / ( 2 * halfCircle ) );
        lon = lon - ( cycles * 2 * halfCircle );
    } 
    if ( lon < -halfCircle ) {
        int cycles = (int)( ( lon - halfCircle ) / ( 2 * halfCircle ) );
        lon = lon - ( cycles * 2 * halfCircle );
    }

    if ( lat > ( +halfCircle / 2.0 ) ) {
        int cycles = (int)( ( lat + halfCircle ) / ( 2 * halfCircle ) );
        qreal temp;
        if( cycles == 0 ) { // pi/2 < lat < pi
            temp = halfCircle - lat;
        } else {
            temp = lat - ( cycles * 2 * halfCircle );
        }
        if ( temp > ( +halfCircle / 2.0 ) ) {
            lat =  +halfCircle - temp;
        }
        if ( temp < ( -halfCircle / 2.0 ) ) {
            lat =  -halfCircle - temp;
        }
        lat = temp;
        if( lon > 0 ) { 
            lon = -halfCircle + lon;
        } else {
            lon = halfCircle + lon;
        }
    } 
    if ( lat < ( -halfCircle / 2.0 ) ) {
        int cycles = (int)( ( lat - halfCircle ) / ( 2 * halfCircle ) );
        qreal temp;
        if( cycles == 0 ) { 
            temp = -halfCircle - lat;
        } else {
            temp = lat - ( cycles * 2 * halfCircle );
        }
        if ( temp > ( +halfCircle / 2.0 ) ) {
            lat =  +halfCircle - temp;
        }
        if ( temp < ( -halfCircle / 2.0 ) ) {
            lat =  -halfCircle - temp;
        }
        lat = temp;
        if( lon > 0 ) { 
            lon = -halfCircle + lon;
        } else {
            lon = halfCircle + lon;
        }
    } 
    return;
}

//static
GeoDataCoordinates GeoDataCoordinates::fromString( const QString& string, bool& successful )
{
    QString input = string.toLower().trimmed();
    mDebug() << "Creating GeoDataCoordinates from string " << input;
    
    qreal lat, lon;
    
    // c is for cardinal directions and is short which saves space in regexps
    QStringList c; 
    c << tr( "North", "The compass direction" ).toLower();
    c << tr( "East",  "The compass direction" ).toLower();
    c << tr( "South", "The compass direction" ).toLower();
    c << tr( "West",  "The compass direction" ).toLower();
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
        mDebug() << "REGEX: " << regexstr << "matches" << regex.cap(0);
        lat = regex.cap(1).toDouble();
        lon = regex.cap(2).toDouble();
        
        mDebug() << "Created lat / lon " << lat << lon;
        
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
        mDebug() << "REGEX: " << regexstr << "matches" << regex.cap(0);
        QString latdeg, londeg, latdir, londir;
        
        if( input.contains( QRegExp( latfirst ) ) ) {
            mDebug() << "latitude comes first";
            latdeg = regex.cap( 1 );
            latdir = regex.cap( 2 );
            londeg = regex.cap( 3 );
            londir = regex.cap( 4 );
        } else { 
            mDebug() << "longitude comes first";
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
        
        mDebug() << "Created lat / lon " << lat << lon;
        
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
        mDebug() << "REGEX: " << regexstr << "matches" << regex.cap(0);
        QString latdeg, londeg, latmin, lonmin, latsec, lonsec, latdir, londir;
        
        if( input.contains( QRegExp( latfirst ) ) ) {
            mDebug() << "latitude comes first";
            latdeg = regex.cap( 1 );
            latmin = regex.cap( 2 );
            latsec = regex.cap( 3 );
            latdir = regex.cap( 4 );
            londeg = regex.cap( 5 );
            lonmin = regex.cap( 6 );
            lonsec = regex.cap( 7 );
            londir = regex.cap( 8 );
        } else { 
            mDebug() << "longitude comes first";
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

        mDebug() << "Created lat / lon " << lat << lon;
        
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
        mDebug() << "REGEX: " << regexstr << "matches" << regex.cap(0);
        QString latdeg, londeg, latmin, lonmin, latdir, londir;
        
        if( input.contains( QRegExp( latfirst ) ) ) {
            mDebug() << "latitude comes first";
            latdeg = regex.cap( 1 );
            latmin = regex.cap( 2 );
            latdir = regex.cap( 3 );
            londeg = regex.cap( 4 );
            lonmin = regex.cap( 5 );
            londir = regex.cap( 6 );
        } else { 
            mDebug() << "longitude comes first";
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

        mDebug() << "Created lat / lon " << lat << lon;
        
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

QString GeoDataCoordinates::toString( GeoDataCoordinates::Notation notation, int precision ) const
{
        return  lonToString( d->m_lon, notation, Radian, precision )
                + QString(", ")
                + latToString( d->m_lat, notation, Radian, precision );
}

QString GeoDataCoordinates::lonToString( qreal lon, GeoDataCoordinates::Notation notation,  
                                                    GeoDataCoordinates::Unit unit, 
                                                    int precision,
                                                    char format )
{
    QString weString = ( lon < 0 ) ? tr("W") : tr("E");

    QString lonString;

    qreal lonDegF = ( unit == Degree ) ? fabs( lon ) : fabs( (qreal)(lon) * RAD2DEG );

    // Take care of -1 case
    precision = ( precision < 0 ) ? 5 : precision;
    
    if ( notation == GeoDataCoordinates::DMS )
    {
        int lonDeg = (int) lonDegF;
        qreal lonMinF = 60 * (lonDegF - lonDeg);
        int lonMin = (int) lonMinF;
        qreal lonSecF = 60 * (lonMinF - lonMin);
        int lonSec = (int) lonSecF;

        // Adjustment for fuzziness (like 49.999999999999999999999)
        if ( precision > 2 ) {
            lonSec = qRound( lonSecF );
        }
        if (lonSec > 59) {
            lonSecF = 0;
            lonSec = lonSecF;
            lonMin = lonMin + 1;
        }
        if ( precision == 0 ) {
            lonMin = qRound( lonMinF );
        }
        if (lonMin > 59) {
            lonMinF = 0;
            lonMin = lonMinF;
            lonDeg = lonDeg + 1;
        }

        // Evaluate the string
        lonString = QString("%1\xb0").arg(lonDeg, 3, 10, QChar(' ') );

        if ( precision == 0 || lonDeg == lonDegF ) {
            return lonString + weString;
        }

        lonString += QString(" %2\'").arg(lonMin, 2, 10, QChar('0') );

        if ( precision < 3 || lonMin == lonMinF || lonSec == 0 ) {
            return lonString + weString;
        }

        // Includes -1 case!
        if ( precision < 5 || lonSec == lonSecF ) {        
            lonString += QString(" %3\"").arg(lonSec, 2, 'f', 0, QChar('0') );
            return lonString + weString;
        }

        lonString += QString(" %L3\"").arg(lonSecF, precision - 1, 'f', precision - 4, QChar('0') );
    }
    else // notation = GeoDataCoordinates::Decimal
    {
        lonString = QString("%L1\xb0").arg(lonDegF, 4 + precision, format, precision, QChar(' ') );
    }

    return lonString + weString;
}

QString GeoDataCoordinates::lonToString() const
{
    return GeoDataCoordinates::lonToString( d->m_lon , s_notation );
}

QString GeoDataCoordinates::latToString( qreal lat, GeoDataCoordinates::Notation notation,
                                                    GeoDataCoordinates::Unit unit,
                                                    int precision,
                                                    char format )
{
    QString nsString = ( lat > 0 ) ? tr("N") : tr("S");

    QString latString;

    qreal latDegF = ( unit == Degree ) ? fabs( lat ) : fabs( (qreal)(lat) * RAD2DEG );

    // Take care of -1 case
    precision = ( precision < 0 ) ? 5 : precision;
    
    if ( notation == GeoDataCoordinates::DMS )
    {
        int latDeg = (int) latDegF;
        qreal latMinF = 60 * (latDegF - latDeg);
        int latMin = (int) latMinF;
        qreal latSecF = 60 * (latMinF - latMin);
        int latSec = (int) latSecF;

        // Adjustment for fuzziness (like 49.999999999999999999999)
        if ( precision > 2 ) {
            latSec = qRound( latSecF );
        }
        if (latSec > 59) {
            latSecF = 0;
            latSec = latSecF;
            latMin = latMin + 1;
        }
        if ( precision == 0 ) {
            latMin = qRound( latMinF );
        }
        if (latMin > 59) {
            latMinF = 0;
            latMin = latMinF;
            latDeg = latDeg + 1;
        }

        // Evaluate the string
        latString = QString("%1\xb0").arg(latDeg, 3, 10, QChar(' ') );

        if ( precision == 0 || latDeg == latDegF ) {
            return latString + nsString;
        }

        latString += QString(" %2\'").arg(latMin, 2, 10, QChar('0') );

        if ( precision < 3 || latMin == latMinF || latSec == 0 ) {
            return latString + nsString;
        }

        // Includes -1 case!
        if ( precision < 5 || latSec == latSecF ) {        
            latString += QString(" %3\"").arg(latSec, 2, 'f', 0, QChar('0') );
            return latString + nsString;
        }

        latString += QString(" %L3\"").arg(latSecF, precision - 1, 'f', precision - 4, QChar('0') );
    }
    else // notation = GeoDataCoordinates::Decimal
    {
        latString = QString("%L1\xb0").arg(latDegF, 4 + precision, format, precision, QChar(' ') );
    }
    return latString + nsString;
}

QString GeoDataCoordinates::latToString() const
{
    return GeoDataCoordinates::latToString( d->m_lat, s_notation );
}

bool GeoDataCoordinates::operator==( const GeoDataCoordinates &rhs ) const
{
    return *d == *rhs.d;
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

bool GeoDataCoordinates::isPole( Pole pole ) const
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
            if ( pole == AnyPole ){
                return true;
            }
            else {
                if ( pole == NorthPole && 2.0 * d->m_lat == +M_PI ) {
                    return true;
                }
                if ( pole == SouthPole && 2.0 * d->m_lat == -M_PI ) {
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
            mDebug() << "GeoDataCoordinates not normalized!";

            // Only as a last resort we cover the unlikely case where
            // the latitude is not normalized to the range of 
            // 90 deg S ... 90 deg N
            if ( fabs( (qreal) 2.0 * normalizeLat( d->m_lat ) ) < M_PI  ) {
                return false;
            }
            else {
                // Ok, we have hit a pole. Now let's check whether it's the one we've asked for:
                if ( pole == AnyPole ){
                    return true;
                }
                else {
                    if ( pole == NorthPole && 2.0 * d->m_lat == +M_PI ) {
                        return true;
                    }
                    if ( pole == SouthPole && 2.0 * d->m_lat == -M_PI ) {
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
