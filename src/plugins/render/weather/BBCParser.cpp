// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "BBCParser.h"

// Marble
#include "BBCWeatherItem.h"
#include "MarbleDebug.h"
#include "MarbleGlobal.h"

// Qt
#include <QDateTime>
#include <QFile>
#include <QMutexLocker>
#include <QRegExp>

using namespace Marble;

BBCParser::BBCParser(QObject *parent)
    : AbstractWorkerThread(parent)
    , m_dayConditions()
    , m_nightConditions()
    , m_windDirections()
    , m_pressureDevelopments()
    , m_visibilityStates()
    , m_monthNames()
{
    m_dayConditions[QStringLiteral("sunny")] = WeatherData::ClearDay;
    m_dayConditions[QStringLiteral("clear")] = WeatherData::ClearDay;
    m_dayConditions[QStringLiteral("clear sky")] = WeatherData::ClearDay;
    m_dayConditions[QStringLiteral("sunny intervals")] = WeatherData::FewCloudsDay;
    m_dayConditions[QStringLiteral("partly cloudy")] = WeatherData::PartlyCloudyDay;
    m_dayConditions[QStringLiteral("white cloud")] = WeatherData::Overcast;
    m_dayConditions[QStringLiteral("grey cloud")] = WeatherData::Overcast;
    m_dayConditions[QStringLiteral("cloudy")] = WeatherData::Overcast;
    m_dayConditions[QStringLiteral("drizzle")] = WeatherData::LightRain;
    m_dayConditions[QStringLiteral("misty")] = WeatherData::Mist;
    m_dayConditions[QStringLiteral("mist")] = WeatherData::Mist;
    m_dayConditions[QStringLiteral("fog")] = WeatherData::Mist;
    m_dayConditions[QStringLiteral("foggy")] = WeatherData::Mist;
    m_dayConditions[QStringLiteral("dense fog")] = WeatherData::Mist;
    m_dayConditions[QStringLiteral("Thick Fog")] = WeatherData::Mist;
    m_dayConditions[QStringLiteral("tropical storm")] = WeatherData::Thunderstorm;
    m_dayConditions[QStringLiteral("hazy")] = WeatherData::Mist;
    m_dayConditions[QStringLiteral("light shower")] = WeatherData::LightShowersDay;
    m_dayConditions[QStringLiteral("light rain shower")] = WeatherData::LightShowersDay;
    m_dayConditions[QStringLiteral("light showers")] = WeatherData::LightShowersDay;
    m_dayConditions[QStringLiteral("light rain")] = WeatherData::ShowersDay;
    m_dayConditions[QStringLiteral("heavy rain")] = WeatherData::Rain;
    m_dayConditions[QStringLiteral("heavy showers")] = WeatherData::Rain;
    m_dayConditions[QStringLiteral("heavy shower")] = WeatherData::Rain;
    m_dayConditions[QStringLiteral("heavy rain shower")] = WeatherData::Rain;
    m_dayConditions[QStringLiteral("thundery shower")] = WeatherData::Thunderstorm;
    m_dayConditions[QStringLiteral("thunderstorm")] = WeatherData::Thunderstorm;
    m_dayConditions[QStringLiteral("thunder storm")] = WeatherData::Thunderstorm;
    m_dayConditions[QStringLiteral("cloudy with sleet")] = WeatherData::RainSnow;
    m_dayConditions[QStringLiteral("sleet shower")] = WeatherData::RainSnow;
    m_dayConditions[QStringLiteral("sleet showers")] = WeatherData::RainSnow;
    m_dayConditions[QStringLiteral("sleet")] = WeatherData::RainSnow;
    m_dayConditions[QStringLiteral("cloudy with hail")] = WeatherData::Hail;
    m_dayConditions[QStringLiteral("hail shower")] = WeatherData::Hail;
    m_dayConditions[QStringLiteral("hail showers")] = WeatherData::Hail;
    m_dayConditions[QStringLiteral("hail")] = WeatherData::Hail;
    m_dayConditions[QStringLiteral("light snow")] = WeatherData::LightSnow;
    m_dayConditions[QStringLiteral("light snow shower")] = WeatherData::ChanceSnowDay;
    m_dayConditions[QStringLiteral("light snow showers")] = WeatherData::ChanceSnowDay;
    m_dayConditions[QStringLiteral("cloudy with light snow")] = WeatherData::LightSnow;
    m_dayConditions[QStringLiteral("heavy snow")] = WeatherData::Snow;
    m_dayConditions[QStringLiteral("heavy snow shower")] = WeatherData::Snow;
    m_dayConditions[QStringLiteral("heavy snow showers")] = WeatherData::Snow;
    m_dayConditions[QStringLiteral("cloudy with heavy snow")] = WeatherData::Snow;
    m_dayConditions[QStringLiteral("sandstorm")] = WeatherData::SandStorm;
    m_dayConditions[QStringLiteral("na")] = WeatherData::ConditionNotAvailable;
    m_dayConditions[QStringLiteral("N/A")] = WeatherData::ConditionNotAvailable;

    m_nightConditions[QStringLiteral("sunny")] = WeatherData::ClearNight;
    m_nightConditions[QStringLiteral("clear")] = WeatherData::ClearNight;
    m_nightConditions[QStringLiteral("clear sky")] = WeatherData::ClearNight;
    m_nightConditions[QStringLiteral("sunny intervals")] = WeatherData::FewCloudsNight;
    m_nightConditions[QStringLiteral("partly cloudy")] = WeatherData::PartlyCloudyNight;
    m_nightConditions[QStringLiteral("white cloud")] = WeatherData::Overcast;
    m_nightConditions[QStringLiteral("grey cloud")] = WeatherData::Overcast;
    m_nightConditions[QStringLiteral("cloudy")] = WeatherData::Overcast;
    m_nightConditions[QStringLiteral("drizzle")] = WeatherData::LightRain;
    m_nightConditions[QStringLiteral("misty")] = WeatherData::Mist;
    m_nightConditions[QStringLiteral("mist")] = WeatherData::Mist;
    m_nightConditions[QStringLiteral("fog")] = WeatherData::Mist;
    m_nightConditions[QStringLiteral("foggy")] = WeatherData::Mist;
    m_nightConditions[QStringLiteral("dense fog")] = WeatherData::Mist;
    m_nightConditions[QStringLiteral("Thick Fog")] = WeatherData::Mist;
    m_nightConditions[QStringLiteral("tropical storm")] = WeatherData::Thunderstorm;
    m_nightConditions[QStringLiteral("hazy")] = WeatherData::Mist;
    m_nightConditions[QStringLiteral("light shower")] = WeatherData::LightShowersNight;
    m_nightConditions[QStringLiteral("light rain shower")] = WeatherData::LightShowersNight;
    m_nightConditions[QStringLiteral("light showers")] = WeatherData::LightShowersNight;
    m_nightConditions[QStringLiteral("light rain")] = WeatherData::ShowersNight;
    m_nightConditions[QStringLiteral("heavy rain")] = WeatherData::Rain;
    m_nightConditions[QStringLiteral("heavy showers")] = WeatherData::Rain;
    m_nightConditions[QStringLiteral("heavy shower")] = WeatherData::Rain;
    m_nightConditions[QStringLiteral("heavy rain shower")] = WeatherData::Rain;
    m_nightConditions[QStringLiteral("thundery shower")] = WeatherData::Thunderstorm;
    m_nightConditions[QStringLiteral("thunderstorm")] = WeatherData::Thunderstorm;
    m_nightConditions[QStringLiteral("thunder storm")] = WeatherData::Thunderstorm;
    m_nightConditions[QStringLiteral("cloudy with sleet")] = WeatherData::RainSnow;
    m_nightConditions[QStringLiteral("sleet shower")] = WeatherData::RainSnow;
    m_nightConditions[QStringLiteral("sleet showers")] = WeatherData::RainSnow;
    m_nightConditions[QStringLiteral("sleet")] = WeatherData::RainSnow;
    m_nightConditions[QStringLiteral("cloudy with hail")] = WeatherData::Hail;
    m_nightConditions[QStringLiteral("hail shower")] = WeatherData::Hail;
    m_nightConditions[QStringLiteral("hail showers")] = WeatherData::Hail;
    m_nightConditions[QStringLiteral("hail")] = WeatherData::Hail;
    m_nightConditions[QStringLiteral("light snow")] = WeatherData::LightSnow;
    m_nightConditions[QStringLiteral("light snow shower")] = WeatherData::ChanceSnowNight;
    m_nightConditions[QStringLiteral("light snow showers")] = WeatherData::ChanceSnowNight;
    m_nightConditions[QStringLiteral("cloudy with light snow")] = WeatherData::LightSnow;
    m_nightConditions[QStringLiteral("heavy snow")] = WeatherData::Snow;
    m_nightConditions[QStringLiteral("heavy snow shower")] = WeatherData::Snow;
    m_nightConditions[QStringLiteral("heavy snow showers")] = WeatherData::Snow;
    m_nightConditions[QStringLiteral("cloudy with heavy snow")] = WeatherData::Snow;
    m_nightConditions[QStringLiteral("sandstorm")] = WeatherData::SandStorm;
    m_nightConditions[QStringLiteral("na")] = WeatherData::ConditionNotAvailable;
    m_nightConditions[QStringLiteral("N/A")] = WeatherData::ConditionNotAvailable;

    m_windDirections[QStringLiteral("N")] = WeatherData::N;
    m_windDirections[QStringLiteral("NE")] = WeatherData::NE;
    m_windDirections[QStringLiteral("ENE")] = WeatherData::ENE;
    m_windDirections[QStringLiteral("NNE")] = WeatherData::NNE;
    m_windDirections[QStringLiteral("E")] = WeatherData::E;
    m_windDirections[QStringLiteral("SSE")] = WeatherData::SSE;
    m_windDirections[QStringLiteral("SE")] = WeatherData::SE;
    m_windDirections[QStringLiteral("ESE")] = WeatherData::ESE;
    m_windDirections[QStringLiteral("S")] = WeatherData::S;
    m_windDirections[QStringLiteral("NNW")] = WeatherData::NNW;
    m_windDirections[QStringLiteral("NW")] = WeatherData::NW;
    m_windDirections[QStringLiteral("WNW")] = WeatherData::WNW;
    m_windDirections[QStringLiteral("W")] = WeatherData::W;
    m_windDirections[QStringLiteral("SSW")] = WeatherData::SSW;
    m_windDirections[QStringLiteral("SW")] = WeatherData::SW;
    m_windDirections[QStringLiteral("WSW")] = WeatherData::WSW;
    m_windDirections[QStringLiteral("N/A")] = WeatherData::DirectionNotAvailable;

    m_pressureDevelopments[QStringLiteral("falling")] = WeatherData::Falling;
    m_pressureDevelopments[QStringLiteral("no change")] = WeatherData::NoChange;
    m_pressureDevelopments[QStringLiteral("steady")] = WeatherData::NoChange;
    m_pressureDevelopments[QStringLiteral("rising")] = WeatherData::Rising;
    m_pressureDevelopments[QStringLiteral("N/A")] = WeatherData::PressureDevelopmentNotAvailable;

    m_visibilityStates[QStringLiteral("excellent")] = WeatherData::VeryGood;
    m_visibilityStates[QStringLiteral("very good")] = WeatherData::VeryGood;
    m_visibilityStates[QStringLiteral("good")] = WeatherData::Good;
    m_visibilityStates[QStringLiteral("moderate")] = WeatherData::Normal;
    m_visibilityStates[QStringLiteral("poor")] = WeatherData::Poor;
    m_visibilityStates[QStringLiteral("very poor")] = WeatherData::VeryPoor;
    m_visibilityStates[QStringLiteral("fog")] = WeatherData::Fog;
    m_visibilityStates[QStringLiteral("n/a")] = WeatherData::VisibilityNotAvailable;

    m_monthNames[QStringLiteral("Jan")] = 1;
    m_monthNames[QStringLiteral("Feb")] = 2;
    m_monthNames[QStringLiteral("Mar")] = 3;
    m_monthNames[QStringLiteral("Apr")] = 4;
    m_monthNames[QStringLiteral("May")] = 5;
    m_monthNames[QStringLiteral("Jun")] = 6;
    m_monthNames[QStringLiteral("Jul")] = 7;
    m_monthNames[QStringLiteral("Aug")] = 8;
    m_monthNames[QStringLiteral("Sep")] = 9;
    m_monthNames[QStringLiteral("Oct")] = 10;
    m_monthNames[QStringLiteral("Nov")] = 11;
    m_monthNames[QStringLiteral("Dec")] = 12;
}

