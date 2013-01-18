//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include <QtCore/QObject>
#include <QtTest/QtTest>

#include <GeoDataParser.h>
#include <GeoDataDocument.h>
#include <GeoDataTour.h>
#include <MarbleDebug.h>
#include <GeoDataFolder.h>
#include <GeoDataScreenOverlay.h>

using namespace Marble;

class TestTour : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void simpleParseTest();
};

void TestTour::initTestCase()
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

void TestTour::simpleParseTest()
{
  QString const centerContent (
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<kml xmlns=\"http://www.opengis.net/kml/2.2\""
        " xmlns:gx=\"http://www.google.com/kml/ext/2.2\">"
        "<Folder>"
        "   <gx:Tour></gx:Tour>"
        "   <gx:Tour id=\"tourId\"></gx:Tour>"
        "</Folder>"
        "</kml>" );

    GeoDataDocument* dataDocument = parseKml( centerContent );
    QCOMPARE( dataDocument->folderList().size(), 1 );
    GeoDataFolder *folder = dataDocument->folderList().at( 0 );

    GeoDataTour *tour_1 = folder->child(0);
    GeoDataTour *tour_2 = folder->child(1);

    QVERIFY(tour_1 != 0);
    QVERIFY(tour_2 != 0);

    QCOMPARE(tour_1->id(), "");
    QCOMPARE(tour_2->id(), "tourId");

    delete dataDocument;
}

QTEST_MAIN( TestTour )

#include "TestTour.moc"

