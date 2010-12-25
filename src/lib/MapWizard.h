//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Utku Aydın <utkuaydin34@gmail.com>
//

#ifndef MARBLE_MAPWIZARD_H
#define MARBLE_MAPWIZARD_H

#include <QtGui/QWizard>
#include <QtXml/QXmlStreamWriter>
#include <QtNetwork/QNetworkReply>

#include "marble_export.h"

/**
 * @file
 * This file contains the header for MapWizard
 * @author Utku Aydın <utkuaydin34@gmail.com>
 */

namespace Marble {

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
    void fillServerCombobox();
    void downloadLevelZero();
    void createLevelZero( QNetworkReply* reply = 0 );
    void suggestPreviewImage();

private:
    void createDgml();
    bool createFiles();
    void createLegendHtml();
    void createLegend();
    void downloadLegend( const QString url );

    Q_DISABLE_COPY( MapWizard )
    MapWizardPrivate * const d;
};

}

#endif