// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_OSMOPENROUTESERVICERUNNER_H
#define MARBLE_OSMOPENROUTESERVICERUNNER_H

#include "RoutingRunner.h"
#include "routing/instructions/RoutingInstruction.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>

namespace Marble
{

class GeoDataCoordinates;

class OpenRouteServiceRunner : public RoutingRunner
{
    Q_OBJECT

    enum PointType { StartPoint, ViaPoint, EndPoint };

public:
    explicit OpenRouteServiceRunner(QObject *parent = nullptr);

    // Overriding MarbleAbstractRunner
    void retrieveRoute(const RouteRequest *request) override;

private Q_SLOTS:
    /** Route data was retrieved via http */
    void retrieveData(QNetworkReply *reply);

    /** A network error occurred */
    void handleError(QNetworkReply::NetworkError);

    void get();

private:
    static QString formatCoordinates(const GeoDataCoordinates &coordinates);

    GeoDataDocument *parse(const QByteArray &input) const;

    static RoutingInstruction::TurnType parseTurnType(const QString &text, QString *road);

    QNetworkAccessManager m_networkAccessManager;

    QNetworkRequest m_request;
};

}

#endif
