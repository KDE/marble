/*
 This file is part of the Marble Virtual Globe.

 This program is free software licensed under the GNU LGPL. You can
 find a copy of this license in LICENSE.txt in the top directory of
 the source code.

 Copyright 2007      Torsten Rahn     <tackat@kde.org>
 Copyright 2011      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
*/

#ifndef MARBLE_VECTORTILEMAPPER_H
#define MARBLE_VECTORTILEMAPPER_H

#include "TextureMapperInterface.h"

#include "MarbleGlobal.h"
#include "TileId.h"
#include "GeoDataDocument.h"

#include <QtCore/QThreadPool>
#include <QtGui/QImage>


namespace Marble
{

class StackedTileLoader;

class VectorTileMapper : public QObject, public TextureMapperInterface
{
    Q_OBJECT

public:
    explicit VectorTileMapper( StackedTileLoader *tileLoader );
    ~VectorTileMapper();

    virtual void mapTexture( GeoPainter *painter,
                             const ViewportParams *viewport,
                             const QRect &dirtyRect,
                             TextureColorizer *texColorizer );

    virtual void setRepaintNeeded();

public Q_SLOTS:

    void updateTile(TileId const & tileId, GeoDataDocument *document, QString const & format );

Q_SIGNALS:
    void tileCompleted( TileId const & tileId, GeoDataDocument * document, QString const & format );

private:
    void mapTexture( const ViewportParams *viewport, MapQuality mapQuality );

private:
    class RenderJob;
    StackedTileLoader *const m_tileLoader;
    bool m_repaintNeeded;
    int m_radius;
    QImage m_canvasImage;
    QThreadPool m_threadPool;
};


class VectorTileMapper::RenderJob : public QObject, public QRunnable
{
    Q_OBJECT

public:

    RenderJob( StackedTileLoader *tileLoader, int tileLevel, const ViewportParams *viewport );

    virtual void run();

    int lon2tilex(double lon, int z);

    int lat2tiley(double lat, int z);

Q_SIGNALS:
    void tileCompleted( TileId const & tileId, GeoDataDocument * document, QString const & format );

private:
    StackedTileLoader *const m_tileLoader;
    const int m_tileLevel;
    const ViewportParams *const m_viewport;
};


}

#endif // MARBLE_VECTORTILEMAPPER_H
