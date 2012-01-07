The online version of this example can be found at
http://techbase.kde.org/Projects/Marble/LayerInterface

The previous tutorial showed how to override the customPaint() method in 
MarbleWidget to paint on top of the map. It is also possible to paint at 
different layer positions. This is similar to providing a z-order of elements 
being painted.
To achieve this, we'll take a look at an example. Instead of deriving from 
MarbleWidget, we create our own Marble::LayerInterface class. After passing it 
to Marble, it will be included in painting similar to how customPaint() was 
called. This time however we are able to specify at which layer to paint.
To illustrate the painting in different layers, the code below paints a clock 
and implements the ability to dynamically switch its layer position by pressing 
'+'. Notice how the current layer position is indicated in the window title. 
When painting in the "STARS" layer, you won't see anything -- we'll paint 
behind the map. In the "SURFACE" layer, city names and other placemarks will be 
painted on top of us. In contrast, "ORBIT" will make us paint over placemarks, 
while float items (info boxes) still paint above us. This will change when we 
paint in the "USER TOOLS" layer.
