// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Ralf Habacker <ralf.habacker@freenet.de>

#ifndef FLIGHTGEARPOSITIONPROVIDERPLUGIN_H
#define FLIGHTGEARPOSITIONPROVIDERPLUGIN_H

#include "PositionProviderPlugin.h"

#include "GeoDataAccuracy.h"
#include "GeoDataCoordinates.h"
#include <QDateTime>

class QUdpSocket;

namespace Marble
{

class FlightGearPositionProviderPlugin : public PositionProviderPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.FlightGearPositionProviderPlugin")
    Q_INTERFACES(Marble::PositionProviderPluginInterface)

public:
    FlightGearPositionProviderPlugin();
    ~FlightGearPositionProviderPlugin() override;

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

private Q_SLOTS:
    void readPendingDatagrams();

private:
    void parseNmeaSentence(const QString &sentence);
    static double parsePosition(const QString &value, bool isNegative);
    QUdpSocket *m_socket = nullptr;
    PositionProviderStatus m_status;
    GeoDataCoordinates m_position;
    GeoDataAccuracy m_accuracy;
    qreal m_speed = 0.0;
    qreal m_track = 0.0;
    QDateTime m_timestamp;
};

}

#endif // FLIGHTGEARPOSITIONPROVIDERPLUGIN_H
