//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

//
// The Tiny Web Browser is a web browser based on QTextBrowser.
//
// Author: Torsten Rahn
//


#ifndef TINYWEBBROWSER_H
#define TINYWEBBROWSER_H

// Qt
#include <qglobal.h>
#include <QtWebKit/QWebView>
#include <QtGui/QDesktopServices>

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
    TinyWebBrowser( QWidget* parent = 0 );
    ~TinyWebBrowser();

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
    TinyWebBrowserPrivate *d;
};

}

#endif // TINYWEBBROWSER_H
