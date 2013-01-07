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

namespace Marble
{

class AbstractFloatItemTest : public QObject
{
    Q_OBJECT

 public:
    AbstractFloatItemTest();

 private slots:
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

}

QTEST_MAIN( Marble::AbstractFloatItemTest )

#include "AbstractFloatItemTest.moc"
