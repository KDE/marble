//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Rene Kuettner <rene@bitkanal.net>
//

#ifndef MARBLE_PLANETARYSATELLITESMODEL_H
#define MARBLE_PLANETARYSATELLITESMODEL_H

// FIXME create an abstract class for other tracker plugins
#include "TrackerPluginModel.h"

namespace Marble {

class MarbleClock;
class MarbleModel;
class GeoDataTreeModel;

class PlanetarySatellitesModel : public TrackerPluginModel
{
    Q_OBJECT

public:
    PlanetarySatellitesModel( GeoDataTreeModel *treeModel,
                              const PluginManager *pluginManager,
                              const MarbleClock *clock );
    ~PlanetarySatellitesModel();

    void setPlanet( const QString &lcPlanet );
    void parseFile( const QString &id, const QByteArray &file );

protected:
    const MarbleClock *m_clock;
    QString m_lcPlanet;
    bool m_enabled;
};

} // namespace Marble

#endif // MARBLE_PLANETARYSATELLITESMODEL_H

