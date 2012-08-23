//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
//

#ifndef MARBLE_EARTHSATELLITESMODEL_H
#define MARBLE_EARTHSATELLITESMODEL_H

#include "TrackerPluginModel.h"


namespace Marble {

class MarbleClock;

class EarthSatellitesModel : public TrackerPluginModel
{
    Q_OBJECT
public:
    EarthSatellitesModel( GeoDataTreeModel *treeModel,
                          const PluginManager *pluginManager,
                          const MarbleClock *clock );
    void parseFile( const QString &id, const QByteArray &file );

private:
    const MarbleClock *m_clock;
};

} // namespace Marble

#endif // MARBLE_EARTHSATELLITESMODEL_H
