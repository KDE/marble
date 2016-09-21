//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012   Mohammed Nafees   <nafees.technocool@gmail.com>
// Copyright 2012   Dennis Nienhüser  <nienhueser@kde.org>
// Copyright 2012   Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
// Copyright 2015   Imran Tatriev     <itatriev@gmail.com>
//

#include "PopupLayer.h"

#include "GeoDataCoordinates.h"
#include "GeoPainter.h"
#include "MarbleWidget.h"
#include "PopupItem.h"
#include "ViewportParams.h"
#include "RenderPlugin.h"
#include "RenderState.h"

#include <QSizeF>

namespace Marble
{

class Q_DECL_HIDDEN PopupLayer::Private
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

    PopupItem *const m_popupItem;
    MarbleWidget *const m_widget;
    QSizeF m_requestedSize;
    bool m_hasCrosshairsPlugin;
    bool m_crosshairsVisible;
};

PopupLayer::Private::Private( MarbleWidget *marbleWidget, PopupLayer *q ) :
    m_popupItem( new PopupItem( q ) ),
    m_widget( marbleWidget ),
    m_hasCrosshairsPlugin( false ),
    m_crosshairsVisible( true )
{
}

PopupLayer::PopupLayer( MarbleWidget *marbleWidget, QObject *parent ) :
    QObject( parent ),
    d( new Private( marbleWidget, this ) )
{
    foreach (const RenderPlugin *renderPlugin, d->m_widget->renderPlugins()) {
        if (renderPlugin->nameId() == QLatin1String("crosshairs")) {
            d->m_hasCrosshairsPlugin = true;
            break;
        }
    }

    connect( d->m_popupItem, SIGNAL(repaintNeeded()), this, SIGNAL(repaintNeeded()) );
    connect( d->m_popupItem, SIGNAL(hide()), this, SLOT(hidePopupItem()) );
}

PopupLayer::~PopupLayer()
{
    delete d;
}

QStringList PopupLayer::renderPosition() const
{
    return QStringList(QStringLiteral("ALWAYS_ON_TOP"));
}

bool PopupLayer::render( GeoPainter *painter, ViewportParams *viewport,
                                const QString&, GeoSceneLayer* )
{
    if ( visible() ) {
        d->setAppropriateSize( viewport );
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
    return RenderState(QStringLiteral("Popup Window"));
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
    GeoDataCoordinates coords = d->m_popupItem->coordinate();
    ViewportParams viewport( d->m_widget->viewport()->projection(),
                             coords.longitude(), coords.latitude(), d->m_widget->viewport()->radius(),
                             d->m_widget->viewport()->size() );
    qreal sx, sy, lon, lat;
    viewport.screenCoordinates( coords, sx, sy );
    sx = viewport.radius() < viewport.width() ? 0.5 * (viewport.width() + viewport.radius()) : 0.75 * viewport.width();
    viewport.geoCoordinates( sx, sy, lon, lat, GeoDataCoordinates::Radian );
    coords.setLatitude( lat );
    coords.setLongitude( lon );
    d->m_widget->centerOn( coords, true );

    if( d->m_hasCrosshairsPlugin ) {
        d->m_crosshairsVisible = d->m_widget->showCrosshairs();

        if( d->m_crosshairsVisible ) {
            d->m_widget->setShowCrosshairs( false );
        }
    }

    setVisible( true );
}

void PopupLayer::setCoordinates( const GeoDataCoordinates &coordinates , Qt::Alignment alignment )
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
    if( d->m_hasCrosshairsPlugin && d->m_crosshairsVisible ) {
        d->m_widget->setShowCrosshairs( d->m_crosshairsVisible );
    }

    setVisible( false );
}

}

#include "moc_PopupLayer.cpp"
