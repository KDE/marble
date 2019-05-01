//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
// Copyright 2011      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "GoToDialog.h"
#include "ui_GoToDialog.h"

#include "BookmarkManager.h"
#include "MarbleWidget.h"
#include "MarbleModel.h"
#include "MarblePlacemarkModel.h"
#include "GeoDataLookAt.h"
#include "GeoDataTreeModel.h"
#include "GeoDataDocument.h"
#include "GeoDataFolder.h"
#include "GeoDataPlacemark.h"
#include "PositionTracking.h"
#include "SearchRunnerManager.h"
#include "routing/RoutingManager.h"
#include "routing/RouteRequest.h"

#include <QAbstractListModel>
#include <QTimer>
#include <QPainter>

namespace Marble
{

class TargetModel : public QAbstractListModel
{
    Q_OBJECT
public:
    TargetModel( MarbleModel* marbleModel, QObject * parent = nullptr );

    int rowCount ( const QModelIndex & parent = QModelIndex() ) const override;

    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const override;

    void setShowRoutingItems( bool show );

private:
    QVariant currentLocationData ( int role ) const;

    QVariant routeData ( const QVector<GeoDataPlacemark> &via, int index, int role ) const;

    QVariant homeData ( int role ) const;

    QVariant bookmarkData ( int index, int role ) const;

    QVector<GeoDataPlacemark> viaPoints() const;

    MarbleModel *const m_marbleModel;

    QVector<GeoDataPlacemark*> m_bookmarks;

    bool m_hasCurrentLocation;

    bool m_showRoutingItems;
};

class GoToDialogPrivate : public Ui::GoTo
{
public:
    GoToDialog* m_parent;

    GeoDataCoordinates m_coordinates;

    MarbleModel *const m_marbleModel;

    TargetModel m_targetModel;

    SearchRunnerManager m_runnerManager;

    GeoDataDocument *m_searchResult;

    GeoDataTreeModel m_searchResultModel;

    QTimer m_progressTimer;

    int m_currentFrame;

    QVector<QIcon> m_progressAnimation;

    GoToDialogPrivate( GoToDialog* parent, MarbleModel* marbleModel );

    void saveSelection( const QModelIndex &index );

    void createProgressAnimation();

    void startSearch();

    void updateSearchResult( const QVector<GeoDataPlacemark*>& placemarks );

    void updateSearchMode();

    void updateProgress();

    void stopProgressAnimation();

