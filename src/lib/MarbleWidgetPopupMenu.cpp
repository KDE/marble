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
#include "AbstractFloatItem.h"
#include "MapInfoDialog.h"
#include "MarbleAboutDialog.h"
#include "MarbleWidget.h"
#include "MarbleModel.h"
#include "GeoDataPlacemark.h"
#include "GeoDataStyle.h"
#include "GeoDataExtendedData.h"
#include "PlacemarkInfoDialog.h"
#include "MarbleClock.h"
#include "MarbleDebug.h"
#include "Planet.h"
#include "routing/RoutingManager.h"
#include "routing/RouteRequest.h"
#include "MarbleRunnerManager.h"
#include "EditBookmarkDialog.h"
#include "BookmarkManager.h"
#include "MarbleDirs.h"

// Qt
#include <QtCore/QMimeData>
#include <QtCore/QPointer>
#include <QtGui/QAction>
#include <QtGui/QClipboard>
#include <QtGui/QMenu>
#include <QtGui/QMessageBox>

using namespace Marble;
/* TRANSLATOR Marble::MarbleWidgetPopupMenu */

MarbleWidgetPopupMenu::MarbleWidgetPopupMenu(MarbleWidget *widget,
                                         const MarbleModel *model)
    : QObject(widget),
      m_model(model),
      m_widget(widget),
      m_lmbMenu( new QMenu( m_widget ) ),
      m_rmbMenu( new QMenu( m_widget ) ),
      m_copyCoordinateAction( new QAction( QIcon(":/icons/copy-coordinates.png"), tr("Copy Coordinates"), this ) ),
      m_rmbExtensionPoint( 0 ),
      m_runnerManager( new MarbleRunnerManager( model->pluginManager(), this ) )
{
    // Property actions (Left mouse button)
    m_infoDialogAction = new QAction( this );
    m_infoDialogAction->setData( 0 );

    m_showOrbitAction = new QAction( tr( "Display orbit" ), this );
    m_showOrbitAction->setCheckable( true );
    m_showOrbitAction->setData( 0 );

    m_trackPlacemarkAction = new QAction( tr( "Keep centered" ), this );
    m_trackPlacemarkAction->setData( 0 );

    //	Tool actions (Right mouse button)
    QAction* fromHere = new QAction( QIcon(":/icons/directions-from.png"), tr( "Directions &from here" ), this );
    QAction* toHere = new QAction( tr( "Directions &to here" ), this );
    QAction* addBookmark = new QAction( QIcon(":/icons/bookmark-new.png"),
                                        tr( "Add &Bookmark" ), this );
    QAction* fullscreenAction = new QAction( tr( "&Full Screen Mode" ), this );
    fullscreenAction->setCheckable( true );

    QAction* aboutDialogAction = new QAction( QIcon(":/icons/marble.png"), tr( "&About" ), this );

    QMenu* infoBoxMenu = createInfoBoxMenu();

    const bool smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;

    if ( !smallScreen ) {
        m_rmbExtensionPoint = m_rmbMenu->addSeparator();
    }

    m_rmbMenu->addAction( fromHere );
    m_rmbMenu->addAction( toHere );
    m_rmbMenu->addSeparator();
    m_rmbMenu->addAction( addBookmark );
    if ( !smallScreen ) {
        m_rmbMenu->addAction( m_copyCoordinateAction );
    }
    m_rmbMenu->addAction( QIcon(":/icons/addressbook-details.png"), tr( "&Address Details" ), this, SLOT( startReverseGeocoding() ) );
    m_rmbMenu->addSeparator();
    m_rmbMenu->addMenu( infoBoxMenu );

    if ( !smallScreen ) {
        m_rmbMenu->addAction( aboutDialogAction );
    } else {
        m_rmbMenu->addAction( fullscreenAction );
    }

    connect( fromHere, SIGNAL( triggered( ) ), SLOT( directionsFromHere() ) );
    connect( toHere, SIGNAL( triggered( ) ), SLOT( directionsToHere() ) );
    connect( addBookmark, SIGNAL( triggered( ) ), SLOT( addBookmark() ) );
    connect( aboutDialogAction, SIGNAL( triggered() ), SLOT( slotAboutDialog() ) );
    connect( m_copyCoordinateAction, SIGNAL( triggered() ), SLOT( slotCopyCoordinates() ) );
    connect( m_infoDialogAction, SIGNAL( triggered() ), SLOT( slotInfoDialog() ) );
    connect( m_showOrbitAction, SIGNAL( triggered(bool) ), SLOT( slotShowOrbit(bool) ) );
    connect( m_trackPlacemarkAction, SIGNAL( triggered(bool) ), SLOT( slotTrackPlacemark() ) );
    connect( fullscreenAction, SIGNAL( triggered( bool ) ), this, SLOT( toggleFullscreen( bool ) ) );

    connect( m_runnerManager, SIGNAL( reverseGeocodingFinished( GeoDataCoordinates, GeoDataPlacemark ) ),
             this, SLOT( showAddressInformation( GeoDataCoordinates, GeoDataPlacemark ) ) );
}

