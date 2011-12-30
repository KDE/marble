//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>
//

#include <QtTest/QtTest>
#include <QtTest/QSignalSpy>

#include "MarbleDirs.h"
#include "MarbleRunnerManager.h"
#include "PluginManager.h"
#include "GeoDataPlacemark.h"
#include "routing/RouteRequest.h"

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
    void testAsyncPlacemarks();

    void testSyncReverse();
    void testAsyncReverse();

    void testSyncRouting();
    void testAsyncRouting();

    void testSyncParsing();
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
    MarbleDirs::setMarbleDataPath( DATA_PATH );
    MarbleDirs::setMarblePluginPath( PLUGIN_PATH );
    m_time = 30000;
    m_name = "Berlin";

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

void MarbleRunnerManagerTest::testAsyncPlacemarks()
{
    MarbleRunnerManager m_runnerManager(&m_pluginManager, this);

    QSignalSpy finishSpy( &m_runnerManager, SIGNAL(placemarkSearchFinished()) );
    QSignalSpy resultSpy( &m_runnerManager, SIGNAL(searchResultChanged(QVector<GeoDataPlacemark*>)) );

    QCOMPARE( finishSpy.count(), 0 );
    QCOMPARE( resultSpy.count(), 0 );

    QTime timer;
    timer.start();
    m_runnerManager.findPlacemarks(m_name);
    QTest::qWait(m_time);

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

void MarbleRunnerManagerTest::testAsyncReverse()
{
    MarbleRunnerManager m_runnerManager(&m_pluginManager, this);

    QSignalSpy finishSpy( &m_runnerManager, SIGNAL(reverseGeocodingFinished()) );
    QSignalSpy resultSpy( &m_runnerManager, SIGNAL(reverseGeocodingFinished(GeoDataCoordinates,GeoDataPlacemark)) );

    QCOMPARE( finishSpy.count(), 0 );
    QCOMPARE( resultSpy.count(), 0 );

    m_runnerManager.reverseGeocoding( m_coords );
    QTest::qWait( m_time );

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

void MarbleRunnerManagerTest::testAsyncRouting()
{
    MarbleRunnerManager m_runnerManager(&m_pluginManager, this);

    QSignalSpy finishSpy( &m_runnerManager, SIGNAL(routingFinished()) );
    QSignalSpy resultSpy( &m_runnerManager, SIGNAL( routeRetrieved(GeoDataDocument*)) );

    QCOMPARE( resultSpy.count(), 0 );
    QCOMPARE( finishSpy.count(), 0 );

    m_runnerManager.retrieveRoute( &m_request );
    QTest::qWait( m_time );

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

void MarbleRunnerManagerTest::testAsyncParsing()
{
    MarbleRunnerManager m_runnerManager(&m_pluginManager, this);

    QSignalSpy finishSpy( &m_runnerManager, SIGNAL( parsingFinished() ) );
    QSignalSpy resultSpy( &m_runnerManager, SIGNAL( parsingFinished(GeoDataDocument*,QString)) );

    QCOMPARE( resultSpy.count(), 0 );
    QCOMPARE( finishSpy.count(), 0 );

    m_runnerManager.parseFile( m_fileName );
    QTest::qWait( m_time );

    QVERIFY( resultSpy.count() > 0 );
    QCOMPARE( finishSpy.count(), 1 );

    QThreadPool::globalInstance()->waitForDone();
}

}

QTEST_MAIN( Marble::MarbleRunnerManagerTest )

#include "MarbleRunnerManagerTest.moc"
