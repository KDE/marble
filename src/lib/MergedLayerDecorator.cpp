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
#include "blendings/BlendingFactory.h"
#include "SunLocator.h"
#include "MarbleGlobal.h"
#include "MarbleDebug.h"
#include "GeoDataDocument.h"
#include "GeoSceneDocument.h"
#include "GeoSceneHead.h"
#include "GeoSceneMap.h"
#include "GeoSceneTiled.h"
#include "MapThemeManager.h"
#include "StackedTile.h"
#include "TileLoaderHelper.h"
#include "Planet.h"
#include "TextureTile.h"
#include "VectorTile.h"
#include "TileCreator.h"
#include "TileCreatorDialog.h"
#include "TileLoader.h"


#include <QtCore/QMutexLocker>
#include <QtCore/QPointer>
#include <QtGui/QPainter>

using namespace Marble;

struct MergedLayerDecorator::Private
{
public:
    Private( TileLoader *tileLoader, const SunLocator *sunLocator );

    static int maxDivisor( int maximum, int fullLength );

    StackedTile *createTile( const QVector<QSharedPointer<Tile> > &tiles ) const;

    void paintSunShading( QImage *tileImage, const TileId &id ) const;
    void paintTileId( QImage *tileImage, const TileId &id ) const;

    TileLoader *const m_tileLoader;
    const SunLocator *const m_sunLocator;
    BlendingFactory m_blendingFactory;
    QString m_themeId;
    int m_levelZeroColumns;
    int m_levelZeroRows;
    bool m_showSunShading;
    bool m_showCityLights;
    bool m_showTileId;
};

MergedLayerDecorator::Private::Private( TileLoader *tileLoader, const SunLocator *sunLocator ) :
    m_tileLoader( tileLoader ),
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

MergedLayerDecorator::MergedLayerDecorator( TileLoader * const tileLoader,
                                            const SunLocator* sunLocator )
    : d( new Private( tileLoader, sunLocator ) )
{
}

MergedLayerDecorator::~MergedLayerDecorator()
{
    delete d;
}

