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
#include "GeoDataExtendedData.h"
#include "GeoSceneDocument.h"
#include "GeoSceneHead.h"
#include "MarbleClock.h"
#include "MarbleDebug.h"
#include "PopupLayer.h"
#include "Planet.h"
#include "routing/RoutingManager.h"
#include "routing/RouteRequest.h"
#include "EditBookmarkDialog.h"
#include "BookmarkManager.h"
#include "MarbleDirs.h"
#include "ReverseGeocodingRunnerManager.h"
#include "TemplateDocument.h"

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
    MarbleWidget   *m_widget;

    QVector<const GeoDataPlacemark*>  m_featurelist;
    QList<AbstractDataPluginItem *> m_itemList;

    QMenu    *const m_lmbMenu;
    QMenu    *const m_rmbMenu;

    QAction *m_infoDialogAction;
    QAction *m_showOrbitAction;
    QAction *m_trackPlacemarkAction;
    QAction *m_directionsToHereAction;

    QAction  *const m_copyCoordinateAction;

    QAction  *m_rmbExtensionPoint;

    ReverseGeocodingRunnerManager *const m_runnerManager;

    QPoint m_mousePosition;

public:
    Private( MarbleWidget *widget, const MarbleModel *model, MarbleWidgetPopupMenu* parent );
    QMenu* createInfoBoxMenu();
    QString filterEmptyShortDescription(const QString &description) const;
    void setupDialogSatellite(PopupLayer *popup, const GeoDataPlacemark *index);
    void setupDialogCity(PopupLayer *popup, const GeoDataPlacemark *index);
    void setupDialogNation(PopupLayer *popup, const GeoDataPlacemark *index);
    void setupDialogGeoPlaces(PopupLayer *popup, const GeoDataPlacemark *index);
    void setupDialogSkyPlaces(PopupLayer *popup, const GeoDataPlacemark *index);

    /**
      * Returns the geo coordinates of the mouse pointer at the last right button menu.
      * You must not pass 0 as coordinates parameter. The result indicates whether the
      * coordinates are valid, which will be true if the right button menu was opened at least once.
      */
    bool mouseCoordinates( GeoDataCoordinates* coordinates, QAction* dataContainer );
};

