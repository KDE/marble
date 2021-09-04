//
// This file is part of the Marble Project.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2006-2007 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
// SPDX-FileCopyrightText: 2008 Simon Hausmann <hausmann@kde.org>
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
#include "MarbleWebView.h"

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
    : QWebEngineView( parent ),
      d( nullptr )
{
    MarbleWebPage * page = new MarbleWebPage();
    setPage(page);

    connect( this, SIGNAL(statusBarMessage(QString)),
             this, SIGNAL(statusMessage(QString)) );

    connect( page, SIGNAL(linkClicked(QUrl)),
             this, SLOT(openExternalLink(QUrl)) );
    connect( this, SIGNAL(titleChanged(QString)),
             this, SLOT(setWindowTitle(QString)) );

    pageAction( QWebEnginePage::OpenLinkInNewWindow )->setEnabled( false );
    pageAction( QWebEnginePage::OpenLinkInNewWindow )->setVisible( false );
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
        page()->print( &printer,  [=](bool){} );
    delete dlg;
#endif
}

QWebEngineView *TinyWebBrowser::createWindow( QWebEnginePage::WebWindowType type )
{
    Q_UNUSED(type)
    TinyWebBrowser *view = new TinyWebBrowser( this );
    return view;
}

void TinyWebBrowser::openExternalLink( const QUrl& url )
{
    QDesktopServices::openUrl( url );
}


} // namespace Marble

#include "moc_TinyWebBrowser.cpp"
