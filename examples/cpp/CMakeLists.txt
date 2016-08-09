# Generate fake headers
# forwarding headers to simulate installed header paths with just a marble/ prefix
# for headers which are located in other prefixes in the sources
set(FAKEHEADERS_DIR ${CMAKE_CURRENT_BINARY_DIR}/fakeheaders)

function(generateFakeHeaders basedir)
    foreach(_filename ${ARGN})
        set(_fileContent "#include <${basedir}/${_filename}>")
        file(WRITE ${FAKEHEADERS_DIR}/marble/${_filename} ${_fileContent})
    endforeach()
endfunction()

# create clean dir
file(REMOVE_RECURSE ${FAKEHEADERS_DIR})
file(MAKE_DIRECTORY ${FAKEHEADERS_DIR}/marble)

generateFakeHeaders(geodata/data
    GeoDataCoordinates.h
    GeoDataDocument.h
    GeoDataFlyTo.h
    GeoDataGeometry.h
    GeoDataGroundOverlay.h
    GeoDataIconStyle.h
    GeoDataLatLonAltBox.h
    GeoDataLinearRing.h
    GeoDataLineString.h
    GeoDataLookAt.h
    GeoDataMultiGeometry.h
    GeoDataPlacemark.h
    GeoDataPlaylist.h
    GeoDataPoint.h
    GeoDataPolygon.h
    GeoDataStyle.h
    GeoDataStyleMap.h
    GeoDataTour.h
)
generateFakeHeaders(geodata/parser
    GeoDataTypes.h
)
generateFakeHeaders(routing
    Route.h
    RouteRequest.h
    RoutingManager.h
    RoutingModel.h
)

# TODO: Change include directories for the library not to include all subdirectories,
# and change #include statements to use <marble/...> and "..." includes only
include_directories(
    ${CMAKE_SOURCE_DIR}/src/lib
    ${FAKEHEADERS_DIR}
)

set( MARBLE_EXAMPLES_INSTALL_DIR ${CMAKE_INSTALL_FULL_DATAROOTDIR}/apps/marble/examples )

add_subdirectory( hello-marble )
add_subdirectory( map-properties )
add_subdirectory( custom-layers )
add_subdirectory( kml-inspector )
add_subdirectory( map-controls )
add_subdirectory( overlay-rendering )
add_subdirectory( squad-interpolation )
add_subdirectory( geopainter )
add_subdirectory( basic-routing )
add_subdirectory( reverse-geocoding )
add_subdirectory( vehicletracking )
add_subdirectory( search )

add_subdirectory( marble-game )
add_subdirectory( marbleQuick2 )

FIND_PACKAGE (OpenCV QUIET COMPONENTS core imgproc highgui videoio)
if(OPENCV_HIGHGUI_FOUND)
  add_subdirectory( animation-video )
  add_subdirectory( tour-preview )
endif()
