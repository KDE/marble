// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include <QSignalSpy>

#include "BookmarkManager.h"
#include "GeoDataFolder.h"
#include "GeoDataTreeModel.h"
#include "TestUtils.h"

namespace Marble
{

class BookmarkManagerTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void construct();

    void loadFile_data();
    void loadFile();
};

void BookmarkManagerTest::construct()
{
    GeoDataTreeModel model;

    QCOMPARE(model.rowCount(), 0);

    {
        const BookmarkManager manager(&model);

        QCOMPARE(model.rowCount(), 1);

        QVERIFY(manager.document() != nullptr);
        QCOMPARE(manager.folders().count(), 1);
        QCOMPARE(manager.folders().first()->size(), 0);
        QCOMPARE(manager.showBookmarks(), true);

        // FIXME this method returns random results (depending on the username and the existence of the bookmarks file)
        //    QCOMPARE( manager.bookmarkFile(), QString() );

        QCOMPARE(model.rowCount(), 1);
    }

    QCOMPARE(model.rowCount(), 0);
}

void BookmarkManagerTest::loadFile_data()
{
    QTest::addColumn<QString>("relativePath");
    QTest::addColumn<bool>("expected");

    addRow() << QString() << false;
    addRow() << QString("lsdkrfuweqofn.kml") << false; // non-existing file

    // FIXME This will create an empty KML file called "LICENSE.txt" under MarbleDirs::localPath().
    //    addRow() << QString( "LICENSE.txt" ) << true; // file exists in MarbleDirs::systemPath()
}

void BookmarkManagerTest::loadFile()
{
    QFETCH(QString, relativePath);
    QFETCH(bool, expected);

    GeoDataTreeModel model;
    BookmarkManager manager(&model);

    QVERIFY(model.rowCount() == 1);

    const bool fileLoaded = manager.loadFile(relativePath);

    QCOMPARE(fileLoaded, expected);
    QVERIFY(manager.document() != nullptr);

    QCOMPARE(model.rowCount(), 1);
}

}

QTEST_MAIN(Marble::BookmarkManagerTest)

#include "BookmarkManagerTest.moc"
