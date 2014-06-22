//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012   Mohammed Nafees   <nafees.technocool@gmail.com>
// Copyright 2012   Dennis Nienhüser  <earthwings@gentoo.org>
// Copyright 2012   Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include "PopupLayer.h"

#include "GeoPainter.h"
#include "MarbleWidget.h"
#include "MarbleModel.h"
#include "MarbleDirs.h"
#include "PopupItem.h"
#include "ViewportParams.h"
#include "GeoDataExtendedData.h"
#include "GeoDataPlacemark.h"
#include "GeoDataStyle.h"
#include "GeoDataSnippet.h"
#include "GeoSceneDocument.h"
#include "GeoSceneHead.h"
#include "TemplateDocument.h"

#include <QSizeF>

namespace Marble
{

class PopupLayer::Private
{
public:
    Private( MarbleWidget *marbleWidget, PopupLayer *q );

    /**
     * @brief Sets size of the popup item, based on the requested size and viewport size
     * @param viewport required to compute the maximum dimensions
     */
    void setAppropriateSize( const ViewportParams *viewport );

    static QString filterEmptyShortDescription(const QString &description);
    void setupDialogSatellite( const GeoDataPlacemark *index );
    void setupDialogCity( const GeoDataPlacemark *index );
    void setupDialogNation( const GeoDataPlacemark *index );
    void setupDialogGeoPlaces( const GeoDataPlacemark *index );
    void setupDialogSkyPlaces( const GeoDataPlacemark *index );

    PopupLayer *const q;
    PopupItem *const m_popupItem;
    MarbleWidget *const m_widget;
    QSizeF m_requestedSize;
    bool m_adjustMap;
};

PopupLayer::Private::Private( MarbleWidget *marbleWidget, PopupLayer *_q ) :
    q( _q ),
    m_popupItem( new PopupItem( _q ) ),
    m_widget( marbleWidget ),
    m_adjustMap( false )
{
}

PopupLayer::PopupLayer( MarbleWidget *marbleWidget, QObject *parent ) :
    QObject( parent ),
    d( new Private( marbleWidget, this ) )
{
    connect( d->m_popupItem, SIGNAL(repaintNeeded()), this, SIGNAL(repaintNeeded()) );
    connect( d->m_popupItem, SIGNAL(hide()), this, SLOT(hidePopupItem()) );
}

PopupLayer::~PopupLayer()
{
    delete d;
}

QStringList PopupLayer::renderPosition() const
{
    return QStringList( "ALWAYS_ON_TOP" );
}

bool PopupLayer::render( GeoPainter *painter, ViewportParams *viewport,
                                const QString&, GeoSceneLayer* )
{
    if ( visible() ) {
        d->setAppropriateSize( viewport );
        if ( d->m_adjustMap ) {
            GeoDataCoordinates coords = d->m_popupItem->coordinate();
            d->m_widget->centerOn( coords, false );
            qreal sx, sy, lon, lat;
            viewport->screenCoordinates(coords, sx, sy);
            sx = viewport->radius() < viewport->width() ? 0.5 * (viewport->width() + viewport->radius()) : 0.75 * viewport->width();
            viewport->geoCoordinates(sx, sy, lon, lat, GeoDataCoordinates::Radian);
            coords.setLatitude(lat);
            coords.setLongitude(lon);
            d->m_widget->centerOn( coords, true );
            d->m_adjustMap = false;
        }
        d->m_popupItem->paintEvent( painter, viewport );
    }

    return true;
}

bool PopupLayer::eventFilter( QObject *object, QEvent *e )
{
    return visible() && d->m_popupItem->eventFilter( object, e );
}

qreal PopupLayer::zValue() const
{
    return 4711.23;
}

RenderState PopupLayer::renderState() const
{
    return RenderState( "Popup Window" );
}

bool PopupLayer::visible() const
{
    return d->m_popupItem->visible();
}

void PopupLayer::setVisible( bool visible )
{
    d->m_popupItem->setVisible( visible );
    if ( !visible ) {
        disconnect( d->m_popupItem, SIGNAL(repaintNeeded()), this, SIGNAL(repaintNeeded()) );
        d->m_popupItem->clearHistory();
        emit repaintNeeded();
    }
    else {
        connect( d->m_popupItem, SIGNAL(repaintNeeded()), this, SIGNAL(repaintNeeded()) );
    }
}

void PopupLayer::popup()
{
    d->m_adjustMap = true;
    setVisible( true );
}

void PopupLayer::setCoordinates(const GeoDataCoordinates &coordinates , Qt::Alignment alignment)
{
    d->m_popupItem->setCoordinate( coordinates );
    d->m_popupItem->setAlignment( alignment );
}

void PopupLayer::setUrl( const QUrl &url )
{
    d->m_popupItem->setUrl( url );
}

void PopupLayer::setContent( const QString &html, const QUrl &baseUrl )
{
    d->m_popupItem->setContent( html, baseUrl );
}

void PopupLayer::setPlacemark( const GeoDataPlacemark *placemark )
{
    bool isSatellite = (placemark->visualCategory() == GeoDataFeature::Satellite);
    bool isCity (placemark->visualCategory() >= GeoDataFeature::SmallCity &&
                     placemark->visualCategory() <= GeoDataFeature::LargeNationCapital);
    bool isNation = (placemark->visualCategory() == GeoDataFeature::Nation);
    bool isSky = false;
    if ( d->m_widget->model()->mapTheme() ) {
        isSky = d->m_widget->model()->mapTheme()->head()->target() == "sky";
    }
    setSize(QSizeF(400, 400));
    if (isSatellite) {
        d->setupDialogSatellite( placemark );
    } else if (isCity) {
        d->setupDialogCity( placemark );
    } else if (isNation) {
        d->setupDialogNation( placemark );
    } else if (isSky) {
        d->setupDialogSkyPlaces( placemark );
    } else if ( placemark->role().isEmpty() ) {
        setContent( placemark->description() );
    } else {
        d->setupDialogGeoPlaces( placemark );
    }

    if (placemark->style() == 0) {
        d->m_popupItem->setBackgroundColor(QColor(Qt::white));
        d->m_popupItem->setTextColor(QColor(Qt::black));
        return;
    }
    if (placemark->style()->balloonStyle().displayMode() == GeoDataBalloonStyle::Hide) {
        setVisible(false);
        return;
    }
    QString content = placemark->style()->balloonStyle().text();
    if (content.length() > 0) {
        content = content.replace("$[name]", placemark->name(), Qt::CaseInsensitive);
        content = content.replace("$[description]", placemark->description(), Qt::CaseInsensitive);
        content = content.replace("$[address]", placemark->address(), Qt::CaseInsensitive);
        // @TODO: implement the line calculation, so that snippet().maxLines actually has effect.
        content = content.replace("$[snippet]", placemark->snippet().text(), Qt::CaseInsensitive);
        content = content.replace("$[id]", placemark->id(), Qt::CaseInsensitive);
        QString const basePath = placemark->resolvePath(".");
        QUrl const baseUrl = basePath != "." ? QUrl::fromLocalFile( basePath + "/" ) : QUrl();
        d->m_popupItem->setContent(content, baseUrl );
    }
    d->m_popupItem->setBackgroundColor(placemark->style()->balloonStyle().backgroundColor());
    d->m_popupItem->setTextColor(placemark->style()->balloonStyle().textColor());
}

void PopupLayer::setBackgroundColor(const QColor &color)
{
    if(color.isValid()) {
        d->m_popupItem->setBackgroundColor(color);
    }
}

void PopupLayer::setTextColor(const QColor &color)
{
    if(color.isValid()) {
        d->m_popupItem->setTextColor(color);
    }
}

QString PopupLayer::Private::filterEmptyShortDescription(const QString &description)
{
    if(description.isEmpty())
        return tr("No description available.");
    return description;
}

void PopupLayer::Private::setupDialogSatellite( const GeoDataPlacemark *index)
{
    GeoDataCoordinates location = index->coordinate(m_widget->model()->clockDateTime());
    q->setCoordinates(location, Qt::AlignRight | Qt::AlignVCenter);

    const QString description = index->description();
    TemplateDocument doc(description);
    doc["altitude"] = QString::number(location.altitude(), 'f', 2);
    doc["latitude"] = location.latToString();
    doc["longitude"] = location.lonToString();
    q->setContent(doc.finalText());
}

void PopupLayer::Private::setupDialogCity( const GeoDataPlacemark *index)
{
    GeoDataCoordinates location = index->coordinate();
    q->setCoordinates(location, Qt::AlignRight | Qt::AlignVCenter);

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
        doc["timezone"] = '+'+dst;
    }

