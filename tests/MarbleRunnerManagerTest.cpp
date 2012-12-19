//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>
// Copyright 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include <QtTest/QtTest>
#include <QtTest/QSignalSpy>
#include <QtCore/QMetaType>

#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "MarbleRunnerManager.h"
#include "PluginManager.h"
#include "GeoDataPlacemark.h"
#include "routing/RouteRequest.h"

#define addRow() QTest::newRow( QString("line %1").arg( __LINE__ ).toAscii().data() )

Q_DECLARE_METATYPE( QList<Marble::GeoDataCoordinates> )

namespace Marble
{

class MarbleRunnerManagerTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();// will be called before the first testfunction is executed.
    void cleanupTestCase();// will be called after the last testfunction was executed.
    void init(){};// will be called before each testfunction is executed.
    void cleanup(){};// will be called after every testfunction.

    void testSyncPlacemarks();

    void testAsyncPlacemarks_data();
    void testAsyncPlacemarks();

    void testSyncReverse();

    void testAsyncReverse_data();
    void testAsyncReverse();

    void testSyncRouting();

    void testAsyncRouting_data();
    void testAsyncRouting();

    void testSyncParsing();

    void testAsyncParsing_data();
    void testAsyncParsing();

public:
    PluginManager m_pluginManager;
    int m_time;
    QString m_name;
    GeoDataCoordinates m_coords;
    GeoDataCoordinates m_coords2;
    RouteRequest m_request;
    QString m_fileName;
};

void MarbleRunnerManagerTest::initTestCase()
{
    // MarbleDebug::enable = true;
    MarbleDirs::setMarbleDataPath( DATA_PATH );
    MarbleDirs::setMarblePluginPath( PLUGIN_PATH );
    m_time = 30000;
    m_name = "Berlin";

    qRegisterMetaType<QList<GeoDataCoordinates> >( "QList<GeoDataCoordinates>" );

    m_coords.setLatitude(52.50160, GeoDataCoordinates::Degree );
    m_coords.setLongitude(13.40233, GeoDataCoordinates::Degree );

    m_coords2.setLatitude(52.52665, GeoDataCoordinates::Degree );
    m_coords2.setLongitude(13.39032, GeoDataCoordinates::Degree );

    m_request.append( m_coords );
    m_request.append( m_coords2 );

    m_fileName = MarbleDirs::path( "placemarks/otherplacemarks.cache" );
}

void MarbleRunnerManagerTest::cleanupTestCase()
{
}

void MarbleRunnerManagerTest::testSyncPlacemarks()
{
    MarbleRunnerManager m_runnerManager(&m_pluginManager, this);

    QSignalSpy finishSpy( &m_runnerManager, SIGNAL(placemarkSearchFinished()) );
    QSignalSpy resultSpy( &m_runnerManager, SIGNAL(searchResultChanged(QVector<GeoDataPlacemark*>)) );

    QCOMPARE( finishSpy.count(), 0 );
    QCOMPARE( resultSpy.count(), 0 );

    QTime timer;
    timer.start();
    QVector<GeoDataPlacemark*> placemarks = m_runnerManager.searchPlacemarks(m_name);

    QVERIFY( timer.elapsed() < m_time );
    QCOMPARE( resultSpy.count(), 1 );
    QVERIFY( placemarks.size() > 0 );
    QCOMPARE( finishSpy.count(), 1 );

    // second search is optimised
    placemarks.clear();
    finishSpy.clear();
    resultSpy.clear();
    timer.start();
    placemarks = m_runnerManager.searchPlacemarks(m_name);

    QVERIFY( timer.elapsed() < m_time );
    QCOMPARE( resultSpy.count(), 1 );
    QVERIFY( placemarks.size() > 0 );
    QCOMPARE( finishSpy.count(), 1 );
}

void MarbleRunnerManagerTest::testAsyncPlacemarks_data()
{
    QTest::addColumn<QString>( "name" );

    addRow() << QString( "Berlin" );
    addRow() << QString( "www.heise.de" );
}

