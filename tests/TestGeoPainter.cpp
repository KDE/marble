// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

#include "GeoDataLatLonAltBox.h"
#include "GeoDataLinearRing.h"
#include "GeoPainter.h"
#include "GeoPainter_p.h"
#include "MarbleGlobal.h"
#include "ViewportParams.h"

#include <QList>
#include <QTest>

namespace Marble
{

class TestGeoPainter : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void createLinearRingFromGeoRectTest();
};

void TestGeoPainter::createLinearRingFromGeoRectTest()
{
    //    GeoPainterPrivate painterp( new ViewportParams(), PrintQuality );
    //
    //    QList<GeoDataLatLonAltBox> list;
    //    list.append( GeoDataLatLonBox( 1.0, -1.0, 1.0, -1.0 ) );
    //    list.append( GeoDataLatLonBox( 1.5708, -1.5708, 3.04291, 3.03326 ) );
    //
    //    for( QList<GeoDataLatLonAltBox>::iterator it = list.begin(); it != list.end(); ++it ) {
    //        GeoDataLinearRing ring
    //                = painterp.createLinearRingFromGeoRect( (*it).center(),
    //                                                        (*it).width( GeoDataCoordinates::Degree ),
    //                                                        (*it).height( GeoDataCoordinates::Degree ) );
    //
    //        QCOMPARE( ring.latLonAltBox().toString(), (*it).toString() );
    //    }
}

}

QTEST_MAIN(Marble::TestGeoPainter)

#include "TestGeoPainter.moc"