BBCParser::~BBCParser() = default;

BBCParser *BBCParser::instance()
{
    static BBCParser parser;
    return &parser;
}

void BBCParser::scheduleRead(const QString &path, BBCWeatherItem *item, const QString &type)
{
    ScheduleEntry entry;
    entry.path = path;
    entry.item = item;
    entry.type = type;

    m_scheduleMutex.lock();
    m_schedule.push(entry);
    m_scheduleMutex.unlock();

    ensureRunning();
}

bool BBCParser::workAvailable()
{
    QMutexLocker locker(&m_scheduleMutex);
    return !m_schedule.isEmpty();
}

void BBCParser::work()
{
    m_scheduleMutex.lock();
    ScheduleEntry entry = m_schedule.pop();
    m_scheduleMutex.unlock();

    QFile file(entry.path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QList<WeatherData> data = read(&file);

    if (!data.isEmpty() && !entry.item.isNull()) {
        if (entry.type == QLatin1StringView("bbcobservation")) {
            entry.item->setCurrentWeather(data.at(0));
        } else if (entry.type == QLatin1StringView("bbcforecast")) {
            entry.item->addForecastWeather(data);
        }

        Q_EMIT parsedFile();
    }
}

QList<WeatherData> BBCParser::read(QIODevice *device)
{
    m_list.clear();
    setDevice(device);

    while (!atEnd()) {
        readNext();

        if (isStartElement()) {
            if (name() == QLatin1StringView("rss"))
                readBBC();
            else
                raiseError(QObject::tr("The file is not a valid BBC answer."));
        }
    }

    return m_list;
}

void BBCParser::readUnknownElement()
{
    Q_ASSERT(isStartElement());

    while (!atEnd()) {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement())
            readUnknownElement();
    }
}

