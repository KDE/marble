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

#include "LonLatParser_p.h"

#include "GeoDataCoordinates.h"

#include "MarbleDebug.h"

#include <QLocale>
#include <QRegExp>
#include <QSet>


namespace Marble
{

LonLatParser::LonLatParser()
    : m_lon(0.0)
    , m_lat(0.0)
    , m_north(QStringLiteral("n"))
    , m_east( QStringLiteral("e"))
    , m_south(QStringLiteral("s"))
    , m_west( QStringLiteral("w"))
    , m_decimalPointExp(createDecimalPointExp())
{
}


void LonLatParser::initAll()
{
    // already all initialized?
    if (! m_dirCapExp.isEmpty()) {
        return;
    }

    const QLatin1String placeholder = QLatin1String("*");
    const QString separator = QStringLiteral("|");

    getLocaleList(m_northLocale, GeoDataCoordinates::tr("*", "North direction terms, see http://techbase.kde.org/Projects/Marble/GeoDataCoordinatesTranslation"),
                  placeholder, separator);
    getLocaleList(m_eastLocale, GeoDataCoordinates::tr("*", "East direction terms, see http://techbase.kde.org/Projects/Marble/GeoDataCoordinatesTranslation"),
                  placeholder, separator);
    getLocaleList(m_southLocale, GeoDataCoordinates::tr("*", "South direction terms, see http://techbase.kde.org/Projects/Marble/GeoDataCoordinatesTranslation"),
                  placeholder, separator);
    getLocaleList(m_westLocale, GeoDataCoordinates::tr("*", "West direction terms, see http://techbase.kde.org/Projects/Marble/GeoDataCoordinatesTranslation"),
                  placeholder, separator);

    // use a set to remove duplicates
    QSet<QString> dirs = QSet<QString>()
        << m_north << m_east << m_south << m_west;
    dirs += m_northLocale.toSet();
    dirs += m_eastLocale.toSet();
    dirs += m_southLocale.toSet();
    dirs += m_westLocale.toSet();

    QString fullNamesExp;
    QString simpleLetters;

    foreach(const QString& dir, dirs) {
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
        QLatin1Char('(') + fullNamesExp + QLatin1Char('[') + simpleLetters + QStringLiteral("])");

    // expressions for symbols of degree, minutes and seconds
    getLocaleList(m_degreeLocale, GeoDataCoordinates::tr("*", "Degree symbol terms, see http://techbase.kde.org/Projects/Marble/GeoDataCoordinatesTranslation"),
                  placeholder, separator);
    getLocaleList(m_minutesLocale, GeoDataCoordinates::tr("*", "Minutes symbol terms, see http://techbase.kde.org/Projects/Marble/GeoDataCoordinatesTranslation"),
                  placeholder, separator);
    getLocaleList(m_secondsLocale, GeoDataCoordinates::tr("*", "Seconds symbol terms, see http://techbase.kde.org/Projects/Marble/GeoDataCoordinatesTranslation"),
                  placeholder, separator);

    // Used unicode regexp expressions:
    // x00B0: ° DEGREE SIGN
    // x00BA: º MASCULINE ORDINAL INDICATOR (found used as degree sign)
    // x2032: ′ PRIME (minutes)
    // x00B4: ´ ACUTE ACCENT (found as minutes sign)
    // x02CA: ˊ MODIFIER LETTER ACUTE ACCENT
    // x2019: ’ RIGHT SINGLE QUOTATION MARK
    // x2033: ″ DOUBLE PRIME (seconds)
    // x201D: ” RIGHT DOUBLE QUOTATION MARK

    m_degreeExp = QStringLiteral("\\x00B0|\\x00BA");
    foreach(const QString& symbol, m_degreeLocale) {
        m_degreeExp += QLatin1Char('|') + regExp(symbol);
    }
    m_minutesExp = QStringLiteral("'|\\x2032|\\x00B4|\\x20C2|\\x2019");
    foreach(const QString& symbol, m_minutesLocale) {
        m_minutesExp += QLatin1Char('|') + regExp(symbol);
    }
    m_secondsExp = QStringLiteral("\"|\\x2033|\\x201D|''|\\x2032\\x2032|\\x00B4\\x00B4|\\x20C2\\x20C2|\\x2019\\x2019");
    foreach(const QString& symbol, m_secondsLocale) {
        m_secondsExp += QLatin1Char('|') + regExp(symbol);
    }
}

bool LonLatParser::parse(const QString& string)
{
    const QString input = string.toLower().trimmed();

    // #1: Just two numbers, no directions, e.g. 74.2245 -32.2434 (assumes lat lon)
    {
        const QString numberCapExp = QStringLiteral(
            "([-+]?\\d{1,3}%1?\\d*(?:[eE][+-]?\\d+)?)(?:,|;|\\s)\\s*"
            "([-+]?\\d{1,3}%1?\\d*(?:[eE][+-]?\\d+)?)"
            ).arg(m_decimalPointExp);

        const QRegExp regex = QRegExp(numberCapExp);
        if (regex.exactMatch(input)) {
            m_lon = parseDouble(regex.cap(2));
            m_lat = parseDouble(regex.cap(1));

            return true;
        }
    }

    initAll();

    if (tryMatchFromD(input, PostfixDir)) {
        return true;
    }

    if (tryMatchFromD(input, PrefixDir)) {
        return true;
    }

    if (tryMatchFromDms(input, PostfixDir)) {
        return true;
    }

    if (tryMatchFromDms(input, PrefixDir)) {
        return true;
    }

    if (tryMatchFromDm(input, PostfixDir)) {
        return true;
    }

    if (tryMatchFromDm(input, PrefixDir)) {
        return true;
    }

    return false;
}

// #3: Sexagesimal
bool LonLatParser::tryMatchFromDms(const QString& input, DirPosition dirPosition)
{
    // direction as postfix
    const QString postfixCapExp = QStringLiteral(
        "([-+]?)(\\d{1,3})(?:%3|\\s)\\s*(\\d{1,2})(?:%4|\\s)\\s*"
        "(\\d{1,2}%1?\\d*)(?:%5)?\\s*%2[,;]?\\s*"
        "([-+]?)(\\d{1,3})(?:%3|\\s)\\s*(\\d{1,2})(?:%4|\\s)\\s*"
        "(\\d{1,2}%1?\\d*)(?:%5)?\\s*%2");

    // direction as prefix
    const QString prefixCapExp = QStringLiteral(
        "%2\\s*([-+]?)(\\d{1,3})(?:%3|\\s)\\s*(\\d{1,2})(?:%4|\\s)\\s*"
        "(\\d{1,2}%1?\\d*)(?:%5)?\\s*(?:,|;|\\s)\\s*"
        "%2\\s*([-+]?)(\\d{1,3})(?:%3|\\s)\\s*(\\d{1,2})(?:%4|\\s)\\s*"
        "(\\d{1,2}%1?\\d*)(?:%5)?");

    const QString &expTemplate = (dirPosition == PostfixDir) ? postfixCapExp
                                                             : prefixCapExp;

    const QString numberCapExp = expTemplate.arg(m_decimalPointExp, m_dirCapExp,
                                                 m_degreeExp, m_minutesExp, m_secondsExp);

    const QRegExp regex = QRegExp(numberCapExp);
    if (!regex.exactMatch(input)) {
        return false;
    }

    bool isDir1LonDir;
    bool isLonDirPosHemisphere;
    bool isLatDirPosHemisphere;
    const QString dir1 = regex.cap(dirPosition == PostfixDir ? 5 : 1);
    const QString dir2 = regex.cap(dirPosition == PostfixDir ? 10 : 6);
    if (!isCorrectDirections(dir1, dir2, isDir1LonDir,
                             isLonDirPosHemisphere, isLatDirPosHemisphere)) {
        return false;
    }

    const int valueStartIndex1 = (dirPosition == PostfixDir ? 1 : 2);
    const int valueStartIndex2 = (dirPosition == PostfixDir ? 6 : 7);
    m_lon = degreeValueFromDMS(regex, isDir1LonDir ? valueStartIndex1 : valueStartIndex2,
                               isLonDirPosHemisphere);
    m_lat = degreeValueFromDMS(regex, isDir1LonDir ? valueStartIndex2 : valueStartIndex1,
                               isLatDirPosHemisphere);

    return true;
}

// #4: Sexagesimal with minute precision
bool LonLatParser::tryMatchFromDm(const QString& input, DirPosition dirPosition)
{
    // direction as postfix
    const QString postfixCapExp = QStringLiteral(
        "([-+]?)(\\d{1,3})(?:%3|\\s)\\s*(\\d{1,2}%1?\\d*)(?:%4)?\\s*%2[,;]?\\s*"
        "([-+]?)(\\d{1,3})(?:%3|\\s)\\s*(\\d{1,2}%1?\\d*)(?:%4)?\\s*%2");

    // direction as prefix
    const QString prefixCapExp = QStringLiteral(
        "%2\\s*([-+]?)(\\d{1,3})(?:%3|\\s)\\s*(\\d{1,2}%1?\\d*)(?:%4)?\\s*(?:,|;|\\s)\\s*"
        "%2\\s*([-+]?)(\\d{1,3})(?:%3|\\s)\\s*(\\d{1,2}%1?\\d*)(?:%4)?");

    const QString& expTemplate = (dirPosition == PostfixDir) ? postfixCapExp
                                                             : prefixCapExp;

    const QString numberCapExp = expTemplate.arg(m_decimalPointExp, m_dirCapExp,
                                                  m_degreeExp, m_minutesExp);
    const QRegExp regex = QRegExp(numberCapExp);
    if (!regex.exactMatch(input)) {
        return false;
    }

    bool isDir1LonDir;
    bool isLonDirPosHemisphere;
    bool isLatDirPosHemisphere;
    const QString dir1 = regex.cap(dirPosition == PostfixDir ? 4 : 1);
    const QString dir2 = regex.cap(dirPosition == PostfixDir ? 8 : 5);
    if (!isCorrectDirections(dir1, dir2, isDir1LonDir,
                             isLonDirPosHemisphere, isLatDirPosHemisphere)) {
        return false;
    }

    const int valueStartIndex1 = (dirPosition == PostfixDir ? 1 : 2);
    const int valueStartIndex2 = (dirPosition == PostfixDir ? 5 : 6);
    m_lon = degreeValueFromDM(regex, isDir1LonDir ? valueStartIndex1 : valueStartIndex2,
                              isLonDirPosHemisphere);
    m_lat = degreeValueFromDM(regex, isDir1LonDir ? valueStartIndex2 : valueStartIndex1,
                              isLatDirPosHemisphere);

    return true;
}

// #2: Two numbers with directions
bool LonLatParser::tryMatchFromD(const QString& input, DirPosition dirPosition)
{
    // direction as postfix, e.g. 74.2245 N 32.2434 W
    const QString postfixCapExp = QStringLiteral(
        "([-+]?\\d{1,3}%1?\\d*)(?:%3)?(?:\\s*)%2(?:,|;|\\s)\\s*"
        "([-+]?\\d{1,3}%1?\\d*)(?:%3)?(?:\\s*)%2");

    // direction as prefix, e.g. N 74.2245 W 32.2434
    const QString prefixCapExp = QStringLiteral(
        "%2\\s*([-+]?\\d{1,3}%1?\\d*)(?:%3)?\\s*(?:,|;|\\s)\\s*"
        "%2\\s*([-+]?\\d{1,3}%1?\\d*)(?:%3)?");

    const QString& expTemplate = (dirPosition == PostfixDir) ? postfixCapExp
                                                             : prefixCapExp;

    const QString numberCapExp = expTemplate.arg(m_decimalPointExp, m_dirCapExp, m_degreeExp);
    const QRegExp regex = QRegExp(numberCapExp);
    if (!regex.exactMatch(input)) {
        return false;
    }

    bool isDir1LonDir;
    bool isLonDirPosHemisphere;
    bool isLatDirPosHemisphere;
    const QString dir1 = regex.cap(dirPosition == PostfixDir ? 2 : 1);
    const QString dir2 = regex.cap(dirPosition == PostfixDir ? 4 : 3);
    if (!isCorrectDirections(dir1, dir2, isDir1LonDir,
                             isLonDirPosHemisphere, isLatDirPosHemisphere)) {
        return false;
    }

    const int valueStartIndex1 = (dirPosition == PostfixDir ? 1 : 2);
    const int valueStartIndex2 = (dirPosition == PostfixDir ? 3 : 4);
    m_lon = degreeValueFromD(regex, isDir1LonDir ? valueStartIndex1 : valueStartIndex2,
                             isLonDirPosHemisphere);
    m_lat = degreeValueFromD(regex, isDir1LonDir ? valueStartIndex2 : valueStartIndex1,
                             isLatDirPosHemisphere);

    return true;
}

double LonLatParser::parseDouble(const QString& input)
{
    // Decide by decimalpoint if system locale or C locale should be tried.
    // Otherwise if first trying with a system locale when the string is in C locale,
    // the "." might be misinterpreted as thousands group separator and thus a wrong
    // value yielded
    QLocale locale = QLocale::system();
    return input.contains(locale.decimalPoint()) ? locale.toDouble(input) : input.toDouble();
}

QString LonLatParser::createDecimalPointExp()
{
    const QChar decimalPoint = QLocale::system().decimalPoint();

    return (decimalPoint == QLatin1Char('.')) ? QStringLiteral("\\.") :
        QStringLiteral("[.") + decimalPoint + QLatin1Char(']');
}

QString LonLatParser::regExp(const QString& string)
{
    QString result;
    for (int i = 0; i < string.length(); ++i) {
        const QChar c = string.at(i);
        if ((QLatin1Char('a') <= c) && (c <= QLatin1Char('z'))) {
            result += c;
        } else if (c.isSpace()) {
            result += QStringLiteral("\\s");
        } else if (c == QLatin1Char('.')) {
            result += QStringLiteral("\\.");
        } else {
            result += QStringLiteral("\\x%1").arg(c.unicode(), 4, 16, QLatin1Char('0'));
        }
    }
    return result;
}

void LonLatParser::getLocaleList(QStringList& localeList, const QString& localeListString,
                                 const QLatin1String& placeholder, const QString& separator)
{
    const QString lowerLocaleListString = localeListString.toLower();
    if (lowerLocaleListString != placeholder) {
        localeList = lowerLocaleListString.split(separator, QString::SkipEmptyParts);
    }
}

bool LonLatParser::isDirection(const QString& input, const QStringList& directions)
{
    return (directions.contains(input));
}

bool LonLatParser::isDirection(const QString& input, const QString& direction)
{
    return (input == direction);
}

bool LonLatParser::isOneOfDirections(const QString& input,
                                     const QString& firstDirection,
                                     const QString& secondDirection,
                                     bool& isFirstDirection)
{
    isFirstDirection = isDirection(input, firstDirection);
    return isFirstDirection || isDirection(input, secondDirection);
}

bool LonLatParser::isOneOfDirections(const QString& input,
                                     const QStringList& firstDirections,
                                     const QStringList& secondDirections,
                                     bool& isFirstDirection)
{
    isFirstDirection = isDirection(input, firstDirections);
    return isFirstDirection || isDirection(input, secondDirections);
}


bool LonLatParser::isLocaleLonDirection(const QString& input,
                                        bool& isDirPosHemisphere) const
{
    return isOneOfDirections(input, m_eastLocale, m_westLocale, isDirPosHemisphere);
}

bool LonLatParser::isLocaleLatDirection(const QString& input,
                                        bool& isDirPosHemisphere) const
{
    return isOneOfDirections(input, m_northLocale, m_southLocale, isDirPosHemisphere);
}

bool LonLatParser::isLonDirection(const QString& input,
                                  bool& isDirPosHemisphere) const
{
    return isOneOfDirections(input, m_east, m_west, isDirPosHemisphere);
}

bool LonLatParser::isLatDirection(const QString& input,
                                  bool& isDirPosHemisphere) const
{
    return isOneOfDirections(input, m_north, m_south, isDirPosHemisphere);
}


qreal LonLatParser::degreeValueFromDMS(const QRegExp& regex, int c, bool isPosHemisphere)
{
    const bool isNegativeValue = (regex.cap(c++) == QLatin1String("-"));
    const uint degree = regex.cap(c++).toUInt();
    const uint minutes = regex.cap(c++).toUInt();
    const qreal seconds = parseDouble(regex.cap(c));

    qreal result = degree + (minutes * MIN2HOUR) + (seconds * SEC2HOUR);

    if (isNegativeValue) {
        result *= -1;
    }
    if (! isPosHemisphere) {
        result *= -1;
    }

    return result;
}

qreal LonLatParser::degreeValueFromDM(const QRegExp& regex, int c, bool isPosHemisphere)
{
    const bool isNegativeValue = (regex.cap(c++) == QLatin1String("-"));
    const uint degree = regex.cap(c++).toUInt();
    const qreal minutes = parseDouble(regex.cap(c));

    qreal result = degree + (minutes * MIN2HOUR);

    if (isNegativeValue) {
        result *= -1;
    }
    if (! isPosHemisphere) {
        result *= -1;
    }

    return result;
}

qreal LonLatParser::degreeValueFromD(const QRegExp& regex, int c, bool isPosHemisphere)
{
    qreal result = parseDouble(regex.cap(c));

    if (! isPosHemisphere) {
        result *= -1;
    }

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

    if (resultLocale) {
        return resultLocale;
    }

    // fallback to try english names as lingua franca
    isDir1LonDir = isLonDirection(dir1, isLonDirPosHemisphere);
    return isDir1LonDir ?
        isLatDirection(dir2, isLatDirPosHemisphere) :
        (isLatDirection(dir1, isLatDirPosHemisphere) &&
         isLonDirection(dir2, isLonDirPosHemisphere));
}

}
