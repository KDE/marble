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

// Self
#include "TinyWebBrowser.h"

// Qt
#include <QtCore/QFileInfo>
#include <QtCore/QUrl>
#include <QtCore/QDebug>
#include <QtCore/QRegExp>
#include <QtGui/QPainter>
#include <QtGui/QPrintDialog>
#include <QtGui/QPrinter>
#include <QtGui/QTextFrame>

// Marble
#include "HttpDownloadManager.h"
#include "CacheStoragePolicy.h"
#include "MarbleDirs.h"
#include "MarbleLocale.h"

using namespace Marble;

static QString guessWikipediaDomain()
{
    const QString code = MarbleLocale::simpleLanguageCode();

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
  QPrinter printer;

  QPrintDialog dlg( &printer, this );
  if ( dlg.exec() )
    QWebView::print( &printer );
}

void TinyWebBrowser::openExternalLink( QUrl url )
{
    QDesktopServices::openUrl( url );
}

#include "TinyWebBrowser.moc"
