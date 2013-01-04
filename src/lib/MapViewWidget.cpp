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
#include "Planet.h"
#include "GeoSceneDocument.h"
#include "GeoSceneHead.h"

// Qt
#include <QResizeEvent>
#include <QtCore/QFileInfo>
#include <QtCore/QSettings>
#include <QtGui/QMenu>
#include <QtGui/QMessageBox>
#include <QtGui/QStandardItemModel>
#include <QtGui/QGridLayout>
#include <QToolBar>
#include <QToolButton>

using namespace Marble;
// Ui
#include "ui_MapViewWidget.h"

namespace Marble
{

class CelestialSortFilterProxyModel : public QSortFilterProxyModel
{
public:
    CelestialSortFilterProxyModel()
    {
        // here we will set m_priority for default order
        const int prefix = 100;
        m_priority["mercury"] = prefix-1;
        m_priority["venus"] = prefix-2;
        m_priority["earth"] = prefix-3;
        m_priority["mars"] = prefix-4;
        m_priority["jupiter"] = prefix-5;
        m_priority["saturn"] = prefix-6;
        m_priority["uranus"] = prefix-7;
        m_priority["neptune"] = prefix-8;
        m_priority["pluto"] = prefix-9;
    }
    ~CelestialSortFilterProxyModel() {}

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const {
        // get planetids for left and right planets
        QString first = sourceModel()->index(left.row(), 1).data().toString();
        QString second = sourceModel()->index(right.row(), 1).data().toString();
        // both are in the list
        if (m_priority.contains(first) && m_priority.contains(second)) {
            return m_priority[first] > m_priority[second];
        }
        // only left in the list
        if (m_priority.contains(first) && !m_priority.contains(second)) {
            return true;
        }
        // only right in the list
        if (!m_priority.contains(first) && m_priority.contains(second)) {
            return false;
        }
        return QSortFilterProxyModel::lessThan(left, right);
    }

private:
    QMap<QString, int> m_priority;
};

class MapViewWidget::Private {
 public:
    Private( MapViewWidget *parent )
        : q( parent ),
          m_widget( 0 ),
          m_mapThemeModel( 0 ),
          m_mapSortProxy(),
          m_celestialList(),
          m_celestialListProxy(),
          m_settings( "kde.org", "Marble Desktop Globe" ),
          m_toolBar( 0 )
    {
        m_celestialListProxy.setSourceModel(&m_celestialList);
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
        m_globeViewButton->setIcon( QIcon(":/icons/map-globe.png") );
        m_globeViewButton->setToolTip( tr("Globe View") );
        m_globeViewButton->setCheckable(true);
        m_globeViewButton->setChecked(false);

        m_mercatorViewButton = new QToolButton;
        m_mercatorViewButton->setIcon( QIcon(":/icons/map-mercator.png") );
        m_mercatorViewButton->setToolTip( tr("Mercator View") );
        m_mercatorViewButton->setCheckable(true);
        m_mercatorViewButton->setChecked(false);
        m_mercatorViewButton->setPopupMode(QToolButton::MenuButtonPopup);

        m_popupMenu = new QMenu;

        m_mercatorViewAction = new QAction( QIcon(":/icons/map-mercator.png"),
                                              tr("Mercator View"),
                                              m_popupMenu );
        m_mercatorViewAction->setCheckable(true);
        m_mercatorViewAction->setChecked(false);

        m_flatViewAction = new QAction( QIcon(":/icons/map-flat.png"),
                                        tr("Flat View"),
                                        m_popupMenu );
        m_flatViewAction->setCheckable(true);
        m_flatViewAction->setChecked(false);

        m_popupMenu->addAction(m_mercatorViewAction);
        m_popupMenu->addAction(m_flatViewAction);
        m_mercatorViewButton->setMenu(m_popupMenu);

        m_toolBar->addWidget(m_globeViewButton);
        m_toolBar->addWidget(m_mercatorViewButton);
        m_toolBar->addSeparator();
        m_toolBar->setContentsMargins(0,0,0,0);
        m_toolBar->setIconSize(QSize(16, 16));
        m_mapViewUi.toolBarLayout->insertWidget(0, m_toolBar);

        applyReducedLayout();
        q->setProjection(m_widget->projection());
    }

