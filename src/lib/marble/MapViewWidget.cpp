//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn  <tackat@kde.org>
// Copyright 2007      Inge Wallin   <ingwa@kde.org>
// Copyright 2007      Thomas Zander <zander@kde.org>
// Copyright 2010      Bastian Holst <bastianholst@gmx.de>
// Coprright 2011-2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// Copyright 2012      Illya Kovalevskyy  <illya.kovalevskyy@gmail.com>
//

// Self
#include "MapViewWidget.h"

// Marble
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "MarbleWidget.h"
#include "MarbleModel.h"
#include "MapThemeManager.h"
#include "MapThemeSortFilterProxyModel.h"
#include "GeoSceneDocument.h"
#include "GeoSceneHead.h"
#include "MapViewItemDelegate.h"
#include "CelestialSortFilterProxyModel.h"

// Qt
#include <QResizeEvent>
#include <QFileInfo>
#include <QSettings>
#include <QMenu>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QGridLayout>
#include <QPainter>
#include <QToolBar>
#include <QToolButton>
#include <QDateTime>

using namespace Marble;
// Ui
#include "ui_MapViewWidget.h"

namespace Marble
{

class Q_DECL_HIDDEN MapViewWidget::Private {
 public:
    Private( MapViewWidget *parent )
        : q( parent ),
          m_marbleModel( 0 ),
          m_mapSortProxy(),
          m_celestialListProxy(),
          m_toolBar( 0 ),
          m_globeViewButton( 0 ),
          m_mercatorViewButton( 0 ),
          m_popupMenuFlat( 0 ),
          m_flatViewAction( 0 ),
          m_mercatorViewAction( 0 ),
          m_celestialBodyAction( 0 ),
          m_gnomonicViewAction( 0 ),
          m_stereographicViewAction( 0 ),
          m_lambertAzimuthalViewAction( 0 ),
          m_azimuthalEquidistantViewAction( 0 ),
          m_verticalPerspectiveViewAction( 0 ),
          m_globeViewAction( 0 )
    {
        m_mapSortProxy.setDynamicSortFilter( true );
        m_celestialListProxy.setDynamicSortFilter( true );
    }

    void applyExtendedLayout()
    {
        m_mapViewUi.projectionLabel_2->setVisible(true);
        m_mapViewUi.celestialBodyLabel->setVisible(true);
        m_mapViewUi.projectionComboBox->setVisible(true);
        m_mapViewUi.mapThemeLabel->setVisible(true);
        m_mapViewUi.line->setVisible(true);

        m_toolBar->setVisible(false);
        const int labelId = m_mapViewUi.verticalLayout->indexOf(m_mapViewUi.celestialBodyLabel);
        m_mapViewUi.verticalLayout->insertWidget(labelId+1, m_mapViewUi.celestialBodyComboBox);
        m_toolBar->removeAction(m_celestialBodyAction);
        m_mapViewUi.celestialBodyComboBox->show();
    }

    void applyReducedLayout()
    {
        m_mapViewUi.projectionLabel_2->setVisible(false);
        m_mapViewUi.celestialBodyLabel->setVisible(false);
        m_mapViewUi.projectionComboBox->setVisible(false);
        m_mapViewUi.mapThemeLabel->setVisible(false);
        m_mapViewUi.line->setVisible(false);

        m_toolBar->setVisible(true);
        m_celestialBodyAction = m_toolBar->addWidget(m_mapViewUi.celestialBodyComboBox);
        m_mapViewUi.verticalLayout->removeWidget(m_mapViewUi.celestialBodyComboBox);
        m_mapViewUi.celestialBodyComboBox->show();
    }

