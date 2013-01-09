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
#include <GeoDataPlacemark.h>
#include <GeoDataStyle.h>
#include <GeoDataBalloonStyle.h>

using namespace Marble;


class TestBalloonStyle : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void simpleParseTest();
};

void TestBalloonStyle::initTestCase()
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

void TestBalloonStyle::simpleParseTest()
{
  QString const content (
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<kml xmlns=\"http://www.opengis.net/kml/2.2\""
        " xmlns:gx=\"http://www.google.com/kml/ext/2.2\">"
        "<Document>"
        "  <name>The one and only BalloonStyle test case</name>"
        "  <Style id=\"my-balloon-style\">"
        "    <BalloonStyle>"
        "      <bgColor>aa112233</bgColor>"
        "      <textColor>bb445566</textColor>"
        "      <text>This is my balloon style. There are many like it, but this is mine.</text>"
        "      <displayMode>hide</displayMode>"
        "    </BalloonStyle>"
        "  </Style>"
        "  <Folder>"
        "  <Placemark>"
        "    <name>The first placemark</name>"
        "    <styleUrl>#my-balloon-style</styleUrl>"
        "    <Point><coordinates>80.0,30.0</coordinates></Point>"
        "  </Placemark>"
        "  </Folder>"
        "</Document>"
        "</kml>" );

    GeoDataDocument* dataDocument = parseKml( content  );
    QCOMPARE( dataDocument->folderList().size(), 1 );
    GeoDataFolder *folder = dataDocument->folderList().at( 0 );
    QCOMPARE( folder->size(), 1 );
    GeoDataPlacemark *placemark1 = dynamic_cast<GeoDataPlacemark*>( folder->child( 0 ) );
    QVERIFY( placemark1 != 0 );

    QCOMPARE( placemark1->name(), QString( "The first placemark" ) );
    QCOMPARE( placemark1->style()->balloonStyle().backgroundColor().red(), 51 );
    QCOMPARE( placemark1->style()->balloonStyle().textColor().blue(), 68 );
    QCOMPARE( placemark1->style()->balloonStyle().displayMode(), GeoDataBalloonStyle::Hide );
    QString const text = "This is my balloon style. There are many like it, but this is mine.";
    QCOMPARE( placemark1->style()->balloonStyle().text(), text );

    delete dataDocument;
}

QTEST_MAIN( TestBalloonStyle )

#include "TestBalloonStyle.moc"
