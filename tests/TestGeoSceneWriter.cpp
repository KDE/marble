// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#include <QDebug>
#include <QDir>
#include <QObject>
#include <QTemporaryFile>
#include <QTest>

#include "DgmlElementDictionary.h"
#include "GeoSceneDocument.h"
#include "GeoSceneGeodata.h"
#include "GeoSceneHead.h"
#include "GeoSceneIcon.h"
#include "GeoSceneItem.h"
#include "GeoSceneLayer.h"
#include "GeoSceneLegend.h"
#include "GeoSceneMap.h"
#include "GeoSceneProperty.h"
#include "GeoSceneSection.h"
#include "GeoSceneSettings.h"
#include "GeoSceneTileDataset.h"
#include "GeoSceneVector.h"
#include "GeoSceneZoom.h"

#include "GeoSceneParser.h"
#include "GeoWriter.h"

using namespace Marble;

class TestGeoSceneWriter : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void saveFile_data();
    void saveFile();
    void saveAndLoad_data();
    void saveAndLoad();
    void saveAndCompare_data();
    void saveAndCompare();

    void writeHeadTag();

private:
    QDir dgmlPath;
    QMap<QString, QSharedPointer<GeoSceneParser>> parsers;
};

Q_DECLARE_METATYPE(QSharedPointer<GeoSceneParser>)

void TestGeoSceneWriter::initTestCase()
{
    QStringList dgmlFilters;
    dgmlFilters << "*.dgml";

    dgmlPath = QDir(DGML_PATH);
    dgmlPath.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);

    const auto dirNames = dgmlPath.entryList();
    for (const QString &dgmlDirname : dirNames) {
        qDebug() << dgmlDirname;
        QDir dataDir(dgmlPath.absoluteFilePath(dgmlDirname));
        dataDir.setNameFilters(dgmlFilters);

        // check there are dgml files in the data dir
        if (dataDir.isEmpty()) {
            continue;
        }

        // test the loading of each file in the data dir
        const auto fileNames = dataDir.entryList(dgmlFilters, QDir::Files);
        for (const QString &filename : fileNames) {
            // Add example files
            QFile file(dataDir.filePath(filename));

            // Verify file existence
            QVERIFY(file.exists());

            // Make the parsers
            auto parser = new GeoSceneParser(GeoScene_DGML);

            QSharedPointer<GeoSceneParser> parserPointer(parser);

            // Open the files and verify
            QVERIFY(file.open(QIODevice::ReadOnly));

            // Parser and verify
            QVERIFY2(parser->read(&file), filename.toLatin1().constData());

            parsers.insert(dataDir.filePath(filename), parserPointer);

            // close files
            file.close();
        }
    }
}

void TestGeoSceneWriter::saveFile_data()
{
    QTest::addColumn<QSharedPointer<GeoSceneParser>>("parser");

    QMap<QString, QSharedPointer<GeoSceneParser>>::iterator itpoint = parsers.begin();
    QMap<QString, QSharedPointer<GeoSceneParser>>::iterator const endpoint = parsers.end();
    for (; itpoint != endpoint; ++itpoint) {
        QTest::newRow(itpoint.key().toLatin1().constData()) << itpoint.value();
    }
}

void TestGeoSceneWriter::saveFile()
{
    QFETCH(QSharedPointer<GeoSceneParser>, parser);

    // attempt to save a file using the GeoWriter
    QTemporaryFile tempFile;

    GeoWriter writer;
    writer.setDocumentType(dgml::dgmlTag_nameSpace20);

    // Open file in right mode
    QVERIFY(tempFile.open());

    QVERIFY(writer.write(&tempFile, (dynamic_cast<GeoSceneDocument *>(parser->activeDocument()))));
}

void TestGeoSceneWriter::saveAndLoad_data()
{
    QTest::addColumn<QSharedPointer<GeoSceneParser>>("parser");

    QMap<QString, QSharedPointer<GeoSceneParser>>::iterator itpoint = parsers.begin();
    QMap<QString, QSharedPointer<GeoSceneParser>>::iterator const endpoint = parsers.end();
    for (; itpoint != endpoint; ++itpoint) {
        QTest::newRow(itpoint.key().toLatin1().constData()) << itpoint.value();
    }
}

void TestGeoSceneWriter::saveAndLoad()
{
    // Save the file and then verify loading it again
    QFETCH(QSharedPointer<GeoSceneParser>, parser);

    QTemporaryFile tempFile;
    GeoWriter writer;
    writer.setDocumentType(dgml::dgmlTag_nameSpace20);

    // Open file in right mode
    QVERIFY(tempFile.open());

    QVERIFY(writer.write(&tempFile, (dynamic_cast<GeoSceneDocument *>(parser->activeDocument()))));

    GeoSceneParser resultParser(GeoScene_DGML);

    tempFile.reset();
    QVERIFY(resultParser.read(&tempFile));
}

void TestGeoSceneWriter::saveAndCompare_data()
{
    QTest::addColumn<QSharedPointer<GeoSceneParser>>("parser");
    QTest::addColumn<QString>("original");

    QMap<QString, QSharedPointer<GeoSceneParser>>::iterator itpoint = parsers.begin();
    QMap<QString, QSharedPointer<GeoSceneParser>>::iterator const endpoint = parsers.end();
    for (; itpoint != endpoint; ++itpoint) {
        QTest::newRow(itpoint.key().toLatin1().constData()) << itpoint.value() << itpoint.key();
    }
}

