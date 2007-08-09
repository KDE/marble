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


#include "tinywebbrowser.h"

#include <QtCore/QFileInfo>
#include <QtCore/QUrl>
#include <QtCore/QDebug>
#include <QtGui/QTextFrame>

#include "HttpDownloadManager.h"
#include "MarbleDirs.h"


TinyWebBrowser::TinyWebBrowser( QWidget *parent )
    : QTextBrowser( parent )
{
    m_downloadManager = new HttpDownloadManager( QUrl("http://en.wikipedia.org/") );
    m_downloadManager->setTargetDir( MarbleDirs::localDir() + "/cache/" );

    connect( m_downloadManager, SIGNAL( downloadComplete( QString, int ) ), 
        this, SLOT( slotDownloadFinished( QString, int ) ) );
    connect( m_downloadManager, SIGNAL( statusMessage( QString ) ), 
        this, SIGNAL( statusMessage( QString ) ) );
//    connect( m_downloadManager, SIGNAL( downloadDone( QString, bool ) ),
//             this,        SLOT( slotDownloadFinished( QString, bool ) ) );
//    connect( m_fetchFile, SIGNAL( statusMessage( QString ) ),
//             SIGNAL( statusMessage( QString) ) );		

    QStringList  searchPaths;
    searchPaths << MarbleDirs::localDir() + "/cache/"
                << MarbleDirs::systemDir() + "/cache/";
    setSearchPaths( searchPaths );
}


QVariant TinyWebBrowser::loadResource ( int type, const QUrl & relativeUrl )
{
    QString relativeUrlString = relativeUrl.toString();

    if (relativeUrlString.startsWith( '/' ) )
        relativeUrlString = relativeUrlString.section( '/', 1, -1 );

    qDebug() << "loadResource: " << relativeUrlString;

    if ( type != QTextDocument::HtmlResource && !m_urlList.contains(relativeUrlString) ) {
        qDebug() << QString("Scheduling %1 for download.")
            .arg(relativeUrlString);
        m_downloadManager->addJob( relativeUrlString, /* id= */ 0 );
    }

    if ( type == QTextDocument::ImageResource && !m_urlList.contains( relativeUrlString ) )
    {
        QPixmap  emptyPixmap(1,1);
        emptyPixmap.fill( Qt::transparent );

        return emptyPixmap;
    }

    return QTextBrowser::loadResource( type, QUrl( relativeUrlString ) );
}


void TinyWebBrowser::setSource( const QString& relativeUrlString )
{
    m_source = relativeUrlString;
    m_downloadManager->addJob( relativeUrlString, /* id= */ 0 );
}


void TinyWebBrowser::slotDownloadFinished( const QString& relativeUrlString, int )
{
    qDebug() << "downloadFinished" << relativeUrlString;
//    if ( !m_urlList.contains( relativeUrlString ) )
//        m_urlList.append( relativeUrlString );

    if ( relativeUrlString == m_source )	{
        QTextBrowser::setSource( relativeUrlString );

        QTextFrameFormat  format = document()->rootFrame()->frameFormat();
        format.setMargin(12);
        document()->rootFrame()->setFrameFormat( format );
    }
    else {
        qDebug( "Reload" );
//        QTextBrowser::setSource( m_source );
        // m_urlList.removeAll();
    }
}


#include "tinywebbrowser.moc"
