# add_subdirectory( test )
add_subdirectory( crosshairs )
add_subdirectory( compass )
add_subdirectory( earthquake )

# Disabled for now until legal issues wrt to the API
# and terms of service of opencaching are cleared up.
#add_subdirectory( opencaching )
add_subdirectory( graticule )
add_subdirectory( satellites )
add_subdirectory( overviewmap )
add_subdirectory( mapscale )
add_subdirectory( speedometer )
add_subdirectory( stars )
add_subdirectory( sun )
# add_subdirectory( panoramio )
add_subdirectory( photo )
add_subdirectory( positionmarker )
add_subdirectory( progress )
add_subdirectory( opendesktop )
# add_subdirectory ( twitter )
add_subdirectory( wikipedia )
add_subdirectory( weather )
# add_subdirectory( osmannotate )
add_subdirectory( postalcode )

add_subdirectory( navigation )
# add_subdirectory ( FITemplate )

# add_subdirectory( fileview )

add_subdirectory( measure )
add_subdirectory( routing )
add_subdirectory( gpsinfo )

# FIXME: The inhibit-screensaver plugin stresses the term render plugin a bit far.
# It should be moved to a new plugin type later.
add_subdirectory( inhibit-screensaver )

if (NOT WIN32)
  add_subdirectory( aprs )
endif (NOT WIN32)

add_subdirectory( elevationprofilefloatitem )
add_subdirectory( elevationprofilemarker )
