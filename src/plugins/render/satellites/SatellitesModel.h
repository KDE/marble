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

#include "AbstractDataPluginModel.h"

namespace Marble {

class SatellitesModel : public AbstractDataPluginModel
{
    Q_OBJECT

public:
    explicit SatellitesModel(Marble::PluginManager *pluginManager, QObject *parent = 0);

    /**
     * @brief Clear the list of items and trigger a download of every TLE in datasets
     * The TLEs will be downloaded from CelesTrak
     */
    void refreshItems( const QStringList &tleList );

protected:
    void getAdditionalItems(const Marble::GeoDataLatLonAltBox& box, const Marble::MarbleModel* model, qint32 number = 10);
    void parseFile(const QByteArray &file);
};

}

#endif // MARBLE_SATELLITESMODEL_H
