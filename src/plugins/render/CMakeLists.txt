# add_subdirectory( test )
add_subdirectory( crosshairs )
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

# add_subdirectory( fileview )

add_subdirectory( routing )

find_package(Qtsysteminfo)
if(QtSystemInfo_FOUND)
  # FIXME: The inhibit-screensaver plugin stresses the term render plugin a bit far.
  # It should be moved to a new plugin type later.
  add_subdirectory( inhibit-screensaver )
endif(QtSystemInfo_FOUND)

if (NOT WIN32)
  add_subdirectory( aprs )
endif (NOT WIN32)