StackedTile *MergedLayerDecorator::Private::createTile( const QVector<QSharedPointer<Tile> > &tiles ) const
{
    Q_ASSERT( !tiles.isEmpty() );

    const TileId firstId = tiles.first()->id();
    const TileId id( 0, firstId.zoomLevel(), firstId.x(), firstId.y() );

    // Image for blending all the texture tiles on it
    QImage resultImage;

    // GeoDataDocument for appending all the vector data features to it
    GeoDataDocument *resultVector = new GeoDataDocument();

    // if there are more than one active texture layers, we have to convert the
    // result tile into QImage::Format_ARGB32_Premultiplied to make blending possible
    const bool withConversion = tiles.count() > 1 || m_showSunShading || m_showTileId;
    foreach ( const QSharedPointer<Tile> &tile, tiles ) {

        // Image blending. If there are several images in the same tile (like clouds
        // or hillshading images over the map) blend them all into only one image

        if (!tile->image()->isNull()){

            const Blending *const blending = tile->blending();
            if ( blending ) {
                mDebug() << Q_FUNC_INFO << "blending";

                Q_ASSERT( !resultImage.isNull());

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

            if ( m_showSunShading && !m_showCityLights ) {
                paintSunShading( &resultImage, id );
            }

            if ( m_showTileId ) {
                paintTileId( &resultImage, id );
            }
        }

        // Geometry appending. If it is a VectorTile append all its geometries to the
        // main GeoDataDocument
        if ( tile->vectorData() ){

            for (int x = 0; x < tile->vectorData()->size(); x++)
                resultVector->append(tile->vectorData()->featureList().at(x));
        }
    }
    return new StackedTile( id, resultImage, *resultVector, tiles );
}

StackedTile *MergedLayerDecorator::loadTile( const TileId &stackedTileId, const QVector<const GeoSceneTiled *> &textureLayers ) const
{
    QVector<QSharedPointer<Tile> > tiles;

    foreach ( const GeoSceneTiled *textureLayer, textureLayers ) {
        const TileId tileId( textureLayer->sourceDir(), stackedTileId.zoomLevel(),
                             stackedTileId.x(), stackedTileId.y() );

        mDebug() << Q_FUNC_INFO << textureLayer->sourceDir() << tileId.toString() << textureLayer->tileSize() << textureLayer->fileFormat();

        // Blending (how to merge the images into an only image)
        const Blending *blending = d->m_blendingFactory.findBlending( textureLayer->blending() );
        if ( blending == 0 && !textureLayer->blending().isEmpty() ) {
            mDebug() << Q_FUNC_INFO << "could not find blending" << textureLayer->blending();
        }

        // File format for creating an ImageTile or a VectorTile
        QString format = textureLayer->fileFormat();

        // VectorTile
        if ( format.toLower() == "js"){
            // FIXME ANDER load an image for testing
            QImage tileImage ( "/home/ander/image.png" );
            //QImage tileImage();

            GeoDataDocument tileVectordata = d->m_tileLoader->loadTileVectorData( tileId, DownloadBrowse, format );

            QSharedPointer<Tile> tile( new VectorTile( tileId, tileImage, tileVectordata, format, blending ) );
            tiles.append( tile );

        }
        // ImageTile
        else {
            const QImage tileImage = d->m_tileLoader->loadTileImage( tileId, DownloadBrowse );

            QSharedPointer<Tile> tile(new TextureTile( tileId, tileImage, format, blending ) );
            tiles.append( tile );
        }
    }

    Q_ASSERT( !tiles.isEmpty() );

    return d->createTile( tiles );
}

StackedTile *MergedLayerDecorator::createTile( const StackedTile &stackedTile, const TileId &tileId, const QImage &tileImage, const QString &format ) const
{
    QVector<QSharedPointer<Tile> > tiles = stackedTile.tiles();

    for ( int i = 0; i < tiles.count(); ++ i) {
        if ( tiles[i]->id() == tileId ) {
            const Blending *blending = tiles[i]->blending();

            // VectorTile
            if ( format.toLower() == "js"){
                const GeoDataDocument* document = new GeoDataDocument;
                tiles[i] = QSharedPointer<Tile>( new VectorTile( tileId, tileImage, *document, format, blending ) );
            }
            // TextureTile
            else{
               tiles[i] = QSharedPointer<Tile>( new TextureTile( tileId, tileImage, format, blending ) );
            }
        }
    }

    return d->createTile( tiles );
}

void MergedLayerDecorator::downloadTile( const TileId &id, const QVector<GeoSceneTiled const *> &textureLayers )
{
    foreach ( const GeoSceneTiled *textureLayer, textureLayers ) {
        const TileId tileId( textureLayer->sourceDir(), id.zoomLevel(), id.x(), id.y() );
        d->m_tileLoader->downloadTile( tileId );
    }
}

void MergedLayerDecorator::reloadTile( const StackedTile &stackedTile )
{
    foreach ( QSharedPointer<Tile> const & tile, stackedTile.tiles() ) {
        // it's debatable here, whether DownloadBulk or DownloadBrowse should be used
        // but since "reload" or "refresh" seems to be a common action of a browser and it
        // allows for more connections (in our model), use "DownloadBrowse"
        d->m_tileLoader->reloadTile( tile->id(), DownloadBrowse );
    }
}

void MergedLayerDecorator::setThemeId( const QString &themeId )
{
    d->m_themeId = themeId;
}

void MergedLayerDecorator::setShowSunShading( bool show )
{
    d->m_showSunShading = show;
}

void MergedLayerDecorator::setLevelZeroLayout( int levelZeroColumns, int levelZeroRows )
{
    d->m_blendingFactory.setLevelZeroLayout( levelZeroColumns, levelZeroRows );

    d->m_levelZeroColumns = levelZeroColumns;
    d->m_levelZeroRows = levelZeroRows;
}

bool MergedLayerDecorator::showSunShading() const
{
    return d->m_showSunShading;
}

void MergedLayerDecorator::setShowCityLights( bool show )
{
    d->m_showCityLights = show;
}

bool MergedLayerDecorator::showCityLights() const
{
    return d->m_showCityLights;
}

void MergedLayerDecorator::setShowTileId( bool visible )
{
    d->m_showTileId = visible;
}

void MergedLayerDecorator::Private::paintSunShading( QImage *tileImage, const TileId &id ) const
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

void MergedLayerDecorator::Private::paintTileId( QImage *tileImage, const TileId &id ) const
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

int MergedLayerDecorator::Private::maxDivisor( int maximum, int fullLength )
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