MarbleWidgetPopupMenu::Private::Private( MarbleWidget *widget, const MarbleModel *model, MarbleWidgetPopupMenu* parent ) :
    m_model(model),
    m_widget(widget),
    m_lmbMenu( new QMenu( m_widget ) ),
    m_rmbMenu( new QMenu( m_widget ) ),
    m_directionsToHereAction( 0 ),
    m_copyCoordinateAction( new QAction( QIcon(":/icons/copy-coordinates.png"), tr("Copy Coordinates"), parent ) ),
    m_rmbExtensionPoint( 0 ),
    m_runnerManager( new ReverseGeocodingRunnerManager( model, parent ) )
{
    // Property actions (Left mouse button)
    m_infoDialogAction = new QAction( parent );
    m_infoDialogAction->setData( 0 );

    m_showOrbitAction = new QAction( tr( "Display orbit" ), parent );
    m_showOrbitAction->setCheckable( true );
    m_showOrbitAction->setData( 0 );

    m_trackPlacemarkAction = new QAction( tr( "Keep centered" ), parent );
    m_trackPlacemarkAction->setData( 0 );

    //	Tool actions (Right mouse button)
    QAction* fromHere = new QAction( tr( "Directions &from here" ), parent );
    m_directionsToHereAction = new QAction( tr( "Directions &to here" ), parent );
    RouteRequest* request = m_widget->model()->routingManager()->routeRequest();
    if ( request ) {
        fromHere->setIcon( QIcon( request->pixmap( 0, 16 ) ) );
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
        m_rmbExtensionPoint = m_rmbMenu->addSeparator();
    }

    m_rmbMenu->addAction( fromHere );
    m_rmbMenu->addAction( m_directionsToHereAction );
    m_rmbMenu->addSeparator();
    m_rmbMenu->addAction( addBookmark );
    if ( !smallScreen ) {
        m_rmbMenu->addAction( m_copyCoordinateAction );
    }
    m_rmbMenu->addAction( QIcon(":/icons/addressbook-details.png"), tr( "&Address Details" ), parent, SLOT(startReverseGeocoding()) );
    m_rmbMenu->addSeparator();
    m_rmbMenu->addMenu( infoBoxMenu );

    if ( !smallScreen ) {
        m_rmbMenu->addAction( aboutDialogAction );
    } else {
        m_rmbMenu->addAction( fullscreenAction );
    }

    parent->connect( fromHere, SIGNAL(triggered()), SLOT(directionsFromHere()) );
    parent->connect( m_directionsToHereAction, SIGNAL(triggered()), SLOT(directionsToHere()) );
    parent->connect( addBookmark, SIGNAL(triggered()), SLOT(addBookmark()) );
    parent->connect( aboutDialogAction, SIGNAL(triggered()), SLOT(slotAboutDialog()) );
    parent->connect( m_copyCoordinateAction, SIGNAL(triggered()), SLOT(slotCopyCoordinates()) );
    parent->connect( m_infoDialogAction, SIGNAL(triggered()), SLOT(slotInfoDialog()) );
    parent->connect( m_showOrbitAction, SIGNAL(triggered(bool)), SLOT(slotShowOrbit(bool)) );
    parent->connect( m_trackPlacemarkAction, SIGNAL(triggered(bool)), SLOT(slotTrackPlacemark()) );
    parent->connect( fullscreenAction, SIGNAL(triggered(bool)), parent, SLOT(toggleFullscreen(bool)) );

    parent->connect( m_runnerManager, SIGNAL(reverseGeocodingFinished(GeoDataCoordinates,GeoDataPlacemark)),
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

    if (!d->m_lmbMenu->isEmpty()) {
        d->m_lmbMenu->clear();
        // just clear()-ing the menu won't delete the submenus
        foreach( QObject *child, d->m_lmbMenu->children() ) {
            child->deleteLater();
        }
    }

    d->m_mousePosition.setX(xpos);
    d->m_mousePosition.setY(ypos);

    const QPoint curpos = QPoint( xpos, ypos );
    d->m_featurelist = d->m_widget->whichFeatureAt( curpos );

    int  actionidx = 1;
    bool hasSatellites = false;

    QVector<const GeoDataPlacemark*>::const_iterator it = d->m_featurelist.constBegin();
    QVector<const GeoDataPlacemark*>::const_iterator const itEnd = d->m_featurelist.constEnd();
    for (; it != itEnd; ++it )
    {
        QString name = (*it)->name();
        QPixmap icon = QPixmap::fromImage( ( *it)->style()->iconStyle().icon() );
        d->m_infoDialogAction->setData( actionidx );

        //TODO: don't hardcode this check ?
        if ( (*it)->visualCategory() != GeoDataFeature::Satellite ) {
            d->m_infoDialogAction->setText( name );
            d->m_infoDialogAction->setIcon( icon );

            d->m_lmbMenu->addAction( d->m_infoDialogAction );
        } else {
	    hasSatellites = true;
            QMenu *subMenu = new QMenu( name, d->m_lmbMenu );
            subMenu->setIcon( icon );
            d->m_infoDialogAction->setText( tr( "Satellite information" ) );
            d->m_infoDialogAction->setIcon( QIcon() );
            subMenu->addAction( d->m_infoDialogAction );

            d->m_showOrbitAction->setChecked( (*it)->style()->lineStyle().penStyle() != Qt::NoPen );
            d->m_showOrbitAction->setData( actionidx );
            subMenu->addAction( d->m_showOrbitAction );

            d->m_trackPlacemarkAction->setData( actionidx );
            subMenu->addAction( d->m_trackPlacemarkAction );

            d->m_lmbMenu->addAction( subMenu->menuAction() );
        }
        actionidx++;
    }

    d->m_itemList = d->m_widget->whichItemAt( curpos );
    QList<AbstractDataPluginItem *>::const_iterator itW = d->m_itemList.constBegin();
    QList<AbstractDataPluginItem *>::const_iterator const itWEnd = d->m_itemList.constEnd();
    for (; itW != itWEnd; ++itW )
    {
        foreach ( QAction* action, (*itW)->actions() ) {
            d->m_lmbMenu->addAction( action );
        }
    }

    switch ( d->m_lmbMenu->actions().size() ) {
    case 0: break; // nothing to do, ignore
    case 1: if ( ! hasSatellites ) {
          d->m_lmbMenu->actions().first()->activate( QAction::Trigger );
	      break; // one action? perform immediately
	    }
    default: d->m_lmbMenu->popup( d->m_widget->mapToGlobal( curpos ) );
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
    RouteRequest* request = d->m_widget->model()->routingManager()->routeRequest();
    if ( request ) {
        int const lastIndex = qMax( 1, request->size()-1 );
        d->m_directionsToHereAction->setIcon( QIcon( request->pixmap( lastIndex, 16 ) ) );
    }

    d->m_rmbMenu->popup( d->m_widget->mapToGlobal( curpos ) );
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
        bool isSatellite = (index->visualCategory() == GeoDataFeature::Satellite);
        bool isCity (index->visualCategory() >= GeoDataFeature::SmallCity &&
                         index->visualCategory() <= GeoDataFeature::LargeNationCapital);
        bool isNation = (index->visualCategory() == GeoDataFeature::Nation);
        bool isSky = false;
        if ( d->m_model->mapTheme() ) {
            isSky = d->m_model->mapTheme()->head()->target() == "sky";
        }
        PopupLayer* popup = d->m_widget->popupLayer();
        popup->setSize(QSizeF(580, 620));
        if (index->role().isEmpty() || isSatellite || isCity || isNation || isSky) {
            if (isSatellite) {
                d->setupDialogSatellite(popup, index);
            } else if (isCity) {
                d->setupDialogCity(popup, index);
            } else if (isNation) {
                d->setupDialogNation(popup, index);
            } else if (isSky) {
                d->setupDialogSkyPlaces(popup, index);
            } else {
                popup->setContent(index->description());
            }
        } else {
            d->setupDialogGeoPlaces(popup, index);
        }
        popup->popup();
    }
}

QString MarbleWidgetPopupMenu::Private::filterEmptyShortDescription(const QString &description) const
{
    if(description.isEmpty())
        return tr("No description available.");
    return description;
}

void MarbleWidgetPopupMenu::Private::setupDialogSatellite(PopupLayer *popup, const GeoDataPlacemark *index)
{
    GeoDataCoordinates location = index->coordinate(m_model->clockDateTime());
    popup->setCoordinates(location, Qt::AlignRight | Qt::AlignVCenter);

    const QString description = index->description();
    TemplateDocument doc(description);
    doc["altitude"] = QString::number(location.altitude(), 'f', 2);
    doc["latitude"] = location.latToString();
    doc["longitude"] = location.lonToString();
    popup->setContent(doc.finalText());
}

void MarbleWidgetPopupMenu::Private::setupDialogCity(PopupLayer *popup, const GeoDataPlacemark *index)
{
    GeoDataCoordinates location = index->coordinate(m_model->clockDateTime());
    popup->setCoordinates(location, Qt::AlignRight | Qt::AlignVCenter);

    QFile descriptionFile(":/marble/webpopup/city.html");
    if (!descriptionFile.open(QIODevice::ReadOnly)) {
        return;
    }

    const QString description = descriptionFile.readAll();
    TemplateDocument doc(description);

    doc["name"] = index->name();
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

    doc["category"] = roleString;
    doc["shortDescription"] = filterEmptyShortDescription(index->description());
    doc["latitude"] = location.latToString();
    doc["longitude"] = location.lonToString();
    doc["elevation"] =  QString::number(location.altitude(), 'f', 2);
    doc["population"] = QString::number(index->population());
    doc["country"] = index->countryCode();
    doc["state"] = index->state();

    QString dst = QString( "%1" ).arg( ( index->extendedData().value("gmt").value().toInt() +
                                         index->extendedData().value("dst").value().toInt() ) /
                                       ( double ) 100, 0, 'f', 1 );
    // There is an issue about UTC.
    // It's possible to variants (e.g.):
    // +1.0 and -1.0, but dst does not have + an the start
    if(dst.startsWith('-')) {
        doc["timezone"] = dst;
    } else {
        doc["timezone"] = "+"+dst;
    }

    const QString flagPath = MarbleDirs::path(
                QString("flags/flag_%1.svg").arg(index->countryCode().toLower()));
    doc["flag"] = flagPath;

    popup->setContent(doc.finalText());
}

void MarbleWidgetPopupMenu::Private::setupDialogNation(PopupLayer *popup, const GeoDataPlacemark *index)
{
    GeoDataCoordinates location = index->coordinate(m_model->clockDateTime());
    popup->setCoordinates(location, Qt::AlignRight | Qt::AlignVCenter);

    QFile descriptionFile(":/marble/webpopup/nation.html");
    if (!descriptionFile.open(QIODevice::ReadOnly)) {
        return;
    }

    const QString description = descriptionFile.readAll();
    TemplateDocument doc(description);

    doc["name"] = index->name();
    doc["shortDescription"] = filterEmptyShortDescription(index->description());
    doc["latitude"] = location.latToString();
    doc["longitude"] = location.lonToString();
    doc["elevation"] = QString::number(location.altitude(), 'f', 2);
    doc["population"] = QString::number(index->population());
    doc["area"] = QString::number(index->area(), 'f', 2);

    const QString flagPath = MarbleDirs::path(QString("flags/flag_%1.svg").arg(index->countryCode().toLower()) );
    doc["flag"] = flagPath;

    popup->setContent(doc.finalText());
}

void MarbleWidgetPopupMenu::Private::setupDialogGeoPlaces(PopupLayer *popup, const GeoDataPlacemark *index)
{
    GeoDataCoordinates location = index->coordinate(m_model->clockDateTime());
    popup->setCoordinates(location, Qt::AlignRight | Qt::AlignVCenter);

    QFile descriptionFile(":/marble/webpopup/geoplace.html");
    if (!descriptionFile.open(QIODevice::ReadOnly)) {
        return;
    }

    const QString description = descriptionFile.readAll();
    TemplateDocument doc(description);

    doc["name"] = index->name();
    doc["latitude"] = location.latToString();
    doc["longitude"] = location.lonToString();
    doc["elevation"] = QString::number(location.altitude(), 'f', 2);
    doc["shortDescription"] = filterEmptyShortDescription(index->description());

    popup->setContent(doc.finalText());
}

void MarbleWidgetPopupMenu::Private::setupDialogSkyPlaces(PopupLayer *popup, const GeoDataPlacemark *index)
{
    GeoDataCoordinates location = index->coordinate(m_model->clockDateTime());
    popup->setCoordinates(location, Qt::AlignRight | Qt::AlignVCenter);

    QFile descriptionFile(":/marble/webpopup/skyplace.html");
    if (!descriptionFile.open(QIODevice::ReadOnly)) {
        return;
    }

    const QString description = descriptionFile.readAll();
    TemplateDocument doc(description);

    doc["name"] = index->name();
    doc["latitude"] = GeoDataCoordinates::latToString(
                            location.latitude(), GeoDataCoordinates::Astro, GeoDataCoordinates::Radian, -1, 'f');
    doc["longitude"] = GeoDataCoordinates::lonToString(
                            location.longitude(), GeoDataCoordinates::Astro, GeoDataCoordinates::Radian, -1, 'f');
    doc["shortDescription"] = filterEmptyShortDescription(index->description());

    popup->setContent(doc.finalText());
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
        const GeoDataPlacemark *index = d->m_featurelist.at( actionidx -1 );

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
        const GeoDataPlacemark *index = d->m_featurelist.at( actionidx -1 );
        emit trackPlacemark( index );
    }
}

