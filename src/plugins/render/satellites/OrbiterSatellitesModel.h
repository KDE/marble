//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Rene Kuettner <rene@bitkanal.net>
//

#ifndef MARBLE_ORBITERSATELLITESMODEL_H
#define MARBLE_ORBITERSATELLITESMODEL_H

#include <QtCore/QStringList>
#include <QtCore/QMap>

#include "TrackerPluginModel.h"

namespace Marble {

class MarbleClock;
class MarbleModel;
class GeoDataTreeModel;
class OrbiterSatellitesItem;

class OrbiterSatellitesModel : public TrackerPluginModel
{
    Q_OBJECT

public:
    OrbiterSatellitesModel( GeoDataTreeModel *treeModel,
                            const PluginManager *pluginManager,
                            const MarbleClock *clock );
    ~OrbiterSatellitesModel();

    void setPlanet( const QString &planetId );
    void parseFile( const QString &id, const QByteArray &file );

public Q_SLOTS:
    void updateVisibility();

protected:
    const MarbleClock *m_clock;
    QString m_lcPlanet;
    bool m_enabled;

};

} // namespace Marble

#endif // MARBLE_ORBITERSATELLITESMODEL_H

