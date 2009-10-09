#include <QtCore/QObject>
#include <QtTest/QtTest>

/**
 * This class is intended to test the GeoDataParser. Should it replace KMLTest?
 */
class TestGeoDataParser : public QObject
{
Q_OBJECT
private slots:
    /// load a KML file and count the features in it
    void countFeatures();
};

void TestGeoDataParser::countFeatures()
{

}

QTEST_MAIN( TestGeoDataParser )
#include "TestGeoDataParser.moc"
