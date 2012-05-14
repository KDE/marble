//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.

// Copyright 2010 Matias Kallio <matias.kallio@gmail.com>
// Copyright 2011 Friedrich W. H. Kossebau <kossebau@kde.org>
// Copyright 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>

#include <QtTest/QtTest>
#include <QtGui/QApplication>
#include "MarbleGlobal.h"
#include "MarbleWidget.h"
#include "AbstractFloatItem.h"
#include "GeoDataCoordinates.h"


using namespace Marble;


class TestGeoDataCoordinates : public QObject
{
    Q_OBJECT

private slots:
    void testConstruction();
    void testSet_Degree();
    void testSet_Radian();
    void testSetLongitude_Degree();
    void testSetLongitude_Radian();
    void testSetLatitude_Degree();
    void testSetLatitude_Radian();
    void testAltitude();
    void testOperatorAssignment();
    void testDetail();
    void testIsPole_data();
    void testIsPole();
    void testNotation();
    void testNormalizeLat_data();
    void testNormalizeLat();
    void testNormalizeLon_data();
    void testNormalizeLon();
    void testNormalize_data();
    void testNormalize();
    void testFromStringDMS_data();
    void testFromStringDMS();
    void testFromStringDM_data();
    void testFromStringDM();
    void testFromStringD_data();
    void testFromStringD();
    void testFromLocaleString_data();
    void testFromLocaleString();
    void testString_data();
    void testString();
    void testPack_data();
    void testPack();
};

/*
 * test constructors
 */
void TestGeoDataCoordinates::testConstruction()
{
    GeoDataCoordinates invalid1;

    QVERIFY(!invalid1.isValid());

    GeoDataCoordinates invalid2(invalid1);

    QVERIFY(!invalid2.isValid());
    QVERIFY(!invalid1.isValid());
    QCOMPARE(invalid1, invalid2);

    const qreal lon = 164.77;
    const qreal lat = 55.9;
    const qreal alt = 400.003;

    GeoDataCoordinates coordinates3(lon, lat, alt, GeoDataCoordinates::Degree);

    QVERIFY(coordinates3.isValid());
    QCOMPARE(coordinates3, GeoDataCoordinates(lon, lat, alt, GeoDataCoordinates::Degree));
    QVERIFY(coordinates3 != invalid1);
    QVERIFY(coordinates3 != invalid2);

    QCOMPARE(coordinates3.longitude(GeoDataCoordinates::Degree), lon);
    QCOMPARE(coordinates3.longitude(), lon*DEG2RAD);
    QEXPECT_FAIL("", "This should fail", Continue);
    QCOMPARE(coordinates3.longitude(), lon);
    QEXPECT_FAIL("", "This should fail", Continue);
    QCOMPARE(invalid1.longitude(GeoDataCoordinates::Degree), lon*DEG2RAD);

    QCOMPARE(coordinates3.latitude(GeoDataCoordinates::Degree), lat);
    QCOMPARE(coordinates3.latitude(), lat*DEG2RAD);
    QEXPECT_FAIL("", "This should fail", Continue);
    QCOMPARE(invalid1.latitude(), lat);
    QEXPECT_FAIL("", "This should fail", Continue);
    QCOMPARE(invalid1.latitude(GeoDataCoordinates::Degree), lat*DEG2RAD);

    QCOMPARE(coordinates3.altitude(), alt);

    qreal myLongitude = 0;
    qreal myLatitude = 0;

    coordinates3.geoCoordinates(myLongitude, myLatitude, GeoDataCoordinates::Degree);

    QCOMPARE(myLongitude, lon);
    QCOMPARE(myLatitude, lat);

    myLongitude = 0;
    myLatitude = 0;

    coordinates3.geoCoordinates(myLongitude, myLatitude);

    QCOMPARE(myLongitude, lon*DEG2RAD);
    QCOMPARE(myLatitude, lat*DEG2RAD);

    GeoDataCoordinates coordinates4(lon*DEG2RAD, lat*DEG2RAD, alt);

    QVERIFY(coordinates4.isValid());
    QCOMPARE(coordinates4, GeoDataCoordinates(lon, lat, alt, GeoDataCoordinates::Degree));
    QCOMPARE(coordinates4, coordinates3);
    QVERIFY(coordinates4 != invalid1);
    QVERIFY(coordinates4 != invalid2);

    GeoDataCoordinates coordinates5(coordinates3);

    QVERIFY(coordinates5.isValid());
    QCOMPARE(coordinates5, GeoDataCoordinates(lon, lat, alt, GeoDataCoordinates::Degree));
    QCOMPARE(coordinates5, coordinates3);
    QCOMPARE(coordinates5, coordinates4);
    QVERIFY(coordinates5 != invalid1);
    QVERIFY(coordinates5 != invalid2);

    GeoDataCoordinates coordinates6(invalid1.longitude(), invalid1.latitude(), invalid1.altitude(), GeoDataCoordinates::Radian, invalid1.detail());

    QVERIFY(coordinates6.isValid());  // it should be valid, even though
    QCOMPARE(coordinates6, invalid1); // it is equal to an invalid one
}

/*
 * test setting coordinates in degree
 */
void TestGeoDataCoordinates::testSet_Degree()
{
    const qreal lon = 345.8;
    const qreal lat = 70.3;
    const qreal alt = 1000.9;

    GeoDataCoordinates coordinates1; // invalid
    coordinates1.set(lon, lat, alt, GeoDataCoordinates::Degree);

    QVERIFY(coordinates1.isValid());

    GeoDataCoordinates coordinates2(coordinates1);
    coordinates2.set(0, 0, 0, GeoDataCoordinates::Degree);

    QVERIFY(coordinates2.isValid());
    QCOMPARE(coordinates1, GeoDataCoordinates(lon, lat, alt, GeoDataCoordinates::Degree));
    QCOMPARE(coordinates2, GeoDataCoordinates(0, 0, 0, GeoDataCoordinates::Degree));

}

/*
 * test setting coordinates in radian
 */
void TestGeoDataCoordinates::testSet_Radian()
{
    const qreal lon = 1.3;
    const qreal lat = 0.7;
    const qreal alt = 6886.44;

    GeoDataCoordinates coordinates1; // invalid
    coordinates1.set(lon, lat, alt);

    QVERIFY(coordinates1.isValid());

    GeoDataCoordinates coordinates2(coordinates1);
    coordinates2.set(0, 0, 0);

    QVERIFY(coordinates2.isValid());
    QCOMPARE(coordinates1, GeoDataCoordinates(lon, lat, alt));
    QCOMPARE(coordinates2, GeoDataCoordinates(0, 0, 0));
}

/*
 * test setLongitude() in degree
 */
void TestGeoDataCoordinates::testSetLongitude_Degree()
{
    const qreal lon = 143.8;

    GeoDataCoordinates coordinates1; // invalid
    coordinates1.setLongitude(lon, GeoDataCoordinates::Degree);

    QVERIFY(coordinates1.isValid());

    GeoDataCoordinates coordinates2(coordinates1);
    coordinates2.setLongitude(0, GeoDataCoordinates::Degree);

    QVERIFY(coordinates2.isValid());
    QCOMPARE(coordinates1, GeoDataCoordinates(lon, 0, 0, GeoDataCoordinates::Degree));
    QCOMPARE(coordinates2, GeoDataCoordinates(0, 0, 0, GeoDataCoordinates::Degree));
}

