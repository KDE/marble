//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012       Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//


#include "GeoDataTreeModel.h"
#include "GeoDataAccuracy.h"
#include "PositionProviderPlugin.h"
#include "PositionTracking.h"
#include "TestUtils.h"

#include <QSignalSpy>

class FakeProvider : public Marble::PositionProviderPlugin
{
public:
    FakeProvider() :
        m_status( Marble::PositionProviderStatusUnavailable ),
        m_position(),
        m_accuracy(),
        m_speed( 0.0 ),
        m_direction( 0.0 ),
        m_timestamp()
    {}

    QString name() const           { return "fake plugin"; }
    QString guiString() const      { return "fake"; }
    QString nameId() const         { return "fakeplugin"; }
    QString version() const        { return "1.0"; }
    QString description() const    { return "plugin for testing"; }
    QIcon icon() const             { return QIcon(); }
    QString copyrightYears() const { return "2012"; }
    QVector<Marble::PluginAuthor> pluginAuthors() const { return QVector<Marble::PluginAuthor>(); }
    void initialize() {}
    bool isInitialized() const     { return true; }

    Marble::PositionProviderStatus status() const { return m_status; }
    Marble::GeoDataCoordinates position() const { return m_position; }
    Marble::GeoDataAccuracy accuracy() const { return m_accuracy; }
    qreal speed() const { return m_speed; }
    qreal direction() const { return m_direction; }
    QDateTime timestamp() const { return m_timestamp; }

    Marble::PositionProviderPlugin *newInstance() const { return 0; }

    void setStatus( Marble::PositionProviderStatus status );
    void setPosition( const Marble::GeoDataCoordinates &position,
                      const Marble::GeoDataAccuracy &accuracy,
                      qreal speed,
                      qreal direction,
                      const QDateTime &timestamp );

private:
    Marble::PositionProviderStatus m_status;
    Marble::GeoDataCoordinates m_position;
    Marble::GeoDataAccuracy m_accuracy;
    qreal m_speed;
    qreal m_direction;
    QDateTime m_timestamp;
};

void FakeProvider::setStatus( Marble::PositionProviderStatus status )
{
    const Marble::PositionProviderStatus oldStatus = m_status;

    m_status = status;

    if ( oldStatus != m_status ) {
        emit statusChanged( m_status );
    }
}

void FakeProvider::setPosition( const Marble::GeoDataCoordinates &position,
                                const Marble::GeoDataAccuracy &accuracy,
                                qreal speed,
                                qreal direction,
                                const QDateTime &timestamp )
{
    m_position = position;
    m_accuracy = accuracy;
    m_speed = speed;
    m_direction = direction;
    m_timestamp = timestamp;

    emit positionChanged( m_position, m_accuracy );
}

namespace Marble
{

class PositionTrackingTest : public QObject
{
    Q_OBJECT

 public:
    PositionTrackingTest();

 private Q_SLOTS:
    void construct();

    void statusChanged_data();
    void statusChanged();

    void setPositionProviderPlugin();

