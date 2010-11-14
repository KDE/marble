//
// This file is part of the Marble Virtual Globe.
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
#include "AbstractWorkerThread.h"
#include "WeatherData.h"

// Qt
#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QMutex>
#include <QtCore/QPointer>
#include <QtCore/QStack>
#include <QtCore/QXmlStreamReader>

class QObject;

namespace Marble
{

class BBCWeatherItem;

struct ScheduleEntry
{
    QString path;
    QPointer<BBCWeatherItem> item;
    QString type;
};

class BBCParser : public AbstractWorkerThread, public QXmlStreamReader
{
    Q_OBJECT
public:
    BBCParser( QObject *parent = 0 );
    ~BBCParser();

    static BBCParser *instance();
    void scheduleRead( const QString& path, BBCWeatherItem *item, const QString& type );

protected:
    bool workAvailable();
    void work();

Q_SIGNALS:
    void parsedFile();

private:
    QList<WeatherData> read( QIODevice *device );

    void readUnknownElement();
    void readBBC();
    void readChannel();
    void readItem();
    void readDescription( WeatherData *data );
    void readTitle( WeatherData *data );
    void readPubDate( WeatherData *data );
    
    void setupHashes();

    QList<WeatherData> m_list;
    QStack<ScheduleEntry> m_schedule;
    QMutex m_scheduleMutex;
    
    static QHash<QString, WeatherData::WeatherCondition> dayConditions;
    static QHash<QString, WeatherData::WeatherCondition> nightConditions;
    static QHash<QString, WeatherData::WindDirection> windDirections;
    static QHash<QString, WeatherData::PressureDevelopment> pressureDevelopments;
    static QHash<QString, WeatherData::Visibility> visibilityStates;
    static QHash<QString, int> monthNames;
};

} // Marble namespace

#endif // BBCPARSER_H
