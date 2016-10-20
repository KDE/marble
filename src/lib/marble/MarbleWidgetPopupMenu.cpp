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
// Copyright 2014      Gábor Péterffy    <peterffy95@gmail.com>
//

// Self
#include "MarbleWidgetPopupMenu.h"

// Marble
#include "AbstractDataPluginItem.h"
#include "AbstractFloatItem.h"
#include "MarbleAboutDialog.h"
#include "MarbleDirs.h"
#include "MarbleWidget.h"
#include "MarbleModel.h"
#include "GeoDataExtendedData.h"
#include "GeoDataFolder.h"
#include "GeoDataPlacemark.h"
#include "GeoDataLookAt.h"
#include "GeoDataData.h"
#include "GeoDataSnippet.h"
#include "GeoDataStyle.h"
#include "GeoDataBalloonStyle.h"
#include "GeoDataIconStyle.h"
#include "GeoDataPoint.h"
#include "GeoDataTypes.h"
#include "GeoDataPhotoOverlay.h"
#include "GeoSceneDocument.h"
#include "GeoSceneHead.h"
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
#include "TemplateDocument.h"
#include "OsmPlacemarkData.h"
#include "StyleBuilder.h"

// Qt
#include <QApplication>
#include <QFile>
#include <QMimeData>
#include <QPointer>
#include <QAction>
#include <QClipboard>
#include <QMenu>
#include <QMessageBox>

namespace Marble {
/* TRANSLATOR Marble::MarbleWidgetPopupMenu */

class Q_DECL_HIDDEN MarbleWidgetPopupMenu::Private {
public:
    const MarbleModel *const m_model;
    MarbleWidget *const m_widget;

    QVector<const GeoDataFeature*>  m_featurelist;
    QList<AbstractDataPluginItem *> m_itemList;

    QMenu m_lmbMenu;
    QMenu m_rmbMenu;

    QAction *m_infoDialogAction;
    QAction *m_directionsFromHereAction;
    QAction *m_directionsToHereAction;

    QAction *const m_copyCoordinateAction;

    QAction *m_rmbExtensionPoint;

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

    static QString filterEmptyShortDescription( const QString &description );
    void setupDialogOsm( PopupLayer *popup, const GeoDataPlacemark* placemark );
    void setupDialogSatellite( const GeoDataPlacemark *placemark );
    static void setupDialogCity( PopupLayer *popup, const GeoDataPlacemark *placemark );
    static void setupDialogNation( PopupLayer *popup, const GeoDataPlacemark *placemark );
    static void setupDialogGeoPlaces( PopupLayer *popup, const GeoDataPlacemark *placemark );
    static void setupDialogSkyPlaces( PopupLayer *popup, const GeoDataPlacemark *placemark );
    static void setupDialogPhotoOverlay( PopupLayer *popup, const GeoDataPhotoOverlay *overlay);
};

MarbleWidgetPopupMenu::Private::Private( MarbleWidget *widget, const MarbleModel *model, MarbleWidgetPopupMenu* parent ) :
    m_model(model),
    m_widget(widget),
    m_lmbMenu( m_widget ),
    m_rmbMenu( m_widget ),
    m_directionsFromHereAction( 0 ),
    m_directionsToHereAction( 0 ),
    m_copyCoordinateAction(new QAction(QIcon(QStringLiteral(":/icons/copy-coordinates.png")), tr("Copy Coordinates"), parent)),
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
    QAction* addBookmark = new QAction( QIcon(QStringLiteral(":/icons/bookmark-new.png")),
                                        tr( "Add &Bookmark" ), parent );
    QAction* fullscreenAction = new QAction( tr( "&Full Screen Mode" ), parent );
    fullscreenAction->setCheckable( true );