    void setupToolBar()
    {
        m_toolBar = new QToolBar;

        m_globeViewButton = new QToolButton;
        m_globeViewButton->setIcon(QIcon(QStringLiteral(":/icons/map-globe.png")));
        m_globeViewButton->setToolTip( tr("Globe View") );
        m_globeViewButton->setCheckable(true);
        m_globeViewButton->setChecked(false);

        m_globeViewAction = new QAction(QIcon(QStringLiteral(":/icons/map-globe.png")),
                                             tr( "Spherical view" ),
                                             m_globeViewButton );
        m_globeViewAction->setCheckable( true );
        m_globeViewAction->setChecked( false );

        m_mercatorViewButton = new QToolButton;
        m_mercatorViewButton->setIcon(QIcon(QStringLiteral(":/icons/map-mercator.png")));
        m_mercatorViewButton->setToolTip( tr("Mercator View") );
        m_mercatorViewButton->setCheckable(true);
        m_mercatorViewButton->setChecked(false);
        m_mercatorViewButton->setPopupMode(QToolButton::MenuButtonPopup);

        m_popupMenuFlat = new QMenu;

        m_mercatorViewAction = new QAction(QIcon(QStringLiteral(":/icons/map-mercator.png")),
                                              tr("Mercator View"),
                                              m_popupMenuFlat );
        m_mercatorViewAction->setCheckable(true);
        m_mercatorViewAction->setChecked(false);

        m_flatViewAction = new QAction( QIcon(QStringLiteral(":/icons/map-flat.png")),
                                        tr("Flat View"),
                                        m_popupMenuFlat );
        m_flatViewAction->setCheckable(true);
        m_flatViewAction->setChecked(false);

        m_gnomonicViewAction = new QAction( QIcon(QStringLiteral(":/icons/map-gnomonic.png")),
                                            tr( "Gnomonic view" ),
                                            m_popupMenuFlat);
        m_gnomonicViewAction->setCheckable( true );
        m_gnomonicViewAction->setChecked( false );

        m_stereographicViewAction = new QAction(QIcon(QStringLiteral(":/icons/map-globe.png")),
                                            tr( "Stereographic view" ),
                                            m_popupMenuFlat);
        m_stereographicViewAction->setCheckable( true );
        m_stereographicViewAction->setChecked( false );

        m_lambertAzimuthalViewAction = new QAction(QIcon(QStringLiteral(":/icons/map-globe.png")),
                                            tr( "Lambert Azimuthal Equal-Area view" ),
                                            m_popupMenuFlat);
        m_lambertAzimuthalViewAction->setCheckable( true );
        m_lambertAzimuthalViewAction->setChecked( false );

        m_azimuthalEquidistantViewAction = new QAction(QIcon(QStringLiteral(":/icons/map-globe.png")),
                                            tr( "Azimuthal Equidistant view" ),
                                            m_popupMenuFlat);
        m_azimuthalEquidistantViewAction->setCheckable( true );
        m_azimuthalEquidistantViewAction->setChecked( false );

        m_verticalPerspectiveViewAction = new QAction(QIcon(QStringLiteral(":/icons/map-globe.png")),
                                            tr( "Perspective Globe view" ),
                                            m_popupMenuFlat);
        m_verticalPerspectiveViewAction->setCheckable( true );
        m_verticalPerspectiveViewAction->setChecked( false );


        m_popupMenuFlat->addAction(m_mercatorViewAction);
        m_popupMenuFlat->addAction(m_flatViewAction);
        m_popupMenuFlat->addAction(m_gnomonicViewAction);
        m_popupMenuFlat->addAction(m_stereographicViewAction);
        m_popupMenuFlat->addAction(m_lambertAzimuthalViewAction);
        m_popupMenuFlat->addAction(m_azimuthalEquidistantViewAction);
        m_popupMenuFlat->addAction(m_verticalPerspectiveViewAction);
        m_mercatorViewButton->setMenu(m_popupMenuFlat);

        m_toolBar->addWidget(m_globeViewButton);
        m_toolBar->addWidget(m_mercatorViewButton);
        m_toolBar->addSeparator();
        m_toolBar->setContentsMargins(0,0,0,0);
        m_toolBar->setIconSize(QSize(16, 16));
        m_mapViewUi.toolBarLayout->insertWidget(0, m_toolBar);

        QObject::connect(m_globeViewButton, SIGNAL(clicked()),
                         q, SLOT(globeViewRequested()));
        QObject::connect(m_mercatorViewButton, SIGNAL(clicked()),
                         q, SLOT(mercatorViewRequested()));
        QObject::connect(m_mercatorViewAction, SIGNAL(triggered()),
                         q, SLOT(mercatorViewRequested()));
        QObject::connect(m_flatViewAction, SIGNAL(triggered()),
                         q, SLOT(flatViewRequested()));
        QObject::connect(m_gnomonicViewAction, SIGNAL(triggered()),
                         q, SLOT(gnomonicViewRequested()));
        QObject::connect(m_stereographicViewAction, SIGNAL(triggered()),
                         q, SLOT(stereographicViewRequested()));
        QObject::connect(m_lambertAzimuthalViewAction, SIGNAL(triggered()),
                         q, SLOT(lambertAzimuthalViewRequested()));
        QObject::connect(m_azimuthalEquidistantViewAction, SIGNAL(triggered()),
                         q, SLOT(azimuthalEquidistantViewRequested()));
        QObject::connect(m_verticalPerspectiveViewAction, SIGNAL(triggered()),
                         q, SLOT(verticalPerspectiveViewRequested()));
        QObject::connect(m_globeViewAction, SIGNAL(triggered()),
                         q, SLOT(globeViewRequested()));

        applyReducedLayout();
    }

