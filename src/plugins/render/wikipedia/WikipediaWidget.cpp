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
#include <QtCore/QDebug>

// Marble
#include "GeoPainter.h"
#include "ViewportParams.h"
#include "GeoSceneLayer.h"

using namespace Marble;

WikipediaWidget::WikipediaWidget( QObject *parent )
    : AbstractDataPluginWidget( parent )
{
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
    // FIXME: Painting required
    return true;
}

qreal WikipediaWidget::longitude() {
    return m_longitude;
}
    
void WikipediaWidget::setLongitude( qreal longitude ) {
    m_longitude = longitude;
}
    
qreal WikipediaWidget::latitude() {
    return m_latitude;
}

void WikipediaWidget::setLatitude( qreal latitude ) {
    m_latitude = latitude;
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