void BBCParser::readBBC()
{
    Q_ASSERT(isStartElement() && name() == QLatin1StringView("rss"));

    while (!atEnd()) {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement()) {
            if (name() == QLatin1StringView("channel"))
                readChannel();
            else
                readUnknownElement();
        }
    }
}

void BBCParser::readChannel()
{
    Q_ASSERT(isStartElement() && name() == QLatin1StringView("channel"));

    while (!atEnd()) {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement()) {
            if (name() == QLatin1StringView("item"))
                readItem();
            else
                readUnknownElement();
        }
    }
}

void BBCParser::readItem()
{
    Q_ASSERT(isStartElement() && name() == QLatin1StringView("item"));

    WeatherData item;

    while (!atEnd()) {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement()) {
            if (name() == QLatin1StringView("description"))
                readDescription(&item);
            else if (name() == QLatin1StringView("title"))
                readTitle(&item);
            else if (name() == QLatin1StringView("pubDate"))
                readPubDate(&item);
            else
                readUnknownElement();
        }
    }

    m_list.append(item);
}

void BBCParser::readDescription(WeatherData *data)
{
    Q_ASSERT(isStartElement() && name() == QLatin1StringView("description"));

    while (!atEnd()) {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement()) {
            readUnknownElement();
        }

        if (isCharacters()) {
            QString description = text().toString();
            QRegExp regExp;

            // Temperature
            regExp.setPattern(QStringLiteral("(Temperature:\\s*)(-?\\d+)(.C)"));
            int pos = regExp.indexIn(description);
            if (pos > -1) {
                QString value = regExp.cap(2);
                data->setTemperature(value.toDouble(), WeatherData::Celsius);
            }

            // Max Temperature
            regExp.setPattern(QStringLiteral("(Max Temp:\\s*)(-?\\d+)(.C)"));
            pos = regExp.indexIn(description);
            if (pos > -1) {
                QString value = regExp.cap(2);
                data->setMaxTemperature(value.toDouble(), WeatherData::Celsius);
            }

            // Min Temperature
            regExp.setPattern(QStringLiteral("(Min Temp:\\s*)(-?\\d+)(.C)"));
            pos = regExp.indexIn(description);
            if (pos > -1) {
                QString value = regExp.cap(2);
                data->setMinTemperature(value.toDouble(), WeatherData::Celsius);
            }

            // Wind direction
            regExp.setPattern(QStringLiteral("(Wind Direction:\\s*)([NESW]+)(,)"));
            pos = regExp.indexIn(description);
            if (pos > -1) {
                QString wind = regExp.cap(2);

                if (m_windDirections.contains(wind)) {
                    data->setWindDirection(m_windDirections.value(wind));
                } else {
                    mDebug() << "UNHANDLED WIND DIRECTION, PLEASE REPORT: " << wind;
                }
            }

            // Wind speed
            regExp.setPattern(QStringLiteral("(Wind Speed:\\s*)(\\d+)(mph)"));
            pos = regExp.indexIn(description);
            if (pos > -1) {
                QString speed = regExp.cap(2);
                data->setWindSpeed(speed.toFloat(), WeatherData::mph);
            }

            // Relative Humidity
            regExp.setPattern(QStringLiteral("(Relative Humidity:\\s*)(\\d+)(.,)"));
            pos = regExp.indexIn(description);
            if (pos > -1) {
                QString humidity = regExp.cap(2);
                data->setHumidity(humidity.toFloat());
            }

            // Pressure
            regExp.setPattern(QStringLiteral("(Pressure:\\s*)(\\d+mB|N/A)(, )([a-z ]+|N/A)(,)"));
            pos = regExp.indexIn(description);
            if (pos > -1) {
                QString pressure = regExp.cap(2);
                if (pressure != QLatin1StringView("N/A")) {
                    pressure.chop(2);
                    data->setPressure(pressure.toFloat() / 1000, WeatherData::Bar);
                }

                QString pressureDevelopment = regExp.cap(4);

                if (m_pressureDevelopments.contains(pressureDevelopment)) {
                    data->setPressureDevelopment(m_pressureDevelopments.value(pressureDevelopment));
                } else {
                    mDebug() << "UNHANDLED PRESSURE DEVELOPMENT, PLEASE REPORT: " << pressureDevelopment;
                }
            }

            // Visibility
            regExp.setPattern(QStringLiteral("(Visibility:\\s*)([^,]+)"));
            pos = regExp.indexIn(description);
            if (pos > -1) {
                QString visibility = regExp.cap(2);

                if (m_visibilityStates.contains(visibility.toLower())) {
                    data->setVisibilty(m_visibilityStates.value(visibility));
                } else {
                    mDebug() << "UNHANDLED VISIBILITY, PLEASE REPORT: " << visibility;
                }
            }
        }
    }
}

