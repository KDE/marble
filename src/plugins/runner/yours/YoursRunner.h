//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
// Copyright 2016      Piotr Wójcik <chocimier@tlen.pl>
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
    explicit YoursRunner( QObject *parent = 0 );

    ~YoursRunner();

    // Overriding MarbleAbstractRunner
    virtual void retrieveRoute( const RouteRequest *request );

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
