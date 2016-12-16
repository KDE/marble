//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#ifndef EARTHQUAKEPLUGIN_H
#define EARTHQUAKEPLUGIN_H

#include "AbstractDataPlugin.h"
#include "DialogConfigurationInterface.h"

#include <QDateTime>

namespace Ui
{
    class EarthquakeConfigWidget;
}

namespace Marble
{

class EarthquakePlugin : public AbstractDataPlugin, public DialogConfigurationInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.EarthquakePlugin")

    Q_INTERFACES( Marble::RenderPluginInterface )
    Q_INTERFACES( Marble::DialogConfigurationInterface )

    MARBLE_PLUGIN( EarthquakePlugin )

public:
    EarthquakePlugin();

    explicit EarthquakePlugin( const MarbleModel *marbleModel );

    void initialize() override;

    QString name() const override;

    QString guiString() const override;

    QString nameId() const override;

    QString version() const override;

    QString description() const override;

    QString copyrightYears() const override;

    QVector<PluginAuthor> pluginAuthors() const override;

    QIcon icon() const override;

    QDialog *configDialog() override;

    /**
     * @return: The settings of the item.
     */
    QHash<QString,QVariant> settings() const override;

    /**
     * Set the settings of the item.
     */
    void setSettings( const QHash<QString,QVariant> &settings ) override;

public Q_SLOTS:
    void readSettings();
    void writeSettings();
    void updateModel();

private:
    Ui::EarthquakeConfigWidget *m_ui;
    QDialog *m_configDialog;
    qreal m_minMagnitude;
    QDateTime m_startDate;
    QDateTime m_endDate;
    int m_pastDays;
    bool m_timeRangeNPastDays;
    int m_numResults;
    int m_maximumNumberOfItems;
    
private Q_SLOTS:
    void validateDateRange();
};

}

#endif // EARTHQUAKEPLUGIN_H