/*
 * test setLongitude() in radian
 */
void TestGeoDataCoordinates::testSetLongitude_Radian()
{
    const qreal lon = 2.5;

    GeoDataCoordinates coordinates1; // invalid
    coordinates1.setLongitude(lon);

    QVERIFY(coordinates1.isValid());

    GeoDataCoordinates coordinates2(coordinates1);
    coordinates2.setLongitude(0);

    QVERIFY(coordinates2.isValid());
    QCOMPARE(coordinates1, GeoDataCoordinates(lon, 0));
    QCOMPARE(coordinates2, GeoDataCoordinates(0, 0));
}

/*
 * test setLatitude() and latitude() in degree
 */
void TestGeoDataCoordinates::testSetLatitude_Degree()
{
    const qreal lat = 75.0;

    GeoDataCoordinates coordinates1; // invalid
    coordinates1.setLatitude(lat, GeoDataCoordinates::Degree);

    QVERIFY(coordinates1.isValid());

    GeoDataCoordinates coordinates2(coordinates1);
    coordinates2.setLatitude(0, GeoDataCoordinates::Degree);

    QVERIFY(coordinates2.isValid());
    QCOMPARE(coordinates1, GeoDataCoordinates(0, lat, 0, GeoDataCoordinates::Degree));
    QCOMPARE(coordinates2, GeoDataCoordinates(0, 0, 0, GeoDataCoordinates::Degree));
}

/*
 * test setLatitude() in radian
 */
void TestGeoDataCoordinates::testSetLatitude_Radian()
{
    const qreal lat = 1.2;

    GeoDataCoordinates coordinates1; // invalid
    coordinates1.setLatitude(lat);

    QVERIFY(coordinates1.isValid());

    GeoDataCoordinates coordinates2(coordinates1);
    coordinates2.setLatitude(0);

    QVERIFY(coordinates2.isValid());
    QCOMPARE(coordinates1, GeoDataCoordinates(0, lat));
    QCOMPARE(coordinates2, GeoDataCoordinates(0, 0));
}

/*
 * test setAltitude()
 */
void TestGeoDataCoordinates::testAltitude()
{
    const qreal alt = 400;

    GeoDataCoordinates coordinates1; // invalid
    coordinates1.setAltitude(alt);

    QVERIFY(coordinates1.isValid());

    GeoDataCoordinates coordinates2(coordinates1);
    coordinates2.setAltitude(0);

    QVERIFY(coordinates2.isValid());
    QCOMPARE(coordinates1, GeoDataCoordinates(0, 0, alt));
    QCOMPARE(coordinates2, GeoDataCoordinates(0, 0, 0));
}

void TestGeoDataCoordinates::testOperatorAssignment()
{
    const qreal lon = 123.4;
    const qreal lat = 56.7;
    const qreal alt = 890.1;

    const GeoDataCoordinates coordinates1(lon, lat, alt, GeoDataCoordinates::Degree);
    const GeoDataCoordinates coordinates2(0, 0, 0);

    GeoDataCoordinates coordinates3; // invalid
    coordinates3 = coordinates1;

    QVERIFY(coordinates3.isValid());
    QCOMPARE(coordinates1, GeoDataCoordinates(lon, lat, alt, GeoDataCoordinates::Degree)); // stays unmodified
    QCOMPARE(coordinates3, coordinates1);

    coordinates3 = GeoDataCoordinates();

    QVERIFY(!coordinates3.isValid());

    GeoDataCoordinates coordinates4(coordinates1);
    coordinates4 = coordinates2;

    QVERIFY(coordinates4.isValid());
    QCOMPARE(coordinates1, GeoDataCoordinates(lon, lat, alt, GeoDataCoordinates::Degree)); // stays unmodified
    QCOMPARE(coordinates2, GeoDataCoordinates(0, 0, 0)); // stays unmodified
    QCOMPARE(coordinates4, coordinates2);
}

/*
 * test setDetail() and detail()
 */
void TestGeoDataCoordinates::testDetail()
{
    const int detailnumber = 15;

    GeoDataCoordinates coordinates1;
    coordinates1.setDetail(detailnumber);

    GeoDataCoordinates coordinates2(coordinates1);
    coordinates2.setDetail(0);

    QCOMPARE(coordinates1.detail(), detailnumber);
}

/*
 * test setDefaultNotation() and defaultNotation
 */
void TestGeoDataCoordinates::testNotation()
{
    GeoDataCoordinates::setDefaultNotation(GeoDataCoordinates::Decimal);
    QVERIFY(GeoDataCoordinates::defaultNotation() == GeoDataCoordinates::Decimal);

    GeoDataCoordinates::setDefaultNotation(GeoDataCoordinates::DMS);
    QVERIFY(GeoDataCoordinates::defaultNotation() == GeoDataCoordinates::DMS);
}

/*
 * test data for testIsPole()
 */
void TestGeoDataCoordinates::testIsPole_data()
{
    QTest::addColumn<qreal>("lon");
    QTest::addColumn<qreal>("lat");
    QTest::addColumn<qreal>("alt");
    QTest::addColumn<QString>("pole");

    QTest::newRow("false") << 50.0 << 50.0 << 0.0 << "false_pole";
    QTest::newRow("south") << 0.0 << -90.0 << 0.0 << "south_pole";
    QTest::newRow("north") << 0.0 << 90.0 << 0.0 << "north_pole";
}

/*
 * Test isPole-method
 */
void TestGeoDataCoordinates::testIsPole()
{
    QFETCH(qreal, lon);
    QFETCH(qreal, lat);
    QFETCH(qreal, alt);
    QFETCH(QString, pole);

    GeoDataCoordinates coordinates1;

    if(pole == "false_pole") {
        coordinates1.set(lon, lat, alt, GeoDataCoordinates::Degree);
        QVERIFY(coordinates1.isPole() == false);
    } else if(pole == "south_pole") {
        coordinates1.set(lon, lat, alt, GeoDataCoordinates::Degree);
        QVERIFY(coordinates1.isPole(SouthPole));
    } else if(pole == "north_pole") {
        coordinates1.set(lon, lat, alt, GeoDataCoordinates::Degree);
        QVERIFY(coordinates1.isPole(NorthPole));
    }
}

void TestGeoDataCoordinates::testNormalizeLat_data()
{
    QTest::addColumn<qreal>( "latRadian" );

    QTest::newRow( "north pole" ) << M_PI / 2;
    QTest::newRow( "south pole" ) << - M_PI / 2;
    QTest::newRow( "somewhere" ) << 1.0;
}

