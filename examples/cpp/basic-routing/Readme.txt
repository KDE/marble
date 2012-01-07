The online version of this example can be found at
http://techbase.kde.org/Projects/Marble/Routing/BasicRouting

The Marble library 0.13 and later (KDE 4.8, Marble 1.3) has an API to calculate 
and manage routes. Let's start with a brief overview of the important classes 
and their interaction. The class RouteRequest holds parameters that are 
constraints for the route to be calculated: Start and destination, optional via 
points and further parameters (e.g. transport type). The RoutingManager passes 
such a request to backends (routing plugins) that calculate possible routes 
from it. The best route is chosen and displayed in a special layer in the 
MarbleWidget. Additionally you can access the route data via the RoutingModel. 
This model can be passed directly to e.g. a QListView to show the turn 
instructions, but also exposes further data like the waypoints of the route via 
the Route class. This one consists of a set of RouteSegment instances, each 
representing a number of waypoints and an optional turn instruction (Maneuver) 
at the end.

