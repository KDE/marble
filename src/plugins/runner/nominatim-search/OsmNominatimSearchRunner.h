//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//


#ifndef MARBLE_OSMNOMINATIMRUNNER_H
#define MARBLE_OSMNOMINATIMRUNNER_H

#include "MarbleAbstractRunner.h"

#include <QtCore/QString>
#include <QtNetwork/QHostInfo>
#include <QtNetwork/QNetworkRequest>

class QNetworkReply;
class QNetworkAccessManager;
class QDomNodeList;

namespace Marble
{

class OsmNominatimRunner : public MarbleAbstractRunner
{
    Q_OBJECT
public:
    explicit OsmNominatimRunner(QObject *parent = 0);

    ~OsmNominatimRunner();

    virtual void search( const QString &searchTerm, const GeoDataLatLonAltBox &preferred );

private Q_SLOTS:
    // Forward a result to the search or reverse geocoding handler
    void handleResult( QNetworkReply* );

    // No results (or an error)
    void returnNoResults();

    void startSearch();

private:
    QNetworkAccessManager* m_manager;

    QNetworkRequest m_request;
};

}

#endif
