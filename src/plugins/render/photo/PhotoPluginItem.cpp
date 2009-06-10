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
#include "PhotoPluginItem.h"

// Plugin
#include "CoordinatesParser.h"
#include "PhotoPluginModel.h"

// Marble
#include "AbstractDataPluginItem.h"
#include "GeoDataCoordinates.h"
#include "GeoPainter.h"
#include "TinyWebBrowser.h"
#include "ViewportParams.h"

// Qt
#include <QtGui/QAction>
#include <QtGui/QIcon>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QHash>
#include <QtCore/QUrl>
#include <QtGui/QMouseEvent>
#include <QtWebKit/QWebView>

using namespace Marble;

PhotoPluginItem::PhotoPluginItem( QObject *parent )
    : AbstractDataPluginItem( parent ),
      m_hasCoordinates( false ),
      m_browser( 0 )
{
    m_action = new QAction( this );
    connect( m_action, SIGNAL( triggered() ), this, SLOT( openBrowser() ) );
}

PhotoPluginItem::~PhotoPluginItem() {
    delete m_browser;
}

QString PhotoPluginItem::name() const {
    return title();
}

QString PhotoPluginItem::itemType() const {
    return QString( "photoItem" );
}
 
bool PhotoPluginItem::initialized() {
    return !m_smallImage.isNull() && m_hasCoordinates;
}

void PhotoPluginItem::addDownloadedFile( const QString& url, const QString& type ) {
    if( type == "thumbnail" ) {
        m_smallImage.load( url );
        setSize( m_smallImage.size() );
    }
    else if ( type == "info" ) {        
        QFile file( url );
        if( !file.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
            return;
        }
        
        GeoDataCoordinates coordinates;
        CoordinatesParser parser( &coordinates );
        
        if( parser.read( &file ) ) {
            setCoordinate( coordinates );
            m_hasCoordinates = true;
        }
    }
}

void PhotoPluginItem::paint( GeoPainter *painter, ViewportParams *viewport,
                             const QString& renderPos, GeoSceneLayer * layer )
{
    Q_UNUSED( renderPos )
    Q_UNUSED( layer )

    painter->drawPixmap( 0, 0, m_smallImage );
}
             
bool PhotoPluginItem::operator<( const AbstractDataPluginItem *other ) const {
    return this->id() < other->id();
}

QUrl PhotoPluginItem::photoUrl() const {
    QString url = "http://farm%1.static.flickr.com/%2/%3_%4_t.jpg";
    
    return QUrl( url.arg( farm() ).arg( server() ).arg( id() ).arg( secret() ) );
}

QUrl PhotoPluginItem::infoUrl() const {
    QHash<QString,QString> options;
    
    options.insert( "photo_id", id() );
    
    return PhotoPluginModel::generateUrl( "flickr", "flickr.photos.geo.getLocation", options );
}

QString PhotoPluginItem::server() const {
    return m_server;
}

void PhotoPluginItem::setServer( const QString& server ) {
    m_server = server;
}

QString PhotoPluginItem::farm() const {
    return m_farm;
}

void PhotoPluginItem::setFarm( const QString& farm ) {
    m_farm = farm;
}

QString PhotoPluginItem::secret() const {
    return m_secret;
}

void PhotoPluginItem::setSecret( const QString& secret ) {
    m_secret = secret;
}

QString PhotoPluginItem::owner() const {
    return m_owner;
}
    
void PhotoPluginItem::setOwner( const QString& owner ) {
    m_owner = owner;
}

QString PhotoPluginItem::title() const {
    return m_title;
}

void PhotoPluginItem::setTitle( const QString& title ) {
    m_title = title;
    m_action->setText( title );
}

QAction *PhotoPluginItem::action() {
    if( m_action->icon().isNull() ) {
        m_action->setIcon( QIcon( m_smallImage ) );
    }
    return m_action;
}

void PhotoPluginItem::openBrowser( ) {
    if( m_browser ) {
        delete m_browser;
    }
    m_browser = new QWebView();
    QString url = "http://www.flickr.com/photos/%1/%2/";
    m_browser->load( QUrl( url.arg( owner() ).arg( id() ) ) );
    m_browser->show();
    connect( m_browser, SIGNAL( titleChanged(QString) ),
             m_browser, SLOT( setWindowTitle(QString) ) );
}

#include "PhotoPluginItem.moc"
