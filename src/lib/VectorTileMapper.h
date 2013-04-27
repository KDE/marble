/*
 This file is part of the Marble Virtual Globe.

 This program is free software licensed under the GNU LGPL. You can
 find a copy of this license in LICENSE.txt in the top directory of
 the source code.

 Copyright 2012 Ander Pijoan <ander.pijoan@deusto.es>
*/

#ifndef MARBLE_VECTORTILEMAPPER_H
#define MARBLE_VECTORTILEMAPPER_H

#include <QtCore/QObject>

#include "GeoDataDocument.h"

#include <QtCore/QThreadPool>


namespace Marble
{

class StackedTileLoader;
class TileId;
class ViewportParams;

class VectorTileMapper : public QObject
{
    Q_OBJECT

public:
    explicit VectorTileMapper( StackedTileLoader *tileLoader );

    void mapTexture( const ViewportParams *viewport, int tileZoomLevel );

public Q_SLOTS:
    void updateTile(TileId const & tileId, GeoDataDocument *document, QString const & format );

Q_SIGNALS:
    void tileCompleted( TileId const & tileId, GeoDataDocument * document, QString const & format );

private:
    void mapTexture( int tileZoomLevel, unsigned int minX, unsigned int minY, unsigned int maxX, unsigned int maxY );

    unsigned int lon2tileX( qreal lon, unsigned int maxTileX );
    unsigned int lat2tileY( qreal lat, unsigned int maxTileY );

private:
    class RenderJob;
    StackedTileLoader *const m_tileLoader;
    QThreadPool m_threadPool;
};


class VectorTileMapper::RenderJob : public QObject, public QRunnable
{
    Q_OBJECT

public:

    RenderJob(StackedTileLoader *tileLoader, int tileLevel,
              unsigned int minTileX, unsigned int minTileY, unsigned int maxTileX, unsigned int maxTileY );

    virtual void run();

Q_SIGNALS:
    void tileCompleted( TileId const & tileId, GeoDataDocument * document, QString const & format );

private:
    StackedTileLoader *const m_tileLoader;
    const int m_tileLevel;

    // Variables for storing current screen tiles
    unsigned int m_minTileX;
    unsigned int m_minTileY;
    unsigned int m_maxTileX;
    unsigned int m_maxTileY;
};


}

#endif // MARBLE_VECTORTILEMAPPER_H
