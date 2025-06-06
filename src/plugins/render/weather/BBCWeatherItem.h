// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

#ifndef BBCWEATHERITEM_H
#define BBCWEATHERITEM_H

#include "WeatherItem.h"

class QString;
class QUrl;

namespace Marble
{

class BBCWeatherItem : public WeatherItem
{
    Q_OBJECT

public:
    explicit BBCWeatherItem(QObject *parent = nullptr);
    ~BBCWeatherItem() override;

    bool request(const QString &type) override;

    QString service() const override;
    void addDownloadedFile(const QString &url, const QString &type) override;

    QUrl observationUrl() const;
    QUrl forecastUrl() const;

    quint32 bbcId() const;
    void setBbcId(quint32 id);

    QString creditHtml() const;

private:
    quint32 m_bbcId;
    bool m_observationRequested;
    bool m_forecastRequested;
};

} // namespace Marble

#endif // BBCWEATHERITEM_H
