//
// This file is part of the Marble Project.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


#include "TinyWebBrowser.h"

#include <QtCore/QFileInfo>
#include <QtCore/QUrl>
#include <QtCore/QDebug>
#include <QtGui/QPainter>
#include <QtGui/QPrintDialog>
#include <QtGui/QPrinter>
#include <QtGui/QTextFrame>

#include "HttpDownloadManager.h"
#include "CacheStoragePolicy.h"
#include "MarbleDirs.h"

static QString guessWikipediaDomain()
{
    const QString lang = qgetenv( "LANG" );
    QString code;

    if ( lang.isEmpty() || lang == "POSIX" || lang == "C" )
        code = "en";
    else {

        int index = lang.indexOf( '_' );
        if ( index != -1 ) {
            code = lang.left( index );
        } else {
            index = lang.indexOf( '@' );
            if ( index != -1 )
                code = lang.left( index );
            else
                code = lang;
        }
    }

    return QString( "http://%1.wikipedia.org/" ).arg( code );
}

TinyWebBrowser::TinyWebBrowser( QWidget *parent )
    : QTextBrowser( parent ),
      m_source( guessWikipediaDomain() )
{
    baseUrlForPanoramioQuery="http://www.panoramio.com/map/get_panoramas.php?";
    m_storagePolicy = new CacheStoragePolicy( MarbleDirs::localPath() + "/cache/" );

    m_downloadManager = new HttpDownloadManager( QUrl( guessWikipediaDomain() ),
                                                 m_storagePolicy );

    connect( m_downloadManager, SIGNAL( downloadComplete( QString, QString ) ), 
        this, SLOT( slotDownloadFinished( QString, QString ) ) );
    connect( m_downloadManager, SIGNAL( statusMessage( QString ) ), 
        this, SIGNAL( statusMessage( QString ) ) );
    connect( this, SIGNAL( anchorClicked( QUrl ) ),
        this, SLOT( linkClicked( QUrl ) ) );

    QStringList  searchPaths;
    searchPaths << MarbleDirs::localPath() + "/cache/"
                << MarbleDirs::systemPath() + "/cache/";
    setSearchPaths( searchPaths );

#if QT_VERSION >= 0x040300
    setOpenLinks( false );
#endif
    setOpenExternalLinks( false );
}

TinyWebBrowser::~TinyWebBrowser()
{
    delete m_downloadManager;
//    delete m_storagePolicy;
}

QVariant TinyWebBrowser::loadResource ( int type, const QUrl &url )
{
    QString server, relativeUrl;
    if ( url.scheme().isEmpty() ) {
      // We have something like 'img/foo.png'
      relativeUrl = url.toString();
    } else {
      server = url.scheme() + "://" + url.host();
      relativeUrl = url.path();
    }

    if ( type == QTextDocument::ImageResource ) {
        if ( m_storagePolicy->fileExists( relativeUrl ) ) {
            const QImage img = QImage::fromData( m_storagePolicy->data( relativeUrl ) );
            return QPixmap::fromImage( img );
        }
    } else if ( type == QTextDocument::StyleSheetResource && m_storagePolicy->fileExists( relativeUrl ) ) {
        return m_storagePolicy->data( relativeUrl );
    }

    if ( type != QTextDocument::HtmlResource && !m_storagePolicy->fileExists( relativeUrl ) ) {
        if ( server.isEmpty() ) {
            m_downloadManager->addJob( relativeUrl, relativeUrl );
        } else {
            m_downloadManager->addJob( server + '/' + relativeUrl, relativeUrl, relativeUrl );
        }
        return QVariant();
    }

    return QTextBrowser::loadResource( type, url );
}

void TinyWebBrowser::setSource( const QString& url )
{
    m_source = url;

    if ( !m_storagePolicy->fileExists( url ) )
        m_downloadManager->addJob( url, url );
    else
        slotDownloadFinished( url, url );

}

void TinyWebBrowser::print()
{
  QPrinter printer;

  QPrintDialog dlg( &printer, this );
  if ( dlg.exec() )
    QTextBrowser::document()->print( &printer );
}

