//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013       Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "TestUtils.h"

#include "AbstractDataPluginModel.h"

#include "AbstractDataPluginItem.h"
#include "MarbleModel.h"
#include "ViewportParams.h"

#include <QTimer>
#include <QSignalSpy>

using namespace Marble;

class TestDataPluginItem : public AbstractDataPluginItem
{
    Q_OBJECT

public:
    TestDataPluginItem( QObject *parent = 0 ) :
        AbstractDataPluginItem( parent ),
        m_initialized( false )
    {}

    void setInitialized( bool initialized ) { m_initialized = initialized; }

    bool initialized() const { return m_initialized; }
    bool operator<( const AbstractDataPluginItem *other ) const { return this < other; }

private:
    bool m_initialized;
};

class TestDataPluginModel : public AbstractDataPluginModel
{
    Q_OBJECT

public:
    TestDataPluginModel( const MarbleModel *marbleModel, QObject *parent = 0 ) :
        AbstractDataPluginModel( "test", marbleModel, parent )
    {}

protected:
    void getAdditionalItems(const GeoDataLatLonAltBox &box, qint32 number)
    {
        Q_UNUSED( box )
        Q_UNUSED( number )
    }
};

class AbstractDataPluginModelTest : public QObject
{
    Q_OBJECT

public:
    AbstractDataPluginModelTest( QObject *parent = 0 ) :
        QObject( parent )
    {}

private Q_SLOTS:
    void init_testcase();

    void defaultConstructor();

    void destructor();

    void addItemToList_data();
    void addItemToList();

    void addItemToList_keepExisting_data();
    void addItemToList_keepExisting();

    void switchMapTheme_data();
    void switchMapTheme();

    void setFavoriteItemsOnly_data();
    void setFavoriteItemsOnly();

    void itemsVersusInitialized_data();
    void itemsVersusInitialized();

    void itemsVersusAddedAngularResolution();

    void itemsVersusSetSticky();

 private:
    const MarbleModel m_marbleModel;
    static const ViewportParams fullViewport;
};

const ViewportParams AbstractDataPluginModelTest::fullViewport( Equirectangular, 0, 0, 100, QSize( 230, 230 ) );

void AbstractDataPluginModelTest::init_testcase()
{
    QCOMPARE( GeoDataLatLonBox( fullViewport.viewLatLonAltBox() ), GeoDataLatLonBox( 90, -90, 180, -180, GeoDataCoordinates::Degree ) );
}

void AbstractDataPluginModelTest::defaultConstructor()
{
    const TestDataPluginModel model( &m_marbleModel );

    QCOMPARE( model.isFavoriteItemsOnly(), false );
}

void AbstractDataPluginModelTest::destructor()
{
    QPointer<AbstractDataPluginItem> item( new TestDataPluginItem );
    item->setId( "foo" );

    QEventLoop loop;
    connect( item, SIGNAL(destroyed()), &loop, SLOT(quit()) );

    {
        TestDataPluginModel model( &m_marbleModel );
        model.addItemToList( item );

        QVERIFY( model.itemExists( "foo" ) );
    }

    QTimer::singleShot( 5000, &loop, SLOT(quit()) ); // watchdog timer
    loop.exec();

    QVERIFY( item.isNull() );
}

void AbstractDataPluginModelTest::addItemToList_data()
{
    QTest::addColumn<bool>( "initialized" );

    const bool isInitialized = true;

    addRow() << isInitialized;
    addRow() << !isInitialized;
}

void AbstractDataPluginModelTest::addItemToList()
{
    QFETCH( bool, initialized );

    TestDataPluginModel model( &m_marbleModel );

    QVERIFY( model.isFavoriteItemsOnly() == false );
    QVERIFY( !model.itemExists( "foo" ) );
    QVERIFY( model.findItem( "foo" ) == 0 );

    TestDataPluginItem *item = new TestDataPluginItem();
    item->setInitialized( initialized );
    item->setId( "foo" );

    QSignalSpy itemsUpdatedSpy( &model, SIGNAL(itemsUpdated()) );

    model.addItemToList( item );

    QVERIFY( model.itemExists( "foo" ) );
    QCOMPARE( model.findItem( "foo" ), item );
    QCOMPARE( itemsUpdatedSpy.count() == 1, initialized );
    QCOMPARE( static_cast<bool>( model.items( &fullViewport, 1 ).contains( item ) ), initialized );
}

void AbstractDataPluginModelTest::addItemToList_keepExisting_data()
{
    QTest::addColumn<bool>( "itemInitialized" );
    QTest::addColumn<bool>( "rejectedInitialized" );

    const bool isInitialized = true;

    addRow() << isInitialized << isInitialized;
    addRow() << isInitialized << !isInitialized;
    addRow() << !isInitialized << isInitialized;
    addRow() << !isInitialized << !isInitialized;
}

