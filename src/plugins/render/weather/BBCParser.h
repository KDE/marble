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
#include <QHash>
#include <QList>
#include <QMutex>
#include <QPointer>
#include <QStack>
#include <QXmlStreamReader>

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
    ~BBCParser();

    static BBCParser *instance();
    void scheduleRead( const QString& path, BBCWeatherItem *item, const QString& type );

protected:
    bool workAvailable();
    void work();

Q_SIGNALS:
    void parsedFile();

private:
    explicit BBCParser( QObject *parent = 0 );
    QList<WeatherData> read( QIODevice *device );

    void readUnknownElement();
    void readBBC();
    void readChannel();
    void readItem();
    void readDescription( WeatherData *data );
    void readTitle( WeatherData *data );
    void readPubDate( WeatherData *data );

    QList<WeatherData> m_list;
    QStack<ScheduleEntry> m_schedule;
    QMutex m_scheduleMutex;

    QHash<QString, WeatherData::WeatherCondition> m_dayConditions;
    QHash<QString, WeatherData::WeatherCondition> m_nightConditions;
    QHash<QString, WeatherData::WindDirection> m_windDirections;
    QHash<QString, WeatherData::PressureDevelopment> m_pressureDevelopments;
    QHash<QString, WeatherData::Visibility> m_visibilityStates;
    QHash<QString, int> m_monthNames;
};

} // Marble namespace

#endif // BBCPARSER_H
