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
#include "GeoSceneGeodata.h"
#include "GeoSceneSettings.h"
#include "GeoSceneProperty.h"
#include "GeoSceneLegend.h"
#include "GeoSceneSection.h"
#include "GeoSceneItem.h"
#include "GeoSceneVector.h"

#include "GeoWriter.h"
#include "GeoSceneParser.h"

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
    
    GeoSceneGeodata* geodata = new GeoSceneGeodata( "cityplacemarks" );
    geodata->setSourceFile( "baseplacemarks.kml" );
    geodata->setSourceFileFormat( "KML" );
    
    GeoSceneLayer* layer = new GeoSceneLayer( "testmap" );
    layer->setBackend( "texture" );
    layer->addDataset( texture );
    
    GeoSceneLayer* secondLayer = new GeoSceneLayer( "standardplaces" );
    secondLayer->setBackend( "geodata" );
    secondLayer->addDataset( geodata );
    
    GeoSceneLayer* thirdLayer = new GeoSceneLayer( "mwdbii" );
    thirdLayer->setBackend( "vector" );
    thirdLayer->setRole( "polyline" );
    
    GeoSceneVector* vector = new GeoSceneVector( "pdiffborder" );
    vector->setFeature( "border" );
    vector->setFileFormat( "PNT" );
    vector->setSourceFile( "earth/mwdbii/PDIFFBORDER.PNT" );
    vector->pen().setColor( "#ffe300" );
    thirdLayer->addDataset( vector );
    
    GeoSceneMap* map = document->map();
    map->addLayer( layer );
    map->addLayer( secondLayer );
    map->addLayer( thirdLayer );
    
    GeoSceneSettings *settings = document->settings();
   
    GeoSceneProperty *coorGrid = new GeoSceneProperty( "coordinate-grid" );
    coorGrid->setValue( true );
    coorGrid->setAvailable( true );
    settings->addProperty( coorGrid );
    
    GeoSceneProperty *overviewmap = new GeoSceneProperty( "overviewmap" );
    overviewmap->setValue( true );
    overviewmap->setAvailable( true );
    settings->addProperty( overviewmap );
    
    GeoSceneProperty *compass = new GeoSceneProperty( "compass" );
    compass->setValue( true );
    compass->setAvailable( true );
    settings->addProperty( compass );
    
    GeoSceneProperty *scalebar = new GeoSceneProperty( "scalebar" );
    scalebar->setValue( true );
    scalebar->setAvailable( true );
    settings->addProperty( scalebar );
    
    GeoSceneLegend* legend = document->legend();
    GeoSceneSection* section = new GeoSceneSection( "areas" );
    section->setHeading( "Areas" );
    legend->addSection( section );
    
    GeoSceneItem* sportsPitch = new GeoSceneItem( "sports_pitch" );
    sportsPitch->setText( "Sports pitch" );
    GeoSceneIcon* sportsPitchIcon = sportsPitch->icon();
    sportsPitchIcon->setPixmap( "maps/earth/testmap/legend/sports_pitch.png" );
    section->addItem( sportsPitch );
    
    GeoSceneItem* sportsCentre = new GeoSceneItem( "sports_centre" );
    sportsCentre->setText( "Sports centre" );
    GeoSceneIcon* sportsCentreIcon = sportsCentre->icon();
    sportsCentreIcon->setColor( "#00FF00" );
    section->addItem( sportsCentre );
        
    QTemporaryFile tempFile;
    tempFile.open();
    
    GeoWriter writer;
    writer.setDocumentType( "http://edu.kde.org/marble/dgml/2.0" );
    QVERIFY( writer.write( &tempFile, document ) );
    
    //Parser and verify
    GeoSceneParser* parser = new GeoSceneParser( GeoScene_DGML );
    tempFile.reset();
    QVERIFY( parser->read( &tempFile ) );

    GeoSceneDocument *document2 = static_cast<GeoSceneDocument*>( parser->activeDocument() );
    QTemporaryFile tempFile2;
    tempFile2.open();
    GeoWriter writer2;
    writer2.setDocumentType( "http://edu.kde.org/marble/dgml/2.0" );
    QVERIFY( writer2.write( &tempFile2, document2 ) );

    tempFile.reset();
    QTextStream file( &tempFile );
    tempFile2.reset();
    QTextStream file2( &tempFile2 );
    QCOMPARE( file.readAll().simplified(), file2.readAll().simplified() );
}

QTEST_MAIN( TestGeoSceneWriter )
#include "TestGeoSceneWriter.moc"