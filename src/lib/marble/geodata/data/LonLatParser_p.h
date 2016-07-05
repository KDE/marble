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

#ifndef MARBLE_LONLATPARSER_P_H
#define MARBLE_LONLATPARSER_P_H

#include <QStringList>

class QRegularExpressionMatch;

namespace Marble
{

// Helper class for GeoDataCoordinates::fromString(...)
class LonLatParser
{
private:
    enum DirPosition { PrefixDir, PostfixDir };

    /**
     * Parses the double value from the input string in system locale
     * if it contains the system locale decimalpoint char,
     * otherwise parses it in the C locale.
     */
    static double parseDouble(const QString& input);
    static QString createDecimalPointExp();
    static void getLocaleList(QStringList& localeList, const QString& localeListString,
                              const QLatin1String& placeholder, const QString& separator);
    static bool isDirection(const QString& input, const QString& direction);
    static bool isDirection(const QString& input, const QStringList& directions);
    static bool isOneOfDirections(const QString& input,
                                  const QString& firstDirection,
                                  const QString& secondDirection,
                                  bool& isFirstDirection);
    static bool isOneOfDirections(const QString& input,
                                  const QStringList& firstDirections,
                                  const QStringList& secondDirections,
                                  bool& isFirstDirection);

    /**
     * function template for the function calculating the degree value from
     * the captured texts with the degree, the minutes, the seconds and the signedness
     * (or less, depending on what the function actually expects)
     * @param regexMatch the regexp match to take the texts from
     * @param c the index in the list of captured texts of @p regex to start with
     * @param isPosHemisphere if the texts of the degree value are relative to the pos hemisphere
     * @return the calculated degree value
     */
    static qreal degreeValueFromDMS(const QRegularExpressionMatch& regexMatch, int c, bool isPosHemisphere);
    static qreal degreeValueFromDM(const QRegularExpressionMatch& regexMatch, int c, bool isPosHemisphere);
    static qreal degreeValueFromD(const QRegularExpressionMatch& regexMatch, int c, bool isPosHemisphere);

public:
    LonLatParser();

    /**
     * @brief parses the complete @p input string and sets the lon and lat properties if successful.
     * @param input the string to parse, must not have other content than the coordinates
     * @return @c true on successful parsing, @c false otherwise.
     */
    bool parse(const QString& input);

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
    bool tryMatchFromDms(const QString& input, DirPosition dirPosition);
    bool tryMatchFromDm(const QString& input, DirPosition dirPosition);
    bool tryMatchFromD(const QString& input, DirPosition dirPosition);

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
    bool isCorrectDirections(const QString& dir1, const QString& dir2,
                             bool& isDir1LonDir,
                             bool& isLonDirPosHemisphere, bool& isLatDirPosHemisphere) const;
    bool isLocaleLonDirection(const QString& input,
                              bool& isDirPosHemisphere) const;
    bool isLocaleLatDirection(const QString& input,
                              bool& isDirPosHemisphere) const;
    bool isLonDirection(const QString& input,
                        bool& isDirPosHemisphere) const;
    bool isLatDirection(const QString& input,
                        bool& isDirPosHemisphere) const;

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

}

#endif
