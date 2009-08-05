#include <QObject>
#include <QtTest/QtTest>

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
