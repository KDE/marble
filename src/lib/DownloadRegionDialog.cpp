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

#include <QtGui/QDialogButtonBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHideEvent>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QShowEvent>
#include <QtGui/QVBoxLayout>

#include "AbstractScanlineTextureMapper.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoSceneTexture.h"
#include "MarbleDebug.h"
#include "MarbleMath.h"
#include "MarbleModel.h"
#include "LatLonBoxWidget.h"
#include "TileId.h"
#include "TileLevelRangeWidget.h"
#include "TileLoaderHelper.h"

namespace Marble
{

int const maxTilesCount = 100000;

class DownloadRegionDialog::Private
{
public:
    Private( MarbleModel const * const model, QDialog * const dialog );
    QWidget * createSelectionMethodBox();
    QLayout * createTilesCounter();
    QWidget * createOkCancelButtonBox();

    int rad2PixelX( qreal const lon ) const;
    int rad2PixelY( qreal const lat ) const;
    AbstractScanlineTextureMapper const * textureMapper() const;

    QDialog * m_dialog;
    QRadioButton * m_visibleRegionMethodButton;
    QRadioButton * m_specifiedRegionMethodButton;
    LatLonBoxWidget * m_latLonBoxWidget;
    TileLevelRangeWidget * m_tileLevelRangeWidget;
    QLabel * m_tilesCountLabel;
    QLabel * m_tilesCountLimitInfo;
    QPushButton * m_okButton;
    QPushButton * m_applyButton;
    int m_originatingTileLevel;
    int m_minimumAllowedTileLevel;
    int m_maximumAllowedTileLevel;
    MarbleModel const * const m_model;
    GeoSceneTexture const * m_textureLayer;
    SelectionMethod m_selectionMethod;
    GeoDataLatLonBox m_visibleRegion;
};

DownloadRegionDialog::Private::Private( MarbleModel const * const model,
                                        QDialog * const dialog )
    : m_dialog( dialog ),
      m_visibleRegionMethodButton( 0 ),
      m_specifiedRegionMethodButton( 0 ),
      m_latLonBoxWidget( new LatLonBoxWidget ),
      m_tileLevelRangeWidget( new TileLevelRangeWidget ),
      m_tilesCountLabel( 0 ),
      m_tilesCountLimitInfo( 0 ),
      m_okButton( 0 ),
      m_applyButton( 0 ),
      m_originatingTileLevel( model->textureMapper()->tileZoomLevel() ),
      m_minimumAllowedTileLevel( -1 ),
      m_maximumAllowedTileLevel( -1 ),
      m_model( model ),
      m_textureLayer( model->textureMapper()->textureLayer() ),
      m_selectionMethod( VisibleRegionMethod ),
      m_visibleRegion()
{
    m_latLonBoxWidget->setEnabled( false );
    m_latLonBoxWidget->setLatLonBox( m_visibleRegion );
    m_tileLevelRangeWidget->setDefaultLevel( m_originatingTileLevel );
}

QWidget * DownloadRegionDialog::Private::createSelectionMethodBox()
{
    m_visibleRegionMethodButton = new QRadioButton( tr( "Visible region" ));
    m_specifiedRegionMethodButton = new QRadioButton( tr( "Specify region" ));
    connect( m_specifiedRegionMethodButton, SIGNAL( toggled( bool )),
             m_dialog, SLOT( toggleSelectionMethod() ));

    QVBoxLayout * const layout = new QVBoxLayout;
    layout->addWidget( m_visibleRegionMethodButton );
    layout->addWidget( m_specifiedRegionMethodButton );
    layout->addWidget( m_latLonBoxWidget );

    QGroupBox * const selectionMethodBox = new QGroupBox( tr( "Selection method" ));
    selectionMethodBox->setLayout( layout );
    return selectionMethodBox;
}

QLayout * DownloadRegionDialog::Private::createTilesCounter()
{
    QLabel * const description = new QLabel( tr( "Number of tiles to download:" ));
    m_tilesCountLabel = new QLabel;
    m_tilesCountLimitInfo = new QLabel;

    QHBoxLayout * const tilesCountLayout = new QHBoxLayout;
    tilesCountLayout->addWidget( description );
    tilesCountLayout->addWidget( m_tilesCountLabel );

    QVBoxLayout * const layout = new QVBoxLayout;
    layout->addLayout( tilesCountLayout );
    layout->addWidget( m_tilesCountLimitInfo );
    return layout;
}

QWidget * DownloadRegionDialog::Private::createOkCancelButtonBox()
{
    QDialogButtonBox * const buttonBox = new QDialogButtonBox;
    m_okButton = buttonBox->addButton( QDialogButtonBox::Ok );
    m_applyButton = buttonBox->addButton( QDialogButtonBox::Apply );
    buttonBox->addButton( QDialogButtonBox::Cancel );
    connect( buttonBox, SIGNAL( accepted() ), m_dialog, SLOT( accept() ));
    connect( buttonBox, SIGNAL( rejected() ), m_dialog, SLOT( reject() ));
    connect( m_applyButton, SIGNAL( clicked() ), m_dialog, SIGNAL( applied() ));
    return buttonBox;
}

// copied from AbstractScanlineTextureMapper and slightly adjusted
int DownloadRegionDialog::Private::rad2PixelX( qreal const lon ) const
{
    qreal const globalWidth = textureMapper()->tileSize().width()
        * TileLoaderHelper::levelToColumn( m_textureLayer->levelZeroColumns(),
                                           m_originatingTileLevel );
    return static_cast<int>( globalWidth * 0.5 + lon * ( globalWidth / ( 2.0 * M_PI ) ));
}

// copied from AbstractScanlineTextureMapper and slightly adjusted
int DownloadRegionDialog::Private::rad2PixelY( qreal const lat ) const
{
    qreal const globalHeight = textureMapper()->tileSize().height()
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

AbstractScanlineTextureMapper const * DownloadRegionDialog::Private::textureMapper() const
{
    AbstractScanlineTextureMapper const * const result = m_model->textureMapper();
    Q_ASSERT( result );
    return result;
}


DownloadRegionDialog::DownloadRegionDialog( MarbleModel const * const model, QWidget * const parent,
                                            Qt::WindowFlags const f )
    : QDialog( parent, f ),
      d( new Private( model, this ))
{
    setWindowTitle( tr( "Download Region" ));

    QVBoxLayout * const layout = new QVBoxLayout;
    layout->addWidget( d->createSelectionMethodBox() );
    layout->addWidget( d->m_tileLevelRangeWidget );
    layout->addLayout( d->createTilesCounter() );
    layout->addWidget( d->createOkCancelButtonBox() );
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
    d->m_tileLevelRangeWidget->setAllowedLevelRange( minimumTileLevel, maximumTileLevel );
}

void DownloadRegionDialog::setOriginatingTileLevel( int const tileLevel )
{
    d->m_originatingTileLevel = tileLevel;
    d->m_tileLevelRangeWidget->setDefaultLevel( tileLevel );
}

void DownloadRegionDialog::setSelectionMethod( SelectionMethod const selectionMethod )
{
    // block signals to prevent infinite recursion:
    // radioButton->setChecked() -> toggleSelectionMethod() -> setSelectionMethod()
    //     -> radioButton->setChecked() -> ...
    d->m_visibleRegionMethodButton->blockSignals( true );
    d->m_specifiedRegionMethodButton->blockSignals( true );

    d->m_selectionMethod = selectionMethod;
    switch ( selectionMethod ) {
    case VisibleRegionMethod:
        d->m_visibleRegionMethodButton->setChecked( true );
        d->m_latLonBoxWidget->setEnabled( false );
        setSpecifiedLatLonAltBox( d->m_visibleRegion );
        break;
    case SpecifiedRegionMethod:
        d->m_specifiedRegionMethodButton->setChecked( true );
        d->m_latLonBoxWidget->setEnabled( true );
        break;
    }

    d->m_visibleRegionMethodButton->blockSignals( false );
    d->m_specifiedRegionMethodButton->blockSignals( false );
}

TileCoordsPyramid DownloadRegionDialog::region() const
{
    // check whether "visible region" or "lat/lon region" is selection method
    GeoDataLatLonBox downloadRegion;
    switch ( d->m_selectionMethod ) {
    case VisibleRegionMethod:
        downloadRegion = d->m_visibleRegion;
        break;
    case SpecifiedRegionMethod:
        downloadRegion = d->m_latLonBoxWidget->latLonBox();
        break;
    }

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

    int const tileWidth = d->textureMapper()->tileSize().width();
    int const tileHeight = d->textureMapper()->tileSize().height();
    mDebug() << "DownloadRegionDialog downloadRegion: tileSize:" << tileWidth << tileHeight;

    int const visibleLevelX1 = qMin( westX, eastX );
    int const visibleLevelY1 = qMin( northY, southY );
    int const visibleLevelX2 = qMax( westX, eastX );
    int const visibleLevelY2 = qMax( northY, southY );

    mDebug() << "visible level pixel coords (level/x1/y1/x2/y2):" << d->m_originatingTileLevel
             << visibleLevelX1 << visibleLevelY1 << visibleLevelX2 << visibleLevelY2;

    int bottomLevelX1, bottomLevelY1, bottomLevelX2, bottomLevelY2;
    // the pixel coords calculated above are referring to the originating ("visible") tile level,
    // if the bottom level is a different level, we have to take it into account
    if ( d->m_originatingTileLevel > d->m_tileLevelRangeWidget->bottomLevel() ) {
        int const deltaLevel = d->m_originatingTileLevel - d->m_tileLevelRangeWidget->bottomLevel();
        bottomLevelX1 = visibleLevelX1 >> deltaLevel;
        bottomLevelY1 = visibleLevelY1 >> deltaLevel;
        bottomLevelX2 = visibleLevelX2 >> deltaLevel;
        bottomLevelY2 = visibleLevelY2 >> deltaLevel;
    }
    else if ( d->m_originatingTileLevel < d->m_tileLevelRangeWidget->bottomLevel() ) {
        int const deltaLevel = d->m_tileLevelRangeWidget->bottomLevel() - d->m_originatingTileLevel;
        bottomLevelX1 = visibleLevelX1 << deltaLevel;
        bottomLevelY1 = visibleLevelY1 << deltaLevel;
        bottomLevelX2 = visibleLevelX2 << deltaLevel;
        bottomLevelY2 = visibleLevelY2 << deltaLevel;
    }
    else {
        bottomLevelX1 = visibleLevelX1;
        bottomLevelY1 = visibleLevelY1;
        bottomLevelX2 = visibleLevelX2;
        bottomLevelY2 = visibleLevelY2;
    }
    mDebug() << "bottom level pixel coords (level/x1/y1/x2/y2):"
             << d->m_tileLevelRangeWidget->bottomLevel()
             << bottomLevelX1 << bottomLevelY1 << bottomLevelX2 << bottomLevelY2;

    TileCoordsPyramid coordsPyramid( d->m_tileLevelRangeWidget->topLevel(),
                                     d->m_tileLevelRangeWidget->bottomLevel() );
    QRect bottomLevelTileCoords;
    bottomLevelTileCoords.setCoords
        ( bottomLevelX1 / tileWidth,
          bottomLevelY1 / tileHeight,
          bottomLevelX2 / tileWidth + ( bottomLevelX2 % tileWidth > 0 ? 1 : 0 ),
          bottomLevelY2 / tileHeight + ( bottomLevelY2 % tileHeight > 0 ? 1 : 0 ));
    mDebug() << "bottom level tile coords: (x1/y1/size):" << bottomLevelTileCoords;
    coordsPyramid.setBottomLevelCoords( bottomLevelTileCoords );
    mDebug() << "tiles count:" << coordsPyramid.tilesCount();
    return coordsPyramid;
}

void DownloadRegionDialog::setSpecifiedLatLonAltBox( GeoDataLatLonAltBox const & region )
{
    d->m_latLonBoxWidget->setLatLonBox( region );
}

void DownloadRegionDialog::setVisibleLatLonAltBox( GeoDataLatLonAltBox const & region )
{
    d->m_visibleRegion = region;
    // update lat/lon widget only if not active to prevent that users unintentionally loose
    // entered values
    if ( d->m_selectionMethod == VisibleRegionMethod ) {
        setSpecifiedLatLonAltBox( region );
    }
    updateTilesCount();
}

void DownloadRegionDialog::updateTextureLayer()
{
    mDebug() << "DownloadRegionDialog::updateTextureLayer";
    AbstractScanlineTextureMapper const * const textureMapper = d->m_model->textureMapper();
    d->m_textureLayer = textureMapper ? textureMapper->textureLayer() : 0;
    updateTilesCount();
}

void DownloadRegionDialog::hideEvent( QHideEvent * event )
{
    emit hidden();
    event->accept();
}

void DownloadRegionDialog::showEvent( QShowEvent * event )
{
    emit shown();
    event->accept();
}

void DownloadRegionDialog::toggleSelectionMethod()
{
    switch ( d->m_selectionMethod ) {
    case VisibleRegionMethod:
        setSelectionMethod( SpecifiedRegionMethod );
        break;
    case SpecifiedRegionMethod:
        setSelectionMethod( VisibleRegionMethod );
        break;
    }
}

void DownloadRegionDialog::updateTilesCount()
{
    qint64 tilesCount;
    if ( d->m_textureLayer ) {
        TileCoordsPyramid const pyramid = region();
        tilesCount = pyramid.tilesCount();
    }
    else {
        tilesCount = 0;
    }

    if ( tilesCount > maxTilesCount ) {
        d->m_tilesCountLimitInfo->setText( tr( "There is a limit of %n tiles to download.", "",
                                               maxTilesCount ));
    } else {
        d->m_tilesCountLimitInfo->clear();
    }
    d->m_tilesCountLabel->setText( QString::number( tilesCount ));
    bool const tilesCountWithinLimits = tilesCount > 0 && tilesCount <= maxTilesCount;
    d->m_okButton->setEnabled( tilesCountWithinLimits );
    d->m_applyButton->setEnabled( tilesCountWithinLimits );
}

}

#include "DownloadRegionDialog.moc"