    void updateMapFilter()
    {
        int currentIndex = m_mapViewUi.celestialBodyComboBox->currentIndex();
        const QString selectedId = m_celestialListProxy.data( m_celestialListProxy.index( currentIndex, 1 ) ).toString();

        if ( !selectedId.isEmpty() ) {
            m_mapSortProxy.setFilterRegExp( QRegExp( selectedId, Qt::CaseInsensitive,QRegExp::FixedString ) );
        }
    }

    void celestialBodySelected( int comboIndex );

    void projectionSelected( int projectionIndex );

    void mapThemeSelected( QModelIndex index );
    void mapThemeSelected( int index );

    void showContextMenu( const QPoint& pos );
    void deleteMap();
    void toggleFavorite();
    void toggleIconSize();

    bool isCurrentFavorite() const;
    QString currentThemeName() const;
    QString currentThemePath() const;
    QString favoriteKey(const QModelIndex &index) const;

    MapViewWidget *const q;

    Ui::MapViewWidget  m_mapViewUi;
    MarbleModel      *m_marbleModel;

    MapThemeSortFilterProxyModel m_mapSortProxy;

    CelestialSortFilterProxyModel m_celestialListProxy;
    QSettings m_settings;
    QToolBar *m_toolBar;
    QToolButton *m_globeViewButton;
    QToolButton *m_mercatorViewButton;
    QMenu *m_popupMenuFlat;
    QAction *m_flatViewAction;
    QAction *m_mercatorViewAction;
    QAction *m_celestialBodyAction;
    QAction *m_gnomonicViewAction;
    QAction *m_stereographicViewAction;
    QAction *m_lambertAzimuthalViewAction;
    QAction *m_azimuthalEquidistantViewAction;
    QAction *m_verticalPerspectiveViewAction;
    QAction *m_globeViewAction;
};

MapViewWidget::MapViewWidget( QWidget *parent, Qt::WindowFlags f )
    : QWidget( parent, f ),
      d( new Private( this ) )
{
    d->m_mapViewUi.setupUi( this );
    layout()->setMargin( 0 );

    if ( MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen ) {
        QGridLayout* layout = new QGridLayout;
        layout->addItem( d->m_mapViewUi.verticalLayout->takeAt( 1 ), 0, 0 );
        layout->addItem( d->m_mapViewUi.verticalLayout->takeAt( 1 ), 0, 1 );
        d->m_mapViewUi.line->setVisible( false );
        layout->addItem( d->m_mapViewUi.verticalLayout->takeAt( 2 ), 1, 0 );
        layout->addItem( d->m_mapViewUi.verticalLayout->takeAt( 2 ), 1, 1 );
        layout->addItem( d->m_mapViewUi.verticalLayout->takeAt( 3 ), 2, 0 );
        layout->addItem( d->m_mapViewUi.verticalLayout->takeAt( 4 ), 2, 1 );
        d->m_mapViewUi.verticalLayout->insertLayout( 0, layout );
        d->m_mapViewUi.mapThemeComboBox->setModel( &d->m_mapSortProxy );
        d->m_mapViewUi.mapThemeComboBox->setIconSize( QSize( 48, 48 ) );
        connect( d->m_mapViewUi.mapThemeComboBox, SIGNAL(activated(int)),
                 this,                            SLOT(mapThemeSelected(int)) );
        d->m_mapViewUi.marbleThemeSelectView->setVisible( false );
    }
    else {
        d->m_mapViewUi.marbleThemeSelectView->setViewMode( QListView::IconMode );
        QSize const iconSize = d->m_settings.value(QStringLiteral("MapView/iconSize"), QSize(90, 90)).toSize();
        d->m_mapViewUi.marbleThemeSelectView->setIconSize( iconSize );
        d->m_mapViewUi.marbleThemeSelectView->setItemDelegate( new MapViewItemDelegate( d->m_mapViewUi.marbleThemeSelectView ) );
        d->m_mapViewUi.marbleThemeSelectView->setAlternatingRowColors( true );
        d->m_mapViewUi.marbleThemeSelectView->setFlow( QListView::LeftToRight );
        d->m_mapViewUi.marbleThemeSelectView->setWrapping( true );
        d->m_mapViewUi.marbleThemeSelectView->setResizeMode( QListView::Adjust );
        d->m_mapViewUi.marbleThemeSelectView->setUniformItemSizes( true );
        d->m_mapViewUi.marbleThemeSelectView->setMovement( QListView::Static );
        d->m_mapViewUi.marbleThemeSelectView->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
        d->m_mapViewUi.marbleThemeSelectView->setEditTriggers( QListView::NoEditTriggers );
        d->m_mapViewUi.marbleThemeSelectView->setSelectionMode( QListView::SingleSelection );
        d->m_mapViewUi.marbleThemeSelectView->setModel( &d->m_mapSortProxy );
        connect( d->m_mapViewUi.marbleThemeSelectView, SIGNAL(pressed(QModelIndex)),
                 this,                                 SLOT(mapThemeSelected(QModelIndex)) );
        connect( d->m_mapViewUi.marbleThemeSelectView, SIGNAL(customContextMenuRequested(QPoint)),
                 this,                                 SLOT(showContextMenu(QPoint)) );

        d->m_mapViewUi.mapThemeComboBox->setVisible( false );
        d->setupToolBar();
    }

    connect( d->m_mapViewUi.projectionComboBox,    SIGNAL(activated(int)),
             this,                                 SLOT(projectionSelected(int)) );

    d->m_mapViewUi.projectionComboBox->setEnabled( true );
    d->m_mapViewUi.celestialBodyComboBox->setModel( &d->m_celestialListProxy );

    connect( d->m_mapViewUi.celestialBodyComboBox, SIGNAL(activated(int)),
             this,                                 SLOT(celestialBodySelected(int)) );

    d->m_settings.beginGroup(QStringLiteral("Favorites"));
    if (!d->m_settings.contains(QStringLiteral("initialized"))) {
        d->m_settings.setValue(QStringLiteral("initialized"), true);
        QDateTime currentDateTime = QDateTime::currentDateTime();
        d->m_settings.setValue(QStringLiteral("Atlas"), currentDateTime);
        d->m_settings.setValue(QStringLiteral("OpenStreetMap"), currentDateTime);
        d->m_settings.setValue(QStringLiteral("Satellite View"), currentDateTime);
    }
    d->m_settings.endGroup();
}

MapViewWidget::~MapViewWidget()
{
    delete d;
}

void MapViewWidget::setMarbleWidget( MarbleWidget *widget, MapThemeManager *mapThemeManager )
{
    d->m_marbleModel = widget->model();
    d->m_mapSortProxy.setSourceModel( mapThemeManager->mapThemeModel() );
    d->m_mapSortProxy.sort( 0 );
    d->m_celestialListProxy.setSourceModel( mapThemeManager->celestialBodiesModel() );
    d->m_celestialListProxy.sort( 0 );

    connect( this, SIGNAL(projectionChanged(Projection)),
             widget, SLOT(setProjection(Projection)) );

    connect( widget, SIGNAL(themeChanged(QString)),
             this, SLOT(setMapThemeId(QString)) );

    connect( widget, SIGNAL(projectionChanged(Projection)),
             this, SLOT(setProjection(Projection)) );

    connect( this, SIGNAL(mapThemeIdChanged(QString)),
             widget, SLOT(setMapThemeId(QString)) );

    setProjection(widget->projection());
    setMapThemeId(widget->mapThemeId());
}

void MapViewWidget::resizeEvent(QResizeEvent *event)
{
    if (!d->m_toolBar)
        return;

    if (d->m_toolBar->isVisible() && event->size().height() > 400) {
        d->applyExtendedLayout();
    } else if (!d->m_toolBar->isVisible() && event->size().height() <= 400) {
        d->applyReducedLayout();
    }
}

void MapViewWidget::setMapThemeId( const QString &themeId )
{
    const bool smallscreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;

    const int currentRow = smallscreen ? d->m_mapViewUi.mapThemeComboBox->currentIndex() :
                                         d->m_mapViewUi.marbleThemeSelectView->currentIndex().row();
    const QString oldThemeId = d->m_mapSortProxy.data( d->m_mapSortProxy.index( currentRow, 0 ), Qt::UserRole + 1 ).toString();

    // Check if the new selected theme is different from the current one
    if ( themeId == oldThemeId )
        return;

    const QString oldCelestialBodyId = oldThemeId.section(QLatin1Char('/'), 0, 0);
    const QString celestialBodyId = themeId.section(QLatin1Char('/'), 0, 0);

    // select celestialBodyId in GUI
    if ( celestialBodyId != oldCelestialBodyId ) {
        for ( int row = 0; row < d->m_celestialListProxy.rowCount(); ++row ) {
            if ( d->m_celestialListProxy.data( d->m_celestialListProxy.index( row, 1 ) ).toString() == celestialBodyId ) {
                d->m_mapViewUi.celestialBodyComboBox->setCurrentIndex( row );
                break;
            }
        }

        d->updateMapFilter();
    }

    // select themeId in GUI
    for ( int row = 0; row < d->m_mapSortProxy.rowCount(); ++row ) {
        if( d->m_mapSortProxy.data( d->m_mapSortProxy.index( row, 0 ), Qt::UserRole + 1 ).toString() == themeId ) {
            if ( smallscreen ) {
                d->m_mapViewUi.mapThemeComboBox->setCurrentIndex( row );
            }
            else {
                const QModelIndex index = d->m_mapSortProxy.index( row, 0 );
                d->m_mapViewUi.marbleThemeSelectView->setCurrentIndex( index );
                d->m_mapViewUi.marbleThemeSelectView->scrollTo( index );
            }

            break;
        }
    }
}

void MapViewWidget::setProjection( Projection projection )
{
    if ( (int)projection != d->m_mapViewUi.projectionComboBox->currentIndex() )
        d->m_mapViewUi.projectionComboBox->setCurrentIndex( (int) projection );

    if (d->m_toolBar) {
        switch (projection) {
        case Marble::Spherical:
            d->m_globeViewButton->setChecked(true);
            d->m_globeViewAction->setChecked(true);
            d->m_mercatorViewButton->setChecked(false);
            d->m_mercatorViewAction->setChecked(false);
            d->m_flatViewAction->setChecked(false);
            d->m_gnomonicViewAction->setChecked(false);
            d->m_stereographicViewAction->setChecked(false);
            d->m_lambertAzimuthalViewAction->setChecked(false);
            d->m_azimuthalEquidistantViewAction->setChecked(false);
            d->m_verticalPerspectiveViewAction->setChecked(false);
            break;
        case Marble::Mercator:
            d->m_mercatorViewButton->setChecked(true);
            d->m_mercatorViewAction->setChecked(true);
            d->m_globeViewButton->setChecked(false);
            d->m_flatViewAction->setChecked(false);
            d->m_gnomonicViewAction->setChecked(false);
            d->m_globeViewAction->setChecked(false);
            d->m_stereographicViewAction->setChecked(false);
            d->m_lambertAzimuthalViewAction->setChecked(false);
            d->m_azimuthalEquidistantViewAction->setChecked(false);
            d->m_verticalPerspectiveViewAction->setChecked(false);
            break;
        case Marble::Equirectangular:
            d->m_flatViewAction->setChecked(true);
            d->m_mercatorViewButton->setChecked(true);
            d->m_globeViewButton->setChecked(false);
            d->m_mercatorViewAction->setChecked(false);
            d->m_gnomonicViewAction->setChecked(false);
            d->m_globeViewAction->setChecked(false);
            d->m_stereographicViewAction->setChecked(false);
            d->m_lambertAzimuthalViewAction->setChecked(false);
            d->m_azimuthalEquidistantViewAction->setChecked(false);
            d->m_verticalPerspectiveViewAction->setChecked(false);
            break;
        case Marble::Gnomonic:
            d->m_flatViewAction->setChecked(false);
            d->m_mercatorViewButton->setChecked(true);
            d->m_globeViewButton->setChecked(false);
            d->m_mercatorViewAction->setChecked(false);
            d->m_gnomonicViewAction->setChecked(true);
            d->m_globeViewAction->setChecked(false);
            d->m_stereographicViewAction->setChecked(false);
            d->m_lambertAzimuthalViewAction->setChecked(false);
            d->m_azimuthalEquidistantViewAction->setChecked(false);
            d->m_verticalPerspectiveViewAction->setChecked(false);
            break;
        case Marble::Stereographic:
            d->m_flatViewAction->setChecked(false);
            d->m_mercatorViewButton->setChecked(true);
            d->m_globeViewButton->setChecked(false);
            d->m_mercatorViewAction->setChecked(false);
            d->m_gnomonicViewAction->setChecked(false);
            d->m_globeViewAction->setChecked(false);
            d->m_stereographicViewAction->setChecked(true);
            d->m_lambertAzimuthalViewAction->setChecked(false);
            d->m_azimuthalEquidistantViewAction->setChecked(false);
            d->m_verticalPerspectiveViewAction->setChecked(false);
            break;
        case Marble::LambertAzimuthal:
            d->m_flatViewAction->setChecked(false);
            d->m_mercatorViewButton->setChecked(true);
            d->m_globeViewButton->setChecked(false);
            d->m_mercatorViewAction->setChecked(false);
            d->m_gnomonicViewAction->setChecked(false);
            d->m_globeViewAction->setChecked(false);
            d->m_stereographicViewAction->setChecked(false);
            d->m_lambertAzimuthalViewAction->setChecked(true);
            d->m_azimuthalEquidistantViewAction->setChecked(false);
            d->m_verticalPerspectiveViewAction->setChecked(false);
            break;
        case Marble::AzimuthalEquidistant:
            d->m_flatViewAction->setChecked(false);
            d->m_mercatorViewButton->setChecked(true);
            d->m_globeViewButton->setChecked(false);
            d->m_mercatorViewAction->setChecked(false);
            d->m_gnomonicViewAction->setChecked(false);
            d->m_globeViewAction->setChecked(false);
            d->m_stereographicViewAction->setChecked(false);
            d->m_lambertAzimuthalViewAction->setChecked(false);
            d->m_azimuthalEquidistantViewAction->setChecked(true);
            d->m_verticalPerspectiveViewAction->setChecked(false);
            break;
        case Marble::VerticalPerspective:
            d->m_flatViewAction->setChecked(false);
            d->m_mercatorViewButton->setChecked(true);
            d->m_globeViewButton->setChecked(false);
            d->m_mercatorViewAction->setChecked(false);
            d->m_gnomonicViewAction->setChecked(false);
            d->m_globeViewAction->setChecked(false);
            d->m_stereographicViewAction->setChecked(false);
            d->m_lambertAzimuthalViewAction->setChecked(false);
            d->m_azimuthalEquidistantViewAction->setChecked(false);
            d->m_verticalPerspectiveViewAction->setChecked(true);
            break;
        }
    }
}

void MapViewWidget::globeViewRequested()
{
    emit projectionChanged(Marble::Spherical);
}

void MapViewWidget::flatViewRequested()
{
    emit projectionChanged(Marble::Equirectangular);
}

void MapViewWidget::mercatorViewRequested()
{
    emit projectionChanged(Marble::Mercator);
}

void MapViewWidget::gnomonicViewRequested()
{
    emit projectionChanged(Marble::Gnomonic);
}

void MapViewWidget::stereographicViewRequested()
{
    emit projectionChanged(Marble::Stereographic);
}

void MapViewWidget::lambertAzimuthalViewRequested()
{
    emit projectionChanged(Marble::LambertAzimuthal);
}

void MapViewWidget::azimuthalEquidistantViewRequested()
{
    emit projectionChanged(Marble::AzimuthalEquidistant);
}

void MapViewWidget::verticalPerspectiveViewRequested()
{
    emit projectionChanged(Marble::VerticalPerspective);
}

void MapViewWidget::Private::celestialBodySelected( int comboIndex )
{
    Q_UNUSED( comboIndex )

    updateMapFilter();

    bool foundMapTheme = false;

    QString currentMapThemeId = m_marbleModel->mapThemeId();
    QString oldPlanetId = m_marbleModel->planetId();

    int row = m_mapSortProxy.rowCount();

    for ( int i = 0; i < row; ++i )
    {
        QModelIndex index = m_mapSortProxy.index(i,0);
        QString itMapThemeId = m_mapSortProxy.data(index, Qt::UserRole + 1).toString();
        if ( currentMapThemeId == itMapThemeId )
        {
            foundMapTheme = true;
            break;
        }
    }
    if ( !foundMapTheme ) {
        QModelIndex index = m_mapSortProxy.index(0,0);
        emit q->mapThemeIdChanged( m_mapSortProxy.data( index, Qt::UserRole + 1 ).toString() );
    }

    if( oldPlanetId != m_marbleModel->planetId() ) {
        emit q->celestialBodyChanged( m_marbleModel->planetId() );
    }
}

// Relay a signal and convert the parameter from an int to a Projection.
void MapViewWidget::Private::projectionSelected( int projectionIndex )
{
    emit q->projectionChanged( (Projection) projectionIndex );
}

void MapViewWidget::Private::mapThemeSelected( QModelIndex index )
{
    mapThemeSelected( index.row() );
}

void MapViewWidget::Private::mapThemeSelected( int index )
{
    const QModelIndex columnIndex = m_mapSortProxy.index( index, 0 );
    const QString currentmaptheme = m_mapSortProxy.data( columnIndex, Qt::UserRole + 1 ).toString();

    mDebug() << Q_FUNC_INFO << currentmaptheme;

    emit q->mapThemeIdChanged( currentmaptheme );
}

QString MapViewWidget::Private::currentThemeName() const
{
    const QModelIndex index = m_mapViewUi.marbleThemeSelectView->currentIndex();
    const QModelIndex columnIndex = m_mapSortProxy.index( index.row(), 0, QModelIndex() );

    return m_mapSortProxy.data( columnIndex ).toString();
}

QString MapViewWidget::Private::currentThemePath() const
{
    const QModelIndex index = m_mapViewUi.marbleThemeSelectView->currentIndex();
    const QModelIndex columnIndex = m_mapSortProxy.index( index.row(), 0 );

    return m_mapSortProxy.data( columnIndex, Qt::UserRole + 1 ).toString();
}

QString MapViewWidget::Private::favoriteKey(const QModelIndex &index) const
{
    return QLatin1String("Favorites/") + m_mapSortProxy.data(index).toString();
}

void MapViewWidget::Private::showContextMenu( const QPoint& pos )
{
    QMenu menu;

    QAction* iconSizeAction = menu.addAction( tr( "&Show Large Icons" ), q, SLOT(toggleIconSize()) );
    iconSizeAction->setCheckable( true );
    iconSizeAction->setChecked( m_mapViewUi.marbleThemeSelectView->iconSize() == QSize( 96, 96 ) );
    QAction *favAction = menu.addAction(QIcon(QStringLiteral(":/icons/bookmarks.png")), tr("&Favorite"), q, SLOT(toggleFavorite()));
    favAction->setCheckable( true );
    favAction->setChecked( isCurrentFavorite() );
    menu.addSeparator();

    menu.addAction(QIcon(QStringLiteral(":/icons/create-new-map.png")), tr("&Create a New Map..."), q, SIGNAL(showMapWizard()));
    if (QFileInfo(MarbleDirs::localPath() + QLatin1String("/maps/") + currentThemePath()).exists()) {
        menu.addAction( tr( "&Delete Map Theme" ), q, SLOT(deleteMap()) );
    }
    menu.addAction( tr( "&Upload Map..." ), q, SIGNAL(showUploadDialog()) );
    menu.exec( m_mapViewUi.marbleThemeSelectView->mapToGlobal( pos ) );
}

void MapViewWidget::Private::deleteMap()
{
    if(QMessageBox::warning( q,
                             tr( "Marble" ),
                             tr( "Are you sure that you want to delete \"%1\"?" ).arg( currentThemeName() ),
                             QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes )
    {
        MapThemeManager::deleteMapTheme( currentThemePath() );
        emit q->mapThemeDeleted();
    }
}

void MapViewWidget::Private::toggleFavorite()
{
    QModelIndex index = m_mapViewUi.marbleThemeSelectView->currentIndex();
    if( isCurrentFavorite() ) {
        m_settings.remove(favoriteKey(index));
    } else {
        m_settings.setValue(favoriteKey(index), QDateTime::currentDateTime() );
    }
    QStandardItemModel* sourceModel = qobject_cast<QStandardItemModel*>(m_mapSortProxy.sourceModel());
    const QModelIndex sourceIndex = m_mapSortProxy.mapToSource(index);
    emit sourceModel->dataChanged( sourceIndex, sourceIndex );
    index = m_mapViewUi.marbleThemeSelectView->currentIndex();
    m_mapViewUi.marbleThemeSelectView->scrollTo(index);
}

void MapViewWidget::Private::toggleIconSize()
{
    bool const isLarge = m_mapViewUi.marbleThemeSelectView->iconSize() == QSize( 96, 96 );
    int const size = isLarge ? 52 : 96;
    m_mapViewUi.marbleThemeSelectView->setIconSize( QSize( size, size ) );
    m_settings.setValue(QStringLiteral("MapView/iconSize"), m_mapViewUi.marbleThemeSelectView->iconSize() );
}

bool MapViewWidget::Private::isCurrentFavorite() const
{
    const QModelIndex index = m_mapViewUi.marbleThemeSelectView->currentIndex();
    return m_settings.contains(favoriteKey(index));
}

}

#include "moc_MapViewWidget.cpp"
