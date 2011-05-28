//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "GoToDialog.h"
#include "MarbleWidget.h"
#include "MarbleModel.h"
#include "MarbleRunnerManager.h"
#include "MarblePlacemarkModel.h"
#include "GeoDataFolder.h"
#include "PositionTracking.h"
#include "BookmarkManager.h"
#include "routing/RoutingManager.h"
#include "routing/RouteRequest.h"

#include <QtCore/QAbstractListModel>
#include <QtCore/QTimer>
#include <QtGui/QPushButton>
#include <QtGui/QPainter>

namespace Marble
{

namespace
{
int const GeoDataLookAtRole = Qt::UserRole + 1;
}

class TargetModel : public QAbstractListModel
{
public:
    TargetModel( MarbleWidget* marbleWidget, QObject * parent = 0 );

    virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;

    virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;

    void setShowRoutingItems( bool show );

private:
    QVariant currentLocationData ( int role ) const;

    QVariant routeData ( const QVector<GeoDataPlacemark> &via, int index, int role ) const;

    QVariant homeData ( int role ) const;

    QVariant bookmarkData ( int index, int role ) const;

    QVector<GeoDataPlacemark> viaPoints() const;

    MarbleWidget* m_marbleWidget;

    QVector<GeoDataPlacemark*> m_bookmarks;

    bool m_hasCurrentLocation;

    bool m_showRoutingItems;
};

class GoToDialogPrivate
{
public:
    GoToDialog* m_parent;

    MarbleWidget* m_marbleWidget;

    GeoDataLookAt m_lookAt;

    TargetModel* m_targetModel;

    MarbleRunnerManager* m_runnerManager;

    MarblePlacemarkModel *m_placemarkModel;

    QTimer m_progressTimer;

    int m_currentFrame;

    QVector<QIcon> m_progressAnimation;

    bool m_workOffline;

    GoToDialogPrivate( GoToDialog* parent, MarbleWidget* marbleWidget );

    void saveSelection( const QModelIndex &index );

