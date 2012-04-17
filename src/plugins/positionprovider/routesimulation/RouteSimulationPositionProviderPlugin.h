//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Konrad Enzensberger <e.konrad@mpegcode.com>
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
// Copyright 2012      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_ROUTESIMULATIONPOSITIONPROVIDERPLUGIN_H
#define MARBLE_ROUTESIMULATIONPOSITIONPROVIDERPLUGIN_H

#include "PositionProviderPlugin.h"
#include "GeoDataLineString.h"

namespace Marble
{

class RouteSimulationPositionProviderPlugin: public PositionProviderPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::PositionProviderPluginInterface )

public:
    RouteSimulationPositionProviderPlugin();
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

private:
    int m_currentIndex;
    PositionProviderStatus m_status;
    GeoDataLineString m_lineString;
};

}

#endif // MARBLE_ROUTESIMULATIONPOSITIONPROVIDERPLUGIN_H
