The online version of this example can be found at
https://techbase.kde.org/Marble/Runners/DisplayGeoDataPlacemark

In this tutorial we'll show how you can create a nice shiny placemark with an icon and associated geometries (such as LineStrings, LinearRings, Polygons or MultiGeometries). We also cover basic styling of placemarks and geometries.

GeoDataPlacemark is a class which implements the features of KML's Placemark. Basically, it represents an interest point (a simple point or a more complex geometry) on the map, with some information attached.

In order to add a GeoDataPlacemark to our widget, we will use the GeoDataDocument class, which is a container for features (including placemarks) and styles.
