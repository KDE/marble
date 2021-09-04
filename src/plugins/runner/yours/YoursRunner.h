//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2016 Piotr Wójcik <chocimier@tlen.pl>
//


#ifndef MARBLE_YOURSRUNNER_H
#define MARBLE_YOURSRUNNER_H

#include "RoutingRunner.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace Marble
{

class YoursRunner : public RoutingRunner
{
    Q_OBJECT

public:
    explicit YoursRunner( QObject *parent = nullptr );

    ~YoursRunner() override;

    // Overriding MarbleAbstractRunner
    void retrieveRoute( const RouteRequest *request ) override;

private Q_SLOTS:
    /** Route data was retrieved via http */
    void retrieveData( QNetworkReply *reply );

    /** A network error occurred */
    void handleError( QNetworkReply::NetworkError );

    void get();

private:
    static GeoDataDocument* parse( const QByteArray &input );

    static qreal distance( const GeoDataDocument* document );

    QNetworkAccessManager m_networkAccessManager;

    QNetworkRequest m_request;
};

}

#endif
