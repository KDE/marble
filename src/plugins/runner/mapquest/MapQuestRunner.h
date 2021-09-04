// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
//


#ifndef MARBLE_OSMMAPQUESTRUNNER_H
#define MARBLE_OSMMAPQUESTRUNNER_H

#include "RoutingRunner.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace Marble
{

class MapQuestRunner : public RoutingRunner
{
    Q_OBJECT

public:
    explicit MapQuestRunner(QObject *parent = nullptr);

    ~MapQuestRunner() override;

    // Overriding MarbleAbstractRunner
    void retrieveRoute( const RouteRequest *request ) override;

private Q_SLOTS:
    void get();

    /** Route data was retrieved via http */
    void retrieveData( QNetworkReply *reply );

    /** A network error occurred */
    void handleError( QNetworkReply::NetworkError );

private:
    static void append( QString* input, const QString &key, const QString &value );

    static int maneuverType( int mapQuestId );

    GeoDataDocument* parse( const QByteArray &input ) const;

    QNetworkAccessManager m_networkAccessManager;

    QNetworkRequest m_request;
};

}

#endif
