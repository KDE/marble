#include <QtCore/QObject>
#include <QtTest/QtTest>

#include "GeoDataParser.h"
#include "GeoDataDocument.h"
#include "GeoWriter.h"

#include <QtCore/QDir>
#include <QtCore/QFile>

using namespace Marble;

class TestGeoDataWriter : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void countFeatures_data();
    void countFeatures();
    void saveFile_data();
    void saveFile();
    void saveAndCompare();
    void cleanupTestCase();
private:
    QDir dataDir;
    QMap<QString, QSharedPointer<GeoDataParser> > parsers;
};

Q_DECLARE_METATYPE( QSharedPointer<GeoDataParser> );

void TestGeoDataWriter::initTestCase()
{
    QStringList filters;
    filters << "*.kml";
    dataDir.setNameFilters( filters );

    dataDir = QDir( TESTSRCDIR );
    dataDir.cd( "data" );
    //check there are files in the data dir
    QVERIFY( dataDir.count() > 0 );

    QString filename;

    //test the loading of each file in the data dir
    foreach( filename, dataDir.entryList(filters, QDir::Files) ){

        //Add example files
        QFile file( dataDir.filePath(filename));

        //Verify file existance
        QVERIFY( file.exists() );

        //Make the parsers
        GeoDataParser* parser = new GeoDataParser( GeoData_KML );

        QSharedPointer<GeoDataParser>parserPointer ( parser );

        //Open the files and verify
        QVERIFY( file.open( QIODevice::ReadOnly ) );

        //Parser and verify
        QVERIFY( parser->read( &file ) );

        parsers.insert( filename, parserPointer );

        //close files
        file.close();
    }
}

void TestGeoDataWriter::countFeatures_data()
{
    // add the tables here
    QTest::addColumn<QSharedPointer<GeoDataParser> >("parser");
    QTest::addColumn<int>("count");

    //Add feature parsers ( referenced by file name ) and their count of features

//    This parser won't work because of the flaw that prevents single placemark files from loading
    QTest::newRow("New York") << parsers.value("NewYork.kml") << 1;
    QTest::newRow("New York Document") << parsers.value("NewYorkDocument.kml") << 2;
}

void TestGeoDataWriter::countFeatures()
{
    //count the features in the loaded KML file
    QFETCH(QSharedPointer<GeoDataParser>, parser);
    QFETCH(int, count );
    GeoDataDocument* document = dynamic_cast<GeoDataDocument*>(parser->activeDocument());
    QVERIFY( document );

    // there should be exactly 1 placemark in the NewYork File
    QCOMPARE( document->size(), count );
}

void TestGeoDataWriter::saveFile_data()
{
    QTest::addColumn<QSharedPointer<GeoDataParser> >( "parser" );

    QTest::newRow( "NewYork" ) << parsers.value("NewYork.kml");
    QTest::newRow( "NewYorkDocument") << parsers.value("NewYorkDocument.kml");
}

void TestGeoDataWriter::saveFile()
{
    QFETCH( QSharedPointer<GeoDataParser>, parser );

    //attempt to save a file using the GeoWriter
    QTemporaryFile tempFile;

    GeoWriter writer;
    //FIXME: a better way to do this?
    writer.setDocumentType( "http://earth.google.com/kml/2.2" );

    // Open file in right mode
    QVERIFY( tempFile.open() );

    QVERIFY( writer.write( &tempFile,
                           *dynamic_cast<GeoDataFeature*>(parser->activeDocument() ) ) );

}

void TestGeoDataWriter::saveAndCompare()
{
    //save the file and compare it to the origional
}

void TestGeoDataWriter::cleanupTestCase()
{
}

QTEST_MAIN( TestGeoDataWriter )
#include "TestGeoDataWriter.moc"

