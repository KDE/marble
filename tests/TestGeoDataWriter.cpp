//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#include <QObject>

#include "GeoDataParser.h"
#include "GeoDataDocument.h"
#include "GeoDataColorStyle.h"
#include "GeoWriter.h"
#include <geodata/handlers/kml/KmlElementDictionary.h>

#include <QDir>
#include <QFile>
#include <QTest>
#include <QTextStream>
#include <QBuffer>

using namespace Marble;

class TestGeoDataWriter : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void countFeatures_data();
    void saveFile_data();
    void saveFile();
    void saveAndLoad_data();
    void saveAndLoad();
    void saveAndCompare_data();
    void saveAndCompare();
    void saveAndCompareEquality_data();
    void saveAndCompareEquality();
    void cleanupTestCase();
private:
    QDir dataDir;
    QMap<QString, QSharedPointer<GeoDataParser> > parsers;
    QStringList m_testFiles;
};

Q_DECLARE_METATYPE( QSharedPointer<GeoDataParser> )

void TestGeoDataWriter::initTestCase()
{
    dataDir = QDir( TESTSRCDIR );
    dataDir.cd( "data" );
    //check there are files in the data dir
    QVERIFY( dataDir.count() > 0 );

    //test the loading of each file in the data dir
    foreach( const QString &filename, dataDir.entryList(QStringList() << "*.kml", QDir::Files) ){
        QFile file( dataDir.filePath(filename));
        QVERIFY( file.exists() );

        // Create parsers
        GeoDataParser* parser = new GeoDataParser( GeoData_KML );
        QSharedPointer<GeoDataParser>parserPointer ( parser );

        // Open the files and verify
        QVERIFY( file.open( QIODevice::ReadOnly ) );
        QVERIFY2(parser->read(&file), filename.toLatin1().constData());

        parsers.insert( filename, parserPointer );
        m_testFiles << filename;
        file.close();
    }
}

void TestGeoDataWriter::countFeatures_data()
{
    QTest::addColumn<QSharedPointer<GeoDataParser> >("parser");
    foreach( const QString &file, m_testFiles ) {
        QTest::newRow(file.toStdString().c_str()) << parsers.value(file);
    }
}

void TestGeoDataWriter::saveFile_data()
{
    QTest::addColumn<QSharedPointer<GeoDataParser> >( "parser" );
    foreach( const QString &file, m_testFiles ) {
        QTest::newRow(file.toStdString().c_str()) << parsers.value(file);
    }
}

void TestGeoDataWriter::saveFile()
{
    QFETCH( QSharedPointer<GeoDataParser>, parser );

    //attempt to save a file using the GeoWriter
    QByteArray data;
    QBuffer buffer( &data );

    GeoWriter writer;
    //FIXME: a better way to do this?
    writer.setDocumentType( kml::kmlTag_nameSpaceOgc22 );

    // Open file in right mode
    QVERIFY( buffer.open( QIODevice::WriteOnly ) );

    QVERIFY( writer.write( &buffer, &(*dynamic_cast<GeoDataFeature*>(parser->activeDocument() ) ) ) );

}

void TestGeoDataWriter::saveAndLoad_data()
{
    QTest::addColumn<QSharedPointer<GeoDataParser> >("parser");
    foreach( const QString &file, m_testFiles ) {
        QTest::newRow(file.toStdString().c_str()) << parsers.value(file);
    }
}

void TestGeoDataWriter::saveAndLoad()
{
    //Save the file and then verify loading it again
    QFETCH( QSharedPointer<GeoDataParser>, parser );

    QByteArray data;
    QBuffer buffer( &data );

    GeoWriter writer;
    //FIXME: a better way to do this?
    writer.setDocumentType( kml::kmlTag_nameSpaceOgc22 );

    // Open file in right mode
    QVERIFY( buffer.open( QIODevice::ReadWrite ) );

    QVERIFY( writer.write( &buffer, &( *dynamic_cast<GeoDataFeature*>(parser->activeDocument() ) ) ) );

    GeoDataParser resultParser( GeoData_KML );

    buffer.reset();
    QVERIFY( resultParser.read( &buffer ) );
}

void TestGeoDataWriter::saveAndCompare_data()
{
    QTest::addColumn<QSharedPointer<GeoDataParser> >("parser");
    QTest::addColumn<QString>("original");

    foreach( const QString &file, m_testFiles ) {
        QTest::newRow(file.toStdString().c_str()) << parsers.value(file) << file;
    }
}

void TestGeoDataWriter::saveAndCompare()
{
    //save the file and compare it to the original
    QFETCH( QSharedPointer<GeoDataParser>, parser );
    QFETCH( QString, original );

    //attempt to save a file using the GeoWriter
    QByteArray data;
    QBuffer buffer( &data );
    buffer.open( QIODevice::ReadWrite );

    GeoWriter writer;
    //FIXME: a better way to do this?
    writer.setDocumentType( kml::kmlTag_nameSpaceOgc22 );

    QVERIFY( writer.write( &buffer, &( *dynamic_cast<GeoDataFeature*>(parser->activeDocument() ) ) ) );

    QFile file( dataDir.filePath( original ) );
    QVERIFY( file.open( QIODevice::ReadOnly ) );
    QVERIFY( buffer.reset() );
    QTextStream oldFile( &file );
    QTextStream newFile( &buffer );

    QCOMPARE( newFile.readAll().simplified(), oldFile.readAll().simplified() );
}

void TestGeoDataWriter::saveAndCompareEquality_data()
{
    QTest::addColumn<QSharedPointer<GeoDataParser> >("parser");
    QTest::addColumn<QString>("original");

    foreach( const QString &file, m_testFiles ) {
        QTest::newRow(file.toStdString().c_str()) << parsers.value(file) << file;
    }
}

void TestGeoDataWriter::saveAndCompareEquality()
{
    QFETCH( QSharedPointer<GeoDataParser>, parser );
    QFETCH( QString, original );

    QByteArray data;
    QBuffer buffer( &data );
    buffer.open( QIODevice::ReadWrite );

    GeoWriter writer;
    //FIXME: a better way to do this?
    writer.setDocumentType( kml::kmlTag_nameSpaceOgc22 );

    GeoDataDocument *initialDoc = dynamic_cast<GeoDataDocument*>( parser->activeDocument() );
    QVERIFY( writer.write( &buffer, initialDoc) );

    buffer.reset();
    GeoDataParser otherParser( GeoData_KML);
    QVERIFY( otherParser.read( &buffer ) );

    GeoDataDocument *otherDoc = dynamic_cast<GeoDataDocument*>( otherParser.activeDocument() );
    QVERIFY( *initialDoc == *otherDoc );
}

void TestGeoDataWriter::cleanupTestCase()
{
    QMap<QString, QSharedPointer<GeoDataParser> >::iterator itpoint = parsers.begin();
    QMap<QString, QSharedPointer<GeoDataParser> >::iterator const endpoint = parsers.end();
    for (; itpoint != endpoint; ++itpoint ) {
        delete itpoint.value()->releaseDocument();
    }
}

QTEST_MAIN( TestGeoDataWriter )
#include "TestGeoDataWriter.moc"

