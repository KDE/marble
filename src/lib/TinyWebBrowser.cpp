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
#include <QtGui/QTextFrame>

#include "HttpDownloadManager.h"
#include "CacheStoragePolicy.h"
#include "MarbleDirs.h"

static QString guessWikipediaDomain()
{
    const QString lang = getenv( "LANG" );
    QString code;

    if ( lang.isEmpty() )
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
    : QTextBrowser( parent )
{
    m_storagePolicy = new CacheStoragePolicy( MarbleDirs::localPath() + "/cache/" );

    m_downloadManager = new HttpDownloadManager( QUrl( guessWikipediaDomain() ),
                                                 m_storagePolicy );

    connect( m_downloadManager, SIGNAL( downloadComplete( QString, QString ) ), 
        this, SLOT( slotDownloadFinished( QString, QString ) ) );
    connect( m_downloadManager, SIGNAL( statusMessage( QString ) ), 
        this, SIGNAL( statusMessage( QString ) ) );

    QStringList  searchPaths;
    searchPaths << MarbleDirs::localPath() + "/cache/"
                << MarbleDirs::systemPath() + "/cache/";
    setSearchPaths( searchPaths );
}

TinyWebBrowser::~TinyWebBrowser()
{
    delete m_downloadManager;
    delete m_storagePolicy;
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

    if ( type == QTextDocument::ImageResource )
    {
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
          m_downloadManager->addJob( server, relativeUrl, relativeUrl );
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


void TinyWebBrowser::slotDownloadFinished( const QString& relativeUrlString, const QString &id )
{
    if ( relativeUrlString == m_source )	{
        QTextBrowser::setHtml( QString::fromUtf8( m_storagePolicy->data( id ) ) );

        QTextFrameFormat  format = document()->rootFrame()->frameFormat();
        format.setMargin(12);
        document()->rootFrame()->setFrameFormat( format );
    } else {
        /**
         * Evil hack to trigger page update
         */
        QSizeF size = document()->pageSize();
        size *= 2;
        document()->setPageSize( size );
        size /= 2;
        document()->setPageSize( size );
    }
}


#include "TinyWebBrowser.moc"
