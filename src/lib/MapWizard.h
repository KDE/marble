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
    
    QString createArchive( QString mapId );
    void deleteArchive( QString mapId );

    // QWizard's functions
    int nextId() const;
    void accept();
    
public slots:
    // WMS protocol
    void queryServerCapabilities();
    bool parseServerCapabilities( QNetworkReply* reply );
    void createWmsLegend( QNetworkReply* reply );

    // Open file dialogs
    void querySourceImage();
    void queryPreviewImage();
    void queryLegendImage();
    void queryStaticUrlLegendImage();

    // Other
    void autoFillDetails();
    void downloadLevelZero();
    void createLevelZero( QNetworkReply* reply = 0 );
    void suggestPreviewImage();

private:
    GeoSceneDocument* createDocument();
    void createDgml( const GeoSceneDocument* document );
    bool createFiles( const GeoSceneHead* head );
    void createLegendHtml();
    void createLegend();
    void downloadLegend( const QString url );

    Q_DISABLE_COPY( MapWizard )
    MapWizardPrivate * const d;
};

}

#endif