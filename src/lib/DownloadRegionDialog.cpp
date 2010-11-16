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
#include <QtGui/QSpinBox>
#include <QtGui/QScrollArea>
#include <QtCore/QSet>

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
#include "routing/RoutingManager.h"
#include "routing/RoutingModel.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLineString.h"

namespace Marble
{

int const maxTilesCount = 100000;
int const minimumRouteOffset = 0;
int const maximumRouteOffset = 10000;
int averageTileSize = 13; //The average size of a tile in kilobytes

class DownloadRegionDialog::Private
{
public:
    Private( MarbleModel *const model, QDialog * const dialog );
    QWidget * createSelectionMethodBox();
    QLayout * createTilesCounter();
    QWidget * createOkCancelButtonBox();

    int rad2PixelX( qreal const lon ) const;
    int rad2PixelY( qreal const lat ) const;
    AbstractScanlineTextureMapper const * textureMapper() const;
    bool hasRoute() const;
    QDialog * m_dialog;
    QRadioButton * m_visibleRegionMethodButton;
    QRadioButton * m_specifiedRegionMethodButton;
    LatLonBoxWidget * m_latLonBoxWidget;
    TileLevelRangeWidget * m_tileLevelRangeWidget;
    QRadioButton *m_routeDownloadMethodButton;
    QLabel* m_routeOffsetLabel;
    QDoubleSpinBox *m_routeOffsetSpinBox;
    QLabel * m_tilesCountLabel;
    QLabel * m_tileSizeInfo;
    QPushButton * m_okButton;
    QPushButton * m_applyButton;
    int m_visibleTileLevel;
    MarbleModel const*const m_model;
    GeoSceneTexture const * m_textureLayer;
    SelectionMethod m_selectionMethod;
    GeoDataLatLonBox m_visibleRegion;
    RoutingModel *m_routingModel;
};

DownloadRegionDialog::Private::Private( MarbleModel * const model,
                                        QDialog * const dialog )
    : m_dialog( dialog ),
      m_visibleRegionMethodButton( 0 ),
      m_specifiedRegionMethodButton( 0 ),
      m_latLonBoxWidget( new LatLonBoxWidget ),
      m_tileLevelRangeWidget( new TileLevelRangeWidget ),
      m_routeDownloadMethodButton( 0 ),
      m_routeOffsetLabel( 0 ),
      m_routeOffsetSpinBox( 0 ),
      m_tilesCountLabel( 0 ),
      m_tileSizeInfo( 0 ),
      m_okButton( 0 ),
      m_applyButton( 0 ),
      m_visibleTileLevel( model->textureMapper()->tileZoomLevel() ),
      m_model( model ),
      m_textureLayer( model->textureMapper()->textureLayer() ),
      m_selectionMethod( VisibleRegionMethod ),
      m_visibleRegion(),
      m_routingModel( model->routingManager()->routingModel() )
{
    m_latLonBoxWidget->setEnabled( false );
    m_latLonBoxWidget->setLatLonBox( m_visibleRegion );
    m_tileLevelRangeWidget->setDefaultLevel( m_visibleTileLevel );
}

QWidget * DownloadRegionDialog::Private::createSelectionMethodBox()
{
    m_visibleRegionMethodButton = new QRadioButton( tr( "Visible region" ) );
    m_specifiedRegionMethodButton = new QRadioButton( tr( "Specify region" ) );

    m_routeDownloadMethodButton = new QRadioButton( tr( "Download Route" ) );
    m_routeDownloadMethodButton->setToolTip( tr( "Enabled when a route exists" ) );
    m_routeDownloadMethodButton->setEnabled( hasRoute() );
    m_routeDownloadMethodButton->setChecked( hasRoute() );
    m_routeOffsetSpinBox = new QDoubleSpinBox();
    m_routeOffsetSpinBox->setEnabled( hasRoute() );
    m_routeOffsetSpinBox->setRange( minimumRouteOffset, maximumRouteOffset );
    int defaultOffset = 500;
    m_routeOffsetSpinBox->setValue( defaultOffset );
    m_routeOffsetSpinBox->setSingleStep( 100 );
    m_routeOffsetSpinBox->setSuffix( " m" );
    m_routeOffsetSpinBox->setDecimals( 0 );
    m_routeOffsetSpinBox->setAlignment( Qt::AlignRight );

    m_routeOffsetLabel = new QLabel( tr( "Offset from route:" ) );
    m_routeOffsetLabel->setAlignment( Qt::AlignHCenter );

    connect( m_visibleRegionMethodButton, SIGNAL( toggled( bool ) ),
             m_dialog, SLOT( toggleSelectionMethod() ) );
    connect( m_specifiedRegionMethodButton, SIGNAL( toggled( bool ) ),
             m_dialog, SLOT( toggleSelectionMethod() ));
    connect( m_routeDownloadMethodButton, SIGNAL( toggled( bool ) ),
             m_dialog, SLOT( toggleSelectionMethod() ) );
    connect( m_routingModel, SIGNAL( modelReset() ), m_dialog, SLOT( updateRouteDialog() ) );
    connect( m_routingModel, SIGNAL( rowsInserted( QModelIndex, int, int ) ),
             m_dialog, SLOT( updateRouteDialog() ) );
    connect( m_routingModel, SIGNAL( rowsRemoved( QModelIndex, int, int ) ),
             m_dialog, SLOT( updateRouteDialog() ) );

    QHBoxLayout *routeOffsetLayout = new QHBoxLayout;
    routeOffsetLayout->addWidget( m_routeOffsetLabel );
    routeOffsetLayout->insertSpacing( 0, 25 );
    routeOffsetLayout->addWidget( m_routeOffsetSpinBox );

    QVBoxLayout * const routeLayout = new QVBoxLayout;
    routeLayout->addWidget( m_routeDownloadMethodButton );
    routeLayout->addLayout( routeOffsetLayout );

    QVBoxLayout * const layout = new QVBoxLayout;
    layout->addWidget( m_visibleRegionMethodButton );
    layout->addLayout( routeLayout );
    layout->addWidget( m_specifiedRegionMethodButton );
    layout->addWidget( m_latLonBoxWidget );

    bool const smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    m_specifiedRegionMethodButton->setVisible( !smallScreen );
    m_latLonBoxWidget->setVisible( !smallScreen );

    if ( smallScreen ) {
        QWidget * const selectionMethodWidget = new QWidget;
        selectionMethodWidget->setLayout( layout );
        return selectionMethodWidget;
    } else {
        QGroupBox * const selectionMethodBox = new QGroupBox( tr( "Selection Method" ) );
        selectionMethodBox->setLayout( layout );
        return selectionMethodBox;
    }
}

QLayout * DownloadRegionDialog::Private::createTilesCounter()
{
    QLabel * const description = new QLabel( tr( "Number of tiles to download:" ) );
    m_tilesCountLabel = new QLabel;
    m_tileSizeInfo = new QLabel;

    QHBoxLayout * const tilesCountLayout = new QHBoxLayout;
    tilesCountLayout->addWidget( description );
    tilesCountLayout->addWidget( m_tilesCountLabel );
    //tilesCountLayout->insertSpacing( 0, 5 );
    QVBoxLayout * const layout = new QVBoxLayout;
    layout->addLayout( tilesCountLayout );
    layout->addWidget( m_tileSizeInfo );
    return layout;
}

QWidget * DownloadRegionDialog::Private::createOkCancelButtonBox()
{
    QDialogButtonBox * const buttonBox = new QDialogButtonBox;
    m_okButton = buttonBox->addButton( QDialogButtonBox::Ok );
    m_applyButton = buttonBox->addButton( QDialogButtonBox::Apply );
    if ( MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen ) {
        buttonBox->removeButton( m_applyButton );
        m_applyButton->setVisible( false );
    }
    buttonBox->addButton( QDialogButtonBox::Cancel );
    connect( buttonBox, SIGNAL( accepted() ), m_dialog, SLOT( accept() ) );
    connect( buttonBox, SIGNAL( rejected() ), m_dialog, SLOT( reject() ) );
    connect( m_applyButton, SIGNAL( clicked() ), m_dialog, SIGNAL( applied() ) );
    return buttonBox;
}

// copied from AbstractScanlineTextureMapper and slightly adjusted
int DownloadRegionDialog::Private::rad2PixelX( qreal const lon ) const
{
    qreal const globalWidth = m_textureLayer->tileSize().width()
        * TileLoaderHelper::levelToColumn( m_textureLayer->levelZeroColumns(),
                                           m_visibleTileLevel );
    return static_cast<int>( globalWidth * 0.5 + lon * ( globalWidth / ( 2.0 * M_PI ) ) );
}

// copied from AbstractScanlineTextureMapper and slightly adjusted
int DownloadRegionDialog::Private::rad2PixelY( qreal const lat ) const
{
    qreal const globalHeight = m_textureLayer->tileSize().height()
        * TileLoaderHelper::levelToRow( m_textureLayer->levelZeroRows(), m_visibleTileLevel );
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

bool DownloadRegionDialog::Private::hasRoute() const
{
    GeoDataLineString waypoints;

    for( int i = 0; i < m_routingModel->rowCount(); ++i ) {
        QModelIndex index = m_routingModel->index( i, 0 );
        GeoDataCoordinates position = qVariantValue<GeoDataCoordinates>( index.data( RoutingModel::CoordinateRole ) );
        RoutingModel::RoutingItemType type = qVariantValue<RoutingModel::RoutingItemType>( index.data( RoutingModel::TypeRole ) );

        if ( type == RoutingModel::WayPoint ) {
            return true;
        }
    }
    return false;
}

DownloadRegionDialog::DownloadRegionDialog( MarbleModel *const model, QWidget * const parent,
                                            Qt::WindowFlags const f )
    : QDialog( parent, f ),
      d( new Private( model, this ))
{
    setWindowTitle( tr( "Download Region" ));
    QVBoxLayout * const layout = new QVBoxLayout;
    layout->addWidget( d->createSelectionMethodBox() );
    layout->addWidget( d->m_tileLevelRangeWidget );
    layout->addLayout( d->createTilesCounter() );

    if ( MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen ) {
        QWidget* widget = new QWidget( this );
        widget->setLayout( layout );
        QScrollArea* scrollArea = new QScrollArea( this );
        scrollArea->setFrameShape( QFrame::NoFrame );
        scrollArea->setWidget( widget );
        QVBoxLayout * const mainLayout = new QVBoxLayout;
        mainLayout->addWidget( scrollArea );
        mainLayout->addWidget( d->createOkCancelButtonBox() );
        setLayout( mainLayout );
    } else {
        layout->addWidget( d->createOkCancelButtonBox() );
        setLayout( layout );
    }

    connect( d->m_latLonBoxWidget, SIGNAL( valueChanged() ), SLOT( updateTilesCount() ) );
    connect( d->m_tileLevelRangeWidget, SIGNAL( topLevelChanged( int ) ),
             SLOT( updateTilesCount() ) );
    connect( d->m_tileLevelRangeWidget, SIGNAL( bottomLevelChanged( int )),
             SLOT( updateTilesCount() ) );
    connect( d->m_routeOffsetSpinBox, SIGNAL( valueChanged( double ) ), SLOT( updateTilesCount() ) );
    connect( d->m_routeOffsetSpinBox, SIGNAL( valueChanged( double ) ), SLOT( setOffsetUnit() ) );
    connect( d->m_model, SIGNAL( themeChanged( QString ) ), SLOT( updateTilesCount() ) );
}

DownloadRegionDialog::~DownloadRegionDialog()
{
    delete d;
}

void DownloadRegionDialog::setAllowedTileLevelRange( int const minimumTileLevel,
                                                     int const maximumTileLevel )
{
    d->m_tileLevelRangeWidget->setAllowedLevelRange( minimumTileLevel, maximumTileLevel );
}

void DownloadRegionDialog::setVisibleTileLevel( int const tileLevel )
{
    d->m_visibleTileLevel = tileLevel;
    d->m_tileLevelRangeWidget->setDefaultLevel( tileLevel );
}

void DownloadRegionDialog::setSelectionMethod( SelectionMethod const selectionMethod )
{
    // block signals to prevent infinite recursion:
    // radioButton->setChecked() -> toggleSelectionMethod() -> setSelectionMethod()
    //     -> radioButton->setChecked() -> ...
    d->m_visibleRegionMethodButton->blockSignals( true );
    d->m_specifiedRegionMethodButton->blockSignals( true );
    d->m_routeDownloadMethodButton->blockSignals( true );

    d->m_selectionMethod = selectionMethod;
    switch ( selectionMethod ) {
    case VisibleRegionMethod:
        d->m_visibleRegionMethodButton->setChecked( true );
        d->m_routeOffsetLabel->setEnabled( false );
        d->m_routeOffsetSpinBox->setEnabled( false );
        d->m_latLonBoxWidget->setEnabled( false );
        setSpecifiedLatLonAltBox( d->m_visibleRegion );
        break;
    case SpecifiedRegionMethod:
        d->m_specifiedRegionMethodButton->setChecked( true );
        d->m_routeOffsetLabel->setEnabled( false );
        d->m_routeOffsetSpinBox->setEnabled( false );
        d->m_latLonBoxWidget->setEnabled( true );
        break;
    case RouteDownloadMethod:
        d->m_routeDownloadMethodButton->setChecked( true );
        d->m_routeOffsetLabel->setEnabled( true );
        d->m_routeOffsetSpinBox->setEnabled( true );
        d->m_latLonBoxWidget->setEnabled( false );
    }

    updateTilesCount();
    d->m_visibleRegionMethodButton->blockSignals( false );
    d->m_specifiedRegionMethodButton->blockSignals( false );
    d->m_routeDownloadMethodButton->blockSignals( false );
}

QVector<TileCoordsPyramid> DownloadRegionDialog::region() const
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
   case RouteDownloadMethod:
        QVector<TileCoordsPyramid> pyramid;
        pyramid =  routeRegion();
        if( !pyramid.isEmpty() ) {
            return pyramid;
        }
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

    int const tileWidth = d->m_textureLayer->tileSize().width();
    int const tileHeight = d->m_textureLayer->tileSize().height();
    mDebug() << "DownloadRegionDialog downloadRegion: tileSize:" << tileWidth << tileHeight;

    int const visibleLevelX1 = qMin( westX, eastX );
    int const visibleLevelY1 = qMin( northY, southY );
    int const visibleLevelX2 = qMax( westX, eastX );
    int const visibleLevelY2 = qMax( northY, southY );

    mDebug() << "visible level pixel coords (level/x1/y1/x2/y2):" << d->m_visibleTileLevel
             << visibleLevelX1 << visibleLevelY1 << visibleLevelX2 << visibleLevelY2;

    int bottomLevelX1, bottomLevelY1, bottomLevelX2, bottomLevelY2;
    // the pixel coords calculated above are referring to the visible tile level,
    // if the bottom level is a different level, we have to take it into account
    if ( d->m_visibleTileLevel > d->m_tileLevelRangeWidget->bottomLevel() ) {
        int const deltaLevel = d->m_visibleTileLevel - d->m_tileLevelRangeWidget->bottomLevel();
        bottomLevelX1 = visibleLevelX1 >> deltaLevel;
        bottomLevelY1 = visibleLevelY1 >> deltaLevel;
        bottomLevelX2 = visibleLevelX2 >> deltaLevel;
        bottomLevelY2 = visibleLevelY2 >> deltaLevel;
    }
    else if ( d->m_visibleTileLevel < d->m_tileLevelRangeWidget->bottomLevel() ) {
        int const deltaLevel = d->m_tileLevelRangeWidget->bottomLevel() - d->m_visibleTileLevel;
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
    mDebug() << "tiles count:" << coordsPyramid.tilesCount( );
    QVector<TileCoordsPyramid> pyramid;
    pyramid << coordsPyramid;
    return pyramid;
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
    // TODO:QButtonGroup would be easier to handle
    switch ( d->m_selectionMethod ) {
    case VisibleRegionMethod:
        if( d->m_specifiedRegionMethodButton->isChecked() ) {
            setSelectionMethod( SpecifiedRegionMethod );
        }
        else if( d->m_routeDownloadMethodButton->isChecked() ) {
            setSelectionMethod( RouteDownloadMethod );
        }

        break;
    case SpecifiedRegionMethod:
        if( d->m_visibleRegionMethodButton->isChecked() ) {
            setSelectionMethod( VisibleRegionMethod );
        }
        else if ( d->m_routeDownloadMethodButton->isChecked() ) {
            setSelectionMethod( RouteDownloadMethod );
        }
        break;
    case RouteDownloadMethod:
        if( d->m_specifiedRegionMethodButton->isChecked() ) {
            setSelectionMethod( SpecifiedRegionMethod );
        }
        else if ( d->m_visibleRegionMethodButton->isChecked() ) {
            setSelectionMethod( VisibleRegionMethod );
        }
        break;

    }
}

void DownloadRegionDialog::updateTilesCount()
{
    qint64 tilesCount = 0;
    QString themeId( d->m_model->mapThemeId() );
    if ( d->m_textureLayer ) {
        QVector<TileCoordsPyramid> const pyramid = region();
        Q_ASSERT( !pyramid.isEmpty() );
        if( pyramid.size() == 1 ) {
            tilesCount = pyramid[0].tilesCount();
        }
        else {
            for( int level = pyramid[0].bottomLevel(); level>= pyramid[0].topLevel(); --level ) {
                QSet<TileId> tileIdSet;
                for( int i = 0; i < pyramid.size(); ++i ) {
                    QRect const coords = pyramid[i].coords( level );
                    int x1, y1, x2, y2;
                    coords.getCoords( &x1, &y1, &x2, &y2 );
                    for ( int x = x1; x <= x2; ++x ) {
                        for ( int y = y1; y <= y2; ++y ) {
                            TileId const tileId( themeId, level, x, y );
                            tileIdSet.insert( tileId );
                        }
                    }
                }
                tilesCount += tileIdSet.count();
            }
        }
    }
    else {
        tilesCount = 0;
    }

    if ( tilesCount > maxTilesCount ) {
        d->m_tileSizeInfo->setToolTip( QString::null  );
        d->m_tileSizeInfo->setText( tr( "There is a limit of %n tiles to download.", "",
                                               maxTilesCount ) );
    } else if ( themeId == "earth/openstreetmap/openstreetmap.dgml" ) {
        qreal tileDownloadSize = tilesCount * averageTileSize;

        d->m_tileSizeInfo->setToolTip( tr( "Approximate size of the tiles to be downloaded" ) );

        if( tileDownloadSize > 1024 ) {
            tileDownloadSize = tileDownloadSize / 1024;
            d->m_tileSizeInfo->setText( tr( "Estimated download size: %1 MB" ).arg( ceil( tileDownloadSize ) ) );
        }
        else {
            d->m_tileSizeInfo->setText( tr( "Estimated download size: %1 kB" ).arg( tileDownloadSize ) );
        }

    }
    else {
        d->m_tileSizeInfo->setToolTip( QString::null );
        d->m_tileSizeInfo->clear();
    }

    d->m_tilesCountLabel->setText( QString::number( tilesCount ) );
    bool const tilesCountWithinLimits = tilesCount > 0 && tilesCount <= maxTilesCount;
    d->m_okButton->setEnabled( tilesCountWithinLimits );
    d->m_applyButton->setEnabled( tilesCountWithinLimits );
}

QVector<TileCoordsPyramid> DownloadRegionDialog::routeRegion() const
{
    if( d->m_routingModel->rowCount() == 0 ) {
         return QVector<TileCoordsPyramid>();
    }
    GeoDataLineString waypoints;
    for( int i = 0; i < d->m_routingModel->rowCount(); ++i ) {
        QModelIndex index = d->m_routingModel->index( i, 0 );
        GeoDataCoordinates position = qVariantValue<GeoDataCoordinates>( index.data( RoutingModel::CoordinateRole ) );
        RoutingModel::RoutingItemType type = qVariantValue<RoutingModel::RoutingItemType>( index.data( RoutingModel::TypeRole ) );

        if ( type == RoutingModel::WayPoint ) {
            waypoints << position;
        }
    }
    int const topLevel = d->m_tileLevelRangeWidget->topLevel();
    int const bottomLevel = d->m_tileLevelRangeWidget->bottomLevel();
    TileCoordsPyramid coordsPyramid( topLevel, bottomLevel );

    int const tileWidth = d->m_textureLayer->tileSize().width();
    int const tileHeight = d->m_textureLayer->tileSize().height();

    qreal offset = d->m_routeOffsetSpinBox->value();
    if( d->m_routeOffsetSpinBox->suffix() == " km") {
        offset *= KM2METER;
    }
    qreal radius = d->m_model->planetRadius();
    QVector<TileCoordsPyramid> pyramid;
    qreal radianOffset = offset / radius;

    for( int i = 1; i < waypoints.size(); ++i ) {
        GeoDataCoordinates position = waypoints[i];
        qreal lonCenter = position.longitude();
        qreal latCenter = position.latitude();

        // coordinates of the of the vertices of the square(topleft and bottomright) at an offset distance from the waypoint
        qreal latNorth = asin( sin( latCenter ) *  cos( radianOffset ) +  cos( latCenter ) * sin( radianOffset )  * cos( 7*M_PI/4 ) );
        qreal dlonWest = atan2( sin( 7*M_PI/4 ) * sin( radianOffset ) * cos( latCenter ),  cos( radianOffset ) -  sin( latCenter ) * sin( latNorth ) );
        qreal lonWest  = fmod( lonCenter - dlonWest + M_PI, 2*M_PI ) - M_PI;
        qreal latSouth = asin( sin( latCenter ) * cos( radianOffset ) + cos( latCenter ) * sin( radianOffset ) * cos( 3*M_PI/4 ) );
        qreal dlonEast =  atan2( sin( 3*M_PI/4 ) * sin( radianOffset ) * cos( latCenter ),  cos( radianOffset ) -  sin( latCenter ) * sin( latSouth ) );
        qreal lonEast  = fmod( lonCenter - dlonEast+M_PI, 2*M_PI ) - M_PI;

        int const northY = d->rad2PixelY( latNorth );
        int const southY = d->rad2PixelY( latSouth );
        int const eastX =  d->rad2PixelX( lonEast );
        int const westX =  d->rad2PixelX( lonWest );

        int const west  = qMin( westX, eastX );
        int const north = qMin( northY, southY );
        int const east  = qMax( westX, eastX );
        int const south = qMax( northY, southY );

        int bottomLevelTileX1 = 0;
        int bottomLevelTileY1 = 0;
        int bottomLevelTileX2 = 0;
        int bottomLevelTileY2 = 0;

        if ( d->m_visibleTileLevel > d->m_tileLevelRangeWidget->bottomLevel() ) {
            int const deltaLevel = d->m_visibleTileLevel - d->m_tileLevelRangeWidget->bottomLevel();
            bottomLevelTileX1 = west  >> deltaLevel;
            bottomLevelTileY1 = north >> deltaLevel;
            bottomLevelTileX2 = east  >> deltaLevel;
            bottomLevelTileY2 = south >> deltaLevel;
        }
        else if ( d->m_visibleTileLevel < bottomLevel ) {
            int const deltaLevel = bottomLevel - d->m_visibleTileLevel;
            bottomLevelTileX1 = west  << deltaLevel;
            bottomLevelTileY1 = north << deltaLevel;
            bottomLevelTileX2 = east  << deltaLevel;
            bottomLevelTileY2 = south << deltaLevel;
        }
        else {
            bottomLevelTileX1 = west;
            bottomLevelTileY1 = north;
            bottomLevelTileX2 = east;
            bottomLevelTileY2 = south;
        }

        QRect waypointRegion;
        //square region around the waypoint
        waypointRegion.setCoords( bottomLevelTileX1/tileWidth, bottomLevelTileY1/tileHeight,
                                  bottomLevelTileX2/tileWidth, bottomLevelTileY2/tileHeight );
        coordsPyramid.setBottomLevelCoords( waypointRegion );
        pyramid << coordsPyramid;
        }

        return pyramid;
}


void DownloadRegionDialog::updateRouteDialog()
{
    d->m_routeDownloadMethodButton->setEnabled( d->hasRoute() );
    d->m_routeDownloadMethodButton->setChecked( d->hasRoute() );
    if( !d->hasRoute() ) {
        setSelectionMethod( VisibleRegionMethod );
    }
}

void DownloadRegionDialog::setOffsetUnit()
{
    qreal offset = d->m_routeOffsetSpinBox->value();

    if( offset >= 1100 ) {
        d->m_routeOffsetSpinBox->setSuffix( " km" );
        d->m_routeOffsetSpinBox->setRange( minimumRouteOffset * METER2KM, maximumRouteOffset * METER2KM );
        d->m_routeOffsetSpinBox->setDecimals( 1 );
        d->m_routeOffsetSpinBox->setValue( offset * METER2KM );
        d->m_routeOffsetSpinBox->setSingleStep( 0.1 );
    }
    else if( offset <= 1 && d->m_routeOffsetSpinBox->suffix() == " km" ) {
        d->m_routeOffsetSpinBox->setSuffix( " m" );
        d->m_routeOffsetSpinBox->setRange( minimumRouteOffset, maximumRouteOffset );
        d->m_routeOffsetSpinBox->setDecimals( 0 );
        d->m_routeOffsetSpinBox->setValue( offset * KM2METER );
        d->m_routeOffsetSpinBox->setSingleStep( 100 );
    }
}

}

#include "DownloadRegionDialog.moc"
