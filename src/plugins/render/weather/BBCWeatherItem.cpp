// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "BBCWeatherItem.h"

// Marble
#include "BBCParser.h"

// Qt
#include <QUrl>

using namespace Marble;
/* TRANSLATOR Marble::BBCWeatherItem */

BBCWeatherItem::BBCWeatherItem(QObject *parent)
    : WeatherItem(parent)
    , m_observationRequested(false)
    , m_forecastRequested(false)
{
}

BBCWeatherItem::~BBCWeatherItem() = default;

bool BBCWeatherItem::request(const QString &type)
{
    if (type == QLatin1StringView("bbcobservation")) {
        if (!m_observationRequested) {
            m_observationRequested = true;
            return true;
        }
    } else if (type == QLatin1StringView("bbcforecast")) {
        if (!m_forecastRequested) {
            m_forecastRequested = true;
            return true;
        }
    }
    return false;
}

QString BBCWeatherItem::service() const
{
    return QStringLiteral("BBC");
}

void BBCWeatherItem::addDownloadedFile(const QString &url, const QString &type)
{
    if (type == QLatin1StringView("bbcobservation") || type == QLatin1StringView("bbcforecast")) {
        BBCParser::instance()->scheduleRead(url, this, type);
    }
}

quint32 BBCWeatherItem::bbcId() const
{
    return m_bbcId;
}

void BBCWeatherItem::setBbcId(quint32 id)
{
    m_bbcId = id;
    setId(QLatin1StringView("bbc") + QString::number(id));
}

QUrl BBCWeatherItem::observationUrl() const
{
    return QUrl(QStringLiteral("http://newsrss.bbc.co.uk/weather/forecast/%1/ObservationsRSS.xml").arg(QString::number(bbcId())));
}

QUrl BBCWeatherItem::forecastUrl() const
{
    return QUrl(QStringLiteral("http://newsrss.bbc.co.uk/weather/forecast/%1/Next3DaysRSS.xml").arg(QString::number(bbcId())));
}

QString BBCWeatherItem::creditHtml() const
{
    return tr(R"(Supported by <a href="https://www.bbc.co.uk/blogs/bbcbackstage" target="_BLANK">backstage.bbc.co.uk</a>.<br>Weather data from UK MET Office)");
}

#include "moc_BBCWeatherItem.cpp"
