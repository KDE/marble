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
// Copyright 2011      Friedrich W. H. Kossebau <kossebau@kde.org>
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

// Helper class for GeoDataCoordinates::fromString(...)
class LonLatParser
{
private:
    static QString createDecimalPointExp();
    static bool isDirection( const QString& input, const QString& direction);
    static bool isOneOfDirections( const QString& input,
                                   const QString& firstDirection,
                                   const QString& secondDirection,
                                   bool& isFirstDirection);

    /**
     * function template for the function calculating the degree value from
     * the captured texts with the degree, the minutes, the seconds and the signedness
     * (or less, depending on what the function actually expects)
     * @param regex the regexp to take the texts from
     * @param c the index in the list of captured texts of @p regex to start with
     * @param isPosHemisphere if the texts of the degree value are relative to the pos hemisphere
     * @return the calculated degree value
     */
    typedef qreal (&degreeValueFromXFunction)( const QRegExp& regex, int c, bool isPosHemisphere );
    static qreal degreeValueFromDMS( const QRegExp& regex, int c, bool isPosHemisphere );
    static qreal degreeValueFromDM( const QRegExp& regex, int c, bool isPosHemisphere );
    static qreal degreeValueFromD( const QRegExp& regex, int c, bool isPosHemisphere );

public:
    LonLatParser();
    /**
     * @brief parses the complete @p input string and sets the lon and lat properties if successful.
     * @param input the string to parse, must not have other content than the coordinates
     * @return @c true on successful parsing, @c false otherwise.
     */
    bool parse( const QString& input );
    /**
    * @brief return the lon value from the last successful parsing
    */
    qreal lon() const { return m_lon; }
    /**
    * @brief return the lat value from the last successful parsing
    */
    qreal lat() const { return m_lat; }

private:
    /**
     * @brief tries to parse the input with the given reg expression and get the lon and lat values
     * @param expTemplate the regular expression to use for matching
     * (should contain placeholders %1 and %2 for non-capturing decimalPoint expression
     * and resp. the capturing direction expression)
     * @param input the string to parse, must not have other content than the coordinates
     * @param degreeValueFromX function pointer of the function to use for calculating
     * the degree value from the captured texts
     * @param c the index in the list of captured texts which has the direction text
     * (assumes that the direction text is the last captured text per coordinate,
     * and that for both coordinates the order and the number of the texts are the same)
     * @return @c true on successful parsing, @c false otherwise.
     */
    bool tryMatch( const char* expTemplate, const QString& input,
                   degreeValueFromXFunction degreeValueFromX, int c );

    /**
    * @brief initializes also all properties which only need to be lazily initialized
    */
    void initAll();
    /**
     * @brief checks if the both passed directions are correct, also returns more data about them
     * @param dir1 first direction string
     * @param dir1 second direction string
     * @param isDir1LonDir is set to @c true if first direction string is a longitude direction,
     *                     @c false otherwise
     * @param isLonDirPosHemisphere is set to @c true if longitude direction is in positive hemisphere,
     *                              @c false otherwise
     * @param isLatDirPosHemisphere is set to @c true if latitude direction is in positive hemisphere,
     *                              @c false otherwise
     * @return @c true if @p dir1 and @p dir2 are correct, @c false otherwise.
     */
    bool isCorrectDirections( const QString& dir1, const QString& dir2,
                              bool& isDir1LonDir,
                              bool& isLonDirPosHemisphere, bool& isLatDirPosHemisphere ) const;
    bool isLocaleLonDirection( const QString& input,
                               bool& isDirPosHemisphere ) const;
    bool isLocaleLatDirection( const QString& input,
                               bool& isDirPosHemisphere ) const;
    bool isLonDirection( const QString& input,
                         bool& isDirPosHemisphere ) const;
    bool isLatDirection( const QString& input,
                         bool& isDirPosHemisphere ) const;

private:
    qreal m_lon;
    qreal m_lat;

private: // helper values
    const QString m_north;
    const QString m_east;
    const QString m_south;
    const QString m_west;

