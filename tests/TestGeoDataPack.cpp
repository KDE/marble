//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Patrick Spendrin  <ps_ml@gmx.de>
//

#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <QtCore/QString>
#include <QtCore/QBuffer>
#include <QtCore/QByteArray>
#include <QtCore/QDebug>

#include "MarbleDirs.h"
#include "GeoDataParser.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"

namespace Marble
{

class TestGeoDataPack : public QObject
{
    Q_OBJECT
    private slots:
        void initTestCase();
        void saveKMLToCache();
        void loadKMLFromCache();
        void saveCitiesToCache();
        void loadCitiesFromCache();

    private:
        QString content;
};

bool comparePlacemarks( GeoDataPlacemark *left, GeoDataPlacemark *right )
{
    bool equal = true;
    equal &= ( left != 0 );
    equal &= ( right != 0 );
    equal &= ( left->name() == right->name() );
    equal &= ( left->coordinate() == right->coordinate() );
    equal &= ( left->geometry()->nodeType() == right->geometry()->nodeType() );
    return equal;
}

bool compareDocuments( GeoDataDocument *left, GeoDataDocument *right )
{
    bool equal = true;
    equal &= ( left->size() == right->size() );
    for( int i=0; i< left->size(); ++i ) {
        if ( left->at(i).nodeType() == GeoDataTypes::GeoDataPlacemarkType ) {
            equal &= comparePlacemarks( dynamic_cast<GeoDataPlacemark*>( left->featureList()[i] ),
                                        dynamic_cast<GeoDataPlacemark*>( right->featureList()[i] ) );
        }
    }
    return equal;
}

void TestGeoDataPack::initTestCase()
{
    MarbleDirs::setMarbleDataPath( DATA_PATH );
    MarbleDirs::setMarblePluginPath( PLUGIN_PATH );

    content = QString( 
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<kml xmlns=\"http://earth.google.com/kml/2.2\">\n"
"  <Document>\n"
"    <Placemark>\n"
"      <name>Empty</name>\n"
"    </Placemark>\n"
"    <Placemark>\n"
"      <name>LinearRingTest.kml</name>\n"
"      <Polygon>\n"
"        <outerBoundaryIs>\n"
"          <LinearRing>\n"
"            <coordinates>\n"
"              -122.365662,37.826988,0\n"
"              -122.365202,37.826302,0\n"
"              -122.364581,37.82655,0\n"
"              -122.365038,37.827237,0\n"
"              -122.365662,37.826988,0\n"
"            </coordinates>\n"
"          </LinearRing>\n"
"        </outerBoundaryIs>\n"
"      </Polygon>\n"
"    </Placemark>\n"
"  </Document>\n"
"</kml>" );
}

void TestGeoDataPack::saveKMLToCache()
{
    GeoDataParser parser( GeoData_KML );
    
    QByteArray array( content.toUtf8() );
    QBuffer buffer( &array );
    buffer.open( QIODevice::ReadOnly );
    if ( !parser.read( &buffer ) ) {
        qWarning( "Could not parse data!" );
        QFAIL( "Could not parse data!" );
        return;
    }
    GeoDocument* document = parser.releaseDocument();
    QVERIFY( document );
    GeoDataDocument *dataDocument = static_cast<GeoDataDocument*>( document );
    QString path = QString( "%1/%2.cache" );
    path = path.arg( QCoreApplication::applicationDirPath() );
    path = path.arg( QString( "KMLTest" ) );

    QFile cacheFile( path );
    if ( cacheFile.open( QIODevice::WriteOnly ) ) {
        QDataStream stream ( &cacheFile );
        dataDocument->pack( stream );
        cacheFile.close();
        qDebug( "Saved kml document to cache: %s", path.toAscii().data() );
    }
    delete document;
}

void TestGeoDataPack::loadKMLFromCache()
{
    GeoDataDocument *cacheDocument = new GeoDataDocument();
    QString path = QString( "%1/%2.cache" );
    path = path.arg( QCoreApplication::applicationDirPath() );
    path = path.arg( QString( "KMLTest" ) );

    QFile cacheFile( path );
    if ( cacheFile.open( QIODevice::ReadOnly ) ) {
        QDataStream stream ( &cacheFile );
        cacheDocument->unpack( stream );
        cacheFile.close();
        qDebug( "Loaded kml document from cache: %s", path.toAscii().data() );
    }
    QVERIFY( cacheDocument );

    GeoDataParser parser( GeoData_KML );
    QByteArray array( content.toUtf8() );
    QBuffer buffer( &array );
    buffer.open( QIODevice::ReadOnly );
    if ( !parser.read( &buffer ) ) {
        qWarning( "Could not parse data!" );
        QFAIL( "Could not parse data!" );
        return;
    }
    GeoDocument* document = parser.releaseDocument();
    QVERIFY( document );
    GeoDataDocument *dataDocument = static_cast<GeoDataDocument*>( document );
    QVERIFY( compareDocuments( cacheDocument, dataDocument ) );

    delete document;
    delete cacheDocument;
}

void TestGeoDataPack::saveCitiesToCache()
{
    GeoDataParser parser( GeoData_KML );
    
    QFile citiesFile( CITIES_PATH );
    citiesFile.open( QIODevice::ReadOnly );
    if ( !parser.read( &citiesFile ) ) {
        qWarning( "Could not parse data!" );
        QFAIL( "Could not parse data!" );
        return;
    }
    GeoDocument* document = parser.releaseDocument();
    QVERIFY( document );
    GeoDataDocument *dataDocument = static_cast<GeoDataDocument*>( document );
    QString path = QString( "%1/%2.cache" );
    path = path.arg( QCoreApplication::applicationDirPath() );
    path = path.arg( QString( "CitiesTest" ) );

    QFile cacheFile( path );
    if ( cacheFile.open( QIODevice::WriteOnly ) ) {
        QDataStream stream ( &cacheFile );
        dataDocument->pack( stream );
        cacheFile.close();
        qDebug( "Saved kml document to cache: %s", path.toAscii().data() );
    }
    QVERIFY( cacheFile.size() > 0 );
    delete document;
}

void TestGeoDataPack::loadCitiesFromCache()
{
    GeoDataDocument *cacheDocument = new GeoDataDocument();
    QString path = QString( "%1/%2.cache" );
    path = path.arg( QCoreApplication::applicationDirPath() );
    path = path.arg( QString( "CitiesTest" ) );

    QFile cacheFile( path );
    if ( cacheFile.open( QIODevice::ReadOnly ) ) {
        QDataStream stream ( &cacheFile );
        cacheDocument->unpack( stream );
        cacheFile.close();
        qDebug( "Loaded kml document from cache: %s", path.toAscii().data() );
    }
    QVERIFY( cacheDocument );

    GeoDataParser parser( GeoData_KML );
    QFile citiesFile( CITIES_PATH );
    citiesFile.open( QIODevice::ReadOnly );
    if ( !parser.read( &citiesFile ) ) {
        qWarning( "Could not parse data!" );
        QFAIL( "Could not parse data!" );
        return;
    }
    GeoDocument* document = parser.releaseDocument();
    QVERIFY( document );
    GeoDataDocument *dataDocument = static_cast<GeoDataDocument*>( document );
    QVERIFY( compareDocuments( cacheDocument, dataDocument ) );

    delete cacheDocument;
    delete dataDocument;
}

}

QTEST_MAIN( Marble::TestGeoDataPack )

#include "TestGeoDataPack.moc"