void TinyWebBrowser::getPanoramio(const QString& place)//This function will get images from Panoramio and Show on the about place placemark
{
    qDebug()<<":::::::"<<place;    
    QString longitude = QString(place.at(1)) + QString(place.at(2));
    QString lattitude = QString(place.at(16)) + QString(place.at(17));
    int lon = longitude.toInt();
    int lat = lattitude.toInt();
   
    //constructing url for panoramio query api
    QString url = QString("order=popularity&set=public&from=0&to=20&")
            + QString( "minx=" )  + QString::number( lon - 1 ) 
            + QString( "&miny=" ) + QString::number( lat - 1 )
            + QString( "&maxx=" ) + QString::number( lon + 1 )
            + QString( "&maxy=" ) + QString::number( lat + 1 )
            + QString( "&size=medium");
    
    url = baseUrlForPanoramioQuery + url;//completing the Panoramio query urlString 
    m_source = url;

    if ( !m_storagePolicy->fileExists( "panoramio/" + place ) )//potential problem even after changing sorting result won't be updated as it won't be downlaoded
        m_downloadManager->addJob( url,"panoramio/" + place , "panoramio" );
    else
        slotDownloadFinished( url, url );

}

void TinyWebBrowser::slotDownloadFinished( const QString& relativeUrlString, const QString &id )
{
    if ( relativeUrlString == m_source )
        setContentHtml( QString::fromUtf8( m_storagePolicy->data( id ) ) );//The page is being converted to simple html 
    else
        viewport()->update();
}

void TinyWebBrowser::linkClicked( const QUrl &url )
{
    // We do the percent encoding later...
    QString urlString = QUrl::fromPercentEncoding( url.toString().toLatin1() );

    if ( urlString.startsWith( '#' ) ) { // Handle anchors
        scrollToAnchor( urlString.mid( 1 ) );
        return;
    } else if ( urlString.startsWith( '/' ) )
        urlString = urlString.mid( 1 ); // Handle local urls

    QString server, relativeUrl;
    if ( url.scheme().isEmpty() ) {
        // We have something like 'img/foo.png'
        relativeUrl = urlString;
    } else {
        server = url.scheme() + "://" + url.host();
        relativeUrl = url.path();
    }

    m_source = relativeUrl;
    if ( !m_storagePolicy->fileExists( relativeUrl ) ) {
        if ( server.isEmpty() ) {
            m_downloadManager->addJob( relativeUrl, relativeUrl );
        } else {
            m_downloadManager->addJob( server, relativeUrl, relativeUrl );
        }
    } else {
        setContentHtml( QString::fromUtf8( m_storagePolicy->data( relativeUrl ) ) );
    }
}

void TinyWebBrowser::setContentHtml( const QString &content )
{
    qDebug()<<":::::::::: iam in setcontenthtml"<<content;
    QString documentContent( content );
    if ( content.contains( "<html>" , Qt::CaseInsensitive ) == true  )//check for if input is html (i.e. wikipedia page )
    {
    // Remove JavaScript code as QTextBrowser can't display it anyways.
    QRegExp scriptExpression( "<\\s*script.*\\s*>.*<\\s*/\\s*script\\s*>" );

    scriptExpression.setCaseSensitivity( Qt::CaseInsensitive );
    scriptExpression.setMinimal( true );
    documentContent.remove( scriptExpression ) ;

    QTextBrowser::setHtml( documentContent );

    QTextFrameFormat format = document()->rootFrame()->frameFormat();
    format.setMargin( 12) ;
    document()->rootFrame()->setFrameFormat( format );
    }
 else
     parseJsonOutputFromPanoramio ( content );//the input is json so hand it over to json parser      
}
void TinyWebBrowser::parseJsonOutputFromPanoramio( const QString &content )//wil convert the parsed Json Output to html
{
   
   qDebug()<<":::::i am in tinywebbrowser's parseJsonOutputFromPanoramio";
   QList <panoramioDataStructure> parsedvalue = panoramioJsonParser.parse(content);
   QString html = QString("<html><image src=\"") 
           + QString(parsedvalue[0].photo_url) 
           + QString("\"></image></html>");
   QTextBrowser::setHtml( html );
   QTextFrameFormat format = document()->rootFrame()->frameFormat();
   format.setMargin( 12) ;
   document()->rootFrame()->setFrameFormat( format );      
}         
#include "TinyWebBrowser.moc"
