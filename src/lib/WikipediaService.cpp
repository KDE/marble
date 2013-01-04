//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include "WikipediaService.h"
#include <QtWebKit/QWebPage>
#include <QtWebKit/QWebFrame>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

namespace Marble
{

WikipediaService::WikipediaService(QObject *parent) :
    QObject(parent),
    m_manager(new QNetworkAccessManager(this)),
    m_page(new QWebPage(this))
{
    m_page->setNetworkAccessManager(m_manager);
    connect(m_manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(handleNetworkReply(QNetworkReply*)));
}

void WikipediaService::checkUrl(const QUrl &url)
{
    m_page->mainFrame()->setUrl(url);
}

void WikipediaService::handleNetworkReply(QNetworkReply *reply)
{
    if(reply->error() == QNetworkReply::NoError) {
        emit articleExists(reply->url(), m_page);
    }
}


} // namespace Marble

#include "WikipediaService.moc"
