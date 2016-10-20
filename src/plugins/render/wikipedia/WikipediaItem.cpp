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
#include <QAction>
#include <QPainter>

// Marble
#include "MarbleDebug.h"
#include "ViewportParams.h"
#include "TinyWebBrowser.h"
#include "MarbleWidget.h"
#include "MarbleModel.h"
#include "RenderPlugin.h"
#include "PluginManager.h"
#include "layers/PopupLayer.h"

using namespace Marble;
/* TRANSLATOR Marble::WikipediaItem */

// The Wikipedia icon is not a square
const QRect wikiIconRect( 0, 0, 22, 19 );
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
    connect( m_action, SIGNAL(triggered()), this, SLOT(openBrowser()) );
    setCacheMode( ItemCoordinateCache );
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

bool WikipediaItem::initialized() const
{
    return true;
}
    
void WikipediaItem::addDownloadedFile( const QString& url, const QString& type )
{
    if (type == QLatin1String("thumbnail")) {
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

qreal WikipediaItem::longitude() const
{
    return coordinate().longitude();
}
    
void WikipediaItem::setLongitude( qreal longitude )
{
    GeoDataCoordinates updatedCoordinates = coordinate();
    updatedCoordinates.setLongitude( longitude );
    setCoordinate( updatedCoordinates );
}
    
qreal WikipediaItem::latitude() const
{
    return coordinate().latitude();
}

void WikipediaItem::setLatitude( qreal latitude )
{
    GeoDataCoordinates updatedCoordinates = coordinate();
    updatedCoordinates.setLatitude( latitude );
    setCoordinate( updatedCoordinates );
}

QUrl WikipediaItem::url() const
{
    return m_url;
}

void WikipediaItem::setUrl( const QUrl& url )
{
    m_url = url;
}

QUrl WikipediaItem::thumbnailImageUrl() const
{
    return m_thumbnailImageUrl;
}

void WikipediaItem::setThumbnailImageUrl( const QUrl& thumbnailImageUrl )
{
    m_thumbnailImageUrl = thumbnailImageUrl;
}

QString WikipediaItem::summary() const
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
        PopupLayer* popup = m_marbleWidget->popupLayer();
        popup->setCoordinates( coordinate(), Qt::AlignRight | Qt::AlignVCenter );
        popup->setSize(QSizeF(520, 570));
        popup->setUrl( url() );
        popup->popup();
    } else {
        if ( !m_browser ) {
            m_browser = new TinyWebBrowser();
        }
        m_browser->load( url() );
        m_browser->show();
    }
}
    
void WikipediaItem::setIcon( const QIcon& icon )
{
    m_action->setIcon( icon );
    m_wikiIcon = icon;

    updateSize();
}

void WikipediaItem::setSettings( const QHash<QString, QVariant>& settings )
{
    const bool showThumbnail = settings.value(QStringLiteral("showThumbnails"), false).toBool();

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
    QString toolTip = QLatin1String(
        "<html><head><meta name=\"qrichtext\" content=\"1\" />"
        "<style type=\"text/css\">\\np, li { white-space: pre-wrap; }\\n</style></head>"
        "<body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; "
        "font-style:normal;\"><p style=\" margin-top:0px; margin-bottom:0px; "
        "margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">");
    if ( summary().isEmpty() ) {
        toolTip += name() + QLatin1String("</p></body></html>\n");
        setToolTip(toolTip);
    }
    else {
        toolTip += tr("<b>%1</b><br>%2", "Title:\nSummary") + QLatin1String("</p></body></html>\n");
        setToolTip( toolTip.arg( name() ).arg( summary() ) );
    }
}

bool WikipediaItem::showThumbnail() const
{
    return m_showThumbnail && !m_thumbnail.isNull();
}

#include "moc_WikipediaItem.cpp"
