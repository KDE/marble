//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013       Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include <QtTest>
#include "TestUtils.h"

#include "AbstractDataPluginModel.h"

#include "AbstractDataPluginItem.h"
#include "MarbleModel.h"
#include "ViewportParams.h"

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

    QString itemType() const { return "test"; }
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

 private slots:
    void init_testcase();

    void defaultConstructor();

    void destructor();

    void addItemToList_data();
    void addItemToList();

    void addItemToList_keepExisting_data();
    void addItemToList_keepExisting();

    void setFavoriteItemsOnly_data();
    void setFavoriteItemsOnly();

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
    QTest::addColumn<QString>( "planetId" );

    const bool isInitialized = true;

    addRow() << isInitialized << m_marbleModel.planetId();
    addRow() << !isInitialized << m_marbleModel.planetId();
    addRow() << isInitialized << QString( "Saturn" );
}

void AbstractDataPluginModelTest::addItemToList()
{
    QFETCH( bool, initialized );
    QFETCH( QString, planetId );

    TestDataPluginModel model( &m_marbleModel );

    QVERIFY( model.isFavoriteItemsOnly() == false );
    QVERIFY( !model.itemExists( "foo" ) );
    QVERIFY( model.findItem( "foo" ) == 0 );

    TestDataPluginItem *item = new TestDataPluginItem();
    item->setInitialized( initialized );
    item->setTarget( planetId );
    item->setId( "foo" );

    QSignalSpy itemsUpdatedSpy( &model, SIGNAL(itemsUpdated()) );

    model.addItemToList( item );

    QVERIFY( model.itemExists( "foo" ) );
    QCOMPARE( model.findItem( "foo" ), item );
    QCOMPARE( itemsUpdatedSpy.count() == 1, initialized );

    const bool visible = initialized && ( m_marbleModel.planetId() == planetId );

    QCOMPARE( static_cast<bool>( model.items( &fullViewport, 1 ).contains( item ) ), visible );
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
    item->setTarget( m_marbleModel.planetId() );
    item->setFavorite( itemIsFavorite );

    TestDataPluginModel model( &m_marbleModel );
    model.setFavoriteItemsOnly( favoriteItemsOnly );
    model.addItemToList( item );

    QVERIFY( model.findItem( item->id() ) == item );

    const bool visible = !favoriteItemsOnly || itemIsFavorite;

    QCOMPARE( static_cast<bool>( model.items( &fullViewport, 1 ).contains( item ) ), visible );
}

QTEST_MAIN( AbstractDataPluginModelTest )

#include "AbstractDataPluginModelTest.moc"
