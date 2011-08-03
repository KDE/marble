//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
//

#ifndef MARBLE_SATELLITESPLUGIN_H
#define MARBLE_SATELLITESPLUGIN_H

#include <QtCore/QObject>

#include "RenderPlugin.h"
#include "sgp4/sgp4unit.h"

namespace Marble
{
class GeoDataCoordinates;

/**
 * @brief This plugin displays satellites and their orbits.
 *
 */
class SatellitesPlugin : public RenderPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::RenderPluginInterface )
    MARBLE_PLUGIN( SatellitesPlugin )

public:
    SatellitesPlugin();
    
    QStringList backendTypes() const;
    QString renderPolicy() const;
    QStringList renderPosition() const;
    QString name() const;
    QString guiString() const;
    QString nameId() const;
    QString description() const;
    QIcon icon() const;

    void initialize();
    bool isInitialized() const;
    bool render( GeoPainter *painter, ViewportParams *viewport, const QString &renderPos, GeoSceneLayer *layer = 0 );

    /**
     * @brief Create a GeoDataCoordinates object from cartesian coordinates(GEI)
     * @param x x coordinate in km
     * @param y y coordinate in km
     * @param z z coordinate in km
     */
    GeoDataCoordinates fromCartesian( double x, double y, double z );

private:
    bool m_isInitialized;
    //TODO: use a data structure more appropriate for searching by satellite name
    QHash<QString, elsetrec> m_satHash;
};

}

#endif // MARBLE_SATELLITESPLUGIN_H
