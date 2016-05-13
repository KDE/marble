//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013       Adrian Draghici <draghici.adrian.b@gmail.com>
//


#include "MarbleGlobal.h"
#include "MarbleMath.h"
#include "TestUtils.h"

namespace Marble
{

class TestGudermannian : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testGudermannian_data();
    void testGudermannian();
};

void TestGudermannian::testGudermannian_data()
{
    QTest::addColumn<qreal>( "angle" );

    addRow() << 1.0;
    addRow() << 5.0;
    addRow() << 15.0;
    addRow() << 20.0;
    addRow() << 30.0;
    addRow() << 40.0;
    addRow() << 60.0;
    addRow() << 70.0;
    addRow() << 80.0;
}

void TestGudermannian::testGudermannian()
{
    QFETCH( qreal, angle );

    QFUZZYCOMPARE( atan( sinh ( angle * DEG2RAD ) ) * RAD2DEG, gd( angle * DEG2RAD ) * RAD2DEG, 0.1 );
}

}

QTEST_MAIN( Marble::TestGudermannian )

#include "TestGudermannian.moc"
