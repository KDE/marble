// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
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
    ~BBCParser() override;

    static BBCParser *instance();
    void scheduleRead( const QString& path, BBCWeatherItem *item, const QString& type );

protected:
    bool workAvailable() override;
    void work() override;

Q_SIGNALS:
    void parsedFile();

private:
    explicit BBCParser( QObject *parent = nullptr );
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
