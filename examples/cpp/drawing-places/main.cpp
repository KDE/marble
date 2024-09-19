#include <QApplication>
#include <QTreeView>

#include <marble/MarbleModel.h>
#include <marble/MarbleWidget.h>

#include <marble/GeoDataCoordinates.h>
#include <marble/GeoDataDocument.h>
#include <marble/GeoDataIconStyle.h>
#include <marble/GeoDataLineString.h>
#include <marble/GeoDataLineStyle.h>
#include <marble/GeoDataLinearRing.h>
#include <marble/GeoDataPlacemark.h>
#include <marble/GeoDataPolyStyle.h>
#include <marble/GeoDataStyle.h>
#include <marble/GeoDataTreeModel.h>

#include <cstdio>

using namespace Marble;

void addPoints(GeoDataLinearRing &linearRing)
{
    linearRing << GeoDataCoordinates(25.97226722704463, 44.43497647488007, 0, GeoDataCoordinates::Degree)
               << GeoDataCoordinates(26.04711276456992, 44.4420741223712, 0, GeoDataCoordinates::Degree)
               << GeoDataCoordinates(25.99712510557899, 44.48015825036597, 0, GeoDataCoordinates::Degree)
               << GeoDataCoordinates(26.11268978668501, 44.53902366720936, 0, GeoDataCoordinates::Degree)
               << GeoDataCoordinates(26.12777496065434, 44.48972441010599, 0, GeoDataCoordinates::Degree)
               << GeoDataCoordinates(26.17769825773425, 44.47685689461117, 0, GeoDataCoordinates::Degree)
               << GeoDataCoordinates(26.16489863910029, 44.45366647920105, 0, GeoDataCoordinates::Degree)
               << GeoDataCoordinates(26.23394105442375, 44.43247765101769, 0, GeoDataCoordinates::Degree)
               << GeoDataCoordinates(26.23388161223319, 44.40720014793351, 0, GeoDataCoordinates::Degree)
               << GeoDataCoordinates(26.18689640043445, 44.40683215952335, 0, GeoDataCoordinates::Degree)
               << GeoDataCoordinates(26.1462530009004, 44.36252655873379, 0, GeoDataCoordinates::Degree)
               << GeoDataCoordinates(25.97226722704463, 44.43497647488007, 0, GeoDataCoordinates::Degree);
}

void createStyleBucharest(GeoDataStyle &style)
{
    GeoDataLineStyle lineStyle(QColor(255, 0, 0, 90));
    lineStyle.setWidth(8);

    GeoDataPolyStyle polyStyle(QColor(255, 0, 0, 40));
    polyStyle.setFill(true);

    style.setLineStyle(lineStyle);
    style.setPolyStyle(polyStyle);
}

void createStyleArch(GeoDataStyle &style)
{
    GeoDataIconStyle iconStyle;
    iconStyle.setIconPath("bucharest_small.jpg");
    style.setIconStyle(iconStyle);
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    // Create a Marble QWidget without a parent
    MarbleWidget *mapWidget = new MarbleWidget();

    // Load the OpenStreetMap map
    mapWidget->setMapThemeId("earth/openstreetmap/openstreetmap.dgml");

    // Create the Linear Ring (polygon) representing Bucharest's boundaries and include it in a placemark
    GeoDataLinearRing *Bucharest = new GeoDataLinearRing;
    addPoints(*Bucharest);

    GeoDataPlacemark *placemarkBucharest = new GeoDataPlacemark;
    placemarkBucharest->setGeometry(Bucharest);

    // Create the placemark representing the Arch of Triumph
    GeoDataPlacemark *placemarkArch = new GeoDataPlacemark("Arch of Triumph");
    placemarkArch->setCoordinate(26.0783, 44.4671, 0, GeoDataCoordinates::Degree);

    // Add styles (icons, colors, etc.) to the two placemarks
    GeoDataStyle::Ptr styleBucharest(new GeoDataStyle);
    GeoDataStyle::Ptr styleArch(new GeoDataStyle);

    createStyleBucharest(*styleBucharest);
    placemarkBucharest->setStyle(styleBucharest);

    createStyleArch(*styleArch);
    placemarkArch->setStyle(styleArch);

    // Create the document and add the two placemarks (the point representing the Arch of Triumph and the polygon with Bucharest's boundaries)
    GeoDataDocument *document = new GeoDataDocument;
    document->append(placemarkBucharest);
    document->append(placemarkArch);

    // Add the document to MarbleWidget's tree model
    mapWidget->model()->treeModel()->addDocument(document);

    // Center the map on Bucharest and set the zoom
    mapWidget->centerOn(GeoDataCoordinates(26.0783, 44.4671, 0, GeoDataCoordinates::Degree));
    mapWidget->zoomView(2400);

    mapWidget->show();

    return app.exec();
}
