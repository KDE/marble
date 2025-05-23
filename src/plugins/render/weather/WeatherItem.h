// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
// SPDX-FileCopyrightText: 2012 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#ifndef WEATHERITEM_H
#define WEATHERITEM_H

// Marble
#include "AbstractDataPluginItem.h"

// Qt
#include <QMap>

class QString;
class QAction;
class QDate;

namespace Marble
{

class MarbleWidget;
class WeatherData;

class WeatherItemPrivate;

/**
 * This is the class painting a weather item on the screen. So it is a subclass of
 * AbstractDataItem.
 */
class WeatherItem : public AbstractDataPluginItem
{
    Q_OBJECT

    Q_PROPERTY(QString station READ stationName WRITE setStationName NOTIFY stationNameChanged)
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
    Q_PROPERTY(QString image READ image NOTIFY imageChanged)
    Q_PROPERTY(double temperature READ temperature NOTIFY temperatureChanged)

public:
    explicit WeatherItem(QObject *parent = nullptr);
    explicit WeatherItem(MarbleWidget *widget, QObject *parent = nullptr);
    ~WeatherItem() override;

    QAction *action() override;

    /**
     * Test if the item wants to request @p type again.
     */
    virtual bool request(const QString &type);

    /**
     * Returns the provider of the weather information.
     */
    virtual QString service() const = 0;

    bool initialized() const override;

    void addDownloadedFile(const QString &url, const QString &type) override = 0;

    bool operator<(const AbstractDataPluginItem *other) const override;

    QString stationName() const;
    void setStationName(const QString &name);

    WeatherData currentWeather() const;
    void setCurrentWeather(const WeatherData &weather);

    QMap<QDate, WeatherData> forecastWeather() const;
    void setForecastWeather(const QMap<QDate, WeatherData> &forecasts);

    /**
     * Adds additional forecasts to the list. If there are multiple forecasts for one day,
     * it will choose the most recent (as of pubDate).
     */
    void addForecastWeather(const QList<WeatherData> &forecasts);

    quint8 priority() const;
    void setPriority(quint8 priority);

    void setSettings(const QHash<QString, QVariant> &settings) override;

    void setMarbleWidget(MarbleWidget *widget);

    QList<QAction *> actions() override;

    QString description() const;

    QString image() const;

    double temperature() const;

public Q_SLOTS:
    void openBrowser();

Q_SIGNALS:
    void stationNameChanged();

    void descriptionChanged();

    void imageChanged();

    void temperatureChanged();

private:
    Q_DISABLE_COPY(WeatherItem)
    WeatherItemPrivate *const d;
    friend class WeatherItemPrivate;
    QString createFromTemplate(const QString &templateHtml);
};

} // namespace Marble

#endif // WEATHERITEM_H
