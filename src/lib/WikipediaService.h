//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#ifndef WIKIPEDIASERVICE_H
#define WIKIPEDIASERVICE_H

#include <QObject>

class QUrl;
class QNetworkAccessManager;
class QNetworkReply;
class QWebPage;

namespace Marble
{

class WikipediaService : public QObject
{
    Q_OBJECT
public:
    explicit WikipediaService(QObject *parent = 0);
    
signals:
    void articleExists(QUrl, QWebPage*);
    
public slots:
    void checkUrl(const QUrl &url);

private slots:
    void handleNetworkReply(QNetworkReply *reply);

private:
    QNetworkAccessManager *m_manager;
    QWebPage *m_page;
};

} // namespace Marble

#endif // WIKIPEDIASERVICE_H
