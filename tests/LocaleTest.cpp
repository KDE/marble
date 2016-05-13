//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015   Ilya Kowalewski <illya.kovalevskyy@gmail.com>
//

#include <QTest>
#include "MarbleLocale.h"
#include "MarbleGlobal.h"
#include "TestUtils.h"

Q_DECLARE_METATYPE(Marble::MarbleLocale::MeasurementSystem)
Q_DECLARE_METATYPE(Marble::MarbleLocale::MeasureUnit)

namespace Marble {

class LocaleTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void meterToCustomUnit();
    void meterToCustomUnit_data();

private:
    MarbleLocale *locale;
};

void LocaleTest::initTestCase()
{
    // Get rid of translation
    QLocale::setDefault(QLocale::C);

    locale = MarbleGlobal::getInstance()->locale();
}

void LocaleTest::meterToCustomUnit_data()
{
    QTest::addColumn<MarbleLocale::MeasurementSystem>("targetSystem");
    QTest::addColumn<qreal>("meters");
    QTest::addColumn<qreal>("targetValue");
    QTest::addColumn<MarbleLocale::MeasureUnit>("targetUnit");

    // Testing metric system
    QTest::newRow("1") << MarbleLocale::MetricSystem << 0.003 << 3.0 << MarbleLocale::Milimeter;
    QTest::newRow("2") << MarbleLocale::MetricSystem << 0.017 << 1.7 << MarbleLocale::Centimeter;
    QTest::newRow("3") << MarbleLocale::MetricSystem << 12.3 << 12.3 << MarbleLocale::Meter;
    QTest::newRow("4") << MarbleLocale::MetricSystem << 7834.139 << 7.83 << MarbleLocale::Kilometer;

    // Testing imperial system
    QTest::newRow("5") << MarbleLocale::ImperialSystem << 0.1 << 3.93 << MarbleLocale::Inch;
    QTest::newRow("6") << MarbleLocale::ImperialSystem << 0.8 << 2.62 << MarbleLocale::Foot;
    QTest::newRow("7") << MarbleLocale::ImperialSystem << 1.8288 << 2.0 << MarbleLocale::Yard;
    QTest::newRow("8") << MarbleLocale::ImperialSystem << 37550.0 << 23.3 << MarbleLocale::Mile;

    // Testing nautical mile
    QTest::newRow("9") << MarbleLocale::NauticalSystem << 3500.0 << 1.89 << MarbleLocale::NauticalMile;
    QTest::newRow("10") << MarbleLocale::NauticalSystem << 1200.0 << 0.648 << MarbleLocale::NauticalMile;
}

void LocaleTest::meterToCustomUnit()
{
    QFETCH(MarbleLocale::MeasurementSystem, targetSystem);
    QFETCH(qreal, meters);
    QFETCH(qreal, targetValue);
    QFETCH(MarbleLocale::MeasureUnit, targetUnit);

    qreal value;
    MarbleLocale::MeasureUnit unit;

    locale->meterToTargetUnit(meters, targetSystem, value, unit);

    QFUZZYCOMPARE(value, targetValue, 0.033);
    QCOMPARE(targetUnit, unit);
}

} // namespace Marble

QTEST_MAIN( Marble::LocaleTest )

#include "LocaleTest.moc"
