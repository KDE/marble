// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library. If not, see <http://www.gnu.org/licenses/>.

#include "DownloadRegionDialog.h"

#include <cmath>

#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QVBoxLayout>

#include "AbstractScanlineTextureMapper.h"
#include "GeoDataLatLonBox.h"
#include "GeoSceneTexture.h"
#include "MarbleDebug.h"
#include "MarbleMath.h"
#include "LatLonBoxWidget.h"
#include "TileId.h"
#include "TileLevelRangeWidget.h"
#include "TileLoaderHelper.h"
#include "ViewportParams.h"

namespace Marble
{

int const maxTilesCount = 100000;

class DownloadRegionDialog::Private
{
public:
    Private( ViewportParams const * const viewport,
             AbstractScanlineTextureMapper const * const textureMapper,
             QDialog * const dialog );
    QGroupBox * createSelectionMethodBox();
    QLayout * createTilesCounter();
    QLayout * createOkCancelButtonBox();

    int rad2PixelX( qreal const lon ) const;
    int rad2PixelY( qreal const lat ) const;

    QDialog * m_dialog;
    LatLonBoxWidget * m_latLonBoxWidget;
    TileLevelRangeWidget * m_tileLevelRangeWidget;
    QLabel * m_tilesCountLabel;
    QPushButton * m_okButton;
    int m_originatingTileLevel;
    int m_minimumAllowedTileLevel;
    int m_maximumAllowedTileLevel;
    ViewportParams const * const m_viewport;
    AbstractScanlineTextureMapper const * const m_textureMapper;
    GeoSceneTexture const * const m_textureLayer;
    GeoDataLatLonBox m_currentRegion;
};

DownloadRegionDialog::Private::Private( ViewportParams const * const viewport,
                                        AbstractScanlineTextureMapper const * const textureMapper,
                                        QDialog * const dialog )
    : m_dialog( dialog ),
      m_latLonBoxWidget( new LatLonBoxWidget ),
      m_tileLevelRangeWidget( new TileLevelRangeWidget ),
      m_tilesCountLabel( 0 ),
      m_okButton( 0 ),
      m_originatingTileLevel( textureMapper->tileZoomLevel() ),
      m_minimumAllowedTileLevel( -1 ),
      m_maximumAllowedTileLevel( -1 ),
      m_viewport( viewport ),
      m_textureMapper( textureMapper ),
      m_textureLayer( textureMapper->textureLayer() ),
      m_currentRegion( viewport->viewLatLonAltBox() )
{
    m_latLonBoxWidget->setEnabled( false );
    m_latLonBoxWidget->setLatLonBox( m_currentRegion );
    m_tileLevelRangeWidget->setDefaultTileLevel( m_originatingTileLevel );
}

QGroupBox * DownloadRegionDialog::Private::createSelectionMethodBox()
{
    QRadioButton * const currentRegionMethodButton = new QRadioButton( tr( "Current region" ));
    currentRegionMethodButton->setChecked( true );
    QRadioButton * const latLonBoxMethodButton = new QRadioButton( tr( "Specify region" ));
    connect( latLonBoxMethodButton, SIGNAL( toggled( bool )),
             m_dialog, SLOT( toggleSelectionMethod() ));

    QVBoxLayout * const layout = new QVBoxLayout;
    layout->addWidget( currentRegionMethodButton );
    layout->addWidget( latLonBoxMethodButton );
    layout->addWidget( m_latLonBoxWidget );

    QGroupBox * const selectionMethodBox = new QGroupBox( tr( "Selection method" ));
    selectionMethodBox->setLayout( layout );
    return selectionMethodBox;
}

QLayout * DownloadRegionDialog::Private::createTilesCounter()
{
    QLabel * const description = new QLabel( tr( "Number of tiles:" ));
    m_tilesCountLabel = new QLabel;

    QHBoxLayout * const layout = new QHBoxLayout;
    layout->addWidget( description );
    layout->addWidget( m_tilesCountLabel );
    return layout;
}

QLayout * DownloadRegionDialog::Private::createOkCancelButtonBox()
{
    QHBoxLayout * const buttonBox = new QHBoxLayout;
    m_okButton = new QPushButton( tr( "Ok" ));
    connect( m_okButton, SIGNAL( clicked() ), m_dialog, SLOT( accept() ));
    QPushButton * const cancelButton = new QPushButton( tr( "Cancel" ));
    connect( cancelButton, SIGNAL( clicked() ), m_dialog, SLOT( reject() ));
    buttonBox->addWidget( m_okButton );
    buttonBox->addWidget( cancelButton );
    return buttonBox;
}

// copied from AbstractScanlineTextureMapper and slightly adjusted
int DownloadRegionDialog::Private::rad2PixelX( qreal const lon ) const
{
    qreal const globalWidth = m_textureMapper->tileSize().width()
        * TileLoaderHelper::levelToColumn( m_textureLayer->levelZeroColumns(),
                                           m_originatingTileLevel );
    return static_cast<int>( globalWidth * 0.5 + lon * ( globalWidth / ( 2.0 * M_PI ) ));
}

// copied from AbstractScanlineTextureMapper and slightly adjusted
int DownloadRegionDialog::Private::rad2PixelY( qreal const lat ) const
{
    qreal const globalHeight = m_textureMapper->tileSize().height()
        * TileLoaderHelper::levelToRow( m_textureLayer->levelZeroRows(), m_originatingTileLevel );
    qreal const normGlobalHeight = globalHeight / M_PI;
    switch ( m_textureLayer->projection() ) {
    case GeoSceneTexture::Equirectangular:
        return static_cast<int>( globalHeight * 0.5 - lat * normGlobalHeight );
    case GeoSceneTexture::Mercator:
        if ( fabs( lat ) < 1.4835 )
            return static_cast<int>( globalHeight * 0.5 - gdInv( lat ) * 0.5 * normGlobalHeight );
        if ( lat >= +1.4835 )
            return static_cast<int>( globalHeight * 0.5 - 3.1309587 * 0.5 * normGlobalHeight );
        if ( lat <= -1.4835 )
            return static_cast<int>( globalHeight * 0.5 + 3.1309587 * 0.5 * normGlobalHeight );
    }

    // Dummy value to avoid a warning.
    return 0;
}

DownloadRegionDialog::DownloadRegionDialog( ViewportParams const * const viewport,
                                            AbstractScanlineTextureMapper const * const textureMapper,
                                            QWidget * const parent, Qt::WindowFlags const f )
    : QDialog( parent, f ),
      d( new Private( viewport, textureMapper, this ))
{
    QVBoxLayout * const layout = new QVBoxLayout;
    layout->addWidget( d->createSelectionMethodBox() );
    layout->addWidget( d->m_tileLevelRangeWidget );
    layout->addLayout( d->createTilesCounter() );
    layout->addLayout( d->createOkCancelButtonBox() );
    setLayout( layout );

    connect( d->m_latLonBoxWidget, SIGNAL( valueChanged() ), SLOT( updateTilesCount() ));
    connect( d->m_tileLevelRangeWidget, SIGNAL( topLevelChanged( int )),
             SLOT( updateTilesCount() ));
    connect( d->m_tileLevelRangeWidget, SIGNAL( bottomLevelChanged( int )),
             SLOT( updateTilesCount() ));
    updateTilesCount();
}

void DownloadRegionDialog::setAllowedTileLevelRange( int const minimumTileLevel,
                                                     int const maximumTileLevel )
{
    d->m_minimumAllowedTileLevel = minimumTileLevel;
    d->m_maximumAllowedTileLevel = maximumTileLevel;
    d->m_tileLevelRangeWidget->setAllowedTileLevelRange( minimumTileLevel, maximumTileLevel );
}

void DownloadRegionDialog::setOriginatingTileLevel( int const tileLevel )
{
    d->m_originatingTileLevel = tileLevel;
    d->m_tileLevelRangeWidget->setDefaultTileLevel( tileLevel );
}

TileCoordsPyramid DownloadRegionDialog::region() const
{
    // check whether "current region" or "lat/lon region" is selection method
    GeoDataLatLonBox downloadRegion = d->m_currentRegion;
    if ( d->m_latLonBoxWidget->isEnabled() )
        downloadRegion = d->m_latLonBoxWidget->latLonBox();

    int const westX = d->rad2PixelX( downloadRegion.west() );
    int const northY = d->rad2PixelY( downloadRegion.north() );
    int const eastX = d->rad2PixelX( downloadRegion.east() );
    int const southY = d->rad2PixelY( downloadRegion.south() );

    // FIXME: remove this stuff
    mDebug() << "DownloadRegionDialog downloadRegion:"
             << "north:" << downloadRegion.north()
             << "south:" << downloadRegion.south()
             << "east:" << downloadRegion.east()
             << "west:" << downloadRegion.west();
    mDebug() << "north/west (x/y):" << westX << northY;
    mDebug() << "south/east (x/y):" << eastX << southY;

    int const tileWidth = d->m_textureMapper->tileSize().width();
    int const tileHeight = d->m_textureMapper->tileSize().height();

    int const currentLevelTileX1 = qMin( westX, eastX ) / tileWidth;
    int const currentLevelTileY1 = qMin( northY, southY ) / tileHeight;
    int const pixelX2 = qMax( westX, eastX );
    int const pixelY2 = qMax( northY, southY );
    int const currentLevelTileX2 = pixelX2 / tileWidth + ( pixelX2 % tileWidth > 0 ? 1 : 0 );
    int const currentLevelTileY2 = pixelY2 / tileHeight + ( pixelY2 % tileHeight > 0 ? 1 : 0 );

    mDebug() << "current level tile coords (x1/y1/x2/y2):"
             << currentLevelTileX1 << currentLevelTileY1 << currentLevelTileX2 << currentLevelTileY2;

    QRect topLevelTileCoords;
    // the pixel coords calculated above are referring to the originating ("current") tile level,
    // if the top level is now a different level, we have to take it into account
    if ( d->m_originatingTileLevel > d->m_tileLevelRangeWidget->topLevel() ) {
        int const deltaLevel = d->m_originatingTileLevel - d->m_tileLevelRangeWidget->topLevel();
        topLevelTileCoords.setCoords( currentLevelTileX1 >> deltaLevel,
                                      currentLevelTileY1 >> deltaLevel,
                                      currentLevelTileX2 >> deltaLevel,
                                      currentLevelTileY2 >> deltaLevel );
    }
    else if ( d->m_originatingTileLevel < d->m_tileLevelRangeWidget->topLevel() ) {
        int const deltaLevel = d->m_tileLevelRangeWidget->topLevel() - d->m_originatingTileLevel;
        topLevelTileCoords.setCoords( currentLevelTileX1 << deltaLevel,
                                      currentLevelTileY1 << deltaLevel,
                                      (( currentLevelTileX2 + 1 ) << deltaLevel ) - 1,
                                      (( currentLevelTileY2 + 1 ) << deltaLevel ) - 1 );
    }
    else
        topLevelTileCoords.setCoords( currentLevelTileX1, currentLevelTileY1,
                                      currentLevelTileX2, currentLevelTileY2 );

    mDebug() << "originating level tile coords (x1/y1/x2/y2):"
             << currentLevelTileX1 << currentLevelTileY1 << currentLevelTileX2 << currentLevelTileY2;
    mDebug() << "top level tile coords (x1/y1/w x h):" << topLevelTileCoords;

    TileCoordsPyramid coordsPyramid( d->m_tileLevelRangeWidget->topLevel(),
                                     d->m_tileLevelRangeWidget->bottomLevel() );
    coordsPyramid.setTopLevelCoords( topLevelTileCoords );
    mDebug() << "tiles count:" << coordsPyramid.tilesCount();
    return coordsPyramid;
}

void DownloadRegionDialog::toggleSelectionMethod()
{
    d->m_latLonBoxWidget->setEnabled( !d->m_latLonBoxWidget->isEnabled() );
}

void DownloadRegionDialog::updateTilesCount()
{
    TileCoordsPyramid const pyramid = region();
    int const tilesCount = pyramid.tilesCount();
    mDebug() << "updateTilesCount:" << tilesCount;
    d->m_tilesCountLabel->setNum( tilesCount );
    d->m_okButton->setEnabled( tilesCount > 0 && tilesCount <= maxTilesCount );
}

}

#include "DownloadRegionDialog.moc"
