// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>
//

#include "GeoDataDocument.h"
#include "GeoDataGeometry.h"
#include "GeoDataParser.h"
#include "GeoDataPlacemark.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "MarbleDirs.h"

#include <QBuffer>
#include <QByteArray>
#include <QCoreApplication>
#include <QDebug>
#include <QString>
#include <QTest>

namespace Marble
{

class TestGeoDataPack : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void saveKMLToCache();
    void loadKMLFromCache();
    void saveCitiesToCache();
    void loadCitiesFromCache();

private:
    QString content;
    QElapsedTimer timer;
};

void TestGeoDataPack::initTestCase()
{
    MarbleDirs::setMarbleDataPath(DATA_PATH);
    MarbleDirs::setMarblePluginPath(PLUGIN_PATH);
    timer.start();

    content = QString(
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n"
        "  <Document>\n"
        "    <Placemark>\n"
        "      <name>Empty</name>\n"
        "    </Placemark>\n"
        "    <Placemark>\n"
        "      <name>LinearRingTest.kml</name>\n"
        "      <Polygon>\n"
        "        <outerBoundaryIs>\n"
        "          <LinearRing>\n"
        "            <coordinates>\n"
        "              -122.365662,37.826988,0\n"
        "              -122.365202,37.826302,0\n"
        "              -122.364581,37.82655,0\n"
        "              -122.365038,37.827237,0\n"
        "              -122.365662,37.826988,0\n"
        "            </coordinates>\n"
        "          </LinearRing>\n"
        "        </outerBoundaryIs>\n"
        "      </Polygon>\n"
        "    </Placemark>\n"
        "  </Document>\n"
        "</kml>");
}

void TestGeoDataPack::saveKMLToCache()
{
    GeoDataParser parser(GeoData_KML);

    QByteArray array(content.toUtf8());
    QBuffer buffer(&array);
    buffer.open(QIODevice::ReadOnly);
    if (!parser.read(&buffer)) {
        qWarning("Could not parse data!");
        QFAIL("Could not parse data!");
        return;
    }
    GeoDocument *document = parser.releaseDocument();
    QVERIFY(document);
    qDebug() << " parse Timer " << timer.elapsed();
    GeoDataDocument *dataDocument = static_cast<GeoDataDocument *>(document);
    QString path = QString("%1/%2.cache");
    path = path.arg(QCoreApplication::applicationDirPath());
    path = path.arg(QString("KMLTest"));

    QFile cacheFile(path);
    if (cacheFile.open(QIODevice::WriteOnly)) {
        QDataStream stream(&cacheFile);
        dataDocument->pack(stream);
        cacheFile.close();
        qDebug("Saved kml document to cache: %s", path.toLatin1().data());
    }
    qDebug() << "write Timer " << timer.elapsed();
    delete document;
}

void TestGeoDataPack::loadKMLFromCache()
{
    GeoDataDocument *cacheDocument = new GeoDataDocument();
    QString path = QString("%1/%2.cache");
    path = path.arg(QCoreApplication::applicationDirPath());
    path = path.arg(QString("KMLTest"));

    QFile cacheFile(path);
    if (cacheFile.open(QIODevice::ReadOnly)) {
        QDataStream stream(&cacheFile);
        cacheDocument->unpack(stream);
        cacheFile.close();
        qDebug("Loaded kml document from cache: %s", path.toLatin1().data());
    }
    QVERIFY(cacheDocument);
    qDebug() << "read Timer " << timer.elapsed();

    GeoDataParser parser(GeoData_KML);
    QByteArray array(content.toUtf8());
    QBuffer buffer(&array);
    buffer.open(QIODevice::ReadOnly);
    if (!parser.read(&buffer)) {
        qWarning("Could not parse data!");
        QFAIL("Could not parse data!");
        return;
    }
    GeoDocument *document = parser.releaseDocument();
    QVERIFY(document);
    qDebug() << "parse Timer " << timer.elapsed();
    GeoDataDocument *dataDocument = static_cast<GeoDataDocument *>(document);
    QCOMPARE(*cacheDocument, *dataDocument);
    qDebug() << "compare Timer " << timer.elapsed();

    delete document;
    delete cacheDocument;
}

void TestGeoDataPack::saveCitiesToCache()
{
    GeoDataParser parser(GeoData_KML);

    QFile citiesFile(CITIES_PATH);
    citiesFile.open(QIODevice::ReadOnly);
    if (!parser.read(&citiesFile)) {
        qWarning("Could not parse data!");
        QFAIL("Could not parse data!");
        return;
    }
    GeoDocument *document = parser.releaseDocument();
    QVERIFY(document);
    qDebug() << "read Timer " << timer.elapsed();
    GeoDataDocument *dataDocument = static_cast<GeoDataDocument *>(document);
    QString path = QString("%1/%2.cache");
    path = path.arg(QCoreApplication::applicationDirPath());
    path = path.arg(QString("CitiesTest"));

    QFile cacheFile(path);
    if (cacheFile.open(QIODevice::WriteOnly)) {
        QDataStream stream(&cacheFile);
        dataDocument->pack(stream);
        cacheFile.close();
        qDebug("Saved kml document to cache: %s", path.toLatin1().data());
    }
    QVERIFY(cacheFile.size() > 0);
    qDebug() << "write Timer " << timer.elapsed();
    delete document;
}

void TestGeoDataPack::loadCitiesFromCache()
{
    GeoDataDocument *cacheDocument = new GeoDataDocument();
    QString path = QString("%1/%2.cache");
    path = path.arg(QCoreApplication::applicationDirPath());
    path = path.arg(QString("CitiesTest"));

    QFile cacheFile(path);
    if (cacheFile.open(QIODevice::ReadOnly)) {
        QDataStream stream(&cacheFile);
        cacheDocument->unpack(stream);
        cacheFile.close();
        qDebug("Loaded kml document from cache: %s", path.toLatin1().data());
    }
    QVERIFY(cacheDocument);
    qDebug() << "read Timer " << timer.elapsed();

    GeoDataParser parser(GeoData_KML);
    QFile citiesFile(CITIES_PATH);
    citiesFile.open(QIODevice::ReadOnly);
    if (!parser.read(&citiesFile)) {
        qWarning("Could not parse data!");
        QFAIL("Could not parse data!");
        return;
    }
    GeoDocument *document = parser.releaseDocument();
    QVERIFY(document);
    qDebug() << "parse Timer " << timer.elapsed();

    // commented out as it timeouts the test on build.kde.org
    //    GeoDataDocument *dataDocument = static_cast<GeoDataDocument*>( document );
    //    QVERIFY( compareDocuments( cacheDocument, dataDocument ) );
    //    qDebug() << "compare Timer " << timer.elapsed();

    delete cacheDocument;
    //    delete dataDocument;
}

}

QTEST_MAIN(Marble::TestGeoDataPack)

#include "TestGeoDataPack.moc"
