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
#include "katlasdirs.h"


TinyWebBrowser::TinyWebBrowser( QWidget *parent )
    : QTextBrowser( parent )
{
    m_downloadManager = new HttpDownloadManager( QUrl("http://en.wikipedia.org/wiki/") );
    m_downloadManager->setTargetDir( KAtlasDirs::localDir() + "/cache/" );

    connect( m_downloadManager, SIGNAL( downloadComplete( QString, int ) ), 
        this, SLOT( slotDownloadFinished( QString, int ) ) );
    connect( m_downloadManager, SIGNAL( statusMessage( QString ) ), 
        this, SIGNAL( statusMessage( QString ) ) );
//    connect( m_downloadManager, SIGNAL( downloadDone( QString, bool ) ),
//             this,        SLOT( slotDownloadFinished( QString, bool ) ) );
//    connect( m_fetchFile, SIGNAL( statusMessage( QString ) ),
//             SIGNAL( statusMessage( QString) ) );		

    QStringList  searchPaths;
    searchPaths << KAtlasDirs::localDir() + "/cache/"
                << KAtlasDirs::systemDir() + "/cache/";
    setSearchPaths( searchPaths );
}


QVariant TinyWebBrowser::loadResource ( int type, const QUrl & name )
{
#if 0
    if ( type != QTextDocument::HtmlResource && !m_urlList.contains(name) ) {
        qDebug() << QString("Scheduling %1 for download.")
            .arg(QFileInfo(name.path()).fileName());
        m_fetchFile->downloadFile( name );
    }
#endif
    // if ( type == QTextDocument::HtmlResource )
    // {
    // }

    if ( type == QTextDocument::ImageResource ) {
        // && m_urlList.contains(name))
	//{
        QPixmap  emptyPixmap(1,1);
        emptyPixmap.fill( Qt::transparent );
        // m_urlList.append(name);

        return emptyPixmap;
    }

    // m_urlList.append(name);
    return QTextBrowser::loadResource( type, name );
}


void TinyWebBrowser::setSource( const QString& relativeUrl )
{
    m_source = QFileInfo( relativeUrl ).fileName();
    m_downloadManager->addJob( relativeUrl, /* id= */ 0 );
}


void TinyWebBrowser::slotDownloadFinished( const QString& filename, int )
{
    if ( filename == m_source )	{
        QTextBrowser::setSource( filename );

        QTextFrameFormat  format = document()->rootFrame()->frameFormat();
        format.setMargin(12);
        document()->rootFrame()->setFrameFormat( format );
    }
    else {
        qDebug( "Reload" );
        // reload();
        // m_urlList.removeAll();
    }
}


#include "tinywebbrowser.moc"
