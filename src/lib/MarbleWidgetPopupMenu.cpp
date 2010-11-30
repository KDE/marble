//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//

// Self
#include "MarbleWidgetPopupMenu.h"

// Marble
#include "AbstractDataPluginItem.h"
#include "MarbleAboutDialog.h"
#include "MarbleWidget.h"
#include "MarbleModel.h"
#include "GeoDataPlacemark.h"
#include "PlacemarkInfoDialog.h"
#include "MarbleDebug.h"
#include "Planet.h"
#include "routing/RoutingManager.h"
#include "routing/RouteRequest.h"
#include "MarbleRunnerManager.h"
#include "BookmarkInfoDialog.h"
#include "GoToDialog.h"

// Qt
#include <QtCore/QMimeData>
#include <QtCore/QPointer>
#include <QtGui/QAction>
#include <QtGui/QClipboard>
#include <QtGui/QMenu>
#include <QtGui/QMessageBox>

using namespace Marble;

MarbleWidgetPopupMenu::MarbleWidgetPopupMenu(MarbleWidget *widget, 
                                         MarbleModel *model)
    : QObject(widget),
      m_model(model),
      m_widget(widget),
      m_lmbMenu( new QMenu( m_widget ) ),
      m_rmbMenu( new QMenu( m_widget ) ),
      m_smallScreenMenu( new QMenu( m_widget ) ),
      m_runnerManager( 0 )
{
    connect( m_lmbMenu, SIGNAL( triggered( QAction* ) ),
             this,      SLOT( showFeatureInfo( QAction* ) ) );
    createActions();
}

void MarbleWidgetPopupMenu::createActions()
{
    //	Property actions (Left mouse button)
    //	m_planetAction = new QAction(QIcon("icon.png"), tr("&Earth"), this);
    m_planetAction = new QAction( tr( "&Earth" ), this );
    m_planetAction->setData( 0 );
    m_copyCoordinateAction = new QAction( tr( "0 N 0 W" ), this );

    //	Tool actions (Right mouse button)
    QAction* fromHere = new QAction( tr( "Directions &from here" ), this );
    QAction* toHere = new QAction( tr( "Directions &to here" ), this );
    m_setHomePointAction  = new QAction( tr( "&Set Home Location" ), this);
    QAction* addBookmark = new QAction( QIcon(":/icons/bookmark-new.png"),
                                        tr( "Add &Bookmark" ), this );

    QAction *reloadAction  = new QAction( tr( "Rel&oad Map" ), this);
    m_aboutDialogAction = new QAction( tr( "&About" ), this );
    QAction *goToAction = new QAction( tr( "&Go to..."), this );

    m_rmbExtensionPoint = m_rmbMenu->addSeparator();
    m_rmbMenu->addAction( fromHere );
    m_rmbMenu->addAction( toHere );
    m_rmbMenu->addSeparator();
    m_rmbMenu->addAction( m_setHomePointAction );
    m_rmbMenu->addAction( addBookmark );
    m_rmbMenu->addSeparator();
    m_rmbMenu->addAction( reloadAction );
    m_rmbMenu->addSeparator();
    m_rmbMenu->addAction( m_aboutDialogAction );

    m_smallScreenMenu->addAction( fromHere );
    m_smallScreenMenu->addAction( toHere );
    m_smallScreenMenu->addSeparator();
    m_smallScreenMenu->addAction( m_setHomePointAction );
    m_smallScreenMenu->addAction( addBookmark );
    m_smallScreenMenu->addSeparator();
    m_smallScreenMenu->addAction( goToAction );
    m_smallScreenMenu->addSeparator();

    connect( fromHere, SIGNAL( triggered( ) ), SLOT( directionsFromHere() ) );
    connect( toHere, SIGNAL( triggered( ) ), SLOT( directionsToHere() ) );
    connect( m_setHomePointAction, SIGNAL( triggered() ), SLOT( slotSetHomePoint() ) );
    connect( addBookmark, SIGNAL( triggered( ) ), SLOT( addBookmark() ) );
    connect( m_aboutDialogAction, SIGNAL( triggered() ), SLOT( slotAboutDialog() ) );
    connect( m_copyCoordinateAction,SIGNAL( triggered() ), SLOT( slotCopyCoordinates() ) );
    connect( reloadAction, SIGNAL(triggered()), m_widget, SLOT(reloadMap()));
    connect( goToAction, SIGNAL(triggered()), this, SLOT( openGoToDialog() ) );
}


