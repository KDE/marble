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
#include <QtCore/qmetatype.h>

#include "MarbleModel.h"
#include "PluginManager.h"
#include "AbstractFloatItem.h"

Q_DECLARE_METATYPE( const Marble::AbstractFloatItem * )

using namespace Marble;

class NullFloatItem : public AbstractFloatItem
{
 public:
    NullFloatItem() :
        AbstractFloatItem( 0 )
    {}

    NullFloatItem( const MarbleModel *model ) :
        AbstractFloatItem( model )
    {}

    QString name() const { return "Null Float Item"; }
    QString nameId() const { return "null"; }
    QString version() const { return "0.0"; }
    QString description() const { return "A null float item just for testing."; }
    QIcon icon() const { return QIcon(); }
    QString copyrightYears() const { return "2013"; }
    QList<PluginAuthor> pluginAuthors() const { return QList<PluginAuthor>() << PluginAuthor( "Bernhard Beschow", "bbeschow@cs.tu-berlin.de" ); }
    void initialize() {}
    bool isInitialized() const { return true; }
    QStringList backendTypes() const { return QStringList() << "null"; }
    QString guiString() const { return "Null"; }
    RenderPlugin *newInstance( const MarbleModel * ) const { return 0; }
};

class AbstractFloatItemTest : public QObject
{
    Q_OBJECT

 public:
    AbstractFloatItemTest();

 private slots:
    void defaultConstructor();

    void newInstance_data();
    void newInstance();

 private:
    MarbleModel m_model;
    QList<const AbstractFloatItem *> m_factories;
};

AbstractFloatItemTest::AbstractFloatItemTest()
{
    foreach ( const RenderPlugin *plugin, m_model.pluginManager()->renderPlugins() ) {
        const AbstractFloatItem *const factory = qobject_cast<const AbstractFloatItem *>( plugin );
        if ( !factory )
            continue;

        m_factories << factory;
    }
}

void AbstractFloatItemTest::defaultConstructor()
{
    const NullFloatItem item;

    QCOMPARE( item.renderPolicy(), QString( "ALWAYS") );
    QCOMPARE( item.renderPosition(), QStringList( "FLOAT_ITEM" ) );
    QCOMPARE( item.cacheMode(), AbstractFloatItem::ItemCoordinateCache );
    QCOMPARE( item.frame(), AbstractFloatItem::RectFrame );
    QCOMPARE( item.padding(), 4. );
    QCOMPARE( item.contentSize(), QSizeF( 150, 50 ) );
    QCOMPARE( item.position(), QPointF( 10, 10 ) );
    QCOMPARE( item.visible(), true );
    QCOMPARE( item.positionLocked(), true );
}

void AbstractFloatItemTest::newInstance_data()
{
    QTest::addColumn<const AbstractFloatItem *>( "factory" );

    foreach ( const AbstractFloatItem *factory, m_factories ) {
        QTest::newRow( factory->nameId().toAscii() ) << factory;
    }
}

void AbstractFloatItemTest::newInstance()
{
    QFETCH( const AbstractFloatItem *, factory );

    RenderPlugin *const instance = factory->newInstance( &m_model );

    QVERIFY( qobject_cast<AbstractFloatItem *>( instance ) != 0 );

    delete instance;
}

QTEST_MAIN( AbstractFloatItemTest )

#include "AbstractFloatItemTest.moc"
