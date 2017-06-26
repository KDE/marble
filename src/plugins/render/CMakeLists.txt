# Example
# add_subdirectory( test )

# Display a control or a screen box
add_subdirectory( compass )
add_subdirectory( elevationprofilefloatitem )
# add_subdirectory( fileview )
add_subdirectory( gpsinfo )
add_subdirectory( license )
add_subdirectory( mapscale )
add_subdirectory( navigation )
add_subdirectory( overviewmap )
add_subdirectory( progress )
add_subdirectory( routing )
add_subdirectory( speedometer )

# Display geographical information on the map
add_subdirectory( annotate )
add_subdirectory( crosshairs )
add_subdirectory( eclipses )
add_subdirectory( elevationprofilemarker )
add_subdirectory( graticule )
add_subdirectory( measure )
add_subdirectory( positionmarker )
add_subdirectory( atmosphere )
add_subdirectory( stars )
add_subdirectory( sun )
add_subdirectory( notes )

# Display online information
add_subdirectory( earthquake )
add_subdirectory(foursquare)
# Disabled for now until legal issues wrt to the API
# and terms of service of opencaching are cleared up.
#add_subdirectory( opencaching )
if(NOT MARBLE_NO_WEBKITWIDGETS)
add_subdirectory( opendesktop )
add_subdirectory( photo )
endif()
# add_subdirectory( panoramio )
add_subdirectory( postalcode )
add_subdirectory( satellites )
# add_subdirectory ( twitter )
if(NOT MARBLE_NO_WEBKITWIDGETS)
add_subdirectory( wikipedia )
add_subdirectory( weather )
endif()

find_package( Perl )
marble_set_package_properties( Perl PROPERTIES URL "http://www.perl.org" )
marble_set_package_properties( Perl PROPERTIES TYPE OPTIONAL PURPOSE "generation of sources in the APRS plugin" )
if( PERL_FOUND AND NOT CMAKE_SYSTEM_NAME STREQUAL Android)
    add_subdirectory( aprs )
endif()