void TestGeoSceneWriter::saveAndCompare()
{
    // save the file and compare it to the original
    QFETCH(QSharedPointer<GeoSceneParser>, parser);
    QFETCH(QString, original);

    // attempt to save a file using the GeoWriter
    QTemporaryFile tempFile;

    GeoWriter writer;
    // FIXME: a better way to do this?
    writer.setDocumentType(dgml::dgmlTag_nameSpace20);

    // Open file in right mode
    QVERIFY(tempFile.open());

    QVERIFY(writer.write(&tempFile, (dynamic_cast<GeoSceneDocument *>(parser->activeDocument()))));

    QFile file(original);
    QVERIFY(file.open(QIODevice::ReadOnly));
    QVERIFY(tempFile.reset());
    QTextStream oldFile(&file);
    QTextStream newFile(&tempFile);

    QVERIFY(newFile.readAll().simplified().compare(oldFile.readAll().simplified()));
}

void TestGeoSceneWriter::writeHeadTag()
{
    auto document = new GeoSceneDocument;

    GeoSceneHead *head = document->head();
    head->setName("Test Map");
    head->setTheme("testmap");
    head->setTarget("earth");

    GeoSceneIcon *icon = document->head()->icon();
    icon->setPixmap("preview.jpg");

    GeoSceneZoom *zoom = document->head()->zoom();
    zoom->setMaximum(1000);
    zoom->setMaximum(500);
    zoom->setDiscrete(true);

    auto texture = new GeoSceneTileDataset("map");
    texture->setSourceDir("earth/testmap");
    texture->setFileFormat("png");
    texture->setTileProjection(GeoSceneAbstractTileProjection::Equirectangular);
    texture->addDownloadUrl(QUrl("http://download.kde.org/marble/map/{x}/{y}/{zoomLevel}"));
    texture->addDownloadUrl(QUrl("http://download.google.com/marble/map/{x}/{y}/{zoomLevel}"));
    texture->addDownloadPolicy(DownloadBrowse, 20);
    texture->addDownloadPolicy(DownloadBulk, 20);
    texture->setMaximumTileLevel(15);
    texture->setLevelZeroColumns(2);
    texture->setLevelZeroRows(2);

    auto geodata = new GeoSceneGeodata("cityplacemarks");
    geodata->setSourceFile("baseplacemarks.kml");

    auto layer = new GeoSceneLayer("testmap");
    layer->setBackend("texture");
    layer->addDataset(texture);

    auto secondLayer = new GeoSceneLayer("standardplaces");
    secondLayer->setBackend("geodata");
    secondLayer->addDataset(geodata);

    auto thirdLayer = new GeoSceneLayer("mwdbii");
    thirdLayer->setBackend("vector");
    thirdLayer->setRole("polyline");

    auto vector = new GeoSceneVector("pdiffborder");
    vector->setFeature("border");
    vector->setFileFormat("PNT");
    vector->setSourceFile("earth/mwdbii/PDIFFBORDER.PNT");
    vector->pen().setColor("#ffe300");
    thirdLayer->addDataset(vector);

    GeoSceneMap *map = document->map();
    map->addLayer(layer);
    map->addLayer(secondLayer);
    map->addLayer(thirdLayer);

    GeoSceneSettings *settings = document->settings();

    auto coorGrid = new GeoSceneProperty("coordinate-grid");
    coorGrid->setValue(true);
    coorGrid->setAvailable(true);
    settings->addProperty(coorGrid);

    auto overviewmap = new GeoSceneProperty("overviewmap");
    overviewmap->setValue(true);
    overviewmap->setAvailable(true);
    settings->addProperty(overviewmap);

    auto compass = new GeoSceneProperty("compass");
    compass->setValue(true);
    compass->setAvailable(true);
    settings->addProperty(compass);

    auto scalebar = new GeoSceneProperty("scalebar");
    scalebar->setValue(true);
    scalebar->setAvailable(true);
    settings->addProperty(scalebar);

    GeoSceneLegend *legend = document->legend();
    auto section = new GeoSceneSection("areas");
    section->setHeading("Areas");
    legend->addSection(section);

    auto sportsPitch = new GeoSceneItem("sports_pitch");
    sportsPitch->setText("Sports pitch");
    GeoSceneIcon *sportsPitchIcon = sportsPitch->icon();
    sportsPitchIcon->setPixmap("maps/earth/testmap/legend/sports_pitch.png");
    section->addItem(sportsPitch);

    auto sportsCentre = new GeoSceneItem("sports_centre");
    sportsCentre->setText("Sports centre");
    GeoSceneIcon *sportsCentreIcon = sportsCentre->icon();
    sportsCentreIcon->setColor("#00FF00");
    section->addItem(sportsCentre);

    QTemporaryFile tempFile;
    tempFile.open();

    GeoWriter writer;
    writer.setDocumentType("http://edu.kde.org/marble/dgml/2.0");
    QVERIFY(writer.write(&tempFile, document));

    // Parser and verify
    GeoSceneParser parser(GeoScene_DGML);
    tempFile.reset();
    QVERIFY(parser.read(&tempFile));

    auto document2 = static_cast<GeoSceneDocument *>(parser.activeDocument());
    QTemporaryFile tempFile2;
    tempFile2.open();
    GeoWriter writer2;
    writer2.setDocumentType("http://edu.kde.org/marble/dgml/2.0");
    QVERIFY(writer2.write(&tempFile2, document2));

    tempFile.reset();
    QTextStream file(&tempFile);
    tempFile2.reset();
    QTextStream file2(&tempFile2);
    QVERIFY(file.readAll().simplified().compare(file2.readAll().simplified()));

    delete document;
}

QTEST_MAIN(TestGeoSceneWriter)
#include "TestGeoSceneWriter.moc"
