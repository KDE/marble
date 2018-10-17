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
#include <QWebEngineView>

// Marble
#include "marble_export.h"

class QString;
class QUrl;

namespace Marble
{

class TinyWebBrowserPrivate;

/**
 * This class provides a tiny web browser based on QWebEngineView (WebKit).
 * It is different from QWebEngineView as it has the button "Open in new Window"
 * disabled per default and instead opens every link in the default web
 * browser of the user.
 */
class MARBLE_EXPORT TinyWebBrowser : public QWebEngineView
{
    Q_OBJECT

 public:
    explicit TinyWebBrowser( QWidget* parent = nullptr );
    ~TinyWebBrowser() override;

 public Q_SLOTS:
    void setWikipediaPath( const QString& relativeUrl );
    void print();

 Q_SIGNALS:
    void statusMessage( const QString& );

 protected:
    QWebEngineView *createWindow( QWebEnginePage::WebWindowType type ) override;

 private Q_SLOTS:
    void openExternalLink( const QUrl& );

 private:
    TinyWebBrowserPrivate * const d;
};

}

#endif