void MarbleWidgetPopupMenu::slotCopyCoordinates()
{
    GeoDataCoordinates coordinates;
    if ( d->mouseCoordinates( &coordinates, d->m_copyCoordinateAction ) ) {
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
    MarbleAboutDialog dlg( d->m_widget );
    dlg.exec();
}

void MarbleWidgetPopupMenu::addAction( Qt::MouseButton button, QAction* action )
{
    if ( button == Qt::RightButton ) {
        d->m_rmbMenu->insertAction( d->m_rmbExtensionPoint, action );
    }
}

void MarbleWidgetPopupMenu::directionsFromHere()
{
    RouteRequest* request = d->m_widget->model()->routingManager()->routeRequest();
    if ( request )
    {
        GeoDataCoordinates coordinates;
        if ( d->mouseCoordinates( &coordinates, d->m_copyCoordinateAction ) ) {
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
        GeoDataCoordinates coordinates;
        if ( d->mouseCoordinates( &coordinates, d->m_copyCoordinateAction ) ) {
            if ( request->size() > 1 ) {
                request->setPosition( request->size()-1, coordinates );
            } else {
                request->append( coordinates );
            }
            d->m_widget->model()->routingManager()->retrieveRoute();
        }
    }
}

bool MarbleWidgetPopupMenu::Private::mouseCoordinates( GeoDataCoordinates* coordinates, QAction* dataContainer )
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
    if ( d->mouseCoordinates( &coordinates, d->m_copyCoordinateAction ) ) {
        d->m_runnerManager->reverseGeocoding( coordinates );
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
    GeoDataCoordinates coordinates;
    if ( d->mouseCoordinates( &coordinates, d->m_copyCoordinateAction ) ) {
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
