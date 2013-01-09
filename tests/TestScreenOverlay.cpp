//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//

#include <QtCore/QObject>
#include <QtTest/QtTest>

#include <GeoDataParser.h>
#include <GeoDataDocument.h>
#include <MarbleDebug.h>
#include <GeoDataFolder.h>
#include <GeoDataScreenOverlay.h>

using namespace Marble;


class TestScreenOverlay : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void simpleParseTest();
};

void TestScreenOverlay::initTestCase()
{
    MarbleDebug::enable = true;
}

GeoDataDocument *parseKml(const QString &content)
{
    GeoDataParser parser( GeoData_KML );

    QByteArray array( content.toUtf8() );
    QBuffer buffer( &array );
    buffer.open( QIODevice::ReadOnly );
    //qDebug() << "Buffer content:" << endl << buffer.buffer();
    if ( !parser.read( &buffer ) ) {
        qFatal( "Could not parse data!" );
    }
    GeoDocument* document = parser.releaseDocument();
    Q_ASSERT( document );
    return static_cast<GeoDataDocument*>( document );
}

void TestScreenOverlay::simpleParseTest()
{
  QString const centerContent (
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<kml xmlns=\"http://www.opengis.net/kml/2.2\""
        " xmlns:gx=\"http://www.google.com/kml/ext/2.2\">"
        "<Folder>"
        "  <ScreenOverlay>"
        "    <overlayXY x=\"2.5\" y=\"-0.5\" xunits=\"fraction\" yunits=\"fraction\"/>"
        "    <screenXY x=\"0.5\" y=\"0.5\" xunits=\"insetpixels\" yunits=\"pixels\"/>"
        "    <rotationXY x=\"1.5\" y=\"3.5\" xunits=\"fraction\" yunits=\"insetPixels\"/>"
        "    <size x=\"23\" y=\"0.5\" xunits=\"pixels\" yunits=\"insetPixels\"/>"
        "    <rotation>23</rotation>"
        "  </ScreenOverlay>"
        "</Folder>"
        "</kml>" );

    GeoDataDocument* dataDocument = parseKml( centerContent  );
    QCOMPARE( dataDocument->folderList().size(), 1 );
    GeoDataFolder *folder = dataDocument->folderList().at( 0 );
    QCOMPARE( folder->size(), 1 );
    GeoDataScreenOverlay *overlay = dynamic_cast<GeoDataScreenOverlay*>( folder->child( 0 ) );
    QVERIFY( overlay != 0 );

    QCOMPARE( overlay->overlayXY().xunit(), GeoDataVec2::Fraction );
    QCOMPARE( overlay->overlayXY().yunit(), GeoDataVec2::Fraction );
    QCOMPARE( overlay->overlayXY().y(), -0.5 );
    QCOMPARE( overlay->overlayXY().x(), 2.5 );

    QCOMPARE( overlay->screenXY().xunit(), GeoDataVec2::Fraction ); // spelling error in kml, so fallback to default
    QCOMPARE( overlay->screenXY().yunit(), GeoDataVec2::Pixels );
    QCOMPARE( overlay->screenXY().x(), 0.5 );
    QCOMPARE( overlay->screenXY().y(), 0.5 );

    QCOMPARE( overlay->rotationXY().xunit(), GeoDataVec2::Fraction );
    QCOMPARE( overlay->rotationXY().yunit(), GeoDataVec2::InsetPixels );
    QCOMPARE( overlay->rotationXY().x(), 1.5 );
    QCOMPARE( overlay->rotationXY().y(), 3.5 );

    QCOMPARE( overlay->size().xunit(), GeoDataVec2::Pixels );
    QCOMPARE( overlay->size().yunit(), GeoDataVec2::InsetPixels );
    QCOMPARE( overlay->size().x(), 23.0 );
    QCOMPARE( overlay->size().y(), 0.5 );

    QCOMPARE( overlay->rotation(), 23.0 );

    delete dataDocument;
}

QTEST_MAIN( TestScreenOverlay )

#include "TestScreenOverlay.moc"
