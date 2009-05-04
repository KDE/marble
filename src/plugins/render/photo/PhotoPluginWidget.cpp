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
#include "AbstractProjection.h"
#include "GeoDataCoordinates.h"
#include "GeoPainter.h"
#include "TinyWebBrowser.h"
#include "ViewportParams.h"

// Qt
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QUrl>
#include <QtGui/QMouseEvent>
#include <QtWebKit/QWebView>

using namespace Marble;

PhotoPluginWidget::PhotoPluginWidget( QObject *parent )
    : AbstractDataPluginWidget( parent ),
      m_hasCoordinates( false ),
      m_browser( 0 )
{
}

PhotoPluginWidget::~PhotoPluginWidget() {
    delete m_browser;
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
    GeoDataCoordinates coords = coordinates();
    painter->drawImage( coords, m_smallImage );
    qreal x[100], y;
    int pointRepeatNumber;
    bool globeHidesPoint;
    if( viewport->currentProjection()->screenCoordinates( coords,
                                                          viewport,
                                                          x, y,
                                                          pointRepeatNumber,
                                                          m_smallImage.size(),
                                                          globeHidesPoint ) )
    {
        // FIXME: We need to handle multiple coords here
        qint32 width = m_smallImage.width();
        qint32 height = m_smallImage.height();
        qint32 leftX = x[0] - ( m_smallImage.width()/2 );
        qint32 topY = y    - ( m_smallImage.height()/2 );
        
        if( leftX < 0 ) {
            width += leftX;
            leftX = 0;
        }
        if( topY < 0 ) {
            height += topY;
            topY = 0;
        }
            
        m_paintPosition.setRect( leftX, topY, width, height );
    }
    else {
        m_paintPosition = QRect();
    }
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
    url +=        "&photo_id=";
    url +=        id();
    url +=        "&api_key=" + apiKey;
    
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

QString PhotoPluginWidget::owner() const {
    return m_owner;
}
    
void PhotoPluginWidget::setOwner( QString owner ) {
    m_owner = owner;
}

bool PhotoPluginWidget::eventFilter( QObject *object, QMouseEvent *event ) {
    if( m_paintPosition.contains( event->pos() ) ) {
        if( m_browser ) {
            delete m_browser;
        }
        m_browser = new QWebView();
        QString url = "http://www.flickr.com/photos/%1/%2/";
        m_browser->load( QUrl( url.arg( owner() ).arg( id() ) ) );
        m_browser->show();
        connect( m_browser, SIGNAL( titleChanged(QString) ),
                 m_browser, SLOT( setWindowTitle(QString) ) );
        return true;
    }
    
    return false;
}

#include "PhotoPluginWidget.moc"