    const QString m_northLocale;
    const QString m_eastLocale;
    const QString m_southLocale;
    const QString m_westLocale;

    const QString m_decimalPointExp;

private: // helper value, lazily set, in initAll();
    QString m_dirCapExp;
};

LonLatParser::LonLatParser()
    : m_lon( 0.0 )
    , m_lat( 0.0 )
    , m_north( QLatin1String("north") )
    , m_east(  QLatin1String("east") )
    , m_south( QLatin1String("south") )
    , m_west(  QLatin1String("west") )
    , m_northLocale( GeoDataCoordinates::tr( "North", "The compass direction" ).toLower() )
    , m_eastLocale(  GeoDataCoordinates::tr( "East",  "The compass direction" ).toLower() )
    , m_southLocale( GeoDataCoordinates::tr( "South", "The compass direction" ).toLower() )
    , m_westLocale(  GeoDataCoordinates::tr( "West",  "The compass direction" ).toLower() )
    , m_decimalPointExp( createDecimalPointExp() )
{
}

void LonLatParser::initAll()
{
    // already all initialized?
    if (! m_dirCapExp.isEmpty() )
        return;

    const int maxLengthUntranslated = 5;
    const int maxLength =
        qMax( maxLengthUntranslated, qMax(
              m_northLocale.length(), qMax(
              m_eastLocale.length(), qMax(
              m_southLocale.length(),
              m_westLocale.length() ))));

    m_dirCapExp = QLatin1String("(\\w{1,") + QString::number(maxLength) + QLatin1String("})");
}

bool LonLatParser::parse( const QString& string )
{
    QString input = string.toLower().trimmed();

    // Used unicode regexp expressions:
    // x00B0: ° DEGREE SIGN
    // x2032: ′ PRIME (minutes)
    // x2033: ″ DOUBLE PRIME (seconds)

    // #1: Just two numbers, no directions, e.g. 74.2245 -32.2434 (assumes lat lon)
    {
        const QString numberCapExp = QString::fromLatin1(
            "([-+]?\\d{1,3}%1?\\d*)(?:\\x00B0?)(?:,|\\s)\\s*"
            "([-+]?\\d{1,3}%1?\\d*)(?:\\x00B0?)"
            ).arg(m_decimalPointExp);

        const QRegExp regex = QRegExp( numberCapExp );
        if( regex.exactMatch(input) ) {
            m_lon = regex.cap(2).toDouble();
            m_lat = regex.cap(1).toDouble();

            return true;
        }
    }

    initAll();

    // #2: Two numbers with directions, e.g. 74.2245 N 32.2434 W
    const char lonLatDCapExp[] =
        "([-+]?\\d{1,3}%1?\\d*)(?:\\x00B0?\\s*)%2(?:,|\\s)\\s*"
        "([-+]?\\d{1,3}%1?\\d*)(?:\\x00B0?\\s*)%2";

    if (tryMatch(lonLatDCapExp, input, degreeValueFromD, 2)) {
        return true;
    }

    // #3: Sexagesimal
    const char lonLatDMSCapExp[] =
        "([-+]?)(\\d{1,3})(?:\\x00B0|\\s)\\s*(\\d{1,2})(?:'|\\x2032|\\s)\\s*"
        "(\\d{1,2}%1?\\d*)(?:\"|\\x2033|'')?\\s*%2,?\\s*"
        "([-+]?)(\\d{1,3})(?:\\x00B0|\\s)\\s*(\\d{1,2})(?:'|\\x2032|\\s)\\s*"
        "(\\d{1,2}%1?\\d*)(?:\"|\\x2033|'')?\\s*%2";

    if (tryMatch(lonLatDMSCapExp, input, degreeValueFromDMS, 5)) {
        return true;
    }

    // #4: Sexagesimal with minute precision
    const char lonLatDMCapExp[] =
        "([-+]?)(\\d{1,3})(?:\\x00B0|\\s)\\s*(\\d{1,2}%1?\\d*)(?:'|\\x2032)?\\s*%2,?\\s*"
        "([-+]?)(\\d{1,3})(?:\\x00B0|\\s)\\s*(\\d{1,2}%1?\\d*)(?:'|\\x2032)?\\s*%2";

    if (tryMatch(lonLatDMCapExp, input, degreeValueFromDM, 4)) {
        return true;
    }

    return false;
}

