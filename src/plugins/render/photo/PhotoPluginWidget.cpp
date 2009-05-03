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
#include "PhotoPluginWidget.h"

// Plugin
#include "CoordinatesParser.h"

// Marble
#include "AbstractDataPluginWidget.h"
#include "GeoDataCoordinates.h"
#include "GeoPainter.h"
#include "ViewportParams.h"

// Qt
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QUrl>

using namespace Marble;

PhotoPluginWidget::PhotoPluginWidget( QObject *parent )
    : AbstractDataPluginWidget( parent ),
      m_hasCoordinates( false )
{
}

QString PhotoPluginWidget::widgetType() const {
    return QString( "photoWidget" );
}
 
bool PhotoPluginWidget::initialized() {
    return !m_smallImage.isNull() && m_hasCoordinates;
}

void PhotoPluginWidget::addDownloadedFile( QString url, QString type ) {
    if( type == "thumbnail" ) {
        m_smallImage.load( url );
    }
    else if ( type == "info" ) {        
        QFile file( url );
        if( !file.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
            return;
        }
        
        GeoDataCoordinates coordinates;
        CoordinatesParser parser( &coordinates );
        
        if( parser.read( &file ) ) {
            setCoordinates( coordinates );
            m_hasCoordinates = true;
        }
    }
}

bool PhotoPluginWidget::render( GeoPainter *painter, ViewportParams *viewport,
                                const QString& renderPos, GeoSceneLayer * layer )
{
    painter->drawImage( coordinates(), m_smallImage );
    return true;
}
             
bool PhotoPluginWidget::operator<( const AbstractDataPluginWidget *other ) const {
    return this->id() < other->id();
}

QUrl PhotoPluginWidget::photoUrl() const {
    QString url = "http://farm%1.static.flickr.com/%2/%3_%4_t.jpg";
    
    return QUrl( url.arg( farm() ).arg( server() ).arg( id() ).arg( secret() ) );
}

QUrl PhotoPluginWidget::infoUrl( QString apiKey ) const {
    QString url = "http://www.flickr.com/services/rest/?method=flickr.photos.geo.getLocation";
    url +=        "&id=";
    url +=        id();
    url +=        "&api_key=" + apiKey;
    
    qDebug() << "Returning " << url << " as infoUrl";
    return QUrl( url );
}

QString PhotoPluginWidget::server() const {
    return m_server;
}

void PhotoPluginWidget::setServer( QString server ) {
    m_server = server;
}

QString PhotoPluginWidget::farm() const {
    return m_farm;
}

void PhotoPluginWidget::setFarm( QString farm ) {
    m_farm = farm;
}

QString PhotoPluginWidget::secret() const {
    return m_secret;
}

void PhotoPluginWidget::setSecret( QString secret ) {
    m_secret = secret;
}

#include "PhotoPluginWidget.moc"
