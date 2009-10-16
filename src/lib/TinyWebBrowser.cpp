//
// This file is part of the Marble Project.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
// Copyright 2008      Simon Hausmann  <hausmann@kde.org>"
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

    return QString ( "http://%1.wikipedia.org/" ).arg ( code );
}

TinyWebBrowser::TinyWebBrowser( QWidget* parent )
    : QWebView( parent )
{
    connect( this, SIGNAL( statusBarMessage( QString ) ),
             this, SIGNAL( statusMessage( QString ) ) );

    page()->setLinkDelegationPolicy( QWebPage::DelegateAllLinks );
    connect( this, SIGNAL( linkClicked( QUrl ) ),
             this, SLOT( openExternalLink( QUrl ) ) );
}

TinyWebBrowser::~TinyWebBrowser()
{
}

void TinyWebBrowser::setSource( const QString& relativeUrl )
{
    QUrl url = relativeUrl;
    if ( url.isRelative() )
        url = QUrl( guessWikipediaDomain() ).resolved( url );
    load( url );
}

void TinyWebBrowser::print()
{
#ifndef QT_NO_PRINTER	
  QPrinter printer;

  QPrintDialog dlg( &printer, this );
  if ( dlg.exec() )
    QWebView::print( &printer );
#endif
}

void TinyWebBrowser::openExternalLink( QUrl url )
{
    QDesktopServices::openUrl( url );
}

#include "TinyWebBrowser.moc"