bool LonLatParser::tryMatch(const char* expTemplate, const QString& input,
                            degreeValueFromXFunction degreeValueFromX, int c)
{
    bool successful = false;

    const QString numberCapExp = QString::fromLatin1(expTemplate).arg(m_decimalPointExp, m_dirCapExp);
    const QRegExp regex = QRegExp( numberCapExp );
    if( regex.exactMatch(input) ) {
        bool isDir1LonDir;
        bool isLonDirPosHemisphere;
        bool isLatDirPosHemisphere;
        const QString dir1 = regex.cap( c );
        const QString dir2 = regex.cap( 2*c );
        if (isCorrectDirections(dir1, dir2, isDir1LonDir,
                                isLonDirPosHemisphere, isLatDirPosHemisphere)) {
            m_lon = degreeValueFromX(regex, isDir1LonDir ? 1 : c+1, isLonDirPosHemisphere);
            m_lat = degreeValueFromX(regex, isDir1LonDir ? c+1 : 1, isLatDirPosHemisphere);

            successful = true;
        }
    }

    return successful;
}

QString LonLatParser::createDecimalPointExp()
{
    const QChar decimalPoint = QLocale::system().decimalPoint();

    return (decimalPoint == QLatin1Char('.')) ? QString::fromLatin1("\\.") :
        QLatin1String("[.") + decimalPoint + QLatin1Char(']');
}

bool LonLatParser::isDirection( const QString& input, const QString& direction)
{
    return ( input == direction.at(0) || input == direction );
}

bool LonLatParser::isOneOfDirections( const QString& input,
                                      const QString& firstDirection,
                                      const QString& secondDirection,
                                      bool& isFirstDirection )
{
    isFirstDirection = isDirection(input, firstDirection);
    return isFirstDirection || isDirection(input, secondDirection);
}


bool LonLatParser::isLocaleLonDirection( const QString& input,
                                         bool& isDirPosHemisphere ) const
{
    return isOneOfDirections(input, m_eastLocale, m_westLocale, isDirPosHemisphere);
}

bool LonLatParser::isLocaleLatDirection( const QString& input,
                                         bool& isDirPosHemisphere ) const
{
    return isOneOfDirections(input, m_northLocale, m_southLocale, isDirPosHemisphere);
}

bool LonLatParser::isLonDirection( const QString& input,
                                   bool& isDirPosHemisphere ) const
{
    return isOneOfDirections(input, m_east, m_west, isDirPosHemisphere);
}

bool LonLatParser::isLatDirection( const QString& input,
                                   bool& isDirPosHemisphere ) const
{
    return isOneOfDirections(input, m_north, m_south, isDirPosHemisphere);
}


qreal LonLatParser::degreeValueFromDMS( const QRegExp& regex, int c, bool isPosHemisphere )
{
    const bool isNegativeValue = (regex.cap( c++ ) == QLatin1String("-"));
    const uint degree = regex.cap( c++ ).toUInt();
    const uint minutes = regex.cap( c++ ).toUInt();
    const qreal seconds = regex.cap( c ).toDouble();

    qreal result = degree + (minutes*MIN2HOUR) + (seconds*SEC2HOUR);

    if (isNegativeValue)
        result *= -1;
    if (! isPosHemisphere)
        result *= -1;

    return result;
}

qreal LonLatParser::degreeValueFromDM( const QRegExp& regex, int c, bool isPosHemisphere )
{
    const bool isNegativeValue = (regex.cap( c++ ) == QLatin1String("-"));
    const uint degree = regex.cap( c++ ).toUInt();
    const qreal minutes = regex.cap( c ).toDouble();

    qreal result = degree + (minutes*MIN2HOUR);

    if (isNegativeValue)
        result *= -1;
    if (! isPosHemisphere)
        result *= -1;

    return result;
}

