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
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>

using namespace Marble;
// Ui
#include "ui_MapViewWidget.h"

namespace Marble
{

class MapViewItemDelegate : public QAbstractItemDelegate
{
public:
    MapViewItemDelegate( QListView* view );
    void paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
    QSize sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const;

private:
    QString text( const QModelIndex &index ) const;
    QListView* m_view;
    QIcon m_bookmarkIcon;
};

class CelestialSortFilterProxyModel : public QSortFilterProxyModel
{
public:
    CelestialSortFilterProxyModel()
    {
        setupPriorities();
        setupMoonsList();
        setupDwarfsList();
    }
    ~CelestialSortFilterProxyModel() {}

    // A small trick to change names for dwarfs and moons
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const
    {
        QVariant var = QSortFilterProxyModel::data(index, role);
        if (role == Qt::DisplayRole && index.column() == 0) {
            QString newOne = var.toString();
            if (newOne == tr("Moon")) {
                return QString("  " + tr("Moon"));
            } else if (m_moons.contains(newOne.toLower())) {
                return QString("  "+newOne+" (" + tr("moon") + ")");
            } else if (m_dwarfs.contains(newOne.toLower())) {
                return QString(newOne+ " (" + tr("dwarf planet") + ")");
            }
            return newOne;
        } else {
            return var;
        }
    }

private:
    // TODO: create priority on the model side (Planet Class) by taking the distance to the "home planet/home star" into account
    void setupPriorities()
    {
        // here we will set m_priority for default order
        int prefix = 100;

        m_priority["sun"] = prefix;
        m_priority["mercury"] = prefix--;
        m_priority["venus"] = prefix--;
        m_priority["earth"] = prefix--;
        m_priority["moon"] = prefix--;
        m_priority["mars"] = prefix--;

        m_priority["jupiter"] = prefix--;
        // Moons of Jupiter
        m_priority["io"] = prefix--;
        m_priority["europa"] = prefix--;
        m_priority["ganymede"] = prefix--;
        m_priority["callisto"] = prefix--;

        m_priority["saturn"] = prefix--;
        // Moons of Saturn
        m_priority["mimas"] = prefix--;
        m_priority["enceladus"] = prefix--;
        m_priority["thetys"] = prefix--;
        m_priority["dione"] = prefix--;
        m_priority["rhea"] = prefix--;
        m_priority["titan"] = prefix--;
        m_priority["iapetus"] = prefix--;

        m_priority["uranus"] = prefix--;
        m_priority["neptune"] = prefix--;
        m_priority["pluto"] = prefix--;
        m_priority["ceres"] = prefix--;
    }

    void setupMoonsList()
    {
        m_moons.push_back("moon");
        m_moons.push_back("europa");
        m_moons.push_back("ganymede");
        m_moons.push_back("callisto");
        m_moons.push_back("mimas");
        m_moons.push_back("enceladus");
        m_moons.push_back("thetys");
        m_moons.push_back("dione");
        m_moons.push_back("rhea");
        m_moons.push_back("titan");
        m_moons.push_back("iapetus");
    }

    void setupDwarfsList()
    {
        m_dwarfs.push_back("pluto");
        m_dwarfs.push_back("ceres");
    }

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const {
        const QString nameLeft = sourceModel()->index(left.row(), 1).data().toString();
        const QString nameRight = sourceModel()->index(right.row(), 1).data().toString();
        const QString first = nameLeft.toLower();
        const QString second = nameRight.toLower();

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
    QList<QString> m_moons;
    QList<QString> m_dwarfs;
};

class MapViewWidget::Private {
 public:
    Private( MapViewWidget *parent )
        : q( parent ),
          m_marbleModel( 0 ),
          m_mapSortProxy(),
          m_celestialListProxy(),
          m_toolBar( 0 )
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

        QObject::connect(m_globeViewButton, SIGNAL(clicked()),
                         q, SLOT(globeViewRequested()));
        QObject::connect(m_mercatorViewButton, SIGNAL(clicked()),
                         q, SLOT(mercatorViewRequested()));
        QObject::connect(m_mercatorViewAction, SIGNAL(triggered()),
                         q, SLOT(mercatorViewRequested()));
        QObject::connect(m_flatViewAction, SIGNAL(triggered()),
                         q, SLOT(flatViewRequested()));

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

    bool isCurrentFavorite();
    QString currentThemeName() const;
    QString currentThemePath() const;

    MapViewWidget *const q;

    Ui::MapViewWidget  m_mapViewUi;
    MarbleModel      *m_marbleModel;