void MarbleWidgetPopupMenu::showLmbMenu( int xpos, int ypos )
{
    QPoint  curpos = QPoint( xpos, ypos ); 
    bool const smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    if ( smallScreen ) {
        m_setHomePointAction->setData( curpos );
        m_smallScreenMenu->popup( m_widget->mapToGlobal( curpos ) );
        return;
    }

    m_lmbMenu->clear();
    m_featurelist = m_widget->whichFeatureAt( curpos );

    int  actionidx = 1;

    QVector<QModelIndex>::const_iterator it = m_featurelist.constBegin();
    QVector<QModelIndex>::const_iterator const itEnd = m_featurelist.constEnd();
    for (; it != itEnd; ++it )
    {
        QAction *action = new QAction( (*it).data().toString(), m_lmbMenu );
        action->setData( actionidx );
        action->setIcon( (*it).data( Qt::DecorationRole ).value<QPixmap>() );
        m_lmbMenu->addAction( action );
        actionidx++;
    }
    
    m_itemList = m_widget->whichItemAt( curpos );
    QList<AbstractDataPluginItem *>::const_iterator itW = m_itemList.constBegin();
    QList<AbstractDataPluginItem *>::const_iterator const itWEnd = m_itemList.constEnd();
    for (; itW != itWEnd; ++itW )
    {
        m_lmbMenu->addAction( (*itW)->action() );
    }

    // Not implemented yet ;-)
    m_planetAction->setEnabled( false );

    m_lmbMenu->addAction( m_planetAction );
    m_lmbMenu->addSeparator();

    // Setting the proper planet name
    QString targetString = m_model->planet()->name();
    mDebug() << "targetString" << targetString;
    // FIXME: this should be removed later on ...
    if ( targetString == "Earth" ) 
        targetString = tr( "&Earth" );

    m_planetAction->setText( targetString );

    qreal  lat;
    qreal  lon;

    m_widget->geoCoordinates( xpos, ypos, lon, lat, GeoDataCoordinates::Radian );

    m_copyCoordinateAction->setEnabled( true );
    m_copyCoordinateAction->setText( tr("Copy Coordinates") );
    m_copyCoordinateAction->setData( curpos );

    QMenu *positionMenu = m_lmbMenu->addMenu( GeoDataCoordinates( lon, lat, GeoDataCoordinates::Radian ).toString() );
    positionMenu->menuAction()->setFont( QFont( "Sans Serif", 7, 50, false ) );
    positionMenu->addAction( m_copyCoordinateAction );
    positionMenu->addAction( tr( "&Address Details" ), this, SLOT( startReverseGeocoding() ) );

    m_lmbMenu->popup( m_widget->mapToGlobal( curpos ) );
}


void MarbleWidgetPopupMenu::showRmbMenu( int xpos, int ypos )
{
    QPoint curpos = QPoint( xpos, ypos );
    m_setHomePointAction->setData( curpos );
    m_rmbMenu->popup( m_widget->mapToGlobal( curpos ) );
}


void MarbleWidgetPopupMenu::showFeatureInfo( QAction* action )
{
    int actionidx = action->data().toInt();

    if ( actionidx > 0 ) {
        QModelIndex index = m_featurelist.at( actionidx -1 );

        QPointer<PlacemarkInfoDialog> dialog = new PlacemarkInfoDialog( index, m_widget );
        dialog->setWindowModality( Qt::WindowModal );
        dialog->exec();
        delete dialog;
    }
}

void MarbleWidgetPopupMenu::slotSetHomePoint()
{
    GeoDataCoordinates coordinates;
    if ( mouseCoordinates( &coordinates, m_setHomePointAction ) ) {
        m_widget->setHome( coordinates, m_widget->zoom() );
    }
}

void MarbleWidgetPopupMenu::slotCopyCoordinates()
{
    GeoDataCoordinates coordinates;
    if ( mouseCoordinates( &coordinates, m_setHomePointAction ) ) {
	const qreal longitude_degrees = coordinates.longitude(GeoDataCoordinates::Degree);
	const qreal latitude_degrees = coordinates.latitude(GeoDataCoordinates::Degree);

	// importing this representation into Marble does not show anything,
	// but Merkaartor shows the point
	const QString kmlRepresentation = QString::fromLatin1(
	  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
	  "<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n"
	  "<Document>\n"
	  " <Placemark>\n"
// 	  "   <name></name>\n"
	  "   <Point>\n"
	  "     <coordinates>%1,%2</coordinates>\n"
	  "   </Point>\n"
	  " </Placemark>\n"
	  "</Document>\n"
	  "</kml>\n"
	  ).arg(longitude_degrees, 0, 'f', 10).arg(latitude_degrees, 0, 'f', 10);

	  // importing this data into Marble and Merkaartor works
	  const QString gpxRepresentation = QString::fromLatin1(
	    "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\n"
	    "<gpx xmlns=\"http://www.topografix.com/GPX/1/1\" creator=\"trippy\" version=\"0.1\"\n"
	    " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n"
	    " xsi:schemaLocation=\"http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd\">\n"
	    "  <wpt lat=\"%1\" lon=\"%2\">\n"
// 	    "   <ele>%3</ele>\n"
//      "   <time></time>\n"
// 	    "   <name>%4</name>\n"
	    "  </wpt>\n"
	    "</gpx>\n"
	    ).arg(latitude_degrees, 0, 'f', 10).arg(longitude_degrees, 0, 'f', 10);

	    QString  positionString = coordinates.toString();

	    QMimeData * const myMimeData = new QMimeData();
	    myMimeData->setText(positionString);
	    myMimeData->setData(QLatin1String("application/vnd.google-earth.kml+xml"), kmlRepresentation.toUtf8());
	    myMimeData->setData(QLatin1String("application/gpx+xml"), gpxRepresentation.toUtf8());

	    QClipboard * const clipboard = QApplication::clipboard();
	    clipboard->setMimeData(myMimeData);
    }
}

