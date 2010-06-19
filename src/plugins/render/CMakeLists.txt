# add_subdirectory( test )
add_subdirectory( crosshairs )
add_subdirectory( geodata )
# add_subdirectory( placemarks )
add_subdirectory( compass )
add_subdirectory( graticule )
# add_subdirectory( satellites )
add_subdirectory( overviewmap )
add_subdirectory( mapscale )
add_subdirectory( stars )
# add_subdirectory( panoramio )
add_subdirectory( photo )
add_subdirectory( positionmarker )
add_subdirectory( progress )
# add_subdirectory ( twitter )
add_subdirectory( wikipedia )
add_subdirectory( weather )
# add_subdirectory( osmannotate )

add_subdirectory( navigation )
# add_subdirectory ( FITemplate )

# add_subdirectory( dataview )
# add_subdirectory( fileview )

# FIXME: this is disabled for now because it uses private
# Qt headers which currently don't get installed by emerge
# (see Bug 241125)
if (NOT WIN32)
  add_subdirectory( aprs )
endif (NOT WIN32)

