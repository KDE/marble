//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Konrad Enzensberger <e.konrad@mpegcode.com>
// Copyright 2011      Dennis Nienhüser <nienhueser@kde.org>
// Copyright 2012      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_ROUTESIMULATIONPOSITIONPROVIDERPLUGIN_H
#define MARBLE_ROUTESIMULATIONPOSITIONPROVIDERPLUGIN_H

#include "PositionProviderPlugin.h"

#include "GeoDataLineString.h"
#include "GeoDataCoordinates.h"

#include <QDateTime>
#include <QTimer>

namespace Marble
{

class MarbleModel;

class RouteSimulationPositionProviderPlugin: public PositionProviderPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::PositionProviderPluginInterface )

public:
    explicit RouteSimulationPositionProviderPlugin( MarbleModel *marbleModel );
    virtual ~RouteSimulationPositionProviderPlugin();

    // Implementing PluginInterface
    QString name() const override;
    QString nameId() const override;
    QString guiString() const override;
    QString version() const override;
    QString description() const override;
    QString copyrightYears() const override;
    QVector<PluginAuthor> pluginAuthors() const override;
    QIcon icon() const override;
    void initialize() override;
    bool isInitialized() const override;
    qreal speed() const override;
    qreal direction() const override;
    QDateTime timestamp() const override;

    // Implementing PositionProviderPlugin
    PositionProviderPlugin * newInstance() const override;

    // Implementing PositionProviderPluginInterface
    PositionProviderStatus status() const override;
    GeoDataCoordinates position() const override;
    GeoDataAccuracy accuracy() const override;

private Q_SLOTS:
    /** Regular (each second) position and status update */
    void update();
    void updateRoute();

private:
    GeoDataCoordinates addNoise(const Marble::GeoDataCoordinates &,const Marble::GeoDataAccuracy &) const;
    qreal addNoise(qreal bearing) const;
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
