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
    virtual QString name() const;
    virtual QString nameId() const;
    virtual QString guiString() const;
    virtual QString version() const;
    virtual QString description() const;
    virtual QString copyrightYears() const;
    virtual QList<PluginAuthor> pluginAuthors() const;
    virtual QIcon icon() const;
    virtual void initialize();
    virtual bool isInitialized() const;
    virtual qreal speed() const;
    virtual qreal direction() const;
    virtual QDateTime timestamp() const;

    // Implementing PositionProviderPlugin
    virtual PositionProviderPlugin * newInstance() const;

    // Implementing PositionProviderPluginInterface
    virtual PositionProviderStatus status() const;
    virtual GeoDataCoordinates position() const;
    virtual GeoDataAccuracy accuracy() const;

private Q_SLOTS:
    /** Regular (each second) position and status update */
    void update();
    void updateRoute();

private:
    GeoDataCoordinates addNoise(const Marble::GeoDataCoordinates &,const Marble::GeoDataAccuracy &) const;
    qreal addNoise(qreal bearing) const;

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
