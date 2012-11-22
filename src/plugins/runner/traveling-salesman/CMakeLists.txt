PROJECT( TravelingSalesmanPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( travelingsalesman_SRCS TravelingSalesmanRunner.cpp TravelingSalesmanPlugin.cpp )

marble_add_plugin( TravelingSalesmanPlugin ${travelingsalesman_SRCS} )
