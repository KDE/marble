//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Dennis Nienhüser <nienhueser@kde.org>

#ifndef MARBLE_HOSTIPRUNNER_H
#define MARBLE_HOSTIPRUNNER_H

#include "SearchRunner.h"

#include <QString>
#include <QHostInfo>
#include <QNetworkAccessManager>
#include <QNetworkRequest>

class QNetworkReply;

namespace Marble
{

class HostipRunner : public SearchRunner
{
    Q_OBJECT
public:
    explicit HostipRunner(QObject *parent = 0);

    ~HostipRunner();

private Q_SLOTS:
    void get();

    // Http request with hostip.info done
    void slotRequestFinished( QNetworkReply* );

    // IP address lookup finished
    void slotLookupFinished(const QHostInfo &host);

    // No results (or an error)
    void slotNoResults();

    void search( const QString &searchTerm, const GeoDataLatLonBox &preferred );

private:
    QHostInfo m_hostInfo;

    QNetworkAccessManager m_networkAccessManager;

    QNetworkRequest m_request;
};

}

#endif
