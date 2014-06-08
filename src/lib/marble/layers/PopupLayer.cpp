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
#include "MarbleWidget.h"
#include "MarbleModel.h"
#include "MarbleDirs.h"
#include "PopupItem.h"
#include "GeoDataExtendedData.h"
#include "GeoDataPlacemark.h"
#include "GeoDataStyle.h"
#include "GeoDataSnippet.h"
#include "GeoSceneDocument.h"
#include "GeoSceneHead.h"
#include "TemplateDocument.h"

#include <QMouseEvent>
#include <QApplication>
#include <QAction>

namespace Marble
{

PopupLayer::PopupLayer( MarbleWidget *marbleWidget, QObject *parent ) :
    QObject( parent ),
    m_popupItem( new PopupItem( this ) ),
    m_widget( marbleWidget ),
    m_adjustMap( false )
{
    connect( m_popupItem, SIGNAL(repaintNeeded()), this, SIGNAL(repaintNeeded()) );
    connect( m_popupItem, SIGNAL(hide()), this, SLOT(hidePopupItem()) );
}

PopupLayer::~PopupLayer()
{
}

QStringList PopupLayer::renderPosition() const
{
    return QStringList( "ALWAYS_ON_TOP" );
}

bool PopupLayer::render( GeoPainter *painter, ViewportParams *viewport,
                                const QString&, GeoSceneLayer* )
{
    if ( visible() ) {
        setAppropriateSize( viewport );
        if ( m_adjustMap ) {
            GeoDataCoordinates coords = m_popupItem->coordinate();
            m_widget->centerOn( coords, false );
            qreal sx, sy, lon, lat;
            viewport->screenCoordinates(coords, sx, sy);
            sx = viewport->radius() < viewport->width() ? 0.5 * (viewport->width() + viewport->radius()) : 0.75 * viewport->width();
            viewport->geoCoordinates(sx, sy, lon, lat, GeoDataCoordinates::Radian);
            coords.setLatitude(lat);
            coords.setLongitude(lon);
            m_widget->centerOn( coords, true );
            m_adjustMap = false;
        }
        m_popupItem->paintEvent( painter, viewport );
    }

    return true;
}

bool PopupLayer::eventFilter( QObject *object, QEvent *e )
{
    return visible() && m_popupItem->eventFilter( object, e );
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
    return m_popupItem->visible();
}

void PopupLayer::setVisible( bool visible )
{
    m_popupItem->setVisible( visible );
    if ( !visible ) {
        disconnect( m_popupItem, SIGNAL(repaintNeeded()), this, SIGNAL(repaintNeeded()) );
        m_popupItem->clearHistory();
        emit repaintNeeded();
    }
    else {
        connect( m_popupItem, SIGNAL(repaintNeeded()), this, SIGNAL(repaintNeeded()) );
    }
}

void PopupLayer::popup()
{
    m_adjustMap = true;
    setVisible( true );
}

void PopupLayer::setCoordinates(const GeoDataCoordinates &coordinates , Qt::Alignment alignment)
{
    m_popupItem->setCoordinate( coordinates );
    m_popupItem->setAlignment( alignment );
}

void PopupLayer::setUrl( const QUrl &url )
{
    m_popupItem->setUrl( url );
}

void PopupLayer::setContent( const QString &html, const QUrl &baseUrl )
{
    m_popupItem->setContent( html, baseUrl );
    emit repaintNeeded();
}

void PopupLayer::setPlacemark( const GeoDataPlacemark *placemark )
{
    bool isSatellite = (placemark->visualCategory() == GeoDataFeature::Satellite);
    bool isCity (placemark->visualCategory() >= GeoDataFeature::SmallCity &&
                     placemark->visualCategory() <= GeoDataFeature::LargeNationCapital);
    bool isNation = (placemark->visualCategory() == GeoDataFeature::Nation);
    bool isSky = false;
    if ( m_widget->model()->mapTheme() ) {
        isSky = m_widget->model()->mapTheme()->head()->target() == "sky";
    }
    setSize(QSizeF(400, 400));
    if (isSatellite) {
        setupDialogSatellite( placemark );
    } else if (isCity) {
        setupDialogCity( placemark );
    } else if (isNation) {
        setupDialogNation( placemark );
    } else if (isSky) {
        setupDialogSkyPlaces( placemark );
    } else if ( placemark->role().isEmpty() ) {
        setContent( placemark->description() );
    } else {
        setupDialogGeoPlaces( placemark );
    }

    if (placemark->style() == 0) {
        m_popupItem->setBackgroundColor(QColor(Qt::white));
        m_popupItem->setTextColor(QColor(Qt::black));
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
        m_popupItem->setContent(content, baseUrl );
    }
    m_popupItem->setBackgroundColor(placemark->style()->balloonStyle().backgroundColor());
    m_popupItem->setTextColor(placemark->style()->balloonStyle().textColor());
    emit repaintNeeded();
}

void PopupLayer::setBackgroundColor(const QColor &color)
{
    if(color.isValid()) {
        m_popupItem->setBackgroundColor(color);
    }
}

void PopupLayer::setTextColor(const QColor &color)
{
    if(color.isValid()) {
        m_popupItem->setTextColor(color);
    }
}

QString PopupLayer::filterEmptyShortDescription(const QString &description)
{
    if(description.isEmpty())
        return tr("No description available.");
    return description;
}

void PopupLayer::setupDialogSatellite( const GeoDataPlacemark *index)
{
    GeoDataCoordinates location = index->coordinate(m_widget->model()->clockDateTime());
    setCoordinates(location, Qt::AlignRight | Qt::AlignVCenter);

    const QString description = index->description();
    TemplateDocument doc(description);
    doc["altitude"] = QString::number(location.altitude(), 'f', 2);
    doc["latitude"] = location.latToString();
    doc["longitude"] = location.lonToString();
    setContent(doc.finalText());
}

void PopupLayer::setupDialogCity( const GeoDataPlacemark *index)
{
    GeoDataCoordinates location = index->coordinate();
    setCoordinates(location, Qt::AlignRight | Qt::AlignVCenter);

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

    setContent(doc.finalText());
}

void PopupLayer::setupDialogNation( const GeoDataPlacemark *index)
{
    GeoDataCoordinates location = index->coordinate();
    setCoordinates(location, Qt::AlignRight | Qt::AlignVCenter);

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

    setContent(doc.finalText());
}

void PopupLayer::setupDialogGeoPlaces( const GeoDataPlacemark *index)
{
    GeoDataCoordinates location = index->coordinate();
    setCoordinates(location, Qt::AlignRight | Qt::AlignVCenter);

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

    setContent(doc.finalText());
}

void PopupLayer::setupDialogSkyPlaces( const GeoDataPlacemark *index)
{
    GeoDataCoordinates location = index->coordinate();
    setCoordinates(location, Qt::AlignRight | Qt::AlignVCenter);

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

    setContent(doc.finalText());
}



void PopupLayer::setSize( const QSizeF &size )
{
    m_requestedSize = size;
}

void PopupLayer::setAppropriateSize( const ViewportParams *viewport )
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
