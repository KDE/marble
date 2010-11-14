//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//

#ifndef MARBLE_TINYWEBBROWSER_H
#define MARBLE_TINYWEBBROWSER_H

// Qt
#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtWebKit/QWebView>

// Marble
#include "marble_export.h"

namespace Marble
{

class TinyWebBrowserPrivate;

/**
 * This class provides a tiny web browser based on QWebView (WebKit).
 * It is different from QWebView as it has the button "Open in new Window"
 * disabled per default and instead opens every link in the default web
 * browser of the user.
 */
class MARBLE_EXPORT TinyWebBrowser : public QWebView
{
    Q_OBJECT

 public:
    explicit TinyWebBrowser( QWidget* parent = 0 );
    ~TinyWebBrowser();

    static QByteArray userAgent(const QString &platform, const QString &plugin);

 public Q_SLOTS:
    void setWikipediaPath( const QString& relativeUrl );
    void print();

 Q_SIGNALS:
    void statusMessage( QString );

 protected:
    QWebView *createWindow( QWebPage::WebWindowType type );

 private Q_SLOTS:
    void openExternalLink( QUrl );

 private:
    TinyWebBrowserPrivate * const d;
};

}

#endif
