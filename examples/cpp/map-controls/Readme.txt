The online version of this example can be found at
http://techbase.kde.org/Projects/Marble/MarbleSignalsSlots

We'd like to add other widgets to our Marble window: A zoom slider and a label 
that shows the current mouse position in geodetic coordinates: longitude and 
latitude.
In order to achieve this we need to create a vertical layout. Once we are done 
we add the slider and the label that we created to the layout. Also we zoom the 
globe to the slider's default value using the MarbleWidget::zoomView(int) 
method.
We want to center our globe onto South America. So we create a new 
GeoDataCoordinates object that takes the longitude and the latitude as a 
parameter and we call MarbleWidget::centerOn.
As you might have realized already GeoDataCoordinates is the geodetic "sister" 
of QPoint. They share a very similar API. Additionally GeoDataCoordinates 
features a nice set of string conversion methods 
(GeoDataCoordinates::fromString(), GeoDataCoordinates::lonToString() and 
GeoDataCoordinates::latToString()). They are used in various places inside 
Marble such as the signal MarbleWidget::mouseMoveGeoPosition(const QString&) .
Finally we connect the signals and slots that MarbleWidget offers to the 
signals and slots of the slider and the label.
