SET (TARGET mapreproject)
PROJECT (${TARGET})

include_directories(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( ${TARGET}_SRC
IntegerInterpolation.cpp
ReadOnlyMapDefinition.cpp
OsmTileClusterRenderer.cpp
NwwMapImage.cpp
ReadOnlyMapImage.cpp
BilinearInterpolation.cpp
InterpolationMethod.cpp
SimpleMapImage.cpp
Thread.cpp
NearestNeighborInterpolation.cpp
NasaWorldWindToOpenStreetMapConverter.cpp
main.cpp
)
add_executable( ${TARGET} ${${TARGET}_SRC} )

target_link_libraries(${TARGET} marblewidget)
