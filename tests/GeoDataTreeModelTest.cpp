// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include <QTest>

#include "GeoDataTreeModel.h"

#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"

namespace Marble
{

class GeoDataTreeModelTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void defaultConstructor();
    void setRootDocument();
    void addDocument();
};

void GeoDataTreeModelTest::defaultConstructor()
{
    const GeoDataTreeModel model;

    QCOMPARE(model.rowCount(), 0);
    QCOMPARE(model.columnCount(), 4);

    QCOMPARE(model.headerData(0, Qt::Horizontal, Qt::DisplayRole), QVariant(tr("Name")));
    QCOMPARE(model.headerData(1, Qt::Horizontal, Qt::DisplayRole), QVariant(tr("Type")));
    QCOMPARE(model.headerData(2, Qt::Horizontal, Qt::DisplayRole), QVariant(tr("Popularity")));
    QCOMPARE(model.headerData(3, Qt::Horizontal, Qt::DisplayRole), QVariant(tr("PopIndex", "Popularity index")));

    QCOMPARE(model.index(0, 0), QModelIndex());
    QCOMPARE(model.index(nullptr), QModelIndex());
    QCOMPARE(model.parent(QModelIndex()), QModelIndex());
    QCOMPARE(model.data(QModelIndex(), Qt::DisplayRole), QVariant());
    QCOMPARE(model.flags(QModelIndex()), Qt::NoItemFlags);

    QVERIFY(const_cast<GeoDataTreeModel *>(&model)->rootDocument() != nullptr);
}

void GeoDataTreeModelTest::setRootDocument()
{
    GeoDataDocument document;

    {
        GeoDataTreeModel model;

        model.setRootDocument(&document);
        // ~GeoDataTreeModel() shouldn't delete document
    }
}

void GeoDataTreeModelTest::addDocument()
{
    {
        auto document = new GeoDataDocument;

        GeoDataTreeModel model;

        model.addDocument(document);
        QCOMPARE(model.rowCount(), 1);
    }
}

}

QTEST_MAIN(Marble::GeoDataTreeModelTest)

#include "GeoDataTreeModelTest.moc"
