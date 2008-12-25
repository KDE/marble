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

using namespace Marble;

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

    return  code;
}

TinyWebBrowser::TinyWebBrowser ( QWidget *parent )
        : QTextBrowser ( parent ),
        m_source ( guessWikipediaDomain() )
{

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

void TinyWebBrowser::setSource ( const QString& urlString )
{
    m_source = urlString;
    m_source.remove(QRegExp("[Â|°]"));//this will remove the spurious Â° ,,thats creeping in urlString
    QStringList localSplittedLatLon = m_source.split(QRegExp("(\\s|,|E|N)"), QString::SkipEmptyParts) ;
    m_downloadManager->addJob(QUrl(QString("http://ws.geonames.org/")
                                   + QString("findNearbyWikipediaJSON?radius=40&maxRows=10&lang=")
                                   + guessWikipediaDomain()
                                   + QString("&lat=") + localSplittedLatLon[1]
                                   + QString("&lng=") + localSplittedLatLon[0]) , "wikia", "wikia");
}

void TinyWebBrowser::print()
{
    QPrinter printer;

    QPrintDialog dlg ( &printer, this );
    if ( dlg.exec() )
        QTextBrowser::document()->print ( &printer );
}

void TinyWebBrowser::slotDownloadFinished ( const QString& relativeUrlString, const QString &id )
{
    int localCounter;
    localStorageOfParsedOutput = wikipediaJsonParser.parseAllObjects(QString::fromUtf8(m_storagePolicy->data(id)));
    QString localCitySummary;
    if (relativeUrlString == "wikia") 
    {
        QString htmlContent;
        if (localStorageOfParsedOutput.empty() == true) 
        {
            htmlContent = "<h1>No Wikipedia article Found in this Region</h1>";
        } 
        else 
        {
            for (localCounter = 0 ; localCounter < localStorageOfParsedOutput[0].numberOfObjects ; localCounter++)
            {
            
            if ( localStorageOfParsedOutput[localCounter].feature == "city" )
            {
            /*qDebug() << "citydistance" << localStorageOfParsedOutput [ localCounter ] . distance << localStorageOfParsedOutput [ localCounter ] . title ;*/
            localCitySummary += "<p> <a href =\"http://"
                                + localStorageOfParsedOutput[localCounter].wikipediaUrl
                                + "\"><b>"
                                + localStorageOfParsedOutput[localCounter].title + "</b></a><br><b>Summary:</b>"
                                + localStorageOfParsedOutput[localCounter].summary
                                + "</p>";
            }
            else 
          {
          /*qDebug() << "distance" <<localStorageOfParsedOutput [ localCounter ] . distance << localStorageOfParsedOutput [ localCounter ] . title ;*/
             htmlContent +=  "<p> <a href =\"http://"
                                + localStorageOfParsedOutput[localCounter].wikipediaUrl
                                + "\"><b>"
                                + localStorageOfParsedOutput[localCounter].title + "</b></a><br><b>Summary:</b>"
                                + localStorageOfParsedOutput[localCounter].summary
                                + "</p>";
            }
            }
            
            htmlContent = "<h1>Wikipedia articles from this Region</h1> " 
                            +localCitySummary
                            +htmlContent;
        }
        htmlContent += "<br><i>Provided under cc-by licence (creative commons attributions license) By GeoNames</i>"     ;  //The page is being converted to simple html
        setContentHtml(htmlContent);
    } 
    else
        viewport()->update();
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
#include "TinyWebBrowser.moc"