void MarbleWidgetPopupMenu::slotAboutDialog()
{
    MarbleAboutDialog dlg( m_widget );
    dlg.exec();
}

void MarbleWidgetPopupMenu::addAction( Qt::MouseButton button, QAction* action )
{
    if ( button == Qt::RightButton ) {
        m_rmbMenu->insertAction( m_rmbExtensionPoint, action );
    }
}

void MarbleWidgetPopupMenu::directionsFromHere()
{
    RouteRequest* request = m_widget->model()->routingManager()->routeRequest();
    if ( request )
    {
        GeoDataCoordinates coordinates;
        if ( mouseCoordinates( &coordinates, m_setHomePointAction ) ) {
            if ( request->size() > 0 ) {
                request->setPosition( 0, coordinates );
            } else {
                request->append( coordinates );
            }
            m_widget->model()->routingManager()->updateRoute();
        }
    }
}

void MarbleWidgetPopupMenu::directionsToHere()
{
    RouteRequest* request = m_widget->model()->routingManager()->routeRequest();
    if ( request )
    {
        GeoDataCoordinates coordinates;
        if ( mouseCoordinates( &coordinates, m_setHomePointAction ) ) {
            if ( request->size() > 1 ) {
                request->setPosition( request->size()-1, coordinates );
            } else {
                request->append( coordinates );
            }
            m_widget->model()->routingManager()->updateRoute();
        }
    }
}

bool MarbleWidgetPopupMenu::mouseCoordinates( GeoDataCoordinates* coordinates, QAction* dataContainer )
{
    Q_ASSERT( coordinates && "You must not pass 0 as coordinates parameter");
    if ( !dataContainer ) {
        return false;
    }

    QPoint p = dataContainer->data().toPoint();
    qreal lat( 0.0 ), lon( 0.0 );
    bool valid = m_widget->geoCoordinates( p.x(), p.y(), lon, lat, GeoDataCoordinates::Radian );
    if ( valid )
    {
        *coordinates = GeoDataCoordinates( lon, lat );
    }

    return valid;
}

void MarbleWidgetPopupMenu::startReverseGeocoding()
{
    if ( !m_runnerManager ) {
        m_runnerManager = new MarbleRunnerManager( m_model->pluginManager(), this );
        connect( m_runnerManager, SIGNAL( reverseGeocodingFinished( GeoDataCoordinates, GeoDataPlacemark ) ),
                 this, SLOT(showAddressInformation( GeoDataCoordinates, GeoDataPlacemark) ) );
    }

    GeoDataCoordinates coordinates;
    if ( mouseCoordinates( &coordinates, m_copyCoordinateAction ) ) {
        m_runnerManager->reverseGeocoding( coordinates );
    }
}

void MarbleWidgetPopupMenu::showAddressInformation(const GeoDataCoordinates &, const GeoDataPlacemark &placemark)
{
    QString text = placemark.address();
    if ( !text.isEmpty() ) {
        QMessageBox::information( m_widget, tr( "Address Details" ), text, QMessageBox::Ok );
    }
}

void MarbleWidgetPopupMenu::addBookmark()
{
    GeoDataCoordinates coordinates;
    if ( mouseCoordinates( &coordinates, m_setHomePointAction ) ) {
        QPointer<BookmarkInfoDialog> dialog = new BookmarkInfoDialog( coordinates, m_widget );
        dialog->exec();
        delete dialog;
    }
}

void MarbleWidgetPopupMenu::openGoToDialog()
{
    QPointer<GoToDialog> dialog = new GoToDialog( m_widget, m_widget );
    if ( dialog->exec() == QDialog::Accepted ) {
        GeoDataLookAt lookAt = dialog->lookAt();
        m_widget->flyTo( lookAt );
    }
    delete dialog;
}

#include "MarbleWidgetPopupMenu.moc"
