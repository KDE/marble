See http://techbase.kde.org/Projects/Marble/MarbleMarbleWidget for the online 
version of this example.

We'd like to display a small weather map. So we need to modify the map. And we 
need to turn on the satellite view, enable the clouds and enable the country 
border lines.
Again MarbleWidget provides a convenient way to make these changes to the 
overall look and feel of the map.
By default Marble shows a few info boxes: Overview Map, Compass and ScaleBar. 
But the size for the widget is very limited. Therefore we want to shrink the 
compass. And we want to get rid of all the clutter, so we turn off the Overview 
Map and the ScaleBar. In the source code the class AbstractFloatItem is used to 
display all kinds of Info Boxes. All the Info Boxes are derived from the 
AbstractFloatItem class. Now we get a list of all the float items that are 
known to MarbleWidget and we go through it. Once we reach the float item which 
has got the name id "compass" we make all the changes we want to it (this has 
been simplified in Marble 0.11.0 where you can access AbstractFloatItems 
directly via their nameId)
