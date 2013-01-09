//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012       Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include <QtTest/QtTest>
#include <QtTest/QSignalSpy>

#include "BookmarkManager.h"
#include "GeoDataFolder.h"
#include "GeoDataTreeModel.h"

#define addRow() QTest::newRow( QString("line %1").arg( __LINE__ ).toAscii().data() )

namespace Marble
{

class BookmarkManagerTest : public QObject
{
    Q_OBJECT

 private slots:
    void construct();

    void loadFile_data();
    void loadFile();
};

void BookmarkManagerTest::construct()
{
    GeoDataTreeModel model;

    QCOMPARE( model.rowCount(), 0 );

    {
        const BookmarkManager manager( &model );

        QCOMPARE( model.rowCount(), 1 );

        QVERIFY( manager.document() != 0 );
        QCOMPARE( manager.folders().count(), 1 );
        QCOMPARE( manager.folders().first()->size(), 0 );
        QCOMPARE( manager.showBookmarks(), true );

        // FIXME this method returns random results (depending on the username and the existence of the bookmarks file)
        //    QCOMPARE( manager.bookmarkFile(), QString() );

        QCOMPARE( model.rowCount(), 1 );
    }

    QCOMPARE( model.rowCount(), 0 );
}

void BookmarkManagerTest::loadFile_data()
{
    QTest::addColumn<QString>( "relativePath" );
    QTest::addColumn<bool>( "expected" );

    addRow() << QString() << false;
    addRow() << QString( "lsdkrfuweqofn.kml" ) << false; // non-existing file

// FIXME This will create an empty KML file called "LICENSE.txt" under MarbleDirs::localPath().
//    addRow() << QString( "LICENSE.txt" ) << true; // file exists in MarbleDirs::systemPath()
}

void BookmarkManagerTest::loadFile()
{
    QFETCH( QString, relativePath );
    QFETCH( bool, expected );

    GeoDataTreeModel model;
    BookmarkManager manager( &model );

    QVERIFY( model.rowCount() == 1 );

    const bool fileLoaded = manager.loadFile( relativePath );

    QCOMPARE( fileLoaded, expected );
    QVERIFY( manager.document() != 0 );

    QCOMPARE( model.rowCount(), 1 );
}

}

QTEST_MAIN( Marble::BookmarkManagerTest )

#include "BookmarkManagerTest.moc"
