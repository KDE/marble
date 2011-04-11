//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//


// local
#include"GLTextureMapper.h"

// posix
#include <cmath>

// Qt
#include <QtCore/qmath.h>
#include <QtCore/QTimer>
#include <QtGui/QImage>
#include <QtGui/QPainter>
#include <QtOpenGL/QGLContext>

// Marble
#include "Projections/AbstractProjection.h"
#include "GeoPainter.h"
#include "MarbleDebug.h"
#include "StackedTileLoader.h"
#include "ViewParams.h"
#include "ViewportParams.h"
#include "TextureColorizer.h"
#include "TileLoader.h"
#include "StackedTile.h"
#include "MathHelper.h"

using namespace Marble;



class GLTextureMapper::Private
{
public:
    Private( QGLContext *glContext, TileLoader *tileLoader );

    QGLContext *const m_glContext;
    QMap<QString, Tile> m_visibleTiles;

    bool m_repaintNeeded;
    int m_previousLevel;

    TileLoader *const m_srtmLoader;
};


GLTextureMapper::Private::Private( QGLContext *glContext, TileLoader *tileLoader )
    : m_glContext( glContext)
    , m_visibleTiles()
    , m_repaintNeeded( true )
    , m_previousLevel( -1 )
    , m_srtmLoader( tileLoader )
{
}


class GLTextureMapper::Tile
{
public:
    Tile();
    Tile( const TileId &id, GLuint glTexture );

    TileId id() const { return m_id; }
    GLuint glTexture() const { return m_glTexture; }
    bool operator==( const Tile &other );

private:
    TileId m_id;
    GLuint m_glTexture;
};


GLTextureMapper::Tile::Tile()
    : m_id()
    , m_glTexture( 0 )
{
}

GLTextureMapper::Tile::Tile( const Marble::TileId &id, GLuint glTexture )
    : m_id( id )
    , m_glTexture( glTexture )
{
}

bool GLTextureMapper::Tile::operator==(const Tile &other)
{
    return m_id == other.id() && m_glTexture == other.glTexture();
}



GLTextureMapper::GLTextureMapper( StackedTileLoader *tileLoader, QGLContext *glContext, TileLoader *srtmLoader, QObject *parent )
    : AbstractScanlineTextureMapper( tileLoader, parent )
    , d( new Private( glContext, srtmLoader ) )
{
    connect( m_tileLoader, SIGNAL( tileUpdateAvailable( const TileId & ) ),
             this,         SLOT( updateTile( const TileId & ) ) );
    connect( m_tileLoader, SIGNAL( tileUpdatesAvailable() ),
             this,         SIGNAL( tileUpdatesAvailable() ) );
}

GLTextureMapper::~GLTextureMapper()
{
    delete d;
}

