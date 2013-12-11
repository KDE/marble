//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mihail Ivchenko <ematirov@gmail.com>
//


#ifndef MARBLE_CYCLESTREETSRUNNER_H
#define MARBLE_CYCLESTREETSRUNNER_H

#include "RoutingRunner.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace Marble
{

class CycleStreetsRunner : public RoutingRunner
{
    Q_OBJECT

public:
    explicit CycleStreetsRunner(QObject *parent = 0);

    ~CycleStreetsRunner();

    // Overriding MarbleAbstractRunner
    virtual void retrieveRoute( const RouteRequest *request );

private Q_SLOTS:
    void get();

    /** Route data was retrieved via http */
    void retrieveData( QNetworkReply *reply );

    /** A network error occurred */
    void handleError( QNetworkReply::NetworkError );

private:

    GeoDataDocument *parse( const QByteArray &content ) const;

    int maneuverType( QString& cycleStreetsName ) const;

    QNetworkAccessManager m_networkAccessManager;

    QNetworkRequest m_request;

    QMap<QString,int> turns;
};

}

#endif
