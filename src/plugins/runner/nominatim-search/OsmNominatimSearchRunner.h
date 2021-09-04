// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//


#ifndef MARBLE_OSMNOMINATIMSEARCHRUNNER_H
#define MARBLE_OSMNOMINATIMSEARCHRUNNER_H

#include "SearchRunner.h"

#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkRequest>

class QNetworkReply;
class QDomNode;

namespace Marble
{
class GeoDataExtendedData;

class OsmNominatimRunner : public SearchRunner
{
    Q_OBJECT
public:
    explicit OsmNominatimRunner(QObject *parent = nullptr);

    ~OsmNominatimRunner() override;

    void search( const QString &searchTerm, const GeoDataLatLonBox &preferred ) override;

private Q_SLOTS:
    // Forward a result to the search or reverse geocoding handler
    void handleResult( QNetworkReply* );

    // No results (or an error)
    void returnNoResults();

    void startSearch();

private:
    static GeoDataExtendedData extractChildren(const QDomNode &node);

    QNetworkAccessManager m_manager;
    QNetworkRequest m_request;
};

}

#endif