void MarbleRunnerManagerTest::testAsyncPlacemarks()
{
    MarbleRunnerManager m_runnerManager(&m_pluginManager, this);

    QSignalSpy finishSpy( &m_runnerManager, SIGNAL( searchFinished( QString ) ) );
    QSignalSpy resultSpy( &m_runnerManager, SIGNAL( searchResultChanged( QVector<GeoDataPlacemark*> ) ) );

    QEventLoop loop;
    connect( &m_runnerManager, SIGNAL( searchFinished( QString ) ),
             &loop, SLOT( quit() ) );

    QFETCH( QString, name );
    m_runnerManager.findPlacemarks( name );

    loop.exec();

    QCOMPARE( resultSpy.count(), 1 );
    QCOMPARE( finishSpy.count(), 1 );

    QThreadPool::globalInstance()->waitForDone();
}

void MarbleRunnerManagerTest::testSyncReverse()
{
    MarbleRunnerManager m_runnerManager(&m_pluginManager, this);

    QSignalSpy finishSpy( &m_runnerManager, SIGNAL(reverseGeocodingFinished()) );
    QSignalSpy resultSpy( &m_runnerManager, SIGNAL(reverseGeocodingFinished(GeoDataCoordinates,GeoDataPlacemark)) );

    QCOMPARE( finishSpy.count(), 0 );
    QCOMPARE( resultSpy.count(), 0 );

    QTime timer;
    timer.start();
    QString placemark = m_runnerManager.searchReverseGeocoding( m_coords );

    QVERIFY( timer.elapsed() < m_time );
    QCOMPARE( resultSpy.count(), 1 );
    QVERIFY( !placemark.isEmpty() );
    QCOMPARE( finishSpy.count(), 1 );

    // second search is optimised
    finishSpy.clear();
    resultSpy.clear();
    timer.start();
    placemark = m_runnerManager.searchReverseGeocoding( m_coords );

    QVERIFY( timer.elapsed() < m_time );
    QCOMPARE( resultSpy.count(), 1 );
    QVERIFY( !placemark.isEmpty() );
    QCOMPARE( finishSpy.count(), 1 );
}

void MarbleRunnerManagerTest::testAsyncReverse_data()
{
    QTest::addColumn<GeoDataCoordinates>( "coordinates" );

    addRow() << m_coords;
}

void MarbleRunnerManagerTest::testAsyncReverse()
{
    MarbleRunnerManager m_runnerManager(&m_pluginManager, this);

    QSignalSpy finishSpy( &m_runnerManager, SIGNAL(reverseGeocodingFinished()) );
    QSignalSpy resultSpy( &m_runnerManager, SIGNAL(reverseGeocodingFinished(GeoDataCoordinates,GeoDataPlacemark)) );

    QEventLoop loop;
    connect( &m_runnerManager, SIGNAL( reverseGeocodingFinished() ),
             &loop, SLOT( quit() ) );

    QFETCH( GeoDataCoordinates, coordinates );
    m_runnerManager.reverseGeocoding( coordinates );

    loop.exec();

    QCOMPARE( resultSpy.count(), 1 );
    QCOMPARE( finishSpy.count(), 1 );

    QThreadPool::globalInstance()->waitForDone();
}

void MarbleRunnerManagerTest::testSyncRouting()
{
    MarbleRunnerManager m_runnerManager(&m_pluginManager, this);

    QSignalSpy finishSpy( &m_runnerManager, SIGNAL(routingFinished()) );
    QSignalSpy resultSpy( &m_runnerManager, SIGNAL( routeRetrieved(GeoDataDocument*)) );

    QCOMPARE( finishSpy.count(), 0 );
    QCOMPARE( resultSpy.count(), 0 );

    QTime timer;
    timer.start();
    QVector<GeoDataDocument*> routes = m_runnerManager.searchRoute( &m_request );

    QVERIFY( timer.elapsed() < m_time );
    QVERIFY( resultSpy.count() > 0 );
    QVERIFY( !routes.isEmpty() );
    QCOMPARE( finishSpy.count(), 1 );
}

void MarbleRunnerManagerTest::testAsyncRouting_data()
{
    QTest::addColumn<QList<GeoDataCoordinates> >( "coordinatesList" );

    addRow() << ( QList<GeoDataCoordinates>() << m_coords << m_coords2 );
}