    void createProgressAnimation();
};

TargetModel::TargetModel( MarbleWidget* marbleWidget, QObject * parent ) :
    QAbstractListModel( parent ),
    m_marbleWidget( marbleWidget ), m_hasCurrentLocation( false ),
    m_showRoutingItems( true )
{
    BookmarkManager* manager = marbleWidget->model()->bookmarkManager();
    foreach( GeoDataFolder * folder, manager->folders() ) {
        QVector<GeoDataPlacemark*> bookmarks = folder->placemarkList();
        QVector<GeoDataPlacemark*>::const_iterator iter = bookmarks.constBegin();
        QVector<GeoDataPlacemark*>::const_iterator end = bookmarks.constEnd();

        for ( ; iter != end; ++iter ) {
            m_bookmarks.push_back( *iter );
        }
    }

    PositionTracking* tracking = m_marbleWidget->model()->positionTracking();
    m_hasCurrentLocation = tracking && tracking->status() == PositionProviderStatusAvailable;
}

QVector<GeoDataPlacemark> TargetModel::viaPoints() const
{
    if ( !m_showRoutingItems ) {
        return QVector<GeoDataPlacemark>();
    }

    RouteRequest* request = m_marbleWidget->model()->routingManager()->routeRequest();
    QVector<GeoDataPlacemark> result;
    for ( int i = 0; i < request->size(); ++i ) {
        if ( request->at( i ).longitude() != 0.0 || request->at( i ).latitude() != 0.0 ) {
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
    PositionTracking* tracking = m_marbleWidget->model()->positionTracking();
    if ( tracking && tracking->status() == PositionProviderStatusAvailable ) {
        GeoDataCoordinates currentLocation = tracking->currentLocation();
        switch( role ) {
        case Qt::DisplayRole: return tr( "Current Location: %1" ).arg( currentLocation.toString() ) ;
        case Qt::DecorationRole: return QIcon( ":/icons/gps.png" );
        case GeoDataLookAtRole: {
            GeoDataLookAt result;
            result.setCoordinates( currentLocation );
            // By happy coincidence this equals OpenStreetMap tile level 15
            result.setRange( 851.807 );
            return qVariantFromValue( result );
        }
        }
    }

    return QVariant();
}

QVariant TargetModel::routeData ( const QVector<GeoDataPlacemark> &via, int index, int role ) const
{
    RouteRequest* request = m_marbleWidget->model()->routingManager()->routeRequest();
    switch( role ) {
    case Qt::DisplayRole: return via.at( index ).name();
    case Qt::DecorationRole: return QIcon( request->pixmap( index ) );
    case GeoDataLookAtRole: {
        GeoDataLookAt result;
        result.setCoordinates( via.at( index ).coordinate() );
        // By happy coincidence this equals OpenStreetMap tile level 16
        result.setRange( 425.903 );
        return qVariantFromValue( result );
    }
    }

    return QVariant();
}

QVariant TargetModel::homeData ( int role ) const
{
    switch( role ) {
    case Qt::DisplayRole: return tr( "Home" );
    case Qt::DecorationRole: return QIcon( ":/icons/go-home.png" );
    case GeoDataLookAtRole: {
        qreal lon( 0.0 ), lat( 0.0 );
        int zoom( 0 );
        m_marbleWidget->model()->home( lon, lat, zoom );
        GeoDataLookAt result;
        result.setLongitude( lon, GeoDataCoordinates::Degree );
        result.setLatitude( lat, GeoDataCoordinates::Degree );
        result.setRange( m_marbleWidget->distanceFromZoom( zoom ) * KM2METER );
        return qVariantFromValue( result );
    }
    }

    return QVariant();
}

QVariant TargetModel::bookmarkData ( int index, int role ) const
{
    switch( role ) {
    case Qt::DisplayRole: {
        GeoDataFolder* folder = dynamic_cast<GeoDataFolder*>( m_bookmarks[index]->parent() );
        Q_ASSERT( folder && "Internal bookmark representation has changed. Please report this as a bug at http://bugs.kde.org." );
        if ( folder ) {
            return folder->name() + " / " + m_bookmarks[index]->name();
        }
    }
    case Qt::DecorationRole: return QIcon( ":/icons/bookmarks.png" );
    case GeoDataLookAtRole: return qVariantFromValue( *m_bookmarks[index]->lookAt() );
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
    reset();
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

GoToDialogPrivate::GoToDialogPrivate( GoToDialog* parent, MarbleWidget* marbleWidget ) :
    m_parent( parent), m_marbleWidget( marbleWidget ), m_targetModel( 0 ),
    m_runnerManager( 0 ), m_placemarkModel( 0 ), m_currentFrame( 0 ),
    m_workOffline( false )
{
    m_progressTimer.setInterval( 100 );
}

void GoToDialogPrivate::saveSelection( const QModelIndex &index )
{
    if ( m_parent->searchButton->isChecked() && m_placemarkModel ) {
        QVariant coordinates = m_placemarkModel->data( index, MarblePlacemarkModel::CoordinateRole );
        m_lookAt = GeoDataLookAt();
        m_lookAt.setCoordinates( qVariantValue<GeoDataCoordinates>( coordinates ) );
        // By happy coincidence this equals OpenStreetMap tile level 16
        m_lookAt.setRange( 425.903 );
    } else {
        QVariant data = index.data( GeoDataLookAtRole );
        m_lookAt = qVariantValue<GeoDataLookAt>( data );
    }
    m_parent->accept();
}

void GoToDialog::startSearch()
{
    QString const searchTerm = searchLineEdit->text().trimmed();
    if ( searchTerm.isEmpty() ) {
        return;
    }

    if ( !d->m_runnerManager ) {
        d->m_runnerManager = new MarbleRunnerManager( d->m_marbleWidget->model()->pluginManager(), this );
        d->m_runnerManager->setModel( d->m_marbleWidget->model() );
        connect( d->m_runnerManager, SIGNAL( searchResultChanged( QAbstractItemModel* ) ),
                 this, SLOT( updateSearchResult( QAbstractItemModel* ) ) );
        connect( d->m_runnerManager, SIGNAL( searchFinished( QString ) ),
                this, SLOT( stopProgressAnimation() ) );
    }

    d->m_runnerManager->setWorkOffline( d->m_workOffline );
    d->m_runnerManager->findPlacemarks( searchTerm );
    if ( d->m_progressAnimation.isEmpty() ) {
        d->createProgressAnimation();
    }
    d->m_progressTimer.start();
    progress->setVisible( true );
    searchLineEdit->setEnabled( false );
    updateResultMessage( 0 );
}

void GoToDialog::updateSearchResult( QAbstractItemModel* model )
{
    d->m_placemarkModel = dynamic_cast<MarblePlacemarkModel*>( model );
    bookmarkListView->setModel( model );
    updateResultMessage( model->rowCount() );
}

GoToDialog::GoToDialog( MarbleWidget* marbleWidget, QWidget * parent, Qt::WindowFlags flags ) :
    QDialog( parent, flags ), d( new GoToDialogPrivate( this, marbleWidget ) )
{
#ifdef Q_WS_MAEMO_5
        setAttribute( Qt::WA_Maemo5StackedWindow );
        setWindowFlags( Qt::Window );
#endif // Q_WS_MAEMO_5
    setupUi( this );

#if QT_VERSION >= 0x40700
    searchLineEdit->setPlaceholderText( tr( "Address or search term" ) );
#endif

    d->m_targetModel = new TargetModel( marbleWidget, this );
    bookmarkListView->setModel( d->m_targetModel );
    connect( bookmarkListView, SIGNAL( activated( QModelIndex ) ),
             this, SLOT( saveSelection ( QModelIndex ) ) );
    connect( searchLineEdit, SIGNAL( returnPressed() ),
             this, SLOT( startSearch() ) );
    buttonBox->button( QDialogButtonBox::Close )->setAutoDefault( false );
    connect( searchButton, SIGNAL( clicked( bool ) ),
             this, SLOT( updateSearchMode() ) );
    connect( browseButton, SIGNAL( clicked( bool ) ),
             this, SLOT( updateSearchMode() ) );
    connect( &d->m_progressTimer, SIGNAL( timeout() ),
             this, SLOT( updateProgress() ) );
    updateSearchMode();
    progress->setVisible( false );
}

GoToDialog::~GoToDialog()
{
    delete d;
}

GeoDataLookAt GoToDialog::lookAt() const
{
    return d->m_lookAt;
}

void GoToDialog::setShowRoutingItems( bool show )
{
    d->m_targetModel->setShowRoutingItems( show );
}

void GoToDialog::setSearchEnabled( bool enabled )
{
    browseButton->setVisible( enabled );
    searchButton->setVisible( enabled );
    if ( !enabled ) {
        searchButton->setChecked( false );
        updateSearchMode();
    }
}

void GoToDialog::setWorkOffline( bool workOffline )
{
    d->m_workOffline = workOffline;
}

void GoToDialog::updateSearchMode()
{
    bool const searchEnabled = searchButton->isChecked();
    searchLineEdit->setVisible( searchEnabled );
    descriptionLabel->setVisible( searchEnabled );
    progress->setVisible( searchEnabled && d->m_progressTimer.isActive() );
    if ( searchEnabled ) {
        bookmarkListView->setModel( d->m_placemarkModel );
    } else {
        bookmarkListView->setModel( d->m_targetModel );
    }
}

void GoToDialog::updateProgress()
{
    if ( !d->m_progressAnimation.isEmpty() ) {
        d->m_currentFrame = ( d->m_currentFrame + 1 ) % d->m_progressAnimation.size();
        QIcon frame = d->m_progressAnimation[d->m_currentFrame];
        progress->setIcon( frame );
    }
}

void GoToDialog::stopProgressAnimation()
{
    searchLineEdit->setEnabled( true );
    d->m_progressTimer.stop();
    updateResultMessage( bookmarkListView->model()->rowCount() );
    progress->setVisible( false );
}

void GoToDialog::updateResultMessage( int results )
{
    descriptionLabel->setText( tr( "%n results found.", "Number of search results", results ) );
}

}

#include "GoToDialog.moc"
