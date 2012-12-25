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
#include <QtCore/QDir>

#include "GeoSceneDocument.h"
#include "GeoSceneHead.h"
#include "GeoSceneZoom.h"
#include "GeoSceneIcon.h"
#include "GeoSceneMap.h"
#include "GeoSceneLayer.h"
#include "GeoSceneTiled.h"
#include "GeoSceneGeodata.h"
#include "GeoSceneSettings.h"
#include "GeoSceneProperty.h"
#include "GeoSceneLegend.h"
#include "GeoSceneSection.h"
#include "GeoSceneItem.h"
#include "GeoSceneVector.h"
#include "DgmlElementDictionary.h"

#include "GeoWriter.h"
#include "GeoSceneParser.h"

using namespace Marble;

class TestGeoSceneWriter : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void saveFile_data();
    void saveFile();
    void saveAndLoad_data();
    void saveAndLoad();
    void saveAndCompare_data();
    void saveAndCompare();

    void writeHeadTag();
private:
    QDir dgmlPath;
    QMap<QString, QSharedPointer<GeoSceneParser> > parsers;
};

Q_DECLARE_METATYPE( QSharedPointer<GeoSceneParser> )

void TestGeoSceneWriter::initTestCase()
{
    QStringList dgmlFilters;
    dgmlFilters << "*.dgml";

    dgmlPath = QDir( DGML_PATH );
    dgmlPath.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);

    QString dgmlDirname;
    foreach( dgmlDirname, dgmlPath.entryList() ) {
        qDebug() << dgmlDirname;
        QDir dataDir(dgmlPath.absoluteFilePath(dgmlDirname));
        dataDir.setNameFilters( dgmlFilters );

        //check there are dgml files in the data dir
        if( dataDir.count() == 0 ) {
            continue;
        }

        QString filename;
        //test the loading of each file in the data dir
        foreach( filename, dataDir.entryList(dgmlFilters, QDir::Files) ){

            //Add example files
            QFile file( dataDir.filePath(filename));

            //Verify file existence
            QVERIFY( file.exists() );

            //Make the parsers
            GeoSceneParser* parser = new GeoSceneParser( GeoScene_DGML );

            QSharedPointer<GeoSceneParser>parserPointer ( parser );

            //Open the files and verify
            QVERIFY( file.open( QIODevice::ReadOnly ) );

            //Parser and verify
            QVERIFY2( parser->read( &file ), filename.toAscii() );

            parsers.insert( dataDir.filePath(filename), parserPointer );

            //close files
            file.close();
        }
    }
}

void TestGeoSceneWriter::saveFile_data()
{
    QTest::addColumn<QSharedPointer<GeoSceneParser> >( "parser" );

    foreach( QString key, parsers.keys() ) {
        QTest::newRow( key.toLocal8Bit() ) << parsers.value(key);
    }
}

void TestGeoSceneWriter::saveFile()
{
    QFETCH( QSharedPointer<GeoSceneParser>, parser );

    //attempt to save a file using the GeoWriter
    QTemporaryFile tempFile;

    GeoWriter writer;
    writer.setDocumentType( dgml::dgmlTag_nameSpace20 );

    // Open file in right mode
    QVERIFY( tempFile.open() );

    QVERIFY( writer.write( &tempFile, (dynamic_cast<GeoSceneDocument*>(parser->activeDocument() ) ) ) );
}

void TestGeoSceneWriter::saveAndLoad_data()
{
    QTest::addColumn<QSharedPointer<GeoSceneParser> >( "parser" );

    foreach( QString key, parsers.keys() ) {
        QTest::newRow( key.toLocal8Bit() ) << parsers.value(key);
    }
}

void TestGeoSceneWriter::saveAndLoad()
{
    //Save the file and then verify loading it again
    QFETCH( QSharedPointer<GeoSceneParser>, parser );

    QTemporaryFile tempFile;
    GeoWriter writer;
    writer.setDocumentType( dgml::dgmlTag_nameSpace20 );

    // Open file in right mode
    QVERIFY( tempFile.open() );

    QVERIFY( writer.write( &tempFile, (dynamic_cast<GeoSceneDocument*>(parser->activeDocument() ) ) ) );

    GeoSceneParser resultParser( GeoScene_DGML );

    tempFile.reset();
    QVERIFY( resultParser.read( &tempFile ) );
}

void TestGeoSceneWriter::saveAndCompare_data()
{
    QTest::addColumn<QSharedPointer<GeoSceneParser> >("parser");
    QTest::addColumn<QString>("original");

    foreach( QString key, parsers.keys() ) {
        QTest::newRow( key.toLocal8Bit() ) << parsers.value(key) << key;
    }
}

void TestGeoSceneWriter::saveAndCompare()
{
    //save the file and compare it to the original
    QFETCH( QSharedPointer<GeoSceneParser>, parser );
    QFETCH( QString, original );

    //attempt to save a file using the GeoWriter
    QTemporaryFile tempFile;

    GeoWriter writer;
    //FIXME: a better way to do this?
    writer.setDocumentType( dgml::dgmlTag_nameSpace20 );

    // Open file in right mode
    QVERIFY( tempFile.open() );

    QVERIFY( writer.write( &tempFile, (dynamic_cast<GeoSceneDocument*>(parser->activeDocument() ) ) ) );

    QFile file( original );
    QVERIFY( file.open( QIODevice::ReadOnly ) );
    QVERIFY( tempFile.reset() );
    QTextStream oldFile( &file );
    QTextStream newFile( &tempFile );

    QVERIFY( newFile.readAll().simplified().compare( oldFile.readAll().simplified() ) );
}

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
    
    GeoSceneTiled* texture = new GeoSceneTiled( "map" );
    texture->setSourceDir( "earth/testmap" );
    texture->setFileFormat( "png" );
    texture->setProjection( GeoSceneTiled::Equirectangular );
    texture->addDownloadUrl( QUrl( "http://download.kde.org/marble/map/{x}/{y}/{zoomLevel}" ) );
    texture->addDownloadUrl( QUrl( "http://download.google.com/marble/map/{x}/{y}/{zoomLevel}" ) );
    texture->addDownloadPolicy( DownloadBrowse, 20 );
    texture->addDownloadPolicy( DownloadBulk, 20 );
    texture->setMaximumTileLevel( 15 );
    texture->setLevelZeroColumns( 2 );
    texture->setLevelZeroRows( 2 );
    
    GeoSceneGeodata* geodata = new GeoSceneGeodata( "cityplacemarks" );
    geodata->setSourceFile( "baseplacemarks.kml" );
    
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
    GeoSceneParser parser( GeoScene_DGML );
    tempFile.reset();
    QVERIFY( parser.read( &tempFile ) );

    GeoSceneDocument *document2 = static_cast<GeoSceneDocument*>( parser.activeDocument() );
    QTemporaryFile tempFile2;
    tempFile2.open();
    GeoWriter writer2;
    writer2.setDocumentType( "http://edu.kde.org/marble/dgml/2.0" );
    QVERIFY( writer2.write( &tempFile2, document2 ) );

    tempFile.reset();
    QTextStream file( &tempFile );
    tempFile2.reset();
    QTextStream file2( &tempFile2 );
    QVERIFY( file.readAll().simplified().compare( file2.readAll().simplified() ) );

    delete document;
}

QTEST_MAIN( TestGeoSceneWriter )
#include "TestGeoSceneWriter.moc"