void BBCParser::readTitle(WeatherData *data)
{
    Q_ASSERT(isStartElement() && name() == QLatin1StringView("title"));

    while (!atEnd()) {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement()) {
            readUnknownElement();
        }

        if (isCharacters()) {
            QString title = text().toString();
            QRegExp regExp;

            // Condition
            regExp.setPattern(QStringLiteral(R"((^.*)(:\s*)([\w ]+)([\,\.]\s*))"));
            int pos = regExp.indexIn(title);
            if (pos > -1) {
                QString value = regExp.cap(3);

                if (m_dayConditions.contains(value)) {
                    // TODO: Switch for day/night
                    data->setCondition(m_dayConditions.value(value));
                } else {
                    mDebug() << "UNHANDLED BBC WEATHER CONDITION, PLEASE REPORT: " << value;
                }

                QString dayString = regExp.cap(1);
                auto dayOfWeek = (Qt::DayOfWeek)0;
                if (dayString.contains(QLatin1StringView("Monday"))) {
                    dayOfWeek = Qt::Monday;
                } else if (dayString.contains(QLatin1StringView("Tuesday"))) {
                    dayOfWeek = Qt::Tuesday;
                } else if (dayString.contains(QLatin1StringView("Wednesday"))) {
                    dayOfWeek = Qt::Wednesday;
                } else if (dayString.contains(QLatin1StringView("Thursday"))) {
                    dayOfWeek = Qt::Thursday;
                } else if (dayString.contains(QLatin1StringView("Friday"))) {
                    dayOfWeek = Qt::Friday;
                } else if (dayString.contains(QLatin1StringView("Saturday"))) {
                    dayOfWeek = Qt::Saturday;
                } else if (dayString.contains(QLatin1StringView("Sunday"))) {
                    dayOfWeek = Qt::Sunday;
                }
                QDate date = QDate::currentDate();
                date = date.addDays(-1);

                for (int i = 0; i < 7; i++) {
                    if (date.dayOfWeek() == dayOfWeek) {
                        data->setDataDate(date);
                    }
                    date = date.addDays(1);
                }
            }
        }
    }
}

