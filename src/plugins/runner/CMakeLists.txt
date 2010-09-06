# Search and/or reverse geocoding
add_subdirectory( hostip )
add_subdirectory( latlon )
add_subdirectory( localdatabase )
add_subdirectory( namefinder )
add_subdirectory( nominatim )

# Routing
add_subdirectory( gosmore )
add_subdirectory( openrouteservice )
add_subdirectory( routino )

# traveling-salesman works, but it is quite slow (tested version 1.0.3-RC1)
#add_subdirectory( traveling-salesman )

# monav works, but you need a custom shell client
add_subdirectory( monav )

add_subdirectory( yours )