    void updateMapFilter()
    {
        int currentIndex = m_mapViewUi.celestialBodyComboBox->currentIndex();
        int const row = m_celestialListProxy.mapToSource( m_celestialListProxy.index( currentIndex, 0 ) ).row();
        QStandardItem * selectedItem = m_celestialList.item( row, 1 );

        if ( selectedItem ) {
            QString selectedId;
            selectedId = selectedItem->text();
            m_mapSortProxy.setFilterRegExp( QRegExp( selectedId, Qt::CaseInsensitive,QRegExp::FixedString ) );
        }

        m_mapSortProxy.sort( 0 );
    }

    void setCelestialBody( int comboIndex );

    /// whenever a new map gets inserted, the following slot will adapt the ListView accordingly
    void updateMapThemeView();

    void projectionSelected( int projectionIndex );

    void mapThemeSelected( QModelIndex index );
    void mapThemeSelected( int index );

    void showContextMenu( const QPoint& pos );
    void deleteMap();
    void toggleFavorite();
    void toggleIconSize();

    bool isCurrentFavorite();
    QString currentThemeName() const;
    QString currentThemePath() const;

    MapViewWidget *const q;

    Ui::MapViewWidget  m_mapViewUi;
    MarbleWidget      *m_widget;

    QStandardItemModel     *m_mapThemeModel;
    MapThemeSortFilterProxyModel m_mapSortProxy;

