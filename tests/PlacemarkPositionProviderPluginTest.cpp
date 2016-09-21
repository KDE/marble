//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012       Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "GeoDataTrack.h"
#include "GeoDataPlacemark.h"
#include "GeoDataAccuracy.h"
#include "PositionProviderPlugin.h"
#include "PluginManager.h"
#include "MarbleModel.h"
#include "TestUtils.h"

#include <QSignalSpy>

Q_DECLARE_METATYPE( Marble::GeoDataPlacemark * )

namespace Marble
{

class PlacemarkPositionProviderPluginTest : public QObject
{
    Q_OBJECT

 public:
    PlacemarkPositionProviderPluginTest();

    static PositionProviderPlugin *createUninitializedPlugin( const PluginManager *pluginManager );
    static PositionProviderPlugin *createInitializedPlugin( const MarbleModel *model );

 private Q_SLOTS:
    void initialize_data();
    void initialize();

    void setTrackedPlacemark_afterInitialize_data();
    void setTrackedPlacemark_afterInitialize();

    void setClockDateTime();

 private:
    const QDateTime m_minTime;
    const QDateTime m_maxTime;
    GeoDataPlacemark m_placemark1;
    GeoDataPlacemark m_placemark2;
};

PlacemarkPositionProviderPluginTest::PlacemarkPositionProviderPluginTest() :
    m_minTime( QDate( 2000, 1, 1 ), QTime( 0, 0 ) ),
    m_maxTime( QDate( 2000, 1, 2 ), QTime( 23, 59 ) )
{
    qRegisterMetaType<GeoDataCoordinates>( "GeoDataCoordinates" );
    qRegisterMetaType<GeoDataAccuracy>( "GeoDataAccuracy" );
    qRegisterMetaType<PositionProviderStatus>( "PositionProviderStatus" );

    GeoDataTrack *track = new GeoDataTrack;
    track->setInterpolate( true );
    track->addPoint( m_minTime, GeoDataCoordinates() );
    track->addPoint( m_maxTime, GeoDataCoordinates() );
    m_placemark1.setGeometry( track );

    m_placemark2 = m_placemark1;
}

PositionProviderPlugin *PlacemarkPositionProviderPluginTest::createUninitializedPlugin( const PluginManager *pluginManager )
{
    foreach ( const PositionProviderPlugin *plugin, pluginManager->positionProviderPlugins() ) {
        if (plugin->nameId() == QLatin1String("Placemark")) {
            PositionProviderPlugin *instance = plugin->newInstance();
            return instance;
        }
    }

    return 0;
}

PositionProviderPlugin *PlacemarkPositionProviderPluginTest::createInitializedPlugin( const MarbleModel *model )
{
    PositionProviderPlugin *const plugin = createUninitializedPlugin( model->pluginManager() );

    if ( plugin == 0 ) {
        return 0;
    }

    plugin->initialize();

    return plugin;
}

void PlacemarkPositionProviderPluginTest::initialize_data()
{
    QTest::addColumn<GeoDataPlacemark *>( "placemark" );

    GeoDataPlacemark *const nullPlacemark = 0;

    addRow() << nullPlacemark;
    addRow() << &m_placemark1;
}

void PlacemarkPositionProviderPluginTest::initialize()
{
    QFETCH( GeoDataPlacemark *, placemark );
    const PositionProviderStatus expectedStatus = placemark ? PositionProviderStatusAvailable : PositionProviderStatusUnavailable;
    const int expectedStatusCount = placemark ? 1 : 0;
    const int expectedPositionCount = placemark ? 1 : 0;

    MarbleModel model;
    model.setClockDateTime( m_minTime ); // FIXME crashes when this line is removed

    model.setTrackedPlacemark( placemark );
    QVERIFY( model.trackedPlacemark() == placemark );

    PositionProviderPlugin *const plugin = createUninitializedPlugin( model.pluginManager() );
    QVERIFY2( plugin != 0, "Need a PlacemarkPositionProviderPlugin!" );

    QCOMPARE( plugin->status(), PositionProviderStatusUnavailable );

    QSignalSpy statusChangedSpy( plugin, SIGNAL(statusChanged(PositionProviderStatus)) );
    QSignalSpy positionChangedSpy( plugin, SIGNAL(positionChanged(GeoDataCoordinates,GeoDataAccuracy)) );

    plugin->initialize();

    QCOMPARE( plugin->status(), expectedStatus );
    QCOMPARE( statusChangedSpy.count(), expectedStatusCount );
    QCOMPARE( positionChangedSpy.count(), expectedPositionCount );

    delete plugin;
}

void PlacemarkPositionProviderPluginTest::setTrackedPlacemark_afterInitialize_data()
{
    QTest::addColumn<GeoDataPlacemark *>( "initialPlacemark" );
    QTest::addColumn<GeoDataPlacemark *>( "newPlacemark" );
    QTest::addColumn<int>( "expectedStatusCount" );

    GeoDataPlacemark *const nullPlacemark = 0;

    addRow() << nullPlacemark << nullPlacemark << 0;
    addRow() << nullPlacemark << &m_placemark1 << 1;
    addRow() << &m_placemark1 << nullPlacemark << 1;
    addRow() << &m_placemark1 << &m_placemark2 << 2;
}

void PlacemarkPositionProviderPluginTest::setTrackedPlacemark_afterInitialize()
{
    QFETCH( GeoDataPlacemark *, initialPlacemark );
    QFETCH( GeoDataPlacemark *, newPlacemark );
    QFETCH( int, expectedStatusCount );
    const PositionProviderStatus expectedStatus = newPlacemark ? PositionProviderStatusAvailable : PositionProviderStatusUnavailable;
    const int expectedPositionCount = newPlacemark ? 1 : 0;

    MarbleModel model;
    model.setClockDateTime( m_minTime ); // FIXME crashes when this line is removed

    model.setTrackedPlacemark( initialPlacemark );
    QVERIFY( model.trackedPlacemark() == initialPlacemark );

    PositionProviderPlugin *const plugin = createInitializedPlugin( &model );
    QVERIFY2( plugin != 0, "Need a PlacemarkPositionProviderPlugin!" );

    QSignalSpy statusChangedSpy( plugin, SIGNAL(statusChanged(PositionProviderStatus)) );
    QSignalSpy positionChangedSpy( plugin, SIGNAL(positionChanged(GeoDataCoordinates,GeoDataAccuracy)) );

    model.setTrackedPlacemark( newPlacemark );

    QCOMPARE( plugin->status(), expectedStatus );
    QCOMPARE( statusChangedSpy.count(), expectedStatusCount );
    QCOMPARE( positionChangedSpy.count(), expectedPositionCount );

    delete plugin;
}

void PlacemarkPositionProviderPluginTest::setClockDateTime()
{
    MarbleModel model;

    model.setClockDateTime( m_minTime ); // FIXME crashes when this line is removed

    model.setTrackedPlacemark( &m_placemark1 );

    PositionProviderPlugin *const plugin = createInitializedPlugin( &model );
    QVERIFY2( plugin != 0, "Need a PlacemarkPositionProviderPlugin!" );

    QSignalSpy positionChangedSpy( plugin, SIGNAL(positionChanged(GeoDataCoordinates,GeoDataAccuracy)) );

    model.setClockDateTime( m_minTime );

    QCOMPARE( positionChangedSpy.count(), 1 );

    model.setClockDateTime( m_minTime ); // FIXME crashes when using m_maxTime

    QCOMPARE( positionChangedSpy.count(), 2 );

    delete plugin;
}

}

QTEST_MAIN( Marble::PlacemarkPositionProviderPluginTest )

#include "PlacemarkPositionProviderPluginTest.moc"
