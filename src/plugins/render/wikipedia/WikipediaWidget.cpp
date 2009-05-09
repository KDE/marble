//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "WikipediaWidget.h"

// Qt
#include <QtGui/QAction>
#include <QtCore/QDebug>
#include <QtGui/QIcon>
#include <QtGui/QPixmap>
#include <QtGui/QMouseEvent>
#include <QtWebKit/QWebView>

// Marble
#include "GeoPainter.h"
#include "ViewportParams.h"
#include "GeoSceneLayer.h"

using namespace Marble;

WikipediaWidget::WikipediaWidget( QObject *parent )
    : AbstractDataPluginWidget( parent ),
      m_browser( 0 )
{
    m_action = new QAction( this );
    connect( m_action, SIGNAL( triggered() ), this, SLOT( openBrowser() ) );
}

WikipediaWidget::~WikipediaWidget() {
    delete m_browser;
}

QString WikipediaWidget::name() const {
    return id();
}

QString WikipediaWidget::widgetType() const {
    return "wikipediaWidget";
}
     
bool WikipediaWidget::initialized() {
    return true;
}
    
void WikipediaWidget::addDownloadedFile( QString url, QString type ) {
    // There shouldn't be downloaded files for this widget
}

bool WikipediaWidget::operator<( const AbstractDataPluginWidget *other ) const {
    return this->id() < other->id();
}
   
bool WikipediaWidget::render( GeoPainter *painter, ViewportParams *viewport,
                              const QString& renderPos, GeoSceneLayer * layer )
{
    painter->drawPixmap( coordinates(), *m_pixmap );
    
    updatePaintPosition( viewport, m_pixmap->size() );
    
    return true;
}

qreal WikipediaWidget::longitude() {
    return coordinates().longitude();
}
    
void WikipediaWidget::setLongitude( qreal longitude ) {
    GeoDataCoordinates updatedCoordinates = coordinates();
    updatedCoordinates.setLongitude( longitude );
    setCoordinates( updatedCoordinates );
}
    
qreal WikipediaWidget::latitude() {
    return coordinates().latitude();
}

void WikipediaWidget::setLatitude( qreal latitude ) {
    GeoDataCoordinates updatedCoordinates = coordinates();
    updatedCoordinates.setLatitude( latitude );
    setCoordinates( updatedCoordinates );
}

QUrl WikipediaWidget::url() {
    return m_url;
}

void WikipediaWidget::setUrl( QUrl url ) {
    m_url = url;
}

QUrl WikipediaWidget::thumbnailImageUrl() {
    return m_thumbnailImageUrl;
}

void WikipediaWidget::setThumbnailImageUrl( QUrl thumbnailImageUrl ) {
    m_thumbnailImageUrl = thumbnailImageUrl;
}

QAction *WikipediaWidget::action() {
    m_action->setText( id() );
    return m_action;
}

void WikipediaWidget::openBrowser( ) {
    if( m_browser ) {
        delete m_browser;
    }
    m_browser = new QWebView();
    m_browser->load( url() );
    m_browser->show();
    connect( m_browser, SIGNAL( titleChanged(QString) ),
             m_browser, SLOT( setWindowTitle(QString) ) );
}

void WikipediaWidget::setPixmap( QPixmap *pixmap ) {
    m_pixmap = pixmap;
}
    
void WikipediaWidget::setIcon( QIcon *icon ) {
    m_action->setIcon( *icon );
}