void GLTextureMapper::mapTexture( GeoPainter *painter,
                                  ViewParams *viewParams,
                                  const QRect &dirtyRect,
                                  TextureColorizer *texColorizer )
{
    if ( viewParams->radius() <= 0 )
        return;

    const bool highQuality  = ( viewParams->mapQuality() == HighQuality
                || viewParams->mapQuality() == PrintQuality );

    m_tilePosX = 65535;
    m_tilePosY = 65535;

    viewParams->viewport()->activateRelief( d->m_srtmLoader );

    painter->endNativePainting();

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    const Quaternion axis = viewParams->viewport()->planetAxis();
    const qreal scale = sqrt( axis.v[Q_X]*axis.v[Q_X] + axis.v[Q_Y]*axis.v[Q_Y] + axis.v[Q_Z]*axis.v[Q_Z] );
    const qreal angle = - 2 * acos( axis.v[Q_W] ) * RAD2DEG;
    const qreal ax = axis.v[Q_X] / scale;
    const qreal ay = axis.v[Q_Y] / scale;
    const qreal az = axis.v[Q_Z] / scale;

    // Calculate translation of center point
    qreal centerLon, centerLat;
    viewParams->centerCoordinates( centerLon, centerLat );

    glLoadIdentity();
    glTranslated( 0, 0, viewParams->viewport()->radius( centerLon, centerLat ) );
    glRotated( -viewParams->viewport()->tilt(), 1, 0, 0 );
    glTranslated( 0, 0, -viewParams->viewport()->radius( centerLon, centerLat ) );

    if ( viewParams->projection() == Spherical ) {
        glRotated( angle, ax, ay, az );
    } else {
        glTranslated( -centerLon * viewParams->viewport()->radius( centerLon, centerLat ),
                      -centerLat * viewParams->viewport()->radius( centerLon, centerLat ),
                      0 );
    }

    foreach ( const Tile &tile, d->m_visibleTiles.values() ) {
        static const int NumLatitudes = 10;
        static const int NumLongitudes = 10;

        const int numXTiles = m_tileLoader->tileColumnCount( tile.id().zoomLevel() );
        const int numYTiles = m_tileLoader->tileRowCount( tile.id().zoomLevel() );

        glBindTexture( GL_TEXTURE_2D, tile.glTexture() );

        for (int row = 0; row < NumLatitudes; row++) {
            glBegin( GL_TRIANGLE_STRIP );
            for (int col = 0; col <= NumLongitudes; col++){
                const qreal x  = (tile.id().x() * NumLongitudes + col    ) * 1.0 / (NumLongitudes*numXTiles);
                const qreal y1 = (tile.id().y() * NumLatitudes  + row    ) * 1.0 / (NumLatitudes *numYTiles);
                const qreal y2 = (tile.id().y() * NumLatitudes  + row + 1) * 1.0 / (NumLatitudes *numYTiles);

                qreal lon, lat;
                qreal w0, w1, w2;

                geoCoordinates( x, y1, lon, lat );
                viewParams->currentProjection()->vertexCoordinates( lon, lat, w0, w1, w2 );
                lon *= DEG2RAD;
                lat *= DEG2RAD;
                w0 *= viewParams->viewport()->radius( lon, lat );
                w1 *= viewParams->viewport()->radius( lon, lat );
                w2 *= viewParams->viewport()->radius( lon, lat );

                glTexCoord2d(col*1.0/NumLatitudes, row*1.0/NumLongitudes);
                glVertex3d(w0, w1, w2);

                geoCoordinates( x, y2, lon, lat );
                viewParams->currentProjection()->vertexCoordinates( lon, lat, w0, w1, w2 );
                lon *= DEG2RAD;
                lat *= DEG2RAD;
                w0 *= viewParams->viewport()->radius( lon, lat );
                w1 *= viewParams->viewport()->radius( lon, lat );
                w2 *= viewParams->viewport()->radius( lon, lat );

                glTexCoord2d(col*1.0/NumLatitudes, (row+1)*1.0/NumLongitudes);
                glVertex3d(w0, w1, w2);
            }
            glEnd();
        }
    }

    painter->beginNativePainting();

    viewParams->viewport()->activateRelief( 0 );
}

void GLTextureMapper::setRepaintNeeded()
{
    d->m_repaintNeeded = true;
}

