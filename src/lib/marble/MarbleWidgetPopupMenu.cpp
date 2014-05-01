//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn      <tackat@kde.org>
// Copyright 2007      Inge Wallin       <ingwa@kde.org>
// Copyright 2012      Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

// Self
#include "MarbleWidgetPopupMenu.h"

// Marble
#include "AbstractDataPluginItem.h"
#include "AbstractFloatItem.h"
#include "MarbleAboutDialog.h"
#include "MarbleWidget.h"
#include "MarbleModel.h"
#include "GeoDataPlacemark.h"
#include "GeoDataStyle.h"
#include "MarbleClock.h"
#include "MarbleDebug.h"
#include "PopupLayer.h"
#include "Planet.h"
#include "routing/RoutingManager.h"
#include "routing/RoutingLayer.h"
#include "routing/RouteRequest.h"
#include "EditBookmarkDialog.h"
#include "BookmarkManager.h"
#include "ReverseGeocodingRunnerManager.h"

// Qt
#include <QApplication>
#include <QMimeData>
#include <QPointer>
#include <QAction>
#include <QClipboard>
#include <QMenu>
#include <QMessageBox>

namespace Marble {
/* TRANSLATOR Marble::MarbleWidgetPopupMenu */

class MarbleWidgetPopupMenu::Private {
public:
    const MarbleModel    *const m_model;
    MarbleWidget   *const m_widget;

    QVector<const GeoDataPlacemark*>  m_featurelist;
    QList<AbstractDataPluginItem *> m_itemList;

    QMenu m_lmbMenu;
    QMenu m_rmbMenu;

    QAction *m_infoDialogAction;
    QAction *m_directionsFromHereAction;
    QAction *m_directionsToHereAction;

    QAction  *const m_copyCoordinateAction;

    QAction  *m_rmbExtensionPoint;

    ReverseGeocodingRunnerManager m_runnerManager;

    QPoint m_mousePosition;

public:
    Private( MarbleWidget *widget, const MarbleModel *model, MarbleWidgetPopupMenu* parent );
    QMenu* createInfoBoxMenu();