    void updateResultMessage( int results );
};

TargetModel::TargetModel( MarbleModel *marbleModel, QObject * parent ) :
    QAbstractListModel( parent ),
    m_marbleModel( marbleModel ),
    m_hasCurrentLocation( false ),
    m_showRoutingItems( true )
{
    BookmarkManager* manager = m_marbleModel->bookmarkManager();
    for( GeoDataFolder * folder: manager->folders() ) {
        QVector<GeoDataPlacemark*> bookmarks = folder->placemarkList();
        QVector<GeoDataPlacemark*>::const_iterator iter = bookmarks.constBegin();
        QVector<GeoDataPlacemark*>::const_iterator end = bookmarks.constEnd();

        for ( ; iter != end; ++iter ) {
            m_bookmarks.push_back( *iter );
        }
    }

    PositionTracking* tracking = m_marbleModel->positionTracking();
    m_hasCurrentLocation = tracking && tracking->status() == PositionProviderStatusAvailable;
}

QVector<GeoDataPlacemark> TargetModel::viaPoints() const
{
    if ( !m_showRoutingItems ) {
        return QVector<GeoDataPlacemark>();
    }

    RouteRequest* request = m_marbleModel->routingManager()->routeRequest();
    QVector<GeoDataPlacemark> result;
    for ( int i = 0; i < request->size(); ++i ) {
        if ( request->at( i ).isValid() ) {
            GeoDataPlacemark placemark;
            placemark.setCoordinate( request->at( i ) );
            placemark.setName( request->name( i ) );
            result.push_back( placemark );
        }
    }
    return result;
}

int TargetModel::rowCount ( const QModelIndex & parent ) const
{
    int result = 0;
    if ( !parent.isValid() ) {
        result += m_hasCurrentLocation ? 1 : 0;
        result += viaPoints().size(); // route
        result += 1; // home location
        result += m_bookmarks.size(); // bookmarks
        return result;
    }

    return result;
}

QVariant TargetModel::currentLocationData ( int role ) const
{
    const PositionTracking* tracking = m_marbleModel->positionTracking();
    if ( tracking->status() == PositionProviderStatusAvailable ) {
        GeoDataCoordinates currentLocation = tracking->currentLocation();
        switch( role ) {
        case Qt::DisplayRole: return tr( "Current Location: %1" ).arg( currentLocation.toString() ) ;
        case Qt::DecorationRole: return QIcon(QStringLiteral(":/icons/gps.png"));
        case MarblePlacemarkModel::CoordinateRole: {
            return qVariantFromValue( currentLocation );
        }
        }
    }

    return QVariant();
}

QVariant TargetModel::routeData ( const QVector<GeoDataPlacemark> &via, int index, int role ) const
{
    RouteRequest* request = m_marbleModel->routingManager()->routeRequest();
    switch( role ) {
    case Qt::DisplayRole: return via.at( index ).name();
    case Qt::DecorationRole: return QIcon( request->pixmap( index ) );
    case MarblePlacemarkModel::CoordinateRole: {
        const GeoDataCoordinates coordinates = via.at( index ).coordinate();
        return qVariantFromValue( coordinates );
    }
    }

    return QVariant();
}

QVariant TargetModel::homeData ( int role ) const
{
    switch( role ) {
    case Qt::DisplayRole: return tr( "Home" );
    case Qt::DecorationRole: return QIcon(QStringLiteral(":/icons/go-home.png"));
    case MarblePlacemarkModel::CoordinateRole: {
        qreal lon( 0.0 ), lat( 0.0 );
        int zoom( 0 );
        m_marbleModel->home( lon, lat, zoom );
        const GeoDataCoordinates coordinates = GeoDataCoordinates( lon, lat, 0, GeoDataCoordinates::Degree );
        return qVariantFromValue( coordinates );
    }
    }

    return QVariant();
}

QVariant TargetModel::bookmarkData ( int index, int role ) const
{
    switch( role ) {
    case Qt::DisplayRole: {
        const GeoDataFolder *folder = geodata_cast<GeoDataFolder>(m_bookmarks[index]->parent());
        Q_ASSERT( folder && "Internal bookmark representation has changed. Please report this as a bug at http://bugs.kde.org." );
        if ( folder ) {
            return QString(folder->name() + QLatin1String(" / ") + m_bookmarks[index]->name());
        }
        return QVariant();
    }
    case Qt::DecorationRole: return QIcon(QStringLiteral(":/icons/bookmarks.png"));
    case MarblePlacemarkModel::CoordinateRole: return qVariantFromValue( m_bookmarks[index]->lookAt()->coordinates() );
    }

    return QVariant();
}


QVariant TargetModel::data ( const QModelIndex & index, int role ) const
{
    if ( index.isValid() && index.row() >= 0 && index.row() < rowCount() ) {
        int row = index.row();
        bool const isCurrentLocation = row == 0 && m_hasCurrentLocation;
        int homeOffset = m_hasCurrentLocation ? 1 : 0;
        QVector<GeoDataPlacemark> via = viaPoints();
        bool const isRoute = row >= homeOffset && row < homeOffset + via.size();

        if ( isCurrentLocation ) {
            return currentLocationData( role );
        } else if ( isRoute ) {
            int routeIndex = row - homeOffset;
            Q_ASSERT( routeIndex >= 0 && routeIndex < via.size() );
            return routeData( via, routeIndex, role );
        } else {
            int bookmarkIndex = row - homeOffset - via.size();
            if ( bookmarkIndex == 0 ) {
                return homeData( role );
            } else {
                --bookmarkIndex;
                Q_ASSERT( bookmarkIndex >= 0 && bookmarkIndex < m_bookmarks.size() );
                return bookmarkData( bookmarkIndex, role );
            }
        }
    }

    return QVariant();
}

void TargetModel::setShowRoutingItems( bool show )
{
    m_showRoutingItems = show;
    beginResetModel();
    endResetModel();
}

void GoToDialogPrivate::createProgressAnimation()
{
    bool const smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    int const iconSize = smallScreen ? 32 : 16;

    // Size parameters
    qreal const h = iconSize / 2.0; // Half of the icon size
    qreal const q = h / 2.0; // Quarter of the icon size
    qreal const d = 7.5; // Circle diameter
    qreal const r = d / 2.0; // Circle radius

    // Canvas parameters
    QImage canvas( iconSize, iconSize, QImage::Format_ARGB32 );
    QPainter painter( &canvas );
    painter.setRenderHint( QPainter::Antialiasing, true );
    painter.setPen( QColor ( Qt::gray ) );
    painter.setBrush( QColor( Qt::white ) );

    // Create all frames
    for( double t = 0.0; t < 2 * M_PI; t += M_PI / 8.0 ) {
        canvas.fill( Qt::transparent );
        QRectF firstCircle( h - r + q * cos( t ), h - r + q * sin( t ), d, d );
        QRectF secondCircle( h - r + q * cos( t + M_PI ), h - r + q * sin( t + M_PI ), d, d );
        painter.drawEllipse( firstCircle );
        painter.drawEllipse( secondCircle );
        m_progressAnimation.push_back( QIcon( QPixmap::fromImage( canvas ) ) );
    }
}

GoToDialogPrivate::GoToDialogPrivate( GoToDialog* parent, MarbleModel* marbleModel ) :
    m_parent( parent),
    m_marbleModel( marbleModel ),
    m_targetModel( marbleModel ),
    m_runnerManager( marbleModel ),
    m_searchResult( new GeoDataDocument ),
    m_currentFrame( 0 )
{
    setupUi( parent );

    m_progressTimer.setInterval( 100 );
}

void GoToDialogPrivate::saveSelection( const QModelIndex &index )
{
    if ( searchButton->isChecked() && m_searchResult->size() ) {
        QVariant coordinates = m_searchResultModel.data( index, MarblePlacemarkModel::CoordinateRole );
        m_coordinates = coordinates.value<GeoDataCoordinates>();
    } else {
        QVariant coordinates = index.data( MarblePlacemarkModel::CoordinateRole );
        m_coordinates = coordinates.value<GeoDataCoordinates>();
    }
    m_parent->accept();
}

void GoToDialogPrivate::startSearch()
{
    QString const searchTerm = searchLineEdit->text().trimmed();
    if ( searchTerm.isEmpty() ) {
        return;
    }

    m_runnerManager.findPlacemarks( searchTerm );
    if ( m_progressAnimation.isEmpty() ) {
        createProgressAnimation();
    }
    m_progressTimer.start();
    progressButton->setVisible( true );
    searchLineEdit->setEnabled( false );
    updateResultMessage( 0 );
}

void GoToDialogPrivate::updateSearchResult( const QVector<GeoDataPlacemark*>& placemarks )
{
    m_searchResultModel.setRootDocument( nullptr );
    m_searchResult->clear();
    for (GeoDataPlacemark *placemark: placemarks) {
        m_searchResult->append( new GeoDataPlacemark( *placemark ) );
    }
    m_searchResultModel.setRootDocument( m_searchResult );
    bookmarkListView->setModel( &m_searchResultModel );
    updateResultMessage( m_searchResultModel.rowCount() );
}

GoToDialog::GoToDialog( MarbleModel* marbleModel, QWidget * parent, Qt::WindowFlags flags ) :
    QDialog( parent, flags ),
    d( new GoToDialogPrivate( this, marbleModel ) )
{
    d->searchLineEdit->setPlaceholderText( tr( "Address or search term" ) );

    d->m_searchResultModel.setRootDocument( d->m_searchResult );
    d->bookmarkListView->setModel( &d->m_targetModel );
    connect( d->bookmarkListView, SIGNAL(activated(QModelIndex)),
             this, SLOT(saveSelection(QModelIndex)) );
    connect( d->searchLineEdit, SIGNAL(returnPressed()),
             this, SLOT(startSearch()) );
    d->buttonBox->button( QDialogButtonBox::Close )->setAutoDefault( false );
    connect( d->searchButton, SIGNAL(clicked(bool)),
             this, SLOT(updateSearchMode()) );
    connect( d->browseButton, SIGNAL(clicked(bool)),
             this, SLOT(updateSearchMode()) );
    connect( &d->m_progressTimer, SIGNAL(timeout()),
             this, SLOT(updateProgress()) );
    connect( d->progressButton, SIGNAL(clicked(bool)),
             this, SLOT(stopProgressAnimation()) );
    d->updateSearchMode();
    d->progressButton->setVisible( false );

    connect( &d->m_runnerManager, SIGNAL(searchResultChanged(QVector<GeoDataPlacemark*>)),
             this, SLOT(updateSearchResult(QVector<GeoDataPlacemark*>)) );
    connect( &d->m_runnerManager, SIGNAL(searchFinished(QString)),
             this, SLOT(stopProgressAnimation()) );
}

GoToDialog::~GoToDialog()
{
    delete d;
}

GeoDataCoordinates GoToDialog::coordinates() const
{
    return d->m_coordinates;
}

void GoToDialog::setShowRoutingItems( bool show )
{
    d->m_targetModel.setShowRoutingItems( show );
}

void GoToDialog::setSearchEnabled( bool enabled )
{
    d->browseButton->setVisible( enabled );
    d->searchButton->setVisible( enabled );
    if ( !enabled ) {
        d->searchButton->setChecked( false );
        d->updateSearchMode();
    }
}

void GoToDialogPrivate::updateSearchMode()
{
    bool const searchEnabled = searchButton->isChecked();
    searchLineEdit->setVisible( searchEnabled );
    descriptionLabel->setVisible( searchEnabled );
    progressButton->setVisible( searchEnabled && m_progressTimer.isActive() );
    if ( searchEnabled ) {
        bookmarkListView->setModel( &m_searchResultModel );
        searchLineEdit->setFocus();
    } else {
        bookmarkListView->setModel( &m_targetModel );
    }
}

void GoToDialogPrivate::updateProgress()
{
    if ( !m_progressAnimation.isEmpty() ) {
        m_currentFrame = ( m_currentFrame + 1 ) % m_progressAnimation.size();
        QIcon frame = m_progressAnimation[m_currentFrame];
        progressButton->setIcon( frame );
    }
}

void GoToDialogPrivate::stopProgressAnimation()
{
    searchLineEdit->setEnabled( true );
    m_progressTimer.stop();
    updateResultMessage( bookmarkListView->model()->rowCount() );
    progressButton->setVisible( false );
}

void GoToDialogPrivate::updateResultMessage( int results )
{
    //~ singular %n result found.
    //~ plural %n results found.
    descriptionLabel->setText( QObject::tr( "%n result(s) found.", "Number of search results", results ) );
}

}

#include "moc_GoToDialog.cpp" // needed for private slots in header
#include "GoToDialog.moc" // needed for Q_OBJECT here in source