QMenu* MarbleWidgetPopupMenu::createInfoBoxMenu()
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

    if (!m_lmbMenu->isEmpty()) {
        m_lmbMenu->clear();
        // just clear()-ing the menu won't delete the submenus
        foreach( QObject *child, m_lmbMenu->children() ) {
            child->deleteLater();
        }
    }

    m_mousePosition.setX(xpos);
    m_mousePosition.setY(ypos);

    const QPoint curpos = QPoint( xpos, ypos );
    m_featurelist = m_widget->whichFeatureAt( curpos );

    int  actionidx = 1;
    bool hasSatellites = false;

    QVector<const GeoDataPlacemark*>::const_iterator it = m_featurelist.constBegin();
    QVector<const GeoDataPlacemark*>::const_iterator const itEnd = m_featurelist.constEnd();
    for (; it != itEnd; ++it )
    {
        QString name = (*it)->name();
        QPixmap icon = QPixmap::fromImage( ( *it)->style()->iconStyle().icon() );
        m_infoDialogAction->setData( actionidx );

        //TODO: don't hardcode this check ?
        if ( (*it)->visualCategory() != GeoDataFeature::Satellite ) {
            m_infoDialogAction->setText( name );
            m_infoDialogAction->setIcon( icon );

            m_lmbMenu->addAction( m_infoDialogAction );
        } else {
	    hasSatellites = true;
            QMenu *subMenu = new QMenu( name, m_lmbMenu );
            subMenu->setIcon( icon );
            m_infoDialogAction->setText( tr( "Satellite information" ) );
            m_infoDialogAction->setIcon( QIcon() );
            subMenu->addAction( m_infoDialogAction );

            m_showOrbitAction->setChecked( (*it)->style()->lineStyle().penStyle() != Qt::NoPen );
            m_showOrbitAction->setData( actionidx );
            subMenu->addAction( m_showOrbitAction );

            m_trackPlacemarkAction->setData( actionidx );
            subMenu->addAction( m_trackPlacemarkAction );

            m_lmbMenu->addAction( subMenu->menuAction() );
        }
        actionidx++;
    }

    m_itemList = m_widget->whichItemAt( curpos );
    QList<AbstractDataPluginItem *>::const_iterator itW = m_itemList.constBegin();
    QList<AbstractDataPluginItem *>::const_iterator const itWEnd = m_itemList.constEnd();
    for (; itW != itWEnd; ++itW )
    {
        foreach ( QAction* action, (*itW)->actions() ) {
            m_lmbMenu->addAction( action );
        }
    }

    switch ( m_lmbMenu->actions().size() ) {
    case 0: break; // nothing to do, ignore
    case 1: if ( ! hasSatellites ) {
	      m_lmbMenu->actions().first()->activate( QAction::Trigger );
	      break; // one action? perform immediately
	    }
    default: m_lmbMenu->popup( m_widget->mapToGlobal( curpos ) );
    }
}


void MarbleWidgetPopupMenu::showRmbMenu( int xpos, int ypos )
{
    qreal lon, lat;
    const bool visible = m_widget->geoCoordinates( xpos, ypos, lon, lat, GeoDataCoordinates::Radian );
    if ( !visible )
        return;

    m_mousePosition.setX(xpos);
    m_mousePosition.setY(ypos);

    QPoint curpos = QPoint( xpos, ypos );
    m_copyCoordinateAction->setData( curpos );

    m_rmbMenu->popup( m_widget->mapToGlobal( curpos ) );
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
        const GeoDataPlacemark *index = m_featurelist.at( actionidx -1 );
        bool isSatellite = (index->visualCategory() == GeoDataFeature::Satellite);
        bool isCity (index->visualCategory() >= GeoDataFeature::SmallCity &&
                         index->visualCategory() <= GeoDataFeature::LargeNationCapital);
        bool isGeoPlace = (index->visualCategory() == GeoDataFeature::GeographicPole ||
                           index->visualCategory() == GeoDataFeature::MagneticPole ||
                           index->visualCategory() == GeoDataFeature::ShipWreck);
        bool isNation = (index->visualCategory() == GeoDataFeature::Nation);

        if (index->role().isEmpty() || isSatellite || isCity || isGeoPlace || isNation) {
            MapInfoDialog* popup = m_widget->mapInfoDialog();
            popup->setSize(QSizeF(580, 620));
            if (isSatellite) {
                setupDialogSatellite(popup, index);
            } else if (isCity) {
                setupDialogCity(popup, index);
            } else if (isNation) {
                setupDialogNation(popup, index);
            } else if (isGeoPlace) {
                setupDialogGeoPlaces(popup, index);
            } else {
                popup->setContent(index->description());
            }

            popup->setVisible(true);
        } else {
            QPointer<PlacemarkInfoDialog> dialog = new PlacemarkInfoDialog( index, m_widget->model()->clock(),  m_widget );
            dialog->setWindowModality( Qt::WindowModal );
            dialog->exec();
            delete dialog;
        }
    }
}

