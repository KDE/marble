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
#include <QtCore/QRegExp>
#include <QtGui/QPainter>
#include <QtGui/QPrintDialog>
#include <QtGui/QPrinter>
#include <QtGui/QTextFrame>
#include "HttpDownloadManager.h"
#include "CacheStoragePolicy.h"
#include "MarbleDirs.h"

static QString guessWikipediaDomain()
{
    const QString lang = qgetenv ( "LANG" );
    QString code;

    if ( lang.isEmpty() || lang == "POSIX" || lang == "C" )
        code = "en";
    else
    {

        int index = lang.indexOf ( '_' );
        if ( index != -1 )
        {
            code = lang.left ( index );
        }
        else
        {
            index = lang.indexOf ( '@' );
            if ( index != -1 )
                code = lang.left ( index );
            else
                code = lang;
        }
    }

    return QString ( "http://%1.wikipedia.org/" ).arg ( code );
}

TinyWebBrowser::TinyWebBrowser ( QWidget *parent )
        : QTextBrowser ( parent ),
        m_source ( guessWikipediaDomain() )
{
    baseUrlForPanoramioQuery="http://www.panoramio.com/map/get_panoramas.php?";

    m_storagePolicy = new CacheStoragePolicy ( MarbleDirs::localPath() + "/cache/" );

    m_downloadManager = new HttpDownloadManager ( QUrl ( guessWikipediaDomain() ),
            m_storagePolicy );

    connect ( m_downloadManager, SIGNAL ( downloadComplete ( QString, QString ) ),
              this, SLOT ( slotDownloadFinished ( QString, QString ) ) );

    connect ( m_downloadManager, SIGNAL ( statusMessage ( QString ) ),
              this, SIGNAL ( statusMessage ( QString ) ) );

    connect ( this, SIGNAL ( anchorClicked ( QUrl ) ),
              this, SLOT ( linkClicked ( QUrl ) ) );

    QStringList  searchPaths;
    searchPaths << MarbleDirs::localPath() + "/cache/"
    << MarbleDirs::systemPath() + "/cache/";
    setSearchPaths ( searchPaths );

#if QT_VERSION >= 0x040300
    setOpenLinks ( false );
#endif
    setOpenExternalLinks ( false );
}

TinyWebBrowser::~TinyWebBrowser()
{
    delete m_downloadManager;
//    delete m_storagePolicy;
}

QVariant TinyWebBrowser::loadResource ( int type, const QUrl &url )
{
    QString server, relativeUrl;
    if ( url.scheme().isEmpty() )
    {
        // We have something like 'img/foo.png' [it's Relative URl as scheme is empty]
        relativeUrl = url.toString();
    }
    else
    {
        server = url.scheme() + "://" + url.host();
        relativeUrl = url.path();
    }

    if ( type == QTextDocument::ImageResource )
    {
        if ( m_storagePolicy->fileExists ( relativeUrl ) )
        {
            const QImage img = QImage::fromData ( m_storagePolicy->data ( relativeUrl ) );
            return QPixmap::fromImage ( img );
        }
    }
    else if ( type == QTextDocument::StyleSheetResource && m_storagePolicy->fileExists ( relativeUrl ) )
    {
        return m_storagePolicy->data ( relativeUrl );
    }

    if ( type != QTextDocument::HtmlResource && !m_storagePolicy->fileExists ( relativeUrl ) )
    {
        if ( server.isEmpty() )
        {
            m_downloadManager->addJob ( relativeUrl, relativeUrl );
        }
        else
        {
            m_downloadManager->addJob ( server + '/' + relativeUrl, relativeUrl, relativeUrl );
        }
        return QVariant();
    }

    return QTextBrowser::loadResource ( type, url );
}

void TinyWebBrowser::setSource ( const QUrl& url )
{
    m_source = url.toString();

    if ( !m_storagePolicy->fileExists ( m_source ) )
        m_downloadManager->addJob ( m_source, m_source );
    else
        slotDownloadFinished ( m_source, m_source );

}

void TinyWebBrowser::print()
{
    QPrinter printer;

    QPrintDialog dlg ( &printer, this );
    if ( dlg.exec() )
        QTextBrowser::document()->print ( &printer );
}