void GLTextureMapper::setViewport( const ViewportParams *viewport )
{
    AbstractScanlineTextureMapper::setViewport( viewport );

    // mark all tiles as unused
    m_tileLoader->resetTilehash();

    const int width = viewport->size().width();
    const int height = viewport->size().height();

    glViewport( 0, 0, width, height );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( -0.5*width, 0.5*width, -0.5*height, 0.5*height, -256000000/M_PI*80, 256/M_PI*32 );

    glMatrixMode( GL_MODELVIEW );

    const GeoDataLatLonAltBox bbox = viewport->viewLatLonAltBox();

    const int numXTiles = m_tileLoader->tileColumnCount( tileZoomLevel() );
    const int numYTiles = m_tileLoader->tileRowCount( tileZoomLevel() );

    qreal topLeftX, topLeftY, botRightX, botRightY;
    projectionCoordinates( bbox.west(), bbox.north(), topLeftX, topLeftY );
    projectionCoordinates( bbox.east(), bbox.south(), botRightX, botRightY );
    const int startXTile =       numXTiles * topLeftX;
    const int startYTile =       numYTiles * topLeftY;
          int endXTile   = 1.5 + numXTiles * botRightX;
          int endYTile   = 1.5 + numYTiles * botRightY;

    if ( endXTile <= startXTile )
        endXTile += numXTiles;
    if ( endYTile <= startYTile )
        endYTile += numYTiles;

    QList<QString> invisibleTiles = d->m_visibleTiles.keys();

    for (int i = startXTile; i < endXTile; ++i)
    {
        for (int j = startYTile; j < endYTile; ++j)
        {
            const TileId id( 0, tileZoomLevel(), i % numXTiles, j % numYTiles );

            // load stacked tile into cache and mark it as used
            StackedTile *stackedTile = m_tileLoader->loadTile( id );
            stackedTile->setUsed( true );

            invisibleTiles.removeAll( id.toString() );
            if ( !d->m_visibleTiles.contains( id.toString() ) ) {
                const QImage image = *stackedTile->resultTile();
                const GLuint texture = d->m_glContext->bindTexture( image, GL_TEXTURE_2D, GL_RGBA, QGLContext::LinearFilteringBindOption | QGLContext::PremultipliedAlphaBindOption );
                d->m_visibleTiles.insert( id.toString(), Tile( id, texture ) );
            }
        }
    }

    foreach ( const QString &id, invisibleTiles ) {
        Tile tile = d->m_visibleTiles[id];
        d->m_visibleTiles.remove( id );
        d->m_glContext->deleteTexture( tile.glTexture() );
    }

    // clear unused tiles
    m_tileLoader->cleanupTilehash();
}

void GLTextureMapper::updateTile( const TileId &id )
{
    if ( d->m_visibleTiles.contains( id.toString() ) ) {
        Tile tile = d->m_visibleTiles[id.toString()];
        d->m_glContext->deleteTexture( tile.glTexture() );
        d->m_visibleTiles.remove( id.toString() );
    }

    emit tileUpdatesAvailable();
}

void GLTextureMapper::geoCoordinates( qreal normalizedX, qreal normalizedY,
                                      qreal& lon, qreal& lat ) const
{
    Q_ASSERT( 0 <= normalizedX && normalizedX <= 1 );
    Q_ASSERT( 0 <= normalizedY && normalizedY <= 1 );

    switch ( m_tileLoader->tileProjection() ) {
    case GeoSceneTexture::Mercator:
        lat = atan( sinh( ( 0.5 - normalizedY ) * 2 * M_PI ) ) * RAD2DEG;
        lon = ( normalizedX - 0.5 ) * 360;
        return;
    case GeoSceneTexture::Equirectangular:
        lat = ( 0.5 - normalizedY ) * 180;
        lon = ( normalizedX - 0.5 ) * 360;
        return;
    }

    Q_ASSERT( false ); // not reached

    return;
}

void GLTextureMapper::projectionCoordinates( qreal lon, qreal lat, qreal &x, qreal &y ) const
{
    switch ( m_tileLoader->tileProjection() ) {
    case GeoSceneTexture::Mercator:
        if ( lat < -85*DEG2RAD ) lat = -85*DEG2RAD;
        if ( lat >  85*DEG2RAD ) lat =  85*DEG2RAD;
        x = ( 0.5 + 0.5 * lon / M_PI );
        y = ( 0.5 - 0.5 * atanh( sin( lat ) ) / M_PI );
        return;
    case GeoSceneTexture::Equirectangular:
        x = ( 0.5 + 0.5 * lon / M_PI );
        y = ( 0.5 - lat / M_PI );
        return;
    }

    Q_ASSERT( false ); // not reached

    return;
}

#include "GLTextureMapper.moc"
