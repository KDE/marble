//
// This file is part of the Marble Desktop Globe.
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

// Qt


namespace Marble
{
class GeoDataDocument;
class GeoPainter;
class ViewportParams;
class GeometryLayerPrivate;

class GeometryLayer : public LayerInterface
{
 public:
    GeometryLayer( GeoDataDocument *document );
    ~GeometryLayer();

    virtual QStringList renderPosition() const;

    virtual bool render( GeoPainter *painter, ViewportParams *viewport,
       const QString& renderPos = "NONE", GeoSceneLayer * layer = 0 );

 private:
    GeometryLayerPrivate *d;
};

} // namespace Marble
#endif // MARBLE_GEOMETRYLAYER_H