    QStandardItemModel m_celestialList;
    CelestialSortFilterProxyModel m_celestialListProxy;
    QSettings m_settings;
    QToolBar *m_toolBar;
    QToolButton *m_globeViewButton;
    QToolButton *m_mercatorViewButton;
    QMenu *m_popupMenu;
    QAction *m_flatViewAction;
    QAction *m_mercatorViewAction;
    QAction *m_celestialBodyAction;
};

MapViewWidget::MapViewWidget( QWidget *parent, Qt::WindowFlags f )
    : QWidget( parent, f ),
      d( new Private( this ) )
{
    d->m_mapViewUi.setupUi( this );

    if ( MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen ) {
        QGridLayout* layout = new QGridLayout;
        layout->addItem( d->m_mapViewUi.verticalLayout->takeAt( 0 ), 0, 0 );
        layout->addItem( d->m_mapViewUi.verticalLayout->takeAt( 0 ), 0, 1 );
        d->m_mapViewUi.line->setVisible( false );
        layout->addItem( d->m_mapViewUi.verticalLayout->takeAt( 1 ), 1, 0 );
        layout->addItem( d->m_mapViewUi.verticalLayout->takeAt( 1 ), 1, 1 );
        layout->addItem( d->m_mapViewUi.verticalLayout->takeAt( 2 ), 2, 0 );
        layout->addItem( d->m_mapViewUi.verticalLayout->takeAt( 3 ), 2, 1 );
        d->m_mapViewUi.verticalLayout->insertLayout( 0, layout );
        d->m_mapViewUi.mapThemeComboBox->setModel( &d->m_mapSortProxy );
        d->m_mapViewUi.mapThemeComboBox->setIconSize( QSize( 48, 48 ) );
        connect( d->m_mapViewUi.mapThemeComboBox, SIGNAL( activated( int ) ),
                 this,                            SLOT( mapThemeSelected( int ) ) );
        d->m_mapViewUi.marbleThemeSelectView->setVisible( false );
    }
    else {
        d->m_mapViewUi.marbleThemeSelectView->setViewMode( QListView::IconMode );
        QSize const iconSize = d->m_settings.value( "MapView/iconSize", QSize( 136, 136 ) ).toSize();
        d->m_mapViewUi.marbleThemeSelectView->setIconSize( iconSize );
        d->m_mapViewUi.marbleThemeSelectView->setFlow( QListView::LeftToRight );
        d->m_mapViewUi.marbleThemeSelectView->setWrapping( true );
        d->m_mapViewUi.marbleThemeSelectView->setResizeMode( QListView::Adjust );
        d->m_mapViewUi.marbleThemeSelectView->setUniformItemSizes( true );
        d->m_mapViewUi.marbleThemeSelectView->setMovement( QListView::Static );
        d->m_mapViewUi.marbleThemeSelectView->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
        d->m_mapViewUi.marbleThemeSelectView->setEditTriggers( QListView::NoEditTriggers );
        d->m_mapViewUi.marbleThemeSelectView->setSelectionMode( QListView::SingleSelection );
        d->m_mapViewUi.marbleThemeSelectView->setModel( &d->m_mapSortProxy );
        connect( d->m_mapViewUi.marbleThemeSelectView, SIGNAL( pressed( QModelIndex ) ),
                 this,                                 SLOT( mapThemeSelected( QModelIndex ) ) );
        connect( d->m_mapViewUi.marbleThemeSelectView, SIGNAL( customContextMenuRequested( QPoint ) ),
                 this,                                 SLOT( showContextMenu( QPoint ) ) );

        d->m_mapViewUi.mapThemeComboBox->setVisible( false );
    }

    connect( d->m_mapViewUi.projectionComboBox,    SIGNAL( activated( int ) ),
             this,                                 SLOT( projectionSelected( int ) ) );

    d->m_mapViewUi.projectionComboBox->setEnabled( true );
    d->m_mapViewUi.celestialBodyComboBox->setModel( &d->m_celestialListProxy );

    connect( d->m_mapViewUi.celestialBodyComboBox, SIGNAL( activated( int ) ),
             this,                                 SLOT( setCelestialBody( int ) ) );

    d->m_settings.beginGroup( "Favorites" );
    if( !d->m_settings.contains( "initialized" ) ) {
        d->m_settings.setValue( "initialized", true );
        QDateTime currentDateTime = QDateTime::currentDateTime();
        d->m_settings.setValue( "Atlas", currentDateTime );
        d->m_settings.setValue( "OpenStreetMap", currentDateTime );
        d->m_settings.setValue( "Satellite View", currentDateTime );
    }
    d->m_settings.endGroup();
}

MapViewWidget::~MapViewWidget()
{
    delete d;
}

void MapViewWidget::setMarbleWidget( MarbleWidget *widget )
{
    d->m_widget = widget;
    d->m_mapThemeModel = widget->model()->mapThemeManager()->mapThemeModel();
    d->m_mapSortProxy.setSourceModel( d->m_mapThemeModel );

    connect( d->m_mapThemeModel, SIGNAL( rowsInserted( QModelIndex, int, int ) ),
             this,               SLOT( updateMapThemeView() ) );

    connect( this,        SIGNAL( projectionChanged( Projection ) ),
             d->m_widget, SLOT( setProjection( Projection ) ) );

    connect( d->m_widget, SIGNAL( themeChanged( QString ) ),
             this,        SLOT( setMapThemeId( QString ) ) );

    connect( d->m_widget, SIGNAL( projectionChanged( Projection ) ),
             this,        SLOT( setProjection( Projection ) ) );
    setProjection( d->m_widget->projection() );

    connect( this,        SIGNAL( mapThemeIdChanged( const QString& ) ),
             d->m_widget, SLOT( setMapThemeId( const QString& ) ) );

    d->updateMapFilter();
    d->updateMapThemeView();

    d->setupToolBar();

    connect(d->m_globeViewButton, SIGNAL(clicked()),
            this, SLOT(globeViewRequested()));
    connect(d->m_mercatorViewButton, SIGNAL(clicked()),
            this, SLOT(mercatorViewRequested()));
    connect(d->m_mercatorViewAction, SIGNAL(triggered()),
            this, SLOT(mercatorViewRequested()));
    connect(d->m_flatViewAction, SIGNAL(triggered()),
            this, SLOT(flatViewRequested()));
}

void MapViewWidget::resizeEvent(QResizeEvent *event)
{
    if (d->m_toolBar->isVisible() && event->size().height() > 400) {
        d->applyExtendedLayout();
    } else if (!d->m_toolBar->isVisible() && event->size().height() <= 400) {
        d->applyReducedLayout();
    }
}

void MapViewWidget::Private::updateMapThemeView()
{
    for ( int i = 0; i < m_mapThemeModel->rowCount(); ++i ) {
        QString celestialBodyId = ( m_mapThemeModel->data( m_mapThemeModel->index( i, 1 ) ).toString() ).section( '/', 0, 0 );
        QString celestialBodyName = Planet::name( celestialBodyId );

        QList<QStandardItem*> matchingItems = m_celestialList.findItems( celestialBodyId, Qt::MatchExactly, 1 );
        if ( matchingItems.isEmpty() ) {
            m_celestialList.appendRow( QList<QStandardItem*>()
                                << new QStandardItem( celestialBodyName )
                                << new QStandardItem( celestialBodyId ) );
        }
    }

    if ( m_widget ) {
        QString mapThemeId = m_widget->mapThemeId();
        if ( !mapThemeId.isEmpty() )
            q->setMapThemeId( mapThemeId );
    }
    m_celestialListProxy.sort(0);
}

void MapViewWidget::setMapThemeId( const QString &theme )
{
    if ( !d->m_mapThemeModel || !d->m_widget )
        return;

    const bool smallscreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;

    // Check if the new selected theme is different from the current one
    const int currentRow = smallscreen ? d->m_mapViewUi.mapThemeComboBox->currentIndex() :
                                         d->m_mapViewUi.marbleThemeSelectView->currentIndex().row();
    QString indexTheme = d->m_mapSortProxy.data( d->m_mapSortProxy.index(
                         currentRow, 1, QModelIndex() ) ).toString();

    if ( theme != indexTheme ) {
        /* indexTheme would be empty if the chosen map has not been set yet. As
        this needs to be done after the mapThemeId has been set, check if that is
        not empty first. The behaviour differs between Linux and Windows: on
        Windows the reading of the settings is not delayed, thus the mapThemeId
        is available earlier than on Linux.
        */
        if( indexTheme.isEmpty() && !d->m_widget->mapThemeId().isEmpty() ) {
            QList<QStandardItem*> items = d->m_mapThemeModel->findItems( theme, Qt::MatchExactly, 1 );
            if( items.size() >= 1 ) {
                QModelIndex iterIndex = items.first()->index();
                QModelIndex iterIndexName = d->m_mapSortProxy.mapFromSource( iterIndex.sibling( iterIndex.row(), 0 ) );

                if ( smallscreen ) {
                    d->m_mapViewUi.mapThemeComboBox->setCurrentIndex( iterIndexName.row() );
                }
                else {
                    d->m_mapViewUi.marbleThemeSelectView->setCurrentIndex( iterIndexName );
                    d->m_mapViewUi.marbleThemeSelectView->scrollTo( iterIndexName );
                }
            }
        }

        QString selectedId = d->m_widget->mapTheme()->head()->target();

        QList<QStandardItem*> itemList = d->m_celestialList.findItems( selectedId, Qt::MatchExactly, 1 );

        if ( !itemList.isEmpty() ) {
            QStandardItem * selectedItem = itemList.first();

            if ( selectedItem ) {
                QModelIndex const selectedIndex = d->m_celestialList.index( selectedItem->row(), 0 );
                int const proxyIndex = d->m_celestialListProxy.mapFromSource( selectedIndex ).row();
                d->m_mapViewUi.celestialBodyComboBox->setCurrentIndex( proxyIndex );
                d->m_mapSortProxy.setFilterRegExp( QRegExp( selectedId, Qt::CaseInsensitive,QRegExp::FixedString ) );
            }

            d->m_mapSortProxy.sort( 0 );
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
            d->m_mercatorViewButton->setChecked(false);
            d->m_mercatorViewAction->setChecked(false);
            d->m_flatViewAction->setChecked(false);
            break;
        case Marble::Mercator:
            d->m_mercatorViewButton->setChecked(true);
            d->m_mercatorViewAction->setChecked(true);
            d->m_globeViewButton->setChecked(false);
            d->m_flatViewAction->setChecked(false);
            break;
        case Marble::Equirectangular:
            d->m_flatViewAction->setChecked(true);
            d->m_mercatorViewButton->setChecked(true);
            d->m_globeViewButton->setChecked(false);
            d->m_mercatorViewAction->setChecked(false);
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

void MapViewWidget::Private::setCelestialBody( int comboIndex )
{
    Q_UNUSED( comboIndex )

    updateMapFilter();

    bool foundMapTheme = false;

    QString currentMapThemeId = m_widget->mapThemeId();
    QString oldPlanetId = m_widget->model()->planetId();

    int row = m_mapSortProxy.rowCount();

    for ( int i = 0; i < row; ++i )
    {
        QModelIndex index = m_mapSortProxy.index(i,1);
        QString itMapThemeId = m_mapSortProxy.data(index).toString();
        if ( currentMapThemeId == itMapThemeId )
        {
            foundMapTheme = true;
            break;
        }
    }
    if ( !foundMapTheme ) {
        QModelIndex index = m_mapSortProxy.index(0,1);
        emit q->mapThemeIdChanged( m_mapSortProxy.data( index ).toString() );
    }

    if( oldPlanetId != m_widget->model()->planetId() ) {
        emit q->celestialBodyChanged( m_widget->model()->planetId() );
    }

    updateMapThemeView();
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
    const QModelIndex columnIndex = m_mapSortProxy.index( index, 1, QModelIndex() );
    const QString currentmaptheme = m_mapSortProxy.data( columnIndex ).toString();

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
    const QModelIndex columnIndex = m_mapSortProxy.index( index.row(), 1, QModelIndex() );

    return m_mapSortProxy.data( columnIndex ).toString();
}

void MapViewWidget::Private::showContextMenu( const QPoint& pos )
{
    QMenu menu;

    QAction* iconSizeAction = menu.addAction( tr( "&Show Large Icons" ), q, SLOT( toggleIconSize() ) );
    iconSizeAction->setCheckable( true );
    iconSizeAction->setChecked( m_mapViewUi.marbleThemeSelectView->iconSize() == QSize( 136, 136 ) );
    QAction *favAction = menu.addAction( tr( "&Favorite" ), q, SLOT( toggleFavorite() ) );
    favAction->setCheckable( true );
    favAction->setChecked( isCurrentFavorite() );
    menu.addSeparator();

    menu.addAction( "&Create a New Map...", q, SIGNAL( showMapWizard() ) );
    if( QFileInfo( MarbleDirs::localPath() + "/maps/" + currentThemePath() ).exists() )
        menu.addAction( tr( "&Delete Map Theme" ), q, SLOT( deleteMap() ) );
    menu.addAction( tr( "&Upload Map..." ), q, SIGNAL( showUploadDialog() ) );
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
    }
}

void MapViewWidget::Private::toggleFavorite()
{
    const QModelIndex index = m_mapViewUi.marbleThemeSelectView->currentIndex();
    const QModelIndex columnIndex = m_mapSortProxy.index( index.row(), 0 );

    if( isCurrentFavorite() ) {
        m_settings.beginGroup( "Favorites" );
        m_settings.remove( m_mapSortProxy.data( columnIndex ).toString() );
    }
    else {
        m_settings.beginGroup( "Favorites" );
        m_settings.setValue( m_mapSortProxy.data( columnIndex ).toString(), QDateTime::currentDateTime() );
    }
    m_settings.endGroup();

    m_mapSortProxy.sort( 0 );
}

void MapViewWidget::Private::toggleIconSize()
{
    bool const isLarge = m_mapViewUi.marbleThemeSelectView->iconSize() == QSize( 136, 136 );
    int const size = isLarge ? 48 : 136;
    m_mapViewUi.marbleThemeSelectView->setIconSize( QSize( size, size ) );
    m_settings.setValue("MapView/iconSize", m_mapViewUi.marbleThemeSelectView->iconSize() );
}

bool MapViewWidget::Private::isCurrentFavorite()
{
    const QModelIndex index = m_mapViewUi.marbleThemeSelectView->currentIndex();
    const QModelIndex nameIndex = m_mapSortProxy.index( index.row(), 0 );

    m_settings.beginGroup( "Favorites" );
    const bool isFavorite = m_settings.contains( m_mapSortProxy.data( nameIndex ).toString() );
    m_settings.endGroup();

    return isFavorite;
}

}

#include "MapViewWidget.moc"
