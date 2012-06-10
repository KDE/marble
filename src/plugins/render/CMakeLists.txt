# Example
# add_subdirectory( test )

# Display a control or a screen box
add_subdirectory( compass )
add_subdirectory( elevationprofilefloatitem )
# add_subdirectory( fileview )
add_subdirectory( gpsinfo )
add_subdirectory( mapscale )
add_subdirectory( navigation )
add_subdirectory( overviewmap )
add_subdirectory( progress )
add_subdirectory( routing )
add_subdirectory( speedometer )

# Display geographical information on the map
add_subdirectory( crosshairs )
add_subdirectory( elevationprofilemarker )
add_subdirectory( graticule )
add_subdirectory( measure )
# add_subdirectory( osmannotate )
add_subdirectory( positionmarker )
add_subdirectory( stars )
add_subdirectory( sun )

# Display online information
add_subdirectory( earthquake )
# Disabled for now until legal issues wrt to the API
# and terms of service of opencaching are cleared up.
#add_subdirectory( opencaching )
add_subdirectory( opendesktop )
# add_subdirectory( panoramio )
add_subdirectory( photo )
add_subdirectory( postalcode )
add_subdirectory( satellites )
# add_subdirectory ( twitter )
add_subdirectory( wikipedia )
add_subdirectory( weather )

# FIXME: The inhibit-screensaver plugin stresses the term render plugin a bit far.
# It should be moved to a new plugin type later.
add_subdirectory( inhibit-screensaver )

if (NOT WIN32)
  add_subdirectory( aprs )
endif (NOT WIN32)

