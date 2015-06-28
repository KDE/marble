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
// Copyright 2011      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// Copyright 2015      Alejandro Garcia Montoro <alejandro.garciamontoro@gmail.com>
//


#include "GeoDataCoordinates.h"
#include "GeoDataCoordinates_p.h"

#include <qmath.h>
#include <QRegExp>
#include <QLocale>
#include <QString>
#include <QStringList>
#include <QCoreApplication>
#include <QAtomicInt>
#include <QDataStream>
#include <QPointF>

#include "MarbleGlobal.h"
#include "MarbleDebug.h"

#include "Quaternion.h"

namespace Marble
{

// Helper class for GeoDataCoordinates::fromString(...)
class LonLatParser
{
private:
    enum DirPosition { PrefixDir, PostfixDir };

    static QString createDecimalPointExp();
    static QString regExp( const QString& string );
    static void getLocaleList( QStringList& localeList, const QString& localeListString,
                               const QLatin1String& placeholder, const QString& separator );
    static bool isDirection( const QString& input, const QString& direction);
    static bool isDirection( const QString& input, const QStringList& directions);
    static bool isOneOfDirections( const QString& input,
                                   const QString& firstDirection,
                                   const QString& secondDirection,
                                   bool& isFirstDirection);
    static bool isOneOfDirections( const QString& input,
                                   const QStringList& firstDirections,
                                   const QStringList& secondDirections,
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
     * @param input the string to parse, must not have other content than the coordinates
     * @param dirPosition position of the dir in the list of captured texts
     * @return @c true on successful parsing, @c false otherwise.
     */
    bool tryMatchFromDms( const QString& input, DirPosition dirPosition );
    bool tryMatchFromDm( const QString& input, DirPosition dirPosition );
    bool tryMatchFromD( const QString& input, DirPosition dirPosition );

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

    const QString m_decimalPointExp;

private: // helper value, lazily set, in initAll();
    QStringList m_northLocale;
    QStringList m_eastLocale;
    QStringList m_southLocale;
    QStringList m_westLocale;

    QStringList m_degreeLocale;
    QStringList m_minutesLocale;
    QStringList m_secondsLocale;

    QString m_dirCapExp;
    QString m_degreeExp;
    QString m_minutesExp;
    QString m_secondsExp;
};

LonLatParser::LonLatParser()
    : m_lon( 0.0 )
    , m_lat( 0.0 )
    , m_north( QLatin1String("n") )
    , m_east(  QLatin1String("e") )
    , m_south( QLatin1String("s") )
    , m_west(  QLatin1String("w") )
    , m_decimalPointExp( createDecimalPointExp() )
{
}


void LonLatParser::initAll()
{
    // already all initialized?
    if (! m_dirCapExp.isEmpty() )
        return;

    const QLatin1String placeholder = QLatin1String("*");
    const QString separator = QLatin1String("|");

    getLocaleList( m_northLocale, GeoDataCoordinates::tr( "*", "North direction terms, see http://techbase.kde.org/Projects/Marble/GeoDataCoordinatesTranslation" ),
                   placeholder, separator );
    getLocaleList( m_eastLocale, GeoDataCoordinates::tr( "*", "East direction terms, see http://techbase.kde.org/Projects/Marble/GeoDataCoordinatesTranslation" ),
                   placeholder, separator );
    getLocaleList( m_southLocale, GeoDataCoordinates::tr( "*", "South direction terms, see http://techbase.kde.org/Projects/Marble/GeoDataCoordinatesTranslation" ),
                   placeholder, separator );
    getLocaleList( m_westLocale, GeoDataCoordinates::tr( "*", "West direction terms, see http://techbase.kde.org/Projects/Marble/GeoDataCoordinatesTranslation" ),
                   placeholder, separator );

    // use a set to remove duplicates
    QSet<QString> dirs = QSet<QString>()
        << m_north << m_east << m_south << m_west;
    dirs += m_northLocale.toSet();
    dirs += m_eastLocale.toSet();
    dirs += m_southLocale.toSet();
    dirs += m_westLocale.toSet();

    QString fullNamesExp;
    QString simpleLetters;

    foreach( const QString& dir, dirs ) {
        // collect simple letters
        if ((dir.length() == 1) && (QLatin1Char('a')<=dir.at(0)) && (dir.at(0)<=QLatin1Char('z'))) {
            simpleLetters += dir;
            continue;
        }

        // okay to add '|' also for last, separates from firstLetters
        fullNamesExp += regExp(dir) + QLatin1Char('|');
    }

    // Sets "(north|east|south|west|[nesw])" in en, as translated names match untranslated ones
    m_dirCapExp =
        QLatin1Char('(') + fullNamesExp + QLatin1Char('[') + simpleLetters + QLatin1String("])");

    // expressions for symbols of degree, minutes and seconds
    getLocaleList( m_degreeLocale, GeoDataCoordinates::tr( "*", "Degree symbol terms, see http://techbase.kde.org/Projects/Marble/GeoDataCoordinatesTranslation" ),
                   placeholder, separator );
    getLocaleList( m_minutesLocale, GeoDataCoordinates::tr( "*", "Minutes symbol terms, see http://techbase.kde.org/Projects/Marble/GeoDataCoordinatesTranslation" ),
                   placeholder, separator );
    getLocaleList( m_secondsLocale, GeoDataCoordinates::tr( "*", "Seconds symbol terms, see http://techbase.kde.org/Projects/Marble/GeoDataCoordinatesTranslation" ),
                   placeholder, separator );

    // Used unicode regexp expressions:
    // x00B0: ° DEGREE SIGN
    // x00BA: º MASCULINE ORDINAL INDICATOR (found used as degree sign)
    // x2032: ′ PRIME (minutes)
    // x00B4: ´ ACUTE ACCENT (found as minutes sign)
    // x02CA: ˊ MODIFIER LETTER ACUTE ACCENT
    // x2019: ’ RIGHT SINGLE QUOTATION MARK
    // x2033: ″ DOUBLE PRIME (seconds)
    // x201D: ” RIGHT DOUBLE QUOTATION MARK

    m_degreeExp = QLatin1String("\\x00B0|\\x00BA");
    foreach(const QString& symbol, m_degreeLocale) {
        m_degreeExp += QLatin1Char('|') + regExp(symbol);
    }
    m_minutesExp = QLatin1String("'|\\x2032|\\x00B4|\\x20C2|\\x2019");
    foreach(const QString& symbol, m_minutesLocale) {
        m_minutesExp += QLatin1Char('|') + regExp(symbol);
    }
    m_secondsExp = QLatin1String("\"|\\x2033|\\x201D|''|\\x2032\\x2032|\\x00B4\\x00B4|\\x20C2\\x20C2|\\x2019\\x2019");
    foreach(const QString& symbol, m_secondsLocale) {
        m_secondsExp += QLatin1Char('|') + regExp(symbol);
    }
}

bool LonLatParser::parse( const QString& string )
{
    QString input = string.toLower().trimmed();

    // #1: Just two numbers, no directions, e.g. 74.2245 -32.2434 (assumes lat lon)
    {
        const QString numberCapExp = QString::fromLatin1(
            "([-+]?\\d{1,3}%1?\\d*(?:[eE][+-]?\\d+)?)(?:,|;|\\s)\\s*"
            "([-+]?\\d{1,3}%1?\\d*(?:[eE][+-]?\\d+)?)"
            ).arg(m_decimalPointExp);

        const QRegExp regex = QRegExp( numberCapExp );
        if( regex.exactMatch(input) ) {
            m_lon = regex.cap(2).toDouble();
            m_lat = regex.cap(1).toDouble();

            return true;
        }
    }

    initAll();

    if ( tryMatchFromD( input, PostfixDir ) ) {
        return true;
    }

    if ( tryMatchFromD( input, PrefixDir ) ) {
        return true;
    }

    if ( tryMatchFromDms( input, PostfixDir ) ) {
        return true;
    }

    if ( tryMatchFromDms( input, PrefixDir ) ) {
        return true;
    }

    if ( tryMatchFromDm( input, PostfixDir ) ) {
        return true;
    }

    if ( tryMatchFromDm( input, PrefixDir ) ) {
        return true;
    }

    return false;
}

// #3: Sexagesimal
bool LonLatParser::tryMatchFromDms( const QString& input, DirPosition dirPosition )
{
    // direction as postfix
    const char *postfixCapExp =
        "([-+]?)(\\d{1,3})(?:%3|\\s)\\s*(\\d{1,2})(?:%4|\\s)\\s*"
        "(\\d{1,2}%1?\\d*)(?:%5)?\\s*%2[,;]?\\s*"
        "([-+]?)(\\d{1,3})(?:%3|\\s)\\s*(\\d{1,2})(?:%4|\\s)\\s*"
        "(\\d{1,2}%1?\\d*)(?:%5)?\\s*%2";

    // direction as prefix
    const char *prefixCapExp =
        "%2\\s*([-+]?)(\\d{1,3})(?:%3|\\s)\\s*(\\d{1,2})(?:%4|\\s)\\s*"
        "(\\d{1,2}%1?\\d*)(?:%5)?\\s*(?:,|;|\\s)\\s*"
        "%2\\s*([-+]?)(\\d{1,3})(?:%3|\\s)\\s*(\\d{1,2})(?:%4|\\s)\\s*"
        "(\\d{1,2}%1?\\d*)(?:%5)?";

    const char *expTemplate = ( dirPosition == PostfixDir ) ? postfixCapExp
                                                            : prefixCapExp;

    const QString numberCapExp =
        QString::fromLatin1( expTemplate ).arg( m_decimalPointExp, m_dirCapExp,
                                                m_degreeExp, m_minutesExp, m_secondsExp);

    const QRegExp regex = QRegExp( numberCapExp );
    if( !regex.exactMatch( input ) ) {
        return false;
    }

    bool isDir1LonDir;
    bool isLonDirPosHemisphere;
    bool isLatDirPosHemisphere;
    const QString dir1 = regex.cap( dirPosition == PostfixDir ? 5 : 1 );
    const QString dir2 = regex.cap( dirPosition == PostfixDir ? 10 : 6 );
    if ( !isCorrectDirections( dir1, dir2, isDir1LonDir,
                               isLonDirPosHemisphere, isLatDirPosHemisphere ) ) {
        return false;
    }

    const int valueStartIndex1 = (dirPosition == PostfixDir ? 1 : 2);
    const int valueStartIndex2 = (dirPosition == PostfixDir ? 6 : 7);
    m_lon = degreeValueFromDMS( regex, isDir1LonDir ? valueStartIndex1 : valueStartIndex2,
                                isLonDirPosHemisphere );
    m_lat = degreeValueFromDMS( regex, isDir1LonDir ? valueStartIndex2 : valueStartIndex1,
                                isLatDirPosHemisphere );

    return true;
}

// #4: Sexagesimal with minute precision
bool LonLatParser::tryMatchFromDm( const QString& input, DirPosition dirPosition )
{
    // direction as postfix
    const char *postfixCapExp =
        "([-+]?)(\\d{1,3})(?:%3|\\s)\\s*(\\d{1,2}%1?\\d*)(?:%4)?\\s*%2[,;]?\\s*"
        "([-+]?)(\\d{1,3})(?:%3|\\s)\\s*(\\d{1,2}%1?\\d*)(?:%4)?\\s*%2";

    // direction as prefix
    const char *prefixCapExp =
        "%2\\s*([-+]?)(\\d{1,3})(?:%3|\\s)\\s*(\\d{1,2}%1?\\d*)(?:%4)?\\s*(?:,|;|\\s)\\s*"
        "%2\\s*([-+]?)(\\d{1,3})(?:%3|\\s)\\s*(\\d{1,2}%1?\\d*)(?:%4)?";

    const char *expTemplate = ( dirPosition == PostfixDir ) ? postfixCapExp
                                                            : prefixCapExp;

    const QString numberCapExp =
        QString::fromLatin1( expTemplate ).arg( m_decimalPointExp, m_dirCapExp,
                                                m_degreeExp, m_minutesExp );
    const QRegExp regex = QRegExp( numberCapExp );
    if( !regex.exactMatch(input) ) {
        return false;
    }

    bool isDir1LonDir;
    bool isLonDirPosHemisphere;
    bool isLatDirPosHemisphere;
    const QString dir1 = regex.cap( dirPosition == PostfixDir ? 4 : 1 );
    const QString dir2 = regex.cap( dirPosition == PostfixDir ? 8 : 5 );
    if ( !isCorrectDirections( dir1, dir2, isDir1LonDir,
                               isLonDirPosHemisphere, isLatDirPosHemisphere ) ) {
        return false;
    }

    const int valueStartIndex1 = ( dirPosition == PostfixDir ? 1 : 2 );
    const int valueStartIndex2 = ( dirPosition == PostfixDir ? 5 : 6 );
    m_lon = degreeValueFromDM( regex, isDir1LonDir ? valueStartIndex1 : valueStartIndex2,
                               isLonDirPosHemisphere );
    m_lat = degreeValueFromDM( regex, isDir1LonDir ? valueStartIndex2 : valueStartIndex1,
                               isLatDirPosHemisphere );

    return true;
}

// #2: Two numbers with directions
bool LonLatParser::tryMatchFromD( const QString& input, DirPosition dirPosition )
{
    // direction as postfix, e.g. 74.2245 N 32.2434 W
    const char *postfixCapExp =
        "([-+]?\\d{1,3}%1?\\d*)(?:%3)?(?:\\s*)%2(?:,|;|\\s)\\s*"
        "([-+]?\\d{1,3}%1?\\d*)(?:%3)?(?:\\s*)%2";

    // direction as prefix, e.g. N 74.2245 W 32.2434
    const char *prefixCapExp =
        "%2\\s*([-+]?\\d{1,3}%1?\\d*)(?:%3)?\\s*(?:,|;|\\s)\\s*"
        "%2\\s*([-+]?\\d{1,3}%1?\\d*)(?:%3)?";

    const char *expTemplate = ( dirPosition == PostfixDir ) ? postfixCapExp
                                                            : prefixCapExp;

    const QString numberCapExp =
        QString::fromLatin1( expTemplate ).arg( m_decimalPointExp, m_dirCapExp, m_degreeExp );
    const QRegExp regex = QRegExp( numberCapExp );
    if( !regex.exactMatch( input ) ) {
        return false;
    }

    bool isDir1LonDir;
    bool isLonDirPosHemisphere;
    bool isLatDirPosHemisphere;
    const QString dir1 = regex.cap( dirPosition == PostfixDir ? 2 : 1 );
    const QString dir2 = regex.cap( dirPosition == PostfixDir ? 4 : 3 );
    if ( !isCorrectDirections( dir1, dir2, isDir1LonDir,
                               isLonDirPosHemisphere, isLatDirPosHemisphere ) ) {
        return false;
    }

    const int valueStartIndex1 = ( dirPosition == PostfixDir ? 1 : 2 );
    const int valueStartIndex2 = ( dirPosition == PostfixDir ? 3 : 4 );
    m_lon = degreeValueFromD( regex, isDir1LonDir ? valueStartIndex1 : valueStartIndex2,
                              isLonDirPosHemisphere );
    m_lat = degreeValueFromD( regex, isDir1LonDir ? valueStartIndex2 : valueStartIndex1,
                              isLatDirPosHemisphere );

    return true;
}

QString LonLatParser::createDecimalPointExp()
{
    const QChar decimalPoint = QLocale::system().decimalPoint();

    return (decimalPoint == QLatin1Char('.')) ? QString::fromLatin1("\\.") :
        QLatin1String("[.") + decimalPoint + QLatin1Char(']');
}

QString LonLatParser::regExp(const QString& string)
{
    QString result;
    for (int i = 0; i < string.length(); ++i) {
        const QChar c = string.at(i);
        if ((QLatin1Char('a') <= c) && (c <= QLatin1Char('z'))) {
            result += c;
        } else if (c.isSpace()) {
            result += QLatin1String("\\s");
        } else if (c == QLatin1Char('.')) {
            result += QLatin1String("\\.");
        } else {
            result += QString::fromLatin1("\\x%1").arg(c.unicode(), 4, 16, QLatin1Char('0'));
        }
    }
    return result;
}

void LonLatParser::getLocaleList( QStringList& localeList, const QString& localeListString,
                                  const QLatin1String& placeholder, const QString& separator )
{
    const QString lowerLocaleListString = localeListString.toLower();
    if (lowerLocaleListString != placeholder) {
        localeList = lowerLocaleListString.split(separator, QString::SkipEmptyParts);
    }
}

bool LonLatParser::isDirection( const QString& input, const QStringList& directions )
{
    return ( directions.contains(input) );
}

bool LonLatParser::isDirection( const QString& input, const QString& direction )
{
    return ( input == direction );
}

bool LonLatParser::isOneOfDirections( const QString& input,
                                      const QString& firstDirection,
                                      const QString& secondDirection,
                                      bool& isFirstDirection )
{
    isFirstDirection = isDirection(input, firstDirection);
    return isFirstDirection || isDirection(input, secondDirection);
}

bool LonLatParser::isOneOfDirections( const QString& input,
                                      const QStringList& firstDirections,
                                      const QStringList& secondDirections,
                                      bool& isFirstDirection )
{
    isFirstDirection = isDirection(input, firstDirections);
    return isFirstDirection || isDirection(input, secondDirections);
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

const qreal GeoDataCoordinatesPrivate::sm_semiMajorAxis = 6378137.0;
const qreal GeoDataCoordinatesPrivate::sm_semiMinorAxis = 6356752.314;
const qreal GeoDataCoordinatesPrivate::sm_eccentricitySquared = 6.69437999013e-03;
const qreal GeoDataCoordinatesPrivate::sm_utmScaleFactor = 0.9996;
GeoDataCoordinates::Notation GeoDataCoordinates::s_notation = GeoDataCoordinates::DMS;

const GeoDataCoordinates GeoDataCoordinates::null = GeoDataCoordinates( 0, 0, 0 ); // don't use default constructor!

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

/* simply copy null's d pointer
 * it will be replaced in the detach function
 */
GeoDataCoordinates::GeoDataCoordinates()
  : d( null.d )
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

bool GeoDataCoordinates::isValid() const
{
    return d != null.d;
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
#if QT_VERSION < 0x050000
    if(d->ref == 1)
#else
    if(d->ref.load() == 1)
#endif
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

void GeoDataCoordinates::geoCoordinates( qreal& lon, qreal& lat, qreal& alt,
                                         GeoDataCoordinates::Unit unit ) const
{
    geoCoordinates( lon, lat, unit );
    alt = d->m_altitude;
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
        QString coordString;

        if( notation == GeoDataCoordinates::UTM ){
            int zoneNumber = GeoDataCoordinatesPrivate::lonLatToZone(d->m_lon, d->m_lat);

            // Handle lack of UTM zone number in the poles
            QString zoneString = ( zoneNumber > 0 ) ? QString::number(zoneNumber) : QString("");

            QString bandString = GeoDataCoordinatesPrivate::lonLatToLatitudeBand(d->m_lon, d->m_lat);

            QString eastingString  = QString::number(GeoDataCoordinatesPrivate::lonLatToEasting(d->m_lon, d->m_lat), 'f', 2);
            QString northingString = QString::number(GeoDataCoordinatesPrivate::lonLatToNorthing(d->m_lon, d->m_lat), 'f', 2);

            return QString("%1%2 %3 m E, %4 m N").arg(zoneString).arg(bandString).arg(eastingString).arg(northingString);
        }
        else{
            coordString = lonToString( d->m_lon, notation, Radian, precision )
                        + QString(", ")
                        + latToString( d->m_lat, notation, Radian, precision );
        }

        return coordString;
}

QString GeoDataCoordinates::lonToString( qreal lon, GeoDataCoordinates::Notation notation,  
                                                    GeoDataCoordinates::Unit unit, 
                                                    int precision,
                                                    char format )
{
    if( notation == GeoDataCoordinates::UTM ){
        /**
         * @FIXME: UTM needs lon + lat to know zone number and easting
         * By now, this code returns the zone+easting of the point
         * (lon, equator), but this can differ a lot at different locations
         * See bug 347536 https://bugs.kde.org/show_bug.cgi?id=347536
         */

        qreal lonRad = ( unit == Radian ) ? lon : lon * DEG2RAD;

        int zoneNumber = GeoDataCoordinatesPrivate::lonLatToZone(lonRad, 0);

        // Handle lack of UTM zone number in the poles
        QString result = ( zoneNumber > 0 ) ? QString::number(zoneNumber) : QString("");

        if(precision > 0){
            QString eastingString = QString::number( GeoDataCoordinatesPrivate::lonLatToEasting(lonRad, 0), 'f', 2 );
            result += QString(" %1 m E").arg(eastingString);
        }

        return result;
    }

    QString weString = ( lon < 0 ) ? tr("W") : tr("E");

    QString lonString;

    qreal lonDegF = ( unit == Degree ) ? fabs( lon ) : fabs( (qreal)(lon) * RAD2DEG );

    // Take care of -1 case
    precision = ( precision < 0 ) ? 5 : precision;
    
    if ( notation == DMS || notation == DM ) {
        int lonDeg = (int) lonDegF;
        qreal lonMinF = 60 * (lonDegF - lonDeg);
        int lonMin = (int) lonMinF;
        qreal lonSecF = 60 * (lonMinF - lonMin);
        int lonSec = (int) lonSecF;

        // Adjustment for fuzziness (like 49.999999999999999999999)
        if ( precision == 0 ) {
            lonDeg = qRound( lonDegF );
        } else if ( precision <= 2 ) {
            lonMin = qRound( lonMinF );
        } else if ( precision <= 4 && notation == DMS ) {
            lonSec = qRound( lonSecF );
        } else {
            if ( notation == DMS ) {
                lonSec = lonSecF = qRound( lonSecF * qPow( 10, precision - 4 ) ) / qPow( 10, precision - 4 );
            }
            else {
                lonMin = lonMinF = qRound( lonMinF * qPow( 10, precision - 2 ) ) / qPow( 10, precision - 2 );
            }
        }

        if (lonSec > 59 && notation == DMS ) {
            lonSec = lonSecF = 0;
            lonMin = lonMinF = lonMinF + 1;
        }
        if (lonMin > 59) {
            lonMin = lonMinF = 0;
            lonDeg = lonDegF = lonDegF + 1;
        }

        // Evaluate the string
        lonString = QString::fromUtf8("%1\xc2\xb0").arg(lonDeg, 3, 10, QChar(' ') );

        if ( precision == 0 ) {
            return lonString + weString;
        }

        if ( notation == DMS || precision < 3 ) {
            lonString += QString(" %2\'").arg(lonMin, 2, 10, QChar('0') );
        }

        if ( precision < 3 ) {
            return lonString + weString;
        }

        if ( notation == DMS ) {
            // Includes -1 case!
            if ( precision < 5 ) {
                lonString += QString(" %3\"").arg(lonSec, 2, 'f', 0, QChar('0') );
                return lonString + weString;
            }

            lonString += QString(" %L3\"").arg(lonSecF, precision - 1, 'f', precision - 4, QChar('0') );
        }
        else {
            lonString += QString(" %L3'").arg(lonMinF, precision + 1, 'f', precision - 2, QChar('0') );
        }
    }
    else if ( notation == GeoDataCoordinates::Decimal )
    {
        lonString = QString::fromUtf8("%L1\xc2\xb0").arg(lonDegF, 4 + precision, format, precision, QChar(' ') );
    }
    else if ( notation == GeoDataCoordinates::Astro )
    {
        if (lon < 0) {
            lon += ( unit == Degree ) ? 360 : 2 * M_PI;
        }

        qreal lonHourF = ( unit == Degree ) ? fabs( lon/15.0  ) : fabs( (qreal)(lon/15.0) * RAD2DEG );
        int lonHour = (int) lonHourF;
        qreal lonMinF = 60 * (lonHourF - lonHour);
        int lonMin = (int) lonMinF;
        qreal lonSecF = 60 * (lonMinF - lonMin);
        int lonSec = (int) lonSecF;

        // Adjustment for fuzziness (like 49.999999999999999999999)
        if ( precision == 0 ) {
            lonHour = qRound( lonHourF );
        } else if ( precision <= 2 ) {
            lonMin = qRound( lonMinF );
        } else if ( precision <= 4 ) {
            lonSec = qRound( lonSecF );
        } else {
            lonSec = lonSecF = qRound( lonSecF * qPow( 10, precision - 4 ) ) / qPow( 10, precision - 4 );
        }

        if (lonSec > 59 ) {
            lonSec = lonSecF = 0;
            lonMin = lonMinF = lonMinF + 1;
        }
        if (lonMin > 59) {
            lonMin = lonMinF = 0;
            lonHour = lonHourF = lonHourF + 1;
        }

        // Evaluate the string
        lonString = QString::fromUtf8("%1h").arg(lonHour, 3, 10, QChar(' ') );

        if ( precision == 0 ) {
            return lonString;
        }

        lonString += QString(" %2\'").arg(lonMin, 2, 10, QChar('0') );

        if ( precision < 3 ) {
            return lonString;
        }

        // Includes -1 case!
        if ( precision < 5 ) {
            lonString += QString(" %3\"").arg(lonSec, 2, 'f', 0, QChar('0') );
            return lonString;
        }

        lonString += QString(" %L3\"").arg(lonSecF, precision - 1, 'f', precision - 4, QChar('0') );
        return lonString;
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
    if( notation == GeoDataCoordinates::UTM ){
        /**
         * @FIXME: UTM needs lon + lat to know latitude band and northing
         * By now, this code returns the band+northing of the point
         * (meridian, lat), but this can differ a lot at different locations
         * See bug 347536 https://bugs.kde.org/show_bug.cgi?id=347536
         */

        qreal latRad = ( unit == Radian ) ? lat : lat * DEG2RAD;

        QString result = GeoDataCoordinatesPrivate::lonLatToLatitudeBand(0, latRad);

        if ( precision > 0 ){
            QString northingString = QString::number( GeoDataCoordinatesPrivate::lonLatToNorthing(0, latRad), 'f', 2 );
            result += QString(" %1 m N").arg(northingString);
        }

        return result;
    }

    QString pmString;
    QString nsString;

    if (notation == GeoDataCoordinates::Astro){
        pmString = ( lat > 0 ) ? "+" : "-";
    }
    else {
        nsString = ( lat > 0 ) ? tr("N") : tr("S");
    }

    QString latString;

    qreal latDegF = ( unit == Degree ) ? fabs( lat ) : fabs( (qreal)(lat) * RAD2DEG );

    // Take care of -1 case
    precision = ( precision < 0 ) ? 5 : precision;
    
    if ( notation == DMS || notation == DM || notation == Astro) {
        int latDeg = (int) latDegF;
        qreal latMinF = 60 * (latDegF - latDeg);
        int latMin = (int) latMinF;
        qreal latSecF = 60 * (latMinF - latMin);
        int latSec = (int) latSecF;

        // Adjustment for fuzziness (like 49.999999999999999999999)
        if ( precision == 0 ) {
            latDeg = qRound( latDegF );
        } else if ( precision <= 2 ) {
            latMin = qRound( latMinF );
        } else if ( precision <= 4 && notation == DMS ) {
            latSec = qRound( latSecF );
        } else {
            if ( notation == DMS || notation == Astro ) {
                latSec = latSecF = qRound( latSecF * qPow( 10, precision - 4 ) ) / qPow( 10, precision - 4 );
            }
            else {
                latMin = latMinF = qRound( latMinF * qPow( 10, precision - 2 ) ) / qPow( 10, precision - 2 );
            }
        }

        if (latSec > 59 && ( notation == DMS || notation == Astro )) {
            latSecF = 0;
            latSec = latSecF;
            latMin = latMin + 1;
        }
        if (latMin > 59) {
            latMinF = 0;
            latMin = latMinF;
            latDeg = latDeg + 1;
        }

        // Evaluate the string
        latString = QString::fromUtf8("%1\xc2\xb0").arg(latDeg, 3, 10, QChar(' ') );

        if ( precision == 0 ) {
            return pmString + latString + nsString;
        }

        if ( notation == DMS || notation == Astro || precision < 3 ) {
            latString += QString(" %2\'").arg(latMin, 2, 10, QChar('0') );
        }

        if ( precision < 3 ) {
            return pmString + latString + nsString;
        }

        if ( notation == DMS || notation == Astro ) {
            // Includes -1 case!
            if ( precision < 5 ) {
                latString += QString(" %3\"").arg(latSec, 2, 'f', 0, QChar('0') );
                return latString + nsString;
            }

            latString += QString(" %L3\"").arg(latSecF, precision - 1, 'f', precision - 4, QChar('0') );
        }
        else {
            latString += QString(" %L3'").arg(latMinF, precision + 1, 'f', precision - 2, QChar('0') );
        }
    }
    else // notation = GeoDataCoordinates::Decimal
    {
        latString = QString::fromUtf8("%L1\xc2\xb0").arg(latDegF, 4 + precision, format, precision, QChar(' ') );
    }
    return pmString + latString + nsString;
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

int GeoDataCoordinates::utmZone() const{
    return GeoDataCoordinatesPrivate::lonLatToZone(d->m_lon, d->m_lat);
}

qreal GeoDataCoordinates::utmEasting() const{
    return GeoDataCoordinatesPrivate::lonLatToEasting(d->m_lon, d->m_lat);
}

QString GeoDataCoordinates::utmLatitudeBand() const{
    return GeoDataCoordinatesPrivate::lonLatToLatitudeBand(d->m_lon, d->m_lat);
}

qreal GeoDataCoordinates::utmNorthing() const{
    return GeoDataCoordinatesPrivate::lonLatToNorthing(d->m_lon, d->m_lat);
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

GeoDataCoordinates GeoDataCoordinates::rotateAround( const GeoDataCoordinates &axis, qreal angle, Unit unit ) const
{
    const Quaternion quatAxis = Quaternion::fromEuler( -axis.latitude() , axis.longitude(), 0 );
    const Quaternion rotationAmount = Quaternion::fromEuler( 0, 0, unit == Radian ? angle : angle * DEG2RAD );
    const Quaternion resultAxis = quatAxis * rotationAmount * quatAxis.inverse();

    Quaternion rotatedQuat = quaternion();
    rotatedQuat.rotateAroundAxis(resultAxis);
    qreal rotatedLon, rotatedLat;
    rotatedQuat.getSpherical(rotatedLon, rotatedLat);
    return GeoDataCoordinates(rotatedLon, rotatedLat, altitude());
}

qreal GeoDataCoordinates::bearing( const GeoDataCoordinates &other, Unit unit, BearingType type ) const
{
    if ( type == FinalBearing ) {
        double const offset = unit == Degree ? 180.0 : M_PI;
        return offset + other.bearing( *this, unit, InitialBearing );
    }

    qreal const delta = other.d->m_lon - d->m_lon;
    double const bearing = atan2( sin ( delta ) * cos ( other.d->m_lat ),
                 cos( d->m_lat ) * sin( other.d->m_lat ) - sin( d->m_lat ) * cos( other.d->m_lat ) * cos ( delta ) );
    return unit == Radian ? bearing : bearing * RAD2DEG;
}

GeoDataCoordinates GeoDataCoordinates::moveByBearing( qreal bearing, qreal distance ) const
{
    qreal newLat = asin( sin(d->m_lat) * cos(distance) +
                         cos(d->m_lat) * sin(distance) * cos(bearing) );
    qreal newLon = d->m_lon + atan2( sin(bearing) * sin(distance) * cos(d->m_lat),
                                     cos(distance) - sin(d->m_lat) * sin(newLat) );

    return GeoDataCoordinates( newLon, newLat );
}

const Quaternion& GeoDataCoordinates::quaternion() const
{
    return d->m_q;
}

GeoDataCoordinates GeoDataCoordinates::interpolate( const GeoDataCoordinates &target, double t_ ) const
{
    double const t = qBound( 0.0, t_, 1.0 );
    Quaternion const quat = Quaternion::slerp( d->m_q, target.d->m_q, t );
    qreal lon, lat;
    quat.getSpherical( lon, lat );
    double const alt = (1.0-t) * d->m_altitude + t * target.d->m_altitude;
    return GeoDataCoordinates( lon, lat, alt );
}

GeoDataCoordinates GeoDataCoordinates::interpolate( const GeoDataCoordinates &before, const GeoDataCoordinates &target, const GeoDataCoordinates &after, double t_ ) const
{
    double const t = qBound( 0.0, t_, 1.0 );
    Quaternion const b1 = GeoDataCoordinatesPrivate::basePoint( before.d->m_q, d->m_q, target.d->m_q );
    Quaternion const a2 = GeoDataCoordinatesPrivate::basePoint( d->m_q, target.d->m_q, after.d->m_q );
    Quaternion const a = Quaternion::slerp( d->m_q, target.d->m_q, t );
    Quaternion const b = Quaternion::slerp( b1, a2, t );
    Quaternion c = Quaternion::slerp( a, b, 2 * t * (1.0-t) );
    qreal lon, lat;
    c.getSpherical( lon, lat );
    // @todo spline interpolation of altitude?
    double const alt = (1.0-t) * d->m_altitude + t * target.d->m_altitude;
    return GeoDataCoordinates( lon, lat, alt );
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

Quaternion GeoDataCoordinatesPrivate::basePoint( const Quaternion &q1, const Quaternion &q2, const Quaternion &q3 )
{
    Quaternion const a = (q2.inverse() * q3).log();
    Quaternion const b = (q2.inverse() * q1).log();
    return q2 * ((a+b)*-0.25).exp();
}



qreal GeoDataCoordinatesPrivate::arcLengthOfMeridian( qreal phi )
{
    // Precalculate n
    qreal const n = (GeoDataCoordinatesPrivate::sm_semiMajorAxis - GeoDataCoordinatesPrivate::sm_semiMinorAxis)
                    / (GeoDataCoordinatesPrivate::sm_semiMajorAxis + GeoDataCoordinatesPrivate::sm_semiMinorAxis);

    // Precalculate alpha
    qreal const alpha = ( (GeoDataCoordinatesPrivate::sm_semiMajorAxis + GeoDataCoordinatesPrivate::sm_semiMinorAxis) / 2.0)
                        * (1.0 + (qPow (n, 2.0) / 4.0) + (qPow (n, 4.0) / 64.0) );

    // Precalculate beta
    qreal const beta = (-3.0 * n / 2.0)
                        + (9.0 * qPow (n, 3.0) / 16.0)
                        + (-3.0 * qPow (n, 5.0) / 32.0);

    // Precalculate gamma
    qreal const gamma = (15.0 * qPow (n, 2.0) / 16.0)
                        + (-15.0 * qPow (n, 4.0) / 32.0);

    // Precalculate delta
    qreal const delta = (-35.0 * qPow (n, 3.0) / 48.0)
                        + (105.0 * qPow (n, 5.0) / 256.0);

    // Precalculate epsilon
    qreal const epsilon = (315.0 * qPow (n, 4.0) / 512.0);

    // Now calculate the sum of the series and return
    qreal const result = alpha * (phi + (beta * qSin (2.0 * phi))
                        + (gamma * qSin (4.0 * phi))
                        + (delta * qSin (6.0 * phi))
                        + (epsilon * qSin (8.0 * phi)));

    return result;
}

qreal GeoDataCoordinatesPrivate::centralMeridianUTM( qreal zone )
{
    return DEG2RAD*(-183.0 + (zone * 6.0));
}

qreal GeoDataCoordinatesPrivate::footpointLatitude( qreal northing )
{
    // Precalculate n (Eq. 10.18)
    qreal const n = (GeoDataCoordinatesPrivate::sm_semiMajorAxis - GeoDataCoordinatesPrivate::sm_semiMinorAxis)
                    / (GeoDataCoordinatesPrivate::sm_semiMajorAxis + GeoDataCoordinatesPrivate::sm_semiMinorAxis);

    // Precalculate alpha (Eq. 10.22)
    // (Same as alpha in Eq. 10.17)
    qreal const alpha = ((GeoDataCoordinatesPrivate::sm_semiMajorAxis + GeoDataCoordinatesPrivate::sm_semiMinorAxis) / 2.0)
                        * (1 + (qPow (n, 2.0) / 4) + (qPow (n, 4.0) / 64));

    // Precalculate y (Eq. 10.23)
    qreal const y = northing / alpha;

    // Precalculate beta (Eq. 10.22)
    qreal const beta = (3.0 * n / 2.0) + (-27.0 * qPow (n, 3.0) / 32.0)
                        + (269.0 * qPow (n, 5.0) / 512.0);

    // Precalculate gamma (Eq. 10.22)
    qreal const gamma = (21.0 * qPow (n, 2.0) / 16.0)
                        + (-55.0 * qPow (n, 4.0) / 32.0);

    // Precalculate delta (Eq. 10.22)
    qreal const delta = (151.0 * qPow (n, 3.0) / 96.0)
                        + (-417.0 * qPow (n, 5.0) / 128.0);

    // Precalculate epsilon (Eq. 10.22)
    qreal const epsilon = (1097.0 * qPow (n, 4.0) / 512.0);

    // Now calculate the sum of the series (Eq. 10.21)
    qreal const result = y + (beta * qSin (2.0 * y))
                        + (gamma * qSin (4.0 * y))
                        + (delta * qSin (6.0 * y))
                        + (epsilon * qSin (8.0 * y));

    return result;
}

QPointF GeoDataCoordinatesPrivate::mapLonLatToXY( qreal lambda, qreal phi, qreal lambda0 )
{
    // Equation (10.15)

    // Precalculate second numerical eccentricity
    qreal const ep2 = (qPow (GeoDataCoordinatesPrivate::sm_semiMajorAxis, 2.0) - qPow (GeoDataCoordinatesPrivate::sm_semiMinorAxis, 2.0))
                    / qPow (GeoDataCoordinatesPrivate::sm_semiMinorAxis, 2.0);

    // Precalculate the square of nu, just an auxiliar quantity
    qreal const nu2 = ep2 * qPow (qCos(phi), 2.0);

    // Precalculate the radius of curvature in prime vertical
    qreal const N = qPow (GeoDataCoordinatesPrivate::sm_semiMajorAxis, 2.0) / (GeoDataCoordinatesPrivate::sm_semiMinorAxis * qSqrt (1 + nu2));

    // Precalculate the tangent of phi and its square
    qreal const t = qTan (phi);
    qreal const t2 = t * t;

    // Precalculate longitude difference
    qreal const l = lambda - lambda0;

    /*
     * Precalculate coefficients for l**n in the equations below
     * so a normal human being can read the expressions for easting
     * and northing
     * -- l**1 and l**2 have coefficients of 1.0
     *
     * The actual used coefficients starts at coef[1], just to
     * follow the meaningful nomenclature in equation 10.15
     * (coef[n] corresponds to qPow(l,n) factor)
     */
    QVector<qreal> coef(9);

    coef[0] = coef[1] = coef[2] = 1.0;

    coef[3] = 1.0 - t2 + nu2;

    coef[4] = 5.0 - t2 + 9 * nu2 + 4.0 * (nu2 * nu2);

    coef[5] = 5.0 - 18.0 * t2 + (t2 * t2) + 14.0 * nu2
            - 58.0 * t2 * nu2;

    coef[6] = 61.0 - 58.0 * t2 + (t2 * t2) + 270.0 * nu2
            - 330.0 * t2 * nu2;

    coef[7] = 61.0 - 479.0 * t2 + 179.0 * (t2 * t2) - (t2 * t2 * t2);

    coef[8] = 1385.0 - 3111.0 * t2 + 543.0 * (t2 * t2) - (t2 * t2 * t2);

    // Calculate easting (x)
    qreal easting = N * qCos(phi) * coef[1] * l
        + (N / 6.0 * qPow (qCos(phi), 3.0) * coef[3] * qPow (l, 3.0))
        + (N / 120.0 * qPow (qCos(phi), 5.0) * coef[5] * qPow (l, 5.0))
        + (N / 5040.0 * qPow (qCos(phi), 7.0) * coef[7] * qPow (l, 7.0));

    // Calculate northing (y)
    qreal northing = arcLengthOfMeridian (phi)
        + (t / 2.0 * N * qPow (qCos(phi), 2.0) * coef[2] * qPow (l, 2.0))
        + (t / 24.0 * N * qPow (qCos(phi), 4.0) * coef[4] * qPow (l, 4.0))
        + (t / 720.0 * N * qPow (qCos(phi), 6.0) * coef[6] * qPow (l, 6.0))
        + (t / 40320.0 * N * qPow (qCos(phi), 8.0) * coef[8] * qPow (l, 8.0));

    return QPointF(easting, northing);
}

int GeoDataCoordinatesPrivate::lonLatToZone( qreal lon, qreal lat ){
    // Converts lon and lat to degrees
    qreal lonDeg = lon * RAD2DEG;
    qreal latDeg = lat * RAD2DEG;

    /* Round the value of the longitude when the distance to the nearest integer
     * is less than 0.0000001. This avoids fuzzy values such as -114.0000000001, which
     * can produce a misbehaviour when calculating the zone associated at the borders
     * of the zone intervals (for example, the interval [-114, -108[ is associated with
     * zone number 12; if the following rounding is not done, the value returned by
     * lonLatToZone(114,0) is 11 instead of 12, as the function actually receives
     * -114.0000000001, which is in the interval [-120,-114[, associated to zone 11
     */
    qreal precision = 0.0000001;

    if ( qAbs(lonDeg - qFloor(lonDeg)) < precision || qAbs(lonDeg - qCeil(lonDeg)) < precision ){
        lonDeg = qRound(lonDeg);
    }

    // There is no numbering associated to the poles, special value 0 is returned.
    if ( latDeg < -80 || latDeg > 84 ) {
        return 0;
    }

    // Obtains the zone number handling all the so called "exceptions"
    // See problem: http://en.wikipedia.org/wiki/Universal_Transverse_Mercator_coordinate_system#Exceptions
    // See solution: http://gis.stackexchange.com/questions/13291/computing-utm-zone-from-lat-long-point

    // General
    int zoneNumber = static_cast<int>( (lonDeg+180) / 6.0 ) + 1;

    // Southwest Norway
    if ( latDeg >= 56 && latDeg < 64 && lonDeg >= 3 && lonDeg < 12 ) {
        zoneNumber = 32;
    }

    // Svalbard
    if ( latDeg >= 72 && latDeg < 84 ) {
        if ( lonDeg >= 0 && lonDeg < 9 ) {
            zoneNumber = 31;
        } else if ( lonDeg >= 9 && lonDeg < 21 ) {
            zoneNumber = 33;
        } else if ( lonDeg >= 21 && lonDeg < 33 ) {
            zoneNumber = 35;
        } else if ( lonDeg >= 33 && lonDeg < 42 ) {
            zoneNumber = 37;
        }
    }

    return zoneNumber;
}

qreal GeoDataCoordinatesPrivate::lonLatToEasting( qreal lon, qreal lat ){
    int zoneNumber = lonLatToZone( lon, lat );

    if ( zoneNumber == 0 ){
        qreal lonDeg = lon * RAD2DEG;
        zoneNumber = static_cast<int>( (lonDeg+180) / 6.0 ) + 1;
    }

    QPointF coordinates = GeoDataCoordinatesPrivate::mapLonLatToXY( lon, lat, GeoDataCoordinatesPrivate::centralMeridianUTM(zoneNumber) );

    // Adjust easting and northing for UTM system
    qreal easting = coordinates.x() * GeoDataCoordinatesPrivate::sm_utmScaleFactor + 500000.0;

    return easting;
}

QString GeoDataCoordinatesPrivate::lonLatToLatitudeBand( qreal lon, qreal lat ){
    // Obtains the latitude bands handling all the so called "exceptions"

    // Converts lon and lat to degrees
    qreal lonDeg = lon * RAD2DEG;
    qreal latDeg = lat * RAD2DEG;

    // Regular latitude bands between 80 S and 80 N (that is, between 10 and 170 in the [0,180] interval)
    int bandLetterIndex = 24; //Avoids "may be used uninitialized" warning

    if ( latDeg < -80 ) {
        // South pole (A for zones 1-30, B for zones 31-60)
        bandLetterIndex = ( (lonDeg+180) < 6*31 ) ? 0 : 1;
    } else if ( latDeg >= -80 && latDeg <= 80 ) {
        // General (+2 because the general lettering starts in C)
        bandLetterIndex = static_cast<int>( (latDeg+80.0) / 8.0 ) + 2;
    } else if ( latDeg >= 80 && latDeg < 84 ) {
        // Band X is extended 4 more degrees
        bandLetterIndex = 21;
    } else if ( latDeg >= 84 ) {
        // North pole (Y for zones 1-30, Z for zones 31-60)
        bandLetterIndex = ((lonDeg+180) < 6*31) ? 22 : 23;
    }

    return QString( "ABCDEFGHJKLMNPQRSTUVWXYZ?" ).at( bandLetterIndex );
}

qreal GeoDataCoordinatesPrivate::lonLatToNorthing( qreal lon, qreal lat ){
    int zoneNumber = lonLatToZone( lon, lat );

    if ( zoneNumber == 0 ){
        qreal lonDeg = lon * RAD2DEG;
        zoneNumber = static_cast<int>( (lonDeg+180) / 6.0 ) + 1;
    }

    QPointF coordinates = GeoDataCoordinatesPrivate::mapLonLatToXY( lon, lat, GeoDataCoordinatesPrivate::centralMeridianUTM(zoneNumber) );

    qreal northing = coordinates.y() * GeoDataCoordinatesPrivate::sm_utmScaleFactor;

    if ( northing < 0.0 ) {
        northing += 10000000.0;
    }

    return northing;
}

}
