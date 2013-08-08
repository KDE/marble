The online version of this example can be found at
http://techbase.kde.org/Projects/Marble/DisplayGeoDataGroundOverlay

GeoDataGroundOverlay is the class used in Marble to implement the features
of the GroundOverlay KML element. More precisely, an instance references
an icon that gets displayed on top of a certain area on the globe and a
LatLonBox (GeoDataLatLonBox in Marble) describing its exact surface.

As in the tutorial for displaying placemarks, we will be using a
GeoDataDocument as the the containter for our overlay. The document is
afterwards exposed to Marble by having it added to the GeoDataTreeModel.
