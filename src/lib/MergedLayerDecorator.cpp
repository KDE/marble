// Copyright 2008 David Roberts <dvdr18@gmail.com>
// Copyright 2009 Jens-Michael Hoffmann <jensmh@gmx.de>
// Copyright 2011 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public 
// License along with this library.  If not, see <http://www.gnu.org/licenses/>.


#include "MergedLayerDecorator.h"

#include "blendings/Blending.h"
#include "SunLocator.h"
#include "global.h"
#include "MarbleDebug.h"
#include "GeoSceneDocument.h"
#include "GeoSceneHead.h"
#include "GeoSceneMap.h"
#include "GeoSceneTexture.h"
#include "MapThemeManager.h"
#include "StackedTile.h"
#include "TileLoaderHelper.h"
#include "Planet.h"
#include "TextureTile.h"
#include "TileCreator.h"
#include "TileCreatorDialog.h"
#include "TileLoader.h"

#include <QtCore/QMutexLocker>
#include <QtCore/QPointer>
#include <QtGui/QPainter>

using namespace Marble;

MergedLayerDecorator::MergedLayerDecorator( TileLoader * const tileLoader,
                                            const SunLocator* sunLocator )
    : m_tileLoader( tileLoader ),
      m_sunLocator( sunLocator ),
      m_blendingFactory( sunLocator ),
      m_themeId(),
      m_levelZeroColumns( 0 ),
      m_levelZeroRows( 0 ),
      m_showSunShading( false ),
      m_showCityLights( false ),
      m_showTileId( false )
{
}

MergedLayerDecorator::~MergedLayerDecorator()
{
}

StackedTile *MergedLayerDecorator::createTile( const QVector<QSharedPointer<TextureTile> > &tiles ) const
{
    Q_ASSERT( !tiles.isEmpty() );

    const TileId firstId = tiles.first()->id();
    const TileId id( 0, firstId.zoomLevel(), firstId.x(), firstId.y() );

    QImage resultImage;

    // if there are more than one active texture layers, we have to convert the
    // result tile into QImage::Format_ARGB32_Premultiplied to make blending possible
    const bool withConversion = tiles.count() > 1 || m_showSunShading || m_showTileId;
    foreach ( const QSharedPointer<TextureTile> &tile, tiles ) {
            const Blending *const blending = tile->blending();
            if ( blending ) {
                mDebug() << Q_FUNC_INFO << "blending";
                blending->blend( &resultImage, tile.data() );
            }
            else {
                mDebug() << Q_FUNC_INFO << "no blending defined => copying top over bottom image";
                if ( withConversion ) {
                    resultImage = tile->image()->convertToFormat( QImage::Format_ARGB32_Premultiplied );
                } else {
                    resultImage = tile->image()->copy();
                }
            }
    }

    if ( m_showSunShading && !m_showCityLights ) {
        paintSunShading( &resultImage, id );
    }

    if ( m_showTileId ) {
        paintTileId( &resultImage, id );
    }

    return new StackedTile( id, resultImage, tiles );
}

StackedTile *MergedLayerDecorator::loadTile( const TileId &stackedTileId, const QVector<const GeoSceneTexture *> &textureLayers ) const
{
    QVector<QSharedPointer<TextureTile> > tiles;

    foreach ( const GeoSceneTexture *textureLayer, textureLayers ) {
        const TileId tileId( textureLayer->sourceDir(), stackedTileId.zoomLevel(),
                             stackedTileId.x(), stackedTileId.y() );

        mDebug() << Q_FUNC_INFO << textureLayer->sourceDir() << tileId.toString() << textureLayer->tileSize();

        const QImage tileImage = m_tileLoader->loadTile( tileId, DownloadBrowse );
        const Blending *blending = m_blendingFactory.findBlending( textureLayer->blending() );
        if ( blending == 0 && !textureLayer->blending().isEmpty() ) {
            mDebug() << Q_FUNC_INFO << "could not find blending" << textureLayer->blending();
        }
        QSharedPointer<TextureTile> tile( new TextureTile( tileId, tileImage, blending ) );
        tiles.append( tile );
    }

    Q_ASSERT( !tiles.isEmpty() );

    return createTile( tiles );
}

void MergedLayerDecorator::setThemeId( const QString &themeId )
{
    m_themeId = themeId;
}

void MergedLayerDecorator::setShowSunShading( bool show )
{
    m_showSunShading = show;
}

void MergedLayerDecorator::setLevelZeroLayout( int levelZeroColumns, int levelZeroRows )
{
    m_blendingFactory.setLevelZeroLayout( levelZeroColumns, levelZeroRows );

    m_levelZeroColumns = levelZeroColumns;
    m_levelZeroRows = levelZeroRows;
}

bool MergedLayerDecorator::showSunShading() const
{
    return m_showSunShading;
}

void MergedLayerDecorator::setShowCityLights( bool show )
{
    m_showCityLights = show;
}

bool MergedLayerDecorator::showCityLights() const
{
    return m_showCityLights;
}

void MergedLayerDecorator::setShowTileId( bool visible )
{
    m_showTileId = visible;
}

