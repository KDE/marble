//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include <QtCore/QObject>
#include <QtTest/QtTest>

#include <GeoDataParser.h>
#include <GeoDataDocument.h>
#include <GeoDataTour.h>
#include <GeoDataPlaylist.h>
#include <GeoDataTourControl.h>
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
        "   <gx:Tour>"
        "   <name>My Tour</name>"
        "   <description>This is my tour.</description>"
        "   </gx:Tour>"
        "   <gx:Tour id=\"tourId\">"
        "       <gx:Playlist>"
        "           <gx:TourControl id=\"space\">"
        "               <gx:playMode>pause</gx:playMode>"
        "           </gx:TourControl>"
        "       </gx:Playlist>"
        "   </gx:Tour>"
        "   <gx:Tour id=\"tourId\"></gx:Tour>"
        "</Folder>"
        "</kml>" );

    GeoDataDocument* dataDocument = parseKml( centerContent );
    QCOMPARE( dataDocument->folderList().size(), 1 );
    GeoDataFolder *folder = dataDocument->folderList().at( 0 );

    GeoDataTour *tour_1 = dynamic_cast<GeoDataTour*>(folder->child(0));
    GeoDataTour *tour_2 = dynamic_cast<GeoDataTour*>(folder->child(1));

    QVERIFY(tour_1 != 0);
    QVERIFY(tour_2 != 0);

    QCOMPARE(tour_1->id(), QString(""));
    QCOMPARE(tour_1->name(), QString("My Tour"));
    QCOMPARE(tour_1->description(), QString("This is my tour."));
    QCOMPARE(tour_2->id(), QString("tourId"));
    QCOMPARE(tour_2->name(), QString());
    QCOMPARE(tour_2->description(), QString());

    GeoDataPlaylist *playlist = tour_2->playlist();
    QVERIFY(playlist != 0);

    GeoDataTourControl *control = dynamic_cast<GeoDataTourControl*>(
                playlist->primitive(0));
    QVERIFY(control != 0);
    QCOMPARE(control->id(), QString("space"));
    QCOMPARE(control->playMode(), GeoDataTourControl::Pause);
    delete dataDocument;
}

QTEST_MAIN( TestTour )

#include "TestTour.moc"

