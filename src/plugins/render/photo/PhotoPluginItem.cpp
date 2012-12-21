//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
// Copyright 2012      Mohammed Nafees <nafees.technocool@gmail.com>
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
#include "LabelGraphicsItem.h"
#include "MarbleGraphicsGridLayout.h"
#include "TinyWebBrowser.h"
#include "ViewportParams.h"
#include "MarbleDebug.h"
#include "MarbleWidget.h"
#include "MarbleModel.h"
#include "RenderPlugin.h"
#include "AbstractInfoDialog.h"
#include "PluginManager.h"

// Qt
#include <QtGui/QAction>
#include <QtGui/QIcon>
#include <QtCore/QFile>
#include <QtCore/QHash>
#include <QtCore/QUrl>
#include <QtGui/QMouseEvent>
#include <QtGui/QPixmap>

using namespace Marble;

PhotoPluginItem::PhotoPluginItem( MarbleWidget *widget, QObject *parent )
    : AbstractDataPluginItem( parent ),
      m_marbleWidget( widget ),
      m_image( this ),
      m_browser( 0 )
{
    m_action = new QAction( this );
    connect( m_action, SIGNAL( triggered() ), this, SLOT( openBrowser() ) );
    setCacheMode( MarbleGraphicsItem::ItemCoordinateCache );

    m_image.setFrame( FrameGraphicsItem::RectFrame );
    MarbleGraphicsGridLayout *layout = new MarbleGraphicsGridLayout( 1, 1 );
    layout->addItem( &m_image, 0, 0 );
    setLayout( layout );
}

PhotoPluginItem::~PhotoPluginItem()
{
    delete m_browser;
}

QString PhotoPluginItem::name() const
{
    return title();
}

QString PhotoPluginItem::itemType() const
{
    return QString( "photoItem" );
}
 
bool PhotoPluginItem::initialized()
{
    return !m_smallImage.isNull() && coordinate().isValid();
}

void PhotoPluginItem::addDownloadedFile( const QString& url, const QString& type )
{
    if( type == "thumbnail" ) {
        m_smallImage.load( url );
        m_image.setImage( m_smallImage );
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
        }
    }

    if ( initialized() ) {
        emit updated();
    }
}
             
bool PhotoPluginItem::operator<( const AbstractDataPluginItem *other ) const
{
    return this->id() < other->id();
}

QUrl PhotoPluginItem::photoUrl() const
{
    QString url = "http://farm%1.static.flickr.com/%2/%3_%4_t.jpg";
    
    return QUrl( url.arg( farm() ).arg( server() ).arg( id() ).arg( secret() ) );
}

QUrl PhotoPluginItem::infoUrl() const
{
    QHash<QString,QString> options;
    
    options.insert( "photo_id", id() );
    
    return PhotoPluginModel::generateUrl( "flickr", "flickr.photos.geo.getLocation", options );
}

QString PhotoPluginItem::server() const
{
    return m_server;
}

void PhotoPluginItem::setServer( const QString& server )
{
    m_server = server;
}

QString PhotoPluginItem::farm() const
{
    return m_farm;
}

void PhotoPluginItem::setFarm( const QString& farm )
{
    m_farm = farm;
}

QString PhotoPluginItem::secret() const
{
    return m_secret;
}

void PhotoPluginItem::setSecret( const QString& secret )
{
    m_secret = secret;
}

QString PhotoPluginItem::owner() const
{
    return m_owner;
}
    
void PhotoPluginItem::setOwner( const QString& owner )
{
    m_owner = owner;
}

QString PhotoPluginItem::title() const
{
    return m_title;
}

void PhotoPluginItem::setTitle( const QString& title )
{
    m_title = title;
    m_action->setText( title );
}

QAction *PhotoPluginItem::action()
{
    if( m_action->icon().isNull() ) {
        m_action->setIcon( QIcon( QPixmap::fromImage( m_smallImage ) ) );
    }
    return m_action;
}

void PhotoPluginItem::openBrowser()
{
    if ( m_marbleWidget ) {
         QList<RenderPlugin*> plugins = m_marbleWidget->renderPlugins();
         foreach( RenderPlugin* renderPlugin, plugins) {

            AbstractInfoDialog* infoDialog = dynamic_cast<AbstractInfoDialog*>( renderPlugin );
            if ( infoDialog ) {
                renderPlugin->setEnabled( true );
                renderPlugin->setVisible( true );
                Q_ASSERT( renderPlugin->isInitialized() );
                if( !renderPlugin->isInitialized() ) {
                    renderPlugin->initialize();
                }
                infoDialog->setCoordinates( coordinate(), Qt::AlignRight | Qt::AlignVCenter );
                infoDialog->setSize( QSizeF( 400, 450 ) );
                infoDialog->setUrl( QUrl( QString( "http://m.flickr.com/photos/%1/%2/" )
                                          .arg( owner() ).arg( id() ) ) );
                return;
            }
         }
         mDebug() << "Unable to find a suitable render plugin for creating an info dialog";
    }

    if( !m_browser ) {
        m_browser = new TinyWebBrowser();
    }

    QString url = "http://www.flickr.com/photos/%1/%2/";
    m_browser->load( QUrl( url.arg( owner() ).arg( id() ) ) );
    m_browser->show();
}

#include "PhotoPluginItem.moc"