    MapThemeSortFilterProxyModel m_mapSortProxy;

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
        QSize const iconSize = d->m_settings.value( "MapView/iconSize", QSize( 90, 90 ) ).toSize();
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

    const QString oldCelestialBodyId = oldThemeId.section( '/', 0, 0 );
    const QString celestialBodyId = themeId.section( '/', 0, 0 );

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

void MapViewWidget::Private::showContextMenu( const QPoint& pos )
{
    QMenu menu;

    QAction* iconSizeAction = menu.addAction( tr( "&Show Large Icons" ), q, SLOT(toggleIconSize()) );
    iconSizeAction->setCheckable( true );
    iconSizeAction->setChecked( m_mapViewUi.marbleThemeSelectView->iconSize() == QSize( 96, 96 ) );
    QAction *favAction = menu.addAction( QIcon( ":/icons/bookmarks.png" ), tr( "&Favorite" ), q, SLOT(toggleFavorite()) );
    favAction->setCheckable( true );
    favAction->setChecked( isCurrentFavorite() );
    menu.addSeparator();

    menu.addAction( QIcon( ":/icons/create-new-map.png" ), tr("&Create a New Map..."), q, SIGNAL(showMapWizard()) );
    if( QFileInfo( MarbleDirs::localPath() + "/maps/" + currentThemePath() ).exists() )
        menu.addAction( tr( "&Delete Map Theme" ), q, SLOT(deleteMap()) );
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
}

void MapViewWidget::Private::toggleIconSize()
{
    bool const isLarge = m_mapViewUi.marbleThemeSelectView->iconSize() == QSize( 96, 96 );
    int const size = isLarge ? 52 : 96;
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

MapViewItemDelegate::MapViewItemDelegate( QListView *view ) :
    m_view( view ), m_bookmarkIcon( ":/icons/bookmarks.png" )
{
    // nothing to do
}

void MapViewItemDelegate::paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
    QStyleOptionViewItemV4 styleOption = option;
    styleOption.text = QString();
    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &styleOption, painter);

    QAbstractTextDocumentLayout::PaintContext paintContext;
    if (styleOption.state & QStyle::State_Selected) {
        paintContext.palette.setColor(QPalette::Text,
            styleOption.palette.color(QPalette::Active, QPalette::HighlightedText));
    }

    QRect const rect = option.rect;
    QSize const iconSize = option.decorationSize;
    QRect const iconRect( rect.topLeft(), iconSize );
    QIcon const icon = index.data( Qt::DecorationRole ).value<QIcon>();
    painter->drawPixmap( iconRect, icon.pixmap( iconSize ) );

    int const padding = 5;
    QString const name = index.data().toString();
    const bool isFavorite = QSettings().contains( "Favorites/" + name );
    QSize const bookmarkSize( 16, 16 );
    QRect bookmarkRect( iconRect.bottomRight(), bookmarkSize );
    bookmarkRect.translate( QPoint( -bookmarkSize.width() - padding, -bookmarkSize.height() - padding ) );
    QIcon::Mode mode = isFavorite ? QIcon::Normal : QIcon::Disabled;
    painter->drawPixmap( bookmarkRect, m_bookmarkIcon.pixmap( bookmarkSize, mode ) );

    QTextDocument document;
    document.setTextWidth( rect.width() - iconSize.width() - padding );
    document.setDefaultFont( option.font );
    document.setHtml( text( index ) );

    QRect textRect = QRect( iconRect.topRight(), QSize( document.textWidth() - padding, rect.height() - padding ) );
    painter->save();
    painter->translate( textRect.topLeft() );
    painter->setClipRect( textRect.translated( -textRect.topLeft() ) );
    document.documentLayout()->draw( painter, paintContext );
    painter->restore();
}

QSize MapViewItemDelegate::sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
    if ( index.column() == 0 ) {
        QSize const iconSize = option.decorationSize;
        QTextDocument doc;
        doc.setDefaultFont( option.font );
        doc.setTextWidth( m_view->width() - iconSize.width() - 10 );
        doc.setHtml( text( index ) );
        return QSize( iconSize.width() + doc.size().width(), iconSize.height() );
    }

    return QSize();
}

QString MapViewItemDelegate::text( const QModelIndex &index ) const
{
    QString const title = index.data().toString();
    QString const description = index.data( Qt::UserRole+2 ).toString();
    return QString("<p><b>%1</b></p>%2").arg( title ).arg( description );
}

}

#include "MapViewWidget.moc"
