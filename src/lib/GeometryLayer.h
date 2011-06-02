//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Patrick Spendrin  <ps_ml@gmx.de>
// Copyright 2010      Thibaut Gridel  <tgridel@free.fr>
//

#ifndef MARBLE_GEOMETRYLAYER_H
#define MARBLE_GEOMETRYLAYER_H

// Marble
#include "LayerInterface.h"
#include "GeoDataFeature.h"

// Qt
class QAbstractItemModel;

namespace Marble
{
class GeoDataDocument;
class GeoPainter;
class ViewportParams;
class GeometryLayerPrivate;

class GeometryLayer : public QObject, public LayerInterface
{
    Q_OBJECT
public:
    GeometryLayer( QAbstractItemModel *model );
    ~GeometryLayer();

    virtual QStringList renderPosition() const;

    virtual bool render( GeoPainter *painter, ViewportParams *viewport,
                         const QString& renderPos = "NONE", GeoSceneLayer * layer = 0 );
    
    static int s_defaultZValues[GeoDataFeature::LastIndex];
    static int s_defaultLODValues[GeoDataFeature::LastIndex];
    static bool s_defaultValuesInitialized;
    static int s_defaultZValue;

public Q_SLOTS:
    void invalidateScene();

private:
    GeometryLayerPrivate *d;
    
    static void initializeDefaultValues();
};

} // namespace Marble
#endif // MARBLE_GEOMETRYLAYER_H