void MarbleWidgetPopupMenu::setupDialogSatellite(MapInfoDialog *popup, const GeoDataPlacemark *index)
{
    GeoDataCoordinates location = index->coordinate(m_model->clockDateTime());
    popup->setCoordinates(location, Qt::AlignRight | Qt::AlignVCenter);

    QString description = index->description();
    description.replace("%altitude%", QString::number(location.altitude(), 'f', 2));
    description.replace("%latitude%", location.latToString());
    description.replace("%longitude%", location.lonToString());
    popup->setContent(description);
}

void MarbleWidgetPopupMenu::setupDialogCity(MapInfoDialog *popup, const GeoDataPlacemark *index)
{
    GeoDataCoordinates location = index->coordinate(m_model->clockDateTime());
    popup->setCoordinates(location, Qt::AlignRight | Qt::AlignVCenter);

    QFile descriptionFile(":/marble/webpopup/city.html");
    if (!descriptionFile.open(QIODevice::ReadOnly)) {
        return;
    }

    QString description = descriptionFile.readAll();
    description.replace("%name%", index->name());

    QString  roleString;
    const QString role = index->role();
    if(role=="PPLC") {
        roleString = tr("National Capital");
    } else if(role=="PPL") {
        roleString = tr("City");
    } else if(role=="PPLA") {
        roleString = tr("State Capital");
    } else if(role=="PPLA2") {
        roleString = tr("County Capital");
    } else if(role=="PPLA3" || role=="PPLA4" ) {
        roleString = tr("Capital");
    } else if(role=="PPLF" || role=="PPLG" || role=="PPLL" || role=="PPLQ" ||
              role=="PPLR" || role=="PPLS" || role=="PPLW" ) {
        roleString = tr("Village");
    }
    description.replace("%category%", roleString);

    description.replace("%shortDescription%", index->description());
    description.replace("%latitude%", location.latToString());
    description.replace("%longitude%", location.lonToString());
    description.replace("%elevation%", QString::number(location.altitude(), 'f', 2));
    description.replace("%population%", QString::number(index->population()));
    description.replace("%country%", index->countryCode());
    description.replace("%state%", index->state());

    QString dst = QString( "%1" ).arg( ( index->extendedData().value("gmt").value().toInt() +
                                         index->extendedData().value("dst").value().toInt() ) /
                                       ( double ) 100, 0, 'f', 1 );
    // There is an issue about UTC.
    // It's possible to variants (e.g.):
    // +1.0 and -1.0, but dst does not have + an the start
    if(dst.startsWith("-")) {
        description.replace("%timezone%", dst);
    } else {
        description.replace("%timezone%", "+"+dst);
    }

    const QString flagPath = MarbleDirs::path(QString("flags/flag_%1.svg").arg(index->countryCode().toLower()) );
    description.replace("%flag%", flagPath);

    popup->setContent(description);
}

void MarbleWidgetPopupMenu::setupDialogNation(MapInfoDialog *popup, const GeoDataPlacemark *index)
{
    GeoDataCoordinates location = index->coordinate(m_model->clockDateTime());
    popup->setCoordinates(location, Qt::AlignRight | Qt::AlignVCenter);

    QFile descriptionFile(":/marble/webpopup/nation.html");
    if (!descriptionFile.open(QIODevice::ReadOnly)) {
        return;
    }

    QString description = descriptionFile.readAll();
    description.replace("%name%", index->name());
    description.replace("%shortDescription%", index->description());
    description.replace("%latitude%", location.latToString());
    description.replace("%longitude%", location.lonToString());
    description.replace("%elevation%", QString::number(location.altitude(), 'f', 2));
    description.replace("%population%", QString::number(index->population()));
    description.replace("%area%", QString::number(index->area(), 'f', 2));

    const QString flagPath = MarbleDirs::path(QString("flags/flag_%1.svg").arg(index->countryCode().toLower()) );
    description.replace("%flag%", flagPath);

    popup->setContent(description);
}

