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
//


#include "GeoDataCoordinates.h"
#include "GeoDataCoordinates_p.h"

#include <QtCore/qmath.h>
#include <QtCore/QRegExp>
#include <QtCore/QLocale>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QCoreApplication>
#include <QtCore/QAtomicInt>

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
        if ( precision == 0 ) {
            lonDeg = qRound( lonDegF );
        } else if ( precision <= 2 ) {
            lonMin = qRound( lonMinF );
        } else if ( precision <= 4 ) {
            lonSec = qRound( lonSecF );
        } else {
            lonSec = lonSecF = qRound( lonSecF * qPow( 10, precision - 4 ) ) / qPow( 10, precision - 4 );
        }

        if (lonSec > 59) {
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

        lonString += QString(" %2\'").arg(lonMin, 2, 10, QChar('0') );

        if ( precision < 3 ) {
            return lonString + weString;
        }

        // Includes -1 case!
        if ( precision < 5 ) {
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
        if ( precision == 0 ) {
            latDeg = qRound( latDegF );
        } else if ( precision <= 2 ) {
            latMin = qRound( latMinF );
        } else if ( precision <= 4 ) {
            latSec = qRound( latSecF );
        } else {
            latSec = latSecF = qRound( latSecF * qPow( 10, precision - 4 ) ) / qPow( 10, precision - 4 );
        }

        if (latSec > 59) {
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
            return latString + nsString;
        }

        latString += QString(" %2\'").arg(latMin, 2, 10, QChar('0') );

        if ( precision < 3 ) {
            return latString + nsString;
        }

        // Includes -1 case!
        if ( precision < 5 ) {
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
