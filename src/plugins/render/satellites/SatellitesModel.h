//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
//

#ifndef MARBLE_SATELLITESMODEL_H
#define MARBLE_SATELLITESMODEL_H

#include "TrackerPluginModel.h"


namespace Marble {

class SatellitesModel : public TrackerPluginModel
{
    Q_OBJECT
public:
    SatellitesModel( GeoDataTreeModel *treeModel, const PluginManager *pluginManager );
    void parseFile( const QString &id, const QByteArray &file );
};

}

#endif // MARBLE_SATELLITESMODEL_H