void TestGeoDataCoordinates::testNormalizeLat()
{
    QFETCH( qreal, latRadian );

    qreal latDegree = RAD2DEG * latRadian;
    for ( int i = 1; i < 10; ++i ) {
        if ( ( i % 2 ) == 0 ) {
            QCOMPARE( GeoDataCoordinates::normalizeLat( latRadian + i * M_PI, GeoDataCoordinates::Radian ), latRadian );
            QCOMPARE( GeoDataCoordinates::normalizeLat( latRadian + i * M_PI ), latRadian );
            QCOMPARE( GeoDataCoordinates::normalizeLat( latDegree + i * 180, GeoDataCoordinates::Degree ), latDegree );
        }
        else {
            QCOMPARE( GeoDataCoordinates::normalizeLat( latRadian + i * M_PI, GeoDataCoordinates::Radian ), -latRadian );
            QCOMPARE( GeoDataCoordinates::normalizeLat( latRadian + i * M_PI ), -latRadian );
            QCOMPARE( GeoDataCoordinates::normalizeLat( latDegree + i * 180, GeoDataCoordinates::Degree ), -latDegree );
        }
    }
}

void TestGeoDataCoordinates::testNormalizeLon_data()
{
    QTest::addColumn<qreal>( "lonRadian" );

    QTest::newRow( "half east" ) << M_PI / 2;
    QTest::newRow( "half west" ) << - M_PI / 2;
    QTest::newRow( "somewhere" ) << 1.0;
    QTest::newRow( "date line east" ) << M_PI;
    QTest::newRow( "date line west" ) << - M_PI;

}

void TestGeoDataCoordinates::testNormalizeLon()
{
    QFETCH( qreal, lonRadian );

    qreal lonDegree = RAD2DEG * lonRadian;
    for ( int i = 1; i < 10; ++i ) {
        if ( lonRadian == M_PI || lonRadian == -M_PI ) {
            int lonRadianLarge = qRound( lonRadian * 1000 );
            int lonDegreeLarge = qRound( lonDegree * 1000 );
            if ( qRound( GeoDataCoordinates::normalizeLon( lonRadian + i * 2 * M_PI ) * 1000 ) != lonRadianLarge
                 && qRound( GeoDataCoordinates::normalizeLon( lonRadian + i * 2 * M_PI ) * 1000 ) != -lonRadianLarge )
            {
                QFAIL( "Error at M_PI/-M_PI" );
            }
            if ( qRound( GeoDataCoordinates::normalizeLon( lonRadian + i * 2 * M_PI, GeoDataCoordinates::Radian ) * 1000 ) != lonRadianLarge
                 && qRound( GeoDataCoordinates::normalizeLon( lonRadian + i * 2 * M_PI, GeoDataCoordinates::Radian ) * 1000 ) != -lonRadianLarge )
            {
                QFAIL( "Error at M_PI/-M_PI" );
            }
            if ( qRound( GeoDataCoordinates::normalizeLon( lonDegree + i * 360, GeoDataCoordinates::Degree ) * 1000 ) != lonDegreeLarge
                 && qRound( GeoDataCoordinates::normalizeLon( lonDegree + i * 360, GeoDataCoordinates::Degree ) * 1000 ) != -lonDegreeLarge )
            {
                QFAIL( "Error at M_PI/-M_PI" );
            }
        }
        else {
            QCOMPARE( GeoDataCoordinates::normalizeLon( lonRadian + i * 2 * M_PI, GeoDataCoordinates::Radian ), lonRadian );
            QCOMPARE( GeoDataCoordinates::normalizeLon( lonRadian + i * 2 * M_PI ), lonRadian );
            QCOMPARE( GeoDataCoordinates::normalizeLon( lonDegree + i * 360, GeoDataCoordinates::Degree ), lonDegree );
        }
    }
}

/*
 * test data for testNormalize()
 */
void TestGeoDataCoordinates::testNormalize_data()
{
    QTest::addColumn<qreal>("lon");
    QTest::addColumn<qreal>("lat");
    QTest::addColumn<QString>("unit");

    QTest::newRow("deg") << 200.0 << 130.0 << "degree";
    QTest::newRow("rad") << 3.6 << 2.7  << "radian";
}

/*
 * test normalizeLon(), normalizeLat() and normalizeLonLat()
 */
void TestGeoDataCoordinates::testNormalize()
{
    QFETCH(qreal, lon);
    QFETCH(qreal, lat);
    QFETCH(QString, unit);

    if(unit == "degree") {
        QCOMPARE(GeoDataCoordinates::normalizeLon(lon, GeoDataCoordinates::Degree), qreal(-160));
        QCOMPARE(GeoDataCoordinates::normalizeLat(lat, GeoDataCoordinates::Degree), qreal(50));

        qreal normalized_lon = lon;
        qreal normalized_lat = lat;

        GeoDataCoordinates::normalizeLonLat( normalized_lon, normalized_lat, GeoDataCoordinates::Degree);
        QCOMPARE(normalized_lon, qreal(20));
        QCOMPARE(normalized_lat, qreal(50));
    } else if (unit == "radian") {
        // Compare up to three decimals
        qreal value = GeoDataCoordinates::normalizeLon(lon, GeoDataCoordinates::Radian);
        QCOMPARE(ceil(value * 1000) / 1000, qreal(-2.683));

        value = GeoDataCoordinates::normalizeLat(lat, GeoDataCoordinates::Radian);
        QCOMPARE(ceil(value * 1000) / 1000, qreal(0.442));

        qreal normalized_lon = lon;
        qreal normalized_lat = lat;

        GeoDataCoordinates::normalizeLonLat( normalized_lon, normalized_lat, GeoDataCoordinates::Radian);
        QCOMPARE(ceil(normalized_lon * 1000) / 1000, qreal(0.459));
        QCOMPARE(ceil(normalized_lat * 1000) / 1000, qreal(0.442));
     }
}

enum SignType {NoSign, PositiveSign, NegativeSign};
enum SphereType {PosSphere, NegSphere};
enum UnitsType {NoUnits, WithUnits};
enum SpacesType {NoSpaces, WithSpaces};

static QString
createDegreeString(SignType signType,
                   int degreeValue, int minutesValue, qreal secondsValue,
                   UnitsType unitsType, SpacesType spacesType)
{
    QString string;

    // add degree
    if (signType != NoSign) string.append(QLatin1Char(signType==PositiveSign?'+':'-'));
    string.append(QString::number(degreeValue));
    if (unitsType == WithUnits) string.append(QChar(0xb0));

    // add minutes
    string.append(QLatin1Char(' ') + QString::number(minutesValue));
    if (unitsType == WithUnits) string.append(QLatin1Char('\''));

    // add seconds
    string.append(QString::fromLatin1(" %L1").arg(secondsValue, 0, 'f', 10));
    if (unitsType == WithUnits) string.append(QLatin1Char('"'));

    if (spacesType == WithSpaces) string.append(QLatin1Char(' '));

    return string;
}

static QString
createDegreeString(SignType signType,
                   int degreeValue, qreal minutesValue,
                   UnitsType unitsType, SpacesType spacesType)
{
    QString string;

    // add degree
    if (signType != NoSign) string.append(QLatin1Char(signType==PositiveSign?'+':'-'));
    string.append(QString::number(degreeValue));
    if (unitsType == WithUnits) string.append(QChar(0xb0));

    // add minutes
    string.append(QString::fromLatin1(" %L1").arg(minutesValue, 0, 'f', 10));
    if (unitsType == WithUnits) string.append(QLatin1Char('\''));

    if (spacesType == WithSpaces) string.append(QLatin1Char(' '));

    return string;
}

