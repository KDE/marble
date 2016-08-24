//
// This file is part of the Marble Project.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2008      Simon Hausmann  <hausmann@kde.org>
//

// Self
#include "TinyWebBrowser.h"

// Qt
#include <QPointer>
#include <QAction>
#include <QDesktopServices>
#include <QPainter>
#include <QPrintDialog>
#include <QPrinter>

// Marble
#include "MarbleGlobal.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "MarbleLocale.h"

namespace Marble
{

class TinyWebBrowserPrivate
{
};

static QString guessWikipediaDomain()
{
    const QString code = MarbleLocale::languageCode();

    return QLatin1String("https://") + code + QLatin1String(".m.wikipedia.org/");
}

TinyWebBrowser::TinyWebBrowser( QWidget* parent )
    : QWebView( parent ),
      d( 0 )
{
    connect( this, SIGNAL(statusBarMessage(QString)),
             this, SIGNAL(statusMessage(QString)) );

    page()->setLinkDelegationPolicy( QWebPage::DelegateAllLinks );
    connect( this, SIGNAL(linkClicked(QUrl)),
             this, SLOT(openExternalLink(QUrl)) );
    connect( this, SIGNAL(titleChanged(QString)),
             this, SLOT(setWindowTitle(QString)) );

    pageAction( QWebPage::OpenLinkInNewWindow )->setEnabled( false );
    pageAction( QWebPage::OpenLinkInNewWindow )->setVisible( false );
}

TinyWebBrowser::~TinyWebBrowser()
{
    // not yet needed
    //delete d;
}

void TinyWebBrowser::setWikipediaPath( const QString& relativeUrl )
{
    QUrl url(relativeUrl);
    if ( url.isRelative() )
        url = QUrl( guessWikipediaDomain() ).resolved( url );
    load( url );
}

void TinyWebBrowser::print()
{
#ifndef QT_NO_PRINTER	
    QPrinter printer;

    QPointer<QPrintDialog> dlg = new QPrintDialog( &printer, this );
    if ( dlg->exec() )
        QWebView::print( &printer );
    delete dlg;
#endif
}

QWebView *TinyWebBrowser::createWindow( QWebPage::WebWindowType type )
{
    TinyWebBrowser *view = new TinyWebBrowser( this );

    if ( type == QWebPage::WebModalDialog ) {
        view->setWindowModality( Qt::WindowModal );
    }

    return view;
}

void TinyWebBrowser::openExternalLink( const QUrl& url )
{
    QDesktopServices::openUrl( url );
}


} // namespace Marble

#include "moc_TinyWebBrowser.cpp"
