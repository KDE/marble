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

#include <QWizard>

#include "marble_export.h"

/**
 * @file
 * This file contains the header for MapWizard
 * @author Utku Aydın <utkuaydin34@gmail.com>
 */

class QNetworkReply;

namespace Marble {

class GeoSceneDocument;

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

    static QString createArchive( QWidget *parent, const QString& mapId );
    static void deleteArchive( const QString& mapId );

    // QWizard's functions
    virtual void accept();
    virtual bool validateCurrentPage();
    virtual int nextId() const;

public Q_SLOTS:
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
    void showPreview();

private:
    Q_PRIVATE_SLOT( d, void pageEntered( int ) )

    GeoSceneDocument* createDocument();
    bool createFiles( const GeoSceneDocument* head );
    static QString createLegendHtml( const QString& image = QLatin1String("./legend/legend.png") );
    void createLegendFile( const QString& legendHtml );
    void createLegend();
    void downloadLegend( const QString& url );

    Q_DISABLE_COPY( MapWizard )
    MapWizardPrivate * const d;
};

}

#endif