    void clearTrack();
};

PositionTrackingTest::PositionTrackingTest()
{
    qRegisterMetaType<GeoDataCoordinates>( "GeoDataCoordinates" );
    qRegisterMetaType<PositionProviderStatus>( "PositionProviderStatus" );
}

void PositionTrackingTest::construct()
{
    GeoDataTreeModel treeModel;
    const PositionTracking tracking( &treeModel );

    QCOMPARE( const_cast<PositionTracking &>( tracking ).positionProviderPlugin(), static_cast<PositionProviderPlugin *>( 0 ) );
    QCOMPARE( tracking.speed(), qreal( 0 ) );
    QCOMPARE( tracking.direction(), qreal( 0 ) );
    QCOMPARE( tracking.timestamp(), QDateTime() );
    QCOMPARE( tracking.accuracy(), GeoDataAccuracy() );
    QCOMPARE( tracking.trackVisible(), true );
    QCOMPARE( tracking.currentLocation(), GeoDataCoordinates() );
    QCOMPARE( tracking.status(), PositionProviderStatusUnavailable );
    QCOMPARE( tracking.isTrackEmpty(), true );

    QCOMPARE( treeModel.rowCount(), 1 );
    const QModelIndex indexPositionTracking = treeModel.index( 0, 0 );
    QCOMPARE( treeModel.data( indexPositionTracking, Qt::DisplayRole ).toString(), QString( "Position Tracking" ) );
    QCOMPARE( treeModel.rowCount( indexPositionTracking ), 2 );
    const QModelIndex indexCurrentPosition = treeModel.index( 0, 0, indexPositionTracking );
    QCOMPARE( treeModel.data( indexCurrentPosition, Qt::DisplayRole ).toString(), QString( "Current Position" ) );
    const QModelIndex indexCurrentTrack = treeModel.index( 1, 0, indexPositionTracking );
    QCOMPARE( treeModel.data( indexCurrentTrack, Qt::DisplayRole ).toString(), QString( "Current Track" ) );
}

void PositionTrackingTest::statusChanged_data()
{
    QTest::addColumn<PositionProviderStatus>( "finalStatus" );

    addRow() << PositionProviderStatusError;
    addRow() << PositionProviderStatusUnavailable;
    addRow() << PositionProviderStatusAcquiring;
    addRow() << PositionProviderStatusAvailable;
}

void PositionTrackingTest::statusChanged()
{
    QFETCH( PositionProviderStatus, finalStatus );
    const int expectedStatusChangedCount = ( finalStatus == PositionProviderStatusUnavailable ) ? 0 : 1;

    GeoDataTreeModel treeModel;
    PositionTracking tracking( &treeModel );

    QSignalSpy statusChangedSpy( &tracking, SIGNAL(statusChanged(PositionProviderStatus)) );

    FakeProvider provider;
    provider.setStatus( finalStatus );

    tracking.setPositionProviderPlugin( &provider );

    QCOMPARE( tracking.status(), finalStatus );
    QCOMPARE( statusChangedSpy.count(), expectedStatusChangedCount );
}

void PositionTrackingTest::setPositionProviderPlugin()
{
    const GeoDataCoordinates coordinates( 1.2, 0.9 );
    const GeoDataAccuracy accuracy( GeoDataAccuracy::Detailed, 10.0, 22.0 );
    const qreal speed = 32.8;
    const qreal direction = 49.7;
    const QDateTime timestamp( QDate( 1, 3, 1994 ) );

    GeoDataTreeModel treeModel;
    PositionTracking tracking( &treeModel );

    QSignalSpy gpsLocationSpy( &tracking, SIGNAL(gpsLocation(GeoDataCoordinates,qreal)) );

    QPointer<FakeProvider> provider( new FakeProvider );
    provider->setStatus( PositionProviderStatusAvailable );
    provider->setPosition( coordinates, accuracy, speed, direction, timestamp );

    tracking.setPositionProviderPlugin( provider );

    QCOMPARE( tracking.currentLocation(), coordinates );
    QCOMPARE( tracking.accuracy(), accuracy );
    QCOMPARE( tracking.speed(), speed );
    QCOMPARE( tracking.direction(), direction );
    QCOMPARE( tracking.timestamp(), timestamp );
    QCOMPARE( gpsLocationSpy.count(), 1 );

    tracking.setPositionProviderPlugin( 0 );

    QVERIFY( provider.isNull() );
}

void PositionTrackingTest::clearTrack()
{
    const GeoDataCoordinates position( 2.1, 0.8 );
    const GeoDataAccuracy accuracy( GeoDataAccuracy::Detailed, 10.0, 22.0 );
    const qreal speed = 32.8;
    const qreal direction = 49.7;
    const QDateTime timestamp( QDate( 1, 3, 1994 ) );

    GeoDataTreeModel treeModel;
    PositionTracking tracking( &treeModel );

    FakeProvider provider;
    tracking.setPositionProviderPlugin( &provider );

    tracking.clearTrack();

    QVERIFY( tracking.isTrackEmpty() );

    provider.setStatus( PositionProviderStatusAvailable );

    provider.setPosition( position, accuracy, speed, direction, timestamp );

    QVERIFY( !tracking.isTrackEmpty() );

    tracking.clearTrack();

    QVERIFY( tracking.isTrackEmpty() );
}

}

QTEST_MAIN( Marble::PositionTrackingTest )

#include "PositionTrackingTest.moc"
