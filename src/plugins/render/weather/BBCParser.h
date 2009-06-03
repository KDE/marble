//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef BBCPARSER_H
#define BBCPARSER_H

// Marble
#include "WeatherData.h"

// Qt
#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QXmlStreamReader>

class QByteArray;
class QObject;

namespace Marble {

class BBCParser : public QXmlStreamReader
{
public:
    BBCParser();

    QList<WeatherData> read( QIODevice *device );

private:
    void readUnknownElement();
    void readBBC();
    void readChannel();
    void readItem();
    void readDescription( WeatherData *data );
    void readTitle( WeatherData *data );
    
    void setupWeatherConditions();

    QList<WeatherData> m_list;
    
    static QHash<QString, WeatherData::WeatherCondition> dayConditions;
    static QHash<QString, WeatherData::WeatherCondition> nightConditions;
};

} // Marble namespace

#endif // BBCPARSER_H
