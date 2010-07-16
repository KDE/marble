//
// This file is part of the Marble Desktop Globe.
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
#include "GeoDataFeature.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPolygon.h"
#include "GeoDataCoordinates.h"

namespace Marble
{

class KMLTest : public QObject
{
    Q_OBJECT
    private slots:
        void initTestCase();
        void loadKMLFromData();
        void saveKMLToCache();
        void loadKMLFromCache();
        void saveCitiesToCache();
        void loadCitiesFromCache();

    private:
        QString content;
};

void KMLTest::initTestCase()
{
    MarbleDirs::setMarbleDataPath( DATA_PATH );
    MarbleDirs::setMarblePluginPath( PLUGIN_PATH );

    content = QString( 
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<kml xmlns=\"http://earth.google.com/kml/2.1\">\n"
"  <Document>\n"
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

void KMLTest::loadKMLFromData()
{
    GeoDataParser parser( GeoData_KML );
    
    QByteArray array( content.toUtf8() );
    QBuffer buffer( &array );
    buffer.open( QIODevice::ReadOnly );
    qDebug() << "Buffer content:" << endl << buffer.buffer();
    if ( !parser.read( &buffer ) ) {
        qWarning( "Could not parse data!" );
        QFAIL( "Could not parse data!" );
        return;
    }
    GeoDocument* document = parser.releaseDocument();
    QVERIFY( document );
    GeoDataDocument *dataDocument = static_cast<GeoDataDocument*>( document );
    GeoDataFeature *feature = dataDocument->featureList().at( 0 );
    GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>( feature );
    GeoDataPolygon *polygon = static_cast<GeoDataPolygon*>( placemark->geometry() );
    qDebug() << placemark->name();

    QVector<GeoDataCoordinates>::iterator iterator = polygon->outerBoundary().begin();
    for( ; iterator != polygon->outerBoundary().end(); ++iterator ) {
        qDebug() << (*iterator).toString();
    }
    delete document;
}

void KMLTest::saveKMLToCache()
{
    GeoDataParser parser( GeoData_KML );
    
    QByteArray array( content.toUtf8() );
    QBuffer buffer( &array );
    buffer.open( QIODevice::ReadOnly );
    qDebug() << "Buffer content:" << endl << buffer.buffer();
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

void KMLTest::loadKMLFromCache()
{
    GeoDataDocument *dataDocument = new GeoDataDocument();
    QString path = QString( "%1/%2.cache" );
    path = path.arg( QCoreApplication::applicationDirPath() );
    path = path.arg( QString( "KMLTest" ) );

    QFile cacheFile( path );
    if ( cacheFile.open( QIODevice::ReadOnly ) ) {
        QDataStream stream ( &cacheFile );
        dataDocument->unpack( stream );
        cacheFile.close();
        qDebug( "Loaded kml document from cache: %s", path.toAscii().data() );
    }
    QVERIFY( dataDocument );

    GeoDataFeature *feature = dataDocument->featureList().at( 0 );
    GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>( feature );
    GeoDataPolygon *polygon = static_cast<GeoDataPolygon*>(placemark->geometry());
    
    qDebug() << placemark->name();
    
    QVector<GeoDataCoordinates>::iterator iterator = polygon->outerBoundary().begin();
    for( ; iterator != polygon->outerBoundary().end(); ++iterator ) {
        qDebug() << (*iterator).toString();
    }
    delete dataDocument;
}

void KMLTest::saveCitiesToCache()
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
    delete document;
}

void KMLTest::loadCitiesFromCache()
{
    GeoDataDocument *dataDocument = new GeoDataDocument();
    QString path = QString( "%1/%2.cache" );
    path = path.arg( QCoreApplication::applicationDirPath() );
    path = path.arg( QString( "CitiesTest" ) );

    QFile cacheFile( path );
    if ( cacheFile.open( QIODevice::ReadOnly ) ) {
        QDataStream stream ( &cacheFile );
        dataDocument->unpack( stream );
        cacheFile.close();
        qDebug( "Loaded kml document from cache: %s", path.toAscii().data() );
    }
    QVERIFY( dataDocument );

    qDebug() << dataDocument->featureList().size();
    for( int i = 0; i < dataDocument->featureList().size(); i++ ) {
        fprintf( stderr, "Debug: %s\n", dataDocument->featureList()[i]->name().toLocal8Bit().data() );
    }
    delete dataDocument;
}

}

QTEST_MAIN( Marble::KMLTest )

#include "KMLTest.moc"
