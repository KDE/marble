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


#include <QtGui/QTextBrowser>


class HttpDownloadManager;


class TinyWebBrowser : public QTextBrowser
{

    Q_OBJECT

 public:
    TinyWebBrowser( QWidget* parent );

 public Q_SLOTS:
    void setSource( const QString& relativeUrl );
    void slotDownloadFinished( const QString&, int );

 Q_SIGNALS:
    void statusMessage( QString );

 protected:
    virtual QVariant loadResource ( int type, const QUrl & name );

 private:
    HttpDownloadManager *m_downloadManager;
    QString         m_source;
    QList<QUrl>     m_urlList;
};


#endif // TINYWEBBROWSER_H
