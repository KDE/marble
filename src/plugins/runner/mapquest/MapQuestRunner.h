//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//


#ifndef MARBLE_OSMMAPQUESTRUNNER_H
#define MARBLE_OSMMAPQUESTRUNNER_H

#include "RoutingRunner.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

namespace Marble
{

class MapQuestRunner : public RoutingRunner
{
    Q_OBJECT

public:
    explicit MapQuestRunner(QObject *parent = 0);

    ~MapQuestRunner();

    // Overriding MarbleAbstractRunner
    virtual void retrieveRoute( const RouteRequest *request );

private Q_SLOTS:
    void get();

    /** Route data was retrieved via http */
    void retrieveData( QNetworkReply *reply );

    /** A network error occurred */
    void handleError( QNetworkReply::NetworkError );

private:
    void append( QString* input, const QString &key, const QString &value );

    int maneuverType( int mapQuestId ) const;

    GeoDataDocument* parse( const QByteArray &input ) const;

    QNetworkAccessManager m_networkAccessManager;

    QNetworkRequest m_request;
};

}

#endif