    const QString flagPath = MarbleDirs::path(
                QString("flags/flag_%1.svg").arg(index->countryCode().toLower()));
    doc["flag"] = flagPath;

    q->setContent(doc.finalText());
}

void PopupLayer::Private::setupDialogNation( const GeoDataPlacemark *index)
{
    GeoDataCoordinates location = index->coordinate();
    q->setCoordinates(location, Qt::AlignRight | Qt::AlignVCenter);

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

    q->setContent(doc.finalText());
}

void PopupLayer::Private::setupDialogGeoPlaces( const GeoDataPlacemark *index)
{
    GeoDataCoordinates location = index->coordinate();
    q->setCoordinates(location, Qt::AlignRight | Qt::AlignVCenter);

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

    q->setContent(doc.finalText());
}

void PopupLayer::Private::setupDialogSkyPlaces( const GeoDataPlacemark *index)
{
    GeoDataCoordinates location = index->coordinate();
    q->setCoordinates(location, Qt::AlignRight | Qt::AlignVCenter);

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

    q->setContent(doc.finalText());
}



void PopupLayer::setSize( const QSizeF &size )
{
    d->m_requestedSize = size;
}

void PopupLayer::Private::setAppropriateSize( const ViewportParams *viewport )
{
    qreal margin = 15.0;

    QSizeF maximumSize;
    maximumSize.setWidth( viewport->width() - margin );
    maximumSize.setHeight( viewport->height() - margin );

    QSizeF minimumSize( 100.0, 100.0 );

    m_popupItem->setSize( m_requestedSize.boundedTo( maximumSize ).expandedTo( minimumSize ) );
}

void PopupLayer::hidePopupItem()
{
    setVisible( false );
}

}

#include "PopupLayer.moc"
