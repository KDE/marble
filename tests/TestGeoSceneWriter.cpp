//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#include <QtCore/QObject>
#include <QtCore/QTemporaryFile>
#include <QtTest/QTest>
#include <QtCore/QDebug>

#include "GeoSceneDocument.h"
#include "GeoSceneHead.h"
#include "GeoSceneZoom.h"
#include "GeoSceneIcon.h"
#include "GeoSceneMap.h"
#include "GeoSceneLayer.h"
#include "GeoSceneTexture.h"
#include "GeoWriter.h"

using namespace Marble;

class TestGeoSceneWriter : public QObject
{
    Q_OBJECT

private slots:
    void writeHeadTag();
};

void TestGeoSceneWriter::writeHeadTag()
{
    GeoSceneDocument *document = new GeoSceneDocument;
    
    GeoSceneHead* head = document->head();
    head->setName( "Test Map" );
    head->setTheme( "testmap" );
    head->setTarget( "earth" );
    
    GeoSceneIcon* icon = document->head()->icon();
    icon->setPixmap( "preview.jpg" );
    
    GeoSceneZoom* zoom = document->head()->zoom();
    zoom->setMaximum( 1000 );
    zoom->setMaximum( 500 );
    zoom->setDiscrete( true );
    
    GeoSceneTexture* texture = new GeoSceneTexture( "map" );
    texture->setSourceDir( "earth/testmap" );
    texture->setFileFormat( "png" );
    texture->setProjection( GeoSceneTexture::Equirectangular );
    texture->addDownloadUrl( QUrl( "http://download.kde.org/marble/map/{x}/{y}/{zoomLevel}" ) );
    texture->addDownloadUrl( QUrl( "http://download.google.com/marble/map/{x}/{y}/{zoomLevel}" ) );
    texture->addDownloadPolicy( DownloadBrowse, 20 );
    texture->addDownloadPolicy( DownloadBulk, 20 );
    texture->setMaximumTileLevel( 15 );
    texture->setLevelZeroColumns( 2 );
    texture->setLevelZeroRows( 2 );
    
    GeoSceneLayer* layer = new GeoSceneLayer( "testmap" );
    layer->setBackend( "texture" );
    layer->addDataset( texture );
    
    GeoSceneMap* map = document->map();
    map->addLayer( layer );
    
    QTemporaryFile tempFile;
    tempFile.open();
    
    GeoWriter writer;
    writer.setDocumentType( "http://edu.kde.org/marble/dgml/2.0" );
    QVERIFY( writer.write( &tempFile, document ) );
}

QTEST_MAIN( TestGeoSceneWriter )
#include "TestGeoSceneWriter.moc"