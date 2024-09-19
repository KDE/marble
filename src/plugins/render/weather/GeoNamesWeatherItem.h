// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Valery Kharitonov <kharvd@gmail.com>
//

#ifndef GEONAMESWEATHERITEM_H
#define GEONAMESWEATHERITEM_H

#include "WeatherItem.h"

namespace Marble
{

class GeoNamesWeatherItem : public WeatherItem
{
    Q_OBJECT

public:
    explicit GeoNamesWeatherItem(QObject *parent = nullptr);
    ~GeoNamesWeatherItem() override;

    void addDownloadedFile(const QString &url, const QString &type) override;

    QString service() const override;
    QString creditHtml() const;
};

} // namespace Marble

#endif // GEONAMESWEATHERITEM_H
