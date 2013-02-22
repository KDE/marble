//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013       Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include <QtTest/QtTest>
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
    TestDataPluginModel( QObject *parent = 0 ) :
        AbstractDataPluginModel( "test", parent )
    {}

protected:
    void getAdditionalItems(const GeoDataLatLonAltBox &box, const MarbleModel *model, qint32 number)
    {
        Q_UNUSED( box )
        Q_UNUSED( model )
        Q_UNUSED( number )
    }
};

class AbstractDataPluginModelTest : public QObject
{
    Q_OBJECT

 private slots:
    void defaultConstructor();

    void destructor();

    void addItemToList_data();
    void addItemToList();

    void addItemToList_keepExisting_data();
    void addItemToList_keepExisting();
};

void AbstractDataPluginModelTest::defaultConstructor()
{
    const TestDataPluginModel model;

    QCOMPARE( model.isFavoriteItemsOnly(), false );
}

void AbstractDataPluginModelTest::destructor()
{
    QPointer<AbstractDataPluginItem> item( new TestDataPluginItem );
    item->setId( "foo" );

    QEventLoop loop;
    connect( item, SIGNAL(destroyed()), &loop, SLOT(quit()) );

    {
        TestDataPluginModel model;
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

    TestDataPluginModel model;

    QVERIFY( !model.itemExists( "foo" ) );
    QVERIFY( model.findItem( "foo" ) == 0 );

    TestDataPluginItem *item = new TestDataPluginItem();
    item->setInitialized( initialized );
    item->setId( "foo" );

    QSignalSpy itemsUpdatedSpy( &model, SIGNAL( itemsUpdated() ) );

    model.addItemToList( item );

    QVERIFY( model.itemExists( "foo" ) );
    QVERIFY( model.findItem( "foo" ) != 0 );
    QCOMPARE( itemsUpdatedSpy.count() == 1, initialized );
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

    TestDataPluginModel model;

    QPointer<TestDataPluginItem> item( new TestDataPluginItem() );
    item->setId( "foo" );
    item->setInitialized( itemInitialized );
    model.addItemToList( item );

    QPointer<TestDataPluginItem> rejectedItem( new TestDataPluginItem() );
    rejectedItem->setId( "foo" );
    rejectedItem->setInitialized( rejectedInitialized );

    QEventLoop loop;
    connect( rejectedItem.data(), SIGNAL( destroyed() ), &loop, SLOT( quit() ) );

    QSignalSpy itemsUpdatedSpy( &model, SIGNAL( itemsUpdated() ) );

    model.addItemToList( rejectedItem );

    QTimer::singleShot( 5000, &loop, SLOT( quit() ) ); // watchdog timer
    loop.exec();

    QVERIFY( !item.isNull() );
    QVERIFY( rejectedItem.isNull() );
    QCOMPARE( itemsUpdatedSpy.count(), 0 );
}

QTEST_MAIN( AbstractDataPluginModelTest )

#include "AbstractDataPluginModelTest.moc"