void BBCParser::readPubDate(WeatherData *data)
{
    Q_ASSERT(isStartElement() && name() == QLatin1StringView("pubDate"));

    while (!atEnd()) {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement()) {
            readUnknownElement();
        }

        if (isCharacters()) {
            QString pubDate = text().toString();
            QRegExp regExp;

            regExp.setPattern(QStringLiteral(R"(([A-Za-z]+,\s+)(\d+)(\s+)([A-Za-z]+)(\s+)(\d{4,4})(\s+)(\d+)(:)(\d+)(:)(\d+)(\s+)([+-])(\d{2,2})(\d{2,2}))"));
            int pos = regExp.indexIn(pubDate);
            if (pos > -1) {
                QDateTime dateTime;
                QDate date;
                QTime time;

                dateTime.setTimeSpec(Qt::UTC);
                date.setDate(regExp.cap(6).toInt(), m_monthNames.value(regExp.cap(4)), regExp.cap(2).toInt());
                time.setHMS(regExp.cap(8).toInt(), regExp.cap(10).toInt(), regExp.cap(12).toInt());

                dateTime.setDate(date);
                dateTime.setTime(time);

                // Timezone
                if (regExp.cap(14) == QLatin1StringView("-")) {
                    dateTime = dateTime.addSecs(60 * 60 * regExp.cap(15).toInt());
                    dateTime = dateTime.addSecs(60 * regExp.cap(16).toInt());
                } else {
                    dateTime = dateTime.addSecs(-60 * 60 * regExp.cap(15).toInt());
                    dateTime = dateTime.addSecs(-60 * regExp.cap(16).toInt());
                }

                data->setPublishingTime(dateTime);
            }
        }
    }
}

#include "moc_BBCParser.cpp"