void AbstractDataPluginModelTest::addItemToList_keepExisting()
{
    QFETCH( bool, itemInitialized );
    QFETCH( bool, rejectedInitialized );

    TestDataPluginModel model( &m_marbleModel );

    QPointer<TestDataPluginItem> item( new TestDataPluginItem() );
    item->setId( "foo" );
    item->setInitialized( itemInitialized );
    model.addItemToList( item );

    QPointer<TestDataPluginItem> rejectedItem( new TestDataPluginItem() );
    rejectedItem->setId( "foo" );
    rejectedItem->setInitialized( rejectedInitialized );

    QEventLoop loop;
    connect( rejectedItem.data(), SIGNAL(destroyed()), &loop, SLOT(quit()) );

    QSignalSpy itemsUpdatedSpy( &model, SIGNAL(itemsUpdated()) );

    model.addItemToList( rejectedItem );

    QTimer::singleShot( 5000, &loop, SLOT(quit()) ); // watchdog timer
    loop.exec();

    QVERIFY( !item.isNull() );
    QVERIFY( rejectedItem.isNull() );
    QCOMPARE( model.findItem( item->id() ), item.data() );
    QCOMPARE( itemsUpdatedSpy.count(), 0 );
}

void AbstractDataPluginModelTest::switchMapTheme_data()
{
    QTest::addColumn<QString>( "mapThemeId" );
    QTest::addColumn<bool>( "planetChanged" );

    addRow() << "earth/bluemarble/bluemarble.dgml" << false;
    addRow() << "moon/clementine/clementine.dgml" << true;
}

void AbstractDataPluginModelTest::switchMapTheme()
{
    QFETCH( QString, mapThemeId );
    QFETCH( bool, planetChanged );

    MarbleModel marbleModel;

    marbleModel.setMapThemeId( "earth/openstreetmap/openstreetmap.dgml" );
    QCOMPARE( marbleModel.mapThemeId(), QString( "earth/openstreetmap/openstreetmap.dgml" ) );

    TestDataPluginModel model( &marbleModel );

    TestDataPluginItem *const item = new TestDataPluginItem();
    item->setId( "foo" );
    model.addItemToList( item );

    QCOMPARE( model.findItem( "foo" ), item );

    marbleModel.setMapThemeId( mapThemeId );
    QCOMPARE( marbleModel.mapThemeId(), mapThemeId );

    if ( planetChanged ) {
        QCOMPARE( model.findItem( "foo" ), static_cast<AbstractDataPluginItem *>( 0 ) );
    }
    else {
        QCOMPARE( model.findItem( "foo" ), item );
    }
}

void AbstractDataPluginModelTest::setFavoriteItemsOnly_data()
{
    QTest::addColumn<bool>( "itemIsFavorite" );
    QTest::addColumn<bool>( "favoriteItemsOnly" );

    addRow() << true << true;
    addRow() << true << false;
    addRow() << false << true;
    addRow() << false << false;
}

void AbstractDataPluginModelTest::setFavoriteItemsOnly()
{
    QFETCH( bool, itemIsFavorite );
    QFETCH( bool, favoriteItemsOnly );

    TestDataPluginItem *item = new TestDataPluginItem;
    item->setId( "foo" );
    item->setInitialized( true );
    item->setFavorite( itemIsFavorite );

    TestDataPluginModel model( &m_marbleModel );
    model.setFavoriteItemsOnly( favoriteItemsOnly );
    model.addItemToList( item );

    QVERIFY( model.findItem( item->id() ) == item );

    const bool visible = !favoriteItemsOnly || itemIsFavorite;

    QCOMPARE( static_cast<bool>( model.items( &fullViewport, 1 ).contains( item ) ), visible );
}

void AbstractDataPluginModelTest::itemsVersusInitialized_data()
{
    QTest::addColumn<bool>( "initialized" );

    addRow() << true;
    addRow() << false;
}

void AbstractDataPluginModelTest::itemsVersusInitialized()
{
    QFETCH( bool, initialized );

    TestDataPluginItem *item = new TestDataPluginItem;
    item->setInitialized( initialized );

    TestDataPluginModel model( &m_marbleModel );
    model.addItemToList( item );

    QCOMPARE( static_cast<bool>( model.items( &fullViewport, 1 ).contains( item ) ), initialized );
}

void AbstractDataPluginModelTest::itemsVersusAddedAngularResolution()
{
    const ViewportParams zoomedViewport( Equirectangular, 0, 0, 10000, QSize( 230, 230 ) );

    TestDataPluginItem *item = new TestDataPluginItem;
    item->setInitialized( true );

    TestDataPluginModel model( &m_marbleModel );
    model.addItemToList( item );

    QVERIFY( model.items( &zoomedViewport, 1 ).contains( item ) ); // calls setAddedAngularResolution()
    QVERIFY( !model.items( &fullViewport, 1 ).contains( item ) ); // addedAngularResolution() is too low
}

void AbstractDataPluginModelTest::itemsVersusSetSticky()
{
    const ViewportParams zoomedViewport( Equirectangular, 0, 0, 10000, QSize( 230, 230 ) );

    TestDataPluginItem *item = new TestDataPluginItem;
    item->setInitialized( true );
    item->setSticky( false );

    TestDataPluginModel model( &m_marbleModel );
    model.addItemToList( item );

    QVERIFY( model.items( &zoomedViewport, 1 ).contains( item ) );
    QVERIFY( !model.items( &fullViewport, 1 ).contains( item ) );

    item->setSticky( true );

    QVERIFY( model.items( &zoomedViewport, 1 ).contains( item ) );
    QVERIFY( model.items( &fullViewport, 1 ).contains( item ) );

    item->setSticky( false );

    QVERIFY( model.items( &zoomedViewport, 1 ).contains( item ) );
    QVERIFY( !model.items( &fullViewport, 1 ).contains( item ) );
}

QTEST_MAIN( AbstractDataPluginModelTest )

#include "AbstractDataPluginModelTest.moc"
