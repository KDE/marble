//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#ifndef MARBLE_MAPWIZARD_H
#define MARBLE_MAPWIZARD_H

#include <QtGui/QWizard>

#include "marble_export.h"

/**
 * @file
 * This file contains the header for MapWizard
 * @author Utku Aydın <utkuaydin34@gmail.com>
 */

class QNetworkReply;

namespace Marble {

class GeoSceneDocument;
class GeoSceneHead;

class MapWizardPrivate;

class MARBLE_EXPORT MapWizard : public QWizard
{
    Q_OBJECT

public:
    explicit MapWizard(QWidget *parent = 0);
    ~MapWizard();

    QStringList wmsServers() const;
    void setWmsServers( const QStringList& uris );

    QStringList staticUrlServers() const;
    void setStaticUrlServers( const QStringList& uris );

    QString createArchive( QString mapId );
    void deleteArchive( QString mapId );

    // QWizard's functions
    virtual void accept();
    virtual bool validateCurrentPage();
    virtual int nextId() const;

public slots:
    // WMS protocol
    void queryServerCapabilities();
    void parseServerCapabilities( QNetworkReply* reply );
    void createWmsLegend( QNetworkReply* reply );

    // Open file dialogs
    void querySourceImage();
    void queryPreviewImage();
    void queryLegendImage();

    // Other
    void autoFillDetails();
    void downloadLevelZero();
    void createLevelZero( QNetworkReply* reply = 0 );

private:
    Q_PRIVATE_SLOT( d, void pageEntered( int ) )

    GeoSceneDocument* createDocument();
    bool createFiles( const GeoSceneDocument* head );
    void createLegendHtml( QString image = "./legend/legend.png" );
    void createLegendFile();
    void createLegend();
    void downloadLegend( const QString& url );

    Q_DISABLE_COPY( MapWizard )
    MapWizardPrivate * const d;
};

}

#endif
