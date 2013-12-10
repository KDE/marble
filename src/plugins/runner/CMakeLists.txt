# Search and/or reverse geocoding
add_subdirectory( hostip )
add_subdirectory( latlon )
add_subdirectory( local-osm-search )
add_subdirectory( localdatabase )
add_subdirectory( nominatim-search )
add_subdirectory( nominatim-reversegeocoding )
add_subdirectory( gosmore-reversegeocoding )

# Routing
add_subdirectory( gosmore-routing )

# As of 2013-07-15 (or rather in effect around 2013-12-01) MapQuest requires an API key,
# and has this nice section in their terms of usage: "You are responsible for maintaining
# the confidentiality of your Credentials and for any usage or abuse of the MapQuest
# Services or MapQuest Content by anyone using your Credentials."
# (from http://developer.mapquest.com/web/info/terms-of-use)
# Good bye, MapQuest
add_subdirectory( mapquest )

add_subdirectory( monav )
add_subdirectory( openrouteservice )
add_subdirectory( open-source-routing-machine )
add_subdirectory( routino )
add_subdirectory( yours )
# traveling-salesman works, but it is quite slow (tested version 1.0.3-RC1)
#add_subdirectory( traveling-salesman )

#Parsing
add_subdirectory( cache )
add_subdirectory( gpx )
add_subdirectory( json )
add_subdirectory( kml )
add_subdirectory( osm )
add_subdirectory( pn2 )
add_subdirectory( pnt )
add_subdirectory( log )
add_subdirectory( gpsbabel )

macro_optional_find_package( libshp )
marble_set_package_properties( libshp PROPERTIES DESCRIPTION "reading and writing of ESRI Shapefiles (.shp)" )
marble_set_package_properties( libshp PROPERTIES URL "http://shapelib.maptools.org/" )
marble_set_package_properties( libshp PROPERTIES TYPE OPTIONAL PURPOSE "reading and displaying .shp files" )
if( LIBSHP_FOUND )
  add_subdirectory( shp )
endif( LIBSHP_FOUND )