void TinyWebBrowser::getPanoramio ( const QString& place ) //This function will get images from Panoramio and Show on the about place placemark
{

    qDebug() <<"place=="<<place;
    QRegExp extractLatLon ( "\\s" );//extract all the whitespace
    QStringList latlon = place.split ( extractLatLon, QString::SkipEmptyParts );
    bool ok;

    int lon = latlon[0].toUInt ( &ok , 10 );//first subtext FIXME toInt is not working
    int lat = latlon[3].toUInt();//fourth subtext
    qDebug() <<ok;
    qDebug() <<lon;
    qDebug() <<"latlon==::::::::::::::::::"<<latlon<<lon<<lat;
    //constructing url for panoramio query api
    QString url = QString ( "order=popularity&set=public&from=0&to=20" )
                  + QString ( "&minx=" ) + QString::number ( lon - 1 )
                  + QString ( "&miny=" ) + QString::number ( lat - 1 )
                  + QString ( "&maxx=" ) + QString::number ( lon + 1 )
                  + QString ( "&maxy=" ) + QString::number ( lat + 1 )
                  + QString ( "&size=medium" );
//                   testing();
    url = baseUrlForPanoramioQuery + url;//completing the Panoramio query urlString

//     url="http://www.panoramio.com/map/get_panoramas.php?from=0&to=20&minx=-180&miny=-90&maxx=180&maxy=90";
//     url="http://twitter.com/statuses/public_timeline.json?since_id=12345";
    m_source = url;
    testing ( url );//remove me
    m_downloadManager->addJob ( QUrl ( url ),place,place );
//     slotDownloadFinished ( place,place );

//     slotPanoramioDownloadFinished ( place,place );
}
void TinyWebBrowser::testing ( QString url )
{
    qDebug() <<"::::::Job Added";
}

void TinyWebBrowser::slotDownloadFinished ( const QString& relativeUrlString, const QString &id )
{
    if ( relativeUrlString == m_source )
        setContentHtml ( QString::fromUtf8 ( m_storagePolicy->data ( id ) ) );//The page is being converted to simple html
    else
        viewport()->update();
}

void TinyWebBrowser::slotPanoramioDownloadFinished ( const QString& urlString, const QString &id )
{
//     if ( urlString == m_source )
    parseJsonOutputFromPanoramio ( QString::fromUtf8 ( m_storagePolicy->data ( id ) ) );//The page is being converted to simple html
//     else
//         viewport()->update();
}

void TinyWebBrowser::linkClicked ( const QUrl &url )
{
    // We do the percent encoding later...
    QString urlString = QUrl::fromPercentEncoding ( url.toString().toLatin1() );

    if ( urlString.startsWith ( '#' ) )  // Handle anchors
    {
        scrollToAnchor ( urlString.mid ( 1 ) );
        return;
    }
    else if ( urlString.startsWith ( '/' ) )
        urlString = urlString.mid ( 1 ); // Handle local urls

    QString server, relativeUrl;
    if ( url.scheme().isEmpty() )
    {
        // We have something like 'img/foo.png'
        relativeUrl = urlString;
    }
    else
    {
        server = url.scheme() + "://" + url.host();
        relativeUrl = url.path();
    }

    m_source = relativeUrl;
    if ( !m_storagePolicy->fileExists ( relativeUrl ) )
    {
        if ( server.isEmpty() )
        {
            m_downloadManager->addJob ( relativeUrl, relativeUrl );
        }
        else
        {
            m_downloadManager->addJob ( server, relativeUrl, relativeUrl );
        }
    }
    else
    {
        setContentHtml ( QString::fromUtf8 ( m_storagePolicy->data ( relativeUrl ) ) );
    }
}

void TinyWebBrowser::setContentHtml ( const QString &content )
{
    QString documentContent ( content );

    // Remove JavaScript code as QTextBrowser can't display it anyways.
    QRegExp scriptExpression ( "<\\s*script.*\\s*>.*<\\s*/\\s*script\\s*>" );

    scriptExpression.setCaseSensitivity ( Qt::CaseInsensitive );
    scriptExpression.setMinimal ( true );
    documentContent.remove ( scriptExpression ) ;

    QTextBrowser::setHtml ( documentContent );

    QTextFrameFormat format = document()->rootFrame()->frameFormat();
    format.setMargin ( 12 ) ;
    document()->rootFrame()->setFrameFormat ( format );
}
void TinyWebBrowser::parseJsonOutputFromPanoramio ( const QString &content ) //wil convert the parsed Json Output to html
{
    QList <panoramioDataStructure> parsedvalue = panoramioJsonParser.parseAllObjects ( content , 20 );
    QString html = QString ( "<html><h1>hi</h1><image src=\"" )
                   + QString ( parsedvalue[0].photo_file_url )
    + QString ( "\"></html>" );
    QTextBrowser::setHtml ( html );
    QTextFrameFormat format = document()->rootFrame()->frameFormat();
    format.setMargin ( 12 ) ;
    document()->rootFrame()->setFrameFormat ( format );
    qDebug() <<html<<"i am in parsed section";

}
#include "TinyWebBrowser.moc"
