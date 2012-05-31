//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011       Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include <QtCore/QMetaType>
#include <QtTest/QtTest>
#include "Quaternion.h"
#include "MarbleGlobal.h"

Q_DECLARE_METATYPE( Marble::Quaternion )

namespace QTest
{

bool qCompare(qreal val1, qreal val2, qreal epsilon, const char *actual, const char *expected, const char *file, int line)
{
    return ( qAbs( val1 - val2 ) < epsilon )
        ? compare_helper( true, "COMPARE()", file, line )
        : compare_helper( false, "Compared qreals are not the same", toString( val1 ), toString( val2 ), actual, expected, file, line );
}

}

#define QFUZZYCOMPARE(actual, expected, epsilon) \
do {\
    if (!QTest::qCompare(actual, expected, epsilon, #actual, #expected, __FILE__, __LINE__))\
        return;\
} while (0)

#define addRow() QTest::newRow( QString("line %1").arg( __LINE__ ).toAscii().data() )

namespace Marble
{

class QuaternionTest : public QObject
{
    Q_OBJECT

 private slots:
    void testEuler_data();
    void testEuler();

    void testSpherical_data();
    void testSpherical();
};

void QuaternionTest::testEuler_data()
{
    QTest::addColumn<qreal>( "pitch" );
    QTest::addColumn<qreal>( "yaw" );
    QTest::addColumn<qreal>( "roll" );

    addRow() << 0.0 << 0.0 << 0.0;

    addRow() << -180.0 << 0.0 << 0.0;
    addRow() <<  -90.0 << 0.0 << 0.0;
    addRow() <<   90.0 << 0.0 << 0.0;
    addRow() <<  180.0 << 0.0 << 0.0;

    addRow() << -180.0 << -90.0 << 0.0;
    addRow() <<  -90.0 << -90.0 << 0.0;
    addRow() <<    0.0 << -90.0 << 0.0;
    addRow() <<   90.0 << -90.0 << 0.0;
    addRow() <<  180.0 << -90.0 << 0.0;

    addRow() << -180.0 << 90.0 << 0.0;
    addRow() <<  -90.0 << 90.0 << 0.0;
    addRow() <<    0.0 << 90.0 << 0.0;
    addRow() <<   90.0 << 90.0 << 0.0;
    addRow() <<  180.0 << 90.0 << 0.0;

    addRow() << -180.0 << 0.0 << 90.0;
    addRow() <<  -90.0 << 0.0 << 90.0;
    addRow() <<   90.0 << 0.0 << 90.0;
    addRow() <<  180.0 << 0.0 << 90.0;

    addRow() << -180.0 << 0.0 << -90.0;
    addRow() <<  -90.0 << 0.0 << -90.0;
    addRow() <<   90.0 << 0.0 << -90.0;
    addRow() <<  180.0 << 0.0 << -90.0;

//
// the following tests currently fail, which should be fixed:
//

//    addRow() << 10.0 << 20.0 << 30.0;

//    addRow() << -180.0 << -89.0 << 90.0;
//    addRow() <<  -90.0 << -89.0 << 90.0;
//    addRow() <<    0.0 << -89.0 << 90.0;
//    addRow() <<   90.0 << -89.0 << 90.0;
//    addRow() <<  180.0 << -89.0 << 90.0;

//    addRow() << -180.0 << 89.0 << 90.0;
//    addRow() <<  -90.0 << 89.0 << 90.0;
//    addRow() <<    0.0 << 89.0 << 90.0;
//    addRow() <<   90.0 << 89.0 << 90.0;
//    addRow() <<  180.0 << 89.0 << 90.0;

//    addRow() << -180.0 << -89.0 << -90.0;
//    addRow() <<  -90.0 << -89.0 << -90.0;
//    addRow() <<    0.0 << -89.0 << -90.0;
//    addRow() <<   90.0 << -89.0 << -90.0;
//    addRow() <<  180.0 << -89.0 << -90.0;

//    addRow() << -180.0 << 89.0 << -90.0;
//    addRow() <<  -90.0 << 89.0 << -90.0;
//    addRow() <<    0.0 << 89.0 << -90.0;
//    addRow() <<   90.0 << 89.0 << -90.0;
//    addRow() <<  180.0 << 89.0 << -90.0;
}

void QuaternionTest::testEuler()
{
    QFETCH( qreal, pitch );
    QFETCH( qreal, yaw );
    QFETCH( qreal, roll );

    const Quaternion quat = Quaternion::fromEuler( pitch * DEG2RAD, yaw * DEG2RAD, roll * DEG2RAD );

    QFUZZYCOMPARE( quat.pitch() * RAD2DEG, pitch, 0.0000001 );
    QFUZZYCOMPARE( quat.yaw()   * RAD2DEG, yaw,   0.0000001 );
    QFUZZYCOMPARE( quat.roll()  * RAD2DEG, roll,  0.0000001 );
}

void QuaternionTest::testSpherical_data()
{
    QTest::addColumn<qreal>( "lon" );
    QTest::addColumn<qreal>( "lat" );

    addRow() << 0.0 << 0.0;

    addRow() << -180.0 << 0.0;
    addRow() <<  -90.0 << 0.0;
    addRow() <<   90.0 << 0.0;
    addRow() <<  180.0 << 0.0;

    addRow() << -180.0 << -89.0;
    addRow() <<  -90.0 << -89.0;
    addRow() <<    0.0 << -89.0;
    addRow() <<   90.0 << -89.0;
    addRow() <<  180.0 << -89.0;

    addRow() << -180.0 << 89.0;
    addRow() <<  -90.0 << 89.0;
    addRow() <<    0.0 << 89.0;
    addRow() <<   90.0 << 89.0;
    addRow() <<  180.0 << 89.0;

//
// the following cases fail, which should be fixed
//
// related bugs:
// bug 285570: animated zooming looks incorrect in globe projection when the earth is upside down
//

//    addRow() << -180.0 << -179.0;
//    addRow() <<  -90.0 << -179.0;
//    addRow() <<    0.0 << -179.0;
//    addRow() <<   90.0 << -179.0;
//    addRow() <<  180.0 << -179.0;

//    addRow() << -180.0 << 179.0;
//    addRow() <<  -90.0 << 179.0;
//    addRow() <<    0.0 << 179.0;
//    addRow() <<   90.0 << 179.0;
//    addRow() <<  180.0 << 179.0;
}

void QuaternionTest::testSpherical()
{
    QFETCH( qreal, lon );
    QFETCH( qreal, lat );

    const Quaternion quat = Quaternion::fromSpherical( lon * DEG2RAD, lat * DEG2RAD );

    qreal _lon, _lat;
    quat.getSpherical( _lon, _lat );

    QCOMPARE( _lat * RAD2DEG, lat );
    QCOMPARE( _lon * RAD2DEG, lon );
}

}

QTEST_MAIN( Marble::QuaternionTest )

#include "QuaternionTest.moc"
