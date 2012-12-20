//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//


#ifndef MARBLE_OSMOSRMRUNNER_H
#define MARBLE_OSMOSRMRUNNER_H

#include "MarbleAbstractRunner.h"
#include "routing/instructions/RoutingInstruction.h"

#include <QtCore/QString>
#include <QtNetwork/QNetworkReply>

class QNetworkAccessManager;

namespace Marble
{

class GeoDataLineString;

class OSRMRunner : public MarbleAbstractRunner
{
    Q_OBJECT

public:
    explicit OSRMRunner(QObject *parent = 0);

    ~OSRMRunner();

    // Overriding MarbleAbstractRunner
    GeoDataFeature::GeoDataVisualCategory category() const;

    // Overriding MarbleAbstractRunner
    virtual void retrieveRoute( const RouteRequest *request );

private Q_SLOTS:
    /** Route data was retrieved via http */
    void retrieveData( QNetworkReply *reply );

    /** A network error occurred */
    void handleError( QNetworkReply::NetworkError );

    void get();

private:
    void append( QString* input, const QString &key, const QString &value ) const;

    GeoDataLineString* decodePolyline( const QString &geometry ) const;

    RoutingInstruction::TurnType parseTurnType( const QString &instruction ) const;

    GeoDataDocument* parse( const QByteArray &input );

    QNetworkAccessManager *m_networkAccessManager;

    QNetworkRequest m_request;

    static QVector<QPair<GeoDataCoordinates,QString> > m_cachedHints;

    static QString m_hintChecksum;
};

}

#endif
