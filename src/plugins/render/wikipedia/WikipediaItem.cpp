//
// This file is part of the Marble Virtual Globe.
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
#include <QtGui/QPainter>
#include <QtGui/QPixmap>
#include <QtGui/QMouseEvent>
#include <QtWebKit/QWebView>

// Marble
#include "MarbleDebug.h"
#include "ViewportParams.h"
#include "TinyWebBrowser.h"
#include "MarbleWidget.h"
#include "MarbleModel.h"
#include "RenderPlugin.h"
#include "AbstractInfoDialog.h"
#include "PluginManager.h"

using namespace Marble;
/* TRANSLATOR Marble::WikipediaItem */

// The Wikipedia icon is not a square
const QRect wikiIconRect( 0, 0, 32, 27 );
const QSize miniWikiIconSize( 22, 19 );
const int miniWikiIconBorder = 3;

WikipediaItem::WikipediaItem( MarbleWidget* widget, QObject *parent )
    : AbstractDataPluginItem( parent ),
      m_marbleWidget( widget ),
      m_rank( 0.0 ),
      m_browser( 0 ),
      m_wikiIcon(),
      m_showThumbnail( false )
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
    WikipediaItem const * otherItem = dynamic_cast<WikipediaItem const *>( other );
    return otherItem ? m_rank > otherItem->m_rank : id() < other->id();
}
   
void WikipediaItem::paint( QPainter *painter )
{
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
    if ( m_marbleWidget ) {
        QList<RenderPlugin*> plugins = m_marbleWidget->renderPlugins();
        foreach( RenderPlugin* renderPlugin, plugins) {
            AbstractInfoDialog* infoDialog = dynamic_cast<AbstractInfoDialog*>( renderPlugin );
            if ( infoDialog ) {
                renderPlugin->setEnabled( true );
                renderPlugin->setVisible( true );
                if( !renderPlugin->isInitialized() ) {
                    renderPlugin->initialize();
                }
                infoDialog->setCoordinates( coordinate(), Qt::AlignTop | Qt::AlignHCenter );
                infoDialog->setSize( QSizeF( 500, 550 ) );
                infoDialog->setUrl( url() );
                return;
            }
        }

        mDebug() << "Unable to find a suitable render plugin for creating an info dialog";
    }

    if ( !m_browser ) {
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
    const bool showThumbnail = settings.value( "showThumbnails", false ).toBool();

    if ( showThumbnail != m_showThumbnail ) {
        m_showThumbnail = showThumbnail;
        updateSize();
        updateToolTip();
        update();
    }
}

void WikipediaItem::setRank( double rank )
{
    m_rank = rank;
}

double WikipediaItem::rank() const
{
    return m_rank;
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
    return m_showThumbnail && !m_thumbnail.isNull();
}

#include "WikipediaItem.moc"
