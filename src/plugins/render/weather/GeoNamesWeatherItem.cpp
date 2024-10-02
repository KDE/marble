// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Valery Kharitonov <kharvd@gmail.com>
//

#include "GeoNamesWeatherItem.h"

using namespace Marble;

GeoNamesWeatherItem::GeoNamesWeatherItem(QObject *parent)
    : WeatherItem(parent)
{
}

GeoNamesWeatherItem::~GeoNamesWeatherItem() = default;

void GeoNamesWeatherItem::addDownloadedFile(const QString &url, const QString &type)
{
    Q_UNUSED(url)
    Q_UNUSED(type)
}

QString GeoNamesWeatherItem::service() const
{
    return QStringLiteral("GeoNames");
}

QString GeoNamesWeatherItem::creditHtml() const
{
    return tr(R"(Supported by <a href="https://www.geonames.org/export/JSON-webservices.html" target="_BLANK">geonames.org</a>)");
}

#include "moc_GeoNamesWeatherItem.cpp"
