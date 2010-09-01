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
#include "WikipediaItem.h"

// Qt
#include <QtGui/QAction>
#include <QtGui/QIcon>
#include <QtGui/QPixmap>
#include <QtGui/QMouseEvent>
#include <QtWebKit/QWebView>

// Marble
#include "MarbleDebug.h"
#include "GeoPainter.h"
#include "ViewportParams.h"
#include "GeoSceneLayer.h"
#include "TinyWebBrowser.h"

using namespace Marble;

// The Wikipedia icon is not a square
const QRect wikiIconRect( 0, 0, 32, 27 );
const QSize miniWikiIconSize( 22, 19 );
const int miniWikiIconBorder = 3;

WikipediaItem::WikipediaItem( QObject *parent )
    : AbstractDataPluginItem( parent ),
      m_browser( 0 ),
      m_wikiIcon(),
      m_settings()
{
    m_action = new QAction( this );
    connect( m_action, SIGNAL( triggered() ), this, SLOT( openBrowser() ) );
    setCacheMode( MarbleGraphicsItem::ItemCoordinateCache );
}

WikipediaItem::~WikipediaItem()
{
    delete m_browser;
}

QString WikipediaItem::name() const
{
    return id();
}

void WikipediaItem::setName( const QString& name )
{
    setId( name );
    updateToolTip();
}

QString WikipediaItem::itemType() const
{
    return "wikipediaItem";
}
     
bool WikipediaItem::initialized()
{
    return true;
}
    
void WikipediaItem::addDownloadedFile( const QString& url, const QString& type )
{
    if ( type == "thumbnail" ) {
        m_thumbnail.load( url );
        updateSize();
        emit updated();
    }
}

bool WikipediaItem::operator<( const AbstractDataPluginItem *other ) const
{
    return this->id() < other->id();
}
   
void WikipediaItem::paint( GeoPainter *painter, ViewportParams *viewport,
                           const QString& renderPos, GeoSceneLayer * layer )
{
    Q_UNUSED( renderPos )
    Q_UNUSED( layer )
    Q_UNUSED( viewport )

    if ( !showThumbnail() ) {
        m_wikiIcon.paint( painter, wikiIconRect );
    }
    else {
        painter->drawPixmap( 0, 0, m_thumbnail );
        QSize minSize = miniWikiIconSize;
        minSize *= 2;
        QSize thumbnailSize = m_thumbnail.size();

        if ( thumbnailSize.width() >= minSize.width()
             && thumbnailSize.height() >= minSize.height() )
        {
            QRect wikiRect( QPoint( 0, 0 ), miniWikiIconSize );
            wikiRect.moveBottomRight( QPoint( m_thumbnail.width() - miniWikiIconBorder,
                                              m_thumbnail.height() - miniWikiIconBorder ) );
            m_wikiIcon.paint( painter, wikiRect );
        }
    }
}

qreal WikipediaItem::longitude()
{
    return coordinate().longitude();
}
    
void WikipediaItem::setLongitude( qreal longitude )
{
    GeoDataCoordinates updatedCoordinates = coordinate();
    updatedCoordinates.setLongitude( longitude );
    setCoordinate( updatedCoordinates );
}
    
qreal WikipediaItem::latitude()
{
    return coordinate().latitude();
}

void WikipediaItem::setLatitude( qreal latitude )
{
    GeoDataCoordinates updatedCoordinates = coordinate();
    updatedCoordinates.setLatitude( latitude );
    setCoordinate( updatedCoordinates );
}

QUrl WikipediaItem::url()
{
    return m_url;
}

void WikipediaItem::setUrl( const QUrl& url )
{
    m_url = url;
}

QUrl WikipediaItem::thumbnailImageUrl()
{
    return m_thumbnailImageUrl;
}

void WikipediaItem::setThumbnailImageUrl( const QUrl& thumbnailImageUrl )
{
    m_thumbnailImageUrl = thumbnailImageUrl;
}

QString WikipediaItem::summary()
{
    return m_summary;
}

void WikipediaItem::setSummary( const QString& summary )
{
    m_summary = summary;
    updateToolTip();
}

QAction *WikipediaItem::action()
{
    m_action->setText( id() );
    return m_action;
}

void WikipediaItem::openBrowser( )
{
    if( !m_browser ) {
        m_browser = new TinyWebBrowser();
    }
    m_browser->load( url() );
    m_browser->show();
}
    
void WikipediaItem::setIcon( const QIcon& icon )
{
    m_action->setIcon( icon );
    m_wikiIcon = icon;

    updateSize();
}

void WikipediaItem::setSettings( const QHash<QString, QVariant>& settings )
{
    if ( settings != m_settings ) {
        m_settings = settings;
        updateSize();
        updateToolTip();
        update();
    }
}

void WikipediaItem::updateSize()
{
    if ( showThumbnail() ) {
        setSize( m_thumbnail.size() );
    }
    else {
        setSize( wikiIconRect.size() );
    }
}

void WikipediaItem::updateToolTip()
{
    QString toolTip;
    toolTip += "<html><head><meta name=\"qrichtext\" content=\"1\" />";
    toolTip += "<style type=\"text/css\">\\np, li { white-space: pre-wrap; }\\n</style></head>";
    toolTip += "<body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; ";
    toolTip += "font-style:normal;\"><p style=\" margin-top:0px; margin-bottom:0px; ";
    toolTip += "margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">";
    if ( summary().isEmpty() ) {
        toolTip += "%1";
        toolTip += "</p></body></html>\n";
        setToolTip( toolTip.arg( name() ) );
    }
    else {
        toolTip += tr( "<b>%1</b><br>%2", "Title:\nSummary" );
        toolTip += "</p></body></html>\n";
        setToolTip( toolTip.arg( name() ).arg( summary() ) );
    }
}

bool WikipediaItem::showThumbnail()
{
    return m_settings.value( "showThumbnails", false ).toBool() && !m_thumbnail.isNull();
}

#include "WikipediaItem.moc"