void MarbleRunnerManagerTest::testAsyncRouting()
{
    MarbleRunnerManager m_runnerManager(&m_pluginManager, this);

    QSignalSpy finishSpy( &m_runnerManager, SIGNAL(routingFinished()) );
    QSignalSpy resultSpy( &m_runnerManager, SIGNAL( routeRetrieved(GeoDataDocument*)) );

    QEventLoop loop;
    connect( &m_runnerManager, SIGNAL( routingFinished() ),
             &loop, SLOT( quit() ) );

    QFETCH( QList<GeoDataCoordinates>, coordinatesList );
    RouteRequest request;
    foreach( const GeoDataCoordinates &coordinates, coordinatesList ) {
        request.append( coordinates );
    }

    m_runnerManager.retrieveRoute( &request );

    loop.exec();

    QVERIFY( resultSpy.count() > 0 );
    QCOMPARE( finishSpy.count(), 1 );

    QThreadPool::globalInstance()->waitForDone();
}

void MarbleRunnerManagerTest::testSyncParsing()
{
    MarbleRunnerManager m_runnerManager(&m_pluginManager, this);

    QSignalSpy finishSpy( &m_runnerManager, SIGNAL( parsingFinished()) );
    QSignalSpy resultSpy( &m_runnerManager, SIGNAL( parsingFinished(GeoDataDocument*,QString)) );

    QCOMPARE( finishSpy.count(), 0 );
    QCOMPARE( resultSpy.count(), 0 );

    QTime timer;
    timer.start();
    GeoDataDocument* file = m_runnerManager.openFile( m_fileName );

    QVERIFY( timer.elapsed() < m_time );
    QVERIFY( resultSpy.count() > 0 );
    QVERIFY( file != 0 );
    QCOMPARE( finishSpy.count(), 1 );
}

void MarbleRunnerManagerTest::testAsyncParsing_data()
{
    QTest::addColumn<QString>( "fileName" );
    QTest::addColumn<int>( "resultCount" );

    addRow() << MarbleDirs::path( "placemarks/otherplacemarks.cache" ) << 1;

    addRow() << MarbleDirs::path( "mwdbii/DATELINE.PNT" )    << 1;
    addRow() << MarbleDirs::path( "mwdbii/PCOAST.PNT" )      << 1;
    addRow() << MarbleDirs::path( "mwdbii/PGLACIER.PNT" )    << 1;
    addRow() << MarbleDirs::path( "mwdbii/PLAKEISLAND.PNT" ) << 1;
    addRow() << MarbleDirs::path( "mwdbii/PDIFFBORDER.PNT" ) << 1;
    addRow() << MarbleDirs::path( "mwdbii/PISLAND.PNT" )     << 1;
    addRow() << MarbleDirs::path( "mwdbii/PLAKE.PNT" )       << 1;
    addRow() << MarbleDirs::path( "mwdbii/PUSA48.DIFF.PNT" ) << 1;
    addRow() << MarbleDirs::path( "mwdbii/RIVER.PNT" )       << 1;

    addRow() << MarbleDirs::path( "flags/flag_tv.svg" ) << 0;
}

void MarbleRunnerManagerTest::testAsyncParsing()
{
    MarbleRunnerManager m_runnerManager(&m_pluginManager, this);

    QSignalSpy finishSpy( &m_runnerManager, SIGNAL( parsingFinished() ) );
    QSignalSpy resultSpy( &m_runnerManager, SIGNAL( parsingFinished(GeoDataDocument*,QString)) );

    QEventLoop loop;
    connect( &m_runnerManager, SIGNAL( parsingFinished() ),
             &loop, SLOT( quit() ), Qt::QueuedConnection );

    QFETCH( QString, fileName );
    QFETCH( int, resultCount );

    m_runnerManager.parseFile( fileName );

    loop.exec();


    QCOMPARE( resultSpy.count(), resultCount );
    QCOMPARE( finishSpy.count(), 1 );

    QThreadPool::globalInstance()->waitForDone();
}

}

QTEST_MAIN( Marble::MarbleRunnerManagerTest )

#include "MarbleRunnerManagerTest.moc"
