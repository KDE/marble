// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Eckhart Wörner <ewoerner@kde.org>
//

#ifndef GPSDPOSITIONPROVIDERPLUGIN_H
#define GPSDPOSITIONPROVIDERPLUGIN_H

#include "GeoDataAccuracy.h"
#include "GeoDataCoordinates.h"
#include "PositionProviderPlugin.h"

#include <QDateTime>

#include <libgpsmm.h>

namespace Marble
{

class GpsdThread;

class GpsdPositionProviderPlugin : public PositionProviderPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.GpsdPositionProviderPlugin")
    Q_INTERFACES(Marble::PositionProviderPluginInterface)

public:
    GpsdPositionProviderPlugin();
    ~GpsdPositionProviderPlugin() override;

    QString name() const override;
    QString nameId() const override;
    QString guiString() const override;
    QString version() const override;
    QString description() const override;
    QString copyrightYears() const override;
    QList<PluginAuthor> pluginAuthors() const override;
    QIcon icon() const override;
    void initialize() override;
    bool isInitialized() const override;

    PositionProviderPlugin *newInstance() const override;

    PositionProviderStatus status() const override;
    GeoDataCoordinates position() const override;
    GeoDataAccuracy accuracy() const override;
    QString error() const override;
    qreal speed() const override;
    qreal direction() const override;
    QDateTime timestamp() const override;

private:
    GpsdThread *m_thread;
    PositionProviderStatus m_status;
    GeoDataCoordinates m_position;
    GeoDataAccuracy m_accuracy;
    qreal m_speed;
    qreal m_track;
    QDateTime m_timestamp;

private Q_SLOTS:
    void update(gps_data_t data);
};

}

#endif