void MarbleWidgetPopupMenu::setupDialogGeoPlaces(MapInfoDialog *popup, const GeoDataPlacemark *index)
{
    GeoDataCoordinates location = index->coordinate(m_model->clockDateTime());
    popup->setCoordinates(location, Qt::AlignRight | Qt::AlignVCenter);

    QFile descriptionFile(":/marble/webpopup/geoplace.html");
    if (!descriptionFile.open(QIODevice::ReadOnly)) {
        return;
    }

    QString description = descriptionFile.readAll();
    description.replace("%name%", index->name());
    description.replace("%latitude%", location.latToString());
    description.replace("%longitude%", location.lonToString());
    description.replace("%elevation%", QString::number(location.altitude(), 'f', 2));
    description.replace("%shortDescription%", index->description());

    popup->setContent(description);
}

void MarbleWidgetPopupMenu::slotShowOrbit( bool show )
{
    QAction *action = qobject_cast<QAction *>( sender() );
    if ( action == 0 ) {
        mDebug() << "Warning: slotShowOrbit should be called by a QAction signal";
        return;
    }

    int actionidx = action->data().toInt();

    if ( actionidx > 0 ) {
        const GeoDataPlacemark *index = m_featurelist.at( actionidx -1 );

        Qt::PenStyle penStyle = show ? Qt::SolidLine : Qt::NoPen;
        index->style()->lineStyle().setPenStyle( penStyle );
    }
}

void MarbleWidgetPopupMenu::slotTrackPlacemark()
{
    QAction *action = qobject_cast<QAction *>( sender() );
    if ( action == 0 ) {
        mDebug() << "Warning: slotTrackPlacemark should be called by a QAction signal";
        return;
    }

    int actionidx = action->data().toInt();

    if ( actionidx > 0 ) {
        mDebug() << actionidx;
        const GeoDataPlacemark *index = m_featurelist.at( actionidx -1 );
        emit trackPlacemark( index );
    }
}

void MarbleWidgetPopupMenu::slotCopyCoordinates()
{
    GeoDataCoordinates coordinates;
    if ( mouseCoordinates( &coordinates, m_copyCoordinateAction ) ) {
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
        if ( mouseCoordinates( &coordinates, m_copyCoordinateAction ) ) {
            if ( request->size() > 0 ) {
                request->setPosition( 0, coordinates );
            } else {
                request->append( coordinates );
            }
            m_widget->model()->routingManager()->retrieveRoute();
        }
    }
}

void MarbleWidgetPopupMenu::directionsToHere()
{
    RouteRequest* request = m_widget->model()->routingManager()->routeRequest();
    if ( request )
    {
        GeoDataCoordinates coordinates;
        if ( mouseCoordinates( &coordinates, m_copyCoordinateAction ) ) {
            if ( request->size() > 1 ) {
                request->setPosition( request->size()-1, coordinates );
            } else {
                request->append( coordinates );
            }
            m_widget->model()->routingManager()->retrieveRoute();
        }
    }
}

bool MarbleWidgetPopupMenu::mouseCoordinates( GeoDataCoordinates* coordinates, QAction* dataContainer )
{
    Q_ASSERT( coordinates && "You must not pass 0 as coordinates parameter");
    if ( !dataContainer ) {
        return false;
    }

    bool valid = true;
    if ( !m_featurelist.isEmpty() ) {
        *coordinates = m_featurelist.first()->coordinate( m_model->clock()->dateTime() );
    } else {
        QPoint p = dataContainer->data().toPoint();
        qreal lat( 0.0 ), lon( 0.0 );

        valid = m_widget->geoCoordinates( p.x(), p.y(), lon, lat, GeoDataCoordinates::Radian );
        if ( valid ) {
            *coordinates = GeoDataCoordinates( lon, lat );
        }
    }

    return valid;
}

void MarbleWidgetPopupMenu::startReverseGeocoding()
{
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
    if ( mouseCoordinates( &coordinates, m_copyCoordinateAction ) ) {
        QPointer<EditBookmarkDialog> dialog = new EditBookmarkDialog( m_widget->model()->bookmarkManager(), m_widget );
        dialog->setMarbleWidget( m_widget );
        dialog->setCoordinates( coordinates );
        dialog->setRange( m_widget->lookAt().range() );
        dialog->setReverseGeocodeName();
        if ( dialog->exec() == QDialog::Accepted ) {
            m_widget->model()->bookmarkManager()->addBookmark( dialog->folder(), dialog->bookmark() );
        }
        delete dialog;
    }
}

void MarbleWidgetPopupMenu::toggleFullscreen( bool enabled )
{
    QWidget* parent = m_widget;
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
    return m_mousePosition;
}

#include "MarbleWidgetPopupMenu.moc"
