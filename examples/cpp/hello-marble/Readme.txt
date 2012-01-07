The online version of this example can be found at
http://techbase.kde.org/Projects/Marble/MarbleCPlusPlus

The API of the Marble library allows for a very easy integration of a map 
widget into your application.
Let's prove that with a tiny Hello world-like example: Qt beginners might want 
to have a look at the Qt Widgets Tutorial to learn more about the details of 
the code. But this is probably not necessary. For a start we just create a 
QApplication object and a MarbleWidget object which serves as a window. By 
default the MarbleWidget uses the Atlas map theme. However for our first 
example we choose to display streets. So we set the maptheme id to 
OpenStreetMap. Then we call QWidget::show() to show the map widget and we call 
QApplication::exec() to start the application's event loop. That's all!
