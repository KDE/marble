The online version of this example can be found at
http://techbase.kde.org/Projects/Marble/MarbleGeoPainter

In the previous tutorial you've seen how easy it is to embed a MarbleWidget 
into a Qt application: Just create a MarbleWidget, set a map theme on it and 
... you're done already.
Next we'll extend that example a bit and write our own little paint method to 
add some extra content to the globe. To facilitate this, Marble provides a 
painting hook called MarbleWidget::customPaint. It is called in between of the 
normal paint operations: After the background and tiles are painted, but before 
the top layers like float items (info boxes).
The customPaint operation is called with a GeoPainter: An extended QPainter 
which not only is able to paint at certain screen (pixel) positions, but also 
at certain geo (lat,lon) positions. We'll make use of that feature now. To keep 
things simple again, we just add a little 'Hello World' message indicated by a 
green circle.

