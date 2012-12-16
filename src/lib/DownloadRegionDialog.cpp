//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Jens-Michael Hoffmann <jmho@c-xx.com>
//

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

#include "GeoDataLatLonAltBox.h"
#include "MarbleDebug.h"
#include "MarbleMath.h"
#include "MarbleModel.h"
#include "MarbleWidget.h"
#include "LatLonBoxWidget.h"
#include "TextureLayer.h"
#include "TileId.h"
#include "TileLevelRangeWidget.h"
#include "TileLoaderHelper.h"
#include "routing/RoutingManager.h"
#include "routing/RoutingModel.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLineString.h"
#include "DownloadRegion.h"
#include "GeoSceneDocument.h"
#include "GeoSceneMap.h"

namespace Marble
{

int const maxTilesCount = 100000;
int const minimumRouteOffset = 0;
int const maximumRouteOffset = 10000;
int averageTileSize = 13; //The average size of a tile in kilobytes

class DownloadRegionDialog::Private
{
public:
    Private( MarbleWidget *const widget, QDialog * const dialog );
    QWidget * createSelectionMethodBox();
    QLayout * createTilesCounter();
    QWidget * createOkCancelButtonBox();

    bool hasRoute() const;
    bool hasTextureLayer() const;
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
    TextureLayer const * m_textureLayer;
    int m_visibleTileLevel;
    MarbleModel const*const m_model;
    MarbleWidget *const m_widget;
    SelectionMethod m_selectionMethod;
    GeoDataLatLonBox m_visibleRegion;
    RoutingModel *m_routingModel;
    DownloadRegion m_downloadRegion;
};

DownloadRegionDialog::Private::Private( MarbleWidget * const widget,
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
      m_textureLayer( widget->textureLayer() ),
      m_visibleTileLevel( m_textureLayer->tileZoomLevel() ),
      m_model( widget->model() ),
      m_widget( widget ),
      m_selectionMethod( VisibleRegionMethod ),
      m_visibleRegion(),
      m_routingModel( widget->model()->routingManager()->routingModel() )
{
    m_latLonBoxWidget->setEnabled( false );
    m_latLonBoxWidget->setLatLonBox( m_visibleRegion );
    m_tileLevelRangeWidget->setDefaultLevel( m_visibleTileLevel );
    m_downloadRegion.setMarbleModel( widget->model() );
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

bool DownloadRegionDialog::Private::hasRoute() const
{
    return !m_routingModel->route().path().isEmpty();
}

bool DownloadRegionDialog::Private::hasTextureLayer() const
{
    return m_model->mapTheme()->map()->hasTextureLayers();
}

DownloadRegionDialog::DownloadRegionDialog( MarbleWidget *const widget, QWidget * const parent,
                                            Qt::WindowFlags const f )
    : QDialog( parent, f ),
      d( new Private( widget, this ))
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
    d->m_downloadRegion.setVisibleTileLevel( tileLevel );
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
    if ( !d->hasTextureLayer() ) {
        return QVector<TileCoordsPyramid>();
    }

    d->m_downloadRegion.setTileLevelRange( d->m_tileLevelRangeWidget->topLevel(),
                                           d->m_tileLevelRangeWidget->bottomLevel() );
    d->m_downloadRegion.setVisibleTileLevel( d->m_visibleTileLevel );
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
        qreal offset = d->m_routeOffsetSpinBox->value();
        if( d->m_routeOffsetSpinBox->suffix() == " km") {
            offset *= KM2METER;
        }
        return d->m_downloadRegion.routeRegion( d->m_textureLayer, offset );
        break;
    }

    return d->m_downloadRegion.region( d->m_textureLayer, downloadRegion );
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
    if ( !isVisible() || !d->hasTextureLayer() ) {
        return;
    }

    qint64 tilesCount = 0;
    QString themeId( d->m_model->mapThemeId() );
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
                        TileId const tileId( 0, level, x, y );
                        tileIdSet.insert( tileId );
                    }
                }
            }
            tilesCount += tileIdSet.count();
        }
    }

    if ( tilesCount > maxTilesCount ) {
        d->m_tileSizeInfo->setToolTip( QString() );
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
        d->m_tileSizeInfo->setToolTip( QString() );
        d->m_tileSizeInfo->clear();
    }

    d->m_tilesCountLabel->setText( QString::number( tilesCount ) );
    bool const tilesCountWithinLimits = tilesCount > 0 && tilesCount <= maxTilesCount;
    d->m_okButton->setEnabled( tilesCountWithinLimits );
    d->m_applyButton->setEnabled( tilesCountWithinLimits );
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