qreal LonLatParser::degreeValueFromD( const QRegExp& regex, int c, bool isPosHemisphere )
{
    qreal result = regex.cap( c ).toDouble();

    if (! isPosHemisphere)
        result *= -1;

    return result;
}

bool LonLatParser::isCorrectDirections(const QString& dir1, const QString& dir2,
                                       bool& isDir1LonDir,
                                       bool& isLonDirPosHemisphere,
                                       bool& isLatDirPosHemisphere) const
{
    // first try localized names
    isDir1LonDir = isLocaleLonDirection(dir1, isLonDirPosHemisphere);
    const bool resultLocale = isDir1LonDir ?
        isLocaleLatDirection(dir2, isLatDirPosHemisphere) :
        (isLocaleLatDirection(dir1, isLatDirPosHemisphere) &&
         isLocaleLonDirection(dir2, isLonDirPosHemisphere));

    if (resultLocale)
        return resultLocale;

    // fallback to try english names as lingua franca
    isDir1LonDir = isLonDirection(dir1, isLonDirPosHemisphere);
    return isDir1LonDir ?
        isLatDirection(dir2, isLatDirPosHemisphere) :
        (isLatDirection(dir1, isLatDirPosHemisphere) &&
         isLonDirection(dir2, isLonDirPosHemisphere));
}



GeoDataCoordinates::Notation GeoDataCoordinates::s_notation = GeoDataCoordinates::DMS;

GeoDataCoordinates::GeoDataCoordinates( qreal _lon, qreal _lat, qreal _alt, GeoDataCoordinates::Unit unit, int _detail )
  : d( new GeoDataCoordinatesPrivate( _lon, _lat, _alt, unit, _detail ) )
{
    d->ref.ref();
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
    d->ref.ref();
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
    d->ref.ref();
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
        d->m_q = Quaternion::fromSpherical( _lon, _lat );
        d->m_lon = _lon;
        d->m_lat = _lat;
        break;
    case Degree:
        d->m_q = Quaternion::fromSpherical( _lon * DEG2RAD , _lat * DEG2RAD  );
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
        d->m_q = Quaternion::fromSpherical( _lon, d->m_lat );
        d->m_lon = _lon;
        break;
    case Degree:
        d->m_q = Quaternion::fromSpherical( _lon * DEG2RAD , d->m_lat  );
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
        d->m_q = Quaternion::fromSpherical( d->m_lon, _lat );
        d->m_lat = _lat;
        break;
    case Degree:
        d->m_q = Quaternion::fromSpherical( d->m_lon, _lat * DEG2RAD   );
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

GeoDataCoordinates GeoDataCoordinates::fromString( const QString& string, bool& successful )
{
    LonLatParser parser;
    successful = parser.parse(string);
    if (successful) {
        return GeoDataCoordinates( parser.lon(), parser.lat(), 0, GeoDataCoordinates::Degree );
    } else {
        return GeoDataCoordinates();
    }
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
        lonString = QString::fromUtf8("%1\xc2\xb0").arg(lonDeg, 3, 10, QChar(' ') );

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
        lonString = QString::fromUtf8("%L1\xc2\xb0").arg(lonDegF, 4 + precision, format, precision, QChar(' ') );
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
        latString = QString::fromUtf8("%1\xc2\xb0").arg(latDeg, 3, 10, QChar(' ') );

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
        latString = QString::fromUtf8("%L1\xc2\xb0").arg(latDegF, 4 + precision, format, precision, QChar(' ') );
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

bool GeoDataCoordinates::operator!=( const GeoDataCoordinates &rhs ) const
{
    return *d != *rhs.d;
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

    d->m_q = Quaternion::fromSpherical( d->m_lon, d->m_lat );
}

}