    QAction* aboutDialogAction = new QAction(QIcon(QStringLiteral(":/icons/marble.png")), tr("&About"), parent);

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
    m_rmbMenu.addAction(QIcon(QStringLiteral(":/icons/addressbook-details.png")), tr("&Address Details"), parent, SLOT(startReverseGeocoding()));
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

QString MarbleWidgetPopupMenu::Private::filterEmptyShortDescription(const QString &description)
{
    if(description.isEmpty())
        return tr("No description available.");
    return description;
}

void MarbleWidgetPopupMenu::Private::setupDialogOsm( PopupLayer *popup, const GeoDataPlacemark *placemark )
{
    const GeoDataCoordinates location = placemark->coordinate();
    popup->setCoordinates(location, Qt::AlignRight | Qt::AlignVCenter);

    QFile descriptionFile(QStringLiteral(":/marble/webpopup/osm.html"));
    if (!descriptionFile.open(QIODevice::ReadOnly)) {
        return;
    }

    const QString none = QStringLiteral("none");

    QString description = descriptionFile.readAll();
    const OsmPlacemarkData& data = placemark->osmData();
    if (!data.containsTagKey("addr:street") && !data.containsTagKey("addr:housenumber")){
        description.replace(QStringLiteral("<br> %postcode%"), QStringLiteral("%postcode%"));
    }
    TemplateDocument doc(description);

    doc[QStringLiteral("name")] = data.tagValue(QStringLiteral("name"));

    QString natural = data.tagValue(QStringLiteral("natural"));
    if (!natural.isEmpty()) {
        natural[0] = natural[0].toUpper();
        if (natural == QLatin1String("Peak")) {
            QString elevation = data.tagValue(QStringLiteral("ele"));
            if (!elevation.isEmpty()) {
                natural = natural + QLatin1String(" - ") + elevation + QLatin1String(" m");
            }
        }
        doc[QStringLiteral("details")] = natural;
    } else {
        doc[QStringLiteral("detailsVisibility")] = none;
    }

    QString amenity;
    QString shop = data.tagValue(QStringLiteral("shop"));
    if (!shop.isEmpty()) {
        shop[0] = shop[0].toUpper();

        if (shop == QLatin1String("Clothes")) {
            QString type = data.tagValue(QStringLiteral("clothes"));
            if (type.isEmpty()) {
                type = data.tagValue(QStringLiteral("designation"));
            }
            if (!type.isEmpty()) {
                type[0] = type[0].toUpper();
                amenity = QLatin1String("Shop - ") + shop + QLatin1String(" (") + type + QLatin1Char(')');
            }
        }
        if (amenity.isEmpty()) {
            amenity = QLatin1String("Shop - ") + shop;
        }
    } else {
        amenity = data.tagValue(QStringLiteral("amenity"));
        if (!amenity.isEmpty()) {
            amenity[0] = amenity[0].toUpper();
        }
    }
    if (!amenity.isEmpty()) {
        doc[QStringLiteral("amenity")] = amenity;
    } else {
        doc[QStringLiteral("amenityVisibility")] = none;
    }

    QString cuisine = data.tagValue(QStringLiteral("cuisine"));
    if (!cuisine.isEmpty()) {
        cuisine[0] = cuisine[0].toUpper();
        doc[QStringLiteral("cuisine")] = cuisine;
    } else {
        doc[QStringLiteral("cuisineVisibility")] = none;
    }

    QString openingHours = data.tagValue(QStringLiteral("opening_hours"));
    if (!openingHours.isEmpty()) {
        doc[QStringLiteral("openinghours")] = openingHours;
    } else {
        doc[QStringLiteral("openinghoursVisibility")] = none;
    }

    bool hasContactsData = false;

    const QStringList addressItemKeys = QStringList()
        << QStringLiteral("street")
        << QStringLiteral("housenumber")
        << QStringLiteral("postcode")
        << QStringLiteral("city");
    bool hasAddressItem = false;
    QStringList addressItems;
    foreach (const QString& key, addressItemKeys) {
        const QString item = data.tagValue(QLatin1String("addr:") + key);
        if (!item.isEmpty()) {
            hasAddressItem = true;
        }
        addressItems << item;
    }
    if (hasAddressItem) {
        hasContactsData = true;
        for(int i = 0; i < addressItemKeys.size(); ++i) {
            doc[addressItemKeys[i]] = addressItems[i];
        }
    } else {
        doc[QStringLiteral("addressVisibility")] = none;
    }

    QString phoneData = data.tagValue(QStringLiteral("phone"));
    if (!phoneData.isEmpty()) {
        hasContactsData = true;
        doc[QStringLiteral("phone")] = phoneData;
    } else {
        doc[QStringLiteral("phoneVisibility")] = none;
    }

    QString websiteData;
    foreach(const QString &tag, QStringList() << "website" << "contact:website" << "facebook" << "contact:facebook" << "url") {
        websiteData = data.tagValue(tag);
        if (!websiteData.isEmpty()) {
            break;
        }
    }
    if (!websiteData.isEmpty()) {
        hasContactsData = true;
        doc[QStringLiteral("website")] = websiteData;
    } else {
        doc[QStringLiteral("websiteVisibility")] = none;
    }

    if (!hasContactsData) {
        doc[QStringLiteral("contactVisibility")] = none;
    }

    bool hasFacilitiesData = false;

    const QString wheelchair = data.tagValue(QStringLiteral("wheelchair"));
    if (!wheelchair.isEmpty()) {
        hasFacilitiesData = true;
        doc[QStringLiteral("wheelchair")] = wheelchair;
    } else {
        doc[QStringLiteral("wheelchairVisibility")] = none;
    }

    const QString internetAccess = data.tagValue(QStringLiteral("internet_access"));
    if (!internetAccess.isEmpty()) {
        hasFacilitiesData = true;
        doc[QStringLiteral("internetaccess")] = internetAccess;
    } else {
        doc[QStringLiteral("internetVisibility")] = none;
    }

    const QString smoking = data.tagValue(QStringLiteral("smoking"));
    if (!smoking.isEmpty()) {
        hasFacilitiesData = true;
        doc[QStringLiteral("smoking")] = smoking;
    } else {
        doc[QStringLiteral("smokingVisibility")] = none;
    }

    if (!hasFacilitiesData) {
        doc[QStringLiteral("facilitiesVisibility")] = none;
    }

    const QString flagPath = m_widget->styleBuilder()->createStyle(StyleParameters(placemark))->iconStyle().iconPath();
    doc["flag"] = flagPath;
    popup->setContent(doc.finalText());
}

void MarbleWidgetPopupMenu::Private::setupDialogSatellite( const GeoDataPlacemark *placemark )
{
    PopupLayer *const popup = m_widget->popupLayer();
    const GeoDataCoordinates location = placemark->coordinate(m_widget->model()->clockDateTime());
    popup->setCoordinates(location, Qt::AlignRight | Qt::AlignVCenter);

    const QString description = placemark->description();
    TemplateDocument doc(description);
    doc["altitude"] = QString::number(location.altitude(), 'f', 2);
    doc["latitude"] = location.latToString();
    doc["longitude"] = location.lonToString();
    popup->setContent(doc.finalText());
}

void MarbleWidgetPopupMenu::Private::setupDialogCity( PopupLayer *popup, const GeoDataPlacemark *placemark )
{
    const GeoDataCoordinates location = placemark->coordinate();
    popup->setCoordinates(location, Qt::AlignRight | Qt::AlignVCenter);

    QFile descriptionFile(QStringLiteral(":/marble/webpopup/city.html"));
    if (!descriptionFile.open(QIODevice::ReadOnly)) {
        return;
    }

    const QString description = descriptionFile.readAll();
    TemplateDocument doc(description);

    doc["name"] = placemark->name();
    QString  roleString;
    const QString role = placemark->role();
    if (role == QLatin1String("PPLC")) {
        roleString = tr("National Capital");
    } else if (role == QLatin1String("PPL")) {
        roleString = tr("City");
    } else if (role == QLatin1String("PPLA")) {
        roleString = tr("State Capital");
    } else if (role == QLatin1String("PPLA2")) {
        roleString = tr("County Capital");
    } else if (role == QLatin1String("PPLA3") ||
               role == QLatin1String("PPLA4")) {
        roleString = tr("Capital");
    } else if (role == QLatin1String("PPLF") ||
               role == QLatin1String("PPLG") ||
               role == QLatin1String("PPLL") ||
               role == QLatin1String("PPLQ") ||
               role == QLatin1String("PPLR") ||
               role == QLatin1String("PPLS") ||
               role == QLatin1String("PPLW")) {
        roleString = tr("Village");
    }

    doc["category"] = roleString;
    doc["shortDescription"] = filterEmptyShortDescription(placemark->description());
    doc["latitude"] = location.latToString();
    doc["longitude"] = location.lonToString();
    doc["elevation"] =  QString::number(location.altitude(), 'f', 2);
    doc["population"] = QString::number(placemark->population());
    doc["country"] = placemark->countryCode();
    doc["state"] = placemark->state();

    QString dst = QStringLiteral("%1").arg((placemark->extendedData().value(QStringLiteral("gmt")).value().toInt() +
                                            placemark->extendedData().value(QStringLiteral("dst")).value().toInt()) /
                                            ( double ) 100, 0, 'f', 1 );
    // There is an issue about UTC.
    // It's possible to variants (e.g.):
    // +1.0 and -1.0, but dst does not have + an the start
    if (dst.startsWith(QLatin1Char('-'))) {
        doc["timezone"] = dst;
    } else {
        doc["timezone"] = QLatin1Char('+') + dst;
    }

    const QString flagPath = MarbleDirs::path(
                QLatin1String("flags/flag_") + placemark->countryCode().toLower() + QLatin1String(".svg"));
    doc["flag"] = flagPath;

    popup->setContent(doc.finalText());
}

void MarbleWidgetPopupMenu::Private::setupDialogNation( PopupLayer *popup, const GeoDataPlacemark *index)
{
    const GeoDataCoordinates location = index->coordinate();
    popup->setCoordinates(location, Qt::AlignRight | Qt::AlignVCenter);

    QFile descriptionFile(QStringLiteral(":/marble/webpopup/nation.html"));
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

void MarbleWidgetPopupMenu::Private::setupDialogGeoPlaces( PopupLayer *popup, const GeoDataPlacemark *index)
{
    const GeoDataCoordinates location = index->coordinate();
    popup->setCoordinates(location, Qt::AlignRight | Qt::AlignVCenter);

    QFile descriptionFile(QStringLiteral(":/marble/webpopup/geoplace.html"));
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

void MarbleWidgetPopupMenu::Private::setupDialogSkyPlaces( PopupLayer *popup, const GeoDataPlacemark *index)
{
    const GeoDataCoordinates location = index->coordinate();
    popup->setCoordinates(location, Qt::AlignRight | Qt::AlignVCenter);

    QFile descriptionFile(QStringLiteral(":/marble/webpopup/skyplace.html"));
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

void MarbleWidgetPopupMenu::Private::setupDialogPhotoOverlay( PopupLayer *popup, const GeoDataPhotoOverlay *index )
{
    const GeoDataCoordinates location = index->point().coordinates();
    popup->setCoordinates(location, Qt::AlignRight | Qt::AlignVCenter);

    QFile descriptionFile(QStringLiteral(":/marble/webpopup/photooverlay.html"));

    if ( !descriptionFile.open(QIODevice::ReadOnly) ) {
        return;
    }

    const QString description = descriptionFile.readAll();
    TemplateDocument doc(description);
    doc["name"] = index->name();
    doc["latitude"] = location.latToString();
    doc["longitude"] = location.lonToString();
    doc["elevation"] = QString::number(location.altitude(), 'f', 2);
    doc["shortDescription"] = filterEmptyShortDescription(index->description());
    doc["source"] = index->absoluteIconFile();
    doc["width"] = QString::number(200);
    doc["height"] = QString::number(100);
    QString const basePath = index->resolvePath(".");
    QUrl const baseUrl = (basePath != QLatin1String(".")) ? QUrl::fromLocalFile(basePath + QLatin1Char('/')) : QUrl();
    popup->setContent(doc.finalText(), baseUrl );
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

    QVector<const GeoDataFeature*>::const_iterator it = d->m_featurelist.constBegin();
    QVector<const GeoDataFeature*>::const_iterator const itEnd = d->m_featurelist.constEnd();
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
        const GeoDataPlacemark *placemark = dynamic_cast<const GeoDataPlacemark*>(d->m_featurelist.at( actionidx -1 ));
        const GeoDataPhotoOverlay *overlay = dynamic_cast<const GeoDataPhotoOverlay*>(d->m_featurelist.at( actionidx - 1 ));
        PopupLayer* popup = d->m_widget->popupLayer();
        bool isSatellite = false;
        bool isCity = false;
        bool isNation = false;

        const OsmPlacemarkData& data = placemark->osmData();

        bool hasOsmData = false;

        QStringList recognizedTags;
        recognizedTags << "name" << "amenity" << "cuisine" << "opening_hours";
        recognizedTags << "addr:street" << "addr:housenumber" << "addr:postcode";
        recognizedTags << "addr:city" << "phone" << "wheelchair" << "internet_access";
        recognizedTags << "smoking" << "website" << "contact:website" << "facebook";
        recognizedTags << "contact:facebook" << "url";

        foreach(const QString &tag, recognizedTags) {
            if (data.containsTagKey(tag)) {
                hasOsmData = true;
                break;
            }
        }

        if ( placemark ) {
            isSatellite = (placemark->visualCategory() == GeoDataPlacemark::Satellite);
            isCity = (placemark->visualCategory() >= GeoDataPlacemark::SmallCity &&
                      placemark->visualCategory() <= GeoDataPlacemark::LargeNationCapital);
            isNation = (placemark->visualCategory() == GeoDataPlacemark::Nation);
        }

        bool isSky = false;

        if ( d->m_widget->model()->mapTheme() ) {
            isSky = d->m_widget->model()->mapTheme()->head()->target() == QLatin1String("sky");
        }

        popup->setSize(QSizeF(420, 420));

        if (hasOsmData){
            d->setupDialogOsm( popup, placemark );
        } else if (isSatellite) {
            d->setupDialogSatellite( placemark );
        } else if (isCity) {
            Private::setupDialogCity( popup, placemark );
        } else if (isNation) {
            Private::setupDialogNation( popup, placemark );
        } else if (isSky) {
            Private::setupDialogSkyPlaces( popup, placemark );
        } else if ( overlay ) {
            Private::setupDialogPhotoOverlay( popup, overlay );
        } else if ( placemark && placemark->role().isEmpty() ) {
            popup->setContent( placemark->description() );
        } else if ( placemark ) {
            Private::setupDialogGeoPlaces( popup, placemark );
        }

        if ( placemark ) {
            if ( placemark->style() == 0 ) {
                popup->setBackgroundColor(QColor(Qt::white));
                popup->setTextColor(QColor(Qt::black));
                return;
            }
            if ( placemark->style()->balloonStyle().displayMode() == GeoDataBalloonStyle::Hide ) {
                popup->setVisible(false);
                return;
            }

            QString content = placemark->style()->balloonStyle().text();
            if (content.length() > 0) {
                content.replace(QStringLiteral("$[name]"), placemark->name(), Qt::CaseInsensitive);
                content.replace(QStringLiteral("$[description]"), placemark->description(), Qt::CaseInsensitive);
                content.replace(QStringLiteral("$[address]"), placemark->address(), Qt::CaseInsensitive);
                // @TODO: implement the line calculation, so that snippet().maxLines actually has effect.
                content.replace(QStringLiteral("$[snippet]"), placemark->snippet().text(), Qt::CaseInsensitive);
                content.replace(QStringLiteral("$[id]"), placemark->id(), Qt::CaseInsensitive);
                QString const basePath = placemark->resolvePath(".");
                QUrl const baseUrl = (basePath != QLatin1String(".")) ? QUrl::fromLocalFile(basePath + QLatin1Char('/')) : QUrl();
                popup->setContent(content, baseUrl );
            }

            popup->setBackgroundColor(placemark->style()->balloonStyle().backgroundColor());
            popup->setTextColor(placemark->style()->balloonStyle().textColor());
        }

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

    if ( !m_featurelist.isEmpty() && m_featurelist.first()->nodeType() == GeoDataTypes::GeoDataPlacemarkType ) {
        const GeoDataPlacemark * placemark =  static_cast<const GeoDataPlacemark*>( m_featurelist.first() );
        return placemark->coordinate( m_model->clock()->dateTime() );
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

#include "moc_MarbleWidgetPopupMenu.cpp"
