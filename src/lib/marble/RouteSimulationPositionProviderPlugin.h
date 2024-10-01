// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Konrad Enzensberger <e.konrad@mpegcode.com>
// SPDX-FileCopyrightText: 2011 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_ROUTESIMULATIONPOSITIONPROVIDERPLUGIN_H
#define MARBLE_ROUTESIMULATIONPOSITIONPROVIDERPLUGIN_H

#include "PositionProviderPlugin.h"

#include "GeoDataCoordinates.h"
#include "GeoDataLineString.h"

#include <QDateTime>
#include <QTimer>

namespace Marble
{

class MarbleModel;

class RouteSimulationPositionProviderPlugin : public PositionProviderPlugin
{
    Q_OBJECT
    Q_INTERFACES(Marble::PositionProviderPluginInterface)

public:
    explicit RouteSimulationPositionProviderPlugin(MarbleModel *marbleModel, QObject *parent = nullptr);
    ~RouteSimulationPositionProviderPlugin() override;

    // Implementing PluginInterface
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
    qreal speed() const override;
    qreal direction() const override;
    QDateTime timestamp() const override;

    // Implementing PositionProviderPlugin
    PositionProviderPlugin *newInstance() const override;

    // Implementing PositionProviderPluginInterface
    PositionProviderStatus status() const override;
    GeoDataCoordinates position() const override;
    GeoDataAccuracy accuracy() const override;

private Q_SLOTS:
    /** Regular (each second) position and status update */
    void update();
    void updateRoute();

private:
    GeoDataCoordinates addNoise(const Marble::GeoDataCoordinates &, const Marble::GeoDataAccuracy &) const;
    static qreal addNoise(qreal bearing);
    void changeStatus(PositionProviderStatus status);

    MarbleModel *const m_marbleModel;
    int m_currentIndex;
    PositionProviderStatus m_status;
    GeoDataLineString m_lineString;
    GeoDataLineString m_lineStringInterpolated;
    GeoDataCoordinates m_currentPosition;
    GeoDataCoordinates m_currentPositionWithNoise;
    QDateTime m_currentDateTime;
    qreal m_speed;
    qreal m_direction;
    qreal m_directionWithNoise;
    QTimer m_updateTimer;
};

}

#endif // MARBLE_ROUTESIMULATIONPOSITIONPROVIDERPLUGIN_H
