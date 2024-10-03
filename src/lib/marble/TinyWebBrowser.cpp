// SPDX-FileCopyrightText: 2006-2007 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
// SPDX-FileCopyrightText: 2008 Simon Hausmann <hausmann@kde.org>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

// Self
#include "TinyWebBrowser.h"

// Qt
#include <QAction>
#include <QDesktopServices>
#include <QPainter>
#include <QPointer>
#include <QPrintDialog>
#include <QPrinter>

// Marble
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "MarbleGlobal.h"
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

    return QLatin1StringView("https://") + code + QLatin1StringView(".m.wikipedia.org/");
}

TinyWebBrowser::TinyWebBrowser(QWidget *parent)
    : QWebEngineView(parent)
    , d(nullptr)
{
    auto page = new MarbleWebPage();
    setPage(page);

    connect(this, SIGNAL(statusBarMessage(QString)), this, SIGNAL(statusMessage(QString)));

    connect(page, &MarbleWebPage::linkClicked, this, &TinyWebBrowser::openExternalLink);
    connect(this, &QWebEngineView::titleChanged, this, &QWidget::setWindowTitle);

    pageAction(QWebEnginePage::OpenLinkInNewWindow)->setEnabled(false);
    pageAction(QWebEnginePage::OpenLinkInNewWindow)->setVisible(false);
}

TinyWebBrowser::~TinyWebBrowser()
{
    // not yet needed
    // delete d;
}

void TinyWebBrowser::setWikipediaPath(const QString &relativeUrl)
{
    QUrl url(relativeUrl);
    if (url.isRelative())
        url = QUrl(guessWikipediaDomain()).resolved(url);
    load(url);
}

void TinyWebBrowser::print()
{
#ifndef QT_NO_PRINTER
    QPrinter printer;

    QPointer<QPrintDialog> dlg = new QPrintDialog(&printer, this);
    if (dlg->exec())
        QWebEngineView::print(&printer);
    delete dlg;
#endif
}

QWebEngineView *TinyWebBrowser::createWindow(QWebEnginePage::WebWindowType type)
{
    Q_UNUSED(type)
    auto view = new TinyWebBrowser(this);
    return view;
}

void TinyWebBrowser::openExternalLink(const QUrl &url)
{
    QDesktopServices::openUrl(url);
}

} // namespace Marble

#include "moc_TinyWebBrowser.cpp"