void MergedLayerDecorator::paintSunShading( QImage *tileImage, const TileId &id ) const
{
    if ( tileImage->depth() != 32 )
        return;

    // TODO add support for 8-bit maps?
    // add sun shading
    const qreal  global_width  = tileImage->width()
        * TileLoaderHelper::levelToColumn( m_levelZeroColumns, id.zoomLevel() );
    const qreal  global_height = tileImage->height()
        * TileLoaderHelper::levelToRow( m_levelZeroRows, id.zoomLevel() );
    const qreal lon_scale = 2*M_PI / global_width;
    const qreal lat_scale = -M_PI / global_height;
    const int tileHeight = tileImage->height();
    const int tileWidth = tileImage->width();

    // First we determine the supporting point interval for the interpolation.
    const int n = maxDivisor( 30, tileWidth );
    const int ipRight = n * (int)( tileWidth / n );

    for ( int cur_y = 0; cur_y < tileHeight; ++cur_y ) {
        const qreal lat = lat_scale * ( id.y() * tileHeight + cur_y ) - 0.5*M_PI;
        const qreal a = sin( (lat+DEG2RAD * m_sunLocator->getLat() )/2.0 );
        const qreal c = cos(lat)*cos( -DEG2RAD * m_sunLocator->getLat() );

        QRgb* scanline = (QRgb*)tileImage->scanLine( cur_y );

        qreal lastShade = -10.0;

        int cur_x = 0;

        while ( cur_x < tileWidth ) {

            const bool interpolate = ( cur_x != 0 && cur_x < ipRight && cur_x + n < tileWidth );

            qreal shade = 0;

            if ( interpolate ) {
                const int check = cur_x + n;
                const qreal checklon   = lon_scale * ( id.x() * tileWidth + check );
                shade = m_sunLocator->shading( checklon, a, c );

                // if the shading didn't change across the interpolation
                // interval move on and don't change anything.
                if ( shade == lastShade && shade == 1.0 ) {
                    scanline += n;
                    cur_x += n;
                    continue;
                }
                if ( shade == lastShade && shade == 0.0 ) {
                    for ( int t = 0; t < n; ++t ) {
                        m_sunLocator->shadePixel( *scanline, shade );
                        ++scanline;
                    }
                    cur_x += n;
                    continue;
                }
                for ( int t = 0; t < n ; ++t ) {
                    const qreal lon   = lon_scale * ( id.x() * tileWidth + cur_x );
                    shade = m_sunLocator->shading( lon, a, c );
                    m_sunLocator->shadePixel( *scanline, shade );
                    ++scanline;
                    ++cur_x;
                }
            }

            else {
                // Make sure we don't exceed the image memory
                if ( cur_x < tileWidth ) {
                    const qreal lon   = lon_scale * ( id.x() * tileWidth + cur_x );
                    shade = m_sunLocator->shading( lon, a, c );
                    m_sunLocator->shadePixel( *scanline, shade );
                    ++scanline;
                    ++cur_x;
                }
            }
            lastShade = shade;
        }
    }
}

void MergedLayerDecorator::paintTileId( QImage *tileImage, const TileId &id ) const
{
    QString filename = QString( "%1_%2.jpg" )
        .arg( id.x(), tileDigits, 10, QChar('0') )
        .arg( id.y(), tileDigits, 10, QChar('0') );

    QPainter painter( tileImage );

    QColor foreground;
    QColor background;

    if ( ( (qreal)(id.x())/2 == id.x()/2 && (qreal)(id.y())/2 == id.y()/2 ) 
         || ( (qreal)(id.x())/2 != id.x()/2 && (qreal)(id.y())/2 != id.y()/2 ) 
       )
    {
        foreground.setNamedColor( "#FFFFFF" );
        background.setNamedColor( "#000000" );
    }
    else {
        foreground.setNamedColor( "#000000" );
        background.setNamedColor( "#FFFFFF" );
    }

    int   strokeWidth = 10;
    QPen  testPen( foreground );
    testPen.setWidth( strokeWidth );
    testPen.setJoinStyle( Qt::MiterJoin );

    painter.setPen( testPen );
    painter.drawRect( strokeWidth / 2, strokeWidth / 2, 
                      tileImage->width()  - strokeWidth,
                      tileImage->height() - strokeWidth );
    QFont testFont( "Sans", 12 );
    QFontMetrics testFm( testFont );
    painter.setFont( testFont );

    QPen outlinepen( foreground );
    outlinepen.setWidthF( 6 );

    painter.setPen( outlinepen );
    painter.setBrush( background );

    QPainterPath   outlinepath;

    QPointF  baseline1( ( tileImage->width() - testFm.boundingRect(filename).width() ) / 2,
                        ( tileImage->height() * 0.25) );
    outlinepath.addText( baseline1, testFont, QString( "level: %1" ).arg(id.zoomLevel()) );

    QPointF  baseline2( ( tileImage->width() - testFm.boundingRect(filename).width() ) / 2,
                        tileImage->height() * 0.50 );
    outlinepath.addText( baseline2, testFont, filename );

    QPointF  baseline3( ( tileImage->width() - testFm.boundingRect(filename).width() ) / 2,
                        tileImage->height() * 0.75 );
    outlinepath.addText( baseline3, testFont, m_themeId );

    painter.drawPath( outlinepath );

    painter.setPen( Qt::NoPen );
    painter.drawPath( outlinepath );
}

// TODO: This should likely go into a math class in the future ...

int MergedLayerDecorator::maxDivisor( int maximum, int fullLength )
{
    // Find the optimal interpolation interval n for the 
    // current image canvas width
    int best = 2;

    int  nEvalMin = fullLength;
    for ( int it = 1; it <= maximum; ++it ) {
        // The optimum is the interval which results in the least amount
        // supporting points taking into account the rest which can't 
        // get used for interpolation.
        int nEval = fullLength / it + fullLength % it;
        if ( nEval < nEvalMin ) {
            nEvalMin = nEval;
            best = it; 
        }
    }
    return best;
}