    /**
      * Returns the geo coordinates of the mouse pointer at the last right button menu.
      * You must not pass 0 as coordinates parameter. The result indicates whether the
      * coordinates are valid, which will be true if the right button menu was opened at least once.
      */
    GeoDataCoordinates mouseCoordinates( QAction* dataContainer ) const;
};

MarbleWidgetPopupMenu::Private::Private( MarbleWidget *widget, const MarbleModel *model, MarbleWidgetPopupMenu* parent ) :
    m_model(model),
    m_widget(widget),
    m_lmbMenu( m_widget ),
    m_rmbMenu( m_widget ),
    m_directionsFromHereAction( 0 ),
    m_directionsToHereAction( 0 ),
    m_copyCoordinateAction( new QAction( QIcon(":/icons/copy-coordinates.png"), tr("Copy Coordinates"), parent ) ),
    m_rmbExtensionPoint( 0 ),
    m_runnerManager( model )
{
    // Property actions (Left mouse button)
    m_infoDialogAction = new QAction( parent );
    m_infoDialogAction->setData( 0 );

    //	Tool actions (Right mouse button)
    m_directionsFromHereAction = new QAction( tr( "Directions &from here" ), parent );
    m_directionsToHereAction = new QAction( tr( "Directions &to here" ), parent );
    RouteRequest* request = m_widget->model()->routingManager()->routeRequest();
    if ( request ) {
        m_directionsFromHereAction->setIcon( QIcon( request->pixmap( 0, 16 ) ) );
        int const lastIndex = qMax( 1, request->size()-1 );
        m_directionsToHereAction->setIcon( QIcon( request->pixmap( lastIndex, 16 ) ) );
    }
    QAction* addBookmark = new QAction( QIcon(":/icons/bookmark-new.png"),
                                        tr( "Add &Bookmark" ), parent );
    QAction* fullscreenAction = new QAction( tr( "&Full Screen Mode" ), parent );
    fullscreenAction->setCheckable( true );

    QAction* aboutDialogAction = new QAction( QIcon(":/icons/marble.png"), tr( "&About" ), parent );

    QMenu* infoBoxMenu = createInfoBoxMenu();

    const bool smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;

    if ( !smallScreen ) {
        m_rmbExtensionPoint = m_rmbMenu.addSeparator();
    }

    m_rmbMenu.addAction( m_directionsFromHereAction );
    m_rmbMenu.addAction( m_directionsToHereAction );
    m_rmbMenu.addSeparator();
    m_rmbMenu.addAction( addBookmark );
    if ( !smallScreen ) {
        m_rmbMenu.addAction( m_copyCoordinateAction );
    }
    m_rmbMenu.addAction( QIcon(":/icons/addressbook-details.png"), tr( "&Address Details" ), parent, SLOT(startReverseGeocoding()) );
    m_rmbMenu.addSeparator();
    m_rmbMenu.addMenu( infoBoxMenu );

    if ( !smallScreen ) {
        m_rmbMenu.addAction( aboutDialogAction );
    } else {
        m_rmbMenu.addAction( fullscreenAction );
    }

    parent->connect( &m_lmbMenu, SIGNAL(aboutToHide()), SLOT(resetMenu()) );
    parent->connect( m_directionsFromHereAction, SIGNAL(triggered()), SLOT(directionsFromHere()) );
    parent->connect( m_directionsToHereAction, SIGNAL(triggered()), SLOT(directionsToHere()) );
    parent->connect( addBookmark, SIGNAL(triggered()), SLOT(addBookmark()) );
    parent->connect( aboutDialogAction, SIGNAL(triggered()), SLOT(slotAboutDialog()) );
    parent->connect( m_copyCoordinateAction, SIGNAL(triggered()), SLOT(slotCopyCoordinates()) );
    parent->connect( m_infoDialogAction, SIGNAL(triggered()), SLOT(slotInfoDialog()) );
    parent->connect( fullscreenAction, SIGNAL(triggered(bool)), parent, SLOT(toggleFullscreen(bool)) );

    parent->connect( &m_runnerManager, SIGNAL(reverseGeocodingFinished(GeoDataCoordinates,GeoDataPlacemark)),
             parent, SLOT(showAddressInformation(GeoDataCoordinates,GeoDataPlacemark)) );
}

MarbleWidgetPopupMenu::MarbleWidgetPopupMenu(MarbleWidget *widget,
                                         const MarbleModel *model)
    : QObject(widget),
      d( new Private( widget, model, this ) )
{
    // nothing to do
}

MarbleWidgetPopupMenu::~MarbleWidgetPopupMenu()
{
    delete d;
}

QMenu* MarbleWidgetPopupMenu::Private::createInfoBoxMenu()
{
    QMenu* menu = new QMenu( tr( "&Info Boxes" ) );
    QList<AbstractFloatItem *> floatItemList = m_widget->floatItems();

    QList<AbstractFloatItem *>::const_iterator iter = floatItemList.constBegin();
    QList<AbstractFloatItem *>::const_iterator const end = floatItemList.constEnd();
    for (; iter != end; ++iter )
    {
        menu->addAction( (*iter)->action() );
    }
    return menu;
}

void MarbleWidgetPopupMenu::showLmbMenu( int xpos, int ypos )
{
    bool const smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    if ( smallScreen ) {
        showRmbMenu( xpos, ypos );
        return;
    }

    d->m_mousePosition.setX(xpos);
    d->m_mousePosition.setY(ypos);

    const QPoint curpos = QPoint( xpos, ypos );
    d->m_featurelist = d->m_widget->whichFeatureAt( curpos );

    int  actionidx = 1;

    QVector<const GeoDataPlacemark*>::const_iterator it = d->m_featurelist.constBegin();
    QVector<const GeoDataPlacemark*>::const_iterator const itEnd = d->m_featurelist.constEnd();
    for (; it != itEnd; ++it )
    {
        QString name = (*it)->name();
        QPixmap icon = QPixmap::fromImage( ( *it)->style()->iconStyle().icon() );
        d->m_infoDialogAction->setData( actionidx );
        d->m_infoDialogAction->setText( name );
        d->m_infoDialogAction->setIcon( icon );
        // Insert as first action in the menu
        QAction *firstAction = 0;
        if( !d->m_lmbMenu.actions().isEmpty() ) {
            firstAction = d->m_lmbMenu.actions().first();
        }
        d->m_lmbMenu.insertAction( firstAction, d->m_infoDialogAction );
        actionidx++;
    }

    d->m_itemList = d->m_widget->whichItemAt( curpos );
    QList<AbstractDataPluginItem *>::const_iterator itW = d->m_itemList.constBegin();
    QList<AbstractDataPluginItem *>::const_iterator const itWEnd = d->m_itemList.constEnd();
    for (; itW != itWEnd; ++itW )
    {
        foreach ( QAction* action, (*itW)->actions() ) {
            d->m_lmbMenu.addAction( action );
        }
    }

    switch ( d->m_lmbMenu.actions().size() ) {
    case 0: // nothing to do, ignore
        break;

    case 1: // one action? perform immediately
        d->m_lmbMenu.actions().first()->activate( QAction::Trigger );
        d->m_lmbMenu.clear();
        break;

    default:
        d->m_lmbMenu.popup( d->m_widget->mapToGlobal( curpos ) );
    }
}


void MarbleWidgetPopupMenu::showRmbMenu( int xpos, int ypos )
{
    qreal lon, lat;
    const bool visible = d->m_widget->geoCoordinates( xpos, ypos, lon, lat, GeoDataCoordinates::Radian );
    if ( !visible )
        return;

    d->m_mousePosition.setX(xpos);
    d->m_mousePosition.setY(ypos);

    QPoint curpos = QPoint( xpos, ypos );
    d->m_copyCoordinateAction->setData( curpos );

    bool const showDirectionButtons = d->m_widget->routingLayer() && d->m_widget->routingLayer()->isInteractive();
    d->m_directionsFromHereAction->setVisible( showDirectionButtons );
    d->m_directionsToHereAction->setVisible( showDirectionButtons );
    RouteRequest* request = d->m_widget->model()->routingManager()->routeRequest();
    if ( request ) {
        int const lastIndex = qMax( 1, request->size()-1 );
        d->m_directionsToHereAction->setIcon( QIcon( request->pixmap( lastIndex, 16 ) ) );
    }

    d->m_rmbMenu.popup( d->m_widget->mapToGlobal( curpos ) );
}

void MarbleWidgetPopupMenu::resetMenu()
{
    d->m_lmbMenu.clear();
}

void MarbleWidgetPopupMenu::slotInfoDialog()
{
    QAction *action = qobject_cast<QAction *>( sender() );
    if ( action == 0 ) {
        mDebug() << "Warning: slotInfoDialog should be called by a QAction signal";
        return;
    }

    int actionidx = action->data().toInt();

    if ( actionidx > 0 ) {
        const GeoDataPlacemark *index = d->m_featurelist.at( actionidx -1 );
        PopupLayer* popup = d->m_widget->popupLayer();
        popup->setPlacemark(index);
        popup->popup();
    }
}

void MarbleWidgetPopupMenu::slotCopyCoordinates()
{
    const GeoDataCoordinates coordinates = d->mouseCoordinates( d->m_copyCoordinateAction );
    if ( coordinates.isValid() ) {
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
    QPointer<MarbleAboutDialog> dialog = new MarbleAboutDialog( d->m_widget );
    dialog->exec();
    delete dialog;
}

void MarbleWidgetPopupMenu::addAction( Qt::MouseButton button, QAction* action )
{
    if ( button == Qt::RightButton ) {
        d->m_rmbMenu.insertAction( d->m_rmbExtensionPoint, action );
    } else {
        d->m_lmbMenu.addAction( action );
    }
}

void MarbleWidgetPopupMenu::directionsFromHere()
{
    RouteRequest* request = d->m_widget->model()->routingManager()->routeRequest();
    if ( request )
    {
        const GeoDataCoordinates coordinates = d->mouseCoordinates( d->m_copyCoordinateAction );
        if ( coordinates.isValid() ) {
            if ( request->size() > 0 ) {
                request->setPosition( 0, coordinates );
            } else {
                request->append( coordinates );
            }
            d->m_widget->model()->routingManager()->retrieveRoute();
        }
    }
}

void MarbleWidgetPopupMenu::directionsToHere()
{
    RouteRequest* request = d->m_widget->model()->routingManager()->routeRequest();
    if ( request )
    {
        const GeoDataCoordinates coordinates = d->mouseCoordinates( d->m_copyCoordinateAction );
        if ( coordinates.isValid() ) {
            if ( request->size() > 1 ) {
                request->setPosition( request->size()-1, coordinates );
            } else {
                request->append( coordinates );
            }
            d->m_widget->model()->routingManager()->retrieveRoute();
        }
    }
}

GeoDataCoordinates MarbleWidgetPopupMenu::Private::mouseCoordinates( QAction* dataContainer ) const
{
    if ( !dataContainer ) {
        return GeoDataCoordinates();
    }

    if ( !m_featurelist.isEmpty() ) {
        return m_featurelist.first()->coordinate( m_model->clock()->dateTime() );
    } else {
        QPoint p = dataContainer->data().toPoint();
        qreal lat( 0.0 ), lon( 0.0 );

        const bool valid = m_widget->geoCoordinates( p.x(), p.y(), lon, lat, GeoDataCoordinates::Radian );
        if ( valid ) {
            return GeoDataCoordinates( lon, lat );
        }
    }

    return GeoDataCoordinates();
}

void MarbleWidgetPopupMenu::startReverseGeocoding()
{
    const GeoDataCoordinates coordinates = d->mouseCoordinates( d->m_copyCoordinateAction );
    if ( coordinates.isValid() ) {
        d->m_runnerManager.reverseGeocoding( coordinates );
    }
}

void MarbleWidgetPopupMenu::showAddressInformation(const GeoDataCoordinates &, const GeoDataPlacemark &placemark)
{
    QString text = placemark.address();
    if ( !text.isEmpty() ) {
        QMessageBox::information( d->m_widget, tr( "Address Details" ), text, QMessageBox::Ok );
    }
}

void MarbleWidgetPopupMenu::addBookmark()
{
    const GeoDataCoordinates coordinates = d->mouseCoordinates( d->m_copyCoordinateAction );
    if ( coordinates.isValid() ) {
        QPointer<EditBookmarkDialog> dialog = new EditBookmarkDialog( d->m_widget->model()->bookmarkManager(), d->m_widget );
        dialog->setMarbleWidget( d->m_widget );
        dialog->setCoordinates( coordinates );
        dialog->setRange( d->m_widget->lookAt().range() );
        dialog->setReverseGeocodeName();
        if ( dialog->exec() == QDialog::Accepted ) {
            d->m_widget->model()->bookmarkManager()->addBookmark( dialog->folder(), dialog->bookmark() );
        }
        delete dialog;
    }
}

void MarbleWidgetPopupMenu::toggleFullscreen( bool enabled )
{
    QWidget* parent = d->m_widget;
    for ( ; parent->parentWidget(); parent = parent->parentWidget() ) {
        // nothing to do
    }

    if ( enabled ) {
        parent->setWindowState( parent->windowState() | Qt::WindowFullScreen );
    } else {
        parent->setWindowState( parent->windowState() & ~Qt::WindowFullScreen );
    }
}

QPoint MarbleWidgetPopupMenu::mousePosition() const
{
    return d->m_mousePosition;
}

}

#include "MarbleWidgetPopupMenu.moc"