static QString
createDegreeString(SignType signType,
                   qreal degreeValue,
                   UnitsType unitsType, SpacesType spacesType)
{
    QString string;

    // add degree
    if (signType != NoSign) string.append(QLatin1Char(signType==PositiveSign?'+':'-'));
    string.append(QString::fromLatin1("%L1").arg(degreeValue, 0, 'f', 10));
    if (unitsType == WithUnits) string.append(QChar(0xb0));

    if (spacesType == WithSpaces) string.append(QLatin1Char(' '));

    return string;
}

/*
 * test data for testStringDMS()
 */
void TestGeoDataCoordinates::testFromStringDMS_data()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<qreal>("lon");
    QTest::addColumn<qreal>("lat");

    const QVector<SignType> signTypes = QVector<SignType>()
        << NoSign << PositiveSign << NegativeSign;
    const QVector<SphereType> sphereTypes = QVector<SphereType>()
        << PosSphere << NegSphere;
    const QVector<UnitsType> unitsTypes = QVector<UnitsType>()
        << NoUnits << WithUnits;
    const QVector<SpacesType> spacesTypes = QVector<SpacesType>()
        << NoSpaces << WithSpaces;

    const QVector<uint> degreeSamples = QVector<uint>()
        << 0 << 140 << 180;
    const QVector<uint> minutesSamples = QVector<uint>()
        << 0 << 23 << 59;
    const QVector<qreal> secondsSamples = QVector<qreal>()
        << 0.0 << 3.14159 << 59.9999999;

    foreach(const UnitsType unitsType, unitsTypes) {
    foreach(const SpacesType spacesType, spacesTypes) {
    // lon
    foreach(const SphereType lonSphere, sphereTypes) {
    foreach(const SignType lonSignType, signTypes) {
        const bool lonIsPositive =
            (lonSphere==PosSphere && lonSignType!=NegativeSign) ||
            (lonSphere==NegSphere && lonSignType==NegativeSign);
    foreach(const uint lonDegree, degreeSamples) {
    foreach(const uint lonMinutes, minutesSamples) {
        if(lonDegree == 180 && lonMinutes != 0) continue;
    foreach(const qreal lonSeconds, secondsSamples) {
        if(lonDegree == 180 && lonSeconds != 0.0) continue;
    // lat
    foreach(const SphereType latSphere, sphereTypes) {
    foreach(const SignType latSignType, signTypes) {
        const bool latIsPositive =
            (latSphere==PosSphere && latSignType!=NegativeSign) ||
            (latSphere==NegSphere && latSignType==NegativeSign);
    foreach(const uint latDegree, degreeSamples) {
    foreach(const uint latMinutes, minutesSamples) {
        if(latDegree == 180 && latMinutes != 0) continue;
    foreach(const qreal latSeconds, secondsSamples) {
        if(latDegree == 180 && latSeconds != 0.0) continue;

    // actual construction
        // Create lon & lat values
        qreal lon = (qreal)lonDegree + lonMinutes*MIN2HOUR + lonSeconds*SEC2HOUR;
        if( ! lonIsPositive )
            lon *= -1;
        qreal lat = (qreal)latDegree + latMinutes*MIN2HOUR + latSeconds*SEC2HOUR;
        if( ! latIsPositive )
            lat *= -1;

        // Create string
        QString string;
        string.append(createDegreeString(latSignType,
                                         latDegree, latMinutes, latSeconds,
                                         unitsType, spacesType));
        string.append(QLatin1Char(latSphere==PosSphere?'N':'S'));
        string.append(QLatin1Char(' '));
        string.append(createDegreeString(lonSignType,
                                         lonDegree, lonMinutes, lonSeconds,
                                         unitsType, spacesType));
        string.append(QLatin1Char(lonSphere==PosSphere?'E':'W'));

        // Create row title
        QString rowTitle;
        rowTitle.append(QLatin1String(spacesType==WithSpaces?"spaced dir":"unspaced dir"))
                .append(QLatin1String(unitsType==WithUnits?"|units":"|no units"))
                .append(QLatin1String("|lon:"))
                .append(QLatin1Char(lonIsPositive?'+':'-'))
                .append(QString::number(lonDegree)+QChar(0xb0))
                .append(QString::number(lonMinutes)+QLatin1Char('\''))
                .append(QString::fromLatin1("%L1").arg(lonSeconds, 0, 'f', 10)+QLatin1Char('"'))
                .append(QLatin1Char(lonSphere==PosSphere?'P':'N'))
                .append(QLatin1String("|lat:"))
                .append(QLatin1Char(latIsPositive?'+':'-'))
                .append(QString::number(latDegree)+QChar(0xb0))
                .append(QString::number(latMinutes)+QLatin1Char('\''))
                .append(QString::fromLatin1("%L1").arg(latSeconds, 0, 'f', 10)+QLatin1Char('"'))
                .append(QLatin1Char(latSphere==PosSphere?'P':'N'))
                .append(QLatin1Char('|')).append(string).append(QLatin1Char('|'));
        QTest::newRow(rowTitle.toLatin1())
            << string
            << lon
            << lat;
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
}

/*
 * test fromString() with DMS notation
 */
void TestGeoDataCoordinates::testFromStringDMS()
{
    QFETCH(QString, string);
    QFETCH(qreal, lon);
    QFETCH(qreal, lat);

    bool succeeded = false;
    const GeoDataCoordinates coords = GeoDataCoordinates::fromString(string, succeeded);

    if(! succeeded)
        qWarning() << "Could not parse"<<string <<"for"<<lon<<lat;

    QVERIFY(succeeded);
    QCOMPARE(coords.longitude(GeoDataCoordinates::Degree), lon);
    QCOMPARE(coords.latitude(GeoDataCoordinates::Degree),  lat);
}

/*
 * test data for testStringDM()
 */
void TestGeoDataCoordinates::testFromStringDM_data()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<qreal>("lon");
    QTest::addColumn<qreal>("lat");

    const QVector<SignType> signTypes = QVector<SignType>()
        << NoSign << PositiveSign << NegativeSign;
    const QVector<SphereType> sphereTypes = QVector<SphereType>()
        << PosSphere << NegSphere;
    const QVector<UnitsType> unitsTypes = QVector<UnitsType>()
        << NoUnits << WithUnits;
    const QVector<SpacesType> spacesTypes = QVector<SpacesType>()
        << NoSpaces << WithSpaces;

    const QVector<uint> degreeSamples = QVector<uint>()
        << 0 << 140 << 180;
    const QVector<qreal> minutesSamples = QVector<qreal>()
        << 0.0 << 3.14159 << 59.9999999;

    foreach(const UnitsType unitsType, unitsTypes) {
    foreach(const SpacesType spacesType, spacesTypes) {
    // lon
    foreach(const SphereType lonSphere, sphereTypes) {
    foreach(const SignType lonSignType, signTypes) {
        const bool lonIsPositive =
            (lonSphere==PosSphere && lonSignType!=NegativeSign) ||
            (lonSphere==NegSphere && lonSignType==NegativeSign);
    foreach(const uint lonDegree, degreeSamples) {
    foreach(const qreal lonMinutes, minutesSamples) {
        if(lonDegree == 180 && lonMinutes != 0.0) continue;
    // lat
    foreach(const SphereType latSphere, sphereTypes) {
    foreach(const SignType latSignType, signTypes) {
        const bool latIsPositive =
            (latSphere==PosSphere && latSignType!=NegativeSign) ||
            (latSphere==NegSphere && latSignType==NegativeSign);
    foreach(const uint latDegree, degreeSamples) {
    foreach(const qreal latMinutes, minutesSamples) {
        if(latDegree == 180 && latMinutes != 0.0) continue;

    // actual construction
        // Create lon & lat values
        qreal lon = (qreal)lonDegree + lonMinutes*MIN2HOUR;
        if( ! lonIsPositive )
            lon *= -1;
        qreal lat = (qreal)latDegree + latMinutes*MIN2HOUR;
        if( ! latIsPositive )
            lat *= -1;

        // Create string
        QString string;
        string.append(createDegreeString(latSignType,
                                         latDegree, latMinutes,
                                         unitsType, spacesType));
        string.append(QLatin1Char(latSphere==PosSphere?'N':'S'));
        string.append(QLatin1Char(' '));
        string.append(createDegreeString(lonSignType,
                                         lonDegree, lonMinutes,
                                         unitsType, spacesType));
        string.append(QLatin1Char(lonSphere==PosSphere?'E':'W'));

        // Create row title
        QString rowTitle;
        rowTitle.append(QLatin1String(spacesType==WithSpaces?"spaced dir":"unspaced dir"))
                .append(QLatin1String(unitsType==WithUnits?"|units":"|no units"))
                .append(QLatin1String("|lon:"))
                .append(QLatin1Char(lonIsPositive?'+':'-'))
                .append(QString::number(lonDegree)+QChar(0xb0))
                .append(QString::fromLatin1("%L1").arg(lonMinutes, 0, 'f', 10)+QLatin1Char('\''))
                .append(QLatin1Char(lonSphere==PosSphere?'P':'N'))
                .append(QLatin1String("|lat:"))
                .append(QLatin1Char(latIsPositive?'+':'-'))
                .append(QString::number(latDegree)+QChar(0xb0))
                .append(QString::fromLatin1("%L1").arg(latMinutes, 0, 'f', 10)+QLatin1Char('\''))
                .append(QLatin1Char(latSphere==PosSphere?'P':'N'))
                .append(QLatin1Char('|')).append(string).append(QLatin1Char('|'));
        QTest::newRow(rowTitle.toLatin1())
            << string
            << lon
            << lat;
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
}

/*
 * test fromString() with DM notation
 */
void TestGeoDataCoordinates::testFromStringDM()
{
    QFETCH(QString, string);
    QFETCH(qreal, lon);
    QFETCH(qreal, lat);

    bool succeeded = false;
    const GeoDataCoordinates coords = GeoDataCoordinates::fromString(string, succeeded);

    if(! succeeded)
        qWarning() << "Could not parse"<<string <<"for"<<lon<<lat;

    QVERIFY(succeeded);
    QCOMPARE(coords.longitude(GeoDataCoordinates::Degree), lon);
    QCOMPARE(coords.latitude(GeoDataCoordinates::Degree),  lat);
}

/*
 * test data for testStringDM()
 */
void TestGeoDataCoordinates::testFromStringD_data()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<qreal>("lon");
    QTest::addColumn<qreal>("lat");

    const QVector<SignType> signTypes = QVector<SignType>()
        << NoSign << PositiveSign << NegativeSign;
    const QVector<SphereType> sphereTypes = QVector<SphereType>()
        << PosSphere << NegSphere;
    const QVector<UnitsType> unitsTypes = QVector<UnitsType>()
        << NoUnits << WithUnits;
    const QVector<SpacesType> spacesTypes = QVector<SpacesType>()
        << NoSpaces << WithSpaces;

    const QVector<qreal> degreeSamples = QVector<qreal>()
        << 0.0 << 3.14159 << 180.0;

    foreach(const UnitsType unitsType, unitsTypes) {
    foreach(const SpacesType spacesType, spacesTypes) {
    // lon
    foreach(const SphereType lonSphere, sphereTypes) {
    foreach(const SignType lonSignType, signTypes) {
        const bool lonIsPositive =
            (lonSphere==PosSphere && lonSignType!=NegativeSign) ||
            (lonSphere==NegSphere && lonSignType==NegativeSign);
    foreach(const qreal lonDegree, degreeSamples) {
    // lat
    foreach(const SphereType latSphere, sphereTypes) {
    foreach(const SignType latSignType, signTypes) {
        const bool latIsPositive =
            (latSphere==PosSphere && latSignType!=NegativeSign) ||
            (latSphere==NegSphere && latSignType==NegativeSign);
    foreach(const qreal latDegree, degreeSamples) {

    // actual construction
        // Create lon & lat values
        qreal lon = lonDegree;
        if (! lonIsPositive)
            lon *= -1;
        qreal lat = latDegree;
        if (! latIsPositive)
            lat *= -1;

        // Create string
        QString string;
        string.append(createDegreeString(latSignType,
                                         latDegree,
                                         unitsType, spacesType));
        string.append(QLatin1Char(latSphere==PosSphere?'N':'S'));
        string.append(QLatin1Char(' '));
        string.append(createDegreeString(lonSignType,
                                         lonDegree,
                                         unitsType, spacesType));
        string.append(QLatin1Char(lonSphere==PosSphere?'E':'W'));

        // Create row title
        QString rowTitle;
        rowTitle.append(QLatin1String(spacesType==WithSpaces?"spaced dir":"unspaced dir"))
                .append(QLatin1String(unitsType==WithUnits?"|units":"|no units"))
                .append(QLatin1String("|lon:"))
                .append(QLatin1Char(lonIsPositive?'+':'-'))
                .append(QString::fromLatin1("%L1").arg(lonDegree, 0, 'f', 10)+QChar(0xb0))
                .append(QLatin1Char(lonSphere==PosSphere?'P':'N'))
                .append(QLatin1String("|lat:"))
                .append(QLatin1Char(latIsPositive?'+':'-'))
                .append(QString::fromLatin1("%L1").arg(latDegree, 0, 'f', 10)+QChar(0xb0))
                .append(QLatin1Char(latSphere==PosSphere?'P':'N'))
                .append(QLatin1Char('|')).append(string).append(QLatin1Char('|'));
        QTest::newRow(rowTitle.toLatin1())
            << string
            << lon
            << lat;
    }
    }
    }
    }
    }
    }
    }
    }
}

/*
 * test fromString() with DM notation
 */
void TestGeoDataCoordinates::testFromStringD()
{
    QFETCH(QString, string);
    QFETCH(qreal, lon);
    QFETCH(qreal, lat);

    bool succeeded = false;
    const GeoDataCoordinates coords = GeoDataCoordinates::fromString(string, succeeded);

    if(! succeeded)
        qWarning() << "Could not parse"<<string <<"for"<<lon<<lat;

    QVERIFY(succeeded);
    QCOMPARE(coords.longitude(GeoDataCoordinates::Degree), lon);
    QCOMPARE(coords.latitude(GeoDataCoordinates::Degree),  lat);
}

class FromStringRegExpTranslator : public QTranslator
{
public:
    FromStringRegExpTranslator(const QString& _degree, const QString& _minutes, const QString& _seconds,
                               const QString& _north, const QString& _south,
                               const QString& _east, const QString& _west)
    : QTranslator((QObject*)0)
    , degree( _degree )
    , minutes( _minutes )
    , seconds( _seconds )
    , north( _north )
    , south( _south )
    , east( _east )
    , west( _west )
    {}

public: // QTranslator API
    virtual bool isEmpty() const { return false; }
    virtual QString translate( const char* context, const char* sourceText,
                               const char* disambiguation = 0 ) const;
private:
    const QString degree;
    const QString minutes;
    const QString seconds;
    const QString north;
    const QString south;
    const QString east;
    const QString west;
};

QString FromStringRegExpTranslator::translate( const char* context, const char* sourceText,
                                               const char* disambiguation ) const
{
    if (qstrcmp(context, "GeoDataCoordinates") != 0 )
        return QString();

    if (qstrcmp(sourceText, "*") != 0 )
        return QString();

    if (qstrcmp(disambiguation, "North direction terms, see http://techbase.kde.org/Projects/Marble/GeoDataCoordinatesTranslation") == 0 )
        return north;
    if (qstrcmp(disambiguation, "South direction terms, see http://techbase.kde.org/Projects/Marble/GeoDataCoordinatesTranslation") == 0 )
        return south;
    if (qstrcmp(disambiguation, "East direction terms, see http://techbase.kde.org/Projects/Marble/GeoDataCoordinatesTranslation") == 0 )
        return east;
    if (qstrcmp(disambiguation, "West direction terms, see http://techbase.kde.org/Projects/Marble/GeoDataCoordinatesTranslation") == 0 )
        return west;
    if (qstrcmp(disambiguation, "Degree symbol terms, see http://techbase.kde.org/Projects/Marble/GeoDataCoordinatesTranslation") == 0 )
        return degree;
    if (qstrcmp(disambiguation, "Minutes symbol terms, see http://techbase.kde.org/Projects/Marble/GeoDataCoordinatesTranslation") == 0 )
        return minutes;
    if (qstrcmp(disambiguation, "Seconds symbol terms, see http://techbase.kde.org/Projects/Marble/GeoDataCoordinatesTranslation") == 0 )
        return seconds;

    return QString();
}

class Sample
{
public:
    Sample() {}
    Sample(const char* _name, const char* _string, qreal _lon, qreal _lat)
    : name(QString::fromUtf8(_name))
    , string(QString::fromUtf8(_string))
    , lon(_lon)
    , lat(_lat)
    {}
    QString name;
    QString string;
    qreal lon;
    qreal lat;
};

class Language {
public:
    Language() {}
    Language(const char* _name,
             const char* _degree, const char* _minutes, const char* _seconds,
             const char* _north, const char* _south, const char* _east, const char* _west,
             const QVector<Sample>& _samples)
    : name(QString::fromUtf8(_name))
    , degree(QString::fromUtf8(_degree))
    , minutes(QString::fromUtf8(_minutes))
    , seconds(QString::fromUtf8(_seconds))
    , north(QString::fromUtf8(_north))
    , south(QString::fromUtf8(_south))
    , east(QString::fromUtf8(_east))
    , west(QString::fromUtf8(_west))
    , samples(_samples)
    {}
    QString name;
    QString degree;
    QString minutes;
    QString seconds;
    QString north;
    QString south;
    QString east;
    QString west;
    QVector<Sample> samples;
};

void TestGeoDataCoordinates::testFromLocaleString_data()
{
    QTest::addColumn<QString>("degree");
    QTest::addColumn<QString>("minutes");
    QTest::addColumn<QString>("seconds");
    QTest::addColumn<QString>("north");
    QTest::addColumn<QString>("south");
    QTest::addColumn<QString>("east");
    QTest::addColumn<QString>("west");

    QTest::addColumn<QString>("string");
    QTest::addColumn<qreal>("lon");
    QTest::addColumn<qreal>("lat");

    const QVector<Language> languages = QVector<Language>()
        << Language(
            "English",
            "*", // degree
            "*", // minutes
            "*", // seconds
            "*", // north
            "*", // south
            "*", // east
            "*", // west
            QVector<Sample>()
                << Sample(
                    "London",
                    "N051 30.150′ W000 07.234′",
                    -0.12056666666666666921, 51.50249999999999772626)
                << Sample(
                    "Ålgård",
                    "N58.764828 E5.855483",
                    5.85548300000000043752, 58.76482800000000139562))

        << Language(
            "Japanese",
            "度", // degree
            "分", // minutes
            "秒", // seconds
            "北緯", // north
            "南緯", // south
            "東経", // east
            "西経", // west
            QVector<Sample>()
                << Sample(
                    "London",
                    "北緯51度30分28秒 西経0度07分41秒",
                    -0.12805555555555556135, 51.50777777777777544088)
                << Sample(
                    "Sydney",
                    "南緯33度52分06秒 東経151度12分31秒",
                    151.20861111111111085847, -33.86833333333333229120))
        << Language(
            "Korean",
            "도", // degree
            "분", // minutes
            "초", // seconds
            "북위", // north
            "남위", // south
            "동경", // east
            "서경", // west
            QVector<Sample>()
                << Sample(
                    "London",
                    "북위 51도 30분 26초, 서경 0도 7분 39초",
                    -0.12750000000000000222, 51.50722222222222512755)
                << Sample(
                    "Sydney",
                    "남위 33도 31분 56초, 동경 151도 12분 40초",
                    151.21111111111110858474, -33.53222222222222370647))

        << Language(
            "Galician",
            "", // degree
            "", // minutes
            "", // seconds
            "N", //north
            "S", // south
            "L|E", // east
            "O|W", // west
            QVector<Sample>()
                << Sample(
                    "Campamento",
                    "36º10,67´N 5º24,29´W",
                    -5.40483333333333337833, 36.17783333333333217752))

        << Language(
            "German",
            "*", // degree
            "*", // minutes
            "*", // seconds
            "N", //north
            "S", // south
            "O", // east
            "W", // west
            QVector<Sample>()
                << Sample(
                    "London",
                    "51° 31′ N, 0° 7′ W",
                    -0.11666666666666666852, 51.51666666666666571928))

        << Language(
            "Greek",
            "", // degree
            "", // minutes
            "", // seconds
            "Β", // north
            "Ν", // south
            "Α", // east
            "Δ", // west
            QVector<Sample>()
                << Sample(
                    "Χαλκίδα",
                    "38° 28′ Β 23° 36′ Α",
                    23.6, 38.46666666666666856))

        << Language(
            "Dutch",
            "", // degree
            "", // minutes
            "", // seconds
            "N|NB", // north
            "Z|ZB", // south
            "O|OL", // east
            "W|WL", // west
            QVector<Sample>()
                << Sample(
                    "Amersfoort",
                    "N 52° 8′ 32.14″ , E 5° 24′ 56.09″",
                    5.41558055555555561966, 52.14226111111111094942)
                << Sample(
                    "London",
                    "51°30'00,55\" NB 0°07'34,45\" WL",
                    -0.12623611111111110450, 51.50015277777777811252)
                << Sample(
                    "Amsterdam",
                    "52°22'12,78\" NB 4°53'42,60\" OL",
                    4.89516666666666644403, 52.37021666666666419587)
                << Sample(
                    "Capetown",
                    "33°55'29,52\" ZB 18°25'26,60\" OL",
                    18.42405555555555451974, -33.92486666666666650372))

        << Language(
            "Polish",
            "", // degree
            "", // minutes
            "", // seconds
            "Pn.|Pn", // north
            "Płd.|Płd", // south
            "Wschod.|Wschod|Wsch.|Wsch|Ws.|Ws", // east
            "Zach.|Zach|Z", // west
            QVector<Sample>()
                << Sample(
                    "Warsaw",
                    "52°13′56″Pn. 21°00′30″Ws.",
                    21.00833333333333285964, 52.23222222222221944321))

        << Language(
            "Esperanto",
            "", // degree
            "", // minutes
            "", // seconds
            "N", // north
            "S", // south
            "Or", // east
            "Ok", // west
            QVector<Sample>()
                << Sample(
                    "London",
                    "52° 8′ 32,14″ N; 5° 24′ 56,09″ Or",
                    5.41558055555555561966, 52.14226111111111094942))

        << Language(
            "Norwegian",
            "", // degree
            "", // minutes
            "", // seconds
            "N", // north
            "S", // south
            "Ø", // east
            "V", // west
            QVector<Sample>()
                << Sample(
                    "London",
                    "51° 30′ 25” N 0° 7′ 39” V",
                    -0.12750000000000000222, 51.50694444444444286546)
                << Sample(
                    "Ålgård",
                    "58° 45′ 53.38″ N 5° 51′ 19.74″ Ø",
                    5.85548333333333292927, 58.76482777777777499750))

        << Language(
            "Swedish",
            "", // degree
            "", // minutes
            "", // seconds
            "N", // north
            "S", // south
            "O", // east
            "V", // west
            QVector<Sample>()
                << Sample(
                    "London",
                    "51°30′29″N 0°7′29″V",
                    -0.12472222222222222043, 51.50805555555555770297)
                << Sample(
                    "Sydney",
                    "33°31′56″S 151°12′40″O",
                    151.21111111111110858474, -33.53222222222222370647))

        << Language(
            "Icelandic",
            "", // degree
            "", // minutes
            "", // seconds
            "N", //north
            "S", // south
            "A", // east
            "V", // west
//TODO:     "breidd 51°30'26\" N, lengd 0°7'39\" V" // London
            QVector<Sample>()
                << Sample(
                    "Sydney",
                    "33°31'56\" S, 151°12'40\" A",
                    151.21111111111110858474, -33.53222222222222370647))

        << Language(
            "Turkish",
            "", // degree
            "", // minutes
            "", // seconds
            "K", // north
            "G", // south
            "D", // east
            "B", // west
            QVector<Sample>()
                << Sample(
                    "London",
                    "51° 30′ 28″ K, 0° 7′ 41″ B",
                    -0.12805555555555556135, 51.50777777777777544088))

        << Language(
            "Spanish", // (incl. Latin America)
            "", // degree
            "", // minutes
            "", // seconds
            "N", // north
            "S", // south
            "E", // east
            "O|W", // west
            QVector<Sample>()
                << Sample(
                    "London",
                    "51°30′25″N 00°07′39″O",
                    -0.12750000000000000222, 51.50694444444444286546)
                << Sample(
                    "Else",
                    "52° 8′ 32.14″ N, 5° 24′ 56.09″ W",
                    -5.41558055555555561966, 52.14226111111111094942)
                << Sample(
                    "Bogotá",
                    "4°35’53″N 74°4’33″O",
                    -74.07583333333333541759, 4.59805555555555667269))

        << Language(
            "French",
            "", // degree
            "", // minutes
            "", // seconds
            "N", // north
            "S", // south
            "E", // east
            "O", // west
            QVector<Sample>()
                << Sample(
                    "London",
                    "51° 30′ 18″ N 0° 04′ 43″ O",
                    -0.07861111111111110383, 51.50500000000000255795))

        << Language(
            "Portuguese", // incl. Brazilian Portuguese
            "", // degree
            "", // minutes
            "", // seconds
            "N", // north
            "S", // south
            "E|L", // east
            "O", // west
            QVector<Sample>()
                << Sample(
                    "London",
                    "52° 8′ 32.14″ N, 5° 24′ 56.09″ E",
                    5.41558055555555561966, 52.14226111111111094942))

        << Language(
            "Arabic",
            "", // degree
            "", // minutes
            "", // seconds
    "شمال", // north
    "جنوب", // south
    "شرق", // east
    "غرب", // west
            QVector<Sample>()
                << Sample(
                    "Warsaw",
    "52°13′56″ شمال 21°00′30″ شرق",
                    21.00833333333333285964, 52.23222222222221944321))

        << Language(
            "Russian",
            "", //"град", "градусов" // degree
            "", //"мин", "минут" // minutes
            "", //"сек", "секунд" // seconds
            "с. ш.", // north
            "ю. ш.", // south
            "в. д.", // east
            "з. д.", // west
            QVector<Sample>()
                << Sample(
                    "London",
                    "51°30′26″ с. ш. 0°07′39″ з. д.",
                    -0.12750000000000000222, 51.50722222222222512755))

        << Language(
            "Ukrainian",
            "", // degree
            "", // minutes
            "", // seconds
            "пн. ш.", // north
            "пд. ш.", // south
            "сх. д.", // east
            "зх. д.", // west
            QVector<Sample>()
                << Sample(
                    "London",
                    "51°30' пн. ш. 0°07' сх. д.",
                    0.11666666666666666852, 51.50000000000000000000)
                << Sample(
                    "Sydney",
                    "33°52'10'' пд. ш. 151°12'30'' сх. д.",
                    151.20833333333334280724, -33.86944444444444712872)
                << Sample(
                    "Rio de Janeiro",
                    "22°54'30'' пд. ш. 43°11'47'' зх. д.",
                    -43.19638888888889027839, -22.90833333333333499127))

        << Language(
            "Bulgarian",
            "", // degree
            "", // minutes
            "", // seconds
            "с. ш.", // north
            "ю. ш.", // south
            "и. д.", // east
            "и. д.", // west
            QVector<Sample>()
                << Sample(
                    "London",
                    "51°30′26″ с. ш. 0°07′39″ и. д.",
                    0.12750000000000000222, 51.50722222222222512755))

        << Language(
            "Czech",
            "", // degree
            "", // minutes
            "", // seconds
            "s. š.", // north
            "j. š.", // south
            "z. d.", // east
            "v. d.", // west
            QVector<Sample>()
                << Sample(
                    "London",
                    "51°30′42″ s. š., 0°02′56″ z. d.",
                    0.04888888888888889145, 51.51166666666666316132)
                << Sample(
                    "Sydney",
                    "33° 52′ j. š., 151° 13′ v. d.",
                    -151.21666666666669698316, -33.86666666666666714036))


        << Language(
            "Hindi",
            "", // degree
            "", // minutes
            "", // seconds
            "उ", // north
            "द", // south
            "पू", // east
            "प", // west
            QVector<Sample>()
                << Sample(
                    "London",
                    "51°30′25″उ 00°07′39″पू",
                    0.12750000000000000222, 51.50694444444444286546))

        << Language(
            "Tamil",
            "", // degree
            "", // minutes
            "", // seconds
            "வ", // north
            "தெ", // south
            "கி", // east
            "மே", // west
            QVector<Sample>()
                << Sample(
                    "London",
                    "51°30′25″ வ 00°07′39″ கி",
                    0.12750000000000000222, 51.50694444444444286546))
        ;

    foreach( const Language& language, languages ) {
    foreach( const Sample& sample, language.samples ) {
        const QString rowTitle =
            language.name +
            QLatin1String("|") + sample.name +
            QLatin1String("|lon:") +
            QString::fromLatin1("%L1").arg(sample.lon, 0, 'f', 10) +
            QLatin1String("|lat:") +
            QString::fromLatin1("%L1").arg(sample.lat, 0, 'f', 10);

        QTest::newRow(rowTitle.toLatin1())
            << language.degree
            << language.minutes
            << language.seconds
            << language.north
            << language.south
            << language.east
            << language.west
            << sample.string
            << sample.lon
            << sample.lat;
    }}
}


void TestGeoDataCoordinates::testFromLocaleString()
{
    QFETCH(QString, degree);
    QFETCH(QString, minutes);
    QFETCH(QString, seconds);
    QFETCH(QString, north);
    QFETCH(QString, south);
    QFETCH(QString, east);
    QFETCH(QString, west);

    QFETCH(QString, string);
    QFETCH(qreal, lon);
    QFETCH(qreal, lat);

    FromStringRegExpTranslator translator(degree, minutes, seconds, north, south, east, west);
    QCoreApplication::installTranslator(&translator);

    bool succeeded = false;
    const GeoDataCoordinates coords = GeoDataCoordinates::fromString(string, succeeded);

    if(! succeeded)
        qWarning() << "Could not parse"<<string <<"for"<<lon<<lat;

    QVERIFY(succeeded);

// Uncomment to get the lon and lat values with more precision
// qWarning() << "lon"<<QString::fromLatin1("%L1").arg(coords.longitude(GeoDataCoordinates::Degree), 0, 'f', 20)
//            << "lat"<<QString::fromLatin1("%L1").arg(coords.latitude(GeoDataCoordinates::Degree), 0, 'f', 20);

    QCOMPARE(coords.longitude(GeoDataCoordinates::Degree), lon);
    QCOMPARE(coords.latitude(GeoDataCoordinates::Degree),  lat);

    QCoreApplication::removeTranslator(&translator);
}

/*
 * test data for testString()
 */
void TestGeoDataCoordinates::testString_data()
{
    QTest::addColumn<qreal>("lon");
    QTest::addColumn<qreal>("lat");
    QTest::addColumn<qreal>("alt");

    QTest::newRow("deg") << 140.0 << 65.0 << 400.0;
}

/*
 * test lonToString(), latToString(), fromString() and toString()
 */
void TestGeoDataCoordinates::testString()
{
    QString lon_str = "150 E";
    QString lat_str = "75 N";
    bool succeeded = false;

    //fromString
    GeoDataCoordinates coordinates1 = GeoDataCoordinates::fromString(lon_str + lat_str , succeeded);
    QCOMPARE(succeeded, true);
    QCOMPARE(lon_str, coordinates1.lonToString().replace(QChar(0x00b0), QChar(' ')));

    QString tmpString = coordinates1.latToString();
    QCOMPARE(lat_str, tmpString.replace(QChar(0x00b0), QChar(' ')).trimmed() );

    QCOMPARE(coordinates1.longitude(GeoDataCoordinates::Degree), qreal(150.0));
    QCOMPARE(coordinates1.latitude(GeoDataCoordinates::Degree), qreal(75.0));

    //latToString and lonToString
    QFETCH(qreal, lon);
    QFETCH(qreal, lat);
    QFETCH(qreal, alt);

    coordinates1.set(lon, lat, alt, GeoDataCoordinates::Degree);
    tmpString = coordinates1.toString(GeoDataCoordinates::Decimal, GeoDataCoordinates::Degree);

    QCOMPARE(tmpString.mid(tmpString.indexOf(", ") + 1).trimmed(),
             GeoDataCoordinates::latToString(lat, GeoDataCoordinates::Decimal, GeoDataCoordinates::Degree, 1).trimmed()); //one decimal precision

    tmpString = coordinates1.toString(GeoDataCoordinates::DMS);
    QCOMPARE(tmpString.mid(tmpString.indexOf(", ") + 1).trimmed(),
             GeoDataCoordinates::latToString(lat, GeoDataCoordinates::DMS,GeoDataCoordinates::Degree).trimmed() );

    tmpString = coordinates1.toString(GeoDataCoordinates::Decimal);
    QCOMPARE(tmpString.remove(tmpString.indexOf(", "), tmpString.length()),
             GeoDataCoordinates::lonToString(lon, GeoDataCoordinates::Decimal, GeoDataCoordinates::Degree).trimmed() );

    tmpString = coordinates1.toString(GeoDataCoordinates::DMS);
    QCOMPARE(tmpString.remove(tmpString.indexOf(", "), tmpString.length()),
             GeoDataCoordinates::lonToString(lon, GeoDataCoordinates::DMS, GeoDataCoordinates::Degree).trimmed() );
}

/*
 * test data for testPack()
 */
void TestGeoDataCoordinates::testPack_data()
{
    QTest::addColumn<qreal>("lon");
    QTest::addColumn<qreal>("lat");
    QTest::addColumn<qreal>("alt");

    QTest::newRow("deg") << 180.0 << 90.0 << 400.0;
}

/*
 * test pack() and unPack()
 */
void TestGeoDataCoordinates::testPack()
{
    QFETCH(qreal, lon);
    QFETCH(qreal, lat);
    QFETCH(qreal, alt);

    GeoDataCoordinates coordinates1,coordinates2;
    coordinates1.set(lon, lat, alt, GeoDataCoordinates::Degree);

    QTemporaryFile file;
    if(file.open()) {
        QDataStream out(&file);
        coordinates1.pack(out);
    }
    file.close();

    if(file.open()) {
        QDataStream in(&file);
        coordinates2.unpack(in);
    }
    file.close();

    QCOMPARE(coordinates1.longitude(GeoDataCoordinates::Degree), coordinates2.longitude(GeoDataCoordinates::Degree));
    QCOMPARE(coordinates1.latitude(GeoDataCoordinates::Degree), coordinates2.latitude(GeoDataCoordinates::Degree));
    QCOMPARE(coordinates1.altitude(), coordinates2.altitude());
}

QTEST_MAIN(TestGeoDataCoordinates)
#include "TestGeoDataCoordinates.moc"



